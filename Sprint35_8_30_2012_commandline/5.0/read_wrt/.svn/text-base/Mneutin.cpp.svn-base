// $Header: /CAMCAD/4.6/read_wrt/Mneutin.cpp 83    5/02/07 2:08p Rick Faltersack $

/****************************************************************************/
/*
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.

   Correct pad surface not implemented

   The component_center attribute G_ATTR will change the geometry definition,
   so that the center is the zero-point of the geom.

   N_PIN - all components are created from GEOM. N_PIN not used

*/           

#include "stdafx.h"
#include "mentor.h"
#include "mneutin.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "centroid.h"
#include "RwLib.h"
#include <float.h>
#include "FileList.h"
#include "DFT.h"
#include "CCEtoODB.h"
#include "Net_Util.h"
#include "RwUiLib.h"
#include "Consolidator.h"


extern CProgressDlg *progress;
extern char *testaccesslayers[];
//extern LayerTypeInfoStruct layertypes[];

/* STATIC PROTOTYPES ************************************************************************/

static void DoFirstPass(CStdioFile *fMen);
static void DoNeutral(CStdioFile *fMen);
static void DoNetlist(CStdioFile *fMen);
static void Do_UNNAMEDNET_Rename();
static void DoPanelOutline();

static void init_mem();
static void deinit_mem();
static void set_skipBoard();
static void LoadSettings(const CString filename, CGeometryConsolidatorControl &consolControl);

static void CreateLayerRangePadstackData(CDataList *dataList, int startLayer, int endLayer);

static int UpdateLayer(CString layerName);
static void update_partnumber(BlockStruct *b);
static void update_centroid(CCEtoODBDoc *Doc);
static int update_padaccesslayer(const char *top, const char *bot);
static int update_mirrorlayers();
static int get_drillindex(const char *pref, double size, int layernum, int unplated);
static int get_padformindex(int form, double sizeA, double sizeB);
static int is_attrib(const char *name, int attrType);

static BOOL GetNextLine(CStdioFile *fMen, CString *fMenLine);
static int get_nextword(char *lp1, char *token,int maxtok, char *attval, int maxatt);
static int get_nextattr(char *lp1, char *token,int maxtok, char *attval, int maxatt);
static BOOL IsContinuationLine(CString line);
static double cnv_tok(char *tok);
static void cnv_addrot(int mirrorRot, int *rot, int*mir);
static int is_command(char *l, List *c_list, int siz_of_command);
static void DoUnits(CString unitName);
static CString GetNetname(char *string);
static BOOL IsUnnamedNet(CString netname);
static void SearchForXOUT(CCEtoODBDoc *doc);

static int update_layerattr();
static void check_create();
static int get_geomlistptr(const char *l);
static int GetPadstackIndex(CString padstackName);
static int GetGeomToFiducial(CString geomName);
static int GetXOutGeom(CString geomName);
static void GetDefaultLayers();
static void CreateLayer(CString layerName, int topLayerType, int botLayerType);
static BOOL IsGenericTopBottomMappingLayer(CString layerName, CString &layerName_1, CString &layerName_2);
static BOOL IsSpecificTopBottomMappingLayer(CString layerName, CString &genericName, BOOL &isTop);
static int AddDataToLayer(CString layerName, CDataList *dataList, DataStruct *data, int dataType);
static void CheckForGeomInBoardstation(int neutralFileNum, int boardstationFileNum);
static void ReassignGeomToInsert(int oldBlockNum, int newBlockNum);
static void ReassignGeomToDeviceType(int oldBlockNum, int newBlockNum);


/* STATIC VARIABLES ************************************************************************/

static CCEtoODBDoc				*doc;
static FileStruct				*boardFile;    // this is the board file
static FileStruct				*panelFile;    // this is the panel file
static BlockStruct			*cur_block;    // this is the current block structure
static DataStruct				*cur_data;     // this is the current block structure
static NetStruct				*cur_net;      // this is the current netname struct
static double					scaleFactor = 1;
static double					unit_to_inch_factor = 1;
static int						pageUnits;

static FILE						*fLog;
static int						display_error;
static long						fMenLineCount = 0;

static int						neutralPcbFileNum;
static int						boardstationPcbFileNum;
static CMapStringToString	pinEditedGeomMap;

static int						cur_geomlistptr;
static int						cur_compplaced;
static int						cur_layer;
static int						cur_entity;
static int						cur_create = 0;
static int						cur_boardcnt, tot_boardcnt, firstboardptr;
static CString					cur_name;
static NeutralBoard			cur_boardname[255];
static NeutralComp			cur_comp;
static char						*component_default_padstack = "DEFAULT_PAD";
static char						*curlp;

static CStringList			skipPrefixList;
static CStringList			skipBADDPList;
static CStringList			skipPADDPList;
static int						skipBoard;
static int						skipComp;

static int						sectionStatus;					// Flags for completed sections
static int						only_one_board = FALSE;    // Disable multiple boards on Pantheon output
static int						padtype;							// PADTYPE_THRU, PADTYPE_SURF, PADTYPE_BLIND
static int						padlayercnt;					// This is used to derive electrical layer stacking
																		// from the pad layers. Only valid on thru and blind pad, 
																		// because a thru and blind pad goes from top to bottom
static CPadformArray			padFormArray;

static NeutralLayer			neutralLayerArray[MAX_LAYERS];
static int						neutralLayerArrayCount;

static CString					thruPadLayerArray[MAX_LAYERS];
static int						thruPadLayerCount;

static NeutralAttr			attrArray[MAX_ATTR];
static int						attrArrayCount;

static NeutralAdef			layer_attr[MAX_LAYERS];       /* Array of layers from pdif.in   */
static int						layer_attr_cnt;

static CPolyArray				polyarray;
static int						polycnt;

static CDrillArray			drillarray;
static int						drillcnt;

static PartnumberArray		partnumberArray;
static int						partnumberArrayCount;

static GeomArray				geomArray;
static int						geomArrayCount;

static PadstackArray			padstackArray;
static int						padstackArrayCount;

static PanelArray				panelArray;
static int						panelArrayCount;

static CAttribmapArray		attribmaparray;
static int						attribmapcnt = 0;

// Setting file variables
static CStringArray			boardOutlineNameArray;
static int						boardOutlineNameArrayCount;

static NeutralGeomToFid		geomToFidArray[MAX_ATTR];
static int						geomToFidCount = 0;

static CStringArray			unnamedNetArray;
static int						unnamedNetCount;

static NeutralXOutGeom		xOutGeomArray[MAX_ATTR];
static int						xOutGeomCount = 0;

static CMapStringToString  mentorLayersMap;
static CMapStringToPtr     layerToDataMap;

static int						unplaced_components; // checks if unplaced components should be translated.
static int						hierachical_netnames = FALSE;
static BOOL						fixBoardOutline;
static CString					centroid_attr;
static CString					centroid_angle;
static CString					panelMirrorBoardName;
static BOOL                     IgnoreEmptyPins; // added DF 2004.10.20
static BOOL						Import_PN_Device;

static CMneutTestPinList   TestPinList;
static CTestProbeDataList	TestProbeDataList;
/******************************************************************************
* ReadMentorNeutral
*/
void ReadMentorNeutral(const char *fullPathName, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits, int currentFileNum, 
      int fileCount)
{
   doc = Doc;
   pageUnits = PageUnits;
   boardFile = NULL;
   cur_block = NULL;
   cur_data  = NULL;
   scaleFactor = Units_Factor(UNIT_INCHES, pageUnits); // initialized to Inches
   CStringArray log_msgs;

   // need to do this, because panel only knows the path.ext
   char driveName[_MAX_DRIVE];
   char dirName[_MAX_DIR];
   char fileName[_MAX_FNAME];
   char extName[_MAX_EXT];
   _splitpath(fullPathName, driveName, dirName, fileName, extName);
   
   init_mem();

   // this can not be rt (because of LB terminator)
   CStdioFile fMen;
   if (!fMen.Open(fullPathName, CFile::modeRead | CFile::typeText))
   {
      ErrorMessage("Error open file", fullPathName);
      return;
   }

   CGeometryConsolidatorControl consolidatorControl;

   CString logFile = GetLogfilePath("NEUTRAL.LOG");
   CString fopenMode = "a+t";
   if (!currentFileNum)
   {
      fopenMode = "wt";
      display_error = 0;
   }

   fLog = fopen(logFile, fopenMode);
   if (!fLog)
   {
      CString err;
      err.Format("Error open [%s] file", logFile);
      ErrorMessage(err, "Error");
      return;
   }

   int fileType = tst_mentorbrdfiletype(fMen.m_pStream);
   if (fileType == MEN_WIRE)
   {
      // here find a file->getBlockType() which is marked as the blockTypePcb
      // the return fill be a NULL file, because this only updates
      // an existing file and does not create a new one.

      BOOL Found = FALSE;

      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *tempFile = doc->getFileList().GetNext(filePos);
         if (tempFile->getBlockType() == blockTypePcb)
         {
            boardFile = tempFile;
            doc->PrepareAddEntity(tempFile);
            Found = TRUE;
            break;
         }
      }
      
      if (Found)  // allow to load a wire file just to see write graphics.
      {
         do_wires(doc, boardFile, fLog, fMen.m_pStream, FALSE, pageUnits);
         
         update_layerattr();
         
         progress->SetStatus("Optimizing Database");
         
         Crack(doc, boardFile->getBlock(), TRUE);          

         double accuracy = get_accuracy(doc);
         EliminateOverlappingTraces(doc, boardFile->getBlock(), TRUE, accuracy);                
      }
      else
      {
         ErrorMessage("No PCB file type found - can not load!");
      }  
   }
   else // fileType != MEN_WIRE)
   {
      sectionStatus = 0;
      cur_create = 0;
      cur_boardcnt = 0; 
      cur_boardname[0].name = fileName;   // just initialize with filename, will be overwrittn on first board
      layer_attr_cnt = 0;

      fMenLineCount = 0;
      fMen.SeekToBegin();
      DoFirstPass(&fMen);

      tot_boardcnt = cur_boardcnt;
      
      padFormArray.SetSize(0,100);
      //padformcnt = 0;

      cur_geomlistptr = -1;

      if (cur_boardcnt == 0)
      {
         fprintf(fLog, "Warning : BOARD header line missing!\n");
         display_error++;
         cur_boardname[0].name = "NO_BOARD_HEADER";
         cur_boardname[0].x = 0.0;
         cur_boardname[0].y = 0.0;
         cur_boardname[0].rot = 0.0;
         tot_boardcnt = 1;
      }
      else
      {
         set_skipBoard();
      }


      firstboardptr = -1;
      
		int i=0;
      for (i=0; i<cur_boardcnt; i++)
      {
         if (cur_boardname[i].flag != 0)
         {
            firstboardptr = i;
            break;
         }
      }
      

      if (firstboardptr < 0) // no valid board found. Valid board is a board with contens
         firstboardptr = 0;


		// Check for Mentor Neutral file
		int count = 0;
		POSITION pos = doc->getFileList().GetHeadPosition();
		FileStruct *boardstationFile = NULL;
		while (pos)
		{
			FileStruct *file = doc->getFileList().GetNext(pos);
			if (file->getBlockType() != blockTypePcb)
				continue;
			if (file->getCadSourceFormat() != Type_Mentor_Layout)
				continue;

			boardstationFile = file;
			count++;
		}

		int answer = IDNO;
		CString msg = "";
		if (count == 1)
		{
			msg =  "There is a Mentor PCB file found.  Would you like to import into it?\n\n";
			msg += "   Yes to import into existing Mentor Boardstation file.\n";
			msg += "   No to import into a new PCB file.\n";
			answer = ErrorMessage(msg, "Mentor Neutral file found", MB_YESNO);

			if (answer == IDYES)
			{
				boardstationPcbFileNum = boardstationFile->getFileNumber();
			}
		}
		else if (count > 1)
		{
			msg =  "There are multiple Mentor PCB files found.  Would you like to select a file to import into it?\n\n";
			msg += "   Yes to import into existing Mentor PCB file.\n";
			msg += "   No to import into a new PCB file.\n";
			answer = ErrorMessage(msg, "Mentor Neutral file found", MB_YESNO);

			if (answer == IDYES)
			{
				SelectFileDlg dlg;
				dlg.doc = doc;
				dlg.SetFileType(blockTypePcb);
				dlg.SetSourceCad(Type_Mentor_Layout);
				if (dlg.DoModal() != IDOK)
					return;

				boardstationPcbFileNum = dlg.file->getFileNumber();
			}
		}


      boardFile = Graph_File_Start(cur_boardname[firstboardptr].name, Type_Mentor_Neutral_Layout);
      boardFile->setBlockType(blockTypePcb);
      boardFile->getBlock()->setBlockType(boardFile->getBlockType());

		neutralPcbFileNum = boardFile->getFileNumber();

      cur_boardname[firstboardptr].filenum = boardFile->getFileNumber();
      Graph_Level("0", "", 1);         // global layer 
      Graph_Level(LAYER_ALL, "", 0);   // signal layer, this is for all layers, such as global VIA_KEEPOUT etc... 

      // Must call this before calling LoadSettings


      // layers in settings file can not be loaded until the first file is open.
      CString settingsFile( getApp().getImportSettingsFilePath("neutral.in") );
      CString settingsFileMsg;
      settingsFileMsg.Format("\nMentor Neutral Reader: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(settingsFileMsg);
      LoadSettings(settingsFile, consolidatorControl);   // mirror layers must be done after file open.
      GetDefaultLayers();

      skipBoard = FALSE;
      skipComp = FALSE;
      cur_boardcnt = 0;
      thruPadLayerCount = 0;

      if (firstboardptr == 0) // no BOARD header entry found, start with one
         cur_create = CREATE_BOARD;

      fMen.SeekToBegin();
      fMenLineCount = 0;

      DoNeutral(&fMen);

      cur_boardcnt = 0; 

      if (firstboardptr == 0)
      {
         FileStruct *f = doc->Find_File(cur_boardname[cur_boardcnt].filenum);
         doc->PrepareAddEntity(f);
      }

      // here need to get pin location from block with movable and update them in the geom defintion.

      fMen.SeekToBegin();
      fMenLineCount = 0;
      DoNetlist(&fMen); // here also check movable pins
      Do_UNNAMEDNET_Rename();

      update_partnumber(boardFile->getBlock());
      update_centroid(doc);
      NetStruct *n = add_net(boardFile, NET_UNUSED_PINS);
      n->setFlagBits(NETFLAG_UNUSEDNET);

      fMen.Close();

      CString padtop, padbot;
      int highlayer = 0;

      for (i=0; i<neutralLayerArrayCount; i++)
      {
         NeutralLayer *layer = &neutralLayerArray[i];

         if (layer->padelectrical == 1)
            padtop = layer->name;

         if (layer->padelectrical > highlayer)
         {
            padbot = layer->name;
            highlayer = layer->padelectrical;
         }
      }

      if (update_padaccesslayer(padtop, padbot))
         generate_PADSTACKACCESSFLAG(doc, 1);

      padFormArray.RemoveAll(); // 60 mil round

		CheckForGeomInBoardstation(neutralPcbFileNum, boardstationPcbFileNum);
		RefreshInheritedAttributes(doc, SA_OVERWRITE);

      SearchForXOUT(doc);

      if (sectionStatus != (NEUT_GEOM | NEUT_B_ATTR | NEUT_COMP | NEUT_PAD))
      {
         CString temp;

         // incomplete message
         if (!(sectionStatus & NEUT_GEOM))
            temp += "GEOM section (Component definition) missing !\n";
         if (!(sectionStatus & NEUT_B_ATTR)) 
            temp += "B_ATTR section (Board definition) missing !\n";
         if (!(sectionStatus & NEUT_COMP))
            temp += "COMP section (Component placement) missing !\n";
         if (!(sectionStatus & NEUT_PAD))
            temp += "PAD section (Padstack definition) missing !\n";

         ErrorMessage(temp, "Incomplete NEUTRAL file !", MB_ICONEXCLAMATION | MB_OK);
         display_error = 0; // do not show read errors
      }

      TestPinList.MarkTestPinsInCamcad(doc, boardFile);
      TestProbeDataList.MarkProbeTestPointsInCamcad(doc, boardFile, &log_msgs);
   }
   
   for(int i = 0; i < log_msgs.GetCount(); i++)
      fprintf(fLog, log_msgs.GetAt(i));
   log_msgs.RemoveAll();

   fclose(fLog);

   if (consolidatorControl.GetIsEnabled())
   {
      CGeometryConsolidator squisher(doc, "" /*no report*/);
      squisher.Go( &consolidatorControl );
   }

   deinit_mem();

   if (display_error && currentFileNum == fileCount-1)
      Logreader(logFile);
}

/******************************************************************************
* DoFirstPass
*  - this routine finds 
*     1. the board name
*     2. the used units
*     3. all geoms and the component_center
*/
static void DoFirstPass(CStdioFile *fMen)
{
   CString fMenLine;
   char *line = NULL;

   while (GetNextLine(fMen, &fMenLine))
   {
      if (line)
         free(line);

      line = STRDUP(fMenLine);

      line = strtok(line, "\n");

      if (!line)
         continue;

      char tok[MAX_TOKEN];
      char att[MAX_ATT];

      if (!get_nextword(line, tok, MAX_TOKEN, att, MAX_ATT))
         continue;

      if (tok[0] == '#') // this is a remark
      {
         // the pantheon neutral file has an error on multiple board in a neutral file.
         // #  Pantheon Release 4.4_01S Neutral File Format
         if (strstr(fMenLine, "Pantheon"))
         {
            if (ErrorMessage("Pantheon Neutral Files do not handle multiple BOARD sections correctly, all information should be merged into 1 board!\nDo you want to merge all information into 1 board?", "Merge Multiple Boards", MB_YESNO | MB_DEFBUTTON2) == IDYES)
               only_one_board = TRUE;
         }

         continue;
      }

      if (!strcmp(tok, "BOARD"))
      {
         if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
            continue;

         if (cur_boardcnt < 255)
         {
            cur_boardname[cur_boardcnt].name = _strupr(tok);
            cur_boardname[cur_boardcnt].x = 0.0;
            cur_boardname[cur_boardcnt].y = 0.0;
            cur_boardname[cur_boardcnt].rot = 0.0;
            cur_boardname[cur_boardcnt].flag = 0;

            if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // OFFSET
               continue;
            if (STRCMPI(tok, "OFFSET"))                     
            {
               fprintf(fLog, "OFFSET expected at %ld\n", fMenLineCount);
               continue;
            }

            if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // x:nnn
               continue;
            if (_strnicmp(tok, "x:", 2))
            {
               fprintf(fLog, "x: expected at %ld\n", fMenLineCount);
               continue;
            }
            cur_boardname[cur_boardcnt].x = atof(&tok[2]);

            if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // y:nnn
               continue;
            if (_strnicmp(tok, "y:", 2))
            {
               fprintf(fLog, "y: expected at %ld\n", fMenLineCount);
               continue;
            }
            cur_boardname[cur_boardcnt].y = atof(&tok[2]);

            if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // ORIENTATION
               continue;
            if (STRCMPI(tok, "ORIENTATION"))                   
            {
               fprintf(fLog, "ORIENTATION expected at %ld\n",fMenLineCount);
               continue;
            }

            if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // rot
               continue;
            cur_boardname[cur_boardcnt].rot = atoi(tok);

            cur_boardcnt++;
         }
      }

      else if (!strcmp(tok, "B_UNITS"))
      {
         if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
            continue;

         DoUnits(tok);

         // here adjust board offset
         if (cur_boardcnt)
         {
            cur_boardname[cur_boardcnt-1].x *= scaleFactor;
            cur_boardname[cur_boardcnt-1].y *= scaleFactor;
         }
      }
      else if (!strcmp(tok, "COMP"))
      {
         if (cur_boardcnt)
            cur_boardname[cur_boardcnt-1].flag |= NEUT_COMP;   
      }
      else if (!strcmp(tok, "B_ATTR"))
      {
         if (cur_boardcnt)
            cur_boardname[cur_boardcnt-1].flag |= NEUT_B_ATTR; 
      }
      else if (!strcmp(tok, "GEOM"))
      {
         if (cur_boardcnt)
            cur_boardname[cur_boardcnt-1].flag |= NEUT_GEOM;

         if (!get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT))
            continue;

         NeutralGeom *neutGeom = new NeutralGeom;
         neutGeom->name = _strupr(tok);
         neutGeom->centerx = 0;
         neutGeom->centery = 0;
         neutGeom->centroidfound = FALSE;
         neutGeom->centroidx = 0;
         neutGeom->centroidy = 0;
         neutGeom->centroidangle = 0;
         neutGeom->boardnumber = cur_boardcnt-1;
         neutGeom->place_outline = 0;
         geomArray.SetAtGrow(geomArrayCount++, neutGeom);  
      }  
      else if (!strcmp(tok, "PAD"))
      {
         if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // via or pad
            continue;
         if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
            continue;

         NeutralPadstack *neutPadstack = new NeutralPadstack;
         neutPadstack->name = _strupr(tok);
         neutPadstack->pinaccess = 0;
         neutPadstack->padtype = -1;
         neutPadstack->layerlist = "";
         neutPadstack->from_topplace = "";
         neutPadstack->to_topplace = "";
         neutPadstack->from_bottomplace = "";
         neutPadstack->to_bottomplace = "";
		 neutPadstack->hasContent = 0; // added DF 2004.10.20, Initialize to 0, later is incremented when content found
         padstackArray.SetAtGrow(padstackArrayCount++, neutPadstack);  
      }  
	  // Begin, added DF 2004.10.20
	  // added a padshape count. if hasContent == 0 then padstack is empty
	  else if (!strcmp(tok, "P_SHAPE"))
	  {
         padstackArray[padstackArrayCount - 1]->hasContent++;
	  }
	  // End, added DF 2004.10.20
	  else if (!strcmp(tok, "G_ATTR"))
      {
         if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
            continue;

         if (!STRCMPI(tok, "COMPONENT_CENTER"))
         {
            // the coo is a string, not 2 tokens.
            if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
               continue;

            char *lp = strtok(tok, " \t");
				// this code was changed to fix case 958, code review by MAN, 8/18/04
				// lp was empty causing atof(lp) to cause a crash.  
				double x = lp?atof(lp):0;

            lp = strtok(NULL, "\n");
				double y = lp?atof(lp):0;
				// end case 958 changes.
            geomArray[geomArrayCount-1]->centerx = x;
            geomArray[geomArrayCount-1]->centery = y;
         }
      }  
   }

   if (line)
      free(line);
}

