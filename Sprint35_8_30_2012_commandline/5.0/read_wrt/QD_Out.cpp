// $Header: /CAMCAD/4.6/read_wrt/QD_out.cpp 114   4/25/07 4:32p Lynn Phung $

#include "stdafx.h"
#include "RwLib.h"
#include <float.h>
#include "format_s.h"
#include "format.h"
#include "DirDlg.h"
#include <direct.h>
#include "ccdoc.h"
#include "extents.h"
#include "pcbutil.h"
#include "xform.h"
#include "ck.h"
#include "QD_out.h"
#include "centroid.h"
#include "Graph.h"
#include "CCEtoODB.h"
#include ".\qd_out.h"
#include "gauge.h"
#include "RwUiLib.h"

extern CProgressDlg *progress; // from PORT.CPP


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* for debug
static bool scanForDamage(BlockStruct *b)
{
   bool retval = false;

   if (b != NULL)
   {
  
         {
            int jj = 0;
            retval = true;
         }
      }
   }

   return retval;
}

static bool scanForDamage(FileStruct *f)
{
   if (f != NULL)
      return scanForDamage(f->getBlock());
   return false;
}
*/

static CString GetAttribStringValue(CCEtoODBDoc *doc, Attrib *attrib)
{
   CString retval;

   if (attrib != NULL)
   {
      retval = attrib->getStringValue();
   }

   return retval;
}

// Case 2104 plus more about normalizing the panel location.
// Previously teh exporter would write out the panel offsets as offsets for the panel in
// the QD file. This was mathematically correct,and would build boards. But apparantly
// non-zero panel offsets cause the customer grief in production, something about simply
// being hard for humans to work with when something goes wrong or changes, but I did
// not ever get a thorough explanation. In any case, teh customer wanted panels normalized
// to 0,0,0, and for the boards locations to be compensated for the panel offsets.
// And so it is, when the NormalPanelPosition is "true".
// Code for the old behavior is still in place, and easily activated, but there is
// currently no user interface to activate it. Mark says we probably won't need it, everyone
// will want normalized panels. We'll see. It is not made available now so as to not
// add complexity if no one needs this. If someone ends up wanting it, just add a .IN
// command to set NormalizePanelPosition to false.
static bool NormalizePanelPosition = true;

//******************************************************************************

CQDOutFile::CQDOutFile()
{
   Reset();
}

void CQDOutFile::Reset()
{
   maxPanelLength = 0;
   maxPanelWidth = 0;
	panelThickness = 100;
	boardThickness = 100;
	glueTop = false;
	glueBottom = false;
	panelFidSymbolNum = 1;
	boardFidSymbolNum = 1;
   boardIDPrefix = "";
   boardIDStartNumber = 0;
   boardIDLength = 1;
   topPanelLocationDefault.Empty();
   bottomPanelLocationDefault.Empty();
   boardTolerance = 0;
   panelTolerance = 0;
   nameFiles = true; // by default it is true
   use_unterseite = true;
   sequenceAttribName.Empty();
}

/******************************************************************************
* Siemens_WriteFiles
*/
void Siemens_WriteFiles(CCEtoODBDoc *Doc)
{
	if (Doc != NULL)
	{
		QDExport qdwriter(Doc);
		qdwriter.DoQDExport();
	}
}

