// $Header: /CAMCAD/5.0/read_wrt/PfwIn.cpp 67    2/27/07 1:58p Kurt Van Ness $

/****************************************************************************/
/*
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright ?1994-2001. All Rights Reserved.

*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"
#include "geomlib.h"
#include "pcbutil.h"
#include "lyrManip.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "pfwin.h"
#include "CCEtoODB.h"
#include "InFile.h"
#include "Consolidator.h"
#include "RwUiLib.h"
#include "PcbUtil.h"
#include "PcbUtil2.h"

// #define DEBUG_UNKNOWN_KEY

#define QPADBOT "PADBOT"
#define QPADTOP "PADTOP"

#define LineStatusUpdateInterval 200

#define QTempMarker "_$$temp$$_"
#define QNEXTLINE   " \t\r\n"
static CFileReadProgress*  fileReadProgress = NULL;

/* Static Variables *********************************************************/
static CCEtoODBDoc  		*doc;
static FileStruct  		*file;         // this is the board file
static BlockStruct 		*cur_block;    // this is the current block structure
static DataStruct  		*cur_data;     // this is the current block structure
static NetStruct   		*cur_net;      // this is the current netname struct

static FILE        		*fPfw;
static char        		fPfwLine[MAX_LINE];
//static long        		fPfwLineCount = 0;
static double      		scale_factor = 1;
static int         		page_unit;
static FILE					*logFp;
static int					display_error;

static int         		cur_filenum = 0;
static int         		cur_layer;
static int         		cur_entity;
static CString     		cur_name;
static CString     		cur_boardname;
static int					cur_geomlistptr;
static int					cur_pfw_version;  // current version 2.7

static CString				prim_boardoutline;
static CString				prim_compoutline;

static double      		origin_x, origin_y;  // needed in V3
static int         		freepadcnt = 0;
static int         		padlayercnt;      // this is used to derive electrical layer stacking
                   								// from the pad layers. Only valid on thru pad, because a thru pad 
                   								// goes from top to bottom
static PfwPoly     		*polyline;
static int         		polycnt;

static CPfwLayerArray   pfwLayerArray;
//static int         		layerscnt;

static PadformArray		padformArray;
static int					padformArrayCount;

static PFWPadstackArray pfwPadstackArray;

static DrillArray			drillArray;
static int					drillArrayCount;

static GeomArray			geomArray;
static int					geomArrayCount;

static FreePadArray		freepadArray;
static int					freepadArrayCount;

static NetArray			netArray;
static int					netArrayCount;

static PolyDataArray		polygonArray;
static int					polygonArrayCount;

static NetCompPinArray  netCompPinArray;
static int					netCompPinArrayCount;

static int					ComponentSMDrule;    // 0 = take existing ATT_SMD
															// 1 = if all pins of comp are marked as SMD
															// 2 = if most pins of comp are marked as SMD
															// 3 = if one pin of comp are marked as SMD

static CString				commentattribute;
static int					convert_freepad_comp;   // 0 do not convert, 1 convert to component 2 convert to via
static CString     		cur_compname;
static int         		cur_comppincnt;
static bool        		TRANSLATE_HATCHLINES = false;
static bool        		TRANSLATE_POLYGONS = true;
static bool        		TEXT_PROPORTIONAL = true;

double              glb_CPEV_val = 0.0;
double              glb_CSEV_val = 0.004;

//_____________________________________________________________________________
static int s_inputFileLineNumber = 0;

static int getInputFileLineNumber()
{
   return s_inputFileLineNumber;
}

static void resetInputFileLineNumber()
{
   s_inputFileLineNumber = 0;
}