/******************************************************************************
* DoNeutral
*/
static void DoNeutral(CStdioFile *fMen)
{
   CString fMenLine;
   char *line = NULL;

   CFileReadProgress fileReadProgress(*fMen);
   CString fileReadStatus;
   fileReadStatus.Format("Reading Board '%s'",fMen->GetFilePath());
   fileReadProgress.updateStatus(fileReadStatus);

   while (GetNextLine(fMen, &fMenLine))
   {
      if ((fMenLineCount % 200) == 1)
      {
         fileReadProgress.updateProgress();
      }

      if (line)
         free(line);

      line = STRDUP(fMenLine);

      line = strtok(line, "\n");

		if (!line) continue;

      char tok[MAX_TOKEN], att[MAX_ATT];
      if (!get_nextword(line, tok, MAX_TOKEN, att, MAX_ATT))
         continue;

      if (tok[0] == '#')
         continue;

      int code = is_command(tok, command_lst, SIZ_OF_COMMAND);
      if (code < 0)
      {
         //fprintf(fLog, "Unknown MENTOR Token [%s] at %ld\n",tok, fMenLineCount);
         //display_error++;
      }
      else
      {
         (*command_lst[code].function)();
      }
   }

   if (line)
      free(line);

   DoPanelOutline();

   check_create();
   update_mirrorlayers();
   update_layerattr();
}

/******************************************************************************
* DoNetlist
*/
static void DoNetlist(CStdioFile *fMen)
{
   CString fMenLine;
   char *line = NULL;

   CFileReadProgress fileReadProgress(*fMen);
   CString fileReadStatus;
   fileReadStatus.Format("Reading Net List '%s'",fMen->GetFilePath());
   fileReadProgress.updateStatus(fileReadStatus);

   while (GetNextLine(fMen, &fMenLine))
   {
      if ((fMenLineCount % 200) == 1)
      {
         fileReadProgress.updateProgress();
      }

      if (line)
         free(line);

      line = STRDUP(fMenLine);

      line = strtok(line, "\n");

		if (!line) continue;

      char tok[MAX_TOKEN], att[MAX_ATT];
      if (get_nextword(line, tok, MAX_TOKEN, att, MAX_ATT) == 0)
         continue;

      if (tok[0] == '#')
         continue;

      int code = is_command(tok, netlist_lst, SIZ_OF_NETLIST);
      if (code < 0)
      {
         //fprintf(fLog,"Unknown MENTOR Token [%s] at %ld\n",tok, fMenLineCount);
         //display_error++;
      }
      else
      {
         (*netlist_lst[code].function)();
      }
   }

   if (line)
      free(line);
}

/******************************************************************************
* Do_UNNAMEDNET_Rename

   This function rename the net to "$$__comp.pin" for netname that start
   with the string specific in the command ".UNNAMEDNET" in neutral.in
*/
static void Do_UNNAMEDNET_Rename()
{
   POSITION pos = boardFile->getNetList().GetHeadPosition();
   while (pos)
   {
      NetStruct *net = boardFile->getNetList().GetNext(pos);

      if (!(net->getFlags() & NETFLAG_UNNAMED))
         continue;

      CompPinStruct *comppin = net->getHeadCompPin();
      CString netName;
      netName.Format("$$__%s.%s", comppin->getRefDes(), comppin->getPinName());
      net->setNetName(netName);
   }
}

/******************************************************************************
* DoPanelOutline
*/
static void DoPanelOutline()
{
   for (int i=0; i<panelArrayCount; i++)
   {
      NeutralPanel *panel = panelArray[i];

      if (!panel->HasPanelOutline)
         continue;
      
      
      doc->PrepareAddEntity(panel->panelFile);

      int layerNum = Graph_Level("PANEL_OUTLINE", "", 0);

      DataStruct *data = Graph_PolyStruct(layerNum, 0, FALSE);
      data->setGraphicClass(GR_CLASS_PANELOUTLINE);


      Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);

      Graph_Vertex(panel->xmin, panel->ymin, 0);
      Graph_Vertex(panel->xmax, panel->ymin, 0);
      Graph_Vertex(panel->xmax, panel->ymax, 0);
      Graph_Vertex(panel->xmin, panel->ymax, 0);
      Graph_Vertex(panel->xmin, panel->ymin, 0);
   }
}

/******************************************************************************
* neut_net
*/
static int neut_net()
{
   check_create();

   if (skipBoard)
      return 1;

   //char         tok[MAX_TOKEN], att[MAX_ATT];
   // if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;

   cur_name = GetNetname(curlp); // netname can contain spaces
   cur_name.TrimLeft();
   cur_name.TrimRight();

   cur_net = add_net(boardFile, cur_name);

   if (IsUnnamedNet(cur_name))
      cur_net->setFlagBits(NETFLAG_UNNAMED);

   return 1;
}

/******************************************************************************
* neut_comp
*/
static int neut_comp()
{
   skipComp = FALSE;

   cur_compplaced = FALSE;

   check_create();

   if (skipBoard)
      return 1;

   cur_create = CREATE_BOARD;
   sectionStatus |= NEUT_COMP;

   cur_data = NULL;

   char tok[MAX_TOKEN], att[MAX_ATT];
   if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
      return 0;
   CString compName = tok;
   compName.MakeUpper();

   CString prefix;
   int i=0;
   while (i<compName.GetLength() && isalpha(compName.GetAt(i)))
      prefix += compName.GetAt(i++);

   if (skipPrefixList.Find(prefix))
   {
      skipComp = TRUE;
      return 1;
   }

   cur_comp.refname = compName;

   if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
      return 0;
   CString partNumber = tok; // partnumber is updated as ATT_TYPELINK in update_partnumber !

   if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
      return 0;
   CString symbol = tok;

   if (!get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
      return 0;
   CString blockName = tok;
   blockName.MakeUpper();
   cur_comp.new_geomname = cur_comp.orig_geomname = blockName;

   // here can be not placed.
   double x = 0;
   double y = 0;
   double rot = 0;
   int mir = 0;
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) 
   {
      cur_compplaced = TRUE;
      x = cnv_tok(tok);
   }
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
      y = cnv_tok(tok);
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) 
      mir = (atoi(tok) > 1);
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) 
      rot = atof(tok);

   rot = normalizeDegrees((mir)?360 - rot:rot);
   
   if (cur_boardcnt)
   {
      x = x - cur_boardname[cur_boardcnt-1].x;
      y = y - cur_boardname[cur_boardcnt-1].y;
   }

   int gptr = get_geomlistptr(blockName);
   
   if (gptr < 0)
   {
      fprintf(fLog, "Geometry [%s] not found\n", blockName);
      display_error++;
      return 1;
   }

   cur_comp.new_geomname.Format("%s_%s", blockName, compName);
   
   fprintf(fLog,"Geometry [%s] Component [%s] has movable pins -> changed to [%s]\n", blockName, compName, cur_comp.new_geomname);
   display_error++;

   BlockStruct *b = Graph_Block_Exists(doc, blockName, neutralPcbFileNum);
   BlockStruct *b1 = Graph_Block_On(GBO_APPEND, cur_comp.new_geomname, neutralPcbFileNum, 0, blockTypePcbComponent);
   b1->setBlockType(blockTypePcbComponent);

   // loop through every pinnr and update the pinname
   Graph_Block_Copy(b, 0, 0, 0, 0, 1, -1, TRUE);
   Graph_Block_Off();

   doc->SetUnknownAttrib(&b1->getAttributesRef(), ATT_DERIVED, blockName, SA_OVERWRITE, NULL);

   blockName = cur_comp.new_geomname;
   pinEditedGeomMap.SetAt(blockName, blockName);

   
   // just make sure, it's there.
   BlockStruct *block = Graph_Block_On(GBO_APPEND, blockName, neutralPcbFileNum, 0);

   Graph_Block_Off();

   NeutralPartnumber *neutPartnumber = new NeutralPartnumber;
   neutPartnumber->compname = compName;
   neutPartnumber->partnumber = partNumber;
   neutPartnumber->pn_ext = "";
   neutPartnumber->bnum = block->getBlockNumber();
   partnumberArray.SetAtGrow(partnumberArrayCount++, neutPartnumber);  

   if (!cur_compplaced)
   {
      if (unplaced_components)
         fprintf(fLog,"Unplaced Component [%s] at %ld\n", compName, fMenLineCount);
      else
         fprintf(fLog,"Unplaced Component [%s] eliminated at %ld\n", compName, fMenLineCount);
      display_error++;     
   }

   if (!cur_compplaced && !unplaced_components) 
      return 1;

   cur_comp.x = x;
   cur_comp.y = y;
   cur_comp.rot = rot;
   cur_comp.mirror = mir;


   int index = GetGeomToFiducial(cur_comp.orig_geomname);
   if (index > -1)
   {
      if (geomToFidArray[index].bottom != 1)
         mir = geomToFidArray[index].bottom;    
      cur_data = Graph_Block_Reference(blockName, compName, neutralPcbFileNum, x, y, DegToRad(1.0 * rot), mir , 1.0, -1, TRUE);
      cur_data->getInsert()->setInsertType(insertTypeFiducial);
   }
   else
   {
      cur_data = Graph_Block_Reference(blockName, compName, neutralPcbFileNum, x, y, DegToRad(1.0 * rot), mir , 1.0, -1, TRUE);
      cur_data->getInsert()->setInsertType(insertTypePcbComponent);
   }

   if (!cur_compplaced)
      doc->SetAttrib(&cur_data->getAttributesRef(), doc->IsKeyWord(ATT_UNPLACED, TRUE), VT_NULL,NULL,SA_OVERWRITE, NULL);

   doc->SetAttrib(&cur_data->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, compName.GetBuffer(0), SA_OVERWRITE, NULL);
   if (!Import_PN_Device)
		doc->SetAttrib(&cur_data->getAttributesRef(),doc->RegisterKeyWord("Symbol", TRUE, VT_STRING), VT_STRING, symbol.GetBuffer(0), SA_OVERWRITE, NULL);
   else
		doc->SetAttrib(&cur_data->getAttributesRef(),doc->RegisterKeyWord("MENTOR_DEVICETYPE", TRUE, VT_STRING), VT_STRING, symbol.GetBuffer(0), SA_OVERWRITE, NULL);

   return 1;
}

/******************************************************************************
* neut_pad
*/
static int neut_pad()
{
   check_create();

   if (skipBoard)
      return 1;

   char tok[MAX_TOKEN], att[MAX_ATT];
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0; // via or pad5
   
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   CString padstackName = _strupr(tok); 
   
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0; // bur, thru, surf
   CString padstackType = tok;

   int padstackflag = 0;
   if (padstackType.CompareNoCase("Thru") == 0)
   {
      padtype = PADTYPE_THRU;
      padstackflag = BL_ACCESS_OUTER;
   }
   else if (padstackType.CompareNoCase("Blind") == 0)
   {
      padtype = PADTYPE_BLIND;
   }
   else if (padstackType.CompareNoCase("Surf") == 0)
   {
      padtype = PADTYPE_SURF;
      //padstackflag = BL_ACCESS_TOP;
   }
   else if (padstackType.CompareNoCase("Bur") == 0)
   {
      padtype = PADTYPE_BURIED;
   }
   else
   {
      fprintf(fLog, "Unknown padtype [%s] at %ld\n", padstackType, fMenLineCount);
      padtype = PADTYPE_THRU;
   }
   
   int padIndex = GetPadstackIndex(padstackName);
   NeutralPadstack *neutPadstack = NULL;
   if (padIndex > -1)
   {
      neutPadstack = padstackArray[padIndex];
      neutPadstack->padtype = padtype;
   }

   CString slay1, slay2, elay1, elay2;
   double drill = 0;

   switch (padtype)
   {
   case PADTYPE_SURF:
      {
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // none (drill)
         {
            if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // non mirror startlayer ?
            {
               slay1 = tok;
               if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // non mirror endlayer ?
               {
                  slay2 = tok;
                  if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // mirror startlayer ?
                  {
                     elay1 = tok;
                     if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT)) // mirror endlayer ?
                     {
                        elay2 = tok;
                        neutPadstack->from_topplace = slay1;
                        neutPadstack->to_topplace = slay2;
                        neutPadstack->from_bottomplace = elay1;
                        neutPadstack->to_bottomplace = elay2;
                     }
                  }
               }
            }
         }
      }
      break;

   case PADTYPE_BLIND:
      {
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
            return 0;   // none (drill)
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
            return 0;   // non mirror startlayer ?
         slay1 = tok;

         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
            return 0;   // non mirror endlayer ?
         slay2 = tok;

         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
            return 0;   // mirror startlayer ?
         elay1 = tok;

         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
            return 0;   // mirror endlayer ?
         elay2 = tok;

         neutPadstack->from_topplace = slay1;
         neutPadstack->to_topplace = slay2;
         neutPadstack->from_bottomplace = elay1;
         neutPadstack->to_bottomplace = elay2;
      }
      break;

   case PADTYPE_BURIED:
      {
         if (get_nextword(NULL, tok,MAX_TOKEN, att, MAX_ATT) == 0)
            return 0;   
         drill = cnv_tok(tok); 

         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))                   // non mirror startlayer ?
         {
            slay1 = tok;
            if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))                // non mirror endlayer ?
            {
               slay2 = tok;
               if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))             // mirror startlayer ?
               {
                  elay1 = tok;
                  if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))          // mirror endlayer ?
                  {
                     elay2 = tok;
                     neutPadstack->from_topplace = slay1;
                     neutPadstack->to_topplace = slay2;
                     neutPadstack->from_bottomplace = elay1;
                     neutPadstack->to_bottomplace = elay2;
                  }
               }
            }
         }
      }
      break;

   default:
      {
         if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
            return 0;
         drill = cnv_tok(tok); 
      }
      break;
   }

   cur_create = CREATE_PAD;
   sectionStatus |= NEUT_PAD;
   padlayercnt = 0;
   
   // just make sure, it's there.
   cur_block = Graph_Block_On(GBO_APPEND, padstackName, neutralPcbFileNum, 0, blockTypePadstack);
   cur_block->setBlockType(blockTypePadstack);
   cur_block->setFlags(padstackflag);

   if (padtype == PADTYPE_SURF)
      doc->SetAttrib(&cur_block->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);
   else if (padtype == PADTYPE_BLIND || padtype == PADTYPE_BURIED)
      doc->SetAttrib(&cur_block->getAttributesRef(), doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);

   if (drill > 0)
   {
      int drillLayerNum = Graph_Level("DRILLHOLE", "", 0);
      int drillIndex = get_drillindex("PLATED", drill, drillLayerNum, 0);
      CString drillName;
      drillName.Format("PLATED_%d", drillIndex);

      Graph_Block_Reference(drillName, NULL, neutralPcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, drillLayerNum, TRUE);
   }

   return 1;
}

/******************************************************************************
* neut_p_shape
*/
static int neut_p_shape()
{
   char tok[MAX_TOKEN], att[MAX_ATT];
   if (skipBoard)
      return 1;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
		return 0;
   CString padstackName = _strupr(tok);
   
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
		return 0;
   CString layerName = tok;
   layerName.MakeUpper();

   int padIndex = GetPadstackIndex(padstackName);
   NeutralPadstack *neutPadstack = NULL;
   if (padIndex > -1)
   {
      neutPadstack = padstackArray[padIndex];

      if (strlen(neutPadstack->layerlist))
         neutPadstack->layerlist += ",";
      neutPadstack->layerlist += layerName;
   }
   else
   {
      fprintf(fLog, "Syntax Error: PAD name [%s] not defined at %ld ->ignored!\n", padstackName, fMenLineCount);
      display_error++;
      return 1;
   }

   if (padtype == PADTYPE_SURF)
   {
      if (neutPadstack->from_topplace.CompareNoCase(layerName) == 0)
         layerName = SURF_TOP;
      else if (neutPadstack->from_bottomplace.CompareNoCase(layerName) == 0)
         layerName = SURF_BOT;
      else
      {
         fprintf(fLog, "Padlayer [%s] for Pad [%s] not recognized!\n", layerName, neutPadstack->name);
         display_error++;
      }
   }
   
   int layerNum = Graph_Level(layerName, "", 0);
   int layerIndex = UpdateLayer(layerName);

   padlayercnt++;

   if (padtype == PADTYPE_THRU)
   {
      thruPadLayerArray[padlayercnt-1] = layerName;
      if (thruPadLayerCount < padlayercnt)
         thruPadLayerCount = padlayercnt;

      // this was a thru pad. Update only if the stored layer is smaller.
      if (neutralLayerArray[layerIndex].padelectrical < padlayercnt)
         neutralLayerArray[layerIndex].padelectrical = padlayercnt;
   }

   CString padShapeName;
   double offsetX = 0, offsetY = 0;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)  
		return 0; // shape

   if (!strcmp(tok, "CIRCLE"))
   {
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
			return 0;
      double size = cnv_tok(tok);

      int padformIndex = get_padformindex(T_ROUND, size, 0);
      padShapeName.Format("PADSHAPE_%d", padformIndex);
   }
   else if (!strcmp(tok, "RECTANGLE"))
   {
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
			return 0;
      double width = cnv_tok(tok);

      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
			return 0;
      double height = cnv_tok(tok);

      // center
      if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)
      {
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)
            offsetX = cnv_tok(tok);
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)   
            offsetY = cnv_tok(tok);
      }

      int padFormIndex = get_padformindex(T_RECTANGLE, width, height);
      padShapeName.Format("PADSHAPE_%d", padFormIndex);
   }
   else
   {
      // unknown padform.
      fprintf(fLog, "Unknown Padform [%s]\n", tok);
      display_error++;
      return 1;
   }
   
   // add to pd
   Graph_Block_On(GBO_APPEND, padstackName, neutralPcbFileNum, 0, blockTypePadstack);
   DataStruct *data = Graph_Block_Reference(padShapeName, NULL, neutralPcbFileNum, offsetX, offsetY, 0.0, 0, 1.0, layerNum, TRUE);
   Graph_Block_Off();
   
   return 1;
}