/******************************************************************************
* DoQDExport
*/
void QDExport::DoQDExport()
{
	// Verify the current view is valid for export
	if (!FileValidatedForExport())
		return;

	// Load preconfigured settings from QD.out
	LoadQDSettings();
   LoadSettingFromAttribute();

	// Get save information (user dialog)
	CString pathname;
	if (!GetExportDetails(&pathname))
		return;

   SaveSettingToAttribute();

	// Create a log file based on the file export details just gathered
	CString logFile;
	if (!CreateLogFile(pathname, &logFile))
		return;

	// Make sure cetroids are current
   generate_CENTROIDS(m_doc);

	// Calculate machine origin
   Find_Machine_Origin();

   CString checkFile( getApp().getSystemSettingsFilePath("qd.chk") );
   check_init(checkFile);

   CString topFile = pathname;
   if (m_outFileSettings.nameFiles)
   {
      topFile += format.filename;
      topFile += "_A.qd";
   }
   else
   {
      topFile += "top.qd";
   }

   CString botFile = pathname;
   if (m_outFileSettings.nameFiles)
   {
      botFile += format.filename;
      botFile += "_B.qd";
   }
   else
   {
      botFile += "bottom.qd";
   }

   UNLINK(topFile);
   UNLINK(botFile);

   CTime currentTime = CTime::GetCurrentTime();
   headerLine.Format("/* --- Created by %s, %s --- */", getApp().getCamCadTitle(), currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));

   // Comment line cannot have /* in the line
	if (!format.QD.boardPartNumber.IsEmpty())
		commentLine = format.QD.boardPartNumber;
	else
		commentLine.Format("--- Created by %s, %s ---", getApp().getCamCadTitle(), currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));

   pcbDataList.RemoveAll();
   if (strlen(topFile))
   {
      fp = fopen(topFile, "wt");
      if (!fp)
      {
         CString buf;
         buf.Format("Can not open [%s]", topFile);
         MessageBox(NULL, buf, "Error File Open", MB_OK | MB_ICONHAND);
         fclose(flog);
         return;
      }

      fprintf(fp, "%s\n", headerLine);

      saveRestoreSettings originalSettings(m_doc, m_panelFile, false);
      m_topPanelLocation.ApplyToCCZ(m_doc, m_panelFile, originalSettings, fileTypeSiemensQd);      

      WritePCBStruct(qdSurfaceTop);
      WritePCBData(qdSurfaceTop);
    
      originalSettings.Restore();

      fclose(fp);
   }


   pcbDataList.RemoveAll();
   if (strlen(botFile))
   {
      fp = fopen(botFile, "wt");
      if (!fp)
      {
         CString buf;
         buf.Format("Can not open [%s]", botFile);
         MessageBox(NULL, buf, "Error File Open", MB_OK | MB_ICONHAND);
         fclose(flog);
         return;
      }

      fprintf(fp, "%s\n", headerLine);

      saveRestoreSettings originalSettings(m_doc, m_panelFile, true);
      m_botPanelLocation.ApplyToCCZ(m_doc, m_panelFile, originalSettings, fileTypeSiemensQd);

      WritePCBStruct(qdSurfaceBottom);
      WritePCBData(qdSurfaceBottom);

		originalSettings.Restore();                  

      fclose(fp);
   }


	//SaveQDPanelLocationSettings();

   pcbDataList.RemoveAll();
   check_deinit();

   fclose(flog);
   if (display_error)
      Logreader(logFile);
}
/******************************************************************************
* CreateLogFile
*/
bool QDExport::CreateLogFile(CString filePath, CString *logFile)
{
	// Prep log-file for writer
	logFile->SetString(GetLogfilePath("qd.log"));
   if ((flog = fopen(logFile->GetBuffer(), "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile->GetBuffer(), MB_ICONEXCLAMATION | MB_OK);
      return false;
   }

	return true;
}

/******************************************************************************
* GetExportDetails
*/
bool QDExport::GetExportDetails(CString *pathname)
{
	// We should not be here if there is not a valid panel file selected
	if (m_panelFile == NULL)
		return false;

	// Hid progress bar while operating GUI
	progress->ShowWindow(SW_HIDE);

	//
	// Panel Location
	//

   // Get panel location from attributes
   if (m_panelFile != NULL)
   {
      //m_panelFile->getBlock()->getAttributesRef();
      //m_doc->SetUnknownAttrib(&m_panelFile->getBlock()->getDefinedAttributes(),"SIEMENS_QD_PANEL_LOCATION_TOP",    m_topPanelLocation.GetSettingsString(), SA_OVERWRITE, NULL);
      //m_doc->SetUnknownAttrib(&m_panelFile->getBlock()->getDefinedAttributes(),"SIEMENS_QD_PANEL_LOCATION_BOTTOM", m_botPanelLocation.GetSettingsString(), SA_OVERWRITE, NULL);

      Attrib *atop =  is_attvalue(m_doc, m_panelFile->getBlock()->getDefinedAttributes(), GetMachineAttributeName(fileTypeSiemensQd,false), 0);   
      Attrib *abot =  is_attvalue(m_doc, m_panelFile->getBlock()->getDefinedAttributes(), GetMachineAttributeName(fileTypeSiemensQd,true), 0);

      if (atop != NULL)
      {
         CString attval = get_attvalue_string(m_doc, atop);
         this->m_topPanelLocation = attval;
      }
      else if (!m_outFileSettings.topPanelLocationDefault.IsEmpty())
      {
         this->m_topPanelLocation = m_outFileSettings.topPanelLocationDefault;
      }
      // else using defaults from constructor

      if (abot != NULL)
      {
         CString attval = get_attvalue_string(m_doc, abot);
         this->m_botPanelLocation = attval;
      }
      else if (!m_outFileSettings.bottomPanelLocationDefault.IsEmpty())
      {
         this->m_botPanelLocation = m_outFileSettings.bottomPanelLocationDefault;
      }
      // else using defaults from constructor

   }



	/*CPrepDlg prepDlg(m_doc, m_panelFile);
	if (prepDlg.DoModal() != IDOK) 
		return false;*/


	//
	// Output File
	//
	CBrowse browseDlg;
   char path[_MAX_PATH];
   _getcwd(path, _MAX_PATH);
   browseDlg.m_strSelDir = path;
   browseDlg.m_strTitle = "Select directory for files";

	if (!browseDlg.DoBrowse())
      return false;

   pathname->SetString(browseDlg.m_strPath);

   if (pathname->Right(1) != "\\")
      pathname->AppendChar('\\');


	//
	// Output Format
	//
	char buffer[128];

   FormatSiemensQDOut qdDlg;
	qdDlg.m_glueBot = m_outFileSettings.glueBottom?TRUE:FALSE;
	qdDlg.m_glueTop = m_outFileSettings.glueTop?TRUE:FALSE;
   qdDlg.m_boardFidNum = m_outFileSettings.boardFidSymbolNum;
   qdDlg.m_panelFidNum = m_outFileSettings.panelFidSymbolNum;
   qdDlg.m_boardThickness = ITOA(m_outFileSettings.boardThickness, buffer, 10); // "100";
   qdDlg.m_panelThickness = ITOA(m_outFileSettings.panelThickness, buffer, 10); // "100";
   qdDlg.m_boardTolerance = ITOA(m_outFileSettings.boardTolerance, buffer, 10); // "100";
   qdDlg.m_panelTolerance = ITOA(m_outFileSettings.panelTolerance, buffer, 10); // "100";
   qdDlg.m_boardPartNumber = format.QD.boardPartNumber;
   qdDlg.m_enableFileName = m_outFileSettings.nameFiles;
   qdDlg.m_fileName.Empty();
   if (m_outFileSettings.nameFiles)
   {
      // Only set file name if .NAME_FILES is not N
      qdDlg.m_fileName = format.filename;
   }

	if (qdDlg.DoModal() != IDOK) 
		return false;

   if (m_outFileSettings.nameFiles)
   {
      // Only copy the file name if .NAME_FILES is not N; otherwise leave format.filename along
      strcpy(format.filename, qdDlg.m_fileName);
   }
   format.QD.boardFiducialSymbolNum = qdDlg.m_boardFidNum;
   format.QD.panelFiducialSymbolNum = qdDlg.m_panelFidNum;
   format.QD.glueTop = (qdDlg.m_glueTop != 0);
   format.QD.glueBot = (qdDlg.m_glueBot != 0);
   format.QD.boardThickness = atoi(qdDlg.m_boardThickness);
   format.QD.panelThickness = atoi(qdDlg.m_panelThickness);
   format.QD.boardTolerance = atoi(qdDlg.m_boardTolerance);
   format.QD.panelTolerance = atoi(qdDlg.m_panelTolerance);
	format.QD.boardPartNumber = qdDlg.m_boardPartNumber;

	// Bring back the progress bar
	progress->ShowWindow(SW_SHOW);

	return true;
}

/******************************************************************************
* FileValidatedForExport
*/
bool QDExport::FileValidatedForExport()
{
	// ensure only one panel is currently active
   m_panelFile = NULL;
   POSITION filePos = m_doc->getFileList().GetHeadPosition();
   while (filePos)
   {      
      FileStruct *file = m_doc->getFileList().GetNext(filePos);
      if (file->getBlockType() == blockTypePanel && file->isShown())
      {
         if (m_panelFile != NULL)
         {
            MessageBox(NULL, "Only one Panel can be on", "Too many Panels", MB_OK | MB_ICONHAND);
            return false;
         }
         m_panelFile = file;
      }
   }

	// ensure at least one panel is active
   if (m_panelFile == NULL)
   {
      MessageBox(NULL, "Panel required for Siemen QD export.\nMake sure a Panel exists and is On, then retry export.", "No Visible Panels", MB_OK | MB_ICONHAND);
      return false;
   }

	return true;
}


/******************************************************************************
* initializeLocalVariables
*/
void QDExport::initializeLocalVariables()
{
	QD_PCB_STRUCTURE                     = "LP STRUKTUR"            ;
	QD_PASSIVE                           = "PASSIV"				       ;
	QD_COORDINATE                        = "KOORDINATEN"		       ;
	QD_COMMENT                           = "KOMMENTAR"			       ;
	QD_OFFSET                            = "OFFSET"				       ;
	QD_ANGLE                             = "WINKEL"				       ;
	QD_BASIS                             = "BASIS"                  ;
	QD_BASIS_LINK                        = "BASIS LINK"				 ;
	QD_PCB_DATA                          = "LP DATEN"					 ;
	QD_BOARD                             = "NUTZEN"						 ;
	QD_UPPERSIDE                         = "OBERSEITE"					 ;
	QD_UNDERSIDE                         = "UNTERSEITE"				 ;
	QD_PROCESSING                        = "BEARBEITUNG"            ;
	QD_INK_DOT_RECOGNITION               = "INKPUNKT ERKENNUNG"     ;
	QD_INK_DOT_DARK                      = "INKPUNKT DUNKEL"			 ;
	QD_INK_DOT_LIGHT                     = "INKPUNKT HELL"			 ;
	QD_WITH_INK_DOT_PLACEMENT            = "BEI INKPUNKT BESTUECKEN";
	QD_PCB_POSITION_RECOGNITION          = "LP LAGE ERKENNUNG"      ;
	QD_COMPONENT_POSITION_RECOGNITION    = "BE LAGE ERKENNUNG"      ;
	QD_BLOCK                             = "SPERREN"                ;
	QD_PLACEMENT                         = "BESTUECKEN"				 ;
	QD_GLUE                              = "KLEBEN"						 ;
	QD_SOLDER_PASTE_DISPENSER            = "LOTPASTEN DISPENSOR"	 ;
	QD_AFTER_PROCESSING                  = "NACH BEARBEITUNG"		 ;
	QD_LENGTH                            = "LAENGE"						 ;
	QD_WIDTH                             = "BREITE"                 ;
	QD_THICKNESS                         = "DICKE"						 ;
	QD_SOLDER_PASTE_THICKNESS            = "LOTPASTEN DICKE"			 ;
	QD_CORNER                            = "ECKE"						 ;
	QD_TOLERANCE                         = "TOLERANZ"					 ;
	QD_FIDUCIAL_SET_LIST                 = "PM SET LISTE"				 ;
	QD_FIDUCIAL_LIST                     = "PM LISTE"               ;
	QD_CODE                              = "CODE"						 ;
	QD_PCB_POSITION                      = "LP POSITION"				 ;
	QD_FIDUCIAL_SET_VALID                = "PM SET GUELTIG"			 ;
	QD_INK_DOT                           = "INKPUNKT"					 ;
	QD_FIDUCIAL_VALID                    = "PM GUELTIG"				 ;
	QD_PROGRAM_PLACEMENT                 = "PROGRAMM BESTUECKEN"    ;
	QD_COMPONENT                         = "BE"							 ;
   QD_PLACEMENT_SEQUENCE                = "EBENE"                  ;

	QD_LEFT_PARENTHESIS                  = "(";
	QD_RIGHT_PARENTHESIS                 = ")";
	QD_LEFT_SQUARE_BRACKET               = "[";
	QD_RIGHT_SQUARE_BRACKET              = "]";
	QD_LEFT_CURLY_BRACKET                = "{";
	QD_RIGHT_CURLY_BRACKET               = "}";
	QD_SIGNLE_QUOTE                      = "'";


	// Variables are initalized here
	// Variables used in setting file are initalizded at LoadQDSettings()
   format.filename[0] = '\0';

	indent = "";
	indentCount = 0;
	display_error = 0;
	headerLine = "";
	commentLine = "";

   display_error = FALSE;
   Machine_Top.x = 0.0;
   Machine_Top.y = 0.0;
   Machine_Bot.x = 0.0;
   Machine_Bot.y = 0.0;
   FIRST_BOARD = TRUE;
   LAST_BOARD = FALSE;
}

/******************************************************************************
* LoadQDSettings
*/
void QDExport::LoadQDSettings()
{
   // Load settings from QD.out file
   CString settingsFile( getApp().getExportSettingsFilePath("qd.out") );

	// default attribute and value to look for
	m_outputAttrib.SetDefaultAttribAndValue();

   m_outFileSettings.Reset();

   Machine_Origin_Top_Type = "";
   Machine_Origin_Bot_Type = "";
   Machine_Origin_Top = "";
   Machine_Origin_Bot = "";
	PARTNUMBER_KEYWORD = "PARTNUMBER";
	ROTATION_OFFSET = "";
	XOFFSET = "";
	YOFFSET = "";
	XYOFFSET_ROTATION_BY_CENTROID = false;
	SUPPRESS_UNLOADED_COMPS = true;
	CADIF_ROTATION = false;
   SUPPRESS_BOARDINFO = FALSE;

   FILE *fSettings = fopen(settingsFile, "rt");
   if (!fSettings)   
   {
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      MessageBox(NULL, tmp, "HAF Settings File", MB_OK | MB_ICONHAND);
      return;
   }

   // read settings from file
   char line[255];
   char *lp;

   while (fgets(line, 255, fSettings))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == ';')
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".SEQUENCE_ATTRIB"))
         {
            if ((lp = get_string(NULL, "  \t\n")) == NULL)
               continue; 
            m_outFileSettings.sequenceAttribName = lp;
         }
         else if (!STRICMP(lp, ".MAXPANEL_LENGTH"))
         {
            if ((lp = get_string(NULL, "  \t\n")) == NULL)
               continue; 
            m_outFileSettings.maxPanelLength = atoi(lp);
         }
         else if (!STRICMP(lp, ".MAXPANEL_WIDTH"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.maxPanelWidth = atoi(lp);
         }
         else if (!STRICMP(lp, ".TOP_DEFAULT"))
         {
            // This is a multi-parameter line, do not break on white space or commas
            if ((lp = get_string(NULL, "\n")) == NULL)
               continue; 
            m_outFileSettings.topPanelLocationDefault = lp;
         }
         else if (!STRICMP(lp, ".BOTTOM_DEFAULT"))
         {
            // This is a multi-parameter line, do not break on white space or commas
            if ((lp = get_string(NULL, "\n")) == NULL)
               continue; 
            m_outFileSettings.bottomPanelLocationDefault = lp;
         }
         else if (!STRICMP(lp, ".BOARD_TOLERANCE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.boardTolerance = atoi(lp);
         }
         else if (!STRICMP(lp, ".PANEL_TOLERANCE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.panelTolerance = atoi(lp);
         }
         else if (!STRICMP(lp, ".PARTNUMBER_KEYWORD"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            PARTNUMBER_KEYWORD = (CString)lp;
         }
         else if (!STRICMP(lp, ".MACHINE_ORIGIN_TOP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
            {
               fprintf(flog, "MACHINE_ORIGIN_TOP is not defined.\n");
               display_error++;  
               continue; 
            }
            CString type = (CString)lp;
            if (!(!type.CompareNoCase("GEOM") || !type.CompareNoCase("REF")))
            {
               fprintf(flog, "MACHINE_ORIGIN_TOP needs type arguement (either GEOM or REF).\n");
               display_error++;  
               continue;
            }

            if (Machine_Origin_Top_Type != "")
            {
               fprintf(flog, "MACHINE_ORIGIN_TOP is defined twice. The first MACHINE_ORIGIN_TOP will be used.\n");
               display_error++;  
               continue;
            }
            Machine_Origin_Top_Type = type;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
            {
               fprintf(flog, "MACHINE_ORIGIN_TOP is not defined.\n");
               display_error++;  
               continue; 
            }
            Machine_Origin_Top = (CString)lp;
         }
         else if (!STRICMP(lp, ".MACHINE_ORIGIN_BOT"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
            {
               fprintf(flog, "MACHINE_ORIGIN_BOT is not defined.\n");
               display_error++;  
               continue; 
            }
            CString type = (CString)lp;
            if (!(!type.CompareNoCase("GEOM") || !type.CompareNoCase("REF")))
            {
               fprintf(flog, "MACHINE_ORIGIN_BOT needs type arguement (either GEOM or REF).\n");
               display_error++;  
               continue;
            }

            if (Machine_Origin_Bot_Type != "")
            {
               fprintf(flog, "MACHINE_ORIGIN_BOT is defined twice. The first MACHINE_ORIGIN_BOT will be used.\n");
               display_error++;  
               continue;
            }
            Machine_Origin_Bot_Type = type;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
            {
               fprintf(flog, "MACHINE_ORIGIN_BOT is not defined.\n");
               display_error++;  
               continue; 
            }
            Machine_Origin_Bot = (CString)lp;

         }
         else if (!STRICMP(lp, ".SUPPRESS_BOARDINFO"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)          
               continue; 
            CString answer = lp;
            if (!answer.CompareNoCase("y"))
               SUPPRESS_BOARDINFO = TRUE;
            else
               SUPPRESS_BOARDINFO = FALSE;
         }
         else if (!STRICMP(lp, ".ROTATION_OFFSET"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            ROTATION_OFFSET = (CString)lp;
         }
         else if (!STRICMP(lp, ".XOFFSET"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            XOFFSET = (CString)lp;
         }
         else if (!STRICMP(lp, ".YOFFSET"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            YOFFSET = (CString)lp;
         }
			else if (!STRICMP(lp, ".COMP_POSITION"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 

				// TOM - Case 1159
				// Check if user specified ORIGIN for rotation
				// Default is to rotate around the CENTROID  
				// (User knows to set this to either ORIGIN or CENTROID; but 
				//  we simply key off of the first character, either O or not)

				// Command was renamed from ".XY_LOCATION" to "COMP_POSITION" by Mark
				CString val = ((CString)lp).MakeLower().Trim();
				XYOFFSET_ROTATION_BY_CENTROID = val[0] != 'o';
			}
			else if (!STRICMP(lp, ".SUPPRESS_UNLOADED_COMPONENTS"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 

				// TOM - Case 1144
				// Check if user flagged not to suppress (FALSE, or NO)
				CString val = ((CString)lp).MakeLower().Trim();
				SUPPRESS_UNLOADED_COMPS = val[0] != 'n' && val[0] != 'f';
			}
			else if (!STRICMP(lp, ".CADIF_ROTATION"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
				if (toupper(lp[0]) == 'Y')
					CADIF_ROTATION = true;
			}
			else if (!STRICMP(lp, ".THICKNESS_PANEL"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.panelThickness = atoi(lp);
			}
			else if (!STRICMP(lp, ".THICKNESS_BOARD"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.boardThickness = atoi(lp);
			}
			else if (!STRICMP(lp, ".PANEL_FIDUCIAL_SYMBOL_NUMBER"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.panelFidSymbolNum = atoi(lp);
			}
			else if (!STRICMP(lp, ".BOARD_FIDUCIAL_SYMBOL_NUMBER"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.boardFidSymbolNum = atoi(lp);
			}
			else if (!STRICMP(lp, ".GLUE_TOP"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
				if (toupper(lp[0]) == 'Y')
					m_outFileSettings.glueTop = true;
			}
			else if (!STRICMP(lp, ".GLUE_BOTTOM"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
				if (toupper(lp[0]) == 'Y')
					m_outFileSettings.glueBottom = true;
			}
			else if (!strcmp(lp, ".OUTPUT_ATTRIBUTE"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
				m_outputAttrib.SetKeyword(lp);
			}
			else if (!strcmp(lp, ".OUTPUT_VALUE"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
				m_outputAttrib.AddValue(lp);
			}
			else if (!strcmp(lp, ".BOARD_ID"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.boardIDPrefix = lp;

            if ((lp = get_string(NULL, " \t\n")) != NULL)
            {
               if (is_number(lp))
                  m_outFileSettings.boardIDStartNumber = atoi(lp);
            }
            if (m_outFileSettings.boardIDStartNumber < 1)
               m_outFileSettings.boardIDStartNumber = 1;

            if ((lp = get_string(NULL, " \t\n")) != NULL)
            {
               if (is_number(lp))
                  m_outFileSettings.boardIDLength = atoi(lp);
            }
            if (m_outFileSettings.boardIDLength < 1)
               m_outFileSettings.boardIDLength = 1;
         }
			else if (!strcmp(lp, ".NAME_FILES"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
				if (toupper(lp[0]) == 'N')
               m_outFileSettings.nameFiles = false;
         }
        	else if (!strcmp(lp, ".USE_UNTERSEITE"))
			{
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            m_outFileSettings.use_unterseite = (toupper(lp[0]) == 'Y')?true:false;
         }
		}
   }


   fclose(fSettings);
}

void QDExport::LoadSettingFromAttribute()
{
	if (m_panelFile == NULL || m_doc == NULL)
		return;

	WORD keyword = (WORD)m_doc->getStandardAttributeKeywordIndex(standartAttributeSiemensDialogSetting);
	Attrib* attrib = NULL;
	if (m_panelFile->getBlock()->getAttributes() == NULL || !m_panelFile->getBlock()->getAttributes()->Lookup(keyword, attrib))
		return;
	
	CStringArray atributesStringArray;
	CSupString attributesSupString = GetAttribStringValue(m_doc, attrib); //m_doc->ValueArray.GetAt(attrib->getStringValueIndex());
	attributesSupString.Parse(atributesStringArray, "|");
	
	for (int i=0; i<atributesStringArray.GetCount(); i++)
	{	
		CString attributeString = atributesStringArray.GetAt(i);
		if (attributeString.Find("FileName") > -1)
		{
			CStringArray params;
			CSupString attributeSupString = atributesStringArray.GetAt(i);
			attributeSupString.ParseQuote(params, "=");
			
			if (params.GetCount() > 1)
			{
            strcpy(format.filename, params[1]);
         }
      }
		else if (attributeString.Find("PartNumber") > -1)
		{
			CStringArray params;
			CSupString attributeSupString = atributesStringArray.GetAt(i);
			attributeSupString.ParseQuote(params, "=");
			
			if (params.GetCount() > 1)
			{
            format.QD.boardPartNumber = params[1];
         }
		}
	}
}

void QDExport::SaveSettingToAttribute()
{
	if (m_panelFile == NULL)
		return;

	int keyword = m_doc->getStandardAttributeKeywordIndex(standartAttributeSiemensDialogSetting);

	CString attributeString;
	attributeString.AppendFormat("%s=\"%s\"|", "FileName", format.filename);
	attributeString.AppendFormat("%s=%s", "PartNumber", format.QD.boardPartNumber);
   m_panelFile->getBlock()->setAttrib(m_doc->getCamCadData(), keyword, valueTypeString, attributeString.GetBuffer(0), attributeUpdateOverwrite, NULL);
}

/******************************************************************************
* WritePanel
*/
void QDExport::WritePCBStruct(EQdSurface surface)
{
   indent = "";
   indentCount = 0;
   fprintf(fp, "%s%s %s\n", indent, QD_PCB_STRUCTURE, QD_LEFT_CURLY_BRACKET);
   IndentPlus();
   
   WritePassive(surface, true);

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WritePassive
*/
void QDExport::WritePassive(EQdSurface surface, bool alreadyMirrored)
{
	CString panelFileName = check_name('r', m_panelFile->getName());
	CString passiveID = "";
   passiveID.Format(".qd/%s.lp", panelFileName);

   fprintf(fp, "%s%s '%s' %s\n", indent, QD_PASSIVE, passiveID, QD_LEFT_CURLY_BRACKET);
   IndentPlus();

   // write coordinate
   if (NormalizePanelPosition)
   {
      // Use 0,0,0 for panel always.
      // Boards will get panel transform applied to each board offset.
      WriteCoordinate(commentLine, 0, 0, 0, surface);
   }
   else
   {
      // What it was before the experiment
      int xOffset = QD_Unit(m_panelFile->getInsertX());
      int yOffset = QD_Unit(m_panelFile->getInsertY());

	   // Changing rotation for mirrored side here, possibly WriteCoordinate
	   // should be doing it always, for all coords ?

	   double rot = RadToDeg(m_panelFile->getRotation());
	   if (surface == qdSurfaceBottom && !alreadyMirrored)
		   rot = 360.0 - rot; // bottom

      int rotation = NormalizeRotationQDOut(rot);
      WriteCoordinate(commentLine, xOffset, yOffset, rotation, surface);
   }

   // write basis
   CString basisID = "";
   basisID.Format("%s/%s.lb", passiveID, panelFileName);
   WriteBasis(surface, basisID);

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WriteCoordinate
*/
void QDExport::WriteCoordinate(CString comment, int xOffset, int yOffset, int rotation, EQdSurface surface)
{
   fprintf(fp, "%s%s %s\n", indent, QD_COORDINATE, QD_LEFT_CURLY_BRACKET);
   IndentPlus();

   // write comment
   fprintf(fp, "%s%s '%s'\n", indent, QD_COMMENT, comment);

   // Bottom file, change x to - x
   if (surface != qdSurfaceTop)
	{
	   xOffset = -xOffset;
	}

   // write offset and angle
	fprintf(fp, "%s%s %s%d , %d %s %d%s\n", indent, QD_OFFSET, QD_LEFT_PARENTHESIS, 
            xOffset, yOffset, QD_ANGLE, rotation, QD_RIGHT_PARENTHESIS);

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WriteBasis
*/
void QDExport::WriteBasis(EQdSurface surface, CString basisID)
{
   fprintf(fp, "%s%s '%s' %s\n",  indent, QD_BASIS, basisID, QD_LEFT_CURLY_BRACKET);
   IndentPlus();

   POSITION filePos = m_doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *pcbFile = m_doc->getFileList().GetNext(filePos);

      if (pcbFile->getBlockType() == blockTypePcb)
      {
         BOOL mirror = FALSE;
         do
         {
            int boardCount = m_outFileSettings.boardIDStartNumber;
            //         int boardCount = 0;
            BOOL curMirror = mirror;

            POSITION dataPos = m_panelFile->getBlock()->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *pcbData = m_panelFile->getBlock()->getDataList().GetNext(dataPos);
               if (!pcbData->isInsertType(insertTypePcb))
                  continue;

               if (pcbData->getInsert()->getBlockNumber() != pcbFile->getBlock()->getBlockNumber())
                  continue;

               if (pcbData->getInsert()->getMirrorFlags() & MIRROR_ALL)
               { 
                  if(curMirror & MIRROR_ALL)
                     mirror = FALSE;
                  else
                     mirror = pcbData->getInsert()->getMirrorFlags();
               }

               if (pcbData->getInsert()->getMirrorFlags() != curMirror)
                  continue;

               BlockStruct *boardGeom = m_doc->getBlockAt(pcbData->getInsert()->getBlockNumber());
               if (boardGeom == NULL)
                  continue;

               // Write the data for each inserted boards
               CString passiveListID = "";
               CString basisListID = "";
               CString pcbFileName = check_name('r', pcbFile->getName());

               if (boardCount == 0)
               {
                  // Board count is zero so don't include the number in the name
                  if (curMirror)
                  {
                     passiveListID.Format("%s/%s_M.lp", basisID, pcbFileName);
                     basisListID.Format("%s/%s_M.lp/%s_M.lb", basisID, pcbFileName, pcbFileName);
                  }
                  else
                  {
                     passiveListID.Format("%s/%s.lp", basisID, pcbFileName);
                     basisListID.Format("%s/%s.lp/%s.lb", basisID, pcbFileName, pcbFileName);
                  }
               }
               else
               {
                  CString boardIDNumber;
                  boardIDNumber.Format("%0*d", m_outFileSettings.boardIDLength, boardCount);

                  // Board count is greater than zero so include the number in the name
                  if (curMirror)
                  {
                     passiveListID.Format("%s/%s_M%s%s.lp", basisID, pcbFileName, m_outFileSettings.boardIDPrefix, boardIDNumber);
                     basisListID.Format("%s/%s_M.lp/%s_M.lb", basisID, pcbFileName, pcbFileName);
                  }
                  else
                  {
                     passiveListID.Format("%s/%s%s%s.lp", basisID, pcbFileName, m_outFileSettings.boardIDPrefix, boardIDNumber);
                     basisListID.Format("%s/%s.lp/%s.lb", basisID, pcbFileName, pcbFileName);
                  }
               }

               if (boardCount == m_outFileSettings.boardIDStartNumber)
               {
                  pcbDataList.SetAt(pcbFileName, pcbData);
                  WritePassiveList(surface, passiveListID, basisListID, FALSE, pcbData);
               }
               else
               {
                  WritePassiveList(surface, passiveListID, basisListID, TRUE, pcbData);
               }

               boardCount++;
            }
         } while (mirror);
      }
   }
   
   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WritePassiveList
*/
void QDExport::WritePassiveList(EQdSurface surface, CString passiveListID, CString basisListID, BOOL basisLink, DataStruct *pcbData)
{
   fprintf(fp, "%s%s '%s' %s\n", indent, QD_PASSIVE, passiveListID, QD_LEFT_CURLY_BRACKET);
   IndentPlus();

   Machine_Origin machine;

   if (surface == qdSurfaceTop)
   {  
      machine.x = pcbData->getInsert()->getOriginX() - Machine_Top.x; 
      machine.y = pcbData->getInsert()->getOriginY() - Machine_Top.y;
   }
   else if (surface == qdSurfaceBottom)
   {
      machine.x = pcbData->getInsert()->getOriginX() - Machine_Bot.x; 
      machine.y = pcbData->getInsert()->getOriginY() - Machine_Bot.y;
   }

   // Case 2104
   if (NormalizePanelPosition)
   {
      // Apply panel offset to each board
      double panelX = m_panelFile->getInsertX();
      double panelY = m_panelFile->getInsertY();
      double panelR = RadToDeg(m_panelFile->getRotation());
      CTMatrix mat;

      if (surface == qdSurfaceBottom)
      {
         panelX = -panelX;
         mat.mirrorAboutYAxis(true);
      }

      mat.rotateDegrees(panelR);
      mat.translate(panelX, panelY);
      mat.transform(machine.x, machine.y);
   }

   int xOffset = QD_Unit(machine.x);
   int yOffset = QD_Unit(machine.y);

   int rotation =  round(RadToDeg(pcbData->getInsert()->getAngle()));

	if (surface == qdSurfaceBottom)
	{
		// bottom file - need to do 
		// QD rotation = 360 - CAMCAD pcb rotation
		rotation = 360 - rotation;

      // The X value we have is already mirrored, but the coord output func will
      // use -X for output on bottom, so negate it now to compensate.
      if (NormalizePanelPosition)
         xOffset = -xOffset;
	}

   if (NormalizePanelPosition)
   {
      // Compensate pcb for panel rotation. This is done after the normalization for
      // CAMCAD bottom pcb rotation, because this panel bottom rotation was set with
      // a direct view of the panel bottom surface, not see-through CAMCAD style.
      rotation += round(RadToDeg(m_panelFile->getRotation()));
   }

	// TOM - Case 1195 - print normalized rotation (only multiply by 100 after all other operations have completed)
	rotation = normalizeDegrees(rotation) * 100;
	WriteCoordinate(commentLine, xOffset, yOffset, rotation, surface);

   if (basisLink)
   {
      fprintf(fp, "%s%s '%s'\n", indent, QD_BASIS_LINK, basisListID);
   }
   else
   {
      fprintf(fp, "%s%s '%s' %s\n", indent, QD_BASIS, basisListID, QD_LEFT_CURLY_BRACKET);
      fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
   }

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WritePCBData
*/
void QDExport::WritePCBData(EQdSurface surface)
{
   indent = "";
   indentCount = 0;
   fprintf(fp, "%s%s %s\n", indent, QD_PCB_DATA, QD_LEFT_CURLY_BRACKET);
   IndentPlus();

   // write panel description
	CString panelFileName = check_name('r', m_panelFile->getName());
	CString panelID = "";
   panelID.Format(".qd/%s.lp/%s.lb", panelFileName, panelFileName);

   WriteBasisData(surface, panelID, m_panelFile->getBlock(), FALSE, FALSE);

   // write each board's description and data
   FIRST_BOARD = TRUE;
   POSITION dataPos = pcbDataList.GetStartPosition();
   while (dataPos)
   {
      CString pcbFileName;
      DataStruct *pcbData = NULL;
      pcbDataList.GetNextAssoc(dataPos, pcbFileName, pcbData);
      if (pcbData == NULL)
         continue;

      BlockStruct *boardGeom = m_doc->getBlockAt(pcbData->getInsert()->getBlockNumber());
      if (boardGeom == NULL)
         continue;

      if (dataPos == NULL)
         LAST_BOARD = TRUE;
      else 
         LAST_BOARD = FALSE;

      CString basisDataID = "";

      if (pcbData->getInsert()->getMirrorFlags())
         basisDataID.Format("%s/%s_M.lp/%s_M.lb", panelID, pcbFileName, pcbFileName);
      else
         basisDataID.Format("%s/%s.lp/%s.lb", panelID, pcbFileName, pcbFileName);

      WriteBasisData(surface, basisDataID, boardGeom, pcbData->getInsert()->getMirrorFlags(), TRUE);
   }

   IndentMinus();
   fprintf(fp, "%s%s", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WriteBasisData
*/
void QDExport::WriteBasisData(EQdSurface surface, CString basisDataID, BlockStruct* boardGeom, BOOL mirror, BOOL boardLevel)
{
	if (boardLevel && SUPPRESS_BOARDINFO)
	{
		// don't show board information
		IndentPlus();
	}
   else
   {
      fprintf(fp, "%s%s '%s' %s\n", indent, QD_BASIS, basisDataID, QD_LEFT_CURLY_BRACKET);
		IndentPlus();
      WriteBoardDescription(surface, boardGeom, mirror);
   }

   WriteComponentPlacement(surface, boardGeom, mirror);

   IndentMinus();

   if (SUPPRESS_BOARDINFO)
   {
      if (boardLevel && LAST_BOARD)
         fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
   }
   else
      fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WriteBoardDescription
*/
void QDExport::WriteBoardDescription(EQdSurface surface, BlockStruct* boardGeom, BOOL mirror)
{
   // Original comment: Always write out QD_UPPERSIDE, spec says so
   // Case dts0100507625 says otherwise. It says write top for top and bottom for bottom.
   fprintf(fp, "%s%s %s %s\n", indent, QD_BOARD, ((surface == qdSurfaceTop || !m_outFileSettings.use_unterseite) ? QD_UPPERSIDE : QD_UNDERSIDE), QD_LEFT_CURLY_BRACKET);
   IndentPlus();
   fprintf(fp, "%s%s '%s'\n", indent, QD_COMMENT, commentLine);

   // write processing code
   fprintf(fp, "%s%s %s\n", indent, QD_PROCESSING, QD_LEFT_SQUARE_BRACKET);
   IndentPlus();

	bool hasXOUT, hasFiducial;
	CheckForXOUTandFiducial(surface, boardGeom, hasXOUT, hasFiducial);
   if (hasXOUT)
      fprintf(fp, "%s%s\n", indent, QD_INK_DOT_RECOGNITION); // temperary hard coded
   else if (hasFiducial)
      fprintf(fp, "%s%s\n", indent, QD_PCB_POSITION_RECOGNITION); // temperary hard coded

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_SQUARE_BRACKET);

   double xMin = 0.0; 
   double yMin = 0.0;
   double xSize = 0.0;
   double ySize = 0.0;
   int thickness = 0;

   if (boardGeom->getBlockType() == blockTypePcb)
   {
      GetSize(boardGeom, GR_CLASS_BOARDOUTLINE, &xSize, &ySize);

      thickness = format.QD.boardThickness;

      if (boardGeom->getAttributesRef())
      {
         Attrib* attrib;

         if (boardGeom->getAttributesRef()->Lookup(m_doc->IsKeyWord(ATT_SIEMENS_THICKNESS_BOARD, 0), attrib))
         {
            thickness = attrib->getIntValue();
         }
      }

      GetLowerLeftCorner(boardGeom, GR_CLASS_BOARDOUTLINE, &xMin, &yMin);
   }
   else
   {
      GetSize(boardGeom, GR_CLASS_PANELOUTLINE, &xSize, &ySize);

      if (QD_Unit(xSize) > m_outFileSettings.maxPanelWidth)
      {
         fprintf(flog, "%s, Width of panel is greater than maximum width setting in QD.out.\n", boardGeom->getName());
         display_error++;
      }

      if (QD_Unit(ySize) > m_outFileSettings.maxPanelLength)
      {
         fprintf(flog, "%s, Length of panel is greater than maximum length setting in QD.out.\n", boardGeom->getName());
         display_error++;
      }

      thickness = format.QD.panelThickness;

      if (boardGeom->getAttributesRef())
      {
         Attrib* attrib;

         if (boardGeom->getAttributesRef()->Lookup(m_doc->IsKeyWord(ATT_SIEMENS_THICKNESS_PANEL, 0), attrib))
         {
            thickness = attrib->getIntValue();
         }
      }

      GetLowerLeftCorner(boardGeom, GR_CLASS_PANELOUTLINE, &xMin, &yMin);

      if (surface == qdSurfaceTop)
      {
         xMin = xMin - Machine_Top.x;
         yMin = yMin - Machine_Top.y;
      }
      else if (surface == qdSurfaceBottom)
      {
         xMin = xMin - Machine_Bot.x;
         yMin = yMin - Machine_Bot.y;
      }
   }

	if (mirror)
	{
		// not size, size is always positive   xSize = -xSize;
      xMin = -xMin;
	}

   // Mirror is entirely taken care of in the NormalizePanelPosition LL corner getter
	if (!NormalizePanelPosition && surface != qdSurfaceTop)
	{
		// Bottom file, change x to - x
		// not size, size is always positive   xSize = -xSize;
		xMin = -xMin;
	}

   // For panel only, pcb uses original coord system
   if (NormalizePanelPosition && boardGeom->getBlockType() == blockTypePanel)
   {
      // Flip length and width if panel is rotated on its side
      int panelRotDeg = normalizeDegrees(round(RadToDeg( m_panelFile->getRotation() )));
      if (panelRotDeg == 90 || panelRotDeg == 270)
      {
         double tmp = xSize;
         xSize = ySize;
         ySize = tmp;
      }
   }         

	fprintf(fp, "%s%s %d\n", indent, QD_LENGTH, QD_Unit(xSize));
   fprintf(fp, "%s%s %d\n", indent, QD_WIDTH, QD_Unit(ySize));
   fprintf(fp, "%s%s %d\n", indent, QD_THICKNESS, thickness);

   fprintf(fp, "%s%s (%d,%d)\n", indent, QD_CORNER, QD_Unit(xMin), QD_Unit(yMin));


   int tolerance  = 0;
   if (boardGeom->getBlockType() == blockTypePanel)  // is panel
      tolerance = format.QD.panelTolerance;
   else  // assume is board
      tolerance = format.QD.boardTolerance;
   fprintf(fp, "%s%s %i\n", indent, QD_TOLERANCE, tolerance);

   // write fiducial list
   if (boardGeom->getBlockType() == blockTypePcb)
      WriteFiducialSetList(surface, boardGeom, mirror, FALSE, hasXOUT);
   else
      WriteFiducialSetList(surface, boardGeom, mirror, TRUE, hasXOUT);

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WriteFiducialSetList
*/
void QDExport::WriteFiducialSetList(EQdSurface surface, BlockStruct* boardGeom, BOOL mirror, BOOL panelFiducial, BOOL hasXOUT)
{
   fprintf(fp, "%s%s %s\n", indent, QD_FIDUCIAL_SET_LIST, QD_LEFT_CURLY_BRACKET);
   IndentPlus();

   bool fidSurfaceError = false;

   CString fiducialSetID = "";
   POSITION dataPos = boardGeom->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *fiducialData = boardGeom->getDataList().GetNext(dataPos);

      if (fiducialData->getDataType() != T_INSERT)
         continue;

      if (fiducialData->getInsert()->getInsertType() != INSERTTYPE_FIDUCIAL && fiducialData->getInsert()->getInsertType() != INSERTTYPE_XOUT)
         continue;

      BlockStruct *fidBlock = m_doc->getCamCadData().getBlockAt(fiducialData->getInsert()->getBlockNumber());
      fidBlock->GenerateDesignSurfaceAttribute(m_doc->getCamCadData(), true);
 
      bool isFiducialMirrored = (mirror && !fiducialData->getInsert()->getLayerMirrored()) || (!mirror && fiducialData->getInsert()->getLayerMirrored());

      bool fidAccessibleTop = fidBlock->IsAccessibleFromTop(m_doc->getCamCadData(), isFiducialMirrored);
      bool fidAccessibleBot = fidBlock->IsAccessibleFromBottom(m_doc->getCamCadData(), isFiducialMirrored);

      // Error check, happens only on surface 0 pass, don't need the same messages twice.
      // If fid is neigther top nor bottom then report error.
      if (surface == qdSurfaceTop && !fidAccessibleTop && !fidAccessibleBot)
      {
         fprintf(flog, "Can not determine surface for %s [%s] Fiducial [%s] (Geometry [%s]), fiducial skipped.\n", 
            panelFiducial ? "Panel" : "Board", boardGeom->getName(), fiducialData->getInsert()->getRefname(), fidBlock->getName() );
         display_error++;
         if (!fidSurfaceError)  // If 1st time encountered, report message only once
         {
            CString msg;
            msg.Format("Could not determine surface for one or more %s fiducials in [%s], these fiducials have been skipped. See log file for details.",
               panelFiducial ? "panel" : "board", boardGeom->getName());

            ErrorMessage(msg, "QD Write");
         }
         fidSurfaceError = true;
         continue;
      }

      if (surface == qdSurfaceTop && !fidAccessibleTop)
         continue;
      else if (surface == qdSurfaceBottom && !fidAccessibleBot)
         continue;

      // *** need to get fiducialSetID

      if (fiducialSetID == "")
      {
         // Only write this the first time in the loop
         // It iss either 'SMD_PASS' or 'INK'
         if (hasXOUT)
            fiducialSetID = "INK";
         else
            fiducialSetID = "SMD_PASS"; 

         fprintf(fp, "%s'%s' %s\n", indent, fiducialSetID, QD_LEFT_PARENTHESIS);
         IndentPlus();

         fprintf(fp, "%s%s %s\n", indent, QD_FIDUCIAL_LIST, QD_LEFT_CURLY_BRACKET);
         IndentPlus();
      }

      CString fiducialID = "";

      if (fiducialData->getAttributesRef())
      {
         Attrib* attrib;

         if (fiducialData->getAttributesRef()->Lookup(m_doc->IsKeyWord(ATT_SIEMENS_FIDSYM, 0), attrib))
         {
            fiducialID = GetAttribStringValue(m_doc, attrib); //m_doc->ValueArray[attrib->getStringValueIndex()];
         }
         else 
         {
            fiducialID.Format("%d",(panelFiducial ? format.QD.panelFiducialSymbolNum : format.QD.boardFiducialSymbolNum));
         }
      }
      else 
      {
         fiducialID.Format("%d",(panelFiducial ? format.QD.panelFiducialSymbolNum : format.QD.boardFiducialSymbolNum));
      }

      WriteFiducialList(fiducialID, fiducialData, mirror, surface, panelFiducial);
   }


   if (fiducialSetID != "") //only write this if there is fiducial
   {
      // close bracket for fiducial list
      IndentMinus();
      fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);

      // write offset and angle, these value are hard code base on spec
      // therefore don't care if it is mirror or not
      int xOffset = 0;
      int yOffset = 0;
      int rotation = -2000000000;

      if (surface == qdSurfaceTop)
      {
         // Top file just write out x & y as is
         fprintf(fp, "%s%s %s%d , %d %s %d%s\n", indent, QD_OFFSET, QD_LEFT_PARENTHESIS, 
               xOffset, yOffset, QD_ANGLE, rotation, QD_RIGHT_PARENTHESIS);
      }
      else
      {
			xOffset = -xOffset;

         fprintf(fp, "%s%s %s%d , %d %s %d%s\n", indent, QD_OFFSET, QD_LEFT_PARENTHESIS, 
               xOffset, yOffset, QD_ANGLE, rotation, QD_RIGHT_PARENTHESIS);
      }

      // write code of fiducial set
      fprintf(fp, "%s%s %s\n", indent, QD_CODE, QD_LEFT_SQUARE_BRACKET);
      IndentPlus();

      if (hasXOUT)
      {
         fprintf(fp, "%s%s\n", indent, QD_FIDUCIAL_SET_VALID); // temperary hard coded
         fprintf(fp, "%s%s\n", indent, QD_INK_DOT);            // temperary hard coded
      }
      else
      {
         fprintf(fp, "%s%s\n", indent, QD_PCB_POSITION);       // temperary hard coded
         fprintf(fp, "%s%s\n", indent, QD_FIDUCIAL_SET_VALID); // temperary hard coded
      }

      IndentMinus();
      fprintf(fp, "%s%s\n", indent, QD_RIGHT_SQUARE_BRACKET);

      // close parenthesis for fiducialSetID
      IndentMinus();
      fprintf(fp, "%s%s\n", indent, QD_RIGHT_PARENTHESIS);
   }

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
}

/******************************************************************************
* WriteFiducialList
*/
void QDExport::WriteFiducialList(CString fiducialID, DataStruct* fiducialData, BOOL mirror, EQdSurface surface, BOOL panelFiducial)
{
   fprintf(fp, "%s'%s' %s\n", indent, fiducialID, QD_LEFT_PARENTHESIS);
   IndentPlus();

   // write offset and angle
   int x = 0; 
   int y = 0; 
   if (panelFiducial)
   {
      // Case dts0100501008 - Panel fid locations should be relative to machine origin.
      // The panel insert location represents how we had to move the panel geometry block
      // to get the selected panel corner to the origin. We want the pane fid locations relative
      // to that original location, which we can get from the inverse of the panel insert.
      // All we really have to do is apply the panel insert transform to the fid loc, to
      // see where it goes.

      CPoint2d fidloc(fiducialData->getInsert()->getOrigin2d());

      ///CBasesVector fiducialBasesVector = fiducialData->getInsert()->getBasesVector();
      
      CBasesVector panBV = m_panelFile->getTMatrix().getBasesVector();
      double panBVx = panBV.getOrigin().x;
      double panBVy = panBV.getOrigin().y;
      double panBVr = panBV.getRotationDegrees();
      bool   panBVm = panBV.getMirror();

      // For top side we can use same transform as cacmad panel, but not for bottom.
      // The QD preparer sets up the transform with the user looking at the bottom surface, instead
      // of through the top like camcad does. So some stuff is backwards from what is normal
      // for camcad. Need to get a transform that maps the user settings to camcad conventions.

      CTMatrix qdpanmat;
      if (panBVm || surface == qdSurfaceBottom)
      {
         qdpanmat.mirrorAboutYAxis(true);
         qdpanmat.rotateDegrees( normalizeDegrees( 360.0 - panBVr ) );
         qdpanmat.translate( -panBVx, panBVy );
      }
      else
      {
         qdpanmat.rotateDegrees( panBVr );
         qdpanmat.translate( panBVx, panBVy );
      }
        
      qdpanmat.transform( fidloc );
      
      // fidloc is already where it belongs, but in the general section below
      // it will flip x for surface, so flip it here so it flips back later.
      if (surface == qdSurfaceBottom)
         fidloc.x = -fidloc.x;

      if (surface == qdSurfaceTop)
      {    
         x = QD_Unit(fidloc.x - Machine_Top.x);
         y = QD_Unit(fidloc.y - Machine_Top.y);
      }
      else if (surface == qdSurfaceBottom)
      {
         x = QD_Unit(fidloc.x - Machine_Bot.x);
         y = QD_Unit(fidloc.y - Machine_Bot.y);
      }

   }
   else
   {
      x = QD_Unit(fiducialData->getInsert()->getOriginX());
      y = QD_Unit(fiducialData->getInsert()->getOriginY());
   }

   int rotation = round(RadToDeg(fiducialData->getInsert()->getAngle()));

	if (fiducialData->getInsert()->getPlacedBottom() && fiducialData->getInsert()->getGraphicMirrored() && CADIF_ROTATION)
		rotation = 360 - rotation;

	if (mirror)
   {
      x = -x;
      rotation = 360 - round(RadToDeg(fiducialData->getInsert()->getAngle()));
   }

   // Bottom file, change x to - x
   if (surface != qdSurfaceTop)
	{
		x = -x;
	}

   if(NormalizePanelPosition && panelFiducial)
   {
      rotation += round(RadToDeg(m_panelFile->getRotation()));
   }

	// TOM - Case 1195 - print normalized rotation (only multiply by 100 after all other operations have completed)
	rotation = normalizeDegrees(rotation) * 100;
   fprintf(fp, "%s%s %s%d , %d %s %d%s\n", indent, QD_OFFSET, QD_LEFT_PARENTHESIS, x, y, QD_ANGLE, rotation, QD_RIGHT_PARENTHESIS);

   // write fiducial code 
   fprintf(fp, "%s%s %s\n", indent, QD_CODE, QD_LEFT_SQUARE_BRACKET);
   IndentPlus();

   fprintf(fp, "%s%s\n", indent, QD_FIDUCIAL_VALID);  // temparary hard code
   if (fiducialData->getInsert()->getInsertType() == INSERTTYPE_XOUT)
      fprintf(fp, "%s%s\n", indent, QD_INK_DOT);

   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_SQUARE_BRACKET);
   
   IndentMinus();
   fprintf(fp, "%s%s\n", indent, QD_RIGHT_PARENTHESIS);
}

/******************************************************************************
* WriteComponentPlacement
*/
void QDExport::WriteComponentPlacement(EQdSurface surface, BlockStruct* boardGeom, BOOL mirror)
{
   WORD partNumberKeywordIndex = (WORD)m_doc->IsKeyWord(PARTNUMBER_KEYWORD, 0);
   WORD loadedKeywordIndex     = (WORD)m_doc->IsKeyWord(ATT_LOADED,0);
   WORD glueKeywordIndex       = (WORD)m_doc->IsKeyWord("GlueSpot",0);
   WORD smdKeywordIndex        = (WORD)m_doc->IsKeyWord(ATT_SMDSHAPE,0);
	WORD technologyKey			 = (WORD)m_doc->RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);

   bool botSurfaceFlag = (surface != qdSurfaceTop);
   bool component = false; 

   for (POSITION compPos = boardGeom->getDataList().GetHeadPosition();compPos != NULL;)
   {
      DataStruct *compData = boardGeom->getDataList().GetNext(compPos);
		InsertStruct *compDataInsert = compData->getInsert();

      if (compData->getDataType() != T_INSERT)
         continue;

      if (compDataInsert->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      if (((compDataInsert->getPlacedBottom() == botSurfaceFlag) && mirror) || ((compDataInsert->getPlacedBottom() != botSurfaceFlag) && !mirror))
         continue;

		bool hasValue			= false;
      bool smdFlag			= false;
      bool loadedAttValue	= true;
      bool glueAttFlag		= false;
      bool glueAttValue		= false;
      CString partNumberKeyValue = "0";  // TOM - Case 1202 - If no PartNumber, default should be 0

      if (compData->getAttributesRef() != NULL && m_outputAttrib.GetKeyword() > -1)
      {
			hasValue = m_outputAttrib.HasValue(&compData->getAttributesRef());

         Attrib* attrib;
   //      if (compData->getAttributesRef()->Lookup((WORD)m_outputAttrib.GetKeyword(), attrib) && attrib != NULL)
			//{
			//	CString technology = doc->ValueArray.GetAt(attrib->getStringValueIndex());
			//	if (technology.CompareNoCase("SMD") == 0)
			//		smdFlag = true;
			//}

         if (compData->getAttributesRef()->Lookup(partNumberKeywordIndex, attrib))
         {
            partNumberKeyValue = GetAttribStringValue(m_doc, attrib); //m_doc->ValueArray[attrib->getStringValueIndex()];
         }

         if (compData->getAttributesRef()->Lookup(loadedKeywordIndex, attrib))
         {
            CString value = GetAttribStringValue(m_doc, attrib); //m_doc->ValueArray[attrib->getStringValueIndex()];
            loadedAttValue = (value.CompareNoCase("FALSE") != 0);
         }

         glueAttFlag = (compData->getAttributesRef()->Lookup(glueKeywordIndex, attrib) != 0);
         if (glueAttFlag)
         {
            CString value = GetAttribStringValue(m_doc, attrib); //m_doc->ValueArray[attrib->getStringValueIndex()];
            glueAttValue = (value.CompareNoCase("FALSE") != 0);
         }
      }

		// TOM - Case 1144 Only write out if loaded or if specified to write unloaded 
		if (!loadedAttValue && SUPPRESS_UNLOADED_COMPS)
			continue;

      // only write SMD components
      //if (!smdFlag)
      //   continue;

		if (!hasValue)
			continue;

      if (!component)
      {
         if (!SUPPRESS_BOARDINFO)
         {
				fprintf(fp, "%s%s %s %s\n", indent, QD_PROGRAM_PLACEMENT, ((surface == qdSurfaceTop || !m_outFileSettings.use_unterseite) ? QD_UPPERSIDE : QD_UNDERSIDE), QD_LEFT_CURLY_BRACKET);
         }


			IndentPlus();
         component = true;
      }

      // write component
      fprintf(fp, "%s%s '%s'\n", indent, QD_COMPONENT, check_name('b',partNumberKeyValue));
      fprintf(fp, "%s%s\n", indent, QD_LEFT_CURLY_BRACKET); 
      IndentPlus();

      // write comment
      CString refdes = compDataInsert->getRefname();
      fprintf(fp, "%s%s '%s'\n", indent, QD_COMMENT, refdes);

      if (!partNumberKeyValue.CompareNoCase("PARTNUMBER"))
      {
         if (botSurfaceFlag)
         {
            fprintf(flog, "Panel:BOTTOM, Board:%s, Component %s without partnumber keyword(%s)\n", boardGeom->getName(),  refdes, PARTNUMBER_KEYWORD);
            display_error++;
         }
         else
         {
            fprintf(flog, "Panel:TOP, Board:%s, Component %s without partnumber keyword(%s)\n", boardGeom->getName(), refdes, PARTNUMBER_KEYWORD);
            display_error++;
         }
      }

      // write processing code
      // we need to check if the attribute LOADED is not FALSE first; 
      // we don't add the BESTUKEN keyword only if loaded is false.
      fprintf(fp, "%s%s %s\n", indent, QD_PROCESSING, QD_LEFT_SQUARE_BRACKET);
      IndentPlus();

      if (loadedAttValue) // if loaded attribute value is not false
         fprintf(fp, "%s%s\n", indent, QD_PLACEMENT); // temparary hard code
      else
         fprintf(fp, "%s%s\n", indent, QD_BLOCK); 

      bool glueFlag = ((surface == qdSurfaceTop && format.QD.glueTop) || 
                       (surface == qdSurfaceBottom && format.QD.glueBot)    );

      if (glueAttFlag)
      {
         glueFlag = glueAttValue;
      }

      if (glueFlag && loadedAttValue)
         fprintf(fp, "%s%s\n", indent, QD_GLUE);

      // write offset and angle
      IndentMinus();
      fprintf(fp, "%s%s\n", indent, QD_RIGHT_SQUARE_BRACKET);


		// Get X and Y offset
		double xOffset = 0.0;
		double yOffset = 0.0;
		if (XOFFSET.Trim().IsEmpty() == false)
		{
			int xOffsetKey = m_doc->IsKeyWord(XOFFSET, 0);
			if (xOffsetKey > -1 && compData->getAttributes() != NULL)
			{
				Attrib *attrib = NULL;
				if (compData->getAttributes()->Lookup((WORD)xOffsetKey, attrib) && attrib != NULL)
				{
					if (attrib->getValueType() == valueTypeDouble)
						xOffset = attrib->getDoubleValue();
					else if (attrib->getValueType() == valueTypeInteger)
						xOffset = attrib->getIntValue();
					else if (attrib->getValueType() == valueTypeString)
						xOffset = atof(GetAttribStringValue(m_doc, attrib)); //m_doc->ValueArray[attrib->getStringValueIndex()]);
				}
			}
		}
		if (YOFFSET.Trim().IsEmpty() == false)
		{
			int yOffsetKey = m_doc->IsKeyWord(YOFFSET, 0);
			if (yOffsetKey > -1 && compData->getAttributes() != NULL)
			{
				Attrib *attrib = NULL;
				if (compData->getAttributes()->Lookup((WORD)yOffsetKey, attrib) && attrib != NULL)
				{
					if (attrib->getValueType() == valueTypeDouble)
						yOffset = attrib->getDoubleValue();
					else if (attrib->getValueType() == valueTypeInteger)
						yOffset = attrib->getIntValue();
					else if (attrib->getValueType() == valueTypeString)
						yOffset = atof(GetAttribStringValue(m_doc, attrib)); //m_doc->ValueArray[attrib->getStringValueIndex()]);
				}
			}
		}


      int x = 0;
      int y = 0;
		int rotation = 0;
      DataStruct *centroidData = centroid_exist_in_block(m_doc->getBlockAt(compDataInsert->getBlockNumber()));

      if (centroidData && XYOFFSET_ROTATION_BY_CENTROID)
      {
         DTransform xform(compDataInsert->getOriginX(), compDataInsert->getOriginY(), 1, compDataInsert->getAngle(), compDataInsert->getMirrorFlags());
         Point2 p;

			// TOM - Case 1159
			// moved offset from after the transform to before the transform
         p.x = centroidData->getInsert()->getOriginX() + xOffset;
         p.y = centroidData->getInsert()->getOriginY() + yOffset;

         xform.TransformPoint(&p);

         x = QD_Unit(p.x);
         y = QD_Unit(p.y);
			rotation = round(RadToDeg(compDataInsert->getAngle()));
      } 
      else
      {
         x = QD_Unit(compDataInsert->getOriginX() + xOffset);
         y = QD_Unit(compDataInsert->getOriginY() + yOffset);
			rotation = round(RadToDeg(compDataInsert->getAngle()));
      }

		if (botSurfaceFlag)
		{
			rotation = normalizeDegrees(360 - rotation);
		}


		if (mirror)
      { 
			// Board is mirror, so mirror the x coordinate
         x = -x;
			// Case 1646 and Case 1680
			// Must adjust rotation here, but not by subtracting 180 (case 1646).
			// Need to use rotation reversal (360-angle) (case 1680).
			rotation = normalizeDegrees(360 - rotation);
      }

      // Case 2065 - Centroid rotation offset has to be added AFTER normalization
      // of insert for mirroring. Previously it was added to insert rotation and whole
      // thing was normalized for mirror, that was wrong.
      if (centroidData && XYOFFSET_ROTATION_BY_CENTROID)
      {
         rotation += round(centroidData->getInsert()->getAngleDegrees());
      }

		// Add feeder rotation specified in ".in" file
		if (ROTATION_OFFSET.Trim().IsEmpty() == false)
		{
			int rotationKey = m_doc->IsKeyWord(ROTATION_OFFSET, 0);
			if (rotationKey > -1 && compData->getAttributes() != NULL)
			{
				Attrib *attrib = NULL;
				if (compData->getAttributes()->Lookup((WORD)rotationKey, attrib) && attrib != NULL)
				{
					if (attrib->getValueType() == valueTypeDouble)
						rotation = rotation + (round(attrib->getDoubleValue()));
					else if (attrib->getValueType() == valueTypeInteger)
						rotation = rotation + (attrib->getIntValue());
					else if (attrib->getValueType() == valueTypeString)
						rotation = rotation + round(atof(GetAttribStringValue(m_doc, attrib)));  //m_doc->ValueArray[attrib->getStringValueIndex()]));
				}
			}
		}


      // Bottom file, change x to - x
		if (botSurfaceFlag)
		{
			x = -x;
		}

		// TOM - Case 1195 - print normalized rotation (only multiply by 100 after all other operations have completed)
		rotation = normalizeDegrees(rotation) * 100;
      fprintf(fp, "%s%s %s%d , %d %s %d%s\n", indent, QD_OFFSET, QD_LEFT_PARENTHESIS, x, y, QD_ANGLE, rotation, QD_RIGHT_PARENTHESIS);

      // DR 738437 Add EBENE (aka Placement Sequence) output. Value is based on attrib, attrib is
      // named in dq.out in the command .SEQUENCE_ATTRIB.
      if (!m_outFileSettings.sequenceAttribName.IsEmpty())
      {
         // Lookup attrib, output value as EBENE. If attrib is not present then output nothing.
          0;
         Attrib *attrib = NULL;
         int psKw = m_doc->IsKeyWord(m_outFileSettings.sequenceAttribName, 0);
         if (psKw > -1 && compData->lookUpAttrib(psKw, attrib))
         {
            int placementSequence = 0;
				
            if (attrib->getValueType() == valueTypeDouble)
               placementSequence = round(attrib->getDoubleValue());
            else if (attrib->getValueType() == valueTypeInteger)
               placementSequence = attrib->getIntValue();
            else if (attrib->getValueType() == valueTypeString)
               placementSequence = atoi(attrib->getStringValue());

            fprintf(fp, "%s%s %d\n", indent, QD_PLACEMENT_SEQUENCE, placementSequence);
         }
         
      }

      IndentMinus();
      fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
   }

   if (!component)
   {
      // only write this if no component is found after the loop
      if (!SUPPRESS_BOARDINFO)
         fprintf(fp, "%s%s\n", indent, QD_PROGRAM_PLACEMENT);
   }
   else
   {
      // close bracket for Program_Placement
      IndentMinus();

//      if (!SUPPRESS_BOARDINFO && FIRST_BOARD)
      if (!SUPPRESS_BOARDINFO)
		{
         fprintf(fp, "%s%s\n", indent, QD_RIGHT_CURLY_BRACKET);
         FIRST_BOARD = FALSE;
		}
   }
}

/******************************************************************************
* GetSize
*/
void QDExport::GetSize(BlockStruct *block, int graphicClass, double *xSize, double *ySize)
{
   *xSize = *ySize = 0;

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

   Mat2x2 m;
   RotMat2(&m, 0);

   // find panel outline
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // found Panel Outline
      if (data->getGraphicClass() == graphicClass && data->getDataType() == T_POLY)
      {
         ExtentRect extents;
         PolyExtents(m_doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, FALSE);

         // get extents
         if (extents.left < xmin) xmin = extents.left;
         if (extents.right > xmax) xmax = extents.right;
         if (extents.bottom < ymin) ymin = extents.bottom;
         if (extents.top > ymax) ymax = extents.top;
      }
   }

   if (xmax > xmin)
   {
      *xSize = xmax - xmin;
      *ySize = ymax - ymin;
   }
}

/******************************************************************************
* GetLowerLeftCorner
*/
void QDExport::GetLowerLeftCorner(BlockStruct *block, int graphicClass, double *xMin, double *yMin)
{
   Mat2x2 m;
   //RotMat2(&m, DegToRad(rotDegrees));

   // These values used for board outline
   double offsetX = 0.0;
   double offsetY = 0.0;
   double offsetR = 0.0;

   int mirror = (this->m_panelFile->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

   // We are normalizing panel location to 0,0,0, so need to apply the panel
   // offsets to get proper panel outline location.
   if (NormalizePanelPosition && graphicClass == GR_CLASS_PANELOUTLINE)
   {
      //mirror = (this->m_panelFile->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);
      offsetX = this->m_panelFile->isMirrored() ? -this->m_panelFile->getInsertX() : this->m_panelFile->getInsertX();
      offsetY = this->m_panelFile->getInsertY();
      offsetR = this->m_panelFile->getRotation();
   }

   RotMat2(&m, offsetR);

   // find panel outline
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // found Panel Outline
      if (data->getGraphicClass() == graphicClass && data->getDataType() == T_POLY)
      {
         ExtentRect extents;
         //PolyExtents(m_doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, FALSE);
         PolyExtents(m_doc, data->getPolyList(), &extents, 1, mirror, offsetX, offsetY, &m, FALSE);

         *xMin = extents.left;
         *yMin = extents.bottom;
         return;
      }
   }

   *xMin = 0;
   *yMin = 0;
}

/******************************************************************************
*/
void QDExport::GetPanelCorner(BlockStruct *block, int graphicClass, ECCorner corner, double *xCorn, double *yCorn)
{
   Mat2x2 m;
   //RotMat2(&m, DegToRad(rotDegrees));

   // These values used for board outline
   double offsetX = 0.0;
   double offsetY = 0.0;
   double offsetR = 0.0;

   int mirror = (this->m_panelFile->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

   // We are normalizing panel location to 0,0,0, so need to apply the panel
   // offsets to get proper panel outline location.
   if (NormalizePanelPosition && graphicClass == GR_CLASS_PANELOUTLINE)
   {
      //mirror = (this->m_panelFile->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);
      offsetX = this->m_panelFile->isMirrored() ? -this->m_panelFile->getInsertX() : this->m_panelFile->getInsertX();
      offsetY = this->m_panelFile->getInsertY();
      offsetR = this->m_panelFile->getRotation();
   }

   RotMat2(&m, offsetR);

   // find panel outline
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // found Panel Outline
      if (data->getGraphicClass() == graphicClass && data->getDataType() == T_POLY)
      {
         ExtentRect extents;
         //PolyExtents(m_doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, FALSE);
         PolyExtents(m_doc, data->getPolyList(), &extents, 1, mirror, offsetX, offsetY, &m, FALSE);

         switch (corner)
         {
         case 	LowerLeft:
            *xCorn = extents.left;
            *yCorn = extents.bottom;
            break;
         case LowerRight:
            *xCorn = extents.right;
            *yCorn = extents.bottom;
            break;
         case UpperLeft:
            *xCorn = extents.left;
            *yCorn = extents.top;
            break;
         case UpperRight:
            *xCorn = extents.right;
            *yCorn = extents.top;
            break;
         default:
            *xCorn = 0;
            *yCorn = 0;
            break;
         }

         return;
      }
   }

   *xCorn = 0;
   *yCorn = 0;
}

/******************************************************************************
* IndentPlus
*/
void QDExport::IndentPlus()
{
   indentCount++;
   indent.Format("%*s", indentCount * 7, " ");
}

/******************************************************************************
* IndentMinus
*/
void QDExport::IndentMinus()
{
   if (indentCount < 1)
   {
      indent = "";
      indentCount = 0;
      ErrorMessage("Error in IndentMinus()");
   }
   else
   {
      indentCount--;
      indent.Format("%*s", indentCount * 7, " ");
   }
}

/******************************************************************************
* NormalizeRotationQDOut
*/
int QDExport::NormalizeRotationQDOut(double rotation)
{
   int newRotation = round(rotation);

   while (newRotation < 0)
      newRotation += 360;
   while (newRotation >= 360)
      newRotation -= 360;


	// Make rotation be the closest orthoganal value
   if ((newRotation % 90) > 45)
		// if mod is greater than 45, take the closest ceiling orthogonal
      newRotation = 90 * (newRotation / 90 + 1);
   else
		// if mode is less than 45, take the closest floor orthogonal
      newRotation = 90 * (newRotation / 90);

	// QD wants the output degrees multiplied by 100
   return newRotation * 100;
}

/******************************************************************************
* Find_Machine_Origin
*/
void QDExport::Find_Machine_Origin()
{
   BOOL TOPFOUND = FALSE;
   BOOL BOTFOUND = FALSE;

   POSITION filePos = m_doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *pcbFile = m_doc->getFileList().GetNext(filePos);
      if (pcbFile->getBlockType() != blockTypePcb)
         continue;
   
      POSITION dataPos = m_panelFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *pcbData = m_panelFile->getBlock()->getDataList().GetNext(dataPos);
         BlockStruct *MachineBlock = m_doc->Find_Block_by_Num(pcbData->getInsert()->getBlockNumber());
         if (MachineBlock == NULL)
            continue;

         if (pcbData->getDataType() != T_INSERT)
            continue;

         CString name;
         if (pcbData->getInsert()->getPlacedBottom() == 0)
         {
            if (!Machine_Origin_Top_Type.CompareNoCase("GEOM"))
               name = MachineBlock->getName();
            else if (!Machine_Origin_Top_Type.CompareNoCase("REF"))
               name = pcbData->getInsert()->getRefname();

            if (!name.CompareNoCase(Machine_Origin_Top) && Machine_Origin_Top != "")
            {
               Machine_Top.x = pcbData->getInsert()->getOriginX();
               Machine_Top.y = pcbData->getInsert()->getOriginY();
               TOPFOUND = TRUE;
            }

            // Only use the top component if there is not one found at the bottom
            if (!BOTFOUND && !name.CompareNoCase(Machine_Origin_Bot) && Machine_Origin_Bot != "")
            {
               Machine_Bot.x = pcbData->getInsert()->getOriginX();
               Machine_Bot.y = pcbData->getInsert()->getOriginY();
               BOTFOUND = TRUE;
            }
         }
         if (pcbData->getInsert()->getPlacedBottom() == 1)
         {
            if (!Machine_Origin_Bot_Type.CompareNoCase("GEOM"))
               name = MachineBlock->getName();
            else if (!Machine_Origin_Bot_Type.CompareNoCase("REF"))
               name = pcbData->getInsert()->getRefname();

            if (!name.CompareNoCase(Machine_Origin_Bot) && Machine_Origin_Bot != "")
            {
               Machine_Bot.x = pcbData->getInsert()->getOriginX();
               Machine_Bot.y = pcbData->getInsert()->getOriginY();
               BOTFOUND = TRUE;
            }
         } 
      }
   }
   if (Machine_Origin_Top != "" && !TOPFOUND)
   {
      fprintf(flog, "\"%s\" for MACHINE_ORIGIN_TOP is not found in the design.\n", Machine_Origin_Top);
      display_error++;
   }
   if (Machine_Origin_Bot != "" && !BOTFOUND)
   {
      fprintf(flog, "\"%s\" for MACHINE_ORIGIN_BOT is not found in the design.\n", Machine_Origin_Bot);
      display_error++;
   }

}

/******************************************************************************
* Inverse_QD_Unit
*/
double QDExport::Inverse_QD_Unit(int number)
{
   number = number/1000;
   return number * Units_Factor(UNIT_MM, m_doc->getSettings().getPageUnits());
}

/******************************************************************************
* QD_Unit
*/
int QDExport::QD_Unit(double number)
{
   return round(number * (Units_Factor(m_doc->getSettings().getPageUnits(), UNIT_MM) * 1000));
}

/******************************************************************************
* CheckForXOUT
*/
void QDExport::CheckForXOUTandFiducial(EQdSurface surface, BlockStruct* boardGeom, bool& hasXOUT, bool& hasFiducial)
{
	hasXOUT = false;
	hasFiducial = false;

   POSITION pos = boardGeom->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = boardGeom->getDataList().GetNext(pos);
      if (data->getDataType() == T_INSERT)
      {
         if ((data->getInsert()->getGraphicMirrored() && surface != qdSurfaceBottom) ||
            (!data->getInsert()->getGraphicMirrored() && surface == qdSurfaceBottom))
            continue;

         if (data->getInsert()->getInsertType() == insertTypeXout)
            hasXOUT = true;
         else if (data->getInsert()->getInsertType() == insertTypeFiducial)
            hasFiducial = true;

         if (hasXOUT && hasFiducial)
            return;
      }
   }
}

/******************************************************************************
*/
//void QDExport::SaveQDPanelLocationSettings()
//{
//	if (m_panelFile != NULL && m_panelFile->getBlock() != NULL)
//	{
//		m_doc->SetUnknownAttrib(&m_panelFile->getBlock()->getDefinedAttributes(),"SIEMENS_QD_PANEL_LOCATION_TOP",    m_topPanelLocation.GetSettingsString(), SA_OVERWRITE, NULL);
//		m_doc->SetUnknownAttrib(&m_panelFile->getBlock()->getDefinedAttributes(),"SIEMENS_QD_PANEL_LOCATION_BOTTOM", m_botPanelLocation.GetSettingsString(), SA_OVERWRITE, NULL);
//	}
//}



/******************************************************************************
*/

QDOutputAttrib::QDOutputAttrib(CCEtoODBDoc& camCadDoc) :
   m_camCadDatabase(camCadDoc)
{
	SetDefaultAttribAndValue();
}

QDOutputAttrib::~QDOutputAttrib()
{
	m_valuesMap.RemoveAll();
}

int QDOutputAttrib::GetKeyword() const
{
	return m_attribKeyword;
}

bool QDOutputAttrib::HasValue(CAttributes** attributes)
{
	CString value, tmpValue;
	m_camCadDatabase.getAttributeStringValue(value, attributes, m_attribKeyword);
	value.MakeLower();
	return (m_valuesMap.Lookup(value, tmpValue)==TRUE)?true:false;
}

void QDOutputAttrib::SetKeyword(const CString keyword)
{
	m_attribKeyword = m_camCadDatabase.getKeywordIndex(keyword);
	m_valuesMap.RemoveAll();
}

void QDOutputAttrib::AddValue(const CString value)
{
	CString tmpValue = value;
	m_valuesMap.SetAt(tmpValue.MakeLower(), tmpValue.MakeLower());
}

void QDOutputAttrib::SetDefaultAttribAndValue()
{
	SetKeyword(ATT_TECHNOLOGY);
	AddValue("SMD");
}// C:\Development\CAMCAD\4.5\read_wrt\QD_out.cpp : implementation file
//