static void incrementInputFileLineNumber()
{
   if (!getApp().m_readWriteJob.IsActivated())
   {
      if ((s_inputFileLineNumber % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
      {
         fileReadProgress->updateProgress();
      }
   }

   ++s_inputFileLineNumber;

   //if (s_inputFileLineNumber == 1674)
   //{
   //   int iii = 3;
   //}
}

//_____________________________________________________________________________
/* Function Prototypes *********************************************************/
static int pfw_pcb();
static int pfw_ct_skip();
static int pfw_cp_skip();
static int pfw_ca_skip();
static int update_layer(const CString& layerName,bool used);
static int get_drillindex(double size, int layernum);
static GraphicClassTag get_layerclass(const char *layername);
static int update_mirrorlayers();
static double get_v3unit(const char *l);
static int get_geomlistptr(int compid);
static int get_pfwheader(CString filename);
static void load_pfwsettings(CString settingsFile, CGeometryConsolidatorControl &consolControl, CTypeChanger &makeInto_CommandProcessor);
static void initMem();
static void freeMem();
static void do_netnames();
static void do_pfwin();
static void do_pfwV3in_1stPass();
static void do_pfwV3in();
#ifdef OBSOLETE
static void compare_pinloc();
#endif
static void doLayerMirroring();
static void place_v3components();
static int skip_to_endcomp(const char *geomname);

static int get_padformindex(double ax, double ay, int ashape, double topx, double topy, int topshape, double midx, double midy, int midshape, 
      double bottomx, double bottomy, int bottomshape, double drill, double soldermask_expansion, double pastemask_expansion, CString layer, int layerpair = -1);

static CString get_cczpadstackname(double ax, double ay, int ashape, double topx, double topy, int topshape, double midx, double midy, int midshape, 
      double bottomx, double bottomy, int bottomshape, double drill, double soldermask_expansion, double pastemask_expansion, CString layer, int layerpair = -1);

static void normalize_koo(double *x, double *y, int geomIndex);
static int unmirrorlayer(int lay);
static int get_v3padshape(CString shapeName);
static BOOL IsCompNameUsed(CString compName);
static int is_command(CString string, List *commandList, int commandListSize);
static int is_V3command(CString string, List *commandList, int commandListSize);
static bool get_keyval(CString string, CString *key, CString *val);
static double cnv_tok(char *tok);
static int add_netcomppin(int compindex, int pinindex);
static const char *get_netname(int nr);
static double normalize_rot(double r);
static int p_error();
static void CheckPinLocOrNameDiff();
static BOOL IsPinLocOrNameDiff(int geomIndex, int compareIndex);


/******************************************************************************
* ReadProtelPFW
*/
void ReadProtelPFW(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   display_error = FALSE;
   
   doc = Doc;
   file = NULL;
   cur_block = NULL;
   cur_data  = NULL;
   ComponentSMDrule = 0;
   resetInputFileLineNumber();

   CTypeChanger MakeInto_CommandProcessor;
   CGeometryConsolidatorControl consolidatorControl;

   // need to do this, because panel only knows the path.ext
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(path_buffer, drive, dir, fname, ext);

   CString filename = (CString)fname + ext;
  
   if ((fPfw = fopen(path_buffer, "rb")) == NULL)
   {
      ErrorMessage("Error open file", path_buffer);
      return;
   }

   if (!getApp().m_readWriteJob.IsActivated())
      fileReadProgress = new CFileReadProgress(fPfw);

   while (true)
   {
      // Open log file.
      CString localLogFilename;
      logFp = getApp().OpenOperationLogFile("protel.log", localLogFilename);
      if (logFp == NULL) // error message already issued, just return.
         break;
      WriteStandardExportLogHeader(logFp, "Protel");
      
      // return 1 is protel V2.8 text format 
      // return 2 is protel V3/4 advanced PCB format
      int pfwVersion = get_pfwheader(filename);

      if (pfwVersion < 0)
         break;

      // Protel units are in 1/1000 of mil.
      scale_factor = Units_Factor(UNIT_MILS, pageunits); // just to initialize
      page_unit = pageunits;
      origin_x = origin_y = 0;
      cur_geomlistptr = -1;

      initMem();

      file = Graph_File_Start(cur_boardname, fileTypeProtelLayout);
      file->setBlockType(blockTypePcb);
      file->getBlock()->setBlockType(file->getBlockType());

      // layers in settings file can not be loaded until the first file is open.
      CString settingsFile( getApp().getImportSettingsFilePath("pfw.in") );
      CString settingsFileMsg;
      settingsFileMsg.Format("\nPFW Reader: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(settingsFileMsg);
      load_pfwsettings(settingsFile, consolidatorControl, MakeInto_CommandProcessor);  // mirror layers must be done after file open.

      switch (pfwVersion)
      {
      case 3:
	   case 5:
		   do_pfwV3in_1stPass();
		   CheckPinLocOrNameDiff();
         do_pfwV3in();
         break;
      case 1:
         do_netnames();
         do_pfwin();
         break;
      }

      fclose(fPfw);

      NetStruct *n = add_net(file, NET_UNUSED_PINS);
      n->setFlagBits(NETFLAG_UNUSEDNET);

      // only need this once
      update_smdpads(doc);
      RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.

      update_smdrule_geometries(doc, ComponentSMDrule);
      update_smdrule_components(doc, file, ComponentSMDrule);

      // Apply MakeInto before generatePINLOC, it may change object types hence the PINLOC result.    
      MakeInto_CommandProcessor.Apply(doc, file);

      // generate pinloc
      generate_PINLOC(doc, file, TRUE);

      doLayerMirroring();

      doc->purgeUnusedBlocks();

      freeMem();

      // Close log file only if local log file  was opened. If it is the system log
      // then leave it be. If the local log file name is empty then the system
      // log file is in use.
      if (!localLogFilename.IsEmpty())
      {
         fclose(logFp);

         // We want this to happen only for local log files, not when
         // system log file is in play.
         if (display_error)
            Logreader(localLogFilename);
      }

      break;
   }
 
   doc->OnToolsNormalizeBottomBuildGeometries();

   if (consolidatorControl.GetIsEnabled())
   {
      CGeometryConsolidator squisher(doc, "");
      squisher.Go( &consolidatorControl );
   }

   if (!getApp().m_readWriteJob.IsActivated())
   {
	   delete fileReadProgress;
	   fileReadProgress = NULL;
   }
}

static void doLayerMirroring()
{
   for (int pfwLayerIndex = 0;pfwLayerIndex < pfwLayerArray.getSize();pfwLayerIndex++)
   {
      CPfwLayer* pfwLayer = pfwLayerArray.getAt(pfwLayerIndex);

      if (pfwLayer->getMirroredPfwLayerIndex() != pfwLayerIndex)
      {
         CPfwLayer* mirroredPfwLayer = pfwLayerArray.getAt(pfwLayer->getMirroredPfwLayerIndex());

         LayerStruct* layer         = doc->getLayerNoCase(pfwLayer->getName());
         LayerStruct* mirroredLayer = doc->getLayerNoCase(mirroredPfwLayer->getName());

         if (layer != NULL && mirroredLayer != NULL)
         {
            Graph_Level_Mirror(layer->getName(),mirroredLayer->getName());
         }
      }
   }
}

/******************************************************************************
* initMem
*/
static void initMem()
{
   if ((polyline = (PfwPoly*)calloc(MAX_POLY, sizeof(PfwPoly))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   //if ((layers = (PfwLayer*)calloc(MAX_LAYERS, sizeof(PfwLayer))) == NULL)
   //   MemErrorMessage(__FILE__, __LINE__);
   pfwLayerArray.empty();
   //layerscnt = 0;

   geomArray.SetSize(100,100);
   geomArrayCount = 0;

   freepadArray.SetSize(100,100);
   freepadArrayCount = 0;

   netArray.SetSize(100,100);
   netArrayCount = 0;

   polygonArray.SetSize(100,100);
   polygonArrayCount = 0;

   netCompPinArray.SetSize(100,100);
   netCompPinArrayCount = 0;

   drillArray.SetSize(100,100);
   drillArrayCount = 0;

   padformArray.SetSize(100,100);
   padformArrayCount = 0;
}

/******************************************************************************
* freeMem
*/
static void freeMem()
{
   //for (int i=0;i<layerscnt;i++)
   //   free(layers[i].name);
   //free(layers);
   pfwLayerArray.empty();

   free(polyline);

   padformArray.RemoveAll();  
	padformArrayCount = 0;

   pfwPadstackArray.empty(); 

   int i=0;

   for (i=0;i<geomArrayCount;i++)
	{
		PfwGeom *geom = geomArray[i];

		for (int j=0; j<geom->pinArrayCount; j++)
			delete geom->pinArray[j];
		geom->pinArray.RemoveAll();
		geom->pinArrayCount = 0;

		for (int j=0; j<geom->ctArray.GetCount(); j++)
			delete geom->ctArray[j];
		geom->ctArray.RemoveAll();

		for (int j=0; j<geom->caArray.GetCount(); j++)
			delete geom->caArray[j];
		geom->caArray.RemoveAll();

      delete geom;  
	}
   geomArray.RemoveAll();
   geomArrayCount = 0;
	

   for (i=0;i<freepadArrayCount;i++)
      delete freepadArray[i];  
   freepadArray.RemoveAll();
   freepadArrayCount = 0;

   for (i=0;i<netArrayCount;i++)
      delete netArray[i];  
   netArray.RemoveAll();
   netArrayCount = 0;

   for (i=0;i<polygonArrayCount;i++)
      delete polygonArray[i];  
   polygonArray.RemoveAll();
   polygonArrayCount = 0;

   for (i=0;i<netCompPinArrayCount;i++)
      delete netCompPinArray[i];  
   netCompPinArray.RemoveAll();
   netCompPinArrayCount = 0;

   drillArray.RemoveAll();
	drillArrayCount = 0;
}

/******************************************************************************
* get_pfwheader
*/
static int get_pfwheader(CString filename)
{
   rewind(fPfw);
   cur_boardname = filename;
   resetInputFileLineNumber();

   // check if this is the correct file format.
   if (fgets(fPfwLine, MAX_LINE, fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // check for Protel V3 and V4
   if (strstr(fPfwLine, "|KIND=Protel_Advanced_PCB|VERSION=3.00|"))
   {
      // this is a protel V3 file
      return 3;
   }
   if (strstr(fPfwLine, "|KIND=Protel_Advanced_PCB|VERSION=5.00|"))
   {
      // this is a protel V5 file
      return 5;
   }

   if (strstr(fPfwLine, "PCB") && strstr(fPfwLine, "Binary File"))
   {
      ErrorMessage("This is a Protel Advanced PCB Binary file!\nPlease \"Save As\" this file in Protel as PCB ASCII File and try again.",
          "Protel ASCII Format Error");
      fclose(fPfw);
      return -1;
   }

   char *lp;
   if ((lp = strtok(fPfwLine, QNEXTLINE)) == NULL)   
   {
      ErrorMessage("Not a Protel Advanced PCB ASCII format", "Protel ASCII Format Error");
      return -1;
   }

   if (!STRICMP(lp, "PCB"))
   {
      if (pfw_pcb() < 0)
      {
         ErrorMessage("Not a Protel Advanced PCB Text format",  "Protel ASCII Format Error");
         fclose(fPfw);
         return -1;
      }
   }
   else
   {
      ErrorMessage("This is a Protel Advanced PCB Binary file!\nPlease \"Save As\" this file in Protel as PCB ASCII File and try again.",
          "Protel ASCII Format Error");
      fclose(fPfw);
      return -1;
   }

   return 1;
}

/******************************************************************************
* load_pfwsettings
*/
static void load_pfwsettings(CString settingsFile, CGeometryConsolidatorControl &consolControl, CTypeChanger &makeInto_CommandProcessor)
{
   prim_boardoutline.Empty();
   prim_compoutline.Empty();
   commentattribute     = ATT_TYPELISTLINK;
   convert_freepad_comp = 0;
   TRANSLATE_HATCHLINES = true;
   TRANSLATE_POLYGONS   = true;
   TEXT_PROPORTIONAL    = true;
   consolControl.SetIsEnabled(true); // Reset to defaul=enabled
   makeInto_CommandProcessor.Reset();

   CInFile inFile;

   if (! inFile.open(settingsFile))
   {
      CString err;
      err.Format("File [%s] not found", settingsFile);
      ErrorMessage(err, "Protel Settings", MB_OK | MB_ICONHAND);
      return;
   }

   while (inFile.getNextCommandLine())
   {
      CString command = inFile.getCommand();

      if (inFile.getParamCount() > 1)
      {
         if (inFile.isCommand(".ComponentSMDrule",2))
         {
            ComponentSMDrule = inFile.getIntParam(1);
         }
         else if (inFile.isCommand(".Commentattribute",2))
         {
            commentattribute = inFile.getParam(1);
         }
         else if (inFile.isCommand(".Convert_freepad_comp",2))
         {
            CString param = inFile.getParam(1).Left(1).MakeLower();

            if (param == "y")
            {
               convert_freepad_comp = 1;
            }
            else if (param == "v")
            {
               convert_freepad_comp = 2;
            }
         }
         else if (inFile.isCommand(".Text_Proportional",2))
         {
            inFile.parseYesNoParam(1,TEXT_PROPORTIONAL,true);
         }
         else if (inFile.isCommand(".TRANSLATE_POLYGONS",2))
         {
            inFile.parseYesNoParam(1,TRANSLATE_POLYGONS,true);
         }
         else if (inFile.isCommand(".TRANSLATE_HATCH",2))
         {
            inFile.parseYesNoParam(1,TRANSLATE_HATCHLINES,true);
         }
         else if (inFile.isCommand(".BOARDOUTLINE",2))
         {
            prim_boardoutline = inFile.getParam(1);
         }
         else if (inFile.isCommand(".COMPOUTLINE",2))
         {
            prim_compoutline = inFile.getParam(1);
         }
         else if (inFile.isCommand(".MIRRORLAYER",3))
         {
            int layerIndex1 = update_layer(inFile.getParam(1),false);
            int layerIndex2 = update_layer(inFile.getParam(2),false);
            pfwLayerArray.mirrorLayers(layerIndex1,layerIndex2);
         }
         else if (consolControl.Parse( inFile.getCommandLineString() ))
         {
            // Nothing more to do, Parse() already did it if it was a Consolidator command.
         }
         else if (makeInto_CommandProcessor.ParseCommand( inFile.getCommandLineString() ))
         {
            // Nothing more to do, ParseCommand() already did it if it was a Make_Into command.
         }
         else
         {
            fprintf(logFp, "Unrecognized command line encountered, '%s', while reading '%s'\n",
               inFile.getCommandLineString(),inFile.getFilePath());
            display_error++;
         }
      }
   }

   if (!TRANSLATE_POLYGONS)
   {
      fprintf(logFp, "Polygons will not be read. (pfw.in)\n");
      display_error++;
   }

   if (!TRANSLATE_HATCHLINES)
   {
      fprintf(logFp, "Polygon hatch lines will not be read. (pfw.in)\n");
      display_error++;
   }
}

/******************************************************************************
* do_netnames
*/
static void do_netnames()
{
   int netcnt = 0;
   BOOL IsCompPin = FALSE;
   
   while (fgets(fPfwLine, MAX_LINE, fPfw))
   {
      incrementInputFileLineNumber();

      char *tok = strtok(fPfwLine,QNEXTLINE);
      
      if (!tok)
         continue;

      if (!STRCMPI(tok, "("))
      {
         IsCompPin = TRUE;
         continue;
      }
      else if (!STRCMPI(tok, ")"))
      {
         IsCompPin = FALSE;
         continue;
      }
      
      if (IsCompPin)
      {
         /* - Joanne;  Don't need to generate netCompPinArray because we are not using net list generated from netDEF.  This was causing a 
         problem since NETDEF keeps the previous pin assignment.
         int compIndex = atoi(tok);

         if ((tok = strtok(NULL,QNEXTLINE)) == NULL) continue;
         int pinIndex = atoi(tok);

         // if a compindex pinindex is in 2 places (2 nets), it seems the last one wins.
         // we however look for the first one !
         int netCompPinIndex = add_netcomppin(compIndex, pinIndex);
         netCompPinArray[netCompPinIndex]->netnr = netArrayCount;
         */
      }

      // do not do the default section. All done here.
      if (!STRCMPI(tok, "DEFAULTS"))
         break;

      if (!STRCMPI(tok, "ENDPCB"))
         break;

      if (!STRCMPI(tok, "NETDEF"))
      {
         netcnt++;

         if (fgets(fPfwLine, MAX_LINE, fPfw))
         {
            incrementInputFileLineNumber();
            CString tmp = fPfwLine;
            tmp.TrimLeft();
            tmp.TrimRight();

            PfwNet *net = new PfwNet;
            netArray.SetAtGrow(netArrayCount++, net);
            net->netname = tmp;
            net->netnr = netcnt;
         }
      }
   }
}

/******************************************************************************
* do_pfwin
*/
static void do_pfwin()
{
   rewind(fPfw);
   resetInputFileLineNumber();

   char *lp;
   int code;
   CString tmp;

   // here do a while loop
   while (fgets(fPfwLine, MAX_LINE, fPfw))
   {
      incrementInputFileLineNumber();
      tmp = fPfwLine;

      if ((lp = strtok(fPfwLine, QNEXTLINE)) == NULL)
         continue;

      // do not do the default section. All done here.
      if (!STRCMPI(lp, "DEFAULTS"))
         break;
      if (!STRCMPI(lp, "ENDPCB"))
         break;

      if ((code = is_command(lp, command_lst, SIZ_OF_COMMAND)) < 0)
      {
         fprintf(logFp, "Unknown PROTEL Token [%s] at %ld\n", lp, getInputFileLineNumber());
         display_error++;
      }
      else
      {
         (*command_lst[code].function)();
      }
   }

   update_mirrorlayers();
}

/******************************************************************************
* do_pfwV3in_1stPass
*/
static void do_pfwV3in_1stPass()
{
   rewind(fPfw);
   resetInputFileLineNumber();

   // here do a while loop
   while (fgets(fPfwLine, MAX_LINE, fPfw))
   {
      incrementInputFileLineNumber();

      CString tmp = fPfwLine;
      tmp.TrimLeft();
      tmp.TrimRight();
      strcpy(fPfwLine, tmp);

      char *string = get_string(fPfwLine, "|");
      if (!string)
         continue;

      int commandCode = is_V3command(string, commandv3_1stPass_lst, SIZ_OF_COMMANDV3_1stPASS);
      if (commandCode >= 0)
         (*commandv3_1stPass_lst[commandCode].function)();
   }
}

/******************************************************************************
* do_pfwV3in
*/
static void do_pfwV3in()
{
   rewind(fPfw);
   resetInputFileLineNumber();

   // here do a while loop
   while (fgets(fPfwLine, MAX_LINE, fPfw))
   {
      incrementInputFileLineNumber();

      CString tmp = fPfwLine;
      tmp.TrimLeft();
      tmp.TrimRight();
      strcpy(fPfwLine, tmp);

      char *string = get_string(fPfwLine, "|");
      if (!string)
         continue;

      int commandCode = is_V3command(string, commandv3_lst, SIZ_OF_COMMANDV3);
      if (commandCode >= 0)
         (*commandv3_lst[commandCode].function)();
      else
      {
#ifdef _DEBUG
         fprintf(logFp,"DEBUG: Unknown PROTEL Token [%s] at %ld\n", string, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

   place_v3components();
   update_mirrorlayers();
}

/******************************************************************************
* place_v3components
*/
static void place_v3components()
{
   for (int i=0; i<geomArrayCount; i++)
   {
      PfwGeom *geom = geomArray[i];

      double rotation = geom->rotation;
      CString refDes = geom->compdes;
      if (!strlen(refDes))
         refDes.Format("COMP_%d", geom->compid);

      cur_block = Graph_Block_On(GBO_APPEND, geom->shapename, -1, 0);
      Graph_Block_Off();
		cur_data = Graph_Block_Reference(geom->shapename, refDes, 0, geom->centerx, geom->centery, DegToRad(rotation), geom->mirror, 1.0, -1, TRUE);

      if (!geom->desfound)
      {
         cur_data->getInsert()->setInsertType(insertTypeUnknown);
         cur_block->setBlockType(BLOCKTYPE_UNKNOWN);
         continue;
      }
		else
		{
			double textAngle = geom->desrot;
			if (geom->mirror)
				textAngle = 360.0 - textAngle;

			cur_data->getInsert()->setInsertType(default_inserttype(cur_block->getBlockType()));
			doc->SetVisAttrib(&cur_data->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, refDes.GetBuffer(0),
					geom->desx, geom->desy, DegToRad(textAngle), geom->desheight,
					geom->desheight*6.0/8.8, 1, 0, 1, SA_OVERWRITE, 0L, geom->deslindex, 0, 0, 0);

			if (geom->commentfound)
			{
				double commentAngle = geom->commentrot;
				if (geom->mirror)
					commentAngle = 360.0 - commentAngle;

				doc->SetVisAttrib(&cur_data->getAttributesRef(), doc->RegisterKeyWord("COMMENT", 0, valueTypeString), valueTypeString,
						geom->comment.GetBuffer(0), geom->commentx, geom->commenty, DegToRad(commentAngle), geom->commentheight,
						geom->commentheight*6.0/8.8, 1, 0, 1, SA_OVERWRITE, 0L, geom->commentlindex, 0, 0, 0);

				CString partnumber = geom->comment;
				if (cur_data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT && strlen(partnumber))
				{
					TypeStruct *t = AddType(file, partnumber);
					if (t->getBlockNumber() < 0)
						t->setBlockNumber( cur_data->getInsert()->getBlockNumber());
					else if (t->getBlockNumber() != cur_data->getInsert()->getBlockNumber())
					{
						BlockStruct *b1 = doc->Find_Block_by_Num(t->getBlockNumber());
						fprintf(logFp, "Component [%s] %s [%s] Shape [%s] has already a different Shape [%s] assigned!\n",
								refDes, ATT_TYPELISTLINK, partnumber, geom->shapename, b1->getName());
						display_error++;

						CString tmp = "";
						tmp.Format("%s_%s", geom->shapename, partnumber);
						partnumber = tmp;
						TypeStruct *t = AddType(file, partnumber);
						if (t->getBlockNumber() < 0)
							t->setBlockNumber( cur_data->getInsert()->getBlockNumber());
					}
					doc->SetUnknownAttrib(&cur_data->getAttributesRef(), ATT_TYPELISTLINK, partnumber, SA_OVERWRITE, NULL);
				} 
				else // if a device link
				{
					CString tname;
					tname.Format("%s_%s", geom->shapename, geom->comment);
					doc->SetUnknownAttrib(&cur_data->getAttributesRef(), ATT_TYPELISTLINK, tname, SA_OVERWRITE, NULL);
					TypeStruct *t = AddType(file, tname);
					if (t->getBlockNumber() < 0)
						t->setBlockNumber( cur_data->getInsert()->getBlockNumber());
				}
			}
		}

		for (int j=0; j<geom->pinArrayCount; j++)
		{
			PfwPin *pin = geom->pinArray[j];

			// here update netlist if it is a electrical pin 
			if (strlen(pin->pinname) && pin->pinname.Compare("0")) // if not mechanical
			{
				CString cur_netname = NET_UNUSED_PINS;
				if (pin->netid >= 0)
					cur_netname = get_netname(pin->netid);

				NetStruct *n = add_net(file, cur_netname);
				if (strlen(geomArray[pin->geomindex]->compdes))
				{
					if (test_add_comppin(geomArray[pin->geomindex]->compdes, pin->pinname, file) == NULL)
					{
						CompPinStruct *cp = add_comppin(file, n, geomArray[pin->geomindex]->compdes, pin->pinname);
						pin->insertType = INSERTTYPE_PIN;
						CString val((pin->addTestPointAttrTop && pin->addTestPointAttrBottom) ? "BOTH" : (pin->addTestPointAttrTop) ? "TOP" : (pin->addTestPointAttrBottom) ? "BOTTOM" : "" );
						if (!val.IsEmpty())
						   doc->SetUnknownAttrib(&cp->getAttributesRef(),"TEST",val.GetBuffer(0), attributeUpdateOverwrite, NULL); 
					}
					else
					{
						fprintf(logFp, "Duplicated pads [%s] in Component [%s]\n", pin->pinname, geomArray[pin->geomindex]->compdes); 
						display_error++;
						pin->insertType = -1;
					}
				}
			}
		}
   }
}

/******************************************************************************
*/

static void AddPartType(CString blockName, CString partNumber, CString refDes)
{
   if (commentattribute.CompareNoCase(ATT_TYPELISTLINK) == 0)
   {
      TypeStruct *type = AddType(file, partNumber);

      if (type->getBlockNumber() < 0)
         type->setBlockNumber( cur_data->getInsert()->getBlockNumber());
      else
      {
         if (type->getBlockNumber() != cur_data->getInsert()->getBlockNumber())
         {
            BlockStruct *b1 = doc->Find_Block_by_Num(type->getBlockNumber());
            fprintf(logFp, "Component [%s] %s [%s] Shape [%s] has already a different Shape [%s] assigned!\n",
               refDes, ATT_TYPELISTLINK, partNumber, blockName, b1->getName());
            display_error++;

            CString tmp;
            tmp.Format("%s_%s", blockName, partNumber);
            partNumber = tmp;

            TypeStruct *type = AddType(file, partNumber);
            if (type->getBlockNumber() < 0)
               type->setBlockNumber( cur_data->getInsert()->getBlockNumber());
         }
      }
   }
   else
   {
      CString typeName;
      typeName.Format("%s_%s", blockName, partNumber);
      doc->SetUnknownAttrib(&cur_data->getAttributesRef(), ATT_TYPELISTLINK, typeName, SA_OVERWRITE, NULL);
      TypeStruct *type = AddType(file, typeName);
      if (type->getBlockNumber() < 0)
         type->setBlockNumber( cur_data->getInsert()->getBlockNumber());
   }
}

/******************************************************************************
* pfw_comp
*  - partnumber is really a comment line.
*/
static int pfw_comp()
{
   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   char *tok;
   CString blockName;
   if ((tok = strtok(fPfwLine, QNEXTLINE)) != NULL)  
	{
      blockName = tok;
		blockName.MakeUpper();
	}

   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // shape data
   if ((tok = strtok(fPfwLine, QNEXTLINE)) == NULL)  return p_error(); // Sel
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // Drc
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // refx
   double x = cnv_tok(tok);
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // refy
   double y = cnv_tok(tok);
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // des hide
   int desVisible = atoi(tok);
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // comment hide
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // not used
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // xmin
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // ymin
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // xmax
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // ymax
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // rot in degrees
   double rotation = normalize_rot(atof(tok));
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // locked
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // prim locked

   // get CS for comp des
   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // get data for reference CS
   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // data for ref data
   if ((tok = strtok(fPfwLine,QNEXTLINE)) == NULL)   return p_error(); // Sel
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // Drc
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // refx
   double desX = cnv_tok(tok);
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // refy
   double desY = cnv_tok(tok);
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // height
   double desHeight = cnv_tok(tok);
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // rot
   double desRot = atof(tok);
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // orient
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // font
   if ((tok = strtok(NULL, QNEXTLINE)) == NULL)      return p_error(); // layer, this determines if a component is mirror or not.
   int desLayer = atoi(tok);

   int mirror;
   if (desLayer & 1)
      mirror = 0;
   else
   {
      mirror = 1;
      rotation = rotation + 180;
      // here norm alize deslay
      desLayer = unmirrorlayer(desLayer); // 17 is top, 18 is bottom etc...
   }
	rotation = normalize_rot(rotation);

   // here is compdes 
   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   CString refDes;
   if (tok = strtok(fPfwLine, QNEXTLINE))
      refDes = tok;

   while (IsCompNameUsed(refDes))
   {
      fprintf(logFp, "Component name [%s] already used !\n", refDes);
      display_error++;
      refDes.Format("$CMP_%d", ++freepadcnt);
   }
   cur_compname = refDes;
   cur_comppincnt = 0;

   if (blockName.IsEmpty())
   {
      if (!refDes.IsEmpty())
      {
         fprintf(logFp, "Component [%s] has no FootPrint.\n", refDes);
         display_error++;
         blockName.Format("NOFOOTPRINT_%s", refDes);
      }
      else
      {
         fprintf(logFp, "Component at line %ld has no FootPrint and no Designator.\n", getInputFileLineNumber());
         display_error++;
         blockName.Format("NOFOOTPRINT_%d", geomArrayCount+1);
      }
   }
   
   // get CS comment
   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // get comment data 
   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // get comment text
   if (!fgets(fPfwLine, MAX_LINE, fPfw))
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   CString partNumber;
   if ((tok = strtok(fPfwLine,QNEXTLINE)) != NULL)   
      partNumber = tok;

   PfwGeom *geom = new PfwGeom;
   geomArray.SetAtGrow(geomArrayCount++, geom);
   geom->compdata = NULL; // will be ptr to this comp's data once it gets made
   geom->shapename = blockName + QTempMarker + refDes;
   geom->compdes = refDes;
   geom->partnumber = partNumber;
	geom->compid = -1;
   geom->centerx = x;
   geom->centery = y;
   geom->rotation = rotation;
   geom->mirror = mirror;
   geom->master = 0;
   geom->desfound = FALSE;
   geom->commentfound = FALSE;
	geom->pinArray.SetSize(0, 20);
	geom->pinArrayCount = 0;
	geom->caArray.SetSize(0, 20);
	geom->ctArray.SetSize(0, 20);

   // need to close for placement
   // check here if comp exists. If yes skip to endcomp
   int already_exist = FALSE;
   if (Graph_Block_Exists(doc, geom->shapename, -1))
   {
      cur_block = Graph_Block_On(GBO_APPEND, geom->shapename, -1, 0);
      Graph_Block_Off();
      already_exist = TRUE;  
   }
   else
   {
      geom->master = 1; 
      cur_block = Graph_Block_On(GBO_APPEND, geom->shapename, -1, 0);
      cur_block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
      Graph_Block_Off();
   }
   cur_data = Graph_Block_Reference(geom->shapename, refDes, 0, x, y, DegToRad(1.0 * rotation), mirror, 1.0, -1, TRUE);
   geom->compdata = cur_data;

   if (!refDes.IsEmpty())
   {
      cur_data->getInsert()->setInsertType(default_inserttype(cur_block->getBlockType()));
      // here normalize refdes koos.
      //desx = desx - x;
      //desy = desy - y;
      CString desLayerName;
      desLayerName = protel_layer[desLayer];

      normalize_koo(&desX, &desY, geomArrayCount-1);

		double textAngle = desRot - rotation;
		if (geom->mirror)
			textAngle = 360.0 - textAngle;

      doc->SetVisAttrib(&cur_data->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, refDes.GetBuffer(0),
            desX, desY, DegToRad(textAngle), desHeight, desHeight*6.0/8.8, 1, 0, desVisible, SA_APPEND,  0L,   
            Graph_Level(desLayerName, "", 0), 0, 0, 0);
   }
   else
   {
      // no compdes name !
      cur_data->getInsert()->setInsertType(insertTypeUnknown);
      cur_block->setBlockType(BLOCKTYPE_UNKNOWN);
   }

   if (!partNumber.IsEmpty())
      doc->SetUnknownAttrib(&cur_data->getAttributesRef(), commentattribute, partNumber, SA_OVERWRITE, NULL);

   if (already_exist)
   {
      skip_to_endcomp(geom->shapename);
      return 0;
   }

   // reopen
   cur_block = Graph_Block_On(GBO_APPEND, geom->shapename, -1, 0);

   return 1;
}


/******************************************************************************
* pfwv3_component
	- This function will create the geometry from geom->shapename that was assigned
	  in the first pass
*/
static int pfwv3_component()
{
   int compID = -1;
   char *string;
   while (string = get_string(NULL, "|"))
   {
      CString val, key;
      get_keyval(string, &key, &val);

      // Only need the compID here since everything is already done in pfwv3_component_1stPass()
      if (!key.CompareNoCase("ID"))
      {
         compID = atoi(val);
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Component at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }
   
	if (compID > -1)
	{
      int geomIndex = get_geomlistptr(compID);
      PfwGeom *geom = geomArray[geomIndex];

		if (geom->master == 1)
		{
			cur_block = Graph_Block_On(GBO_APPEND, geom->shapename, -1, 0);
			cur_block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
			Graph_Block_Off();
		}	
	}

   return 1;
}

/******************************************************************************
* pfwv3_component_1stPass
	- This function will gather the information. The geom will be built in pfwv3_component()
*/
static int pfwv3_component_1stPass()
{
   int compID = -1;
   CString pattern = "";
   double x = 0.0;
	double y = 0.0;
	double rotation = 0.0;
   int mirror = 0;

   char *string;
   while (string = get_string(NULL, "|"))
   {
      CString val, key;
      get_keyval(string, &key, &val);   

      if (!key.CompareNoCase("ID"))
         compID = atoi(val);
      else if (!key.CompareNoCase("LAYER"))
      {
         if (val.CompareNoCase("TOP"))
            mirror = 1;
      }
      else if (!key.CompareNoCase("PATTERN"))
         pattern = val;
      else if (!key.CompareNoCase("X"))
         x = get_v3unit(val) - origin_x;
      else if (!key.CompareNoCase("Y"))
         y = get_v3unit(val) - origin_y;
      else if (!key.CompareNoCase("ROTATION"))
         rotation = atof(val);
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=COmponent at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

   if (pattern.IsEmpty())
   {
      fprintf(logFp, "Component at line %ld has no Pattern.\n", getInputFileLineNumber());
      display_error++;
      pattern.Format("NOFOOTPRINT_%d", compID);
   }

	if (mirror)
		rotation -= 180;
	rotation = normalize_rot(rotation);

   PfwGeom *geom = new PfwGeom;
   geomArray.SetAtGrow(geomArrayCount++, geom);
   geom->compdata = NULL; // not used for these versions of PFWIN formats
   geom->shapename = pattern;
   geom->compid = compID;
   geom->partnumber = "";
   geom->compdes = "";
   geom->centerx = x;
   geom->centery = y;
   geom->rotation = rotation;
   geom->mirror = mirror;
   geom->master = -1;
   geom->desfound = FALSE;
	geom->commentfound = FALSE;
	geom->pinArray.SetSize(0, 20);
	geom->pinArrayCount = 0;
	geom->caArray.SetSize(0, 20);
	geom->ctArray.SetSize(0, 20);

	return 1;
}

/******************************************************************************
* pfwv3_pad_1stPass

*/
static int pfwv3_pad_1stPass()
{
   int netID = -1;
   int compID = -1;
   CString pinName = "";
   double x = 0.0;
	double y = 0.0;
	double rot = 0.0;
	//case 1015: added the CPEV & CSEV commad reading
	
	int    cpev = 1;
	int    csev = 1;
	double cpe = 1;
	double cse = 1;
   bool addTestPointAttrTop = false;
   bool addTestPointAttrBottom = false;
   char *string;
   while (string = get_string(NULL, "|"))
   {
      CString key, val;
      if (get_keyval(string, &key, &val))
      {
         if (!key.CompareNoCase("NET"))
            netID = atoi(val);
         else if (!key.CompareNoCase("COMPONENT"))
            compID = atoi(val);
         else if (!key.CompareNoCase("X"))
            x = get_v3unit(val) - origin_x;
         else if (!key.CompareNoCase("Y"))
            y = get_v3unit(val) - origin_y;
         else if (!key.CompareNoCase("NAME"))
            pinName = val;
         else if (!key.CompareNoCase("ROTATION"))
            rot = normalize_rot(atof(val));
         else if (!key.CompareNoCase("CPEV"))
            cpev = atoi(val);
         else if (!key.CompareNoCase("CSEV"))
            csev = atoi(val);
         else if (!key.CompareNoCase("CPE"))
         {
            cpe = get_v3unit(val);
         }
         else if (!key.CompareNoCase("CSE"))
         {
            cse = get_v3unit(val);
         }
         else if (!key.CompareNoCase("TESTPOINT_TOP"))
         {
            addTestPointAttrTop = true;
         }
         else if (!key.CompareNoCase("TESTPOINT_BOTTOM"))
         {
            addTestPointAttrBottom = true;
         }
         else
         {
#ifdef DEBUG_UNKNOWN_KEY
            fprintf(logFp,"Unknown Key [%s] in RECORD=PAD at %ld\n", key, getInputFileLineNumber());
            display_error++;
#endif
         }
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=PAD at %ld\n", string, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

	//case 1015: if the rule is set to 1, use global (defined as glb_CSEV_val), o/w use local cpe/cse
	//the local one is read above in the data line of thi pad.
	if (csev == 1)    
		cse = glb_CSEV_val;


	if (cpev == 1)      
		cpe = glb_CPEV_val;

	if (compID > -1)
	{
      int geomIndex = get_geomlistptr(compID);
      if (geomIndex < 0)
      {
         ErrorMessage("Geom Index???", "Record=PAD");
         return 0;
      }

      PfwGeom *geom = geomArray[geomIndex];
      PfwPin *pin = new PfwPin;
		geom->pinArray.SetAtGrow(geom->pinArrayCount++, pin);
      pin->geomindex = geomIndex;
      pin->pinnr = geom->pinArrayCount;
		pin->netid = netID;
      pin->pinx = x;
      pin->piny = y;
		pin->pinrot = rot;
      pin->pinname = pinName;
      pin->insertType = -1;
		pin->cpe = cpe;
		pin->cse = cse;
      pin->addTestPointAttrBottom = addTestPointAttrBottom;
      pin->addTestPointAttrTop = addTestPointAttrTop;
	}

	return 1;
}

/******************************************************************************
* pfwv3_text_1stPass
*/
static int pfwv3_text_1stPass()
{
   int compID = -1;
	CString text = "";
	BOOL designator = FALSE;

	char *string;
	while (string = get_string(NULL, "|"))
   {
      CString key, val;
      get_keyval(string, &key, &val); 

		if (!key.CompareNoCase("COMPONENT"))
      {
         compID = atoi(val);
      }
      else if (!key.CompareNoCase("TEXT"))
      {
         text = val;
      }
      else if (!key.CompareNoCase("DESIGNATOR"))
      {
         if (val.CompareNoCase("TRUE") == 0)
            designator = TRUE;
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=TEXT at %ld\n", string, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

	if (compID > -1 && designator)
	{
      int geomIndex = get_geomlistptr(compID);
      if (geomIndex < 0)
      {
         ErrorMessage("Geom Index???", "Record=TEXT");
         return 0;
      }

		PfwGeom *geom = geomArray[geomIndex];
      geom->desfound = TRUE;
      geom->compdes = text;
	}

	return 1;
}

/******************************************************************************
* CheckPinLocOrNameDiff
	
	- This function will check to see if a geometry definetion has pin locations
	  or pin name modified.  If yes, then create a unique geometry
*/
static void CheckPinLocOrNameDiff()
{
	CMapStringToString shapeNameMap;

	for (int i=0; i<geomArrayCount; i++)
	{
		PfwGeom *geom = geomArray[i];
		
		// not -1 means it is already checked
		if (geom->master != -1)
			continue;
		geom->master = 1;

		CString shapename = geom->shapename;
		if (shapeNameMap.Lookup(shapename, shapename))
		{
			// the same shape name already used
			if (geom->compdes != "")
            shapename.Format("%s_%s", geom->shapename, geom->compdes);
			else
            shapename.Format("%s_%d", geom->shapename, geom->compid);
		}
		else
		{
			// add the shape name to the map
			shapeNameMap.SetAt(shapename, shapename);
		}

		for (int j=i+1; j<geomArrayCount; j++)
		{
			PfwGeom *compareGeom = geomArray[j];

			// not -1 means it is already checked
			if (compareGeom->master != -1) 
				continue;

			// Not of the same shapename, skip
			if (compareGeom->shapename.CompareNoCase(geom->shapename))
				continue;

			// Pin location is different, skip
			if (IsPinLocOrNameDiff(i, j))
				continue;

			// compareGeom is the same as geom 
			compareGeom->master = 0;
			compareGeom->shapename = shapename;						
		}

		// Set shapename to new name in case it is already existed
		geom->shapename = shapename;
	}
}

/******************************************************************************
* IsPinLocOrNameDiff
*/
static BOOL IsPinLocOrNameDiff(int geomIndex, int compareIndex)
{
	const double oneMil = cnv_tok("1000");	// PFW in 1000th mil
	PfwGeom *geom = geomArray[geomIndex];
	PfwGeom *compareGeom = geomArray[compareIndex];

	for (int i=0; i<geom->pinArrayCount; i++)
	{
		PfwPin *pin = geom->pinArray[i];
		BOOL diffLoc = FALSE;
		BOOL diffName = TRUE;

		double rot = geom->rotation - pin->pinrot;
		if (geom->mirror)
			rot -= 180;
		rot = normalize_rot(rot);

		double x = pin->pinx;
		double y = pin->piny;
		normalize_koo(&x, &y, geomIndex);

		for (int j=0; j<compareGeom->pinArrayCount; j++)
		{
			PfwPin *comparePin = compareGeom->pinArray[j];
			if (!comparePin->pinname.CompareNoCase(pin->pinname))
				diffName = FALSE;
			else	
				continue;  // pin name is not the same, so skip

			double compareRot = compareGeom->rotation - comparePin->pinrot;
			if (compareGeom->mirror)
				compareRot -= 180;
			compareRot = normalize_rot(compareRot);

			double compareX = comparePin->pinx;
			double compareY = comparePin->piny;
			normalize_koo(&compareX, &compareY, compareIndex);

			if ((fabs(compareX - x) > oneMil || fabs(compareY - y) > oneMil || fabs(compareRot - rot) > oneMil))
				diffLoc = TRUE;
		}

		if (diffLoc || diffName)
			return TRUE;
	}

	return FALSE;
}

/******************************************************************************
* pfwv3_pad
*/
static int pfwv3_pad()
{
   int netID = -1;
   int compID = -1;
   CString pinName;
   CString layer;
   double x = 0.0, y = 0.0, rot = 0.0;
   double aSizeX = 0.0, aSizeY = 0.0, topSizeX = 0.0, topSizeY = 0.0, midSizeX = 0.0, midSizeY = 0.0, bottomSizeX = 0.0, bottomSizeY = 0.0;
   int aShape, topShape, midShape, bottomShape;
   double drillSize = 0.0;
	int cpev, csev;
	double cpe = 0.0, cse = 0.0;
   
   aShape = topShape = midShape = bottomShape = -1;

   char *string;
   while (string = get_string(NULL, "|"))
   {
      CString key, val;
      if (get_keyval(string, &key, &val))
      {
         if (!key.CompareNoCase("LAYER"))
            layer = val;
         else if (!key.CompareNoCase("NET"))
            netID = atoi(val);
         else if (!key.CompareNoCase("COMPONENT"))
            compID = atoi(val);
         else if (!key.CompareNoCase("X"))
            x = get_v3unit(val) - origin_x;
         else if (!key.CompareNoCase("Y"))
            y = get_v3unit(val) - origin_y;
         else if (!key.CompareNoCase("TOPXSIZE"))
            topSizeX = get_v3unit(val);
         else if (!key.CompareNoCase("TOPYSIZE"))
            topSizeY = get_v3unit(val);
         else if (!key.CompareNoCase("MIDXSIZE"))
            midSizeX = get_v3unit(val);
         else if (!key.CompareNoCase("MIDYSIZE"))
            midSizeY = get_v3unit(val);
         else if (!key.CompareNoCase("BOTXSIZE"))
            bottomSizeX = get_v3unit(val);
         else if (!key.CompareNoCase("BOTYSIZE"))
            bottomSizeY = get_v3unit(val);
         else if (!key.CompareNoCase("XSIZE"))
            aSizeX = get_v3unit(val);
         else if (!key.CompareNoCase("YSIZE"))
            aSizeY = get_v3unit(val);
         else if (!key.CompareNoCase("SHAPE"))
            aShape = get_v3padshape(val); // see protel_padform
         else if (!key.CompareNoCase("TOPSHAPE"))
            topShape = get_v3padshape(val);  // see protel_padform
         else if (!key.CompareNoCase("MIDSHAPE"))
            midShape = get_v3padshape(val);  // see protel_padform
         else if (!key.CompareNoCase("BOTSHAPE"))
            bottomShape = get_v3padshape(val);  // see protel_padform
         else if (!key.CompareNoCase("NAME"))
            pinName = val;
         else if (!key.CompareNoCase("HOLESIZE"))
            drillSize = get_v3unit(val);
         else if (!key.CompareNoCase("ROTATION"))
            rot = atof(val);
         else if (!key.CompareNoCase("CPEV"))
            cpev = atoi(val);
         else if (!key.CompareNoCase("CSEV"))
            csev = atoi(val);
         else if (!key.CompareNoCase("CPE"))
         {
            cpe = get_v3unit(val);
         }
         else if (!key.CompareNoCase("CSE"))
         {
            cse = get_v3unit(val);
         }
         else
         {
#ifdef DEBUG_UNKNOWN_KEY
            fprintf(logFp,"Unknown Key [%s] in RECORD=PAD at %ld\n", key, getInputFileLineNumber());
            display_error++;
#endif
         }
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=PAD at %ld\n", string, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

	if (cpev == 1)
		cpe = glb_CPEV_val;

	if (csev == 1)
		cse = glb_CSEV_val;

   int padformIndex = get_padformindex(aSizeX, aSizeY, aShape, topSizeX, topSizeY, topShape, midSizeX, midSizeY, midShape, bottomSizeX, bottomSizeY,
                      bottomShape, drillSize, cse, cpe, layer);

   CString padstackName;
   padstackName.Format("PADSTACK_%d", padformIndex);
   
   if (compID < 0)   // free pad
   {
      if (convert_freepad_comp == 1)
      {
         if (strlen(pinName) == 0)
            pinName.Format("$FP_%d", ++freepadcnt);

         while (IsCompNameUsed(pinName))
            pinName.Format("$FP_%d", ++freepadcnt);

         CString geom = "";
         geom.Format("FREEPAD_%d", padformIndex);
         if (Graph_Block_Exists(doc, geom, -1) == NULL)
         {
            BlockStruct *block = Graph_Block_On(GBO_APPEND, geom, -1, 0L);
            block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
            DataStruct *data = Graph_Block_Reference(padstackName, "1", 0, 0, 0, 0, 0 , 1.0,Graph_Level("0", "", 1), TRUE);
            data->getInsert()->setInsertType(insertTypePin);
            Graph_Block_Off();
         }
         DataStruct *data = Graph_Block_Reference(geom, pinName, 0, x, y, DegToRad(rot), 0, 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypePcbComponent);

         // dummy device
         CString tmp;
         tmp.Format("$FP_%s", geom);
         TypeStruct *type = AddType(file, tmp);
         if (type->getBlockNumber() < 0)
            type->setBlockNumber( data->getInsert()->getBlockNumber());

         doc->SetAttrib(&data->getAttributesRef(), doc->RegisterKeyWord(ATT_TYPELISTLINK, TRUE, VT_STRING),
                  VT_STRING, tmp.GetBuffer(0), SA_OVERWRITE, NULL); 
         // end of dummy device

         PfwFreePad *freepad = new PfwFreePad;
         freepadArray.SetAtGrow(freepadArrayCount++, freepad);
         freepad->shapename = geom;
         freepad->compdes = pinName;

         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, pinName.GetBuffer(0), SA_OVERWRITE, NULL);         

         if (netID > -1)
         {
            CString netname = get_netname(netID);
            NetStruct *net = add_net(file, netname);
            add_comppin(file, net, pinName, "1");
         }         
      }
      else
      {
         DataStruct *data = Graph_Block_Reference(padstackName, pinName, 0, x, y, DegToRad(rot), 0, 1.0, -1, TRUE);
         
         if (convert_freepad_comp == 2 && netID > -1)
            data->getInsert()->setInsertType(insertTypeVia);
         else
            data->getInsert()->setInsertType(insertTypeFreePad);
         
         if (netID > -1)
         {        
            CString netname = get_netname(netID);
            doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, netname.GetBuffer(0), SA_APPEND, NULL);
         }         
      }
   }
   else
   {
      if (compID == 155)
		  int stopHere = 1;

	  int geomIndex = get_geomlistptr(compID);

      if (geomIndex < 0)
      {
         ErrorMessage("Geom Index???", "Record=PAD");
         return 0;
      }

		// Find the geometry where this pin belong
      PfwGeom *geom = geomArray[geomIndex];
      
      if (geom->master == 1)
      {
			// Add the pin to the geometry if the geometry is a master copy, don't need to 
			// add pin to pinArrray because it is already done in pfwv3_pad_1stPass() during first pass

         normalize_koo(&x, &y, geomIndex);
         rot = normalize_rot(geom->rotation - rot);

         if (geom->mirror)
         {
            if (!STRCMPI(layer, "TOP"))
            {
               bottomShape = topShape;
               bottomSizeX = topSizeX;
               bottomSizeY = topSizeY;
               layer = "BOTTOM";
            }
            else if (!STRCMPI(layer, "BOTTOM"))
            {
               topShape = bottomShape;
               topSizeX = bottomSizeX;
               topSizeY = bottomSizeY;
               layer = "TOP";
            }
			else if (!STRCMPI(layer, "MULTILAYER"))
			{
               // Swap top and bottom shapes and sizes
			   int temp1 = topShape;
			   topShape = bottomShape;
			   bottomShape = temp1;
			   double temp2 = topSizeX;
			   topSizeX = bottomSizeX;
			   bottomSizeX = temp2;
			   double temp3 = topSizeY;
			   topSizeY = bottomSizeY;
			   bottomSizeY = temp3;
			}
         }

         CString padstackName;
         padstackName = get_cczpadstackname(aSizeX, aSizeY, aShape, topSizeX, topSizeY, topShape, midSizeX, midSizeY, midShape, bottomSizeX, bottomSizeY,
                            bottomShape, drillSize, cse, cpe, layer);
   
         Graph_Block_On(GBO_APPEND, geom->shapename, -1, 0);

         DataStruct *data = Graph_Block_Reference(padstackName, pinName, 0, x, y, DegToRad(rot), 0, 1.0, -1, TRUE);

         Graph_Block_Off();
   
         if (!pinName.Compare("0")) // if a pin is 0, it is a mechanical pin
            data->getInsert()->setInsertType(insertTypeMechanicalPin);
         else if (!pinName.IsEmpty())  // only a pin if it has a name
            data->getInsert()->setInsertType(insertTypePin);
         else
            data->getInsert()->setInsertType(insertTypeMechanicalPin);                
      } 
   }

   return 1;
}


/****************************************************************************/
/*
*/
static   int   get_mirror_layer(int l)
{
   switch (l)
   {
      case  1: return 16;
      case 16: return 1;
      case 17: return 18;
      case 18: return 17;
      case 19: return 20;
      case 20: return 19;
      case 21: return 22;
      case 22: return 21;
   }
   return l;
}

/****************************************************************************/
/*
*/
static int  get_cc_form(int protel_form, double x, double y)
{
   if (x == y)
   {
      if (protel_form == 1)
         return T_ROUND;
      else
      if (protel_form == 2)   
         return T_SQUARE;
      else
         return T_ROUND;
   }
   else
   {
      if (protel_form == 1)
         return T_OBLONG;
      else
      if (protel_form == 2)
         return T_RECTANGLE;
      else
         return T_OBLONG;
   }
   return   T_ROUND;
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

/****************************************************************************/
/*
*/
static DataStruct *write_poly(int layer, double width, int fill, int classtype, 
                              double xoff, double yoff, int boundary, int hidden)
{
   int         i;
   int         widthindex = 0;
   int         err;
   DataStruct  *d;

   if (polycnt < 2)  
   {
      polycnt = 0;
      return NULL;
   }

   if (width == 0)
      widthindex = 0;   // make it small width.
   else
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if (polycnt > 2)
   {
      d = Graph_PolyStruct(layer,0,FALSE);
      d->setGraphicClass(classtype);
      int close = (polyline[0].x == polyline[polycnt-1].x && polyline[0].y == polyline[polycnt-1].y);
      CPoly *pp = Graph_Poly(NULL,widthindex, fill,0, close);
      pp->setFloodBoundary(boundary);
      pp->setHidden(hidden);

      for (i=0;i<polycnt;i++)
         Graph_Vertex(polyline[i].x-xoff, polyline[i].y-yoff, 0.0);
   }
   else
   {
      d = Graph_Line(layer,polyline[0].x-xoff, polyline[0].y-yoff,
                  polyline[1].x-xoff, polyline[1].y-yoff, 0, widthindex , FALSE); // index of width table
   }

   polycnt = 0;
   return d;
}


/****************************************************************************/
/*
*/
static int do_circle(int lay, int widthindex)
{
   double   x1,y1,r;
   char     *lp;

   if ((lp = strtok(NULL," \r\t\n")) == NULL)  return 0;
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \r\t\n")) == NULL)  return 0;
   y1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \r\t\n")) == NULL)  return 0;
   r = cnv_tok(lp);
   Graph_Circle(lay,x1,y1,r, 0L, widthindex , FALSE, TRUE); 

   return 1;
}

/****************************************************************************/
/*
*/
static int do_rectangle(int lay, int widthindex,
                  double x1, double y1, double x2, double y2)
{
   Graph_PolyStruct(lay, 0, FALSE);
   Graph_Poly(NULL,widthindex, TRUE, 0, 1);
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);
   return 1;
}

/****************************************************************************/
/*
*/
int get_drillindex(double size, int layernum)
{
   PFWDrill p;
   int      i;

   if (size == 0) return -1;

   for (i=0;i<drillArrayCount;i++)
   {
      p = drillArray.ElementAt(i);
      if (p.d == size)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d",drillArrayCount);
   //BlockStruct  *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
   //curblock->blocktype = BLOCKTYPE_PADSHAPE;

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);
/*
   Graph_Aperture(name, T_ROUND, size, 0.0, 0.0, 0.0, 0.0, 0, 
            BL_DRILL, TRUE, &err);
*/
   p.d = size;
   drillArray.SetAtGrow(drillArrayCount,p);  
   drillArrayCount++;

   return drillArrayCount -1;
}

/****************************************************************************/
/*
*/
static int update_mirrorlayers()
{
   int         l;
   LayerStruct *lp;

   // this is a fixed attribute
   l = Graph_Level(protel_layer[17],"", 0);
   if (lp = doc->FindLayer(l))
   {
      lp->setLayerType(LAYTYPE_SILK_TOP);
   }
   l = Graph_Level(protel_layer[18],"", 0);
   if (lp = doc->FindLayer(l))
   {
      lp->setLayerType(LAYTYPE_SILK_BOTTOM);
   }
   l = Graph_Level(QPADTOP,"", 0);
   if (lp = doc->FindLayer(l))
   {
      lp->setLayerType(LAYTYPE_PAD_TOP);
   }
   l = Graph_Level("PADINT","", 0);
   if (lp = doc->FindLayer(l))
   {
      lp->setLayerType(LAYTYPE_PAD_INNER);
   }
   l = Graph_Level(QPADBOT,"", 0);
   if (lp = doc->FindLayer(l))
   {
      lp->setLayerType(LAYTYPE_PAD_BOTTOM);
   }

   Graph_Level_Mirror(protel_layer[17], protel_layer[18], "");
   Graph_Level_Mirror(protel_layer[19], protel_layer[20], "");
   Graph_Level_Mirror(protel_layer[21], protel_layer[22], "");
   Graph_Level_Mirror(QPADTOP, QPADBOT, "");
   Graph_Level_Mirror(protel_layer[1], protel_layer[16], "");


/*
   "Unknown",
   "Top",               //1
   "Mid Layer 1",       //2
   "Mid Layer 2",       //3
   "Mid Layer 3",       //4
   "Mid Layer 4",       //5
   "Mid Layer 5",       //6
   "Mid Layer 6",       //7
   "Mid Layer 7",       //8
   "Mid Layer 8",       //9
   "Mid Layer 9",       //10
   "Mid Layer 10",      //11
   "Mid Layer 11",      //12
   "Mid Layer 12",      //13
   "Mid Layer 13",      //14
   "Mid Layer 14",      //15
   "Bottom",            //16
   "Top Silkscreen",    //17
   "Bottom Silkscreen", //18
   "Top Paste Mask",    //19
   "Bottom Paste Mask", //20
   "Top Solder Mask",   //21
   "Bottom Solder Mask",//22
   "Plane 1",           //23
   "Plane 2",           //24
   "Plane 3",           //25
   "Plane 4",           //26
   "Drill Guide"        //27
   "Keep Out Layer",    //28
   "Mechanical Layer 1",//29
   "Mechanical Layer 2",//30
   "Mechanical Layer 3",//31
   "Mechanical Layer 4",//32
   "Drill Drawing",     //33
   "Multi Layer",       //34
*/
   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      LayerStruct *l = doc->getLayerArray()[j];
      if (l == NULL) continue; // could have been deleted.

      CString  lname = l->getName();

      if (lname.CompareNoCase(protel_layer[33]) == 0)
      {
         l->setLayerType(LAYTYPE_DRILL);
      }
      else
      if (lname.CompareNoCase(protel_layer[27]) == 0)
      {
         l->setLayerType(LAYTYPE_DRILL);
      }
      else
      if (lname.CompareNoCase(protel_layer[17]) == 0)
      {
         l->setLayerType(LAYTYPE_SILK_TOP);
      }
      else
      if (lname.CompareNoCase(protel_layer[18]) == 0)
      {
         l->setLayerType(LAYTYPE_SILK_BOTTOM);
      }
      else
      if (lname.CompareNoCase(protel_layer[19]) == 0)
      {
         l->setLayerType(LAYTYPE_PASTE_TOP);
      }
      else
      if (lname.CompareNoCase(protel_layer[20]) == 0)
      {
         l->setLayerType(LAYTYPE_PASTE_BOTTOM);
      }
      else
      if (lname.CompareNoCase(protel_layer[21]) == 0)
      {
         l->setLayerType(LAYTYPE_MASK_TOP);
      }
      else
      if (lname.CompareNoCase(protel_layer[22]) == 0)
      {
         l->setLayerType(LAYTYPE_MASK_BOTTOM);
      }
      else
      if (lname.CompareNoCase(protel_layer[1]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_TOP);
      }
      else
      if (lname.CompareNoCase(protel_layer[2]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[3]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[4]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[5]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[6]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[7]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[8]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[9]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[10]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[11]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[12]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[13]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[14]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
      else
      if (lname.CompareNoCase(protel_layer[15]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }  
      else
      if (lname.CompareNoCase(protel_layer[16]) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_BOT);
      }
      else
      if (lname.CompareNoCase(protel_layer[28]) == 0)
      {
         l->setLayerType(LAYTYPE_ALL);
      }
      else
      if (lname.CompareNoCase(protel_layer[34]) == 0) // multilayer
      {
         l->setLayerType(LAYTYPE_PAD_ALL);
      }

      if (lname.CompareNoCase(prim_boardoutline) == 0)
      {
         l->setLayerType(LAYTYPE_BOARD_OUTLINE);
      }
   }


   /* blind via layers
      l.Format("V %s", protel_layer[i]);  // here use a funny layer, because later I have to kill 
                                          // unused inner layers !
   */

   int   electcnt = 0;
	int	i=0;
   for (i=1;i<=16;i++)
   {
      // assign stack numbers
      for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
      {
         LayerStruct *l = doc->getLayerArray()[j];
         if (l == NULL) continue; // could have been deleted.

         CString  blindlay, lname = l->getName();
         
         if (lname.CompareNoCase(protel_layer[i]) == 0)
         {
            l->setElectricalStackNumber(++electcnt);
            // now check if a blind via layer exists and if yes, move it and delete it.
            blindlay.Format("V %s", protel_layer[i]);
            LayerStruct *bl;
            if (bl = doc->FindLayer_by_Name(blindlay))
            {
               MoveLayer(doc, bl, l, TRUE);
            }
         }
      }
   }

   // now remove all other  blind layers
   for (i=1;i<=16;i++)
   {
      CString  blindlay;
      blindlay.Format("V %s", protel_layer[i]);
      LayerStruct *bl;
      if (bl = doc->FindLayer_by_Name(blindlay))
      {
         DeleteLayer(doc, bl, TRUE);
      }
   }

   // assign padtop, padbottom layerstack
   l = Graph_Level(QPADTOP,"", 0);
   if (lp = doc->FindLayer(l))
   {
      lp->setElectricalStackNumber(1);
   }
   l = Graph_Level(QPADBOT,"", 0);
   if (lp = doc->FindLayer(l))
   {
      lp->setElectricalStackNumber(electcnt);
   }

   return 1;
}

/****************************************************************************/
/*
*/
int update_layer(const CString& layerName,bool used)
{
   int   i;

   for (i=0;i<pfwLayerArray.getSize();i++)
   {
      CPfwLayer* layer = pfwLayerArray.getAt(i);

      if (layer != NULL && layer->getName().CompareNoCase(layerName) == 0)
      {
         if (used)
         {
            layer->setUsed(true);
         }

         return i;      
      }
   }

   int layersCount = pfwLayerArray.getSize();

   pfwLayerArray.getAt(layersCount)->setName(layerName);
   pfwLayerArray.getAt(layersCount)->setUsed(used);

   return layersCount;
}

/******************************************************************************
* pfw_pcb
*/
static int pfw_pcb()
{
   char  *lp;
   int   fver;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  return   p_error();  // FILE
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  return   p_error();  // version
   fver = atoi(lp);

   if (fver != 6)
   {
      CString  tmp;
      tmp.Format("File %s", lp);
      ErrorMessage("Unsupported FILE ",tmp);
      return -1;
   }

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  return   p_error();
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  return   p_error();
   cur_pfw_version = (int)floor(atof(lp)*10);

   if (cur_pfw_version < 27)
   {
      CString  tmp;
      tmp.Format("Version %s detected - Version 2.7/2.8 needed", lp);

      ErrorMessage(tmp, "Unsupported Version");
      return -1;
   }

   // this line is not needed.
   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      ErrorMessage("Not a Protel Advanced PCB format");
      return -1;
   }

   incrementInputFileLineNumber();

   return 1;
}

/******************************************************************************
* get_geomindex
*/
static int get_geomindex(CString geomName)
{
   for (int i=0;i<geomArrayCount;i++)
   {
      PfwGeom *geom = geomArray[i];
      
      if (!geom->compdes.CompareNoCase(geomName))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pfw_ct()
{
   char *lp;

	if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

	int geomIndex = get_geomindex(cur_compname);
	if (geomIndex < 0)
   {
      ErrorMessage("Geom Index???", "Record=CT");
      return 0;
   }
   PfwGeom *geom = geomArray[geomIndex];

   PfwCT *c = new PfwCT;
   geom->ctArray.SetAtGrow(geom->ctArray.GetCount(), c);  
   c->geomindex = geomIndex;

	if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   c->x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   c->y1 = cnv_tok(lp);

	double x1 = c->x1, y1 = c->y1;
   normalize_koo(&x1, &y1, geomArrayCount-1);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   c->x2 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y2
   c->y2 = cnv_tok(lp);

	double x2 = c->x2, y2 = c->y2;
   normalize_koo(&x2, &y2, geomArrayCount-1);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // width
   c->width = cnv_tok(lp);

   int err;
   int widthindex = 0;
	if (c->width > 0)
      widthindex = Graph_Aperture("", T_ROUND, c->width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer
   if (geomArray[geomArrayCount-1]->mirror)
      c->layer = protel_layer[get_mirror_layer(atoi(lp))];
   else
      c->layer = protel_layer[atoi(lp)];

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   int laynr = Graph_Level(c->layer,"", 0);
   DataStruct *d = Graph_PolyStruct(laynr,0,FALSE);
   d->setGraphicClass(get_layerclass(c->layer));

   Graph_Poly(NULL, widthindex, 0,0, 0);
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);

   return 1;
}

/****************************************************************************/
/*
*/
static int get_netlistnetnr(int compindex, int pinindex, int netnr, const char *pinname)
{
   int   i;

   // here check the netnr against the loaded netlist
   for (i=0;i<netCompPinArrayCount;i++)
   {
#ifdef _DEBUG
      PfwNetCompPin *n = netCompPinArray[i];
#endif
      if (netCompPinArray[i]->compnr == compindex && netCompPinArray[i]->pinnr == pinindex)
      {
         if (netCompPinArray[i]->netnr == netnr)   
            return netnr;

         CString  n1 = get_netname(netnr);                     // from CP record
         CString  n2 = get_netname(netCompPinArray[i]->netnr); // from netlist section

         // here the netnumber of the loaded netlist is different.
         fprintf(logFp, "DRC Error 1: Component [%s] Pin [%s] in nets [%s] and [%s]: The Netlist does not match the design !\n",
            cur_compname, pinname, n1, n2);
         display_error++;
         return netCompPinArray[i]->netnr; // in Protel the netlist record netname seems to win.
      }
   }

   if (netCompPinArrayCount)
   {
      if (netnr != 0)
      {
         // has a netnumber, but is not found in loaded netlist
         fprintf(logFp, "DRC Error 2: Component [%s] Pin [%s] : The Netlist does not match the design !\n",
            cur_compname, pinname);
         display_error++;
         return 0;   // not in  loaded netlist
      }
   }

   // if netnr == 0, it is not in the netcomppin array !
   return netnr;
}


/****************************************************************************/
/*
*/
static int pfw_cp()
{
   char     *lp;
   double   x1 = 0.0, y1 = 0.0,rot = 0.0;
   CString  lay;
   CString  pshapename, pinname;
   double   topx = 0.0, topy = 0.0, midx = 0.0, midy = 0.0, bottomx = 0.0, bottomy = 0.0;
   int      topshape = -1, midshape = -1, bottomshape = -1, padlay = -1;
   double   drill = 0.0;
   int      netnr;
   CString layer;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

	int geomIndex = get_geomindex(cur_compname);
	if (geomIndex < 0)
   {
      ErrorMessage("Geom Index???", "Record=CP");
      return 0;
   }

   PfwGeom *geom = geomArray[geomIndex];

   PfwPin *c = new PfwPin;
   geom->pinArray.SetAtGrow(geom->pinArrayCount++, c);  
   c->geomindex = geomIndex;
   c->pinnr = ++cur_comppincnt;
	c->netid = 0;

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   c->pinx = x1;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);
   c->piny = y1;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   topx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   topy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   topshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   midx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   midy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   midshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   bottomx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   bottomy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   bottomshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // drill
   drill = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // power layer stat
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // pad layer
   padlay = atoi(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // netnr
   netnr = atoi(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // connect term
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // rot
   rot = atof(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // soldermask expansion
   double soldermask_expansion = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // pastemask expansion
   double pastemask_expansion = cnv_tok(lp);

   normalize_koo(&x1, &y1, geomArrayCount-1);

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // allow pins without names
   lp = strtok(fPfwLine,QNEXTLINE);
   pinname = lp;
   c->pinname = pinname;

   // here update netnr with the netlist
   // geomlist is index+1, because Protel indexes starting 1. Pincnt is already added, netnr is from the file
   //netnr = get_netlistnetnr(geomArrayCount, cur_comppincnt, netnr, pinname);  - Joanne  Don't compare the net list defined in NETDEF.
   // NETDEF also keeps the previous pin assignment for record keeping.  This will cause a problem.  Use net list defined in COMP Def.

   if (!strlen(pinname) && netnr)
   {
      pinname.Format("#%d",cur_comppincnt);
      // this pin must get a name because it is in the netlist !!!
      fprintf(logFp, "Component [%s] has a pin in the netlist [%s] without a name -> assigned [%s]\n",
         geomArray[geomArrayCount-1]->compdes, get_netname(netnr), pinname);
      display_error++;
   }

   rot = normalize_rot(geomArray[geomArrayCount-1]->rotation - rot);
	c->pinrot = rot;

   if (geomArray[geomArrayCount-1]->mirror)
   {
      // switch top bottom
      double   tmpx, tmpy;
      int      tmpform;
      tmpx = topx;
      tmpy = topy;
      tmpform = topshape;
      topx = bottomx;
      topy = bottomy;
      topshape = bottomshape;
      bottomx = tmpx;
      bottomy = tmpy;
      bottomshape = tmpform;
      if (padlay == 1)
         padlay = 16;
      else
      if (padlay == 16)
         padlay = 1;
   }

   // make SMD correct.
   if (padlay == 1)
   {
      midx = midy = 0;
      bottomx = bottomy = 0;
	  layer = "TOP";
   }
   else
   if (padlay == 16)
   {
      midx = midy = 0;
      topx = topy = 0;
	  layer = "BOTTOM";
   }
   else
   {
	   layer = "MULTILAYER";
   }

   c->padstackIndex = get_padformindex(0.0, 0.0, 0, topx, topy, topshape, midx, midy, midshape,
                          bottomx, bottomy,bottomshape, drill, soldermask_expansion, pastemask_expansion, layer);

   pshapename.Format("PADSTACK_%d", c->padstackIndex);

   DataStruct* data = Graph_Block_Reference(pshapename, pinname, 0, x1, y1, DegToRad(rot), 0, 1.0, -1, TRUE);

   if (pinname.Compare("0") == 0)   // if a pin is 0, it is a mecahnical pin
      data->getInsert()->setInsertType(insertTypeMechanicalPin);
   else
   if (strlen(pinname)) // only a pin if it has a name
      data->getInsert()->setInsertType(insertTypePin);
   else
      data->getInsert()->setInsertType(insertTypeMechanicalPin);

   c->insertType = data->getInsert()->getInsertType();

   // same in pfw_cp_skip
   // here update netlist if it is a electrical pin 
   if (strlen(pinname) && pinname.Compare("0")) // if not mechanical
   {
      CString  cur_netname;
      if (netnr == 0)
         cur_netname = NET_UNUSED_PINS;
      else
         cur_netname = get_netname(netnr);

      NetStruct *n = add_net(file,cur_netname);

      if (strlen(geomArray[geomArrayCount-1]->compdes))
      {
         if (test_add_comppin(geomArray[geomArrayCount-1]->compdes, pinname, file) == NULL)
         {
            add_comppin(file, n, geomArray[geomArrayCount-1]->compdes, pinname);
            c->insertType = INSERTTYPE_PIN;
         }  
         else
         {
            fprintf(logFp, "Duplicated pads [%s] in Component [%s]\n", pinname, 
               geomArray[geomArrayCount-1]->compdes); 
            display_error++;
            c->insertType = -1;
            data->getInsert()->setInsertType(insertTypeMechanicalPin);
         }
      } // no compname
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_fp()
{
   char     *lp;
   CString  lay, layer;
   CString  pshapename, pinname;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL)
      return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   double x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   double topx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double topy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   int topshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double midx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double midy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   int midshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double bottomx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   double bottomy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   int bottomshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   double drill = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // power layer stat
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  
      return p_error();
   int padlay = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   int netnr = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // connect term
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   double rot = normalize_rot(atof(lp));

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   double soldermask_expansion = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   double pastemask_expansion = cnv_tok(lp);

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // must not have a pinname
   pinname = "";
   if ((lp = strtok(fPfwLine,QNEXTLINE)) != NULL) 
   {
      pinname = lp;
   }

   int   mirror = FALSE;

   // make SMD correct.
   if (padlay == 1)
   {
      midx = midy = 0;
      bottomx = bottomy = 0;
	  layer = "TOP";
   }
   else if (padlay == 16)
   {
      // switch top bottom
      topx = bottomx;
      topy = bottomy;
      topshape = bottomshape;
      padlay = 1;
      midx = midy = 0;
      bottomx = bottomy = 0;
      mirror = TRUE;
	  layer = "TOP";
   }
   else
   {
	   layer = "MULTILAYER";
   }

   int pindex = get_padformindex(0.0, 0.0, 0, topx, topy, topshape, midx, midy, midshape, bottomx, bottomy,bottomshape,
                     drill,soldermask_expansion, pastemask_expansion, layer);

   pshapename.Format("PADSTACK_%d",pindex);

   DataStruct *data = NULL;

   if (convert_freepad_comp == 1)
   {

      if (strlen(pinname) == 0)
         pinname.Format("$FP_%d", ++freepadcnt);

      while (IsCompNameUsed(pinname))
         pinname.Format("$FP_%d", ++freepadcnt);

      CString geom;
      geom.Format("FREEPAD_%d", pindex);
      if (Graph_Block_Exists(doc, geom, -1) == NULL)
      {
         BlockStruct *block = Graph_Block_On(GBO_APPEND, geom, -1, 0L);
         block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
         data = Graph_Block_Reference(pshapename, "1", 0, 0, 0, 0, 0 , 1.0,Graph_Level("0", "", 1), TRUE);
         data->getInsert()->setInsertType(insertTypePin);
      
         Graph_Block_Off();
      }
      data = Graph_Block_Reference(geom, pinname, 0, x1, y1, DegToRad(rot), mirror, 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypePcbComponent);

      // dummy device
      CString tmp;
      tmp.Format("$FP_%s", geom);
      TypeStruct *type = AddType(file,tmp);
      if (type->getBlockNumber() < 0)
         type->setBlockNumber( data->getInsert()->getBlockNumber());

      doc->SetAttrib(&data->getAttributesRef(), doc->RegisterKeyWord(ATT_TYPELISTLINK, TRUE, VT_STRING),
               VT_STRING, tmp.GetBuffer(0), SA_OVERWRITE, NULL); 
      // end of dummy device

      PfwFreePad *freepad = new PfwFreePad;
      freepadArray.SetAtGrow(freepadArrayCount++, freepad);
      freepad->shapename = geom;
      freepad->compdes = pinname;

      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, pinname.GetBuffer(0), SA_OVERWRITE, NULL);

      if (netnr)
      {
         CString cur_netname;
         cur_netname = get_netname(netnr);
         NetStruct *net = add_net(file, cur_netname);
         add_comppin(file, net, pinname, "1");
      }
   }
   else
   {

      data = Graph_Block_Reference(pshapename, pinname, 0, x1, y1, DegToRad(rot), mirror, 1.0, -1, TRUE);
   
      if (convert_freepad_comp == 2 && netnr)
         data->getInsert()->setInsertType(insertTypeVia);
      else
         data->getInsert()->setInsertType(insertTypeFreePad);

      if (netnr)
      {
         CString cur_netname;
         cur_netname = get_netname(netnr);
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE),
               VT_STRING, cur_netname.GetBuffer(0), SA_APPEND, NULL);
      }
   }

   return 1;
}

/******************************************************************************
* get_masterpinname
*/
static CString get_masterpinname(int mastergeom,  int pinindex, CString oldpinname)
{
	if (mastergeom < 0)
		return oldpinname;

   CString pname = "";
   int found = FALSE;
   PfwGeom *geom = geomArray[mastergeom];

   for (int i=0; i<geom->pinArrayCount; i++)
   {
		PfwPin *pin = geom->pinArray[i];
      if (pin->pinnr == pinindex)
      {
         pname = pin->pinname;
         return pname;
      }
   }

   fprintf(logFp, "A master pinname could not be found for [%s]\n", oldpinname);
   display_error++;
   return oldpinname;
}

/****************************************************************************/
/*
*/
static int pfw_ct_skip()
{
   char     *lp;

	if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

	int geomIndex = get_geomindex(cur_compname);
	if (geomIndex < 0)
   {
      ErrorMessage("Geom Index???", "Record=CT");
      return 0;
   }
   PfwGeom *geom = geomArray[geomIndex];

   PfwCT *c = new PfwCT;
   geom->ctArray.SetAtGrow(geom->ctArray.GetCount(), c);  
	c->geomindex = geomIndex;

	if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   c->x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   c->y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   c->x2 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y2
   c->y2 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // width
   c->width = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer
   if (geomArray[geomArrayCount-1]->mirror)
      c->layer = protel_layer[get_mirror_layer(atoi(lp))];
   else
      c->layer = protel_layer[atoi(lp)];

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_cp_skip()
{
   char     *lp;
   double   x1, y1, rot;
   CString  pshapename, pinname, layer;
   double   topx = 0.0, topy = 0.0, midx = 0.0, midy = 0.0, bottomx = 0.0, bottomy = 0.0;
   int      topshape, midshape, bottomshape, padlay;
   double   drill;
   int      netnr;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

	int geomIndex = get_geomindex(cur_compname);
	if (geomIndex < 0)
   {
      ErrorMessage("Geom Index???", "Record=CP");
      return 0;
   }

   PfwGeom *geom = geomArray[geomIndex];

   PfwPin *c = new PfwPin;
   geom->pinArray.SetAtGrow(geom->pinArrayCount++, c);  
   c->geomindex = geomIndex;
   c->pinnr = ++cur_comppincnt;
	c->netid = 0;

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   c->pinx = x1;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);
   c->piny = y1;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   topx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   topy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   topshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   midx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   midy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   midshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   bottomx = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   bottomy = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   bottomshape = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // drill
   drill = cnv_tok(lp);


   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // power layer stat
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // pad layer
   padlay = atoi(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // netnr
   netnr = atoi(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // connect term
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // rot
   rot = atof(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // soldermask expansion
   double soldermask_expansion = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // pastemask expansion
   double pastemask_expansion = cnv_tok(lp);

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   pinname = "";  // allow no pinname

   if ((lp = strtok(fPfwLine,QNEXTLINE)) != NULL)
      pinname = lp;  

   // netnr = get_netlistnetnr(geomArrayCount, cur_comppincnt, netnr, pinname); - Joanne  Don't compare the net list defined in NETDEF.
   // NETDEF also keeps the previous pin assignment for record keeping.  This will cause a problem.  Use net list defined in COMP Def.


   // here we need a check for pin name.
   // it can happen that the MASTER defintion has a different pinname than this 
   // instance. (Wolfgang)
   // pinname = get_masterpinname(mastergeomindex, cur_comppincnt, pinname);  - Joanne This function call checks the pin order and change
   // pinname occurring to the master pin order. Since order of the pin could be different in geometry definition, changing pin name by checking Geomindex
   // and pin index would not work correctly.  This causes the net list to be wrong.
   c->pinname = pinname;

   rot = normalize_rot(geomArray[geomArrayCount-1]->rotation - rot);
	c->pinrot = rot;

   if (geomArray[geomArrayCount-1]->mirror)
   {
      // switch top bottom
      double   tmpx, tmpy;
      int      tmpform;
      tmpx = topx;
      tmpy = topy;
      tmpform = topshape;
      topx = bottomx;
      topy = bottomy;
      topshape = bottomshape;
      bottomx = tmpx;
      bottomy = tmpy;
      bottomshape = tmpform;
      if (padlay == 1)
         padlay = 16;
      else
      if (padlay == 16)
         padlay = 1;
   }

   // make SMD correct.
   if (padlay == 1)
   {
      midx = midy = 0;
      bottomx = bottomy = 0;
	  layer = "TOP";
   }
   else
   if (padlay == 16)
   {
      midx = midy = 0;
      topx = topy = 0;
	  layer = "BOTTOM";
   }
   else
   {
	   layer = "MULTILAYER";
   }

   c->padstackIndex = get_padformindex(0.0, 0.0, 0, topx, topy, topshape, midx, midy, midshape, bottomx, bottomy, 
									bottomshape, drill, soldermask_expansion, pastemask_expansion, layer);

   if (pinname.Compare("0") == 0)   // if a pin is 0, it is a mecahnical pin
      c->insertType = insertTypeMechanicalPin;
   else
   if (strlen(pinname)) // only a pin if it has a name
      c->insertType = insertTypePin;
   else
      c->insertType = insertTypeMechanicalPin;

	// same also in pfw_cp
   // here update netlist if it is a electrical pin
   if (strlen(pinname) && pinname.Compare("0")) // if not mechanical
   {
      CString  cur_netname;
      if (netnr == 0)
         cur_netname = NET_UNUSED_PINS;
      else
         cur_netname = get_netname(netnr);

      NetStruct *n = add_net(file,cur_netname);

      if (test_add_comppin(geomArray[geomArrayCount-1]->compdes, pinname, file) == NULL)
      {
         add_comppin(file, n, geomArray[geomArrayCount-1]->compdes, pinname);
         c->insertType = INSERTTYPE_PIN;
      }
      else
      {
         fprintf(logFp, "Duplicated pads [%s] in Component [%s]\n", pinname, 
            geomArray[geomArrayCount-1]->compdes); 
         display_error++;
         c->insertType = -1;  // duplicated;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_netdef()
{
   char     *lp;
   CString  nname;
   char     iline[MAX_LINE+1];

   if (fgets(iline,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(iline,QNEXTLINE)) == NULL) return p_error(); 

   nname = lp;

   // some more net params
   if (fgets(iline,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   int brk_open = 0;

   while (TRUE)
   {
      if (fgets(iline,MAX_LINE,fPfw) == NULL)
      {
         fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
         display_error++;
         return -1;
      }

      incrementInputFileLineNumber();

      if ((lp = strtok(iline,QNEXTLINE)) != NULL) 
      {
         if (!STRCMPI(lp,"("))
         {
            brk_open++;
         }
         else
         if (!STRCMPI(lp,"{"))
            brk_open++;
         else
         if (!STRCMPI(lp,"}"))
            brk_open--;
         else
         if (!STRCMPI(lp,")"))
         {
            brk_open--;
         }
         else
         if (!brk_open)    // end of netdef
         {
            // get 1 more lines
            if (fgets(iline,MAX_LINE,fPfw) == NULL)
            {
               fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
               display_error++;
               return -1;
            }

            incrementInputFileLineNumber();

            return 1;
         }
      }
   }

   return 1;
}

/******************************************************************************
* pfw_fv
*/
static int pfw_fv()
{
   char     *lp;
   CString  pshapename, layer;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL)
      return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double diam = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); 
   double drill = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // via route status

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // layer pair
   int layerpair = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();
   int netnr = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // connect nr
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error(); // soldermask expansion
   double soldermask_expension = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)
      return p_error();

   //static int get_padformindex(double ax, double ay, int ashape, 
   //                            double topx, double topy, int topshape, 
   //                            double midx, double midy, int midshape, 
   //                            double bottomx, double bottomy, int bottomshape, 
   //                            double drill, 
   //                            double soldermask_expansion, double pastemask_expansion, CString layer)
   pshapename = get_cczpadstackname(
      0.0, 0.0, 0, 
      diam, diam, 0, 
      diam, diam, 0, 
      diam, diam, 0, 
      drill, 0.0, 0.0, "MULTILAYER", layerpair);

   DataStruct *data = Graph_Block_Reference(pshapename, NULL, 0, x1, y1, DegToRad(0.0), 0, 1.0, -1, TRUE);
   data->getInsert()->setInsertType(insertTypeVia);

   if (netnr)
   {
      CString  cur_netname;
      cur_netname = get_netname(netnr);
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname.GetBuffer(0), SA_APPEND, NULL); 
   }

   return 1;
}

/****************************************************************************/
/*
   Component vias are converted to mechanical padstacks

*/
static int pfw_cv()
{
   char     *lp;
   double   x1, y1;
   CString  pshapename;
   double   diam = 0.0;
   double   drill = 0.0;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // diam
   diam = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // drill
   drill = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // via route status
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer pair

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // netnr
   int netnr = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // connect nr
   
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // soldermask expansion
   double soldermask_expension = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // power/ground expansion
   double pastemask_expension = cnv_tok(lp);

   pshapename = get_cczpadstackname(0.0,  0.0,  0, 
	                             diam, diam, 0,
                                 diam, diam, 0,
                                 diam, diam, 0, 
								 drill, soldermask_expension, pastemask_expension, "MULTILAYER");


   normalize_koo(&x1, &y1, geomArrayCount-1);

   DataStruct *d = Graph_Block_Reference(pshapename, NULL, 
      0, x1, y1, DegToRad(0.0), 0, 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeMechanicalPin);

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_fs()
{
   char     *lp;
   double   x1, y1;
   CString  lay;
   double   height, rot;
   int      mirror = 0;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // text height
   height = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // rot
   rot = normalize_rot(atof(lp));

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // orientation
   mirror = atoi(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // font
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer 
   lay = protel_layer[atoi(lp)];

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // char stroke width

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // no empty text
   if ((lp = strtok(fPfwLine,"\n")) != NULL) 
   {
      int   laynr = Graph_Level(lay,"", 0);

      if (!strcmp(lp,".LAYER_NAME"))
         lp = lay.GetBuffer(0);
      DataStruct *d = Graph_Text(laynr,lp,x1,y1,
            height,height*TEXTRATIO,DegToRad(rot),
            0L, TEXT_PROPORTIONAL,     // protel has proportional text
            mirror,  
            0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_pg()
{
   char     *lp;
   CString  lay;
   char     longline[MAX_POLY];
   int      i, netnr, vertexcnt;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   int lnr = atoi(lp);
   lay = protel_layer[lnr];

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   netnr = atoi(lp);

   // parameters
   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   // here check if horizontal or vertical tracks are used. If yes, do not do the plane, because 
   // tracks are used to fill (flood) and the plane is only the flood border, but not the actual 
   // graphic
   int tracks = 0;
   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error();    // remove dead copper
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  return p_error();    // horizontal tracks
   tracks += atoi(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  return p_error();    // vertical tracks
   tracks += atoi(lp);

   // grid parameters
   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if (cur_pfw_version >= 28)    // i need a desciption for this !?!
   {
      if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
      {
         fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
         display_error++;
         return -1;
      }

      incrementInputFileLineNumber();
   }

   // number of vertex
   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL)    return p_error(); 
   vertexcnt = atoi(lp) + 1;  // last repeats

   if (vertexcnt >= MAX_POLY)
   {
      fprintf(logFp, "Too many vertex at %ld\n",getInputFileLineNumber());
      display_error++;
   }

   polycnt = 0;

   for (i=0;i<vertexcnt;i++)
   {
      if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
      {
         fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
         display_error++;
         return -1;
      }

      incrementInputFileLineNumber();

      if (polycnt < MAX_POLY)
      {
         if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL)    return p_error(); // x1
         polyline[polycnt].x = cnv_tok(lp);
         if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
         polyline[polycnt].y = cnv_tok(lp);
         polyline[polycnt].arcflg = 0;
         polycnt++;
      }
   }

   // track, arc flags
   if (fgets(longline,MAX_POLY,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(longline,QNEXTLINE)) == NULL)    return p_error(); 

   int pcnt = 0;
   while (lp)
   {
      polyline[pcnt].arcflg = atoi(lp);
      pcnt++;
      if ((lp = strtok(NULL,QNEXTLINE)) == NULL)  break;
   }

   if (pcnt != polycnt)
   {
      fprintf(logFp, "Poly format error at %ld\n", getInputFileLineNumber());
      display_error++;
   }

   int   laynr = Graph_Level(lay,"", 0);
   int   boundary = TRUE, hidden = FALSE, fill = FALSE;

   if (tracks && TRANSLATE_HATCHLINES) 
   {
      hidden = TRUE;
   }

   if (!hidden)   fill = TRUE;

   DataStruct *d = write_poly(laynr, 0, fill, 0, 0.0, 0.0, boundary, hidden); // a pg is not filled, only the tracks will fill it.
   
   PfwPolyData *polyData = new PfwPolyData;
   polygonArray.SetAtGrow(polygonArrayCount++, polyData);  
   polyData->np = d;
   polyData->layer_error_msg = 0;

   // here assign netname
   if (d)   // can be NULL if write_poly does not have 2 cnts.
   {
      if (lnr >= 1 && lnr <= 16 && netnr)
      {
         CString  cur_netname;
         cur_netname = get_netname(netnr);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,
            cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
         d->setGraphicClass(graphicClassNormal); // polygon is not an edge_class !
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_cs()
{
   char     *lp;
   double   x1, y1;
   CString  lay;
   double   height, rot;
   int      mirror = 0;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // text height
   height = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // rot
   rot = normalize_rot(atof(lp));

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // orientation
   mirror = atoi(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // font
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer 
   if (geomArray[geomArrayCount-1]->mirror)
      lay = protel_layer[get_mirror_layer(atoi(lp))];
   else
      lay = protel_layer[atoi(lp)];

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // char stroke width

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   normalize_koo(&x1, &y1, geomArrayCount-1);

   rot = normalize_rot(geomArray[geomArrayCount-1]->rotation - rot);
   if (geomArray[geomArrayCount-1]->mirror)
   {
      // switch top bottom
   }

   // no empty text
   if ((lp = strtok(fPfwLine,"\n")) != NULL) 
   {
      int   laynr = Graph_Level(lay,"", 0);

      if (!strcmp(lp,".LAYER_NAME"))
         lp = lay.GetBuffer(0);
      DataStruct *d = Graph_Text(laynr,lp,x1,y1,
            height,height*TEXTRATIO,DegToRad(rot),
            0, TEXT_PROPORTIONAL,      // protel has proportional text
            mirror,  
            0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_ft()
{
   char     *lp;
   double   x1, y1, x2, y2, width;
   CString  lay;
   int      widthindex;
   int      hatchline = FALSE;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   x2 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y2
   y2 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // width
   width = cnv_tok(lp);

   int err;

   if (width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer

   if (atoi(lp) > 34)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;

      if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
      {
         return -1;
      }

      incrementInputFileLineNumber();
      return 1;
   }
   else
   {
      cur_data = NULL;
   }

   lay = protel_layer[atoi(lp)];
   int   lnr = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // route status
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // netnr
   int netnr = atoi(lp);

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL)    return p_error(); // polygonnr
   int pnr = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // teardrop
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // lockstatus

   int   laynr = Graph_Level(lay,"", 0);

   DataStruct *d = NULL;

   if (pnr < 1)            pnr = 0;
   if (pnr > polygonArrayCount)  pnr = 0;


   if (pnr)
   {
      hatchline = TRUE;
      if (d = polygonArray[pnr-1]->np)
      {
         if (d->getLayerIndex() != laynr)
         {
            if (polygonArray[pnr-1]->layer_error_msg == 0)
            {
               fprintf(logFp,"DRC Error: Polygon [%d] and associated FT is on different layer at %ld -> not linked!\n", 
                  pnr, getInputFileLineNumber());
               display_error++;
            }
            polygonArray[pnr-1]->layer_error_msg++;
            d = NULL;
         }
      }
   }
   
   if (!TRANSLATE_HATCHLINES && hatchline)
      return 0;

   if (d == NULL)
   {
      d = Graph_PolyStruct(laynr,0,FALSE);

      if (lnr >= 1 && lnr <= 16 && netnr)
      {
         CString  cur_netname;
         cur_netname = get_netname(netnr);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,
            cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
         if (!hatchline)
            d->setGraphicClass(GR_CLASS_ETCH);
      }
      else
         d->setGraphicClass(get_layerclass(lay));
   }
   
   CPoly *cp = Graph_Poly(d, widthindex, 0, 0, 0);
   cp->setHatchLine(hatchline);
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_fa()
{
   char     *lp;
   double   x, y, rad, sa, da, ea, width;
   CString  lay;
   int      widthindex;
   int      hatchline = FALSE;

   cur_data = NULL;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   rad = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   sa = atof(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   ea = atof(lp);
   if (ea == 0)   ea = 360;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // width
   width = cnv_tok(lp);

   int err;

   if (width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer
   int lnr = atoi(lp);
   lay = protel_layer[lnr];

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // netnr
   int netnr = atoi(lp);

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL)    return p_error(); // polygonnr
   int pnr = atoi(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // lockstatus

   if (rad < 0)
   {
      fprintf(logFp, "Arc Radius Error [%lf] -> ignored at %ld\n", rad, getInputFileLineNumber());
      display_error++;
      return 1;
   }

   int   laynr = Graph_Level(lay,"", 0);

   if (ea < sa)   ea += 360;
   da = ea - sa;

   DataStruct *d = NULL;

   if (pnr < 1)            pnr = 0;
   if (pnr > polygonArrayCount)  pnr = 0;

   if (pnr)
   {
      hatchline = TRUE;

      if (d = polygonArray[pnr-1]->np)
      {
         if (d->getLayerIndex() != laynr)
         {
            if (polygonArray[pnr-1]->layer_error_msg == 0)
            {
               fprintf(logFp,"DRC Error: Polygon [%d] and associated FT is on different layer at %ld -> not linked!\n", 
                  pnr, getInputFileLineNumber());
               display_error++;
            }
            polygonArray[pnr-1]->layer_error_msg++;
            d = NULL;
         }
      }
   }

   if (!TRANSLATE_HATCHLINES && hatchline)
      return 0;

   if (d == NULL)    
   {
      // could not find a existing PG structure
      d = Graph_PolyStruct(laynr,0,FALSE);
   }

   if (da == 360)
   {
      CPoly *cp = Graph_Poly(d, widthindex, 0, 0, 0);
      cp->setHatchLine(true);
      Graph_Vertex(x, y+rad, 1.0);
      Graph_Vertex(x, y-rad, 1.0);
      Graph_Vertex(x, y+rad, 0.0);
   }
   else
   {
      double   x1,y1,x2,y2,bulge;
      bulge = tan(DegToRad(da)/4);
      x1 = x + rad * cos(DegToRad(sa));
      y1 = y + rad * sin(DegToRad(sa));
      x2 = x + rad * cos(DegToRad(sa+da));
      y2 = y + rad * sin(DegToRad(sa+da));
      CPoly *cp = Graph_Poly(d, widthindex, 0, 0, 0);
      cp->setHatchLine(hatchline);
      Graph_Vertex(x1, y1, bulge);
      Graph_Vertex(x2, y2, 0.0);
   }

   if (lnr >= 1 && lnr <= 16 && netnr)
   {
      CString  cur_netname;
      cur_netname = get_netname(netnr);
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,
            cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
      if (!hatchline)
         d->setGraphicClass(GR_CLASS_ETCH);
   }
   else
      d->setGraphicClass(get_layerclass(lay));

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_ca()
{
   char     *lp;
   double   x1, y1, rad, sa, da, ea, width;
   CString  lay;
   int      widthindex;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   rad = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   sa = atof(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   ea =atof(lp);
   if (ea == 0)   ea = 360;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // width
   width = cnv_tok(lp);

   int err;

   if (width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer

   if (geomArray[geomArrayCount-1]->mirror)
   {
      lay = protel_layer[get_mirror_layer(atoi(lp))];
      sa = sa - geomArray[geomArrayCount-1]->rotation;
      ea = ea - geomArray[geomArrayCount-1]->rotation;
   }
   else
   {
      lay = protel_layer[atoi(lp)];
      sa = sa - geomArray[geomArrayCount-1]->rotation;
      ea = ea - geomArray[geomArrayCount-1]->rotation;
   }

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   int   laynr = Graph_Level(lay,"", 0);

   normalize_koo(&x1, &y1, geomArrayCount-1);

   if (ea < sa)   ea += 360;
   da = ea - sa;
   if (da == 360)
   {
      DataStruct *d = Graph_Circle(laynr, x1, y1, rad,
            0L, widthindex , FALSE,FALSE); 
      d->setGraphicClass(get_layerclass(lay));

   }
   else
   {
      DataStruct *d = Graph_Arc(laynr, x1, y1, rad,
            DegToRad(sa),DegToRad(da),0, // start & delta angle,flag
            widthindex, FALSE); // index of width table
      d->setGraphicClass(get_layerclass(lay));
   }

   return 1;
}

/******************************************************************************
* pfw_ca_skip
*/
static int pfw_ca_skip()
{
   char     *lp;
   double   x1, y1, rad, sa, ea;
   CString  lay;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   rad = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   sa = atof(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   ea =atof(lp);
   if (ea == 0)   ea = 360;

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // width
   double width = cnv_tok(lp);

	if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer
	int geomIndex = get_geomindex(cur_compname);
	if (geomIndex < 0)
   {
      ErrorMessage("Geom Index???", "Record=CP");
      return 0;
   }
   PfwGeom* geom = geomArray[geomIndex];

   if (geomArray[geomArrayCount-1]->mirror)
   {
      lay = protel_layer[get_mirror_layer(atoi(lp))];
      sa = sa - geom->rotation;
      ea = ea - geom->rotation;
   }
   else
   {
      lay = protel_layer[atoi(lp)];
      sa = sa - geom->rotation;
      ea = ea - geom->rotation;
   }

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

	PfwCA* ca = new PfwCA;
	geom->caArray.SetAtGrow(geom->caArray.GetCount(), ca);
	ca->geomindex = geomIndex;
	ca->x1 = x1;
	ca->y1 = y1;
	ca->radius = rad;
	ca->width = width;
	ca->startAngle = sa;
	ca->endAngle = ea;
	ca->layer = lay;

   return 1;
}

/****************************************************************************/
/*
*/
static int pfw_ff()
{
   char     *lp;
   double   x1, y1, x2, y2;
   CString  lay;

   cur_data = NULL;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   x2 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y2
   y2 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer
   lay = protel_layer[atoi(lp)];
   int   lnr = atoi(lp);

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   int   laynr = Graph_Level(lay,"", 0);

   DataStruct *d = Graph_PolyStruct(laynr,0,FALSE);
   if (lnr >= 1 && lnr <= 16)
      d->setGraphicClass(GR_CLASS_ETCH);
   else
      d->setGraphicClass(get_layerclass(lay));

   Graph_Poly(NULL,0, TRUE,0, TRUE);   // filled, void, closed
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);

   return 1;
}

/*****************************************************************************/
/*
*/
static GraphicClassTag get_layerclass(const char *layername)
{

   if (!STRCMPI(layername,prim_boardoutline))
      return graphicClassBoardOutline;
   if (!STRCMPI(layername,prim_compoutline))
      return graphicClassComponentOutline;

   return graphicClassNormal;
}

/****************************************************************************/
/*
*/
static int pfw_cf()
{
   char *lp;
   double x1, y1, x2, y2;
   CString lay;

   if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();

   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL) return p_error(); 
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // Drc

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x1
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y1
   y1 = cnv_tok(lp);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // x2
   x2 = cnv_tok(lp);
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // y2
   y2 = cnv_tok(lp);

   normalize_koo(&x1, &y1, geomArrayCount-1);
   normalize_koo(&x2, &y2, geomArrayCount-1);

   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // layer
   if (geomArray[geomArrayCount-1]->mirror)
      lay = protel_layer[get_mirror_layer(atoi(lp))];
   else
      lay = protel_layer[atoi(lp)];

   if (fgets(fPfwLine, MAX_LINE, fPfw) == NULL)
   {
      fprintf(logFp, "Syntax error at %ld \n", getInputFileLineNumber());
      display_error++;
      return -1;
   }

   incrementInputFileLineNumber();


   if ((lp = strtok(fPfwLine,QNEXTLINE)) == NULL)     return p_error(); // 0?
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // 1?
   if ((lp = strtok(NULL,QNEXTLINE)) == NULL)     return p_error(); // rotation	in degrees
   double rotation = atof(lp);

	if (fabs(rotation) > SMALLNUMBER)
	{
		CPoint2d center((x1 + x2) / 2, (y1 + y2) / 2);
		CTMatrix mat;
		mat.translate(-center);
		mat.rotateDegrees(rotation);
		mat.translate(center);
		mat.transform(x1, y1);
		mat.transform(x2, y2);
	}


   int laynr = Graph_Level(lay, "", 0);

   DataStruct *d = Graph_PolyStruct(laynr, 0, FALSE);
   d->setGraphicClass(get_layerclass(lay));

   Graph_Poly(NULL, 0, TRUE, 0, TRUE); // filled, void, closed
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);

   return 1;
}



/****************************************************************************/

static bool BlockHasData(BlockStruct *b2, DataStruct *d1)
{
   if (b2 != NULL && d1 != NULL)
   {
      POSITION pos = b2->getHeadDataPosition();
      while (pos != NULL)
      {
         DataStruct *d2 = b2->getNextData(pos);
         if (d2->getLayerIndex() == d1->getLayerIndex())
         {
            DataTypeTag dt1 = d1->getDataType();
            DataTypeTag dt2 = d2->getDataType();
            if (dt1 == dataTypeInsert && dt2 == dataTypeInsert)
            {
               InsertStruct *ins1 = d1->getInsert();
               InsertStruct *ins2 = d2->getInsert();
               if (ins1->getRefname().CompareNoCase(ins2->getRefname()) == 0)
               {
                  // SMALLNUMBER is too big, case 2043 had "same" geometry definitions
                  // with differences at .0001 resolution
                  if ((fabs(ins1->getOriginX() - ins2->getOriginX()) < 0.0005) &&
                     (fabs(ins1->getOriginY() - ins2->getOriginY()) < 0.0005))
                  {
                     // Might have to get more specific, we'll try this much for now
                     return true;
                  }
               }
            }
            else if (dt1 == dataTypePoly && dt2 == dataTypePoly)
            {
               // weak but maybe close enough
               if (d1->getPolyList()->GetCount() == d2->getPolyList()->GetCount())
               {
                  return true;
               }
            }
            else if (dt1 == dt2)
            {
               // really weak. Not an insert and not a poly, since both have "one of these"
               // we'll call it close enough for now.
               return true;
            }
         }
      }
   }

   return false;
}

/****************************************************************************/

static bool BlocksMatch(BlockStruct *b1, BlockStruct *b2)
{
   // Return true is block content matches, false otherwise. Block name does not matter.

   if (b1 != NULL && b2 != NULL)
   {
      // Check that block 2 has all that 1 has
      POSITION pos1 = b1->getHeadDataPosition();
      while (pos1 != NULL)
      {
         DataStruct *d1 = b1->getNextData(pos1);
         if (!BlockHasData(b2, d1))
            return false;
      }

      // And 1 has all that 2 has
      POSITION pos2 = b2->getHeadDataPosition();
      while (pos2 != NULL)
      {
         DataStruct *d2 = b2->getNextData(pos2);
         if (!BlockHasData(b1, d2))
            return false;
      }
   }

   return true; // no differences found
}

/****************************************************************************/

static BlockStruct *findMasterBlock(PfwGeom *newGeom)
{
   // Return pointer to a block that matches newGeom's definition, if we find one.
   // Such would be a prior existing block, and can be used in place of newGeom.
   // If none is found return NULL, meaning newGeom is new and unique.

   if (newGeom != NULL && newGeom->compdata != NULL && newGeom->compdata->getInsert() != NULL)
   {
      int blNum = newGeom->compdata->getInsert()->getBlockNumber();
      BlockStruct *newBlock = doc->getBlockAt(blNum);

      if (newBlock != NULL)
      {
         for (int i = 0; i < doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *goodBlock = doc->getBlockAt(i);
 
            if (goodBlock != newBlock)
            {
               if (BlocksMatch(doc, goodBlock, newBlock, 0.0005))
               {
                  return goodBlock;
               }
            }
         }
      }
   }

   return NULL;
}

/****************************************************************************/

static int pfw_endcomp()
{
   // Case 2043 - The essence of the fix is to let the parser initially create blocks for
   // the given component insert as it is being parsed. The block names are tagged with
   // QTempMarker. When we get here, we check if the geometry block matches one that is
   // already defined, that is what findMasterBlock() does. If we find one, we use it and
   // ignore the block we just defined. This ignored block will be purged later as an 
   // unused block. In this way, on the fly, we eliminated use of duplicated geometry
   // blocks, and we eliminate the need for a post process to create separate blocks
   // for "different pin definitions". There is more to component matching than just
   // pin definitions, that is the essence of case 2043. E.g. layers used for polystructs
   // matter too. If this scheme fails, it will most likely be due to the functions
   // BlocksMatch() and BlockHasData() not being thorough enough.

   if (cur_block)
   {
      // only do this if it had pins.
      Graph_Block_Off();
      
      int geomindex = get_geomindex(cur_compname);

		if (geomindex >= 0)
		{
			PfwGeom *geom = geomArray[geomindex];

         BlockStruct *masterBlk = findMasterBlock(geom);
         if (masterBlk != NULL)
         {
            geom->compdata->getInsert()->setBlockNumber(masterBlk->getBlockNumber());
         }
         else if (geom->pinArrayCount == 0)
			{
				cur_data->getInsert()->setInsertType(insertTypeUnknown);
				cur_block->setBlockType(BLOCKTYPE_UNKNOWN);

				int keyindex = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);

				if (is_attvalue(doc, cur_data->getAttributesRef(),ATT_TYPELISTLINK, 1)) 
					RemoveAttrib(keyindex, &cur_data->getAttributesRef());
			}

         // Fix up the block name, remove the "temp tag".
         int blknum = geom->compdata->getInsert()->getBlockNumber();
         BlockStruct *blk = doc->getBlockAt(blknum);
         if (blk != NULL)
         {
            CString blkname = blk->getName();
            if (blkname.Find(QTempMarker) > -1)
            {
               CString rawBlkName = blkname.Left(blkname.Find(QTempMarker));
               CString newBlkName = rawBlkName;
               int suffixI = 1;
               while (doc->Find_Block_by_Name(newBlkName, -1) != NULL)
               {
                  newBlkName.Format("%s_%03d", rawBlkName, suffixI++);
               }
               blk->setName(newBlkName);
            }

            if (geom->compdata->getInsert()->getInsertType() == insertTypePcbComponent && strlen(commentattribute) && strlen(geom->partnumber))
            {
               AddPartType(blk->getName(), geom->partnumber, geom->compdes);
            }
         }
		}
   }
   cur_block = NULL;
   cur_data = NULL;

   return 1;
}

/****************************************************************************/
/*
*/
static int prt_circle()
{
   double   x1,y1,r;
   char  *lp;
   char  lay[80];
   int      laynr;

   if ((lp = strtok(NULL,",")) == NULL)  return 0;
   strcpy(lay,lp);
   clean_prosa(lay);
   update_layer(lay,true);

   laynr = Graph_Level(lay,"", 0);
   if ((lp = strtok(NULL,",")) == NULL)  return 0;
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL,",")) == NULL)  return 0;
   y1 = cnv_tok(lp);
   if ((lp = strtok(NULL,",")) == NULL)  return 0;
   r = cnv_tok(lp)/2;

   Graph_Circle(laynr,x1,y1,r, 0L, 0 , FALSE, FALSE); 

   return 1;
}

/****************************************************************************/
/*
*/
static void cnv_addrot(int mrot,int *rot,int*mir)
{
   *rot = 0;
   *mir = 0;

   switch (mrot)
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
   }

   return;
}

/**** Version 3 section ****/

/******************************************************************************
* pfwv3_board
*/
static int pfwv3_board()
{
   char *lp;
   while (lp = get_string(NULL, "|"))
   {
      CString val, key;
      get_keyval(lp, &key, &val);  

      if (!key.CompareNoCase("PLANE1NETNAME") || !key.CompareNoCase("PLANE2NETNAME") ||
          !key.CompareNoCase("PLANE3NETNAME") || !key.CompareNoCase("PLANE4NETNAME"))
      {
         // plane netnames
      }
      else if (!key.CompareNoCase("ORIGINX"))
         origin_x = get_v3unit(val);
      else if (!key.CompareNoCase("ORIGINY"))
         origin_y = get_v3unit(val);
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int pfwv3_net()
{
   char     *lp;
   CString  val, key;
   int      netnr = -1;
   CString  netname;

   netname = "";

   while (lp = get_string(NULL, "|"))
   {
      get_keyval(lp, &key, &val);   

      if (!key.CompareNoCase("ID"))
      {
         netnr = atoi(val);
      }
      else if (!key.CompareNoCase("NAME"))
      {
         netname = val;
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

   if (netnr > -1)
   {
      PfwNet *net = new PfwNet;
      netArray.SetAtGrow(netArrayCount++,net);
      net->netname = netname;
      net->netnr = netnr;
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static const char *unmirror_v3layer(const char *l)
{
   int   i;

   for (i=0;i<35;i++)
   {
      if (!STRCMPI(protel_layer[i], l))
      {
         return protel_layer[get_mirror_layer(i)];
      }
   }

   return l;
}

/*****************************************************************************/
/*
*/
static double  get_v3unit(const char *l)
{
   double   d;

   if (strstr(l,"mil"))
   {
      d = atof(l) * Units_Factor(UNIT_MILS, page_unit);
   }
   else
   {
      ErrorMessage(l,"Unknown Unit");     
      d = atof(l) * Units_Factor(UNIT_MILS, page_unit);
   }

   return d;
}

/*****************************************************************************/
/*
*/
static int pfwv3_track()
{
   char     *lp;
   CString  val, key;
   double   x1, y1, x2, y2, width;
   CString  layer;
   int      compid, netid;    // either component or netid

   layer = "";
   netid = -1;
   compid = -1;

   int polygonoutline = FALSE;
   int polygon = -1;
   int subpolyindex = -1;

   while (lp = get_string(NULL, "|"))
   {
      if (get_keyval(lp, &key, &val))  
      {  
         if (!key.CompareNoCase("SUBPOLYINDEX"))
         {
            subpolyindex = atoi(val);
         }
         else if (!key.CompareNoCase("POLYGON"))
         {
            polygon = atoi(val);
         }
         else if (!key.CompareNoCase("POLYGONOUTLINE"))
         {
            polygonoutline = (val.CompareNoCase("TRUE") == 0);
         }
         else if (!key.CompareNoCase("LAYER"))
         {
            layer = val;
         }
         else if (!key.CompareNoCase("NET"))
         {
            netid = atoi(val);
         }
         else if (!key.CompareNoCase("COMPONENT"))
         {
            compid = atoi(val);
         }
         else if (!key.CompareNoCase("X1"))
         {
            x1 = get_v3unit(val) - origin_x;
         }
         else if (!key.CompareNoCase("Y1"))
         {
            y1 = get_v3unit(val) - origin_y;
         }
         else if (!key.CompareNoCase("X2"))
         {
            x2 = get_v3unit(val) - origin_x;
         }
         else  if (!key.CompareNoCase("Y2"))
         {
            y2 = get_v3unit(val) - origin_y;
         }
         else if (!key.CompareNoCase("WIDTH"))
         {
            width = get_v3unit(val);
         }
         else
         {
#ifdef DEBUG_UNKNOWN_KEY
            fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
            display_error++;
#endif
         }
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }


   // if do not translate hatch lines
   if (!TRANSLATE_POLYGONS)
   {
      if (polygon > -1)
         return 0;
   }
   if (!TRANSLATE_HATCHLINES)
   {
      if (polygon > -1 && polygonoutline == FALSE)
         return 0;
   }

   if (compid > -1)
   {

      int geomlistptr = get_geomlistptr(compid);

      if (geomlistptr < 0)
      {
         ErrorMessage("Geomlist ptr ???", "Record=TRACK");
         return 0;
      }

      if (geomArray[geomlistptr]->master)
      {
         normalize_koo(&x1, &y1, geomlistptr);
         normalize_koo(&x2, &y2, geomlistptr);

         if (geomArray[geomlistptr]->mirror)
         {
            layer = unmirror_v3layer(layer);
         }

         Graph_Block_On(GBO_APPEND,geomArray[geomlistptr]->shapename,-1,0);

         // must be a nettrace
         int fill = 0;
         int close = 0;
         int widthindex;

         if (width == 0)
            widthindex = 0;   // make it small width.
         else
         {
            int   err;
            widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         }

         int lindex = Graph_Level(layer,"", 0);
         DataStruct *d = Graph_PolyStruct(lindex,0,FALSE);
         d->setGraphicClass(get_layerclass(layer));
         Graph_Poly(NULL,widthindex, fill,0, close);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);

         Graph_Block_Off();
      } // if master
   }
   else
   {
      // must be a nettrace
      int fill = 0;
      int close = 0;
      int widthindex;

      if (width == 0)
         widthindex = 0;   // make it small width.
      else
      {
         int   err;
         widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      }

      int lindex = Graph_Level(layer,"", 0);
      DataStruct *d = Graph_PolyStruct(lindex,0,FALSE);
      d->setGraphicClass(get_layerclass(layer));

      CPoly *cp = Graph_Poly(NULL,widthindex, fill,0, close);
      if (polygon > -1 && polygonoutline == FALSE) 
         cp->setHatchLine(true);

      Graph_Vertex(x1, y1, 0.0);
      Graph_Vertex(x2, y2, 0.0);

      if (netid > -1)
      {
         CString  cur_netname;
         cur_netname = get_netname(netid);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,
            cur_netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
         d->setGraphicClass(GR_CLASS_ETCH);
      }

#ifdef _DEBUG
      CString w;
      if (polygon > -1)
      {
         w.Format("%d", polygon);
         doc->SetUnknownAttrib(&d->getAttributesRef(), "POLYGON", w, SA_OVERWRITE, NULL); // x, y, rot, height
      }
      if (subpolyindex > -1)
      {
         w.Format("%d", subpolyindex);
         doc->SetUnknownAttrib(&d->getAttributesRef(), "SUBPOLY", w, SA_OVERWRITE, NULL); // x, y, rot, height
         
      }
#endif
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int pfwv3_fill()
{
   char     *lp;
   CString  val, key;
   double   x1, y1, x2, y2, width, rot;
   CString  layer;
   int      compid, netid;    // either component or netid

   layer = "";
   netid = -1;
   compid = -1;
	x1 = y1 = x2 = y2 = width = rot = 0.0;

   while (lp = get_string(NULL, "|"))
   {
      if (get_keyval(lp, &key, &val))  
      {   
         if (!key.CompareNoCase("LAYER"))
         {
            layer = val;
         }
         else  if (!key.CompareNoCase("NET"))
         {
            netid = atoi(val);
         }
         else   if (!key.CompareNoCase("COMPONENT"))
         {
            compid = atoi(val);
         }
         else  if (!key.CompareNoCase("X1"))
         {
            x1 = get_v3unit(val) - origin_x;
         }
         else if (!key.CompareNoCase("Y1"))
         {
            y1 = get_v3unit(val) - origin_y;
         }
         else if (!key.CompareNoCase("X2"))
         {
            x2 = get_v3unit(val) - origin_x;
         }
         else if (!key.CompareNoCase("Y2"))
         {
            y2 = get_v3unit(val) - origin_y;
         }
         else if (!key.CompareNoCase("WIDTH"))
         {
            width = get_v3unit(val);
         }
         else if (!key.CompareNoCase("ROTATION"))
         {
            rot = atof(val);
         }
         else
         {
#ifdef DEBUG_UNKNOWN_KEY
            fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
            display_error++;
#endif
         }
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }


   PfwGeom *geom = NULL;
	bool addGraphOn = false;

   if (compid > -1)
   {
      int geomIndex = get_geomlistptr(compid);

      if (geomIndex < 0)
      {
         ErrorMessage("Geom Index???", "Record=FILL");
         return 0;
      }

		// Find the geometry where this pin belong
      PfwGeom *geom = geomArray[geomIndex];

		// Add the graphic to the geometry if the geometry is a master copy
      if (geom->master == 1)
      {
			normalize_koo(&x1, &y1, geomIndex);
			normalize_koo(&x2, &y2, geomIndex);
			rot = normalize_rot(geom->rotation - rot);

			addGraphOn = (Graph_Block_On(GBO_APPEND, geom->shapename, -1, 0) != NULL);
		}
	}

   int fill = TRUE;
   int close = TRUE;
   int widthindex;

   if (width == 0)
      widthindex = 0;   // make it small width.
   else
   {
      int   err;
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   }

	// we need to calculate the poly using the rotation that was read for this filled item
	CPoint2d center((x1 + x2) / 2, (y1 + y2) / 2);
	CTMatrix mat;
	mat.translate(-center);
	mat.rotateDegrees(rot);
	mat.translate(center);
	mat.transform(x1, y1);
	mat.transform(x2, y2);

	int lindex = Graph_Level(layer,"", 0);
	DataStruct *d = Graph_PolyStruct(lindex,0,FALSE);
	d->setGraphicClass(get_layerclass(layer));
	CPoly *cp = Graph_Poly(NULL,widthindex, fill,0, close);

	Graph_Vertex(x1, y1, 0.0);
	Graph_Vertex(x2, y1, 0.0);
	Graph_Vertex(x2, y2, 0.0);
	Graph_Vertex(x1, y2, 0.0);
	Graph_Vertex(x1, y1, 0.0);

	if (netid > -1)
	{
		CString  cur_netname;
		cur_netname = get_netname(netid);
		doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
			VT_STRING,cur_netname.GetBuffer(0),SA_APPEND, NULL); // x, y, rot, height
		d->setGraphicClass(GR_CLASS_ETCH);
	}

	if (addGraphOn)
      Graph_Block_Off();

   //{
   //   // must be a nettrace
   //   DataStruct *d = Graph_PolyStruct(lindex,0,FALSE);
   //   d->setGraphicClass(get_layerclass(layer));
   //   CPoly *cp = Graph_Poly(NULL,widthindex, fill,0, close);

   //   Graph_Vertex(x1, y1, 0.0);
   //   Graph_Vertex(x2, y1, 0.0);
   //   Graph_Vertex(x2, y2, 0.0);
   //   Graph_Vertex(x1, y2, 0.0);
   //   Graph_Vertex(x1, y1, 0.0);

   //   if (netid > -1)
   //   {
   //      CString  cur_netname;
   //      cur_netname = get_netname(netid);
   //      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
   //         VT_STRING,cur_netname.GetBuffer(0),SA_APPEND, NULL); // x, y, rot, height
   //      d->setGraphicClass(GR_CLASS_ETCH);
   //   }
   //}

   return 1;
}

/******************************************************************************
* get_geomlistptr
*/
static int get_geomlistptr(int compid)
{
   for (int i=0; i<geomArrayCount; i++)
   {
      if (geomArray[i]->compid == compid)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pfwv3_arc()
{
   char     *lp;
   double   x, y, rad, sa, da, ea, width;
   int      widthindex;
   CString  layer;
   CString  val, key;
   int      compid, netid;    // either component or netid

   layer = "";
   netid = -1;
   compid = -1;

   while (lp = get_string(NULL, "|"))
   {
      if (get_keyval(lp, &key, &val)) 
      {   
         if (!key.CompareNoCase("LAYER"))
         {
            layer = val;
         }
         else if (!key.CompareNoCase("NET"))
         {
            netid = atoi(val);
         }
         else if (!key.CompareNoCase("COMPONENT"))
         {
            compid = atoi(val);
         }
         else if (!key.CompareNoCase("LOCATION.X"))
         {
            x = get_v3unit(val) - origin_x;
         }
         else if (!key.CompareNoCase("LOCATION.Y"))
         {
            y = get_v3unit(val) - origin_y;
         }
         else if (!key.CompareNoCase("RADIUS"))
         {
            rad = get_v3unit(val);
         }
         else if (!key.CompareNoCase("STARTANGLE"))
         {
            sa = atof(val);
         }
         else if (!key.CompareNoCase("ENDANGLE"))
         {
            ea = atof(val);
         }
         else if (!key.CompareNoCase("WIDTH"))
         {
            width = get_v3unit(val);
         }
         else
         {
#ifdef DEBUG_UNKNOWN_KEY
            fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
            display_error++;
#endif
         }
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

   int err;

   if (width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   if (compid > -1)
   {

      int geomlistptr = get_geomlistptr(compid);

      if (geomlistptr < 0)
      {
         ErrorMessage("Geomlist ptr ???", "Record=TRACK");
         return 0;
      }

      if (geomArray[geomlistptr]->master)
      {
         if (geomArray[geomlistptr]->mirror)
         {
            layer = unmirror_v3layer(layer);
            sa = sa - geomArray[geomlistptr]->rotation;
            ea = ea - geomArray[geomlistptr]->rotation;
         }
         else
         {
            sa = sa - geomArray[geomlistptr]->rotation;
            ea = ea - geomArray[geomlistptr]->rotation;
         }

         normalize_koo(&x, &y, geomlistptr);

         Graph_Block_On(GBO_APPEND,geomArray[geomlistptr]->shapename,-1,0);

         int lindex = Graph_Level(layer,"", 0);

         if (ea < sa)   ea += 360;
         da = ea - sa;
         if (da == 360)
         {
            DataStruct *d = Graph_Circle(lindex, x, y, rad,
               0L, widthindex , FALSE,FALSE); 
            d->setGraphicClass(get_layerclass(layer));
         }
         else
         {
            DataStruct *d = Graph_Arc(lindex, x, y, rad,
               DegToRad(sa),DegToRad(da),0, // start & delta angle,flag
            widthindex, FALSE); // index of width table
            d->setGraphicClass(get_layerclass(layer));
         }
         Graph_Block_Off();
      } // master
   } // compid
   else
   {
      int lindex = Graph_Level(layer,"", 0);
      DataStruct *d;

      if (ea < sa)   ea += 360;
      da = ea - sa;
      if (da == 360)
      {
         d = Graph_Circle(lindex, x, y, rad,
            0L, widthindex , FALSE,FALSE); 
         d->setGraphicClass(get_layerclass(layer));
      }
      else
      {
         d = Graph_Arc(lindex, x, y, rad,
            DegToRad(sa),DegToRad(da),0, // start & delta angle,flag
         widthindex, FALSE); // index of width table
         d->setGraphicClass(get_layerclass(layer));
      }
      if (netid > -1)
      {
         CString  cur_netname;
         cur_netname = get_netname(netid);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING,cur_netname.GetBuffer(0),SA_APPEND, NULL); // x, y, rot, height
         d->setGraphicClass(GR_CLASS_ETCH);
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pfwv3_text()
{
   char     *lp;
   double   x, y, rot, penwidth, height;
   char     mirror = FALSE;
   CString  layer, prosa;
   CString  val, key;
   int      compid, netid;    // either component or netid
   int      designator = FALSE, comment = FALSE;

   layer = "";
   netid = -1;
   compid = -1;

   while (lp = get_string(NULL, "|"))
   {
      get_keyval(lp, &key, &val);
  
      if (!key.CompareNoCase("LAYER"))
      {
         layer = val;
      }
      else if (!key.CompareNoCase("NET"))
      {
         netid = atoi(val);
      }
      else if (!key.CompareNoCase("COMPONENT"))
      {
         compid = atoi(val);
      }
      else if (!key.CompareNoCase("TEXT"))
      {
         prosa = val;
      }
      else if (!key.CompareNoCase("X"))
      {
         x = get_v3unit(val) - origin_x;
      }
      else if (!key.CompareNoCase("Y"))
      {
         y = get_v3unit(val) - origin_y;
      }
      else if (!key.CompareNoCase("ROTATION"))
      {
         rot = normalize_rot(atof(val));
      }
      else if (!key.CompareNoCase("MIRROR"))
      {
         mirror = FALSE;
         if (val.CompareNoCase("TRUE") == 0)
            mirror = TRUE;
      }
      else if (!key.CompareNoCase("DESIGNATOR"))
      {
         if (val.CompareNoCase("TRUE") == 0)
            designator = TRUE;
      }
      else if (!key.CompareNoCase("COMMENT"))
      {
         if (val.CompareNoCase("TRUE") == 0)
            comment = TRUE;
      }
      else if (!key.CompareNoCase("WIDTH"))
      {
         penwidth = get_v3unit(val);
      }
      else if (!key.CompareNoCase("HEIGHT"))
      {
         height = get_v3unit(val);
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Board at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

   if (compid > -1)
   {

      int geomlistptr = get_geomlistptr(compid);

      if (geomlistptr < 0)
      {
         ErrorMessage("Geomlist ptr ???", "Record=TEXT");
         return 0;
      }

      if (geomArray[geomlistptr]->mirror)
      {
         layer = unmirror_v3layer(layer);
      }
      rot = normalize_rot(rot - geomArray[geomlistptr]->rotation);

      normalize_koo(&x, &y, geomlistptr);

      int lindex = Graph_Level(layer,"", 0);

      if (designator)
      {
         geomArray[geomlistptr]->desfound = TRUE;
         geomArray[geomlistptr]->compdes = prosa;
         geomArray[geomlistptr]->desx = x;
         geomArray[geomlistptr]->desy = y;
         geomArray[geomlistptr]->desrot = rot;
         geomArray[geomlistptr]->desheight = height;
         geomArray[geomlistptr]->deslindex = lindex;
      }
      else if (comment)
      {
         geomArray[geomlistptr]->commentfound = TRUE;
         geomArray[geomlistptr]->comment = prosa;
         geomArray[geomlistptr]->commentx = x;
         geomArray[geomlistptr]->commenty = y;
         geomArray[geomlistptr]->commentrot = rot;
         geomArray[geomlistptr]->commentheight = height;
         geomArray[geomlistptr]->commentlindex = lindex;
      }
      else
      {
         if (geomArray[geomlistptr]->master)
         {
            Graph_Block_On(GBO_APPEND,geomArray[geomlistptr]->shapename,-1,0);
            DataStruct *d = Graph_Text(lindex,prosa,x,y,
                  height,height*TEXTRATIO,DegToRad(rot),
                  0L, TEXT_PROPORTIONAL,     // protel has proportional text
                  mirror,  
                  0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
            Graph_Block_Off();
         }
      } // master
   } // compid
   else
   {
      int lindex = Graph_Level(layer,"", 0);
      DataStruct *d = Graph_Text(lindex,prosa,x,y,
            height,height*TEXTRATIO,DegToRad(rot),
            0L, TEXT_PROPORTIONAL,     // protel has proportional text
            mirror,  
            0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int make_blind_via(const char *padname, double top, double inner, double bottom, 
                          double drill, const char *startlayer, const char *endlayer)
{
   BlockStruct *b = Graph_Block_On(GBO_APPEND,padname,-1,0);
   b->setBlockType(BLOCKTYPE_PADSTACK);

   CString  name;
   int      err;
   name.Format("%s_%1.3lf_%1.3lf",protel_padform[0],inner, inner );
   Graph_Aperture(name, get_cc_form(0, inner, inner), inner, inner, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);

   int write_pad = FALSE;
   for (int i = 1;i<=16;i++)
   {
      CString l;
      
      l.Format("V %s", protel_layer[i]);  // here use a funny layer, because later I have to kill 
                                          // unused inner layers !

      if (!STRCMPI(startlayer, protel_layer[i]))   write_pad = TRUE;

      if (write_pad)
      {

         int layerindex= Graph_Level(l, "", 0);
         Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      }
      if (!STRCMPI(endlayer, protel_layer[i]))     write_pad = FALSE;
   }


   if (drill > 0)
   {
      CString  drillname, drilllayer;

      drilllayer.Format("%s_%s_%s", protel_layer[33], startlayer, endlayer);
      int layernum = Graph_Level(drilllayer,"",0);

      int dindex = get_drillindex(drill, layernum);
      drillname.Format("DRILL_%d",dindex);
      Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
   }


   doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE),
         VT_NONE, NULL, SA_OVERWRITE, NULL); //  


   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* pfwv3_via
*/
static int pfwv3_via()
{
   char     *lp;
   double   x = 0.0, y = 0.0, diam = 0.0, hole = 0.0;
   CString  layer, startlayer, endlayer;
   CString  val, key;
   int      compid, netid;    // either component or netid
   bool addTestPointAttrTop = false;
   bool addTestPointAttrBottom = false;

	//case 1015: added the CPEV & CSEV commad reading
	
	int    cpev = 1;
	int    csev = 1;
	double cpe = 1;
	double cse = 1;

   layer = "";
   startlayer = "TOP";
   endlayer = "BOTTOM";
   netid = -1;
   compid = -1;



   while (lp = get_string(NULL, "|"))
   {
      get_keyval(lp, &key, &val); 

      if (!key.CompareNoCase("STARTLAYER"))
         startlayer = val;
      else if (!key.CompareNoCase("ENDLAYER"))
         endlayer = val;
      else if (!key.CompareNoCase("LAYER"))
         layer = val;
      else if (!key.CompareNoCase("NET"))
         netid = atoi(val);
      else if (!key.CompareNoCase("COMPONENT"))
         compid = atoi(val);
      else if (!key.CompareNoCase("X"))
         x = get_v3unit(val) - origin_x;
      else if (!key.CompareNoCase("Y"))
         y = get_v3unit(val) - origin_y;
      else if (!key.CompareNoCase("DIAMETER"))
         diam = get_v3unit(val);
      else if (!key.CompareNoCase("HOLESIZE"))
         hole = get_v3unit(val);
      else if (!key.CompareNoCase("CPEV"))
         cpev = atoi(val);
      else if (!key.CompareNoCase("CSEV"))
			csev = atoi(val);
      else if (!key.CompareNoCase("CPE"))
		{
			cpe = get_v3unit(val);
		}
      else if (!key.CompareNoCase("CSE"))
		{
			cse = get_v3unit(val);
      }
      else if (!key.CompareNoCase("TESTPOINT_TOP"))
      {
         addTestPointAttrTop = true;
      }
      else if (!key.CompareNoCase("TESTPOINT_BOTTOM"))
      {
         addTestPointAttrBottom = true;
      }
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Via at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
   }

   CString  pshapename;
   int      pindex;

	//case 1015: if the rule is set to 1, use global (defined as glb_CSEV_val), o/w use local cpe/cse
	//the local one is read above in the data line of thi pad.
	if (csev == 0)      
		cse = 0.0;	
	else if (csev == 1)    
		cse = glb_CSEV_val;

	if (cpev == 0)      
		cpe = 0.0;
	else if (cpev == 1)      
		cpe = glb_CPEV_val;

   // if not start on top and end on bottom
   if (startlayer.CompareNoCase("TOP") || endlayer.CompareNoCase("BOTTOM"))
   {
      // here make a natural padform. This is unique for top, mid, bottom.
      pindex = get_padformindex(0.0, 0.0, 0, diam, diam, 0, diam, diam, 0, diam, diam, 0, hole, 0.0, 0.0, "MULTILAYER");

      // this via now is a subset, meaning that the pindex is oK and we just have to filter the layers.
      pshapename.Format("VIA_%s_%s_%d", startlayer, endlayer, pindex);
      if (Graph_Block_Exists(doc, pshapename, -1) == NULL)
         make_blind_via(pshapename, diam, diam, diam, hole, startlayer, endlayer);
   }
   else
   {
      pshapename = get_cczpadstackname(0.0, 0.0, 0, diam, diam, 0, diam, diam, 0, diam, diam, 0, hole, cse, cpe, "MULTILAYER");
   }


   if (compid > -1)
   {
      int geomlistptr = get_geomlistptr(compid);

      if (geomlistptr < 0)
      {
         ErrorMessage("Geomlist ptr ???", "Record=VIA");
         return 0;
      }

      if (geomArray[geomlistptr]->master)
      {
         normalize_koo(&x, &y, geomlistptr);
         Graph_Block_On(GBO_APPEND,geomArray[geomlistptr]->shapename,-1,0);
         Graph_Level(layer,"", 0);
         DataStruct *data = Graph_Block_Reference(pshapename, NULL, 0, x, y, DegToRad(0.0), 0, 1.0, -1, TRUE);
         Graph_Block_Off();

         CString val((addTestPointAttrTop && addTestPointAttrBottom) ? "BOTH" : (addTestPointAttrTop) ? "TOP" : (addTestPointAttrBottom) ? "BOTTOM" : "" );
         if (!val.IsEmpty())
            doc->SetUnknownAttrib(&data->getAttributesRef(),"TEST",val.GetBuffer(0), attributeUpdateOverwrite, NULL); 
      }
   }
   else
   {
      DataStruct *data = Graph_Block_Reference(pshapename, NULL, 0, x, y, DegToRad(0.0), 0, 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypeVia);

      if (netid > -1)
      {
         CString  cur_netname;
         cur_netname = get_netname(netid);
         doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname.GetBuffer(0),SA_APPEND, NULL);         
      }

      CString val((addTestPointAttrTop && addTestPointAttrBottom) ? "BOTH" : (addTestPointAttrTop) ? "TOP" : (addTestPointAttrBottom) ? "BOTTOM" : "" );
      if (!val.IsEmpty())
         doc->SetUnknownAttrib(&data->getAttributesRef(),"TEST",val.GetBuffer(0), attributeUpdateOverwrite, NULL); 
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int pfwv3_polygon()
{
   // not needed record
   return 1;
}


/*****************************************************************************/
/*
*/
static int pfwv3_skiprecord()
{
   // not needed record
   return 1;
}

/******************************************************************************
* skip_to_endcomp
*/
static int skip_to_endcomp(const char *geomname)
{
   char     *lp;
   int      i, found = -1;

   for (i=0;i<geomArrayCount;i++)
   {
      if (geomArray[i]->master == 0)   continue;
      if (geomArray[i]->shapename.CompareNoCase(geomname) == 0)
      {
         found = i;
         break;
      }
   }

   if (found < 0)
   {
      fprintf(logFp, "Can not find a Master Component Definition for [%s]\n", geomname);
      display_error++;
   }

   while (TRUE)
   {
      if (fgets(fPfwLine,MAX_LINE,fPfw) == NULL)
      {
         fprintf(logFp, "Syntax error at %ld \n",getInputFileLineNumber());
         display_error++;
         return -1;
      }

      incrementInputFileLineNumber();

      if ((lp = strtok(fPfwLine,QNEXTLINE)) != NULL) 
      {
         if (!STRCMPI(lp, "CP"))
            pfw_cp_skip();
         else  if (!STRCMPI(lp, "CT"))   
				pfw_ct_skip();
			else if (!STRCMPI(lp, "CA"))
				pfw_ca_skip();
         else if (!STRCMPI(lp, "ENDCOMP"))   
         {
            cur_data = NULL;
            return 1;
         }
      }
   }

   return 0;
}

/******************************************************************************
*/

static void getCczLayersForPfwPair(int layerpair, CString &fromLayerName, CString &toLayerName)
{
   fromLayerName.Empty();
   toLayerName.Empty();

   // This table of layer mappings for layerpair is from a Protel HELP file.
   //-----------------------------------------------------------------
   //   Layer pair
	//   Defines the via type as Through Hole or Blind/Buried (0-8).
   //
   //      0 = Multilayer (all layers)
	//      1 = Top - Mid 1 pair
	//      2 = Mid 2 - Mid 3 pair
   //      3 = Mid 4 - Mid 5 pair
   //      4 = Mid 6 - Mid 7 pair
   //      5 = Mid 8 - Mid 9 pair
   //      6 = Mid 10 - Mid 11 pair
   //      7 = Mid 12 - Mid 13 pair
   //      8 = Mid 14 - Bottom pair
   //-----------------------------------------------------------------
   // Our purpose is for pads.
   // Pads on top or bottom will go to PADTOP or PADBOTTOM, respetively.
   // Pads on mid-layers will go on the mid-layer (Mid1, Mid2, ..., Mid14.
   // 
   // Special case for bottom-handling. There are not always 14 mid layers in a PCB.
   // So if a layerpair is set in the middle, and that ending midlayer does not 
   // exist, we use bottom.
   // E.g. layerpair = 4 is for midlayers 6 to 7.
   // Say we have an 8 layer board, then top is 1, there are 6 mid layers, and
   // then bottom. So 4 would map to the pair mid6 and bottom.

   switch (layerpair)
   {
   case 1:
      fromLayerName = QPADTOP;
      toLayerName   = protel_layer[2];
      break;
   case 2:
   case 3:
   case 4:
   case 5:
   case 6:
   case 7:
      {
         fromLayerName = "Error1";
         toLayerName = "Error2";
         int start = (layerpair * 2) - 1;
         int end = start + 1;
         if (start > 1 && start < 16)  // range of midlayer names in protel_layers[]
            fromLayerName = protel_layer[start];
         if (end > 1 && end < 16)     // range of midlayer names in protel_layers[]
            toLayerName = protel_layer[end];
         // If bot midlayer is out of range, change to PADBOT
         if (doc->FindLayer_by_Name(toLayerName) == NULL)
            toLayerName = QPADBOT;
      }
      break;

   case 8:
      fromLayerName = protel_layer[15];
      toLayerName   = QPADBOT;
      break;

   case 0:   // Thru all layers of PCB
   default:  // We'll default to that too.
      fromLayerName = QPADTOP;
      toLayerName   = QPADBOT;
      break;
   }
}

/******************************************************************************
*/
static CString get_cczpadstackname(double ax, double ay, int ashape, 
                            double topx, double topy, int topshape, 
                            double midx, double midy, int midshape, 
                            double botx, double boty, int botshape, 
                            double drill, 
                            double soldermask_expansion, double pastemask_expansion, 
                            CString layer, int layerpair)
{
      int pindex = get_padformindex(ax, ay, ashape, topx, topy, topshape, midx, midy, midshape,
         botx, boty, botshape, drill, soldermask_expansion, pastemask_expansion, layer, layerpair);

      CString pshapename;
      pshapename.Format("PADSTACK_%d",pindex);

      return pshapename;
}

/******************************************************************************
*/
static int get_padformindex(double ax, double ay, int ashape, 
                            double topx, double topy, int topshape, 
                            double midx, double midy, int midshape, 
                            double bottomx, double bottomy, int bottomshape, 
                            double drill, 
                            double soldermask_expansion, double pastemask_expansion, 
                            CString layer, int layerpair)
{
   CString topGraphLayer;
   CString midGraphLayer("PADINT"); // default for most through padstacks

   // make sure shape is 0..3 that seems all what Protel V2.8 can do
   ashape = ashape % 4;
   topshape = topshape % 4;
   midshape = midshape % 4;
   bottomshape = bottomshape % 4;

   if (!STRCMPI(layer, "TOP"))
   {
      if (topx == 0.0)
         topx = ax;
      if (topy == 0.0)
         topy = ay;
      if (topshape == -1)
         topshape = ashape;
      midx = midy = 0;
      bottomx = bottomy = 0;
	  topGraphLayer = QPADTOP;
   }
   else if (!STRCMPI(layer, "BOTTOM"))
   {
      if (bottomx == 0.0)
         bottomx = ax;
      if (bottomy == 0.0)
         bottomy = ay;
	  if (bottomshape == -1)
         bottomshape = ashape;
      midx = midy = 0;
      topx = topy = 0;
   }
   else if ((STRCMPI(layer, "MULTILAYER") == 0) && (layerpair < 1))  // layerpair = 0 means all layers, == -1 means not in use
   {
      if (topx == 0.0)
         topx = ax;
      if (topy == 0.0)
         topy = ay;
      if (topshape == -1)
         topshape = ashape;
      topGraphLayer = QPADTOP;
      if (midx == 0.0)
         midx = ax;
      if (midy == 0.0)
         midy = ay;
      if (midshape == -1)
         midshape = ashape;
      if (bottomx == 0.0)
         bottomx = ax;
      if (bottomy == 0.0)
         bottomy = ay;
      if (bottomshape == -1)
         bottomshape = ashape;
   }
   else if (layerpair > 0)
   {
      // Layer Pair - Single value indicates specific layer pair.
      // For these we will always use top for the upper-most layer in pair,
      // and will use mid for the lower-most layer. Bottom is not used, so zero it out.
      bottomx = 0.;
      bottomy = 0.;
      bottomshape = 0;

      CString fromLayerName, toLayerName;
      getCczLayersForPfwPair(layerpair, fromLayerName, toLayerName);

      // This is the tricky part, get the layer names.
      topGraphLayer = fromLayerName;
      midGraphLayer = toLayerName;
   }
   else
   {
      if (topx == 0.0)
         topx = ax;
      if (topy == 0.0)
         topy = ay;
      if (topshape == -1)
         topshape = ashape;
      midx = midy = 0;
      bottomx = bottomy = 0;
      pastemask_expansion = 0.0;
      soldermask_expansion = 0.0;
	   topGraphLayer = layer;
   }

   PFWPadstack p(topx, topy, topshape,
      midx, midy, midshape,
      bottomx, bottomy, bottomshape,
      drill, soldermask_expansion, pastemask_expansion,
      layer, layerpair);

   int padformIndx = pfwPadstackArray.FindIndex(&p); // Look for existing one that is already like p

   if (padformIndx >= 0)
   {
      return padformIndx;
   }

   int      err, smd = 0;
   CString  padname, name;
   int nextIndx = pfwPadstackArray.getNextIndex();
   padname.Format("PADSTACK_%d", nextIndx);

   BlockStruct *b = Graph_Block_On(GBO_APPEND, padname, -1, 0);
   b->setBlockType(BLOCKTYPE_PADSTACK);
   if (topx > 0 && topy > 0)
   {
      name.Format("%s_%1.3lf_%1.3lf",protel_padform[topshape],topx, topy );
      Graph_Aperture(name, get_cc_form(topshape, topx, topy), topx, topy, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);
      int layerindex= Graph_Level(topGraphLayer, "", 0);
      Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      smd |= 1;

      if (soldermask_expansion != 0)
      {
         double mask_expansion( soldermask_expansion * 2.0 );
         name.Format("%s_%1.3lf_%1.3lf",protel_padform[topshape],topx+mask_expansion, 
                                                                 topy+mask_expansion);
         Graph_Aperture(name, get_cc_form(topshape, topx+mask_expansion, topy+mask_expansion), 
            topx+mask_expansion, topy+mask_expansion, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);
         int layerindex= Graph_Level("TOPSOLDER", "", 0);
         Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      }

      if (pastemask_expansion != 0)
      {
         name.Format("%s_%1.3lf_%1.3lf",protel_padform[topshape],topx+pastemask_expansion, 
                                                                 topy+pastemask_expansion);
         Graph_Aperture(name, get_cc_form(topshape, topx+pastemask_expansion, topy+pastemask_expansion), 
            topx+pastemask_expansion, topy+pastemask_expansion, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);
         int layerindex= Graph_Level("TOPPASTE", "", 0);
         Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      }
   }

   if (midx > 0 && midy > 0)
   {
      name.Format("%s_%1.3lf_%1.3lf",protel_padform[midshape],midx, midy );
      Graph_Aperture(name, get_cc_form(midshape, midx, midy), midx, midy, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);
      int layerindex= Graph_Level(midGraphLayer, "", 0);
      Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      smd |=3;
   }

   if (bottomx > 0 && bottomy > 0)
   {
      name.Format("%s_%1.3lf_%1.3lf",protel_padform[bottomshape],bottomx, bottomy );
      Graph_Aperture(name, get_cc_form(bottomshape, bottomx, bottomy), 
            bottomx, bottomy, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);
      int layerindex= Graph_Level(QPADBOT, "", 0);
      Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      smd |=2;

      if (soldermask_expansion > 0)
      {
         double mask_expansion( soldermask_expansion * 2.0 );
         name.Format("%s_%1.3lf_%1.3lf",protel_padform[bottomshape],bottomx+mask_expansion, 
                                                                    bottomy+mask_expansion);
         Graph_Aperture(name, get_cc_form(bottomshape, bottomx+mask_expansion, bottomy+mask_expansion), 
            bottomx+mask_expansion, bottomy+mask_expansion, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);
         int layerindex= Graph_Level("BOTTOMSOLDER", "", 0);
         Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      }

      if (pastemask_expansion > 0)
      {
         name.Format("%s_%1.3lf_%1.3lf",protel_padform[bottomshape],bottomx+pastemask_expansion, 
                                                                    bottomy+pastemask_expansion);
         Graph_Aperture(name, get_cc_form(bottomshape, bottomx+pastemask_expansion, bottomy+pastemask_expansion), 
            bottomx+pastemask_expansion, bottomy+pastemask_expansion, 0.0, 0.0, 0, 0, 
            BL_APERTURE, TRUE, &err);
         int layerindex= Graph_Level("BOTTOMPASTE", "", 0);
         Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
      }
   }

   if (drill > 0)
   {
      CString  drillname;
      int layernum = Graph_Level(protel_layer[33],"",0);
      int dindex = get_drillindex(drill, layernum);
      drillname.Format("DRILL_%d",dindex);
      Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
      smd |= 3;
   }

   if (smd == 1)
   {
      doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
         VT_NONE, NULL, SA_OVERWRITE, NULL); //  
   }

   Graph_Block_Off();

   pfwPadstackArray.Add(p);  

   return nextIndx;
}

//----------------------------------------------------------------------------

PFWPadstack::PFWPadstack(double topx, double topy, int topshape,
                         double midx, double midy, int midshape,
                         double botx, double boty, int botshapem,
                         double drill, double solderexp, double pasteexp,
                         CString layer, int layerpair)
: m_topx(0.)
, m_topy(0.)
, m_topshape(0)
, m_midx(0.)
, m_midy(0.)
, m_midshape(0)
, m_botx(0.)
, m_boty(0.)
, m_botshape(0)
, m_drill(0.)
, m_soldermask_expansion(0.)
, m_pastemask_expansion(0.)
, m_layerpair(layerpair)  // valid values are 0..8,  -1 == not used

{
}

//----------------------------------------------------------------------------

PFWPadstack::PFWPadstack(PFWPadstack &other)
: m_topx(other.m_topx)
, m_topy(other.m_topy)
, m_topshape(other.m_topshape)
, m_midx(other.m_midx)
, m_midy(other.m_midy)
, m_midshape(other.m_midshape)
, m_botx(other.m_botx)
, m_boty(other.m_boty)
, m_botshape(other.m_botshape)
, m_drill(other.m_drill)
, m_soldermask_expansion(other.m_soldermask_expansion)
, m_pastemask_expansion(other.m_pastemask_expansion)
, m_layer(other.m_layer)
, m_layerpair(other.m_layerpair)
{
}

//----------------------------------------------------------------------------

//PFWPadstack *PFWPadstackArray::Find(PFWPadstack *pattern)
int PFWPadstackArray::FindIndex(PFWPadstack *pattern)
{
   if (pattern != NULL)
   {
      for (int i = 0; i < this->m_padstacks.GetCount(); i++)
      {
         PFWPadstack *p = this->m_padstacks.GetAt(i);
         if (p == pattern)
            return i;//p;         
      }
   }

   return -1;//NULL;
}

//----------------------------------------------------------------------------

 bool PFWPadstack::operator==(PFWPadstack &other)
 {
    if (this->m_topx        != other.m_topx)        return false;
    if (this->m_topy        != other.m_topy)        return false;
    if (this->m_topshape    != other.m_topshape)    return false;
    if (this->m_midx        != other.m_midx)        return false;
    if (this->m_midy        != other.m_midy)        return false;
    if (this->m_midshape    != other.m_midshape)    return false;
    if (this->m_botx        != other.m_botx)        return false;
    if (this->m_boty        != other.m_boty)        return false;
    if (this->m_botshape    != other.m_botshape)    return false;
    if (this->m_drill       != other.m_drill)       return false;
    if (this->m_soldermask_expansion != other.m_soldermask_expansion) return false; 
    if (this->m_pastemask_expansion  != other.m_pastemask_expansion)  return false;
    if (this->m_layer     != other.m_layer)         return false;
    if (this->m_layerpair != other.m_layerpair)     return false;

    // All members match
    return true;
 }

/******************************************************************************
* normalize_koo
*/
static void normalize_koo(double *x, double *y, int geomIndex)
{
   PfwGeom *geom = geomArray[geomIndex];

   double newX = *x - geom->centerx;
   double newY = *y - geom->centery;

   double xRot, yRot;

   //mirror
   if (geom->mirror)
   {
      newX = -newX;
      Rotate(newX, newY, geom->rotation, &xRot, &yRot); 
   }
   else
   {
      Rotate(newX, newY, 360 - geom->rotation, &xRot, &yRot); 
   }

   *x = xRot;
   *y = yRot;
}


/******************************************************************************
* unmirrorlayer
*/
static int unmirrorlayer(int lay)
{
   if (lay == 18) return 17;
   if (lay == 20) return 19;
   if (lay == 21) return 20;
   if (lay == 16) return 1;

   return lay;
}


/******************************************************************************
* get_v3padshape
*/
static int get_v3padshape(CString shapeName)
{
   if (!STRCMPI(shapeName, "RECTANGLE"))
      return 2;

// if (!STRCMPI(shapeName, "ROUND"))
//    return 1;

   return 1;
}


/******************************************************************************
* IsCompNameUsed
*/
static BOOL IsCompNameUsed(CString compName)
{
   if (get_geomindex(compName) > -1)   // here check the comp index
      return 1;

   for (int i=0; i<freepadArrayCount; i++)
   {
      if (!freepadArray[i]->compdes.CompareNoCase(compName))
         return TRUE;
   }

   return FALSE;
}


/******************************************************************************
* is_command
*/
static int is_command(CString string, List *commandList, int commandListSize)
{
   for (int i=0; i<commandListSize; i++)
   {
      if (!STRCMPI(commandList[i].token, string))
         return i;
   }

   return -1;
}

/******************************************************************************
* is_V3command
*/
static int is_V3command(CString string, List *commandList, int commandListSize)
{
   CString key, val;

   if (get_keyval(string, &key, &val))
   {
      for (int i=0; i<commandListSize; i++)
      {
         if (!STRCMPI(commandList[i].token, val))
            return i;
      }
   }

   return -1;
}


/******************************************************************************
* get_keyval
*/
static bool get_keyval(CString string, CString *key, CString *val)
{
   *key = string;  // Init key to entire string, in case there is no "=" it gets whole input field
   *val = "";

   int index = string.Find('=');

   if (index < 1) // first character returns 0, not found returns -1
      return false;

   *key = string.Left(index);
   *val = string.Mid(index+1);

   return true;
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

   double x = atof(tok);

   // units are in 1/1000 of mils
   x = x/1000 * scale_factor;

   return x;
}


/******************************************************************************
* add_netcomppin
*/
static int add_netcomppin(int compindex, int pinindex)
{
   for (int i=0; i<netCompPinArrayCount; i++)
   {
      if (netCompPinArray[i]->compnr == compindex && netCompPinArray[i]->pinnr == pinindex)
         return i;
   }

   PfwNetCompPin *pin = new PfwNetCompPin;
   netCompPinArray.SetAtGrow(netCompPinArrayCount++, pin);  
   pin->compnr = compindex;
   pin->pinnr = pinindex;
   pin->netnr = -1;

   return netCompPinArrayCount-1;
}


/******************************************************************************
* get_netname
*/
static const char *get_netname(int nr)
{
   static   char  tmp[255];
   int      i;

   for (i=0;i<netArrayCount;i++)
   {
      if (netArray[i]->netnr == nr)
         return netArray[i]->netname;
   }

   if (nr)
      sprintf(tmp,"Net_%d",nr);
   else
      return NET_UNUSED_PINS;

   return tmp;
}


/******************************************************************************
* normalize_rot
*/
static double normalize_rot(double r)
{
   while (r < 0)
      r += 360;
   while (r >= 360)
      r -= 360;
   return r;
}


/******************************************************************************
* p_error
*/
static int p_error()
{
   fprintf(logFp, "Token expected at %ld\n", getInputFileLineNumber());
   display_error++;
   return -1;
}

/******************************************************************************
* pfwv3_rule
*/
static int pfwv3_rule()
{

	//case 1015: added the rule data line read, we care about only the 3 commands:
	//rulekind, enabled, expansion
	CString rulekind = "";
	BOOL enabled = TRUE;
	double expansion;
   char *string;
   while (string = get_string(NULL, "|"))
   {
      CString key, val;
      get_keyval(string, &key, &val);

      if (!key.CompareNoCase("RULEKIND"))
         rulekind = val;
      else if (!key.CompareNoCase("ENABLED"))
			enabled = !val.CompareNoCase("TRUE")?TRUE:FALSE;
      else if (!key.CompareNoCase("EXPANSION"))
		{
         expansion = get_v3unit(val);
		}
      else
      {
#ifdef DEBUG_UNKNOWN_KEY
         fprintf(logFp,"Unknown Key [%s] in RECORD=Rule at %ld\n", key, getInputFileLineNumber());
         display_error++;
#endif
      }
	}

	if (!rulekind.CompareNoCase("SolderMaskExpansion"))
	{
		if (enabled)
			glb_CSEV_val = expansion;
	}
	else if (!rulekind.CompareNoCase("PasteMaskExpansion"))
	{
		if (enabled)
         glb_CPEV_val = expansion;
	}

	return 0;
}

//_____________________________________________________________________________
CPfwLayer::CPfwLayer(int pfwLayerIndex)
: m_pfwLayerIndex(pfwLayerIndex)
, m_usedFlag(false)
, m_mirroredPfwLayerIndex(pfwLayerIndex)
{
}

//_____________________________________________________________________________
CPfwLayer* CPfwLayerArray::getAt(int index)
{
   CPfwLayer* layer = NULL;

   if (index >= 0 && index < m_layerArray.GetSize())
   {
      layer = m_layerArray.GetAt(index);
   }

   if (layer == NULL)
   {
      layer = new CPfwLayer(index);
      m_layerArray.SetAtGrow(index,layer);
   }

   return layer;
}

void CPfwLayerArray::mirrorLayers(int pfwLayerIndex1,int pfwLayerIndex2)
{
   CPfwLayer* layer1 = getAt(pfwLayerIndex1);
   CPfwLayer* layer2 = getAt(pfwLayerIndex2);

   if (layer1->getMirroredPfwLayerIndex() != pfwLayerIndex2 ||
       layer2->getMirroredPfwLayerIndex() != pfwLayerIndex1    )
   {
      unmirrorLayer(pfwLayerIndex1);
      unmirrorLayer(pfwLayerIndex2);
   }

   layer1->setMirroredPfwLayerIndex(pfwLayerIndex2);
   layer2->setMirroredPfwLayerIndex(pfwLayerIndex1);
}

void CPfwLayerArray::unmirrorLayer(int pfwLayerIndex)
{
   CPfwLayer* layer = getAt(pfwLayerIndex);

   if (layer->getMirroredPfwLayerIndex() != pfwLayerIndex)
   {
      CPfwLayer* mirroredLayer = getAt(layer->getMirroredPfwLayerIndex());

      if (mirroredLayer->getMirroredPfwLayerIndex() == pfwLayerIndex)
      {
         mirroredLayer->setMirroredPfwLayerIndex(mirroredLayer->getPfwLayerIndex());
      }

      layer->setMirroredPfwLayerIndex(layer->getPfwLayerIndex());
   }
}