/******************************************************************************
* neut_p_addp
*  - panel added part
*/
static int neut_p_addp()
{
   char tok[MAX_TOKEN], att[MAX_ATT];

   check_create();
   cur_create = CREATE_PANEL;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   CString panelName = tok;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   CString geomName = _strupr(tok);
   if (skipPADDPList.Find(geomName))
      return 1;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   CString refName = tok;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   double x = cnv_tok(tok);

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   double y = cnv_tok(tok);
   
   CString  t1, t2, t3, t4;
   // in older version there are only 3 tokens, in a newer 
   // version 4. 
   // it seems that in older version the rotation is missing
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // rotation
      t1 = tok;
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // ??
      t2 = tok;
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // ??
      t3 = tok;
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // mirror
      t4 = tok;

   int rot = 0;
   int mir = 0;
   if (!t4.IsEmpty())
   {
      rot = atoi(t1);
      if (atoi(t4) > 1)
         mir = TRUE;
   }
   else
   {
      cnv_addrot(atoi(t1), &rot, &mir);
   }

   int parttype = 0;
   if (is_attrib(geomName, FIDUCIAL_ATTR))
      parttype = FIDUCIAL_ATTR;
   else if (is_attrib(geomName, GENERIC_ATTR))
      parttype = GENERIC_ATTR;
   else if (is_attrib(geomName, TOOLING_ATTR))
      parttype = TOOLING_ATTR;
   else if (!geomName.CompareNoCase(panelMirrorBoardName))
      geomName = cur_boardname[0].name;
   
   // just make sure, it's there.
   if (Graph_Block_Exists(doc, geomName, neutralPcbFileNum) == NULL)
   {  
      // do the message only once per new type.
      // only do addp which are in the is_attrib list
      if (!parttype) 
      {
#ifdef _DEBUG
         fprintf(fLog,"DEBUG: P_ADDP [%s] not typed in neutral.in -> added as \"GENERIC COMPONENT\" at %ld\n",
               geomName, fMenLineCount);
         display_error++;
#endif
      }

      if (parttype == FIDUCIAL_ATTR)
      {
         BlockStruct *geom;
         geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);
         int laynr = Graph_Level("FIDUCIAL_1", "", 0);
         Graph_Level("FIDUCIAL_2", "", 0);
         Graph_Level_Mirror("FIDUCIAL_1", "FIDUCIAL_2", "");

         int err;
         Graph_Aperture(neutralPcbFileNum, "FID_APP", T_ROUND, 0.02, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
         Graph_Block_Reference("FID_APP", NULL, neutralPcbFileNum, 0, 0, 0.0, 0 , 1.0, laynr, TRUE);
         Graph_Block_Off();
         geom->setBlockType(blockTypeFiducial);
      }
      else if (parttype == TOOLING_ATTR)
      {
         BlockStruct *geom;

         // must be a PCBCOMPONENT
         // defined as a block with a padstack inserted as a INSERTTYPE_PIN
         // the padstack has a Graph_Tool inside.
         int layerNum;

         if (Graph_Block_Exists(doc, "TOOL_PIN", neutralPcbFileNum) == NULL)
         {
            layerNum = Graph_Level("TOOLING", "", 0);
            Graph_Tool("TOOL_DRILL", 0, 0.1, 0, 0, 0, 0L);

            geom = Graph_Block_On(GBO_APPEND, "TOOL_PIN", neutralPcbFileNum, 0);
            layerNum = Graph_Level("TOOLING", "", 0);
            Graph_Block_Reference("TOOL_DRILL", NULL, neutralPcbFileNum, 0, 0, 0.0, 0 , 1.0, layerNum, TRUE);
            Graph_Block_Off();
            geom->setBlockType(blockTypePadstack);
         }

         geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);
         layerNum = Graph_Level("TOOLING", "", 0);

         DataStruct *data = Graph_Block_Reference("TOOL_PIN", NULL, neutralPcbFileNum, 0, 0, 0.0, 0 , 1.0, layerNum, TRUE);
         data->getInsert()->setInsertType(insertTypePin);
         Graph_Block_Off();
         geom->setBlockType(blockTypeTooling);
      }
      else
      {
         CString attlevel = "";
         CString mirattlevel = "";
         attlevel.Format("P_ADDP(%s)_1", geomName);
         mirattlevel.Format("P_ADDP(%s)_2", geomName);

         int attLayerNum = Graph_Level(attlevel,"",0);
         int mirattLayerNum = Graph_Level(mirattlevel,"",0);

         LayerStruct *layer = doc->FindLayer(attLayerNum);
         layer->setVisible(false);
         layer = doc->FindLayer(mirattLayerNum);
         layer->setVisible(false);

         Graph_Level_Mirror(attlevel, mirattlevel, "");

         // create the geometry
         BlockStruct *geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);

         const double radius = 0.1;
         Graph_Circle(attLayerNum, 0, 0, radius, 0L, 0 , FALSE, FALSE); 

         double len = strlen(geomName) * 0.07;
         Graph_Text(attLayerNum, geomName, -len/2, -0.07/2 , 0.07, 0.05, 0.0, 0, FALSE, 0, 1, FALSE, -1, 0);

         Graph_Block_Off();
         geom->setBlockType(blockTypeGenericComponent);
      }
   }

   panelFile = NULL;
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *tempFile = doc->getFileList().GetNext(filePos);

      if (!tempFile->getName().CompareNoCase(panelName) && tempFile->getBlockType() == blockTypePanel)
      {
         panelFile = tempFile;
         break;
      }
   }

   if (!panelFile)
   {
      panelFile = Graph_File_Start(panelName, Type_Mentor_Neutral_Layout);
      panelFile->setBlockType(blockTypePanel);
      panelFile->getBlock()->setBlockType(blockTypePanel);
      panelFile->setShow(false); 

      NeutralPanel *panel = new NeutralPanel;
      panel->panelFile = panelFile;
      panel->HasPanelOutline = FALSE;
      panel->xmin = panel->ymin = DBL_MAX;
      panel->xmax = panel->ymax = -DBL_MAX;
      panelArray.SetAtGrow(panelArrayCount++, panel);
   }

   // panel outline
   if (!geomName.CompareNoCase("N_UMR"))
   {
      for (int i=0; i<panelArrayCount; i++)
      {
         NeutralPanel *panel = panelArray[i];

         if (panel->panelFile != panelFile)
            continue;

         panel->HasPanelOutline = TRUE;

         if (x < panel->xmin)
            panel->xmin = x;
         if (x > panel->xmax)
            panel->xmax = x;
         if (y < panel->ymin)
            panel->ymin = y;
         if (y > panel->ymax)
            panel->ymax = y;
      }
   }

   doc->PrepareAddEntity(panelFile);

   // Christ Czernel said to check if geomName is XOUT first.
   // If not then check if it is Fiducial afterward
   int index = GetXOutGeom(geomName);
   if (index > -1 )
   {
      if (xOutGeomArray[index].bottom != -1)
         mir = xOutGeomArray[index].bottom;     
      DataStruct *data = Graph_Block_Reference(geomName, refName, neutralPcbFileNum, x, y, DegToRad(1.0 * rot), mir , 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypeXout);
   }
   else
   {
      index = GetGeomToFiducial(geomName);
      if (index > -1 )
      {
         if (geomToFidArray[index].bottom != -1)
            mir = geomToFidArray[index].bottom;    
         DataStruct *data = Graph_Block_Reference(geomName, refName, neutralPcbFileNum, x, y, DegToRad(1.0 * rot), mir , 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypeFiducial);
      }
      else
      {
         DataStruct *data = Graph_Block_Reference(geomName, refName, neutralPcbFileNum, x, y, DegToRad(1.0 * rot), mir , 1.0, -1, TRUE);

         for (int i=0; i<tot_boardcnt; i++)
         {
            if (cur_boardname[i].skip_board)
               continue;
            
            if (!strcmp(geomName, cur_boardname[i].name))
            {
					double newRot = 0;
					if (mir)
						newRot = rot - (360 - cur_boardname[i].rot);
					else
						newRot = rot - cur_boardname[i].rot;

					while (newRot >= 360)
						newRot -= 360;
					while (newRot < 0)
						newRot += 360;

               data->getInsert()->setAngle(DegToRad(newRot));
               data->getInsert()->setInsertType(insertTypePcb);
               break;
            }
         }
      }
   }

   doc->PrepareAddEntity(boardFile);

   return 1;
}

/******************************************************************************
* neut_b_addp
*  - board add part
*     These parts are not defined in the Neutral file. Therefore I need to check if there is a GEOM define, if not make one.
*/
static int neut_b_addp()
{
   char     tok[MAX_TOKEN], att[MAX_ATT];
   int      rot = 0;
   int      mir = 0;
   int      parttype = 0;

   check_create();
   if (skipBoard)
      return 1;

   cur_create = CREATE_BOARD;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   CString geomName = _strupr(tok);
   if (skipBADDPList.Find(geomName))
      return 1;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   CString refName = _strupr(tok);

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   double x = cnv_tok(tok);

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   double y = cnv_tok(tok);

   if (cur_boardcnt)
   {
      x = x - cur_boardname[cur_boardcnt-1].x;
      y = y - cur_boardname[cur_boardcnt-1].y;
   }

   CString t1,t2,t3,t4 = "";
   // in older version there are only 3 tokens, in a newer 
   // version 4. 
   // it seems that in older version the rotation is missing
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // rotation
      t1 = tok;
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // ??
      t2 = tok;
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // ??
      t3 = tok;
   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) != 0)     // mirror
      t4 = tok;

   if (strlen(t4))
   {
      rot = atoi(t1);
      if (atoi(t4) > 1)
         mir = TRUE;
   }
   else
   {
      cnv_addrot(atoi(t1), &rot, &mir);
   }

   if (is_attrib(geomName, FIDUCIAL_ATTR))
      parttype = FIDUCIAL_ATTR;
   else if (is_attrib(geomName, GENERIC_ATTR))
      parttype = GENERIC_ATTR;
   else if (is_attrib(geomName, TOOLING_ATTR))
      parttype = TOOLING_ATTR;
   else
      parttype = 0;

   // here is some more info on mirror, rotate etc...
   
   // just make sure, it's there.
   if (!Graph_Block_Exists(doc, geomName, neutralPcbFileNum))
   {
      // do the message only once per type.
      // only do addp which are in the is_attrib list
      if (!parttype) 
      {
#ifdef _DEBUG
         fprintf(fLog, "DEBUG: B_ADDP [%s] not typed in neutral.in -> added as \"GENERIC COMPONENT\" at %ld\n",
               geomName, fMenLineCount);
         display_error++;
#endif
      }
      
      if (parttype == FIDUCIAL_ATTR)
      {
         BlockStruct *geom;
         geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);
         geom->setBlockType(blockTypeFiducial);

         int laynr = Graph_Level("FIDUCIAL_1", "", 0);
         Graph_Level("FIDUCIAL_2", "", 0);
         Graph_Level_Mirror("FIDUCIAL_1", "FIDUCIAL_2", "");

         int err;
         Graph_Aperture(neutralPcbFileNum, "FID_APP", T_ROUND, 0.02, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
         Graph_Block_Reference("FID_APP", NULL, neutralPcbFileNum, 0, 0, 0.0, 0, 1.0, laynr, TRUE);
         Graph_Block_Off();
      }
      else if (parttype == TOOLING_ATTR)
      {
         BlockStruct *geom;

         // must be a PCBCOMPONENT
         // defined as a block with a padstack inserted as a INSERTTYPE_PIN
         // the padstack has a Graph_Tool inside.
         int layerNum;

         if (Graph_Block_Exists(doc, "TOOL_PIN", neutralPcbFileNum) == NULL)
         {
            layerNum = Graph_Level("TOOLING", "", 0);
            Graph_Tool("TOOL_DRILL", 0, 0.1, 0, 0, 0, 0L);

            geom = Graph_Block_On(GBO_APPEND, "TOOL_PIN", neutralPcbFileNum, 0);
            geom->setBlockType(blockTypePadstack);

            layerNum = Graph_Level("TOOLING", "", 0);
            Graph_Block_Reference("TOOL_DRILL", NULL, neutralPcbFileNum, 0, 0, 0.0, 0 , 1.0, layerNum, TRUE);
            Graph_Block_Off();
         }

         geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);
         geom->setBlockType(blockTypeTooling);

         layerNum = Graph_Level("TOOLING", "", 0);

         DataStruct *data = Graph_Block_Reference("TOOL_PIN", NULL, neutralPcbFileNum, 0, 0, 0.0, 0 , 1.0, layerNum, TRUE);
         data->getInsert()->setInsertType(insertTypePin);
         Graph_Block_Off();
      }
      else
      {
         BlockStruct *geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);
         geom->setBlockType(blockTypeGenericComponent);

         CString attrLevel;
         attrLevel.Format("B_ADDP(%s)_1", geomName);

         int layerNum = Graph_Level(attrLevel, "", 0);
         LayerStruct *layer = doc->FindLayer(layerNum);
         layer->setVisible(false);
         Graph_Circle(layerNum, 0, 0, 0.1 * scaleFactor, 0L, 0 , FALSE, FALSE); 

         double len = strlen(geomName) * 0.07 * scaleFactor;
         Graph_Text(layerNum, geomName, -len/2, -0.07/2 * scaleFactor, 0.07 * scaleFactor, 0.05 * scaleFactor, 0.0,
               0, FALSE, 0, 1, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg

         Graph_Block_Off();

         CString mirrAttrLevel;
         mirrAttrLevel.Format("B_ADDP(%s)_2", geomName);
         int mirrLayerNum = Graph_Level(mirrAttrLevel, "", 0);
         LayerStruct *mirrLayer = doc->FindLayer(mirrLayerNum);
         mirrLayer->setVisible(false);

         Graph_Level_Mirror(attrLevel, mirrAttrLevel, "");
      }
   }

   // Christ Czernel said to check if geomName is XOUT first.
   // If not then check if it is Fiducial afterward
   int index = GetXOutGeom(geomName);
   if (index > -1)
   {
      if (xOutGeomArray[index].bottom != -1)
         mir = xOutGeomArray[index].bottom;     
      DataStruct *data = Graph_Block_Reference(geomName, refName, neutralPcbFileNum, x, y, DegToRad(1.0 * rot), mir , 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypeXout);
   }
   else
   {
      index = GetGeomToFiducial(geomName);
      if (index > -1)
      {
         DataStruct *data = Graph_Block_Reference(geomName, refName, neutralPcbFileNum, x, y, DegToRad(1.0 * rot), mir , 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypeFiducial);
      }
      else
      {
         DataStruct *data = Graph_Block_Reference(geomName, NULL, neutralPcbFileNum, x, y, DegToRad(rot), mir, 1, -1, TRUE);
         if (parttype == XOUTGEOM_ATTR)
            data->getInsert()->setInsertType(insertTypeXout);
         else if (parttype == FIDUCIAL_ATTR)
            data->getInsert()->setInsertType(insertTypeFiducial);
         else if (parttype == TOOLING_ATTR)
            data->getInsert()->setInsertType(insertTypeDrillTool);
      }
   }


   return 1;
}

/******************************************************************************
* neut_hole
*     These parts are not defined in the Neutral file. Therefore I need to check if there is a GEOM define, if not make one.
*/
static int neut_hole()
{
   char     tok[MAX_TOKEN],att[MAX_ATT];
   CString  plated, bname, drillname;
   double   x,y;
   double   diam = 0.1;

   // a hole can be a PANEL hole or a BOARD hole
   check_create();

   if (skipBoard) return 1;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // plated NPTH or PTH
   plated = tok;
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   x = cnv_tok(tok);
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   y = cnv_tok(tok);
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   diam = cnv_tok(tok);
   bname.Format("HOLE_%s",tok);


   // if more than 1 board is done, the holes are paert of the panel.
   if (cur_boardcnt && cur_create == CREATE_BOARD)
   {
      x = x - cur_boardname[cur_boardcnt-1].x;
      y = y - cur_boardname[cur_boardcnt-1].y;
   }


   int drilllayernum = Graph_Level("DRILLHOLE","",0);

   if (plated.CompareNoCase("PTH") == 0)
      drillname.Format("%s_%d","PLATED", get_drillindex("PLATED", diam, drilllayernum, 0));
   else
      drillname.Format("%s_%d","UNPLATED", get_drillindex("UNPLATED", diam, drilllayernum, 1));

   if (panelFile != NULL && cur_create == CREATE_PANEL)
   {
		// Case #1523 - Modification was made for this case to place holes on panel 
		// when hole definitions follow immediately after a P_ADDP and there is a panel

      doc->PrepareAddEntity(panelFile);
      DataStruct *data = Graph_Block_Reference(drillname, NULL, neutralPcbFileNum, x, y, 0.0, 0 , 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypeDrillHole);

      doc->PrepareAddEntity(boardFile);
   }
   else
   {
      DataStruct *data = Graph_Block_Reference(drillname, NULL, neutralPcbFileNum, x, y, 0.0, 0 , 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypeDrillHole);
   }

   return 1;
}


/******************************************************************************
* get_b_attr_graphicclass
*/
// type and usage of this should be changed to GraphicClassTag

static int get_b_attr_graphicclass(CString attribName)
{
   for (int i=0; i<boardOutlineNameArrayCount; i++)
   {
      if (!boardOutlineNameArray[i].CompareNoCase(attribName))
         return GR_CLASS_BOARDOUTLINE;
   }

   return graphicClassNormal;
}

/****************************************************************************/
/*
*/
static const char *get_attribmap(const char *c)
{
   int   i;

   for (i=0;i<attribmapcnt;i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return   attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   return c;
}

/******************************************************************************
* update_attr
*/
static int update_attr(const char *name, int type)
{
   if (attrArrayCount < MAX_ATTR)
   {
      if ((attrArray[attrArrayCount].name = STRDUP(name)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      attrArray[attrArrayCount].atttype = type;
      attrArrayCount++;
   }

   return attrArrayCount-1;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0;i<MAX_LAYTYPE;i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return   LAYTYPE_UNKNOWN; 
//}

/******************************************************************************
* get_lay_index
*/
static int get_lay_index(const char *l)
{
   for (int i=0; i<layer_attr_cnt; i++)
   {
      if (!STRCMPI(l,layer_attr[i].name))
      {
         return i;
      }
   }

   if (layer_attr_cnt < MAX_LAYERS)
   {
      layer_attr[layer_attr_cnt].name = l;
      layer_attr[layer_attr_cnt].attr = 0;
      layer_attr_cnt++;
   }
   else
   {
      fprintf(fLog, "Too many layers\n");
      return -1;
   }

   return layer_attr_cnt-1;
}

/******************************************************************************
* LoadSettings
*/
static void LoadSettings(const CString settingsFile, CGeometryConsolidatorControl &consolControl)
{
	boardOutlineNameArrayCount = 0;
	geomToFidCount = 0;  // reset the geomToFidArray
	unnamedNetCount = 0;
	xOutGeomCount = 0;
	unplaced_components = TRUE;
	hierachical_netnames = FALSE;
	fixBoardOutline = FALSE;
	centroid_attr = "";
	Import_PN_Device = FALSE;   // case 1934
	centroid_angle= "";
	panelMirrorBoardName = "";
	IgnoreEmptyPins = TRUE; // added DF 2004.10.20
   consolControl.SetIsEnabled(true); // Reset to defaul=enabled

   FILE *fSet = fopen(settingsFile, "rt");
   if (!fSet)
   {
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      ErrorMessage(tmp, "Mentor Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   char line[255];
   while (fgets(line, 255, fSet))
   {
      char *string = get_string(line, " \t\n");

      if (!string)
         continue;

      if (string[0] != '.')
         continue;

      if (!STRICMP(string, ".HIERACHICAL_NETNAMES"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (toupper(string[0]) == 'Y')
            hierachical_netnames = TRUE;
         if (toupper(string[0]) == 'N')
            hierachical_netnames = FALSE;
      }
      else if (!STRICMP(string, ".UNPLACED_COMPONENT"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (toupper(string[0]) == 'N')
            unplaced_components = FALSE;
      }                                 
      else if (!STRICMP(string, ".CENTROID_ATTR"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue; 
         centroid_attr = string;
         centroid_attr.TrimLeft();
         centroid_attr.TrimRight();
      }
      else if (!STRICMP(string, ".CENTROID_ANGLE"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue; 
         centroid_angle = string;
         centroid_angle.TrimLeft();
         centroid_angle.TrimRight();
      }
      else if (consolControl.Parse(line))
      {
         // Nothing more to do, Parse() already did it if it was a Consolidator command.
      }
      else if (!STRICMP(string, ".UNNAMEDNET"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue; 
         CString c = string;
         c.TrimLeft();
         c.TrimRight();
         if (unnamedNetCount < 30)
         {
            unnamedNetArray.SetAtGrow(unnamedNetCount++, c);
         }
         else
         {
            fprintf(fLog, "Too many unnamed nets defined.\n");
            display_error++;
         }
      }
      else if (!STRICMP(string, ".ATTRIBMAP"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString a1 = string;
         a1.MakeUpper();
         a1.TrimLeft();
         a1.TrimRight();

         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString a2 = string;
         a2.MakeUpper();
         a2.TrimLeft();
         a2.TrimRight();

         NeutralAttribmap  *c = new NeutralAttribmap;
         attribmaparray.SetAtGrow(attribmapcnt,c);

         c->attrib = a1;
         c->mapattrib = a2;
         attribmapcnt++;
         
      }
      else if (!STRICMP(string, ".BOARDOUTLINE"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         boardOutlineNameArray.SetAtGrow(boardOutlineNameArrayCount++, string);
      }
      else if (!STRICMP(string, ".FIDUCIALATTR"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         update_attr(string, FIDUCIAL_ATTR);
      }
      else if (!STRICMP(string, ".PADATTR"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString padname = string;

         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (toupper(string[0]) == 'I')
            update_attr(padname, PADIGNORE_ATTR);
         else if (toupper(string[0]) == 'F')
            update_attr(padname, PADFIDUCIAL_ATTR);
      }
      else if (!STRICMP(string, ".TOOLINGATTR"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         update_attr(string, TOOLING_ATTR);
      }
      else if (!STRICMP(string, ".GENERICATTR"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         update_attr(string, GENERIC_ATTR);
      }
      else if (!STRICMP(string, ".XOUTGEOM"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         update_attr(string, XOUTGEOM_ATTR);
         CString geomName = _strupr(string);
         BOOL compGeom = TRUE;

         double diameter = 1.0;
         if ((string = get_string(NULL, " \t\n")) != NULL)
            diameter = atof(string);

         if ((string = get_string(NULL, " \t\n")) != NULL)
         {
            CString tmpUnit = string;
            double scale;
            if (!tmpUnit.CompareNoCase("MM"))
               scale = Units_Factor(UNIT_MM, pageUnits);
            else
               scale = Units_Factor(UNIT_INCHES, pageUnits);
            diameter = diameter * scale;
            compGeom = FALSE;
         }

         int bottom = -1; // no surface value, this apply to component and board added parts
         if ((string = get_string(NULL, " \t\n")) != NULL)
         {
            CString surface = string;
            if (!surface.CompareNoCase("BOT"))
               bottom = 1; // BOTTOM
            else
               bottom = 0; // TOP
            compGeom = FALSE;
         }
         if (!compGeom)
         {
            // Create the geometry
            BlockStruct *geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);
            geom->setBlockType(blockTypeXOut);

            int laynr = Graph_Level("XOUTGEOM_1", "", 0);
            Graph_Level("XOUTGEOM_1", "", 0);
            Graph_Level_Mirror("XOUTGEOM_1", "XOUTGEOM_2", "");

            int err;
            CString apertureName = "";
            apertureName.Format("%s_%d_APT", geomName, diameter);
            Graph_Aperture(neutralPcbFileNum, apertureName, T_ROUND, diameter, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
            Graph_Block_Reference(apertureName, NULL, neutralPcbFileNum, 0, 0, 0.0, 0, 1.0, laynr, TRUE);
            Graph_Block_Off();
         }
         xOutGeomArray[xOutGeomCount].geomName = geomName;
         xOutGeomArray[xOutGeomCount].bottom = bottom;         
         xOutGeomCount++;
      }
      else if (!STRICMP(string, ".GEOMTOFIDUCIAL"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString geomName = _strupr(string);
         BOOL compGeom = TRUE;

         double diameter = 1.0;
         if ((string = get_string(NULL, " \t\n")) != NULL)
            diameter = atof(string);

         if ((string = get_string(NULL, " \t\n")) != NULL)
         {
            CString tmpUnit = string;
            double scale;
            if (!tmpUnit.CompareNoCase("MM"))
               scale = Units_Factor(UNIT_MM, pageUnits);
            else
               scale = Units_Factor(UNIT_INCHES, pageUnits);
            diameter = diameter * scale;
            compGeom = FALSE;
         }

         int bottom = -1; // no surface value, this apply to component and board added parts
         if ((string = get_string(NULL, " \t\n")) != NULL)
         {
            CString surface = string;
            if (!surface.CompareNoCase("BOT"))
               bottom = 1; // BOTTOM
            else
               bottom = 0; // TOP
            compGeom = FALSE;
         }

         if (!compGeom)
         {
            // Create the geometry
            BlockStruct *geom = Graph_Block_On(GBO_APPEND, geomName, neutralPcbFileNum, 0);
            geom->setBlockType(blockTypeFiducial);

            int laynr = Graph_Level("GEOMTOFIDUCIAL_1", "", 0);
            Graph_Level("GEOMTOFIDUCIAL_1", "", 0);
            Graph_Level_Mirror("GEOMTOFIDUCIAL_1", "GEOMTOFIDUCIAL_2", "");

            int err;
            CString apertureName = "";
            apertureName.Format("%s_%d_APT", geomName, diameter);
            Graph_Aperture(neutralPcbFileNum, apertureName, T_ROUND, diameter, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
            Graph_Block_Reference(apertureName, NULL, neutralPcbFileNum, 0, 0, 0.0, 0, 1.0, laynr, TRUE);
            Graph_Block_Off();
         }

         geomToFidArray[geomToFidCount].geomName = geomName;
         geomToFidArray[geomToFidCount].bottom = bottom;       
         geomToFidCount++;
      }
      else if (!STRICMP(string, ".LAYERATTR"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue; 
         CString tllayer = _strupr(string);
         tllayer.TrimLeft();
         tllayer.TrimRight();

         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue; 
         CString cclayer = string;
         cclayer.TrimLeft();
         cclayer.TrimRight();

         CString tmp1 = "";
         CString tmp2 = "";
         BOOL isTop = FALSE;
         if (!IsGenericTopBottomMappingLayer(tllayer, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(tllayer, tmp1, isTop) &&
             !IsGenericTopBottomMappingLayer(cclayer, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(cclayer, tmp1, isTop))
         {
            int laytype = stringToLayerTypeTag(cclayer);
            int layindex = get_lay_index(tllayer);
            layer_attr[layindex].attr = laytype;
         }
         else if ((tllayer.CompareNoCase("PLACE_1") == 0) || (tllayer.CompareNoCase("PLACE_2") == 0))
         {
            int laytype = stringToLayerTypeTag(cclayer);
            int layindex = get_lay_index(tllayer);
            layer_attr[layindex].attr = laytype;
         }
      }
      else if (!STRICMP(string, ".MIRRORLAYER"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString layerName1 = _strupr(string);

         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString layerName2 = _strupr(string);
         
			CString tmp1 = "";
			CString tmp2 = "";
			BOOL isTop = FALSE;
			if (!IsGenericTopBottomMappingLayer(layerName1, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(layerName1, tmp1, isTop) &&
				 !IsGenericTopBottomMappingLayer(layerName2, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(layerName2, tmp1, isTop))
			{
				int layerIndex1 = UpdateLayer(layerName1);
				int layerIndex2 = UpdateLayer(layerName2);

				neutralLayerArray[layerIndex1].mirrorLayerNum = layerIndex2;
				neutralLayerArray[layerIndex2].mirrorLayerNum = layerIndex1;
			}
      }
      else if (!STRICMP(string, ".SKIP"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;

         if (!STRICMP(string, "REFDESPREFIX"))
         {
            if ((string = get_string(NULL, " \t\n")) == NULL)
               continue;
            skipPrefixList.AddTail(_strupr(string));
         }
         else if (!STRICMP(string, "B_ADDP"))
         {
            if ((string = get_string(NULL, " \t\n")) == NULL)
               continue;            
            skipBADDPList.AddTail(_strupr(string));
         }
         else if (!STRICMP(string, "P_ADDP"))
         {
            if ((string = get_string(NULL, " \t\n")) == NULL)
               continue;
            skipPADDPList.AddTail(_strupr(string));
         }
      }
      else if (!STRICMP(string, ".PANEL_MIRRORED_BOARD_NAME"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         panelMirrorBoardName = (CString)string;
         
      }
	  else if (!STRICMP(string, ".FIX_BOARD_OUTLINE"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (toupper(string[0]) == 'Y')
            fixBoardOutline = TRUE;
      }                                 
   	  // Begin, added DF 2004.10.20
	  // New IN file command to Ignore empty pins, default = TRUE
	  else if (!STRICMP(string, ".IGNORE_EMPTY_PINS"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (toupper(string[0]) == 'N')
            IgnoreEmptyPins = FALSE;
      }    

	  else if (!STRICMP(string, ".IMPORT_PARTNUMBER_DEVICETYPE"))
      {
         if ((string = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (string[0] == '2')
            Import_PN_Device = TRUE;
      }    
	  // End, added DF 2004.10.20
   }

   fclose(fSet);
}

/****************************************************************************/
/*
*/
static void clean_prosa(char *p)
{
   if (strlen(p) == 0)  return;

   // delete all traling isspace
   while (strlen(p) && isspace(p[strlen(p)-1])) 
         p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')   p[strlen(p)-1] = '\0';

   STRREV(p);
   while (strlen(p) && isspace(p[strlen(p)-1])) 
         p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')   p[strlen(p)-1] = '\0';
   STRREV(p);
   return;
}

/******************************************************************************
* write_poly
*/
static DataStruct *write_poly(int layerIndex, double width, int fill, DbFlag flg, int classtype, double xoff, double yoff, BOOL removePoly)
{
   int i;
   int widthindex = 0;
   int err;

   if (polycnt < 2)  
   {
      polyarray.RemoveAll();
      polycnt = 0;
      return NULL;
   }

   if (width == 0)
      widthindex = 0;   // make it small width.
   else
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   NeutralPoly p1 = polyarray.ElementAt(0);
   NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

   DataStruct *data = Graph_PolyStruct(layerIndex,0,FALSE);
   data->setGraphicClass(classtype);
   int close = (p1.x == p2.x && p1.y == p2.y);
   Graph_Poly(NULL, widthindex, fill,0, close);

	if (classtype == GR_CLASS_BOARDOUTLINE && fixBoardOutline == TRUE)
	{
		if (close && polycnt == 5)
		{
			NeutralPoly nextToLastPoly = polyarray.ElementAt(polycnt-3);
			NeutralPoly lastPoly = polyarray.ElementAt(polycnt-2);

			// now swap the two poly
			polyarray.SetAt(polycnt-3, lastPoly);
			polyarray.SetAt(polycnt-2, nextToLastPoly);
		}
		else if (polycnt == 4)
		{
			NeutralPoly nextToLastPoly = polyarray.ElementAt(polycnt-2);
			NeutralPoly lastPoly = polyarray.ElementAt(polycnt-1);

			// now swap the two poly
			polyarray.SetAt(polycnt-2, lastPoly);
			polyarray.SetAt(polycnt-1, nextToLastPoly);
		}
	}

   for (i=0;i<polycnt;i++)
   {
      NeutralPoly p = polyarray.ElementAt(i);
      Graph_Vertex(p.x-xoff, p.y-yoff, 0.0);
   }

	if (removePoly)
	{
		polyarray.RemoveAll();
		polycnt = 0;
	}

   return data;
}

/******************************************************************************
* update_partnumber
*/
static void update_partnumber(BlockStruct *b)
{
   int   i;
   CString  pn;

   COperationProgress operationProgress(partnumberArrayCount);
   operationProgress.updateStatus("Updating Partnumbers");
   int updateInterval = partnumberArrayCount/100;

   if (updateInterval < 1) updateInterval = 1;

   for (i=0;i<partnumberArrayCount;i++)
   {
      if ((i % updateInterval) == 0)
      {
         operationProgress.updateProgress(i);
      }

      pn = partnumberArray[i]->partnumber;
      pn += partnumberArray[i]->pn_ext;

      TypeStruct *t = AddType(boardFile, pn);
      t->setBlockNumber( partnumberArray[i]->bnum);

      DataStruct *d = datastruct_from_refdes(doc, b, partnumberArray[i]->compname);

      if (d)
      {
		  if (!Import_PN_Device)
		  {	doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TYPELISTLINK, TRUE),
            VT_STRING,pn.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
		  }
		  else   // put both the partnumber attribute and the $$device$$
		  {	doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TYPELISTLINK, TRUE),
            VT_STRING,pn.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height

			doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_PARTNUMBER, TRUE),
            VT_STRING,pn.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
		  }
      }
   }
}

/******************************************************************************
* update_centroid
*/
static void update_centroid(CCEtoODBDoc *Doc)
{
   for (int i=0; i<geomArrayCount; i++)
   {
      NeutralGeom *neutGeom = geomArray[i];

      if (!neutGeom->centroidfound)
         continue;

      BlockStruct *centroid = Doc->GetCentroidGeometry();

      Graph_Block_On(GBO_APPEND, neutGeom->name, neutralPcbFileNum, 0);

      DataStruct *data = Graph_Block_Reference(centroid->getName(), NULL, neutralPcbFileNum, neutGeom->centroidx, neutGeom->centroidy,
            DegToRad(neutGeom->centroidangle), 0, 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypeCentroid);

      Graph_Block_Off();
   }
}

/******************************************************************************
* update_padaccesslayer
*/
static int update_padaccesslayer(const char *top, const char *bot)
{
   if (!strlen(top) && !strlen(bot))
   {
      CString tmp = "Could not determine Pad Layers. Update NEUTRAL.IN with .LAYERATTR and .MIRRORLAYER.";
      fprintf(fLog ,"%s\n", tmp);

      for (int i=0; i<padstackArrayCount; i++)
      {
         NeutralPadstack *neutPadstack = padstackArray[i];
         if (neutPadstack->padtype < 0)
            continue;

         if (neutPadstack->layerlist)
            fprintf(fLog, "Pad [%s] -> Layer [%s]\n", neutPadstack->name, neutPadstack->layerlist);
      }

      display_error++;
      ErrorMessage(tmp, "Pad Layer Setup needed.");
      return 0;
   }

   return 1;
}

/******************************************************************************
* init_mem
*/
static void init_mem()
{
   // all units are normalized to INCHES

   polyarray.SetSize(100, 100);
   polycnt = 0;

   attribmaparray.SetSize(100, 100);
   attribmapcnt = 0;

   memset(neutralLayerArray, 0, sizeof(neutralLayerArray));
   neutralLayerArrayCount = 0;

   memset(attrArray, 0, sizeof(attrArray));
   attrArrayCount = 0;

   for (int i=0; i<MAX_LAYERS; i++)
      thruPadLayerArray[i] = "";

   geomArray.SetSize(100, 100);
   geomArrayCount = 0;

   padstackArray.SetSize(100, 00);
   padstackArrayCount = 0;

   partnumberArray.SetSize(100, 100);
   partnumberArrayCount = 0;

   drillarray.SetSize(100, 100);
   drillcnt = 0;

   panelArray.SetSize(10, 10);
   panelArrayCount = 0;

   unnamedNetArray.SetSize(10, 10);
   unnamedNetCount = 0;

	mentorLayersMap.RemoveAll();
	layerToDataMap.RemoveAll();

	neutralPcbFileNum = -1;
	boardstationPcbFileNum = -1;
	pinEditedGeomMap.RemoveAll();

   TestPinList.empty();
   TestProbeDataList.empty();
}

/******************************************************************************
* deinit_mem
*/
static void deinit_mem()
{
   int i;

   for (i=0; i<geomArrayCount; i++)
      delete geomArray[i];  
   geomArray.RemoveAll();
   geomArrayCount = 0;

   for (i=0; i<padstackArrayCount; i++)
      delete padstackArray[i];  
   padstackArray.RemoveAll();
   padstackArrayCount = 0;

   for (i=0; i<partnumberArrayCount; i++)
      delete partnumberArray[i];  
   partnumberArray.RemoveAll();
   partnumberArrayCount = 0;

   drillarray.RemoveAll();

   polyarray.RemoveAll();

   for (i=0; i<attribmapcnt; i++)
      delete attribmaparray[i];
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   for (i=0; i<neutralLayerArrayCount; i++)
      free(neutralLayerArray[i].name);

   for (i=0; i<attrArrayCount; i++)
      free(attrArray[i].name);

   for (i=0; i<panelArrayCount; i++)
      delete panelArray[i];  
   panelArray.RemoveAll();
   panelArrayCount = 0;

   unnamedNetArray.RemoveAll();
   unnamedNetCount = 0;

	mentorLayersMap.RemoveAll();
	layerToDataMap.RemoveAll();

	neutralPcbFileNum = -1;
	boardstationPcbFileNum = -1;
	pinEditedGeomMap.RemoveAll();

   TestPinList.empty();
   TestProbeDataList.empty();
}

/******************************************************************************
* set_skipBoard
*/
static void set_skipBoard()
{
   // first skip all boards with flag == 0
	int i=0; 
   for (i=0; i<cur_boardcnt; i++)
   {
      if (cur_boardname[i].flag == 0)
         cur_boardname[i].skip_board = TRUE;
      else
         cur_boardname[i].skip_board = FALSE;
   }

   // now skip all boards with the same name
   for (i=0; i<cur_boardcnt; i++)
   {
      if (cur_boardname[i].skip_board)
         continue;

      for (int ii=i+1; ii<cur_boardcnt; ii++)
      {
         if (!strcmp(cur_boardname[ii].name, cur_boardname[i].name))
            cur_boardname[ii].skip_board = TRUE;
      }
   }
}

/****************************************************************************/
/*
*/
static DbFlag  get_textjust(char *l)
{
   DbFlag j = 0;

   while (*l && (isspace(*l) || (*l) == '@'))
         *l++;

   if (strlen(l) < 2)   
      return 0;
   
   // length justification
   switch (toupper(l[1]))
   {
      case  'L':
         j = GRTEXT_W_L;
      break;
      case  'C':
        j = GRTEXT_W_C;
      break;
      case  'R':
        j = GRTEXT_W_R;
      break;
   }
   // next is height 
   switch (toupper(l[0]))
   {
      case  'B':
         j |= GRTEXT_H_B;
      break;
      case  'C':
         j |= GRTEXT_H_C;
      break;
      case  'T':
         j |= GRTEXT_H_T;
      break;
   }

   return j;
}

/******************************************************************************
* neut_c_prop
*/
static int neut_c_prop()
{
   char     tok[MAX_TOKEN],att[MAX_ATT];

   if (skipBoard) return 1;

   if (skipComp)
      return 1;

   // this can happen if the preceeding component did not place.
   if (cur_data == NULL)   return 0;

   while (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 2)   
   {
      if (!STRCMPI(tok,"$PN_EXT"))
      {
         if (att[1] == '|')
            partnumberArray[partnumberArrayCount-1]->pn_ext = &att[1];  // drop the | char
         else
            partnumberArray[partnumberArrayCount-1]->pn_ext = att;   // drop the | char
      }
      else
      if (!STRCMPI(tok,"REFLOC"))
      {
         int i = 0;
         int    visible = TRUE, mirror = FALSE;
         double h = 0.07;
         double x = 0, y = 0, r = 0; 
         char   *lp;
         DbFlag   just = 0;
         double old_scale_factor = scaleFactor;
         int   nounits = TRUE;

         // IN, x, y, 0 , CC, h, w, 0, ,,1
         if (lp = strtok(att," \t,"))
         {
            if (isalpha(lp[0]))
            {
               nounits = FALSE;
               DoUnits(lp);
            }
         }

         // there was a refloc without a unit statement, so this is the first x
         if (nounits)
         {
            x = cnv_tok(lp);
         }
         else
         {
            if (lp = strtok(NULL," \t,"))
            {
               x = cnv_tok(lp);
            }
         }

         if (lp = strtok(NULL," \t,"))
         {
            y = cnv_tok(lp);
         }
         if (lp = strtok(NULL," \t,"))
         {
            r = DegToRad(atof(lp));
            if (cur_data->getInsert()->getMirrorFlags())
               r = DegToRad(360 - atof(lp));
         }

         if (lp = strtok(NULL," \t,"))
         {
            just = get_textjust(lp);
         }
         if (lp = strtok(NULL," \t,"))
         {
            h = cnv_tok(lp);
         }

         int lay;
         // no mirror, that is done if comp is mirrored. Attribut is normalized defined.
         lay = Graph_Level("REFLOC_TOP","",0);

         normalize_text(&x, &y, just, r, (unsigned char)cur_data->getInsert()->getMirrorFlags(), h, 
                        h * 0.8 * strlen(cur_data->getInsert()->getRefname()));

         CString refName(cur_data->getInsert()->getRefname());
         doc->SetVisAttrib(&cur_data->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, TRUE),
            VT_STRING,
            refName.GetBuffer(),
            x, y, r, h, h*0.8, 1, 0, visible, SA_OVERWRITE, 0L,lay,0, 0, 0);
         scaleFactor = old_scale_factor;
      }
      else
      {
         const char *key = get_attribmap(tok);
         if (!STRCMPI(key,ATT_TOLERANCE))
         {
            double      ptol, mtol, tol;
            CString  t;

            check_tolerance(att,&mtol,&ptol,&tol);

            if (tol > -1)
            {
               double tt = tol;
               doc->SetAttrib(&cur_data->getAttributesRef(),doc->IsKeyWord(ATT_TOLERANCE, TRUE),
                  VT_DOUBLE,&tt,SA_OVERWRITE, NULL); // x, y, rot, height
            }
            if (mtol > -1)
            {
               double tt = -mtol;
               doc->SetAttrib(&cur_data->getAttributesRef(),doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE),
                  VT_DOUBLE,&tt,SA_OVERWRITE, NULL); // x, y, rot, height
            }           
            if (ptol > -1)
            {
               double tt = ptol;
               doc->SetAttrib(&cur_data->getAttributesRef(),doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE),
                  VT_DOUBLE,&tt,SA_OVERWRITE, NULL); // x, y, rot, height
            }
         }
         else
         {
            doc->SetUnknownAttrib(&cur_data->getAttributesRef(), key, att,SA_OVERWRITE, NULL); // x, y, rot, height
         }
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int neut_n_prop()
{
   char     tok[MAX_TOKEN],att[MAX_ATT];

   // neut_n_prop is only used in the netlist run. skipBoard does not apply.
   if (skipBoard) return 1;

   while (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 2)   
   {
      if (cur_net)
      {
         doc->SetUnknownAttrib(&cur_net->getAttributesRef(), get_attribmap(tok), att, SA_APPEND,NULL); // x, y, rot, height
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static void check_create()
{
   if (cur_create > 0)
   {
      Graph_Block_Off();
      cur_create = 0;
   }
   return;
}

/****************************************************************************/
/*
*/
static int get_koos(int typ, int apply_boardoffset)
{
   double   x,y;
   char     tok[MAX_TOKEN], att[MAX_ATT];

   while (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT))
   {
      x = cnv_tok(tok);
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT))
      {
         y = cnv_tok(tok);

         if (cur_boardcnt && apply_boardoffset)
         {
            x = x - cur_boardname[cur_boardcnt-1].x;
            y = y - cur_boardname[cur_boardcnt-1].y;
         }
         NeutralPoly p;
         p.x = x;
         p.y = y;
         p.fill = 0;
         p.type = 0;
         polyarray.SetAtGrow(polycnt,p);  // fill, type
         polycnt++;
      } // if y koo
   }  // while 
   return polycnt;
}

/****************************************************************************/
/*
*/
static int  get_geomlistptr(const char *l)
{
   int   i;

   for (i=0;i<geomArrayCount;i++)
   {
      if (geomArray[i]->name.CompareNoCase(l) == 0)
      {
         return i;
      }
   }

   fprintf(fLog,"GEOM [%s] could not be found in geomlist\n",l);
   display_error++;

   return -1;
}

/******************************************************************************
* GetPadstackIndex
*/
static int GetPadstackIndex(CString padstackName)
{
   for (int i=0; i<padstackArrayCount; i++)
   {
      NeutralPadstack *neutPadstack = padstackArray[i];

      if (!neutPadstack->name.CompareNoCase(padstackName))
         return i;
   }

   fprintf(fLog,"PAD [%s] could not be found in padlist\n", padstackName);
   display_error++;
   return -1;
}

/****************************************************************************/
/*
*/
static int neut_geom()
{
   char        tok[MAX_TOKEN], att[MAX_ATT];

   check_create();
   if (skipBoard) return 1;

   sectionStatus |= NEUT_GEOM;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   cur_name = _strupr(tok);

   cur_block = Graph_Block_On(GBO_APPEND, tok, neutralPcbFileNum, 0, blockTypePcbComponent);
   cur_block->setBlockType(blockTypePcbComponent);

   doc->SetUnknownAttrib(&cur_block->getAttributesRef(), ATT_REFNAME, "" ,SA_OVERWRITE, NULL); // x, y, rot, height

   cur_name = tok;
   cur_create = CREATE_GEOM;

   cur_geomlistptr = get_geomlistptr(cur_name);

   return 1;
}

/****************************************************************************/
/*
*/
static int neut_board()
{
   check_create();

   skipBoard = cur_boardname[cur_boardcnt].skip_board;

/*** empty boards
# file : /tmp_mnt/net/pcbhp/designs/33xxx/33400f1/pcb/mfg/neut3.tmp 
# date : Friday April 7, 2000; 13:20:41 
#
#############################################
###Board Information
#############################################
BOARD beast OFFSET x:9.79 y:0.041 ORIENTATION   90
B_UNITS Inches
#
#
#############################################
###Board Information
#############################################
BOARD beast OFFSET x:4.665 y:0.041 ORIENTATION   90
# file : /tmp_mnt/net/pcbhp/designs/33xxx/33400f1/pcb/mfg/neut4.tmp 
# date : Friday April 7, 2000; 13:20:41 
#
***/

   if (!skipBoard)
   {
      if (cur_boardcnt == firstboardptr)
      {
         // 
      }
      else
      {
         // here is a problem:
         // if multiple boardfiles, same compnames, same netlist etc...
         // therefore I stop the do_neutral here.
         if (!only_one_board) // pantheon error
         {
            // if board has same name, just place in panel, and skip it.
            CString  t;
            t.Format("%s_%d", cur_boardname[cur_boardcnt].name, cur_boardcnt);
            boardFile = Graph_File_Start(t, Type_Mentor_Neutral_Layout);   // second boardFile can not be opened with the same name.
            boardFile->setBlockType(blockTypePcb);
            boardFile->getBlock()->setBlockType(boardFile->getBlockType());
            boardFile->setShow(false);
         }
         cur_boardname[cur_boardcnt].filenum = boardFile->getFileNumber();
      }
   } // if skipBoard

   cur_create = CREATE_BOARD;
   cur_boardcnt++;
   return 1;
}

/****************************************************************************/
/*
*/
static int net_board()
{
   skipBoard = cur_boardname[cur_boardcnt].skip_board;

   if (!skipBoard)
   {
      FileStruct *f = doc->Find_File(cur_boardname[cur_boardcnt].filenum);
      doc->PrepareAddEntity(f);
   }
   cur_boardcnt++;
   return 1;
}

/******************************************************************************
* normalize_layer
*/
static const char *normalize_layer(const char *l)
{
   if (!STRCMPI(l, "COMPONENT_OUTLINE"))            return "COMPONENT_OUTLINE_1";
   if (!STRCMPI(l, "COMPONENT_PLACEMENT_OUTLINE"))  return "COMPONENT_PLACEMENT_OUTLINE_1";

   return l;
}

/******************************************************************************
* neut_g_attr
   G_ATTR 'COMPONENT_HEIGHT' ''  0.307
   G_ATTR 'COMPONENT_INSERT_CENTER' ''  0.0 0.0
*/
static int neut_g_attr()
{
   if (skipBoard)
      return 1;

   if (!cur_block)
   {
      fprintf(fLog,"Syntax Error: GEOM statement did not preceed G_ATTR at %ld ->ignored!\n", fMenLineCount);
      display_error++;
		return 1;
   }

   char tok[127], att[MAX_ATT];
   if (get_nextattr(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0) return 0;

   CString key = tok;
   CString layerName = tok;

   if (!strcmp(tok, "COMPONENT_PINS_MOVEABLE"))
   {
      // do nothing
      return 1;
   }
   
   if (!strcmp(tok, "COMPONENT_PLACEMENT_OUTLINE") || !strcmp(tok, "COMPONENT_BODY_OUTLINE") || !strcmp(tok, "COMPONENT_THERMAL_OUTLINE"))
   {
      int graphicClass = graphicClassNormal;

      if (!strcmp(tok, "COMPONENT_BODY_OUTLINE")) // make this the Primary Component Outline
      {
         POSITION dataPos = cur_block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = cur_block->getDataList().GetNext(dataPos);

            if (data->getGraphicClass() == GR_CLASS_COMPOUTLINE)
               data->setGraphicClass(graphicClassNormal);
         }

         graphicClass = GR_CLASS_COMPOUTLINE;
      }

      if (!strcmp(tok, "COMPONENT_PLACEMENT_OUTLINE")) // only make this the Primary Componenet Outline if there is not one already
      {
         graphicClass = GR_CLASS_COMPOUTLINE;

         POSITION dataPos = cur_block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = cur_block->getDataList().GetNext(dataPos);

            if (data->getGraphicClass() == GR_CLASS_COMPOUTLINE)
            {
               graphicClass = graphicClassNormal;
               break;
            }
         }
      }

      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT); // get layer (this is allowed to be emtpy)
      if (res == 3 && strlen(tok))
         layerName = tok;
      else if (!strcmp(tok, "COMPONENT_PLACEMENT_OUTLINE") || strlen(tok) == 0 )
         layerName = "PLACE";
      layerName.MakeUpper();
      
      polycnt = 0;
      get_koos(0, FALSE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
      {
         polyarray.SetAtGrow(polycnt,p1);
         polycnt++;
      }

      double xoff =0, yoff = 0;
      if (cur_geomlistptr > -1)
      {
         xoff = geomArray[cur_geomlistptr]->centerx * scaleFactor;
         yoff = geomArray[cur_geomlistptr]->centery * scaleFactor;
      }

		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layerName, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, graphicClass, xoff, yoff, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, graphicClass, xoff, yoff, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(normalize_layer(layerName), "", 0);
			write_poly(layerNum, 0.0, 0, 0, graphicClass, xoff, yoff, TRUE);
		}

      return 1;
   }

   if (!strcmp(tok, "VIA_KEEPOUT"))
   {
      int res = get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3)
      {
         layerName = LAYER_ALL;
         if (strlen(tok))
            layerName = tok;
      }
      layerName.MakeUpper();

      polycnt = 0;
      get_koos(0, FALSE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
      {
         polyarray.SetAtGrow(polycnt,p1);
         polycnt++;
      }

      double xoff =0, yoff = 0;
      if (cur_geomlistptr > -1)
      {
         xoff = geomArray[cur_geomlistptr]->centerx * scaleFactor;
         yoff = geomArray[cur_geomlistptr]->centery * scaleFactor;
      }

		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layerName, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_VIAKEEPOUT, xoff, yoff, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_VIAKEEPOUT, xoff, yoff, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(layerName, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_VIAKEEPOUT, xoff, yoff, TRUE);
		}

      return 1;
   }

   if (!strcmp(tok,"ROUTING_KEEPOUT") || !strcmp(tok,"TRACE_KEEPOUT"))
   {
      int res = get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3)
      {
         layerName = LAYER_ALL;

         if (strlen(tok) && STRCMPI(tok, "ROUTING_KEEPOUT"))   // if not routing keepout -- this is all layers.
            layerName = tok;
      }

      layerName.MakeUpper();
      polycnt = 0;
      get_koos(0, FALSE);

      if (polycnt)
      {
         NeutralPoly p1 = polyarray.ElementAt(0);
         NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

         // make sure it is closed
         if (p1.x != p2.x || p1.y != p2.y)
         {
            polyarray.SetAtGrow(polycnt,p1);
            polycnt++;
         }

         double xoff =0, yoff = 0;
         if (cur_geomlistptr > -1)
         {
            xoff = geomArray[cur_geomlistptr]->centerx * scaleFactor;
            yoff = geomArray[cur_geomlistptr]->centery * scaleFactor;
         }

			CString layerName_1 = "";
			CString layerName_2 = "";
			if (IsGenericTopBottomMappingLayer(layerName, layerName_1, layerName_2))
			{
				int layerNum = Graph_Level(layerName_1, "", 0);
				write_poly(layerNum, 0.0, 0, 0, GR_CLASS_ROUTKEEPOUT, xoff, yoff, FALSE);

				layerNum = Graph_Level(layerName_2, "", 0);
				write_poly(layerNum, 0.0, 0, 0, GR_CLASS_ROUTKEEPOUT, xoff, yoff, TRUE);
			}
			else
			{
				int layerNum = Graph_Level(layerName, "", 0);
				write_poly(layerNum, 0.0, 0, 0, GR_CLASS_ROUTKEEPOUT, xoff, yoff, TRUE);
			}
      }

      return 1;
   }

   if (!strcmp(tok, "COMPONENT_HEIGHT"))
   {
      double h;
      get_nextattr(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
      h = cnv_tok(tok);
      if (cur_block)
         doc->SetAttrib(&cur_block->getAttributesRef(),doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL);

      return 1;
   }

   if (!strcmp(tok, "COMPONENT_LAYOUT_TYPE"))
   {
      get_nextattr(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
		if (cur_block)
		{
			doc->SetUnknownAttrib(&cur_block->getAttributesRef(), get_attribmap(key), tok, SA_APPEND, NULL);

			if(!STRCMPI(tok,"surface"))
			{
				doc->SetAttrib(&cur_block->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
					VT_NONE,NULL,SA_OVERWRITE, NULL);
			}
		}

		return 1;
   }

   if (strlen(centroid_attr) && !strcmp(tok, centroid_attr))
   {
      double x,y;
      get_nextattr(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // x 
      x = cnv_tok(tok);
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // y 
      y = cnv_tok(tok);

      if (cur_geomlistptr > -1)
      {
         geomArray[cur_geomlistptr]->centroidfound = TRUE;
         geomArray[cur_geomlistptr]->centroidx = x;
         geomArray[cur_geomlistptr]->centroidy = y;
      }

      return 1;
   }

   if (strlen(centroid_angle) && !strcmp(tok, centroid_angle))
   {
      double a;
      get_nextattr(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // x 
      a = atof(tok);

      if (cur_geomlistptr > -1)
      {
         geomArray[cur_geomlistptr]->centroidfound = TRUE;
         geomArray[cur_geomlistptr]->centroidangle = a;
      }

      return 1;
   }

   if (!strcmp(tok, "COMPONENT_INSERT_CENTER"))
   {
      double x,y;
      get_nextattr(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // x 
      x = cnv_tok(tok);
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // y 
      y = cnv_tok(tok);

      BlockStruct *block = doc->GetCentroidGeometry();
      if (cur_block)
      {
         DataStruct *data = Graph_Block_Reference(block->getName(), NULL, neutralPcbFileNum, x, y, DegToRad(0.0), 0, 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypeCentroid);
      }

      return 1;
   }

   if (!strcmp(tok, "DRILL_DEFINITION_UNPLATED"))
   {
      double x,y, drill;
      get_nextattr(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
      drill = cnv_tok(tok);
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // x 
      x = cnv_tok(tok);
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // y 
      y = cnv_tok(tok);

      CString drillname;
      int drilllayernum = Graph_Level("DRILLHOLE","",0);
      drillname.Format("%s_%d","UNPLATED", get_drillindex("UNPLATED", drill, drilllayernum, 1));
      DataStruct *data = Graph_Block_Reference(drillname, NULL, neutralPcbFileNum, x, y, 0.0, 0 , 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypeDrillHole);

      return 1;
   }

   if (!strcmp(tok, "DRILL_DEFINITION_PLATED"))
   {
      double x,y, drill;
      get_nextattr(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // this is allowed to be emtpy 
      drill = cnv_tok(tok);
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // x 
      x = cnv_tok(tok);
      get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT);   // y 
      y = cnv_tok(tok);

      CString  drillname;
      int drilllayernum = Graph_Level("DRILLHOLE","",0);
      drillname.Format("%s_%d","PLATED", get_drillindex("PLATED", drill, drilllayernum, 0));
      DataStruct *data = Graph_Block_Reference(drillname, NULL, neutralPcbFileNum, x, y, 0.0, 0 , 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypeDrillHole);

      return 1;
   }

   if (cur_block)
   {
      int res = get_nextattr(NULL,tok,127,att,MAX_ATT);  // this is allowed to be emtpy 
      doc->SetUnknownAttrib(&cur_block->getAttributesRef(), get_attribmap(key), tok, SA_OVERWRITE, NULL);
   }

   return 1;
}

/******************************************************************************
* neut_g_pin
*/
static int neut_g_pin()
{
   if (skipBoard)
      return 1;

   char tok[MAX_TOKEN], att[MAX_ATT];
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   CString pinName = tok;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   double x = cnv_tok(tok);

   if (cur_geomlistptr > -1)
      x -= geomArray[cur_geomlistptr]->centerx * scaleFactor;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   double y = cnv_tok(tok);

   if (cur_geomlistptr > -1)
      y -= geomArray[cur_geomlistptr]->centery * scaleFactor;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   CString padName = _strupr(tok);

   if (is_attrib(padName, PADIGNORE_ATTR))
      return 0;

   // Begin, added DF 2004.10.20
   // added check if padstack used is empty and IGNORE_EMPTY_PIN set to Y then skip empty pin and log
   int padIndex = GetPadstackIndex(padName);
   NeutralPadstack *neutPadstack = NULL;
   if (padIndex > -1)
   {
      neutPadstack = padstackArray[padIndex];
      if (!(neutPadstack->hasContent) && IgnoreEmptyPins)
	  {
         fprintf(fLog,"Skipped Pin: GEOM: \"%s\", PIN: \"%s\" at line %ld \n", cur_name, pinName, fMenLineCount);
         display_error++;
	  }
	  else
	  {
   // Pause, added DF 2004.10.20
		 if (strlen(cur_name))
         {
            Graph_Block_On(GBO_APPEND, padName, neutralPcbFileNum, 0, blockTypePadstack);
            Graph_Block_Off();

            Graph_Block_On(GBO_APPEND, cur_name, neutralPcbFileNum, 0, blockTypePcbComponent);

            // Case dts0100383941 - Sample data for this case has padstacks and pcb components with exact same name.
            // This code originally made the Reference by name only, resulting in a recursive pcb component insert,
            // which in turn crashed camcad. If we are inserting a pin here, thenit should be a padstack, so be
            // specific about the block type we are inserting.
            BlockTypeTag pinBlockType = blockTypePadstack; // assume normal situation
            // I'm not sure about this one, so if a fiducial seems to be involved, then leave the block type open
            if (is_attrib(padName, PADFIDUCIAL_ATTR))
               pinBlockType = blockTypeUndefined;
            DataStruct *padInsert = Graph_Block_Reference(padName, pinName, neutralPcbFileNum, x, y, 0, 0, 1, -1, TRUE, blockTypePadstack);

            if (is_attrib(padName, PADFIDUCIAL_ATTR))
               padInsert->getInsert()->setInsertType(insertTypeFiducial);
            else
               padInsert->getInsert()->setInsertType(insertTypePin);

            Graph_Block_Off();
         }
         else
         {
            fprintf(fLog,"Syntax Error: GEOM statement did not preceed G_ATTR at %ld ->ignored!\n", fMenLineCount);
            display_error++;
         }
   // Resume, added DF 2004.10.20
	  }
   }
   // End, added DF 2004.10.20
   return 1;
}

/******************************************************************************
* neut_n_via
*  - this is done in the second pass, so I can know the padlayer index
*/
static int neut_n_via()
{
   char tok[MAX_TOKEN], att[MAX_ATT];

   if (skipBoard)
      return 1;
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;
   double x = cnv_tok(tok);

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;
   double y = cnv_tok(tok);

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;
   CString padstackName = _strupr(tok);

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;
   int startLayer = atoi(tok);

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;
   int endLayer = atoi(tok);

   if (cur_boardcnt)
   {
      x = x - cur_boardname[cur_boardcnt-1].x;
      y = y - cur_boardname[cur_boardcnt-1].y;
   }

   if (startLayer == 1 && endLayer == thruPadLayerCount)
   {
      // this is a complete thru hole via
   }
   else
   {
      int padIndex = GetPadstackIndex(padstackName);
      if (padIndex < 0)
      {
         fprintf(fLog, "Can not find padstackName [%s] in PAD definition.\n", padstackName);
         display_error++;
      }
      else
      {
         NeutralPadstack *neutPadstack = padstackArray[padIndex];

         if (neutPadstack->padtype != PADTYPE_BURIED)
         {
            fprintf(fLog, "Layer range [%d %d] on via [%s] done in a non buried via -> definition ignored.\n", startLayer, endLayer, padstackName);
            display_error++;
         }
         else
         {  
            CString viaName;
            viaName.Format("%s (%s_%s)", padstackName, thruPadLayerArray[startLayer-1], thruPadLayerArray[endLayer-1]);

            if (Graph_Block_Exists(doc, viaName, neutralPcbFileNum) == NULL)
            {
               BlockStruct *origVia = Graph_Block_Exists(doc, padstackName, neutralPcbFileNum);

               BlockStruct *newVia = Graph_Block_On(GBO_APPEND, viaName, neutralPcbFileNum, 0);
               newVia->setBlockType(blockTypePadstack);

               CreateLayerRangePadstackData(&origVia->getDataList(), startLayer, endLayer);

               Graph_Block_Off();
            }

            padstackName = viaName;
         }
      }
   }

   DataStruct *data = Graph_Block_Reference(padstackName, NULL, neutralPcbFileNum, x, y, DegToRad(0), 0 , 1.0, -1, TRUE);

   if (strlen(cur_name))
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_name.GetBuffer(0), SA_APPEND, NULL);

   //if (startLayer == endLayer)
   //   data->getInsert()->setInsertType(insertTypeTestPad);
   //   // this is a free pad - a via which starts and ends on the same layer
   //   // the layer is an index number, which can be derived from a thru p_shape
   //   // this must be from type bur
   //   // if there is a N_Test on the same location with same layer, drop the via
   //   // and use the n_Test point
   //else
      data->getInsert()->setInsertType(insertTypeVia);

   return 1;
}

/****************************************************************************/
/*
*/
static int correct_pinposition(const char *compname, const char *pinname, 
                               double x , double y, double padrot)
{
   double   pinx, piny;
   BlockStruct *b;

   if (!(sectionStatus & NEUT_GEOM))
   {
      fprintf(fLog, "Can not update moved pin position, because no GEOMETRY section is defined before Line %ld\n",
         fMenLineCount);
      display_error++;
      return 1;
   }

   // normalize pin koo to geom level.
   pinx = x - cur_comp.x;
   piny = y - cur_comp.y;

   if (cur_comp.rot)
   {
      // need to unrotate
      double xrot, yrot;
      Rotate(pinx , piny , 360-cur_comp.rot, &xrot, &yrot);
      pinx = xrot;
      piny = yrot;
   }

   if (cur_comp.mirror)
   {
      // need to unmirror
      pinx = -pinx;
   }

   // pin pin in geom.
   int      found = 0;

   if ((b = Graph_Block_Exists(doc, cur_comp.new_geomname, neutralPcbFileNum)) == NULL)
   {
      fprintf(fLog, "Can not update moved pin position, because no GEOMETRY [%s] not defined\n", cur_comp.new_geomname);
      display_error++;
      return -1;
   }


   DataStruct *np;
   POSITION pos = b->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = b->getDataList().GetNext(pos);
      
      if (np->getDataType() == T_INSERT)        
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {
            // pinname is in refname if a insert
            if (!np->getInsert()->hasRefnameData()) continue;
            if (!strcmp(np->getInsert()->getRefname(), pinname))
            {
               np->getInsert()->setOriginX(pinx);
               np->getInsert()->setOriginY(piny);
               np->getInsert()->setAngleDegrees(padrot);
               found++;
               break;
            }
         }
      }
   } 

   if (!found)
   {
      fprintf(fLog, "Can not update moved pin position, because no Pin [%s] found in GEOMETRY [%s]\n", 
            pinname, cur_comp.new_geomname);
      display_error++;
   }
 
   return 1;
}

/******************************************************************************
* neut_c_pin
*/
static int neut_c_pin()
{
   if (skipBoard)
      return 1;

   if (skipComp)
      return 1;

   // skip if the current component is not placed and unplaced_components is false
   if (!cur_compplaced && !unplaced_components)
      return 1;
   char tok[MAX_TOKEN], att[MAX_ATT];

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;

   char *lp;
   if ((lp = strtok(tok,"-")) == NULL)
      return 0;
   CString compName = lp;
   compName.MakeUpper();

   if ((lp = strtok(NULL,"\n")) == NULL)
      return 0;
   CString pinName = lp;
   
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;
   double x = cnv_tok(tok);

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
      return 0;
   double y = cnv_tok(tok);

   if (cur_boardcnt)
   {
      x = x - cur_boardname[cur_boardcnt-1].x;
      y = y - cur_boardname[cur_boardcnt-1].y;
   }


   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   int pinAccess = atoi(tok); // 0 all 1 top other than 1 bottom

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   
   int compPlace = atoi(tok); 
   int padMirror = (compPlace > 1)?1:0;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;

   double padRot = atof(tok);
   padRot = normalizeDegrees(((cur_comp.mirror)?360 - padRot:padRot) - cur_comp.rot);

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   CString padName = _strupr(tok);

   // Begin, added DF 2004.10.20
   // Here we skip comp pins if the used padstack is empty and the IGNORE_EMPTY_PINS is set to Y.
   int padIndex = GetPadstackIndex(padName);
   NeutralPadstack *neutPadstack = NULL;
   if (padIndex > -1)
   {
      neutPadstack = padstackArray[padIndex];
      if ((neutPadstack->hasContent == 0) && (IgnoreEmptyPins == TRUE))
		  return 1;
   }
   // End, added DF 2004.10.20
   
   if (is_attrib(padName, PADIGNORE_ATTR))
      return 0;
   if (is_attrib(padName, PADFIDUCIAL_ATTR))
      return 0;

   CString netname = GetNetname(curlp);   // netname can contain spaces.
   netname.TrimLeft();
   netname.TrimRight();
   //if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) != 0) 
   //{
   // netname = tok;
   //}

   if (netname.CompareNoCase("$None$") == 0)
   {
      netname = NET_UNUSED_PINS;
   }
   NetStruct *n = add_net(boardFile, netname);

   CompPinStruct *cp = add_comppin(boardFile, n, compName, pinName);
   cp->setPinCoordinatesComplete(true);
   cp->setOrigin(x,y);
   cp->setRotationDegrees(padRot);
   cp->setMirror(padMirror);
   BlockStruct *padGeom = Graph_Block_On(GBO_APPEND, padName, neutralPcbFileNum, 0);
   Graph_Block_Off();
   cp->setPadstackBlockNumber( padGeom->getBlockNumber());
   
   char *tl;
   switch (pinAccess)
   {
   case 0:
      tl = testaccesslayers[ATT_TEST_ACCESS_ALL];
      cp->setVisible(VISIBLE_BOTH);
      break;
   case 1:
      tl = testaccesslayers[ATT_TEST_ACCESS_TOP];
      cp->setVisible(VISIBLE_TOP);
      break;
   default: // greater 1
      tl = testaccesslayers[ATT_TEST_ACCESS_BOT];
      cp->setVisible(VISIBLE_BOTTOM);
      break;
   }

   doc->SetAttrib(&cp->getAttributesRef(), doc->IsKeyWord(ATT_PINACCESS, TRUE), VT_STRING, tl, SA_OVERWRITE, NULL);

   correct_pinposition(compName, pinName, x, y, padRot);

   return 1;
}

/****************************************************************************/
/*
*/
static int get_testaccess(const char *l)
{
   if (!STRCMPI(l,"TOP|BOT"))
      return ATT_TEST_ACCESS_ALL;
   
   if (!STRCMPI(l,"TOP"))
      return ATT_TEST_ACCESS_TOP;

   if (!STRCMPI(l,"BOT"))
      return ATT_TEST_ACCESS_BOT;

   return ATT_TEST_ACCESS_ALL;
}

/****************************************************************************/
/*
*/
static int neut_n_test()
{
   char        tok[MAX_TOKEN], att[MAX_ATT];
   double      x,y;
   CString     testlayer;  // TOP, BOT, TOP|BOT
   CString vianame;

   if (skipBoard) return 1;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;

   if (!STRCMPI(tok,"VIA"))
   {
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
      x = cnv_tok(tok);
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
      y = cnv_tok(tok);
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // test layer
      testlayer = tok;
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // test padstack
      vianame = tok;
   
      if (cur_boardcnt)
      {
         x = x - cur_boardname[cur_boardcnt-1].x;
         y = y - cur_boardname[cur_boardcnt-1].y;
      }

      TestPinList.Add(vianame, x, y, get_testaccess(testlayer), cur_net);
   }
   else
   {
      char  *lp;
      CString compname, pinname;
      CString testlayer;   // TOP, BOT, TOP|BOT

      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
         return 0;   // x
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
         return 0;   // y
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
         return 0;   // access
      testlayer = tok;
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
         return 0;   // padstack

      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)
         return 0;   // name-pin
      if ((lp = strtok(tok,"-")) == NULL)
         return 0;
      compname = lp;
      if ((lp = strtok(NULL,"\n")) == NULL)
         return 0;
      pinname = lp;

      TestPinList.Add(compname, pinname, get_testaccess(testlayer), cur_net);
   }
   return 1;
}


static int get_testpointattr(char *neutralfmt, CString *refname, PageUnitsTag *pageunit)
{
   char *startp, *endp;
   int len;

   //get refname
   startp = strstr(neutralfmt,"[") + 1;
   endp = strchr(neutralfmt,'(');
   if(!endp || !startp)
	   return 0;
   len = endp - startp;
   *endp = '\0';
   *refname = startp;

   //get pageunit
   startp = startp + len +1;
   endp = strchr(startp,',');
   if(!endp || !startp)
	   return 0;
   *endp = '\0';
   *pageunit = unitStringToTag(startp);

   return 1;
}

static int get_testpinattr(char *neutralfmt, CString *pinname, CString *pinnumber)
{
   char *startp, *endp;

   //XXX-XX
   //get pinname
   startp = neutralfmt;
   endp = strchr(neutralfmt,'-');
   if(!endp || !startp)
	   return 0;

   *endp = '\0';
   *pinname = startp;
   //get pinnumber
   startp = endp + 1;
   *pinnumber = startp;

   return 1;
}

/****************************************************************************/
/* neut_t_test
*/
static int neut_t_test()
{
   char  tok[MAX_TOKEN], att[MAX_ATT];
   TTestPoints	t_testpoint;
   PageUnitsTag	pageunit;
   CString compname, pinname, tokline;

   if (skipBoard) return 1;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0; //VIA or PIN
	t_testpoint.pintype = (!strcmp(tok,"VIA"))?TestProbePinTypeVia:TestProbePinTypeComppin; 
   
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // netname
   t_testpoint.netname = GetNetname(tok);
   
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;	// x location
   t_testpoint.x = atof(tok);
   
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;	// y location
   t_testpoint.y = atof(tok);   
   
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;	// PCB side
   tokline = tok;
   t_testpoint.pcbside = (tokline.MakeUpper().Find("BOT") > -1)?testSurfaceBottom:testSurfaceTop;
   
   if(t_testpoint.pintype == TestProbePinTypeVia) // VIA
   {
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // VIA-vianame; 
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // VIA-inserted or mapped; 
   }
   else	//COMPPIN
   {
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // PIN - 
      if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // PIN - name and number		
      if (get_testpinattr(tok,&compname,&pinname) == 0)	return 0;   			
   }

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;   // refname, unitpage
   if (get_testpointattr(tok,&t_testpoint.refname,&pageunit) == 0) return 0;

   //transform units
   double unitFactor = Units_Factor(pageunit, doc->getSettings().getPageUnits());
   t_testpoint.x *= unitFactor;
   t_testpoint.y *= unitFactor;

   if(t_testpoint.pintype == TestProbePinTypeVia) // VIA
   {
      TestProbeDataList.Add(t_testpoint.refname, t_testpoint.netname, t_testpoint.pcbside, 
	      t_testpoint.x, t_testpoint.y);
   }
   else
   {
      TestProbeDataList.Add(t_testpoint.refname, t_testpoint.netname, t_testpoint.pcbside,compname,pinname, 
	      t_testpoint.x, t_testpoint.y);
   }
   
   return 1;    
}

/******************************************************************************
* neut_n_pin
*/
static int neut_n_pin()
{
   if (skipBoard)
      return 1;

/*
   char        tok[MAX_TOKEN], att[MAX_ATT];
   int         compplace, pinaccess;
   CString     pinname, compname, pname;
   double      x,y;
   char        *lp;
   CompPinStruct *cp;

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   if ((lp = strtok(tok,"-")) == NULL)          return 0;
   compname = lp;
   if ((lp = strtok(NULL,"\n")) == NULL)        return 0;
   pinname = lp;
   
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   x = cnv_tok(tok);
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   y = cnv_tok(tok);
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   pname = tok;
   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   pinaccess = atoi(tok);  // 0 all 1 top other than 1 bottom

   if (get_nextword(NULL,tok,MAX_TOKEN,att,MAX_ATT) == 0)   return 0;
   compplace = atoi(tok);  

   int padmirror = 0;
   double padrot = 0.0;

   if (compplace > 1)
      padmirror = 1;

   cp = add_comppin(boardFile, cur_name,compname,pinname);
   sprintf(tok,"%lg, %lg, %lg, %s, %d", x, y, padrot , pname, padmirror);  // mirror
   doc->SetAttrib(&cp->getAttributesRef(),doc->RegisterKeyWord(ATT_PINLOC, TRUE),
         VT_STRING,
         tok, 0.0,0.0,0.0,0.0, FALSE, SA_OVERWRITE, 0L,0,0); // x, y, rot, height

*/
   return 1;
}

/****************************************************************************/
/*
*/
static int neut_skip()
{
   // do nothing, skip the line.
   return 1;
}

/******************************************************************************
* neut_b_attr
*/
static int neut_b_attr()
{
   char tok[MAX_TOKEN], att[MAX_ATT];
   CString layername;
   CString attrname;
   int grclass = 0;

   if (skipBoard)
      return 1;

   if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT) == 0)
      return 0;
   layername = tok;
   attrname  = tok;
   sectionStatus |= NEUT_B_ATTR;

   grclass = get_b_attr_graphicclass(tok);

   // here is component pins, outlines and other block attributes.
   if (!strcmp(tok, "BOARD_ROUTING_OUTLINE"))
   {
      layername = tok;
      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3 && grclass != GR_CLASS_BOARDOUTLINE)
      {
         layername = LAYER_ALL;
         if (strlen(tok))
				layername = tok;
      }
      layername.MakeUpper();

      polycnt = 0;
      if (res != 3)  // there was no ' quoted second argument.
      {
         double y, x = cnv_tok(tok);
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
         {
            y = cnv_tok(tok);

            if (cur_boardcnt)
            {
               x = x - cur_boardname[cur_boardcnt-1].x;
               y = y - cur_boardname[cur_boardcnt-1].y;
            }
            NeutralPoly p;
            p.x = x;
            p.y = y;
            p.fill = 0;
            p.type = 0;
            polyarray.SetAtGrow(polycnt++, p);  // fill, type
         }
      }
      get_koos(0, TRUE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
         polyarray.SetAtGrow(polycnt++, p1);

		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(layername, "", 0);
			write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, TRUE);
		}
   }
   else if (!strcmp(tok, "BOARD_OUTLINE"))
   {
      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3 && grclass != GR_CLASS_BOARDOUTLINE)
      {
         if (strlen(tok))
				layername = tok;
      }
      layername.MakeUpper();

		polycnt = 0;
      if (res != 3)  // there was no ' quoted second argument.
      {
         double y, x = cnv_tok(tok);
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
         {
            y = cnv_tok(tok);

            if (cur_boardcnt)
            {
               x = x - cur_boardname[cur_boardcnt-1].x;
               y = y - cur_boardname[cur_boardcnt-1].y;
            }
            NeutralPoly p;
            p.x = x;
            p.y = y;
            p.fill = 0;
            p.type = 0;
            polyarray.SetAtGrow(polycnt++, p);  // fill, type
         }
      }
      get_koos(0, TRUE);

		if (polycnt > 0)
		{
			NeutralPoly p1 = polyarray.ElementAt(0);
			NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

			// make sure it is closed
			if (p1.x != p2.x || p1.y != p2.y)
				polyarray.SetAtGrow(polycnt++, p1);

			CString layerName_1 = "";
			CString layerName_2 = "";
			if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
			{
				int layerNum = Graph_Level(layerName_1, "", 0);
				write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, FALSE);

				layerNum = Graph_Level(layerName_2, "", 0);
				write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, TRUE);
			}
			else
			{
				int layerNum = Graph_Level(layername, "", 0);
				write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, TRUE);
			}
		}
   }
   else if (!strcmp(tok, "ROUTING_KEEPOUT") || !strcmp(tok, "TRACE_KEEPOUT"))
   {
      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3 && grclass != GR_CLASS_BOARDOUTLINE)
      {
         layername = LAYER_ALL;
         if (strlen(tok))
				layername = tok;
      }
      layername.MakeUpper();

      polycnt = 0;
      if (res != 3)  // there was no ' quoted second argument.
      {
         double y,x = cnv_tok(tok);
         if (get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT))
         {
            y = cnv_tok(tok);

            if (cur_boardcnt)
            {
               x = x - cur_boardname[cur_boardcnt-1].x;
               y = y - cur_boardname[cur_boardcnt-1].y;
            }
            NeutralPoly p;
            p.x = x;
            p.y = y;
            p.fill = 0;
            p.type = 0;
            polyarray.SetAtGrow(polycnt++, p);  // fill, type
         }
      }
      get_koos(0, TRUE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
         polyarray.SetAtGrow(polycnt++, p1);

		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_ROUTKEEPOUT, 0, 0, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_ROUTKEEPOUT, 0, 0, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(layername, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_ROUTKEEPOUT, 0, 0, TRUE);
		}
   }
   else if (!strcmp(tok, "BOARD_PLACEMENT_KEEPOUT"))
   {
      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3 && grclass != GR_CLASS_BOARDOUTLINE)
      {
         layername = LAYER_ALL;
         if (strlen(tok))
				layername = tok;
      }
      layername.MakeUpper();

      polycnt = 0;
      get_koos(0, TRUE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
         polyarray.SetAtGrow(polycnt++, p1);

		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_PLACEKEEPOUT, 0, 0, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_PLACEKEEPOUT, 0, 0, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(layername, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_PLACEKEEPOUT, 0, 0, TRUE);
		}
   }
   else if (!strcmp(tok, "BOARD_PLACEMENT_REGION"))
   {
      // layer is boardside, fillpattern, maxheight, minheight, ciruitgroup

      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3 && grclass != GR_CLASS_BOARDOUTLINE)
      {
         layername = LAYER_ALL;
         if (strlen(tok))  
         {
            char *lp = strtok(tok, ",");   // layer
            if (lp && strlen(lp))
               layername = lp;

         }
      }
      layername.MakeUpper();

      polycnt = 0;
      get_koos(0, TRUE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
         polyarray.SetAtGrow(polycnt++, p1);

		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, graphicClassPlacementRegion, 0, 0, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, graphicClassPlacementRegion, 0, 0, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(layername, "", 0);
			write_poly(layerNum, 0.0, 0, 0, graphicClassPlacementRegion, 0, 0, TRUE);
		}
   }
   else if (!strcmp(tok, "BOARD_PLACEMENT_OUTLINE"))
   {
      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3 && grclass != GR_CLASS_BOARDOUTLINE)
      {
         layername = LAYER_ALL;
         if (strlen(tok))
				layername = tok;
      }
      layername.MakeUpper();

      polycnt = 0;
      get_koos(0, TRUE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)      
         polyarray.SetAtGrow(polycnt++, p1);
      
		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_PLACEKEEPIN, 0, 0, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_PLACEKEEPIN, 0, 0, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(layername, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_PLACEKEEPIN, 0, 0, TRUE);
		}
   }
   else if (!strcmp(tok, "VIA_KEEPOUT"))
   {
      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3 && grclass != GR_CLASS_BOARDOUTLINE)
      {
         layername = LAYER_ALL;
         if (strlen(tok))
				layername = tok;
      }
      layername.MakeUpper();

      polycnt = 0;
      get_koos(0, TRUE);

      NeutralPoly p1 = polyarray.ElementAt(0);
      NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
         polyarray.SetAtGrow(polycnt++, p1);

		CString layerName_1 = "";
		CString layerName_2 = "";
		if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
		{
			int layerNum = Graph_Level(layerName_1, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_VIAKEEPOUT, 0, 0, FALSE);

			layerNum = Graph_Level(layerName_2, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_VIAKEEPOUT, 0, 0, TRUE);
		}
		else
		{
			int layerNum = Graph_Level(layername, "", 0);
			write_poly(layerNum, 0.0, 0, 0, GR_CLASS_VIAKEEPOUT, 0, 0, TRUE);
		}
   }
   else if (!STRCMPI(tok, "FIXED_COMPONENT_LOCATION") || !STRCMPI(tok, "BOARD_ROUTING_DIRECTION") ||
				!STRCMPI(tok, "TJUNCTIONS_ALLOWED")			|| !STRCMPI(tok, "DIAGONAL_ROUTING_ALLOWED") ||
				!STRCMPI(tok, "DEFAULT_PAD_SIZE")			|| !STRCMPI(tok, "BOARD_DEFAULT_PADSTACK") ||
				!STRCMPI(tok, "BOARD_DEFAULT_VIA")			|| !STRCMPI(tok, "DRILL_ORIGIN") ||
				!STRCMPI(tok, "MILLING_ORIGIN")				|| !STRCMPI(tok, "BOARD_DEFINITION_IDENTIFIER") ||
				!STRCMPI(tok, "BOARD_PLACEMENT_CLEARANCE") || !STRCMPI(tok, "BOARD_PLACEMENT_GRID") )
   {
      // no message
   }
   else if (!STRCMPI(tok, "POWER_NET_NAMES"))
   {
      // do nothing
   }
   else if (!STRCMPI(tok, "POINTER"))
   {
      // do nothing
   }
   else  if (!strcmp(tok, "DRILL_DEFINITION_UNPLATED"))
   {
      get_nextattr(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // this is allowed to be emtpy 
      double drill = cnv_tok(tok);

      get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // x 
      double x = cnv_tok(tok);

      get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // y 
      double y = cnv_tok(tok);

      if (cur_boardcnt)
      {
         x = x - cur_boardname[cur_boardcnt-1].x;
         y = y - cur_boardname[cur_boardcnt-1].y;
      }

      CString drillname;
      int drilllayernum = Graph_Level("DRILLHOLE", "", 0);
      drillname.Format("%s_%d", "UNPLATED", get_drillindex("UNPLATED", drill, drilllayernum, 1));
      DataStruct *data = Graph_Block_Reference(drillname, NULL, neutralPcbFileNum, x, y, 0.0, 0, 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypeDrillTool);
   }
   else if (!strcmp(tok, "BOARD_THICKNESS"))
   {
      get_nextattr(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // this is allowed to be emtpy 
      get_nextattr(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // 
      double thickness = cnv_tok(tok);

      if (thickness > 0)
      {
         doc->SetAttrib(&boardFile->getBlock()->getAttributesRef(), doc->IsKeyWord(BOARD_THICKNESS, 0),
            VT_UNIT_DOUBLE, &thickness, SA_OVERWRITE, NULL);
      }
   }
   else if (!strcmp(tok, "BOARD_ROUTING_LAYERS"))
   {
      int number_of_routinglayers;
      get_nextattr(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // this is allowed to be emtpy 
      get_nextattr(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // 

      if (strlen(tok))
         number_of_routinglayers = atoi(tok);
   }
   else if (!strcmp(tok, "DRILL_DEFINITION_PLATED"))
   {
      get_nextattr(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // this is allowed to be emtpy 
      double drill = cnv_tok(tok);

      get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // x 
      double x = cnv_tok(tok);

      get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);   // y 
      double y = cnv_tok(tok);

      if (cur_boardcnt)
      {
         x = x - cur_boardname[cur_boardcnt-1].x;
         y = y - cur_boardname[cur_boardcnt-1].y;
      }

      CString drillname;
      int drilllayernum = Graph_Level("DRILLHOLE", "", 0);
      drillname.Format("%s_%d", "PLATED", get_drillindex("PLATED", drill, drilllayernum, 0));
      DataStruct *data = Graph_Block_Reference(drillname, NULL, neutralPcbFileNum, x, y, 0.0, 0, 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypeDrillTool);
   }
   else
   {
      int res = get_nextword(NULL, tok, MAX_TOKEN, att, MAX_ATT);  // this is allowed to be emtpy 
      if (res == 3)
      {
         if (strlen(tok))
				layername = tok;
      }
      layername.MakeUpper();

      polycnt = 0;
      get_koos(0, TRUE);

      if (polycnt > 2)
      {
         NeutralPoly p1 = polyarray.ElementAt(0);
         NeutralPoly p2 = polyarray.ElementAt(polycnt-1);

         // make sure it is closed
         if (p1.x != p2.x || p1.y != p2.y)
            polyarray.SetAtGrow(polycnt++, p1);

			CString layerName_1 = "";
			CString layerName_2 = "";
			if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
			{
				int layerNum = Graph_Level(layerName_1, "", 0);
				write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, FALSE);

				layerNum = Graph_Level(layerName_2, "", 0);
				write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, TRUE);
			}
			else
			{
				int layerNum = Graph_Level(layername, "", 0);
				write_poly(layerNum, 0.0, 0, 0, grclass, 0, 0, TRUE);
			}
      }
      else
      {
#ifdef _DEBUG
         fprintf(fLog, "DEBUG: Unknown B_ATTR [%s] at %ld\n", attrname, fMenLineCount);
#endif
      }
   }
   return 1;
}

/******************************************************************************
* CreateLayerRangePadstackData
*/
static void CreateLayerRangePadstackData(CDataList *dataList, int startLayer, int endLayer)
{                                    
   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (block->getFlags() & BL_TOOL)
      {
         // always do the drill
         DataStruct *insert = Graph_Block_Reference(block->getName(), data->getInsert()->getRefname(), block->getFileNumber(), 
               data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), 
               data->getInsert()->getScale(), data->getLayerIndex(), TRUE);
         insert->getInsert()->setInsertType(data->getInsert()->getInsertType());
      }
      else
      {
         LayerStruct *layer = doc->FindLayer(data->getLayerIndex());

         for (int i=startLayer-1; i<endLayer; i++)
         {
            if (!layer->getName().CompareNoCase(thruPadLayerArray[i]))
            {
               DataStruct *insert = Graph_Block_Reference(block->getName(), data->getInsert()->getRefname(), block->getFileNumber(), 
                     data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), 
                     data->getInsert()->getScale(), data->getLayerIndex(), TRUE);
               insert->getInsert()->setInsertType(data->getInsert()->getInsertType());
               break;
            }
         }
      }
   }
}

/******************************************************************************
* UpdateLayer
*/
static int UpdateLayer(CString layerName)
{
   layerName.MakeUpper();

   for (int i=0; i<neutralLayerArrayCount; i++)
   {
      NeutralLayer *neutLayer = &neutralLayerArray[i]; 

      if (!layerName.CompareNoCase(neutLayer->name))
         return i;      
   }

   if (neutralLayerArrayCount < MAX_LAYERS)
   {
      NeutralLayer *neutLayer = &neutralLayerArray[neutralLayerArrayCount++]; 

      neutLayer->name = STRDUP(layerName);
      neutLayer->mirrorLayerNum = -1;
      neutLayer->padelectrical = 0;
   }

   return neutralLayerArrayCount-1;
}

/******************************************************************************
* update_mirrorlayers
*/
static int update_mirrorlayers()
{
   int highest_padlayer = 1;

   // assign pad layers
	int i=0;
   for (i=0; i<neutralLayerArrayCount; i++)
   {
      if (doc->FindLayer_by_Name(neutralLayerArray[i].name) == NULL)
			continue;

      if (neutralLayerArray[i].padelectrical == 0)
			continue;
      
      if (neutralLayerArray[i].padelectrical > highest_padlayer)
         highest_padlayer = neutralLayerArray[i].padelectrical;         
   }

   LayerStruct *layer = doc->FindLayer_by_Name("DRILLHOLE");
   if (layer)
      layer->setLayerType(LAYTYPE_DRILL);

   // this is a fixed attribute
   int layerNum = Graph_Level("COMPONENT_PLACEMENT_OUTLINE_1", "", 0);
   layer = doc->FindLayer(layerNum);
   if (layer)
   {
      layer->setLayerType(LAYTYPE_SILK_TOP);
      layer->setComment("Silkscreen Top");
   }

   layerNum = Graph_Level("COMPONENT_PLACEMENT_OUTLINE_2", "", 0);
   layer = doc->FindLayer(layerNum);
   if (layer)
   {
      layer->setLayerType(LAYTYPE_SILK_BOTTOM);
      layer->setComment("Silkscreen Bottom");
   }

   Graph_Level_Mirror("COMPONENT_PLACEMENT_OUTLINE_1", "COMPONENT_PLACEMENT_OUTLINE_2", "");

   layerNum = Graph_Level("REFLOC_TOP", "", 0);
   layer = doc->FindLayer(layerNum);
   if (layer)
   {
      layer->setLayerType(LAYTYPE_TOP);
      layer->setComment("Reference Name Top");
   }

   layerNum = Graph_Level("REFLOC_BOT", "", 0);
   layer = doc->FindLayer(layerNum);
   if (layer)
   {
      layer->setLayerType(LAYTYPE_BOTTOM);
      layer->setComment("Reference Name Bottom");
   }

   Graph_Level_Mirror("REFLOC_TOP", "REFLOC_BOT", "");

   for (i=0; i<neutralLayerArrayCount; i++)
   {
      if (doc->FindLayer_by_Name(neutralLayerArray[i].name) == NULL)
			continue;

      // mirror from Settings file neutral.in
      if (neutralLayerArray[i].mirrorLayerNum > -1)
      {
         CString layerName = neutralLayerArray[i].name;
         CString mirrorLayerName = neutralLayerArray[neutralLayerArray[i].mirrorLayerNum].name;
         Graph_Level_Mirror(layerName, mirrorLayerName, "");
         continue;
      }
   }

   for (i=1; i<=highest_padlayer; i++)
   {
      for (int ii=0; ii<neutralLayerArrayCount; ii++)
      {
         int pl = neutralLayerArray[ii].padelectrical;
         if (neutralLayerArray[ii].padelectrical == i)   
         {
            int layerNum = Graph_Level(neutralLayerArray[ii].name, "", 0);
            LayerStruct *layer = doc->FindLayer(layerNum);

            if (i == 1)
            {
               layer->setComment("Pad Top");
               layer->setLayerType(LAYTYPE_PAD_TOP);
               layer->setElectricalStackNumber(i);
            }
            else if (i == highest_padlayer)
            {
               layer->setComment("Pad Bottom");
               layer->setLayerType(LAYTYPE_PAD_BOTTOM);
               layer->setElectricalStackNumber(i);
            }
            else
            {
               layer->setComment("Pad Inner");
               layer->setLayerType(LAYTYPE_PAD_INNER);
               layer->setElectricalStackNumber(i);
               layer->setVisible(false);
            }
         }  
      }
   }

   for (i=0; i<thruPadLayerCount; i++)
   {
      int layerNum = Graph_Level(thruPadLayerArray[i], "", 0);
      LayerStruct *layer = doc->FindLayer(layerNum);

      if (i == 0)
      {
         layer->setComment("Pad Top");
         layer->setLayerType(LAYTYPE_PAD_TOP);
      }
      else if (i == thruPadLayerCount - 1)
      {
         layer->setComment("Pad Bottom");
         layer->setLayerType(LAYTYPE_PAD_BOTTOM);
      }
      else
      {
         layer->setComment("Pad Inner");
         layer->setLayerType(LAYTYPE_PAD_INNER);
      }

      layer->setElectricalStackNumber(i+1);
   }

   // now mirror all thruPadLayerArray
   for (i=0; i<thruPadLayerCount; i++)
   {
      Graph_Level_Mirror(thruPadLayerArray[i], thruPadLayerArray[thruPadLayerCount-i-1], "");
   }

   // now mark layer as a boardoutline layer
   for (int j = 0; j<doc->getMaxLayerIndex(); j++)
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (!layer)
         continue;

      if (!layer->getName().CompareNoCase(LAYER_ALL)) // generic all
         layer->setLayerType(LAYTYPE_ALL);

      if (!layer->getName().CompareNoCase("COMPONENT_BODY_OUTLINE_1"))
      {
         layer->setLayerType(LAYTYPE_TOP);
         
         int layerNum2 = Graph_Level("COMPONENT_BODY_OUTLINE_2", "", 0);
         LayerStruct *layer2 = doc->FindLayer(layerNum2);
         layer2->setLayerType(LAYTYPE_BOTTOM);
      
         Graph_Level_Mirror("COMPONENT_BODY_OUTLINE_1", "COMPONENT_BODY_OUTLINE_2", "");
      }

      for (i=0; i<boardOutlineNameArrayCount; i++)
      {
         if (boardOutlineNameArray[i].CompareNoCase(layer->getName()) == 0)
            layer->setLayerType(LAYTYPE_BOARD_OUTLINE);
      }
   }

   return 1;
}

/******************************************************************************
* update_layerattr
*/
static int update_layerattr()
{
   // now attributes after mirror layers
   for (int j=0; j< doc->getMaxLayerIndex(); j++)
   {
      LayerStruct *lp = doc->getLayerArray()[j];
      if (lp == NULL)
			continue; // could have been deleted.

      for (int i=0; i<layer_attr_cnt; i++)
      {
         if (lp->getName().CompareNoCase(layer_attr[i].name) == 0)
         {
            lp->setLayerType(layer_attr[i].attr);
            break;
         }
      }
   }

   return 1;
}

/******************************************************************************
* GetNextLine
*/
static BOOL GetNextLine(CStdioFile *fMen, CString *fMenLine)
{
   fMenLineCount++;
   *fMenLine = "";

   CString temp;
   if (!fMen->ReadString(temp))
      return FALSE;

   temp.TrimLeft();
   temp.TrimRight();

   *fMenLine += temp;
   if (IsContinuationLine(*fMenLine))
      *fMenLine = fMenLine->Left(fMenLine->GetLength()-1);

   while (IsContinuationLine(temp))
   {
      fMenLineCount++;
      if (!fMen->ReadString(temp))
         return FALSE;

      temp.TrimLeft();
      temp.TrimRight();

      *fMenLine += temp;
      if (IsContinuationLine(*fMenLine))
         *fMenLine = fMenLine->Left(fMenLine->GetLength()-1);
   }

   return TRUE;
}

/******************************************************************************
* IsContinuationLine
*  - there can be a name ending in a - sign
*/
static BOOL IsContinuationLine(CString line)
{
   if (line.IsEmpty())
      return FALSE;
   
   int length = line.GetLength();
   if (line[length-1] == '-' && isspace(line[length-2]))
      return TRUE;

   return FALSE;
}

/******************************************************************************
* GetNextWord
*  - like strtok, lp1 is the first and NULL to continue
*/
static int get_nextword(char *lp1, char *token, int maxtok, char *attval, int maxatt)
{
   int      res = 0;
   int      cnt;

   if (lp1)
      curlp = lp1;

   while (*curlp && isspace(*curlp))   *curlp++;
   if(*curlp == '(')
   {
      // do until ), but watch out it is allowed in text
      cnt = 0;
      *curlp++;   // advance (
      while (*curlp && *curlp != ',')  
      {
         if (*curlp == '\\')  *curlp++;
         if(cnt < maxtok-1)
            token[cnt++] = *curlp;
         *curlp++;
      }
      if (*curlp) *curlp++;   // skip the closing braket
      token[cnt] = '\0';

      while (*curlp && isspace(*curlp))   *curlp++;
      if (*curlp == NULL)  return 0;

      // do until ), but watch out it is allowed in text
      cnt = 0;
      *curlp++;   // advance "
      while (*curlp && *curlp != '"')  
      {
         if (*curlp == '\\')  *curlp++;
         if(cnt < maxatt-1)
            attval[cnt++] = *curlp;
         *curlp++;
      }
      if (*curlp) *curlp++;   // skip the closing quote
      if (*curlp) *curlp++;   // skip the closing braket
      attval[cnt] = '\0';
      res = 2;
   }
   else
   if (*curlp == '\'')
   {
      // do until \'
      cnt = 0;
      *curlp++;   // advance 1 step
      while (*curlp && *curlp != '\'') 
      {
         if (*curlp == '\\')  *curlp++;         
         if(cnt < maxtok-1)
            token[cnt++] = *curlp;
         *curlp++;
      }
      if (*curlp) *curlp++;   // skip the endquote
      token[cnt] = '\0';
      res = 3;
   }
   else
   {
      // do until white space
      cnt = 0;
      while (*curlp && !isspace(*curlp))  
      {
         if(cnt < maxtok-1)
         token[cnt++] = *curlp;
         *curlp++;
         res = 1; // need to read min one char.
      }
      token[cnt] = '\0';
   }
   return res;
}

/****************************************************************************/
/*
   _ATTR  'key' 'value'

*/
static int get_nextattr(char *lp1,char *token,int maxtok, char *attval, int maxatt)
{
   int      res = 0;
   int      cnt;

   if (lp1)
      curlp = lp1;

   while (*curlp && isspace(*curlp))   *curlp++;
   if (*curlp == '\'')
   {
      // do until \'
      cnt = 0;
      *curlp++;   // advance 1 step
      while (*curlp && *curlp != '\'') 
      {
         if (*curlp == '\\')  *curlp++;         
         if(cnt < (maxtok-1))
            token[cnt++] = *curlp;
         *curlp++;
      }
      if (*curlp) *curlp++;   // skip the endquote
      token[cnt] = '\0';
      res = 3;
   }
   else
   {
      // do until white space
      cnt = 0;
      while (*curlp && !isspace(*curlp))  
      {
         if(cnt < maxtok-1)
         token[cnt++] = *curlp;
         *curlp++;
         res = 1; // need to read min one char.
      }
      token[cnt] = '\0';
   }
   return res;
}


/******************************************************************************
* cnv_tok 
*/
static double cnv_tok(char *tok)
{
   if (!tok)
      return 0.0;

   if (!strlen(tok))
     return 0.0;

   double value = atof(tok);

   // wrong out-of-range koos happen quite often on the Neutral file !!!
   value *= scaleFactor;
   double unitsFactor = Units_Factor(pageUnits, UNIT_INCHES);
   if (fabs(value * unitsFactor) > 99)
   {
      fprintf(fLog, "Coordinate error [%s] at %ld\n", tok, fMenLineCount);
      display_error++;
      value = 0.0;
   }

   return value;
}

/******************************************************************************
* cnv_addrot
*/
static void cnv_addrot(int mirrorRot, int *rot, int*mir)
{
   switch (mirrorRot)
   {
      case 1:  *rot = 0;   *mir = 0;   break;
      case 2:  *rot = 0;   *mir = 1;   break;
      case 4:  *rot = 180; *mir = 0;   break;
      case 3:  *rot = 180; *mir = 1;   break;
      case 7:  *rot = 270; *mir = 0;   break;
      case 5:  *rot = 270; *mir = 1;   break;
      case 6:  *rot = 90;  *mir = 0;   break;
      case 8:  *rot = 90;  *mir = 1;   break;
      case 9:  *rot = 135; *mir = 0;   break;
      case 13: *rot = 135; *mir = 1;   break;
      case 15: *rot = 225; *mir = 0;   break;
      case 11: *rot = 225; *mir = 1;   break;
      case 12: *rot = 135; *mir = 0;   break;
      case 16: *rot = 135; *mir = 1;   break;
      case 14: *rot = 45;  *mir = 0;   break;
      case 10: *rot = 45;  *mir = 1;   break;
      default: *rot = 0;   *mir = 0;   break;
   }
}

/******************************************************************************
* DoUnits
*/
static void DoUnits(CString unitName)
{
   CString unitName2 = unitName.Left(2);

   if (!unitName2.CompareNoCase("ML"))
      scaleFactor = Units_Factor(UNIT_MILS, pageUnits);
   else if (!unitName2.CompareNoCase("MI"))
      scaleFactor = Units_Factor(UNIT_MILS, pageUnits);
   else if (!unitName2.CompareNoCase("MM"))
      scaleFactor = Units_Factor(UNIT_MM, pageUnits);
   else if (!unitName2.CompareNoCase("TN"))
      scaleFactor = Units_Factor(UNIT_TNMETER, pageUnits);
   else if (!unitName2.CompareNoCase("IN"))
      scaleFactor = Units_Factor(UNIT_INCHES, pageUnits);
   else
   {
      CString err;
      err.Format("Unknown UNITS [%s] %ld", unitName, fMenLineCount);
      ErrorMessage(err, "Unit read error", MB_OK | MB_ICONHAND);
   }
}

/******************************************************************************
* is_command
*/
static int is_command(char *l, List *c_list, int siz_of_command)
{
   for (int i=0; i<siz_of_command; i++)
   {
      if (!STRCMPI(c_list[i].token, l))
         return i;
   }

   return -1;
}

/******************************************************************************
* GetNetname
*  - change /name to name
*  - change /xxx/yyy/name to name
*/
static CString GetNetname(char *string)
{
   CSupString line(string);
   CStringArray params;
   CString netname;

   int numPar = line.ParseQuote(params," ");

   if (numPar > 0)
   {
      netname = params.GetAt(0);
   }

   if (!hierachical_netnames)
   {
		int index = netname.ReverseFind('/');

		if (index > -1)
		{
			CString whatssecond = netname.Right(netname.GetLength()-index-1);
         CString whatsfirst = netname.Left(index+1);

         netname = whatssecond; // always

         // dts0100382477 - somebody with money thinks the net name looks nicer in a different order
         if (whatssecond.Find("N$") == 0 && whatsfirst.GetLength() > 1 /*want it only if more than just a slash*/)  
         {
            netname += "(" + whatsfirst + ")";
         }
		}
   }

   return netname;
}

/******************************************************************************
* IsUnnamedNet
*/
static BOOL IsUnnamedNet(CString netname)
{
   for (int i=0; i<unnamedNetCount; i++)
   {
      if (!STRNICMP(netname, unnamedNetArray[i], unnamedNetArray[i].GetLength()))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* get_drillindex
*/
static int get_drillindex(const char *pref, double size, int layernum, int unplated)
{
   NeutralDrill p;
   int         i;

   if (size == 0) return -1;

   for (i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size && p.unplated == unplated)
         return i;         
   }

   CString  name;
   name.Format("%s_%d",pref, drillcnt);

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

	p.d = size;
   p.unplated = unplated;
   drillarray.SetAtGrow(drillcnt,p);  
   drillcnt++;

   return drillcnt -1;
}

/******************************************************************************
* get_padformindex
*/
static int get_padformindex(int form, double sizeA, double sizeB) 
{
   for (int i=0;i < padFormArray.GetSize();i++)
   {
      NeutralPadform* padform = padFormArray.GetAt(i);

      if (padform != NULL &&
            padform->form == form &&
            padform->getSizeA() == sizeA &&
            padform->getSizeB() == sizeB)
      {
         return i;  
      }
   }

   CString  name;

   while (true)
   {
      name.Format("PADSHAPE_%d",padFormArray.GetSize());

      if (Graph_Block_Exists(doc, name ,neutralPcbFileNum))
      {
         padFormArray.Add(NULL);
      }
      else
      {
         break;
      }
   }

   int err;
   Graph_Aperture(neutralPcbFileNum, name, form, sizeA, sizeB, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);

   NeutralPadform* padform = new NeutralPadform();
   padform->form = form;
   padform->setSizeA(sizeA);
   padform->setSizeB(sizeB);

   padFormArray.Add(padform);  
   //padformcnt++;

   return padFormArray.GetSize() - 1;
}

/******************************************************************************
* SearchForXOUT
*/
static void SearchForXOUT(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (block->getFlags() & BL_WIDTH)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         BlockStruct *insertGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

         if (is_attrib(insertGeom->getName(), XOUTGEOM_ATTR))
            data->getInsert()->setInsertType(insertTypeXout);
      }
   }
}

/******************************************************************************
* is_attrib
*/
static int is_attrib(const char *name, int attrType)
{
   for (int i=0; i<attrArrayCount; i++)
   {
      if (attrArray[i].atttype == attrType && !STRICMP(attrArray[i].name, name))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* GetGeomToFiducial
*/
static int GetGeomToFiducial(CString geomName)
{
   for (int i=0; i<geomToFidCount; i++)
   {
      if (!geomToFidArray[i].geomName.CompareNoCase(geomName))
         return i;
   }

   return -1;
}

/******************************************************************************
* GetXOutGeom
*/
static int GetXOutGeom(CString geomName)
{
   for (int i=0; i<xOutGeomCount; i++)
   {
      if (!xOutGeomArray[i].geomName.CompareNoCase(geomName))
         return i;
   }

   return -1;
}

/******************************************************************************
* GetDefaultLayers
*/
static void GetDefaultLayers()
{
   int TLT, BLT;
   TLT = LAYTYPE_TOP;
   BLT = LAYTYPE_BOTTOM;
   CreateLayer("PAD", LAYTYPE_PAD_TOP, LAYTYPE_PAD_BOTTOM);
   for (int i=0; i<layer_attr_cnt; i++)
   {
      if (layer_attr[i].name.CompareNoCase("PLACE_1") == 0)
         TLT = layer_attr[i].attr;
	  else if (layer_attr[i].name.CompareNoCase("PLACE_2") == 0)
         BLT = layer_attr[i].attr;
   }
   CreateLayer("PLACE", TLT, BLT);
   CreateLayer("SILKSCREEN", LAYTYPE_SILK_TOP, LAYTYPE_SILK_BOTTOM);
}

/******************************************************************************
* CreateLayer
*/
static void CreateLayer(CString layerName, int topLayerType, int botLayerType)
{
   //// Create the generic layer
   //Graph_Level(layerName, "", FALSE);

   // Create the specific layer_1
	CString layerName_1 = layerName + "_1";
   int layerNum = Graph_Level(layerName_1, "", FALSE);
   LayerStruct *layer = doc->FindLayer(layerNum);
   layer->setFlagBits(LY_NEVERMIRROR);
	layer->setLayerType(topLayerType);

   // Create the specific layer_2
	CString layerName_2 = layerName + "_2";
   layerNum = Graph_Level(layerName_2, "", FALSE);
   layer = doc->FindLayer(layerNum);
   layer->setFlagBits(LY_MIRRORONLY);
	layer->setLayerType(botLayerType);
   
   // Create the Top & Bottom mirroring layers and set them to mirror each others
   CString layerTop = layerName + "_TOP";
   layerNum = Graph_Level(layerTop, "", FALSE);
   layer = doc->FindLayer(layerNum);
	layer->setLayerType(topLayerType);

   CString layerBot = layerName + "_BOT";
   layerNum = Graph_Level(layerBot, "", FALSE);
   layer = doc->FindLayer(layerNum);
	layer->setLayerType(botLayerType);

	Graph_Level_Mirror(layerTop, layerBot, "");

   // Add the layers to array
//	mentorLayersMap.SetAt(layerName, layerName);
	mentorLayersMap.SetAt(layerName_1, layerName_1);
	mentorLayersMap.SetAt(layerName_2, layerName_2);
	mentorLayersMap.SetAt(layerTop, layerTop);
	mentorLayersMap.SetAt(layerBot, layerBot);
}

/******************************************************************************
* IsGenericTopBottomMappingLayer
*/
static BOOL IsGenericTopBottomMappingLayer(CString layerName, CString &layerName_1, CString &layerName_2)
{
   layerName.MakeUpper();
   layerName.Trim();

   if (layerName == "PAD" || layerName == "PLACE" || layerName == "SILKSCREEN")
   {
      layerName_1 = layerName + "_1";
      layerName_2 = layerName + "_2";
      return TRUE;
   }
	else
	{
		layerName_1 = "";
		layerName_2 = "";
		return FALSE;
	}
}

/******************************************************************************
* IsSpecificTopBottomMappingLayer
*/
static BOOL IsSpecificTopBottomMappingLayer(CString layerName, CString &genericName, BOOL &isTop)
{
   layerName.MakeUpper();
   layerName.Trim();
   genericName = "";
   isTop = FALSE;

   if (layerName == "PAD_1"			|| layerName == "PAD_2" ||
       layerName == "PLACE_1"       || layerName == "PLACE_2" ||
       layerName == "SILKSCREEN_1"  || layerName == "SILKSCREEN_2")
   {
      int temp = layerName.ReverseFind('_');
      genericName = layerName.Left(temp);

      if (layerName.Right(2) == "_1")
         isTop = TRUE;
      else
         isTop = FALSE;

      return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* AddDataToLayer
*/
static int AddDataToLayer(CString layerName, CDataList *dataList, DataStruct *data, int dataType)
{
   NeutralData *neutralData = new NeutralData;
   neutralData->data = data;
   neutralData->dataList = dataList;
   neutralData->dataType = dataType;

   void *voidPtr = NULL;
   if (layerToDataMap.Lookup(layerName, voidPtr))
   {

      NeutralLayerData *layerData = (NeutralLayerData*)voidPtr;
      layerData->dataArray.SetAtGrow(layerData->dataCnt++, neutralData);
   }
   else
   {
      NeutralLayerData *layerData = new NeutralLayerData;
      layerToDataMap.SetAt(layerName, layerData);
      layerData->layerName = layerName;
      layerData->dataCnt = 0;
      layerData->dataArray.SetSize(20, 20);
      layerData->dataArray.SetAtGrow(layerData->dataCnt++, neutralData);
   }

   return 1;
}

void CheckForGeomInBoardstation(int neutralFileNum, int boardstationFileNum)
{
	if (boardstationFileNum == -1)
		return;

	FileStruct *neutralFile = doc->Find_File(neutralFileNum);
	if (neutralFile == NULL)
		return;

	FileStruct *boardstationFile = doc->Find_File(boardstationFileNum);
	if (boardstationFile == NULL)
		return;

	for (int i=0; i<doc->getMaxBlockIndex(); i++)
	{
		// Find the Neutral geom
		BlockStruct *neutralBlock = doc->getBlockAt(i);
		if (neutralBlock == NULL || neutralBlock->getFileNumber() != neutralFileNum ||
			 neutralBlock->getBlockType() == blockTypePcb || neutralBlock->getBlockType() == blockTypePanel)
			continue;

		// Skip replacing Neutral geom with Boardstation geom if the geom has moved pin
		CString newBlockName = "";
		if (pinEditedGeomMap.Lookup(neutralBlock->getName(), newBlockName))
			continue;

		// Find the Boardstation geom with the same name
		BlockStruct *stationBlock = Graph_Block_Exists(doc, neutralBlock->getName(), boardstationFileNum);
		if (stationBlock == NULL)
			continue;

		ReassignGeomToInsert(neutralBlock->getBlockNumber(), stationBlock->getBlockNumber());
		ReassignGeomToDeviceType(neutralBlock->getBlockNumber(), stationBlock->getBlockNumber());

		for (int k=0; k<doc->getWidthTable().GetCount(); k++)
		{
			BlockStruct *widthBlock = doc->getWidthTable()[k];
			if (widthBlock == neutralBlock)
            doc->getWidthTable().SetAt(k,stationBlock);
		}

		doc->RemoveBlock(neutralBlock);
	}
}

/******************************************************************************
* ReassignGeomToInsert
*/
void ReassignGeomToInsert(int oldBlockNum, int newBlockNum)
{
	for (int i=0; i<doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);
		if (block == NULL)
			continue;

		POSITION pos = block->getDataList().GetHeadPosition();
		while (pos)
		{
			DataStruct *data = block->getDataList().GetNext(pos);
			if (data == NULL || data->getDataType() != dataTypeInsert)
				continue;
			if (data->getInsert()->getBlockNumber() != oldBlockNum)
				continue;

			data->getInsert()->setBlockNumber(newBlockNum);
		}
	}
}

/******************************************************************************
* ReassignGeomToDeviceType
*/
void ReassignGeomToDeviceType(int oldBlockNum, int newBlockNum)
{
	POSITION pos = doc->getFileList().GetHeadPosition();
	while (pos)
	{
		FileStruct *file = doc->getFileList().GetNext(pos);

		POSITION typePos = file->getTypeList().GetHeadPosition();
		while (typePos)
		{
			TypeStruct *type = file->getTypeList().GetNext(typePos);		
			if (type == NULL)
				continue;

			if (type->getBlockNumber() == oldBlockNum)
				type->setBlockNumber( newBlockNum);
		}
	}
}

//****************************************************************************

CMneutTestPin::CMneutTestPin(CString refname, CString pinname, int testaccess, NetStruct *net)
: m_pinType(mneutTestPinTypeComppin)
, m_refname(refname)
, m_pinname(pinname)
, m_testAccessCode(testaccess)
, m_net(net)
, m_x(0.0) 
, m_y(0.0)
{
}

CMneutTestPin::CMneutTestPin(CString refname, double x, double y, int testaccess, NetStruct *net)
: m_pinType(mneutTestPinTypeVia)
, m_refname(refname)
, m_testAccessCode(testaccess)
, m_net(net)
, m_x(x) 
, m_y(y)
{
}

void CMneutTestPin::SetTestAttrib(CCEtoODBDoc *doc, CAttributes *attributes)
{
   int testKw = doc->getStandardAttributeKeywordIndex( standardAttributeTest );

   switch (this->GetTestAccessCode())
   {
   case ATT_TEST_ACCESS_ALL:
      doc->SetAttrib(&attributes, testKw, DFT_TESTACCESS_BOTH, attributeUpdateOverwrite, NULL);
      break;
   case ATT_TEST_ACCESS_TOP:
		doc->SetAttrib(&attributes, testKw, DFT_TESTACCESS_TOP, attributeUpdateOverwrite, NULL); 
      break;
   case ATT_TEST_ACCESS_BOT:
		doc->SetAttrib(&attributes, testKw, DFT_TESTACCESS_BOTTOM, attributeUpdateOverwrite, NULL); 
      break;
   }
}

void CMneutTestPinList::Add(CString refname, CString pinname, int testaccess, NetStruct *net)
{
   CMneutTestPin *testpin = new CMneutTestPin(refname, pinname, testaccess, net);
   this->AddTail(testpin);
}

void CMneutTestPinList::Add(CString refname, double x, double y, int testaccess, NetStruct *net)
{
   CMneutTestPin *testpin = new CMneutTestPin(refname, x, y, testaccess, net);
   this->AddTail(testpin);
}

void CMneutTestPinList::MarkTestPinsInCamcad(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   POSITION pos = this->GetHeadPosition();
   while (pos != NULL)
   {
      CMneutTestPin *testpin = this->GetNext(pos);

      if (testpin->GetTestPinType() == mneutTestPinTypeComppin)
      {
         NetStruct *net = NULL;
         CompPinStruct *cp = FindCompPin(pcbFile, testpin->GetRefname(), testpin->GetPinname(), &net);
         if (cp == NULL)
         {
            fprintf(fLog,"N_TEST PIN [%s] [%s] CompPin could not be found, can not set TEST attribute.\n", 
               testpin->GetRefname(), testpin->GetPinname());
            display_error++;
         }
         else
         {
            testpin->SetTestAttrib(doc, cp->getDefinedAttributes());
         }

      }
      else if (testpin->GetTestPinType() == mneutTestPinTypeVia)
      {
         WORD netKw = doc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);

         bool success = false;
         POSITION pos = boardFile->getBlock()->getHeadDataInsertPosition();
         while (pos != NULL)
         {
            DataStruct* data = boardFile->getBlock()->getNextDataInsert(pos);
            if (data->isInsertType(insertTypeVia) || data->isInsertType(insertTypeTestPad))
            {
               Attrib *attrib = NULL;
               if (data->lookUpAttrib(netKw, attrib))
               {
                  // Make sure the via or testpad belong to the current net
                  CString netName = attrib->getStringValue();
                  if (netName.CompareNoCase(testpin->GetNet()->getNetName()) == 0)
                  {
                     BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
                     CString blockName = block->getName();
                     CString vianame = testpin->GetRefname();

                     blockName.MakeUpper();
                     vianame.MakeUpper();
   
                     if (fabs(data->getInsert()->getOriginX() - testpin->GetX()) < TOL && 
                        fabs(data->getInsert()->getOriginY() - testpin->GetY()) < TOL && blockName.Find(vianame) > -1)
                     {
                        testpin->SetTestAttrib(doc, data->getDefinedAttributes());  
                        pos = NULL;  // finished
                        success = true;
                     }
                  }
               }
            }
         }
         if (!success)
         {
            fprintf(fLog,"N_TEST VIA at location [%lf, %lf] on net %s could not be found, can not set TEST attribute.\n", 
               testpin->GetX(), testpin->GetY(), testpin->GetNet()->getNetName());
            display_error++;
         }
      }
   }
}
