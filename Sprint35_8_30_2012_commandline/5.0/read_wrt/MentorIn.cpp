// $Header: /CAMCAD/5.0/read_wrt/MentorIn.cpp 159   6/17/07 8:58p Kurt Van Ness $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.
          
   The theory is             
   1. All blocks are global
   2. A geometry file must be loaded first - create board is the file 
   3. All file headers are check in the first line for file type and must follow
      Mentor spec
   4. All create views are also files
   5. Create Panels are also files
                                                          
                                                     
   All blocks have -1 filenumber, because files, blocks and references can come in any order 
   and different files.

   PAD create_pin rules

   PAD is only the outer layer.
   PAD in SMD is PAD_TOP or PAD_BOT
   PAD in thru is PAD_1 and PAD_2


From the mentor usegroup email:
-----------------------------------------------------------------
Naming conventions:
-------------------

Must be unique within a design, and should be unique throughout your
design environment.

Should not be any of the predefined Mentor Graphics property names.

Must begin with a letter (a-z), followed by any combination of letters
(a-z),
digits (0-9), underbars (_) or dollar signs ($). Underbar and dollar sign
must
be preceded and followed by alpahnumeric characters or digits.

Are not case sensitive in most applications.

Cannot use special characters used by Mentor Graphics tools to indicate
reserved property names. The specific Mentor Graphics application manual
provide more details on these excepitions. For example in ICgraph names
that begin with "#$_".

Cannot contain forward or backward slashes (/ or \).

Cannot contain embebbed spaces.

Cannot contain periods in an identifier.


Note: The only restriction on design management property names is they
cannot contain embebbed spaces.

*/           

// MENTORIN.CPP



/*************** RECENT UPDATES ****************
*
*  05-30-02: Revamping - Sadek Noureddine
*
*  06-04-02: TSR 3644 -- Sadek Noureddine
*
*  06-14-02: TSR 3657 -- Sadek Noureddine
*
*
*/


#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"                                           
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "pcbutil2.h"
#include "format_s.h"
#include "format.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "filetype.h"
#include "mentor.h"
#include "logwrite.h"
#include "menlib.h"
#include "Polylib.h"
#include "mentorin.h"
#include "centroid.h"
#include "lyrmanip.h"
#include "Attribute.h"
#include "RwLib.h"
#include "FileList.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "DFT.h"
#include "SchematicNetlistIn.h"  // for CMentorBoardstationNetlistReader

/******************************************************************************
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Suffix used to distinguish special bottom side entities that are generated
// while processing asymmetric pads.
#define bottomSuffix "_$BOT"

extern CProgressDlg			*progress;
extern FileStruct				*currentPCBFile; // from PORT_LIB.CPP
//extern LayerTypeInfoStruct	layertypes[];

/* Static Variable Section *********************************************************/

static MENAdef             layer_attr[MAX_LAYERS];    /* Array of layers from mentor.in */
static int                 layer_attr_cnt;

static MENPhyslayers       layer_phys[MAX_LAYERS];    /* Array of layers from techfile, from 1 to layer_phys_cnt */
static int                 layer_phys_cnt;

static CCompPinArray       comppin;
static int                 comppincnt;

static CPadlistArray       padlistarray;
static int                 padlistcnt;

static CPadRuleArray			padrulearray;
static int						padrulecnt = 0;

static CPartArray          partarray;
static int                 partcnt;

static CPowernetArray      powernetarray;
static int                 powernetcnt;

static CAttrmapArray       attrmaparray;
static int                 attrmapcnt;

static CRenamelayerArray   renamelayerarray;
static int                 renamelayercnt;

static CBuriedViaArray     buriedviaarray;
static int                 buriedviacnt;

static CPolyArray          polyarray;
static int                 polycnt;

static CDrillArray         drillarray;
static int                 drillcnt;

static CPadformArray       padFormArray;
//static int                 padformcnt;

static CUIntArray          tttArray;
static int                 tttCnt;

static CDWordArray         mentorLayerColors;
static CMENLayersArray     mentorLayers;
static int                 mentorLayersCnt;

static Attr                *attr;
static int                 attrcnt;

static CArray<int, int>		grClassFilterArray;
static int						grClassFilterCnt;

static CArray<int, int>		insertTypeFilterArray;
static int						insertTypeFilterCnt;

static CMapStringToPtr     layerToDataMap;
static CMapStringToString  componentSpecificLayerOnMap;
static BOOL                isThruholeBlindBuried = FALSE;

static CCEtoODBDoc          *doc;
static BlockStruct         *cur_b;
static double              scale_factor;
static int                 page_unit;
static FILE                *ifp;
static char                *ifp_line = NULL;
static long                ifp_linecnt;
static CString             lastline;   // last line from fgets
static int                 Push_NextLine = FALSE;
static FILE                *ferr;
static int                 display_error = 0;

static CString             cur_filename;
static int                 cur_filecount;
static int                 cur_padlistptr;
static int                 cur_layer;
static int                 cur_entity;
static int                 cur_create = CREATE_UNKNOWN;
static char                cur_name[80];
static char                cur_boardname[80];

static int						pcbFileNum;			// this can be the num of the Mentor Boardstation or Neutral PCB file 
static bool						useNeutralFile;	// this indicates that a Mentor Neutral file is used
static CMapStringToString	replacedOriginalNeutralGeom;
static CBoardLevelGeometryMap boardLevelGeometryMap;

static char                component_default_padstack[80];
static char                board_default_padstack[80];
static Centroid            centroid;
static int                 number_of_routing_layers;
static int                 pad2_stackup_layer;
static double              smallestdrill = 0.0;
static CMapStringToString  compToBeRemove;


// These are variable for load_mentorsettings function
static CString             prim_boardoutline;
static CString             prim_compoutline;
static CString             prim_paneloutline;
static int                 component_layer;
static int                 solder_layer;
static double              template_path_width = 0;
static BOOL                MAKE_LOCATION;
static CString             DEFAULT_REFLOC_UNITS;
static int                 IGNORE_MISSING_GEOM_ENTRIES;
static CString             REFNAME_LAYER;
static int                 ask_file_type;
static int                 convert_complex_power;
static int                 partnumberextension = TRUE;
static int                 hierachical_netnames = TRUE;
static int                 FIL_only = FALSE;
static int                 ignore_Islands = FALSE;
static int                 ignore_Thermal_Tie = FALSE;
static char                copper_cutout = 'N';
static BOOL                explodePowerSignalLayer = FALSE;
static BOOL                hasLayerFile = FALSE;
static BOOL                hasNetlistFile = FALSE;
static BOOL						hasCompFile = FALSE;
static bool						curDrawingInsertBoard = false;
static bool						hasTechFile = false;
static BOOL                hasTestPointFile = FALSE;
static bool						missingTechReported = false;
static bool                setPartnumberAttribFromDevice = true;
static CString             mentor_defaultunits;  // set boardstation unit by default TN
static bool                techFileProcessed = false; // set true if/when a technology file is processed

static CFileReadProgress*	fileReadProgress = NULL; 

static CTypeChanger MakeInto_CommandProcessor;

/* Function Prototypes *********************************************************/

static void InitializeMembers();
static void FreeMembers();
static int tst_filetype(FILE *file, const char *filename);
static int RemoveGenericLayers();
static int DeleteUnusedPowerSignalLayers();
static BOOL IsNumber(CString value);
static void getLayerColors(CString filename);
static int do_layers();
static int do_default_layers();
static void GetLayersFromFile();
static MENLayer *SetMentorLayer(CString layerName, CString topUserDefLayer, CString bottomUserDefLayer);
static void GetDefaultLayers();
static void CreateLayer();
static void SortLayerName();
static BOOL IsGenericTopBottomMappingLayer(CString layerName, CString &layerName_1, CString &layerName_2);
static BOOL IsSpecificTopBottomMappingLayer(CString layerName, CString &genericName, BOOL &isTop);
static int GetNumberOfSignalSpecificLayers();
static int GetNumberOfPowerSpecificLayers();
static void CheckAttribOnCompSpecificLayerForGeom();
static void CheckAttributeOnComponentSpecificLayer(CAttributes *attributesRef);
static int CheckTopBottomLayerMappingInserts();
static int AddDataToLayer(CString layerName, CDataList *dataList, DataStruct *data, int dataType);
static int do_geom_ascii();
static int do_technology();
static int do_testpoints();
static int do_pins();
static int process_asymmetric_padstacks();
static int loop_geom(List *l, int s);
static int do_component();
static int do_netlist(FileStruct *fl);
static int do_netlist(CNetList &netlistRef);
static int do_package(FileStruct *fl);
static int update_layer(CString &layerName);
static int write_polypin(const char *l, int dataType, BOOL clearPolyArray);
static int write_poly(int layer, double width, int fill, int close, int graphic_class, int dataType, BOOL clearPolyArray, BOOL addto_layerToDataMap = TRUE);
static int do_arc(const char *lay, BOOL addto_layerToDataMap = TRUE);
static int update_part(const char *p, int typ);
static void make_otherlayer(char *lay);
static int do_units(const char *u);
static void make_padlayer(char *lay, int smd);
static double get_smallestdrill(int unit);
static int load_mentorsettings(const CString fname);
static int change_vias_2_testpads();
static int get_buriedvianames();
static CString get_blindvianamelist();
static void setFileReadProgress(const CString& status);
static BlockStruct *Mentor_Graph_Block_On(int method, CString geomName, int fileNum, DbFlag flag, BlockTypeTag blockType = blockTypeUndefined);
static void updateMirroredInserts();
static int get_part(const char *p);
static int get_testsets(char *neutralfmt, CString &refname, CString &Fixture);
static void InitPropertyType(CMapStringToInt &propertyTypeMap);

void FilterGraphicClass(int graphicClass);
void FilterInsertType(int insertType);


/******************************************************************************
* ReadMentorBoardStation    Mentor Board Station
*/
int ReadMentorBoardStation(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits, int cur_filecnt, int tot_filecnt)
{   
   // all units are normalized to INCHES
   scale_factor = Units_Factor(UNIT_INCHES, pageunits); // just to initialize
   page_unit = pageunits;
   strcpy(component_default_padstack, _strupr(""));
   strcpy(board_default_padstack, _strupr(""));
   cur_filename = path_buffer;
   cur_filecount = cur_filecnt;
   doc = Doc;
   cur_b = NULL;
   smallestdrill = get_smallestdrill(pageunits);
   mentor_defaultunits = "TN";

   CString mentorLogFile = GetLogfilePath("mentor.log");
   if (cur_filecount > 0)
   {
      if ((ferr = fopen(mentorLogFile, "a+t")) == NULL)
      {
         CString message;
         message.Format("Error open [%s] file", mentorLogFile);
         ErrorMessage(message, "Error");
         return 1;
      }
   }
   else
   {
      if ((ferr = fopen(mentorLogFile, "wt")) == NULL)
      {
         CString message;
         message.Format("Error open [%s] file", mentorLogFile);
         ErrorMessage(message, "Error");
         return 1;
      }
      display_error = 0;
   }

   InitializeMembers();
   if (cur_filecount == 0)
   {
      // only initialize this member when the first file is load
      mentorLayers.SetSize(100, 100);
      mentorLayersCnt = 0;
      hasLayerFile = FALSE;
      hasNetlistFile = FALSE;
		hasCompFile = FALSE;
		hasTechFile = false;
		missingTechReported = false;
      techFileProcessed = false;
      hasTestPointFile = FALSE;

		pcbFileNum = -1;
		useNeutralFile = false;
		replacedOriginalNeutralGeom.RemoveAll();
      boardLevelGeometryMap.empty();

		// Check for Mentor Neutral file
		int count = 0;
		POSITION pos = doc->getFileList().GetHeadPosition();
		FileStruct *boardstationFile = NULL;
		while (pos)
		{
			FileStruct *file = doc->getFileList().GetNext(pos);
			if (file->getBlockType() != blockTypePcb)
				continue;
			if (file->getCadSourceFormat() != Type_Mentor_Neutral_Layout)
				continue;

			boardstationFile = file;
			count++;
		}

		int answer = IDNO;
		CString msg;
		if (count == 1 && boardstationFile != NULL)
		{
			msg =  "There is a Mentor Neutral file found.  Would you like to import into it?\n\n";
			msg += "   Yes to import into existing Mentor Neutral file.\n";
			msg += "   No to import into a new PCB file.\n";
			answer = ErrorMessage(msg, "Mentor Neutral file found", MB_YESNO);

			if (answer == IDYES)
			{
				pcbFileNum = boardstationFile->getFileNumber();
				useNeutralFile = true;
			}
		}
		else if (count > 1)
		{
			msg =  "There are multiple Mentor Neutral files found.  Would you like to select a file to import into it?\n\n";
			msg += "   Yes to import into existing Mentor Neutral file.\n";
			msg += "   No to import into a new PCB file.\n";
			answer = ErrorMessage(msg, "Mentor Neutral file found", MB_YESNO);

			if (answer == IDYES)
			{
				SelectFileDlg dlg;
				dlg.doc = doc;
				dlg.SetFileType(blockTypePcb);
				dlg.SetSourceCad(Type_Mentor_Neutral_Layout);
				if (dlg.DoModal() != IDOK)
					return 1;

				pcbFileNum = dlg.file->getFileNumber();
				useNeutralFile = true;
			}
		}
	}

   log_set_filename(path_buffer);
   
   // need to do this, because panel only knows the path.ext
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fnameAndExt[_MAX_FNAME+4];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(fnameAndExt, fname);
   strcat(fnameAndExt, ext);
   
   int res = 1;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(path_buffer, "rb")) == NULL)    // not rt because the is a Ctr Z allowed in text
   {
      ErrorMessage("Error open file", path_buffer);
      return 1;
   }

   cur_create = CREATE_UNKNOWN;
   ifp_linecnt = 0;
   Push_NextLine = FALSE;

   number_of_routing_layers = 0;
   pad2_stackup_layer = 0;

   // - Load settings need to be call each time a file is read because everything 
   // will be reset each time this function is called
   // - layers in settings file can not be loaded until the first file is open.
   CString settingsFile( getApp().getImportSettingsFilePath("mentor.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nMentor BoardStation Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_mentorsettings(settingsFile);

	int fileType = tst_filetype(ifp, path_buffer);

	if (cur_filecount == 0 && !hasLayerFile && fileType != MEN_LAYER)
		do_default_layers();

   bool hasPCBFile = true;
	// Case 1685
	// Files are processed in a particular order. See SortImportFiles() in Port_Lib.cpp.
	// If current file type is beyond "tech" file, check on status of buried/blind vias
	// and tech file.
	if (fileType == MEN_TECHNOLOGY ||
      fileType == MEN_COMP ||
		fileType == MEN_WIRE  ||
		fileType == MEN_NETLIST ||
		fileType == MEN_PACKAGE ||
      fileType == MEN_PINS ||
		fileType == MEN_TESTPOINT)
	{
      // verify the file sequence, technology need to be handled first
      if(fileType != MEN_TECHNOLOGY )
      {
		   CString blindvialist = get_blindvianamelist();

		   if (!blindvialist.IsEmpty() && !hasTechFile && !missingTechReported)
		   {
			   CString msg1 = "The design loaded contains Blind and/or Buried Vias that may not be defined correctly\nbecause of the absence of the TECH.TECH file from the Mentor System.";
			   CString msg2 = msg1 + "\nSee the log file for a list of geometry names.";
			   // Tell the user now via pop-up
			   ErrorMessage(msg2, "Missing Input Data");

			   // And tell 'em again later via log file
			   fprintf(ferr, "\n%s\n", msg1);
			   fprintf(ferr, "These include: %s\n\n", blindvialist);
			   missingTechReported = true;
		   }
      }

      //detect PCB File
      if (currentPCBFile == NULL || currentPCBFile->getBlockType() != BLOCKTYPE_PCB)
      {
         FormatPCBFileDlg dlg;
         dlg.doc = doc;
         int fileCount = dlg.LegalFileCount();

         if (fileCount == 0)
         {
            CString message;
            message.Format("Loading [%s]", path_buffer); 
            ErrorMessage("No PCB file type found!\nYou must first load the geometry_ascii file (part file)!", message);
            hasPCBFile = false;
         }
         else if (fileCount > 1) // user picks file
         {
            if (dlg.DoModal() != IDOK)
               hasPCBFile = false;
         }
         if(hasPCBFile)
         {
            currentPCBFile = dlg.pcbFile;

				if (!useNeutralFile)
					pcbFileNum = currentPCBFile->getFileNumber();

         }
      }
	}
      
   switch (fileType)
   {
   case MEN_LAYER:
      {
         // get layer colors from Mentor.CLR file (if available)
         getLayerColors(getApp().getSystemSettingsFilePath("Mentor.CLR"));
			do_layers();			
      }
      break;

   case MEN_GEOM:
      {
         strcpy(cur_boardname, fnameAndExt);

         loop_geom(find_lst, SIZ_OF_FIND);
         ifp_linecnt = 0;
         Push_NextLine = FALSE;

         rewind(ifp);

         currentPCBFile = Graph_File_Start(cur_boardname, Type_Mentor_Layout);
         currentPCBFile->setBlockType(blockTypePcb);
         currentPCBFile->getBlock()->setBlockType(currentPCBFile->getBlockType());

			if (!useNeutralFile)
				pcbFileNum = currentPCBFile->getFileNumber();

         Graph_Level("0", "", 1);   // this should always be the first layer

         loop_geom(brd_lst, SIZ_OF_BRD);
         ifp_linecnt = 0;
         Push_NextLine = FALSE;

         rewind(ifp);
         do_geom_ascii();
         boardLevelGeometryMap.derivedSpecialVersionOfBoardLevelGeometries();
         boardLevelGeometryMap.empty();

         OptimizePadstacks(doc, pageunits, convert_complex_power);
         generate_PADSTACKACCESSFLAG(doc, 1);

         // change vias to testpads if they have no drill and only exist on 1 layer
         // change_vias_2_testpads();  // This is removed to fix TSR #3644

         int err = check_primary_boardoutline(currentPCBFile);

         switch (err)
         {        
         case 1:
            fprintf(ferr, "Primary Boardoutline found, but not closed!\n");
            display_error++;
            break;
         case 2:
            fprintf(ferr, "No Primary Boardoutline found!\n");
            display_error++;
            break;
         case 3:
            fprintf(ferr, "One Primary closed Boardoutline and multiple open elements found!\n");
            fprintf(ferr, "The open elements will be set to BOARD GEOMETRIES!\n");
            display_error++;
            break;
         case 4:
            fprintf(ferr, "Multiple Primary closed Boardoutline and multiple open elements found!\n");
            fprintf(ferr, "All elements will be set to BOARD GEOMETRIES!\n");
            display_error++;
            break;
         }
      }
      break;

   case MEN_COMP:
      {
         if(hasPCBFile)
         {
            doc->PrepareAddEntity(currentPCBFile);

            compToBeRemove.RemoveAll();

            res = do_component();
			   hasCompFile = TRUE;
         }
      }
      break;

   case MEN_NETLIST:
      {
         if(hasPCBFile)
         {
            doc->PrepareAddEntity(currentPCBFile);

            do_netlist(currentPCBFile);
            compToBeRemove.RemoveAll();

            generate_PINLOC(doc, currentPCBFile, 1);  // this function generates the PINLOC argument for all pins.
            hasNetlistFile = TRUE;
         }
      }
      break;

   case MEN_PACKAGE:
      {
         if(hasPCBFile)
         {
            doc->PrepareAddEntity(currentPCBFile);

            do_package(currentPCBFile);
         }
      }
      break;

   case MEN_WIRE:
      {
         if(hasPCBFile)
         {
			   if (hasCompFile == FALSE)
			   {
				   fprintf(ferr, "WARNING!  Skip TRACE file because no COMP file is loaded.");
				   break;
			   }

            doc->PrepareAddEntity(currentPCBFile);

            get_buriedvianames();
            do_wires(doc, currentPCBFile, ferr, ifp, TRUE, page_unit);
      
            EliminateSinglePointPolys(doc);                
            progress->SetStatus("Optimizing Database");

            double accuracy = get_accuracy(doc);
            EliminateDuplicateVias(doc, currentPCBFile->getBlock());
            Crack(doc, currentPCBFile->getBlock(), TRUE);        
            EliminateOverlappingTraces(doc, currentPCBFile->getBlock(), TRUE, accuracy);     
         }
      }
      break;
   
   case MEN_TECHNOLOGY:
      {
         if(hasPCBFile)
         {
			   hasTechFile = true;         
            doc->PrepareAddEntity(currentPCBFile);

            do_technology();
			   process_asymmetric_padstacks();
            generate_PADSTACKACCESSFLAG(doc, 1);
         }
      }      
		break;
   case MEN_TESTPOINT:
      {
         if(hasPCBFile)
         {
            do_testpoints();
            hasTestPointFile = TRUE;
         }
      }
      break;
   case MEN_PINS:
      {
         if(hasPCBFile)
         {
            do_pins();
         }
      }
      break;
   default:
      {
         // unknown file
         CString message;
         message.Format(" File [%s] is not a recognized.", path_buffer);
         ErrorMessage(message, "Unknown FileType");
      }
      break;
   }

   fclose(ifp);
   fclose(ferr); 

   FreeMembers();

   if (cur_filecount == (tot_filecnt-1))
   {
		int i=0;
		for (i=0; i<grClassFilterCnt; i++)
			FilterGraphicClass((int)grClassFilterArray.GetAt(i));
		grClassFilterArray.RemoveAll();
		grClassFilterCnt = 0;

		for (i=0; i<insertTypeFilterCnt; i++)
			FilterInsertType((int)insertTypeFilterArray.GetAt(i));
		insertTypeFilterArray.RemoveAll();
		insertTypeFilterCnt = 0;

		RemoveGenericLayers();
      DeleteUnusedPowerSignalLayers();

      // only free this member after the last file is read
      for (i=0; i<mentorLayersCnt ;i++)
         delete mentorLayers[i];
      mentorLayers.RemoveAll();
      mentorLayersCnt = 0;

		mentorLayerColors.RemoveAll();

      if (display_error)
         Logreader(mentorLogFile);

      hasLayerFile = FALSE;
		hasCompFile = FALSE;
		pcbFileNum = -1;
		useNeutralFile = false;
		replacedOriginalNeutralGeom.RemoveAll();

      boardLevelGeometryMap.empty();

      MakeInto_CommandProcessor.Apply(doc, currentPCBFile);
      generate_PINLOC(doc, currentPCBFile, 1);  // in case MakeInto added pins to items converted to testpoints

		updateMirroredInserts();
   }

   MakeInto_CommandProcessor.Reset();
   setFileReadProgress("");

   return res;
}

/******************************************************************************
* InitializeMembers
*/
static void InitializeMembers()
{
   if ((attr = (Attr *)calloc(MAX_ATTR, sizeof(Attr))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   attrcnt = 0;
   
   comppin.SetSize(1000, 1000);
   comppincnt = 0;

   polyarray.SetSize(1000, 1000);
   polycnt = 0;

   padlistarray.SetSize(100, 100);
   padlistcnt = 0;

   padrulearray.SetSize(100, 100);
	padrulecnt = 0;

   partarray.SetSize(100, 100);
   partcnt = 0;

   powernetarray.SetSize(100, 100);
   powernetcnt = 0;

   attrmaparray.SetSize(100, 100);
   attrmapcnt = 0;

   renamelayerarray.SetSize(100, 100);
   renamelayercnt = 0;

   buriedviaarray.SetSize(100, 100);
   buriedviacnt = 0;

   tttArray.SetSize(100, 100);
   tttCnt = 0;

   drillarray.SetSize(100, 100);
   drillcnt = 0;

   padFormArray.SetSize(100, 100);
   //padformcnt = 0;

	grClassFilterArray.SetSize(50, 50);
	grClassFilterCnt = 0;

	insertTypeFilterArray.SetSize(50, 50);
	insertTypeFilterCnt = 0;

   layerToDataMap.RemoveAll();
   componentSpecificLayerOnMap.RemoveAll();
   isThruholeBlindBuried = FALSE;
	curDrawingInsertBoard = false;
}

/******************************************************************************
* FreeMembers
*/
static void FreeMembers()
{
	int i=0;
   for (i=0; i<attrcnt; i++)
      free(attr[i].name);
   free(attr);
   attrcnt = 0;

   for (i=0; i<comppincnt; i++)
   {
      delete comppin[i];
   }
   comppin.RemoveAll();
   comppincnt = 0;

   for (i=0; i<padlistcnt; i++)
      delete padlistarray[i];  
   padlistarray.RemoveAll();
   padlistcnt = 0;

	for (i=0; i<padrulecnt; i++)
		delete padrulearray[i];  
	padrulearray.RemoveAll();
	padrulecnt = 0;

   for (i=0; i<partcnt; i++)
      delete partarray[i];  
   partarray.RemoveAll();
   partcnt = 0;

   for (i=0; i<powernetcnt; i++)
      delete powernetarray[i];  
   powernetarray.RemoveAll();
   powernetcnt = 0;

   for (i=0; i<attrmapcnt; i++)
      delete attrmaparray[i];  
   attrmaparray.RemoveAll();
   attrmapcnt = 0;

   for (i=0; i<renamelayercnt; i++)
      delete renamelayerarray[i];  
   renamelayerarray.RemoveAll();
   renamelayercnt = 0;

   for (i=0; i<buriedviacnt; i++)
      delete buriedviaarray[i];  
   buriedviaarray.RemoveAll();
   buriedviacnt = 0;

   tttArray.RemoveAll();
   tttCnt = 0;

   drillarray.RemoveAll();
   drillcnt = 0;

   padFormArray.RemoveAll();  // 60 mil round
   //padformcnt = 0;

   layerToDataMap.RemoveAll();
   componentSpecificLayerOnMap.RemoveAll();
   isThruholeBlindBuried = FALSE;
}

//_____________________________________________________________________________
static void setFileReadProgress(const CString& status)
{
   delete fileReadProgress;

   if (status.IsEmpty())
   {
      fileReadProgress = NULL;
   }
   else
   {
      fileReadProgress = new CFileReadProgress(ifp);
      fileReadProgress->updateStatus(status);
   }
}

/******************************************************************************
* get_layerrename
*/
static const char *get_layerrename(const char *l)
{
   for(int i=0; i<renamelayercnt; i++)
   {
      if (renamelayerarray[i]->men_name.CompareNoCase(l) == 0)
         return renamelayerarray[i]->cc_name.GetBuffer(0);
   }

   return l;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0; i<MAX_LAYTYPE; i++)
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
      if (!STRCMPI(l, layer_attr[i].name))
         return i;
   }

   if (layer_attr_cnt < MAX_LAYERS)
   {
      layer_attr[layer_attr_cnt].name = l;
      layer_attr[layer_attr_cnt].attr = 0;
      layer_attr[layer_attr_cnt].used = 0;
      layer_attr[layer_attr_cnt].mirror = -1;
      layer_attr_cnt++;
   }
   else
   {
      fprintf(ferr, "Too many layers\n");
      return -1;
   }

   return layer_attr_cnt-1;
}

/******************************************************************************
* int get_padlistptr
*/
static int get_padlistptr(const char *l)
{
   for (int i=0; i<padlistcnt; i++)
   {
      MEN4Padlist *pad = padlistarray[i];
      if (pad->name.CompareNoCase(l) == 0)
         return i;
   }

   return -1;
}


/******************************************************************************
* int get_padrule
*/
static CMentorPadRule *get_padrule(CString padname, int top)
{
	for (int i=0; i<padrulecnt; i++)
   {
      CMentorPadRule *padrule = padrulearray[i];
		if (padrule->getPadname().CompareNoCase(padname) == 0 &&
			padrule->getTopSide() == top)
		{
         return padrule;
		}
   }

   return NULL;
}

/******************************************************************************
* is_pad_complex
*/
static int is_pad_complex(int index)
{
   // this can be 0 because for SMD pins, the layer PAD may change to PAD_1
   // but I do not know if this is an SMD yet.
   // But I know if it has more than 1 element, it is not a simple pad.

   MEN4Padlist *padlist = padlistarray[cur_padlistptr];
   CString countStr;
   CString layer;
   layer.Format("%d", index);

   int count = 0;
   if (padlist->layer_elem_cnt.Lookup(layer, countStr))
      return atoi(countStr);
   else
      return 0;
}

/******************************************************************************
* is_complex_attached
*/
static int is_complex_attached(int index)
{
   MEN4Padlist *padlist = padlistarray[cur_padlistptr];
   CString countStr;
   CString layer;
   layer.Format("%d", index);

   int count = 0;
   if (padlist->complex_cnt.Lookup(layer, countStr))
      return atoi(countStr);
   else
      return 0;
}

/******************************************************************************
* get_graphic_class
*/
static int get_graphic_class(const char *l)
{
   int graphic_class = 0;

   if (!strcmp(prim_boardoutline, l))
      graphic_class = GR_CLASS_BOARDOUTLINE;
   else if (!strcmp(prim_compoutline, l))
      graphic_class = GR_CLASS_COMPOUTLINE;
   else if (!strcmp(prim_paneloutline, l))
      graphic_class = GR_CLASS_PANELOUTLINE;
   else if (!strcmp("ROUTING_KEEPOUT", l))
      graphic_class = GR_CLASS_ROUTKEEPOUT;
   else if (!strcmp("BOARD_PLACEMENT_KEEPOUT", l))
      graphic_class = GR_CLASS_PLACEKEEPOUT;
   else if (!strcmp("BOARD_ROUTING_OUTLINE", l))
      graphic_class = GR_CLASS_ROUTKEEPIN;
   else if (!strcmp("BOARD_PLACEMENT_OUTLINE", l))
      graphic_class = GR_CLASS_PLACEKEEPIN;
   else if (!strcmp("VIA_KEEPOUT", l))
      graphic_class = GR_CLASS_VIAKEEPOUT;
   else if (!strcmp("TESTPOINT_KEEPOUT", l))
      graphic_class = graphicClassTestPointKeepOut;
   else if (!strcmp("TRACE_KEEPOUT", l))
      graphic_class = GR_CLASS_ROUTKEEPOUT;
   else if (!strcmp("MILLING_PATH", l))
      graphic_class = GR_CLASS_MILLINGPATH;

   return graphic_class;
}

/******************************************************************************
* update_attr
*/
static int update_attr(const char *lp,int typ, int unit)
{
   if (attrcnt < MAX_ATTR)
   {
      if ((attr[attrcnt].name = STRDUP(lp)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      attr[attrcnt].atttype = typ;
      attr[attrcnt].unit = unit;
      attrcnt++;
   }
   else
   {
      ErrorMessage("Too many attributes");
   }
   return attrcnt-1;
}

/******************************************************************************
* char* get_attrmap
*/
static const char* get_attrmap(const char *a)
{
   for (int i=0; i<attrmapcnt; i++)
   {
      if (attrmaparray[i]->men_name.CompareNoCase(a) == 0)
         return attrmaparray[i]->cc_name.GetBuffer(0);
   }

   return a;
}

/******************************************************************************
* load_mentorsettings
*/
static int load_mentorsettings(const CString fname)
{
   FILE *fp;
   char line[255];
   char *lp;
   long linecnt = 0;

   Graph_Level("0","",1);  // this should always be the first layer
   prim_boardoutline = "";
   prim_compoutline = "";
   prim_paneloutline = "";
   template_path_width = 0;
   component_layer = solder_layer = 0;
   ask_file_type = FALSE;
   MAKE_LOCATION = FALSE;
   layer_attr_cnt = 0;
   REFNAME_LAYER = "SILKSCREEN";
   convert_complex_power = TRUE;
   partnumberextension = TRUE;
   hierachical_netnames = TRUE;
   FIL_only = FALSE;
   ignore_Islands = FALSE;
   ignore_Thermal_Tie = FALSE;
   copper_cutout = 'N';
   explodePowerSignalLayer = FALSE;
   setPartnumberAttribFromDevice = true;
   MakeInto_CommandProcessor.Reset();

   // init phys array
   for (int i =0; i<MAX_LAYERS; i++)
   {
      layer_phys[i].stacknum = -1;
      layer_phys[i].physname = "";
      layer_phys[i].logicalname = "";
   }
   layer_phys_cnt = 0;

   DEFAULT_REFLOC_UNITS = "IN";
   IGNORE_MISSING_GEOM_ENTRIES = FALSE;

   if ((fp = fopen(fname, "rt")) == NULL)  
   {
      // no settings file found
      CString message;
      message.Format("File [%s] not found", fname);
      ErrorMessage(message, "Mentor Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      linecnt++;
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".ASKFILETYPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               ask_file_type = TRUE;
         }
         else if (MakeInto_CommandProcessor.ParseCommand(line)) // send whole text line, not just command
         {
            // Already did what it needed to do
         }
         else if (!STRICMP(lp, ".MAKE_LOCATION"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               MAKE_LOCATION = TRUE;
         }
         else if (!STRICMP(lp, ".CONVERTCOMPLEXPOWER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               convert_complex_power = FALSE;
         }
         else if (!STRICMP(lp, ".HIERACHICAL_NETNAMES"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               hierachical_netnames = FALSE;
         }
         else if (!STRICMP(lp, ".PARTNUMBEREXTENSION"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               partnumberextension = FALSE;
         }
         else if (!STRICMP(lp, ".IGNORE_MISSING_GEOM_ENTRIES"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               IGNORE_MISSING_GEOM_ENTRIES = TRUE;
         }
         else if (!STRICMP(lp, ".REFNAMELAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            REFNAME_LAYER = lp;
         }
         else if (!STRICMP(lp, ".BOARDOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            prim_boardoutline = lp;
         }
         else if (!STRICMP(lp, ".COMPOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            prim_compoutline = lp;
         }
         else if (!STRICMP(lp, ".PANELOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            prim_paneloutline = lp;
         }
         else if (!STRICMP(lp, ".TYPEATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp,TYPE_ATT,0);
         }
         else if (!STRICMP(lp, ".HEIGHTATTR"))
         {
            int unit  = page_unit;
            char *lp1;
            
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString lp2 = (CString)lp;
            if ((lp1 = get_string(NULL, " \t\n")) != NULL)
            {
               if (!STRNICMP(lp1, "IN", 2))
                  unit = UNIT_INCHES;
               else if (!STRNICMP(lp1, "MM", 2))
                  unit = UNIT_MM;
               else if (!STRNICMP(lp1, "MI", 2))
                  unit = UNIT_MILS;
               else if (!STRNICMP(lp1, "TN", 2))
                  unit = UNIT_TNMETER;
            }
            update_attr(lp2, HEIGHT_ATT, unit);
         }
         else if (!STRICMP(lp, ".FIDUCIALPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_FIDUCIAL);
         }
         else if (!STRICMP(lp, ".TOOLPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_TOOL);
         }
         else if (!STRICMP(lp, ".FIDUCIALTOPPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_FIDUCIALTOP);
         }
         else if (!STRICMP(lp, ".FIDUCIALBOTPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_FIDUCIALBOT);
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString menattr = _strupr(lp);
            menattr.TrimLeft();
            menattr.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString ccattr = lp;
            ccattr.TrimLeft();
            ccattr.TrimRight();

            MEN4Attrmap *attrMap = new MEN4Attrmap;
            attrmaparray.SetAtGrow(attrmapcnt++, attrMap);  
            attrMap->men_name = menattr;
            attrMap->cc_name = ccattr;
         }
         else if (!STRICMP(lp, ".LAYERRENAME"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString olayer = _strupr(lp);
            olayer.TrimLeft();
            olayer.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString rlayer = lp;
            rlayer.TrimLeft();
            rlayer.TrimRight();

            if (!hasLayerFile)   // mentorLayersCnt <= 0 means that there is not layer file read
            {
					CString tmp1;
					CString tmp2;
					BOOL isTop = FALSE;
					if (!IsGenericTopBottomMappingLayer(olayer, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(olayer, tmp1, isTop) &&
						 !IsGenericTopBottomMappingLayer(rlayer, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(rlayer, tmp1, isTop))
					{
						MEN4Renamelayer *renameLayer = new MEN4Renamelayer;
						renamelayerarray.SetAtGrow(renamelayercnt++, renameLayer);  
						renameLayer->men_name = olayer;
						renameLayer->cc_name = rlayer;
					}
            }
            else if (hasLayerFile && cur_filecount == 1)
            {
               // only write out this error message once after the layer file is read
               fprintf(ferr, "Layer is read so \".LAYERRENAME\" command in mentor.in file is ignored.\n");
               display_error++;
            }

         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString tllayer = _strupr(lp);
            tllayer.TrimLeft();
            tllayer.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);
            int layindex = get_lay_index(tllayer);
            layer_attr[layindex].attr = laytype;
         }
         else if (!STRICMP(lp, ".MIRRORLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString layer1 = _strupr(lp);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString layer2 = _strupr(lp);

            if (!hasLayerFile)   // mentorLayersCnt <= 0 means that there is not layer file read
            {
					CString tmp1;
					CString tmp2;
					BOOL isTop = FALSE;
					if (!IsGenericTopBottomMappingLayer(layer1, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(layer1, tmp1, isTop) &&
						 !IsGenericTopBottomMappingLayer(layer2, tmp1, tmp2) && !IsSpecificTopBottomMappingLayer(layer2, tmp1, isTop))
					{
						int layindex1 = get_lay_index(layer1);          
						int layindex2 = get_lay_index(layer2);
						layer_attr[layindex1].mirror = layindex2;          
						layer_attr[layindex2].mirror = layindex1;
					}
            }
            else if (hasLayerFile && cur_filecount == 1)
            {
               // only write out this error message once after the layer file is read
               fprintf(ferr, "Layer is read so \".MIRRORLAYER\" command in mentor.in file is ignored.\n");
               display_error++;
            }

         }
         else if (!STRICMP(lp, ".DEFAULT_REFLOC_UNITS"))
         {
            double scale = scale_factor;
            CString fileName = cur_filename;
            long lineCount = ifp_linecnt;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            cur_filename = fname;
            ifp_linecnt = linecnt;
            if (do_units(lp)) // just for checking
               DEFAULT_REFLOC_UNITS = lp;       

            scale_factor = scale;            
            cur_filename = fileName;
            ifp_linecnt = lineCount;
         }
         else if (!STRICMP(lp, ".FIL_ONLY"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               FIL_only = TRUE;
         }
         else if (!STRICMP(lp, ".IGNORE_ISLANDS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               ignore_Islands = TRUE;
         }
         else if (!STRICMP(lp, ".IGNORE_THERMAL_TIE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               ignore_Thermal_Tie = TRUE;
         }
         else if (!STRICMP(lp, ".COPPER_CUTOUT"))
         {
             if ((lp = get_string(NULL, " \t\n")) == NULL)
                 continue;
             if (toupper(lp[0]) == 'O' || toupper(lp[0]) == 'V')
                 copper_cutout = toupper(lp[0]);
         }
         else if (!STRICMP(lp, ".EXPLODE_POWER_SIGNAL_LAYER_SHAPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
//            if (toupper(lp[0]) == 'Y' && hasLayerFile)
               explodePowerSignalLayer = TRUE;
         }
			else if(!STRICMP(lp, ".FILTER_GRAPHIC_CLASS"))
			{
				if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;
				int graphicClass = atoi(lp);
				if (graphicClass > -1 && graphicClass < MAX_CLASSTYPE)
					grClassFilterArray.SetAtGrow(grClassFilterCnt++, graphicClass);
			}
			else if(!STRICMP(lp, ".FILTER_INSERT_TYPE"))
			{
				if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;
				int insertType = atoi(lp);
				if (insertType > -1 && insertType < MAX_INSERTTYPE)
					insertTypeFilterArray.SetAtGrow(insertTypeFilterCnt++, insertType);
			}
         else if (!STRICMP(lp, ".SET_PARTNUMBER_FROM_DEVICE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               setPartnumberAttribFromDevice = false;
         }
         else if(!STRICMP(lp, ".DEFAULT_UNITS"))
         {
            if ((lp = get_string(NULL, " \t\n")) != NULL)
            {
               mentor_defaultunits = lp;
               mentor_defaultunits.MakeUpper();
            }
         }
		}
   }

   fclose(fp);
   return 1;
}

/******************************************************************************
* clean_text
*/
static void clean_text(char *text)
{
   if (strlen(text) == 0)
      return;

   // delete all traling isspace
   while (strlen(text) && isspace( text[strlen(text) - 1])) 
      text[strlen(text) - 1] = '\0';

   if (strlen(text) && text[strlen(text) - 1] == '\"')
      text[strlen(text) - 1] = '\0';
   STRREV(text);

   while (strlen(text) && isspace(text[strlen(text) - 1]))  
      text[strlen(text) - 1] = '\0';

   if (strlen(text) && text[strlen(text) - 1] == '\"')
      text[strlen(text) - 1] = '\0';
   STRREV(text);

   // only allow printable characters
   CString tmp;
   for (unsigned int i=0; i<strlen(text); i++)
   {
      if (text[i] >= 0 && text[i] < 256 && isprint(text[i]))
         tmp += text[i];
   }
   strcpy(text, tmp);
}

/******************************************************************************
* clean_name
*/
static void clean_name(char *name)
{
   if (strlen(name) == 0)
      return;

   while (strlen(name) && isspace(name[strlen(name) - 1]))  
      name[strlen(name) - 1] = '\0';

   if (strlen(name) && name[strlen(name) - 1] == '\"')
      name[strlen(name)-1] = '\0';
   STRREV(name);

   while (strlen(name) && isspace(name[strlen(name) - 1]))  
      name[strlen(name) - 1] = '\0';

   if (strlen(name) && name[strlen(name) - 1] == '\"')
      name[strlen(name) - 1] = '\0';
   STRREV(name);
   return;
}

/******************************************************************************
* clean_layer
*/
static void clean_layer(char *layer)
{
   if (strlen(layer) == 0)
      return;

   // delete all traling isspace
   while (strlen(layer) && isspace(layer[strlen(layer)-1])) 
      layer[strlen(layer) - 1] = '\0';

   if (strlen(layer) && layer[strlen(layer) - 1] == '\"')
      layer[strlen(layer) - 1] = '\0';
   STRREV(layer); 

   while (strlen(layer) && isspace(layer[strlen(layer) - 1]))  
      layer[strlen(layer) - 1] = '\0';

   if (strlen(layer) && layer[strlen(layer) - 1] == '\"')
      layer[strlen(layer) - 1] = '\0';
   STRREV(layer);

   const char *layerRename = get_layerrename(layer);
   strcpy(layer, layerRename);

   return;
}

/******************************************************************************
* int write_poly
*/
static int write_poly(int layer, double width, int fill, int close, int graphic_class, 
							 int dataType, BOOL clearPolyArray, BOOL addto_layerToDataMap)
{
   if (polycnt < 2)  
   {
      polyarray.RemoveAll();
      polycnt = 0;
      return 0;
   }

   int err;
   int widthindex = 0;
   if (width > 0)
      widthindex = Graph_Aperture("", T_ROUND, width, 0, 0, 0, 0, 0, BL_WIDTH, 0, &err);

   DataStruct *data = Graph_PolyStruct(layer, 0, 0);
   data->setGraphicClass(graphic_class);

   MenPoly poly1 = polyarray[0];
   MenPoly poly2 = polyarray[polycnt-1];

   int closed;
   if (close)  // make sure it is closed
   {
      if (poly1.x != poly2.x || poly1.y != poly2.y)
         polyarray.SetAtGrow(polycnt++, poly1);
      closed = TRUE;
   }
   else
   {
      closed = (poly1.x == poly2.x && poly1.y == poly2.y);
   }
   Graph_Poly(NULL, widthindex, fill, 0, closed);

	// here write lines.
	for (int i=0; i<polycnt; i++)
	{
		MenPoly poly = polyarray.ElementAt(i);
		Graph_Vertex(poly.x, poly.y, 0.0);
	}

	if (clearPolyArray)
	{
		polyarray.RemoveAll();
		polycnt = 0;
	}

	if (addto_layerToDataMap)
	{
		// Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
		LayerStruct *lay = doc->FindLayer(layer); 
		CDataList *dataList = GetCurrentDataList();
		AddDataToLayer(lay->getName(), dataList, data, dataType);
	}

   return 0;
}

/******************************************************************************
* cnv_tok
*/
static double cnv_tok(char *l)
{
   if (l == NULL || strlen(l) == 0)
      return 0.0;

   double x = atof(l);
   return x * scale_factor;
}

/****************************************************************************/
/*
      Like fgets only better, no pain in the neck arbitrary fixed buffers
*/

static bool fgetcs(CString &string, FILE *ifp)
{
   string.Empty();
	
   char c;
   while ( (c = fgetc(ifp)) != EOF )
   {
      string += c;
      
      if (c == '\n')
         return true;
   }
 
   // Even apparantly blank lines in the file will have a newline, so we should
   // never get a truly empty string unless we hit EOF.
   if(EOF == c)
      return false;
   else 
      return true;
}

/******************************************************************************
* *get_nextline
*/
static bool get_nextline(CString &string, FILE *fp)
{
   CString tmp;

   if (Push_NextLine)
   {
      //string.CompareNoCase(lastline);
      Push_NextLine = FALSE;
      return true;
   }
   
   string.Empty();
   if (!fgetcs(tmp, fp))
      return false;
   ifp_linecnt++;
   
   if (((ifp_linecnt % 200) == 1) && fileReadProgress != NULL)
   {
      fileReadProgress->updateProgress();
   }
   
   // clean whitespace in the end
   tmp.TrimRight();
   string.Append(tmp);
   string.TrimRight("-");
   
   while(tmp.GetLength() && tmp.Right(1).CompareNoCase("-") == 0)
   {
      //tmp.TrimRight("-");
      if (!fgetcs(tmp, fp))
         break;
      ifp_linecnt++;
      
      // clean whitespace in the end
      tmp.TrimRight();
      string.Append(tmp);
      string.TrimRight("-");
   }
   
   //lastline = string;
   return true;
}

/******************************************************************************
* *get_nextline
*/
static char *get_nextline(FILE *fp)
{
   static CString cifp_line;
   
   if(cifp_line.GetLength())
      cifp_line.ReleaseBuffer();
   
   return (get_nextline(cifp_line, fp))? cifp_line.GetBuffer(0):NULL;
}


/******************************************************************************
* do_arc
*/
static int do_arc(const char *lay, BOOL addto_layerToDataMap)
{
   double cx;
   double cy;
   double radius;
   double sa;
   double da;
   char *lp;
   
   int err;
   int widthindex = 0;
   if (template_path_width > 0)
      widthindex = Graph_Aperture("", T_ROUND, template_path_width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   int laynr;
	if (!STRCMPI(lay, "0"))
		laynr = Graph_Level(lay, "", TRUE); // Floating layer
	else
		laynr = Graph_Level(lay, "", 0);

   lp = get_string(NULL, " \r\t\n");  
   if (lp && strlen(lp) > 1)  // just a comma here, use polycnt
   {
      double x1 = cnv_tok(lp);

      if ((lp = get_string(NULL," \r\t\n")) == NULL)  
         return 0;
      double y1 = cnv_tok(lp);

      if ((lp = get_string(NULL," \r\t\n")) == NULL)  
         return 0;
      double x2 = cnv_tok(lp);

      if ((lp = get_string(NULL," \r\t\n")) == NULL)  
         return 0;
      double y2 = cnv_tok(lp);

      if ((lp = get_string(NULL," \r\t\n")) == NULL)  
         return 0;
      double x3 = cnv_tok(lp);

      if ((lp = get_string(NULL," \r\t\n")) == NULL)  
         return 0;
      double y3 = cnv_tok(lp);

      double w = template_path_width;
      if ((lp = get_string(NULL, " \r\t\n")) != NULL)  
         w = cnv_tok(lp);

      if (w == 0)
         widthindex = 0;   // make it small width.
      else
         widthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

      if (fabs(x1 - x3) < SMALLNUMBER && fabs(y1 - y3) < SMALLNUMBER)
      {
         // this is only defined if points are in counter clock wise                   
         // but this CirclePoint3 function does not assume this.  
         CirclePoint3(x1, y1, x2, y2, x3, y3, &cx, &cy, &radius);
         DataStruct *data = Graph_Circle(laynr, cx, cy, radius, 0L, widthindex ,FALSE, FALSE); 
         data->setGraphicClass(get_graphic_class(lay));

			if (addto_layerToDataMap)
			{
				// Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
				CDataList *dataList = GetCurrentDataList();
				AddDataToLayer(lay, dataList, data, DATA_ARC);
			}
         return 1;
      }
      else
      {
         ArcPoint3(x1, y1, x2, y2, x3, y3, &cx, &cy, &radius, &sa, &da);
         // arc is placed down there.
      }
   }
   else
   {
      if (polycnt != 3)
         return 0;

      // get template width
      // 0 = centerx,y
      // 1 = startx,y
      // 2 = endx,y  

      MenPoly poly = polyarray[0];
      cx = poly.x;
      cy = poly.y;

      poly = polyarray[1];
      double x1 = poly.x;
      double y1 = poly.y;

      poly = polyarray[2];
      double x2 = poly.x;
      double y2 = poly.y;

      ArcCenter2(x1, y1, x2, y2, cx, cy, &radius, &sa, &da, 0);
      polycnt = 0;
   }

   DataStruct *data = Graph_Arc(laynr, cx, cy, radius, sa, da, 0L, widthindex, FALSE); 
   data->setGraphicClass(get_graphic_class(lay));

	if (addto_layerToDataMap)
	{
		// Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
		CDataList *dataList = GetCurrentDataList();
		AddDataToLayer(lay, dataList, data, DATA_ARC);
	}

   return 1;
}

/******************************************************************************
* get_nextword
*  like get_string, lp1 is the first and NULL to continue
*/
static int get_nextword(char *lp1, char *token, int maxtok, char *attval, int maxatt)
{
   int res = 0;
   int cnt;
   static char *lp;

   if (lp1)
      lp = lp1;

   while (*lp && isspace(*lp))
      *lp++;

   switch (*lp)
   {
   case '(':
      {
         // do until ), but watch out it is allowed in text
         cnt = 0;
         *lp++;   // advance (
         while (*lp && *lp != ',')  
         {
            if (*lp == '\\')
               *lp++;
            if (cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }

         if (*lp)
            *lp++;   // skip the closing braket

         token[cnt] = '\0';

         while (*lp && isspace(*lp))
            *lp++;

         if (*lp == NULL)  
            return 0;

         // do until ), but watch out it is allowed in text
         cnt = 0;
         char qualifier = '"';
         if (*lp == '\'')
            qualifier = *lp;

         *lp++;   // advance "
         while (*lp && *lp != qualifier)  
         {
            if (*lp == '\\')
               *lp++;
            if (cnt < maxatt)
               attval[cnt++] = *lp;
            *lp++;
         }

         if (*lp)
            *lp++;   // skip the closing quote

         if (*lp)
            *lp++;   // skip the closing braket

         attval[cnt] = '\0';
         res = 2;
      }
      break;

   case '\'':
      {
         // do until \'
         cnt = 0;
         *lp++;   // advance 1 step
         while (*lp && *lp != '\'') 
         {
            if (*lp == '\\')
               *lp++;

            if (cnt < maxtok)
               token[cnt++] = *lp;

            *lp++;
         }
         if (*lp)
            *lp++;   // skip the endquote
         token[cnt] = '\0';
         res = 1;
      }
      break;

   default:
      {
         // do until white space
         cnt = 0;
         while (*lp && !isspace(*lp))  
         {
            if (cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
            res = 1;
         }
         token[cnt] = '\0';
      }
      break;
   }

   return res;
}

/******************************************************************************
* get_nextnetword
*  like get_string, lp1 is the first and NULL to continue
*/
static int get_nextnetword(char *lp1,char *token,int maxtok, char *attval, int maxatt, int netname)
{
   int res = 0;
   int cnt;
   static char *lp;

   if (lp1)
      lp = lp1;

   while (*lp && isspace(*lp))
      *lp++;
   while (*lp && *lp == '-')
      *lp++;

   if (*lp == NULL)
      return 0;

   // on netname allow '-5V'
   // on pinname 'R5-1' is R5 and next 1
   if (netname)   
   {
      if(*lp == '(')
      {
         // do until ), but watch out it is allowed in text
         cnt = 0;
         *lp++;   // advance (
         while (*lp && *lp != ',')  
         {
            if (*lp == '\\')
               *lp++;
            if(cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }
         if (*lp)
            *lp++;   // skip the closing braket
         token[cnt] = '\0';

         while (*lp && isspace(*lp))
            *lp++;
         if (*lp == NULL)
            return 0;

         // do until ), but watch out it is allowed in text
         cnt = 0;
         // need to deal with quote and without
         // (NET_TYPE,strobe)  (PCB_BA_NAME,"/CAS(0)")
         if (*lp == '"')
         {
            *lp++;   // advance "
            while (*lp && *lp != '"' ) 
            {
               if (*lp == '\\')
                  *lp++;
               if(cnt < maxatt)
                  attval[cnt++] = *lp;
               *lp++;
            }
            if (*lp)
               *lp++;   // skip the closing quote
         }
         else
         {
            while (*lp && *lp != ')' ) 
            {
               if (*lp == '\\')
                  *lp++;
               if(cnt < maxatt)
                  attval[cnt++] = *lp;
               *lp++;
            }
         }

         if (*lp)
            *lp++;   // skip the closing braket
         attval[cnt] = '\0';
         res = 2;
      }  
      else if (*lp == '\'')
      {
         // do until \'
         cnt = 0;
         *lp++;   // advance 1 step
         while (*lp && *lp != '\'') 
         {
            if (*lp == '\\')
               *lp++;         
            if(cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }
         if (*lp)
            *lp++;   // skip the endquote
         token[cnt] = '\0';
         res = 1;
      }
      else if (*lp == '\"')
      {
         // do until \"
         cnt = 0;
         *lp++;   // advance 1 step
         while (*lp && *lp != '\"') 
         {
            if (*lp == '\\')
               *lp++;         
            if(cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }
         if (*lp)
            *lp++;   // skip the endquote
         token[cnt] = '\0';
         res = 1;
      }
      else
      {
         // do until white space
         cnt = 0;
         while (*lp && !isspace(*lp) && *lp != '\'')  
         {
            if(cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }
         if(*lp == '\'')
            *lp++;
         token[cnt] = '\0';
         res = 1;
      }
   }
   else
   {
      if(*lp == '(')
      {
         // do until ), but watch out it is allowed in text
         cnt = 0;
         *lp++;   // advance (
         while (*lp && *lp != ',')  
         {
            if (*lp == '\\')
               *lp++;
            if(cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }
         if (*lp)
            *lp++;   // skip the closing braket
         token[cnt] = '\0';

         while (*lp && isspace(*lp))
            *lp++;
         if (*lp == NULL)
            return 0;

         // do until ), but watch out it is allowed in text
         cnt = 0;
         // need to deal with quote and without
         // (NET_TYPE,strobe)  (PCB_BA_NAME,"/CAS(0)")
         if (*lp == '"')
         {
            *lp++;   // advance "
            while (*lp && *lp != '"' ) 
            {
               if (*lp == '\\')
                  *lp++;
               if(cnt < maxatt)
                  attval[cnt++] = *lp;
               *lp++;
            }
            if (*lp)
               *lp++;   // skip the closing quote
         }
         else
         {
            while (*lp && *lp != ')' ) 
            {
               if (*lp == '\\')
                  *lp++;
               if(cnt < maxatt)
                  attval[cnt++] = *lp;
               *lp++;
            }
         }

         if (*lp)
            *lp++;   // skip the closing braket
         attval[cnt] = '\0';
         res = 2;
      }  
      else if (*lp == '\'')
      {
         // do until \'
         cnt = 0;
         *lp++;   // advance 1 step
         while (*lp && *lp != '\'' && *lp != '-')  
         {
            if (*lp == '\\')
               *lp++;         
            if(cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }
         if (*lp)
            *lp++;   // skip the endquote
         
         // check for refdes of A- or -, i.e. '--1' = comp -, pin 1
         if (*lp && *lp == '-')
            token[cnt++] = '-';
         token[cnt] = '\0';
         res = 1;
      }
      else
      {
         // do until white space
         cnt = 0;
         while (*lp && !isspace(*lp) && *lp != '-' && *lp != '\'')   
         {
            if(cnt < maxtok)
               token[cnt++] = *lp;
            *lp++;
         }
         if(*lp == '\'')
            *lp++;
         token[cnt] = '\0';
         res = 1;
      }
   }

   return res;
}

/******************************************************************************
* is_command
*/
static int is_command(char *command, List *c_list, int siz_of_command)
{
   for (int i=0; i<siz_of_command; i++)
   {
      if (!STRCMPI(c_list[i].token, command))
         return i;
   }

   return -1;
}

/******************************************************************************
*/
static CString get_blindvianamelist()
{
	int count = 0;
	CString namelist;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
   
      if (block->getBlockType() != BLOCKTYPE_PADSTACK)
         continue;

      Attrib* attrib = is_attvalue(doc, block->getAttributesRef(), ATT_BLINDSHAPE, 0);

		if (attrib)
		{
			count++;
			CString name = block->getName();
			if (count > 1)
				namelist += ", ";
			namelist += name;
		}
	}

	return namelist;
}

/******************************************************************************
* get_buriedvianames
   only used before reading trace file.
*/
static int get_buriedvianames()
{
	// Make sure list is cleared
   for (int i=0; i<buriedviacnt; i++)
      delete buriedviaarray[i];  
   buriedviaarray.RemoveAll();
   buriedviacnt = 0;

	// Build list
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
   
      if (block->getBlockType() != BLOCKTYPE_PADSTACK)
         continue;

      Attrib* attrib = is_attvalue(doc, block->getAttributesRef(), ATT_BLINDSHAPE, 0);
      if (!attrib)
         continue;

      int l1, l2;
      char lp[80];
      int res = sscanf(block->getName(), "%s (%d:%d)", &lp, &l1, &l2);

      if (res == 3)
      {
         MEN4BuriedVia *c = new MEN4BuriedVia;
         buriedviaarray.SetAtGrow(buriedviacnt++, c);  
         c->vianame = block->getName();
         c->origname = lp;
         c->l1 = l1;
         c->l2 = l2;
      }
   }

   return 1;
}

/******************************************************************************
* change_vias_2_testpads
*/
static int change_vias_2_testpads()
{
   for (int i=0; i<padlistcnt; i++)
   {
      MEN4Padlist *padList = padlistarray[i];
      if (padList->via == FALSE)
         continue;

      BlockStruct *block = Graph_Block_Exists(doc, padList->name, pcbFileNum);
      if (!block)
         continue;

      double drill = get_drill_from_block_no_scale(doc, block);
      if (drill == 0)
      {
         // can not be a via, must be a testpad
         block->setBlockType(BLOCKTYPE_TESTPAD);
      }
   }

   return 1;
}

/******************************************************************************
* tst_filetype
*/
int tst_filetype(FILE *file, const char *filename)
{
   // MEN_UNKNOWN          
   // MEN_GEOM          
   // MEN_COMP          
   // MEN_WIRE       
   // MEN_TECHNOLOGY
   // MEN_PACKAGE
   // MEN_LAYER

   int ftype = MEN_UNKNOWN;

   if (!ask_file_type)
      return tst_mentorbrdfiletype(file);
   rewind(file);

   // here it is unknown - need to give the user a change to force a filetype
   MentorFileType dlg;
   dlg.DoModal();

   switch (dlg.m_format)
   {  // 0=Geometry, 1=Component, 2=Netlist, 3=Traces, 4 technology, 5 package, 6 layer
   case 0:  
      return MEN_GEOM;
   case 1:
      return MEN_COMP;
   case 2:
      return MEN_NETLIST;
   case 3:
      return MEN_WIRE;
   case 4:
      return MEN_TECHNOLOGY;
   case 5:
      return MEN_PACKAGE;
   case 6:
      return MEN_LAYER;
   default:
      return MEN_UNKNOWN;
   }

   return ftype;
}

/******************************************************************************
* tst_mentorbrdfiletype
*/
int tst_mentorbrdfiletype(FILE *file)
{
   CString iline;

   while (get_nextline(iline, file) != 0)
   {
      CString header = iline;
      header.MakeUpper();

      // MEN_GEOM
      if(iline.Find("$$create_") == 0)
      {
         rewind(file);
         return MEN_GEOM;
      }

      // MEN_COMP
      if (header.Find ("PANTHEON") > 0 && header.Find("COMP FILE") > 0)
      {
         rewind(file);
         return MEN_COMP;
      }
      if (header.Find ("BOARD STATION") > 0 && header.Find("COMPONENTS FILE") > 0)
      {
         rewind(file);
         return MEN_COMP;
      }

      // MEN_PACKAGE
      if (header.Find ("BOARD STATION") > 0 && header.Find("GATE (INSTANCE) FILE") > 0) 
      {
         rewind(file);
         return MEN_PACKAGE;
      }

      // MEN_TECHNOLOGY
      if (header.Find ("BOARD STATION") > 0 && header.Find("TECHNOLOGY FILE") > 0)
      {
         rewind(file);
         return MEN_TECHNOLOGY;
      }

      // MEN_NETLIST
      if (header.Find ("BOARD STATION") > 0 && header.Find("NETS FILE") > 0)
      {
         rewind(file);
         return MEN_NETLIST;
      }
      if (header.Find ("PANTHEON") > 0 && header.Find("NETS FILE") > 0)
      {
         rewind(file);
         return MEN_NETLIST;
      }

      // MEN_WIRE
      if (header.Find ("BOARD STATION") > 0 && header.Find("TRACES FILE") > 0)
      {
         rewind(file);
         return MEN_WIRE;
      }
      if (header.Find ("BOARD STATION") > 0 && header.Find("WIRES FILE") > 0)
      {
         rewind(file);
         return MEN_WIRE;
      }
      if (header.Find ("PANTHEON") > 0 && header.Find("TRACES FILE") > 0)
      {
         rewind(file);
         return MEN_WIRE;
      }

      // MEN_LAYER
      if (header.Find ("BOARD STATION") > 0 && header.Find("LAYER FILE") > 0)
      {
         rewind(file);
         return MEN_LAYER;
      }

      // no header, nets start immediately
      // NET 'PA16' U108-33 U14-2
      if (iline.Find("NET ") == 0)  
      {
         rewind(file);
         return MEN_NETLIST;
      }

      //# Application : LAYOUT v8.5_4.1 Wed Feb  5 17:59:23 PST 1997
      //# date : Thursday January 22, 1998; 09:33:54 
      //#
      //UNITS TN
      //ST 30008576 38101000 19 9732
      //XRF   1 SIGNAL_1
      //XRF   2 SIGNAL_2
      if (iline.Find("ST ") == 0)   
      {
         rewind(file);
         return MEN_WIRE;
      }

      if (header.Find ("BOARD STATION") > 0 && header.Find("TESTPOINTS FILE") > 0)
      {
         rewind(file);
         return MEN_TESTPOINT;
      }

      if (header.Find ("BOARD STATION") > 0 && header.Find("PINS FILE") > 0)
      {
         rewind(file);
         return MEN_PINS;
      }

   }

   return MEN_UNKNOWN;
}

/******************************************************************************
* do_units
*  - Units from Wire, Trace, Component file.
*/
static int do_units(const char *unitName)
{
   if (!STRNICMP(unitName, "TN", 2))
      scale_factor = Units_Factor(UNIT_TNMETER, page_unit);
   else if (!STRNICMP(unitName, "IN", 2))
      scale_factor = Units_Factor(UNIT_INCHES, page_unit);
   else if (!STRNICMP(unitName, "MI", 2))
      scale_factor = Units_Factor(UNIT_MILS, page_unit);
   else if (!STRNICMP(unitName, "ML", 2))
      scale_factor = Units_Factor(UNIT_MILS, page_unit);
   else if (!STRNICMP(unitName, "MM", 2))
      scale_factor = Units_Factor(UNIT_MM, page_unit);
   else
   {
      fprintf(ferr, "Unknown UNITS [%s] in [%s] at %ld ", unitName, cur_filename, ifp_linecnt);
      display_error++;
      return 0;
   }

   return 1;
}

/******************************************************************************
* is_attrib
*/
static int is_attrib(const char *name, int atttype)
{
   for (int i=0; i<attrcnt; i++)
   {
      if (attr[i].atttype == atttype && !STRICMP(attr[i].name, name))
         return i+1;
   }

   return 0;
}

/******************************************************************************
* get_textjust
*/
static DbFlag get_textjust(const char* l)
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

   // height justification
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

// 0=left, 1=center, 2=right
HorizontalPositionTag getTextAlignment(DbFlag textJust)
{
   HorizontalPositionTag alignment = horizontalPositionLeft;

   if      ((textJust & GRTEXT_W_L) != 0) alignment = horizontalPositionLeft;
   else if ((textJust & GRTEXT_W_C) != 0) alignment = horizontalPositionCenter;
   else if ((textJust & GRTEXT_W_R) != 0) alignment = horizontalPositionRight;

   return alignment;
}

// 0=bottom, 1=center, 2=top
VerticalPositionTag getLineAlignment(DbFlag textJust)
{
   VerticalPositionTag alignment = verticalPositionBaseline;

   if      ((textJust & GRTEXT_H_B) != 0) alignment = verticalPositionBaseline;
   else if ((textJust & GRTEXT_H_C) != 0) alignment = verticalPositionCenter;
   else if ((textJust & GRTEXT_H_T) != 0) alignment = verticalPositionTop;

   return alignment;
}

/******************************************************************************
* correct_pinposition
*/
static int correct_pinposition(const char *compname, const char *pinname, const char *geomname, double pinx , double piny)
{

   // pin pin in geom.
   int found = 0;

   BlockStruct *block = Graph_Block_Exists(doc, geomname, pcbFileNum);
   if (!block)
   {
      fprintf(ferr, "Can not update moved pin position, because no GEOMETRY [%s] not defined\n", geomname);
      display_error++;
      return -1;
   }

   DataStruct *data;
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      data = block->getDataList().GetNext(pos);
      
      if (data->getDataType() != T_INSERT)        
         continue;
      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

      // - pinname is in refname if a insert
      // - this can not be compares against refname but ATT_COMPPINNR, because is 1 2 is changed to 2 1 
      // then it will be wrong.
      // -these are absolute instructions.
      Attrib* attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_COMPPINNR, 0);
      if (!attrib)
         continue;

      if (attrib->getValueType() == VT_INTEGER)
      {
         if (atoi(pinname) == attrib->getIntValue())
         {
            data->getInsert()->setOriginX(pinx);
            data->getInsert()->setOriginY(piny);
            found++;
            break;
         }
      }
      else if (attrib->getValueType() == valueTypeString)
      {
         CString pinName = attrib->getStringValue();
         if (pinName.Compare(pinname) == 0)
         {
            data->getInsert()->setOriginX(pinx);
            data->getInsert()->setOriginY(piny);
            found++;
            break;
         }
      }
   } 

   if (!found)
   {
      fprintf(ferr, "Can not update moved pin position, because no Pin [%s] not found in GEOMETRY [%s]\n", 
            pinname, geomname);
      display_error++;
   }
 
   return 1;
}

/******************************************************************************
* do_component
*/
static int do_component()
{
   char *line = NULL;
   int refNameKeyword = doc->IsKeyWord(ATT_REFNAME, TRUE);
   setFileReadProgress("Processing components");

   while ((ifp_line = get_nextline(ifp)))
   {
      if (strlen(ifp_line) < 2)
         continue;
      if (ifp_line[strlen(ifp_line)-1] == '\n')
         ifp_line[strlen(ifp_line)-1] = '\0';
      CString copy = ifp_line;

      char *tempLine;
      if ((tempLine = get_string(ifp_line, "\n")) == NULL) 
         continue;

      if (line)
      {
         free(line);
         line = NULL;
      }
      line = STRDUP(tempLine);

      // UNITS TN, ...
      //# Reference  Part_number  Symbol  Geometry  Board_location  Properties

      char tok[80], att[255];
      if (get_nextword(line, tok, 80, att, 255) == 0)
         continue;
      if (tok[0] == '#')   // remark
         continue;

      if (!STRCMPI(tok, "UNITS"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0) 
            continue;
         do_units(tok);
         continue;
      }
      CString compName = tok;
      compName.MakeUpper();   // Mentor component are not case sensitive

      if (!get_nextword(NULL, tok, 80, att, 255))  // Part_Number
         continue; 
      char partNumber[80];
      strcpy(partNumber, tok);

      if (!get_nextword(NULL, tok, 80, att, 255))  // Symbol
         continue; 
      char symbol[80];
      strcpy(symbol, tok);

      if (!get_nextword(NULL, tok, 80, att, 255)) 
         continue;
      CString blockName = tok;
      blockName.MakeUpper();

      BlockStruct *block = Graph_Block_Exists(doc, blockName, pcbFileNum);

      if (!block)
      {
         if (IGNORE_MISSING_GEOM_ENTRIES)
         {
            // Block type is unknown so added insert refname to the map to be remove from netlist later
            compToBeRemove.SetAt(compName, compName);

            fprintf(ferr, "Error : Geometry [%s] for Component [%s] not defined - Component will be empty.\n", blockName, compName);
            display_error++;

            block = Mentor_Graph_Block_On(GBO_APPEND, blockName, pcbFileNum, 0);
            Graph_Block_Off();
         }
         else
         {
            CString msg;
            msg.Format("Error : Geometry [%s] for Component [%s] not defined. Processing will halt due to incomplete input data.  To countine processing, set the command \".IGNORE_MISSING_GEOM_ENTRIES\" to \"Y\".\n", blockName, compName);
            ErrorMessage(msg, "File Error");
            return -1;
         }
      }

      int stored_att = FALSE;
      double x, y;
      int rot, mir = 0;
      BOOL unplaced;

      int res = get_nextword(NULL, tok, 80, att, 255);
      if (!res && MAKE_LOCATION) // unplaced without attributes
      {
         x = 0;
         y = 0;
         mir = 0;
         rot = 0;
         unplaced = TRUE;
      }
      else if (!res)
      {
         continue;
      }
      else if (res == 2) // unplaced with attributes
      {
         // here attrib
         //continue;
         x = 0;
         y = 0;
         mir = 0;
         rot = 0;
         unplaced = TRUE;
         stored_att = TRUE;
      }
      else
      {
         unplaced = FALSE;
         x = cnv_tok(tok);

         if ((res = get_nextword(NULL, tok, 80, att, 255)) == 0) 
            continue;
         if (res == 2)  // here attrib
            continue;
         y = cnv_tok(tok);

         if ((res = get_nextword(NULL, tok, 80, att, 255)) == 0) 
            continue;
         if (res == 2)  // here attrib
            continue;
         mir = (atoi(tok) > 1);

         if ((res = get_nextword(NULL, tok, 80, att, 255)) == 0) 
            continue;
         if (res == 2)  // here attrib
            continue;
         rot = atoi(tok);

         if(rot==270 && mir)
            rot = rot;
      }

      if (mir)
         rot = 360 - rot;

      while (rot < 0)
         rot += 360;
      while (rot >=360)
         rot -= 360;

      BOOL visible = TRUE;

      // check here if there was a MovePin. If yes, make a new geomname_refname, copy and go from here.
      char *t1;
		if (t1 = strstr(copy.GetBuffer(), "$MP"))
      {
         CString newGeomName;
         newGeomName.Format("%s_%s", blockName, compName);

         CString mess;
         mess.Format("Geometry [%s] Component [%s] has moved pins -> changed to [%s]\n", blockName, compName, newGeomName);
         display_error++;

         log_information_refname(ferr, compName, mess, ifp_linecnt);
         BlockStruct *origGeom = Graph_Block_Exists(doc, blockName, pcbFileNum);

         BlockStruct *newGeom = Mentor_Graph_Block_On(GBO_APPEND, newGeomName, pcbFileNum, 0, blockTypePcbComponent);
         Graph_Block_Copy(origGeom, 0, 0, 0, 0, 1, -1, TRUE);
         Graph_Block_Off();

         doc->SetUnknownAttrib(&newGeom->getAttributesRef(), ATT_DERIVED, blockName, attributeUpdateOverwrite, NULL);
         blockName = newGeomName;
      }

      block = Graph_Block_Exists(doc, blockName, pcbFileNum);

      if (!block)
      {
         if (IGNORE_MISSING_GEOM_ENTRIES)
         {
            compToBeRemove.SetAt(compName, compName);
            fprintf(ferr, "Try to place a component [%s] Geometry [%s], which is not defined!\n", compName, blockName);
            display_error++;
            continue;
         }
         else
         {
            CString msg;
            msg.Format("Error : Geometry [%s] for Component [%s] not defined. Processing will halt due to incomplete input data.\n", blockName, compName);
            ErrorMessage(msg, "File Error");
            return -1;
         }
      }

      // check if not already placed as fixed component.
      DataStruct *data = datastruct_from_refdes(doc, currentPCBFile->getBlock(), compName);
      if (!data)
      {
         data = Graph_Block_Reference(block->getBlockNumber(), compName, x, y, DegToRad(1.0 * rot), mir , 1.0, -1);

         if (block->getBlockType() == blockTypeTooling)  // Check if the geometry type is Tooling, then insert type should be tooling also            
            data->getInsert()->setInsertType(insertTypeDrillTool);
			else if (block->getBlockType() == blockTypeFiducial)
				data->getInsert()->setInsertType(insertTypeFiducial);
         else
            data->getInsert()->setInsertType(insertTypePcbComponent);
      }
      else
      {
         // already placed in FIXED_COMPONENT_LOCATION in geoms_ascii
         // this could have been an $$ADD generic component.
         if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         {
            // rename the refname of the generic comp
            CString refName;
            refName.Format("$$%s", data->getInsert()->getRefname());
            data->getInsert()->setRefname(STRDUP(refName));

            fprintf(ferr, "Component Reference name [%s] used $$add -> changed to [%s]\n", compName, refName);
            display_error++;

            // place the part
            data = Graph_Block_Reference(block->getBlockNumber(), compName, x, y, DegToRad(1.0 * rot), mir, 1.0, -1);
            data->getInsert()->setInsertType(insertTypePcbComponent);
         }
			else
			{
				data->getInsert()->setBlockNumber(block->getBlockNumber());
			}
      }

      if (unplaced)
         doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_UNPLACED, TRUE), VT_NONE, NULL, attributeUpdateOverwrite, NULL);

      // here check if a REFNAME was there - update rotation and name
      Attrib* attrib = is_attvalue(doc, data->getAttributesRef(), ATT_REFNAME, 1);
      if (attrib != NULL)
      {
         // inherit all instances, not just one
         attrib->setValueFromString(doc->getCamCadData(), refNameKeyword, data->getInsert()->getRefname());
      }

      // here get $PN_EXT : I need to get it  here, because you can not change
      // the partnumber after it is added to the list.
		if ((t1 = strstr(copy.GetBuffer(), "$PN_EXT")) && partnumberextension)
      {
         char *t2 = strchr(t1,')');
         *t2 = '\0';

         while (*t1 && *t1 !='|')
            *t1++;
         *t1++;

         if (t1 && t1[strlen(t1)-1] =='"')   
            t1[strlen(t1)-1] = '\0';

         strcat(partNumber, t1);
      }

      TypeStruct *type = AddType(currentPCBFile, partNumber);
      if (type->getBlockNumber() < 0)
      {
         type->setBlockNumber( data->getInsert()->getBlockNumber());
      }
      else
      {
         if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
         {
            BlockStruct *b1 = doc->Find_Block_by_Num(type->getBlockNumber());
            fprintf(ferr, "Component [%s] %s [%s] Shape [%s] has already a different Shape [%s] assigned!\n",
                  compName, ATT_TYPELISTLINK, partNumber, blockName, b1->getName());
            display_error++;

            CString newPartNumber;
            newPartNumber.Format("%s_%s", partNumber, blockName);
            strcpy(partNumber, newPartNumber);

            TypeStruct *type = AddType(currentPCBFile, partNumber);
            if (type->getBlockNumber() < 0)
               type->setBlockNumber( data->getInsert()->getBlockNumber());
         }
      }
   
      int aptr; // pointer +1 to attr list

      while (stored_att || (res = get_nextword(NULL, tok, 80, att, 255)) == 2)
      {
         stored_att = FALSE;
         if (is_attrib(tok, TYPE_ATT))
         {
            if (!STRCMPI(get_attrmap(tok), ATT_TOLERANCE) || 
                !STRCMPI(get_attrmap(tok), ATT_PLUSTOLERANCE) ||
                !STRCMPI(get_attrmap(tok), ATT_MINUSTOLERANCE))
            {  
               // (TOLER,"TOLER")
               double tol = atof(att);
               if (fabs(tol) > SMALLNUMBER)
                  doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap(tok), att, attributeUpdateOverwrite, NULL); 
            }
            else
            {
               //do_attrib(&d->getAttributesRef(), keyword, valueTypeString, prosa, 0.0, 0, 0L);
               // here need to check if this a an existing attribute.
               doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap(tok), att, attributeUpdateOverwrite, NULL);
            }
         }
         else if (!STRCMPI(tok, "$MP"))   // movable pins
         {
            // att pinnumber,x,y
            // here need to collect the moved pins          
            char *lp;
            if ((lp = get_string(att, ",")) == NULL)
               continue;
            CString pinname = lp;

            if ((lp = get_string(NULL, ",")) == NULL)
               continue;
            double pinx = cnv_tok(lp);

            if ((lp = get_string(NULL, ",")) == NULL)
               continue;
            double piny = cnv_tok(lp);

            correct_pinposition(compName, pinname, blockName, pinx, piny);
         }
         else if (!STRCMPI(tok, "$PN_EXT"))
         {
            // partnumber extension
            // done before.
         }
         else if (!STRCMPI(tok, "REFLOC"))
         {
            int i = 0;
            int rReadingFlag = 0;
            int visible = TRUE;
            int mirror = FALSE;
            double h = 0.07;
            double w = h * 0.8;
            double x = 0;
            double y = 0;
            double r = 0;
            double textR = 0; 
            double old_scale_factor = scale_factor;
            char *string;
            DbFlag just = 0;
            CString strJust;
            CString layername;

            // IN, x, y, 0 , CC, h, w, 0, ,,1
            // some old mentor version does not have the unit command as the start. It is always inch ???
            if (isalpha(att[0]))
            {
               if (string = get_string(att, " \t,"))
                  do_units(string);
               if (string = get_string(NULL, " \t,"))
                  x = cnv_tok(string);
            }
            else
            {
               do_units(DEFAULT_REFLOC_UNITS);
               if (string = get_string(att, " \t,"))
                  x = cnv_tok(string);
            }

            if (string = get_string(NULL, " \t,"))
               y = cnv_tok(string);

            if (string = get_string(NULL, " \t,"))
               r = atof(string); // this is relative to the component

            if (string = get_string(NULL, " \t,"))
            {
               strJust = string;
               just = get_textjust(strJust);
            }

            if (string = get_string(NULL, " \t,"))
               h = cnv_tok(string);
            
            if (string = get_string(NULL, " \t,"))
				w = cnv_tok(string); // char width
			if (w == 0)
				w = h * 0.8; // correct char width if 0
            string = get_string(NULL, " \t,"); // stroke width
            string = get_string(NULL, " \t,"); // font

            if (string = get_string(NULL, " \t,"))
               rReadingFlag = atoi(string);

            if (string = get_string(NULL, " \t,"))
               layername = string;

            CString refnamelayer;
            refnamelayer.Format("%s_1", REFNAME_LAYER);
            int lay;

			if (layername != "")
			{
                CString newLayerName = layername;
                CString layerName_1;
                CString layerName_2;

                // If the layer is a generic Top & Bottom mirror layer, then always use the TOP layer of the generic layer
                if (IsGenericTopBottomMappingLayer(layername, layerName_1, layerName_2))
                    newLayerName.Format("%s_TOP", layername);

                lay = Graph_Level(newLayerName, "", 0);
			}
			else
			{
				lay = Graph_Level(refnamelayer, "", 0);
			}

            if (rReadingFlag)
            {  
               double textR;
               textR = r + rot;
               strJust.MakeUpper();

               while (textR < 0)
                  textR += 360;
               while (textR >= 360)
                  textR -= 360;

               if (!mir && textR >= 112 && textR <= 292)
               {
                  r = r + 180;
                  if (strJust.Find("B") != -1)
                     strJust.Replace("B", "T");
                  else if (strJust.Find("T") != -1)
                     strJust.Replace("T", "B"); 
                  else if (strJust.Find("L") != -1)
                     strJust.Replace("L", "R"); 
                  else if (strJust.Find("R") != -1)
                     strJust.Replace("R", "L");
               }

               if (mir)
                  r = 360 - r;

               if (mir && textR >= 68 && textR <= 248)
               {
                  r = r + 180;
                  if (strJust.Find("B") != -1)
                     strJust.Replace("B", "T");
                  else if (strJust.Find("T") != -1)
                     strJust.Replace("T", "B"); 
                  else if (strJust.Find("L") != -1)
                     strJust.Replace("L", "R"); 
                  else if (strJust.Find("R") != -1)
                     strJust.Replace("R", "L");
               }

               while (r< 0)
                  r += 360;
               while (r >= 360)
                  r -= 360;

               char chrJust[2];
               strcpy(chrJust, strJust);
               just = get_textjust(chrJust);
               //normalize_text(&x, &y, just, r, 0, h, h * strlen(data->getInsert()->refname));
            }

            // only add attributes that do not presently exist on the layer
            // update attributes that do exist on the layer
			// If the refloc does not have a layer and a refloc from the
			// geometry exists then overwrite the geom location for this 
			// instance.
            CAttributeMapWrap dataAttributeMap(data->getAttributesRef());
            Attrib* refNameAttribute;

            if (dataAttributeMap.Lookup(refNameKeyword, refNameAttribute))
            {
               bool foundFlag = false;

               AttribIterator attribIterator(refNameAttribute);
               Attrib attribute(refNameAttribute->getCamCadData());

               for (foundFlag = false; !foundFlag && attribIterator.getNext(attribute);)
               {
                  if (layername == "")
				  {
                     // if no layername specified alter the first refname instance
					 foundFlag = true;
                refNameAttribute->setValueFromString(doc->getCamCadData(), refNameKeyword, data->getInsert()->getRefname());
					 refNameAttribute->setX(x);
				     refNameAttribute->setY(y);
					 refNameAttribute->setRotationDegrees(r);
					 refNameAttribute->setHeight(h);
					 refNameAttribute->setWidth(w);
					 refNameAttribute->setHorizontalPosition(HorizontalPositionTag(getTextAlignment(just)));
					 refNameAttribute->setVerticalPosition(VerticalPositionTag(getLineAlignment(just)));
				  }
			      else if (attribute.getLayerIndex() == lay)
                  {
                     // if layername specified and match found
					 foundFlag = true;
                     refNameAttribute->setValueFromString(doc->getCamCadData(), refNameKeyword, data->getInsert()->getRefname());
					 refNameAttribute->setX(x);
				     refNameAttribute->setY(y);
					 refNameAttribute->setRotationDegrees(r);
					 refNameAttribute->setHeight(h);
					 refNameAttribute->setWidth(w);
					 refNameAttribute->setHorizontalPosition(HorizontalPositionTag(getTextAlignment(just)));
					 refNameAttribute->setVerticalPosition(VerticalPositionTag(getLineAlignment(just)));
                  }
               }

               if (!foundFlag)
               {
                  CString refName(data->getInsert()->getRefname());

                  dataAttributeMap.addInstance(doc->getCamCadData(), refNameKeyword, valueTypeString, refName.GetBuffer(),
                     x, y, DegToRad(r), h, w, 1, 0, visible != 0, 0L, lay, 0, 0,
                     getTextAlignment(just),getLineAlignment(just));
               }
            }

            scale_factor = old_scale_factor;
         }
         else if (!STRCMPI(tok, "$G"))
         {
            // component group - not needed.
         }
         else if (aptr = is_attrib(get_attrmap(tok), HEIGHT_ATT))
         {  
            if (att && strlen(att))
            {
               double height = atof(att) * Units_Factor(attr[aptr-1].unit, page_unit);
               doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &height, attributeUpdateOverwrite, NULL);
            }
         }
         else if (!STRCMPI(get_attrmap(tok), ATT_TOLERANCE) || !STRCMPI(get_attrmap(tok), ATT_PLUSTOLERANCE) ||
               !STRCMPI(get_attrmap(tok), ATT_MINUSTOLERANCE))
         {  
            double tol = atof(att);

            if (fabs(tol) > SMALLNUMBER)
               doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap(tok), att, attributeUpdateOverwrite, NULL);
         }
         else if (!STRCMPI(tok, "MULTI_ASSY"))
         {
            char *tmp = strtok(att, ",");

            while (tmp)
            {
               doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap(tok), tmp, SA_APPEND, NULL);
               tmp = strtok(NULL,",");
            }
         }
         else
         {
            doc->SetUnknownAttrib(&data->getAttributesRef(), get_attrmap(tok), att, attributeUpdateOverwrite, NULL); 
         }
      }

      doc->SetAttrib(&data->getAttributesRef(),    doc->IsKeyWord(ATT_TYPELISTLINK, TRUE), VT_STRING, partNumber, SA_OVERWRITE, NULL);
      if (setPartnumberAttribFromDevice)
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_PARTNUMBER  , TRUE), VT_STRING, partNumber, SA_OVERWRITE, NULL);


      // HERE DO TYPE LIST : Partnumber links uniquely symbol (schematic) and geom (layout)
      doc->SetAttrib(&type->getAttributesRef(), doc->RegisterKeyWord("SYMBOL", 0, valueTypeString), valueTypeString, symbol, attributeUpdateOverwrite, NULL);
   }

   if (line)
   {
      free(line);
      line = NULL;
   }

   return 1;
}

/******************************************************************************
* find_buriedvia
*/
static const char *find_buriedvia(const char *viaName, int l1, int l2)
{
   if (l1 > l2)
   {
      int i = l1;
      l1 = l2;
      l2 = i;
   }

   for (int i=0; i<buriedviacnt; i++)
   {
      if (buriedviaarray[i]->origname.CompareNoCase(viaName) == 0)
      {
         if (buriedviaarray[i]->l1 <= l1 && buriedviaarray[i]->l2 >= l2)
            return buriedviaarray[i]->vianame;
      }
   }

   return viaName;
}

/******************************************************************************
* is_via
   In mentor the via and with have the same entry, A with is a clean number,
   a via has alpha in the number/
*/
static int is_via(const char *vname)
{
   for (unsigned int i=0; i<strlen(vname); i++)
   {
      if (isdigit(vname[i]))
         continue;
      if (vname[i] == '.')
         continue;

      return 1;
   }

   return 0;
}

/******************************************************************************
* get_wire
*/
static int get_wire(int wcnt, const char *cur_netname, CString *xrflayer, int via,const char *keyword = NULL,const char *value = NULL);
static int get_wire(int wcnt, const char *cur_netname, CString *xrflayer, int via,const char *keyword,const char *value)
{
   int widthindex = 0;
   int oldxrflay = -1;
   int oldwidthindex = -1;
   int place_via = 0; // when this is set, it is the starting layer
   double x1;
   double y1;
   char *lp;
   char tok[80];
   char att[255];
   CString vname;   // vianame to place

   for (int i=0; i<wcnt; i++)
   {
      if (!(ifp_line = get_nextline(ifp)))
      {
         CString tmp;
         tmp.Format("Invalid trace file!\nSEG expected at %ld",ifp_linecnt);
         ErrorMessage(tmp,"TRACE read error", MB_OK | MB_ICONHAND);
         return 0;
      }

      if (strlen(ifp_line) < 2)
         continue;
      if (ifp_line[strlen(ifp_line)-1] == '\n')
         ifp_line[strlen(ifp_line)-1] = '\0';
      if ((lp = get_string(ifp_line, "\n")) == NULL)
         continue;

      if (get_nextword(lp, tok, 80, att, 255) == 0) 
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)
         continue;
      double x2 = cnv_tok(tok);

      if (get_nextword(NULL, tok, 80, att, 255) == 0)
         continue;
      double y2 = cnv_tok(tok);

      if (get_nextword(NULL, tok, 80, att, 255) == 0)  
         continue;
      int lnr = atoi(tok);

      if (atoi(tok) < 0 || atoi(tok) > 255)     
         continue;

      // needs to get real layer name
      int xrflay = 0;
      if (strlen(xrflayer[atoi(tok)]))
         xrflay = Graph_Level(xrflayer[atoi(tok)], "", 0);
      else
         xrflay = Graph_Level(tok, "", 0);

      if (get_nextword(NULL, tok, 80, att, 255) == 0)
         continue;

      if (i > 0 && widthindex > 0)  // widthindex 0 is not routed.
      {
         // write line
         if (x1 != x2 || y1 != y2)  
         {
            if (xrflay != oldxrflay || widthindex != oldwidthindex)
            {
               DataStruct *data = Graph_PolyStruct(xrflay, 0, FALSE);
               data->setGraphicClass(graphicClassEtch);

               if (strlen(cur_netname))
                  doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), valueTypeString, (char *)cur_netname, SA_APPEND, NULL);

               if ((NULL != keyword) && strlen(keyword) && (NULL != value))
                  doc->SetUnknownAttrib(&data->getAttributesRef(),keyword,value, attributeUpdateOverwrite, NULL);

               Graph_Poly(NULL, widthindex, FALSE, FALSE, FALSE);
               Graph_Vertex(x1, y1, 0.0);
            }

            Graph_Vertex(x2, y2, 0.0);
            oldxrflay = xrflay;
            oldwidthindex = widthindex;
         }
         else
         {
            // if there was a via, place it.
            if (place_via)
            {
               // via
               // here check if it is a buried via and which name to expand it to.
               // a buried via can be via (1:4) but used on 2:3
               // lnr is the from starting layer
               // the to layer is in the next line !!!
               vname = find_buriedvia(vname, place_via, lnr);

               DataStruct *data = Graph_Block_Reference(vname, NULL, pcbFileNum, x2, y2, DegToRad(0.0), 0, 1.0, Graph_Level("0", "", 1), TRUE);
               BlockStruct *block = Graph_Block_Exists(doc, vname, pcbFileNum);

               if (block->getBlockType() != BLOCKTYPE_PADSTACK)  
                  data->getInsert()->setInsertType(insertTypeTestPad);
               else
                  data->getInsert()->setInsertType(insertTypeVia);

               // this also can be a testpoint.
               // a testpoint is a via which exist on only 1 layer and has no drill
               if (place_via == lnr)
                  doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_TEST, "", attributeUpdateOverwrite, NULL); //  

               if (strlen(cur_netname))
                  doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), valueTypeString, (char *)cur_netname, SA_APPEND, NULL); 

               if ((NULL != keyword) && strlen(keyword) && (NULL != value))
                  doc->SetUnknownAttrib(&data->getAttributesRef(),keyword,value, attributeUpdateOverwrite, NULL);

               oldwidthindex = -1;
               oldxrflay = -1;
            }
            place_via = FALSE;
         }
      }
      else
      {
         if (place_via)
         {
            // via
            // here check if it is a buried via and which name to expand it to.
            // a buried via can be via (1:4) but used on 2:3
            // lnr is the from starting layer
            // the to layer is in the next line !!!
            vname = find_buriedvia(vname, place_via, lnr);

            DataStruct *data = Graph_Block_Reference(vname, NULL, pcbFileNum, x2, y2, DegToRad(0.0), 0, 1.0, Graph_Level("0","",1), TRUE);
            BlockStruct *block = Graph_Block_Exists(doc, vname, pcbFileNum);

            if (block->getBlockType() != BLOCKTYPE_PADSTACK)  
               data->getInsert()->setInsertType(insertTypeTestPad);
            else
               data->getInsert()->setInsertType(insertTypeVia);

            // this also can be a testpoint.
            // a testpoint is a via which exist on only 1 layer and has no drill
            if (place_via == lnr)
            {
               doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_TEST, "", attributeUpdateOverwrite, NULL); //  
            }

            if (strlen(cur_netname))
               doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), valueTypeString, (char *)cur_netname, attributeUpdateOverwrite, NULL); 

            if ((NULL != keyword) && strlen(keyword) && (NULL != value))
                  doc->SetUnknownAttrib(&data->getAttributesRef(),keyword,value, attributeUpdateOverwrite, NULL);
         }

         place_via = FALSE;
      }

      x1 = x2;
      y1 = y2;
      // via or with or end
      if (is_via(tok))
      {
         if (via)
         {
            vname = _strupr(tok);
            place_via = lnr;  // start layer of via
         }
      }
      else
      {
         int err;
         double width = cnv_tok(tok);
         if (width == 0)
            widthindex = 0; 
         else
            widthindex = Graph_Aperture("", T_ROUND, width, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      }

      if (get_nextword(NULL, tok, 80, att, 255) == 0)
         continue;
   }

   return 1;
}

/******************************************************************************
* get_vertex
*/
static int get_vertex(int wcnt, int close)
{
   char *lp;
   char tok[80];
   char att[255];

   for (polycnt=0; polycnt<wcnt; polycnt++)
   {
      if (!(ifp_line = get_nextline(ifp)))
      {
         CString tmp;
         tmp.Format("ARE expected at %ld", ifp_linecnt);
         ErrorMessage(tmp, lp, MB_OK | MB_ICONHAND);
         return -1;
      }

      if ((lp = get_string(ifp_line, "\n")) == NULL)
         continue;
      if (get_nextword(lp, tok, 80, att, 255) == 0) 
         continue;

      if (!strcmp(tok, "VER"))
      {
         MenPoly p;
         p.fill = 0;
         p.type = 0;

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         p.x = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         p.y = cnv_tok(tok);

         polyarray.SetAtGrow(polycnt, p);  // fill, type
      }
      else
      {
         // VER expected
         fprintf(ferr, "VER expected -> [%s] found at %ld\n", tok, ifp_linecnt);
         display_error++;
         return -1;
      }
   }

   if (close)
   {
      // need to close last
      MenPoly p = polyarray.ElementAt(0);  
      polyarray.SetAtGrow(polycnt++, p);
   }

   return polycnt;
}

/******************************************************************************
* get_netname
   change /name to name
   change /xxx/yyy/name to name
*/
static const char *get_netname(char *n)
{
   if (!hierachical_netnames)
   {
      if (n[0] == '/')
      {
         char *lp = strrchr(n, '/');
         return lp+1;
      }
   }

   return n;
}

/******************************************************************************
* get_instname
   change /name to name
   change /xxx/yyy/name to name
*/
static const char *get_instname(char *n)  // instance names are always done.
{
   if (n[0] == '/')
   {
      char *lp = strrchr(n, '/');
      return lp+1;
   }

   return n;
}

/******************************************************************************
* clean_layerstackup
   The stackup can create back layer stacking for Electrical layers.
   This celan then and the Trace XRF layer and if avail the technology file will
   be used.
*/
static int clean_layerstackup(CCEtoODBDoc *doc, bool cleanElect, bool cleanPhys)
{
   for (int j=0; j< doc->getMaxLayerIndex(); j++)
   {
      LayerStruct *layer = doc->getLayerArray()[j];

      if (layer)
      {
         if (cleanElect)
            layer->setElectricalStackNumber(0);
         if (cleanPhys)
            layer->setPhysicalStackNumber(0);
      }
   }

   return 1;
}

/******************************************************************************
* do_wires
   this function is used in MENTORBRD and NEUTRAL read. vias are not read in
   neutral read, because they are loaded in NEUTRAL.

   pcb_overview.pdf PCB Design Objects
      Trace Data
*/
int do_wires(CCEtoODBDoc *Doc, FileStruct *pcbFile, FILE *fLog, FILE *fWire, int via, int pageUnits)
{
   char *lp;
   char tok[80];
   char att[255];
   CString cur_netname;
   CString xrflayer[256];

   page_unit = pageUnits;
   ifp = fWire;
   ferr = fLog;

   doc = Doc;
   clean_layerstackup(doc, true, !techFileProcessed); 

	int i=0;
   for (i=0; i<255; i++)
      xrflayer[i] = "";

   setFileReadProgress("Processing wires");

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {
      if (strlen(ifp_line) < 2)
         continue;

      if (ifp_line[strlen(ifp_line)-1] == '\n')
         ifp_line[strlen(ifp_line)-1] = '\0';

      if ((lp = get_string(ifp_line, "\n")) == NULL)
         continue;

      if (get_nextword(lp, tok, 80, att, 255) == 0)
         continue;

      if (tok[0] == '#')   
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;

         if (!strcmp(tok, "NET"))
         {
            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;
            cur_netname = get_netname(tok);
            cur_netname.MakeUpper();      // Mentor netname are not case sensitive
            NetStruct *net;
            net = find_net(pcbFile, cur_netname);
            if (net == NULL)
            {
               fprintf(ferr, "Net [%s] found at line %ld in Traces file. Net not in Nets file.\n", cur_netname, ifp_linecnt);
               display_error++;
            }
            // add_net(pcbFile, cur_netname); Do not add nets from sources other than nets file.
         }
      }
      else if (!STRCMPI(tok, "UNITS"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         do_units(tok);
      }
      else if (!STRCMPI(tok, "TET"))   // tear drop template
      {
         // not needed
      }
      
      /* TSR 3644: TTT handling added*/
      else if (!STRCMPI(tok, "TTT"))   // thermal tie template
      {
         tttCnt++;

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         tttArray.SetAtGrow(tttCnt, atoi(tok));

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
      }
      else if (!STRCMPI(tok, "XRF"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;

         int lindex = atoi(tok);

         if (lindex < 0 || lindex > 255)
            continue;
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         xrflayer[lindex] = _strupr(tok);

         int xrflay = Graph_Level(_strupr(tok), "", 0);
         LayerStruct *layer = doc->FindLayer(xrflay);

         if (!STRNICMP(tok, "SIGNAL", 6))
         {
            if (layer->getElectricalStackNumber() == 0) // if not set in PRT file
            {
               layer->setElectricalStackNumber(lindex);
               if (!techFileProcessed)
                  layer->setPhysicalStackNumber(lindex);
               layer->setLayerType(LAYTYPE_SIGNAL);
            }
         }
         else if (!STRNICMP(tok, "POWER", 6))
         {
            if (layer->getElectricalStackNumber() == 0) // if not set in PRT file
            {
               layer->setElectricalStackNumber(lindex);
               if (!techFileProcessed)
                  layer->setPhysicalStackNumber(lindex);
               layer->setLayerType(LAYTYPE_POWERNEG);
            }
         }

      }
      else if (!STRCMPI(tok, "ST"))
      {
         continue;
      }
      else if (!STRCMPI(tok, "GUI"))
      {

      }
      else if (!STRCMPI(tok, "CHG"))
      {
         // change guide ???
      }
      else if (!STRCMPI(tok, "NET"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         cur_netname = get_netname(tok);
      }
      else if (!STRCMPI(tok, "WIR"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int wcnt = atoi(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x2 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y2 = cnv_tok(tok);

         char keyword[80],value[80];
         if((ifp_line = get_nextline(ifp)))
         {
            if ((lp = get_string(ifp_line, "\n")) != NULL)
            {
               if (get_nextword(lp, tok, 80, att, 255) != 0)
               {
                  if (STRCMPI(tok, "PRP"))
                     Push_NextLine = TRUE;
                  else
                  {
                     //Process PRP                     
                     if (get_nextword(NULL, keyword, 80, att, 255) != 0)
                     {
                        if(get_nextword(NULL, value, 80, att, 255) != 0)
                        {  
                           //add the attribute
                        }
                     }
                  }
               }
            }
         }
         get_wire(wcnt, cur_netname, xrflayer, via,keyword,value);
      }
      else if (!STRCMPI(tok, "INC"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int wcnt = atoi(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x2 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y2 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         get_wire(wcnt, cur_netname, xrflayer, via);
      }
      else if (!STRCMPI(tok, "ANT"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int wcnt = atoi(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x2 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y2 = cnv_tok(tok);

         get_wire(wcnt, cur_netname ,xrflayer, via);
      }
      else if (!STRCMPI(tok, "TIE"))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int wcnt = atoi(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y1 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double x2 = cnv_tok(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         double y2 = cnv_tok(tok);

         get_wire(wcnt, cur_netname, xrflayer, via);

      }
      else if ((!STRCMPI(tok, "ARE")) || (!STRCMPI(tok, "ARS")))
      {
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         cur_netname = get_netname(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int xrflay = Graph_Level(tok, "", 0);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int wcnt = atoi(tok);

         DataStruct *data = Graph_PolyStruct(xrflay, 0, 0);
         if (strlen(cur_netname))  // here do netname
         {
            data->setGraphicClass(graphicClassEtch);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), valueTypeString, cur_netname.GetBuffer(0), SA_APPEND, NULL);
         }

         get_vertex(wcnt, TRUE);
         Graph_Poly(NULL, 0, TRUE, FALSE, TRUE); // Filled & Closed

         for (int i=0; i<polycnt; i++)
         {
            MenPoly poly = polyarray.ElementAt(i);  
            Graph_Vertex(poly.x, poly.y, 0.0);
         }

         polyarray.RemoveAll();
         polycnt = 0;
      }
      else if (!STRCMPI(tok, "FIL"))   // area fill/power fill input shape or cutout
      {
// we need to switch here.
// the ARE will be switched to ARS
// ISL will be switched to ISS. 
// I believe the right thing to do is if an ARS follows an ARE, delete the ARE (or do not store it)
// same with is a ISS follows a ISL

         int widthindex = 0;
         int cutoutshape = 0; // 4 rectangle, 8 octagon, 1 polygon, 0 area cutout
         
         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         if (!STRCMPI(tok, "$NONE"))
            cur_netname = "";
         else
            cur_netname = get_netname(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int xrflay = Graph_Level(tok, "", 0);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;   // solid or cutout
         CString fillpattern = tok;

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;
         int wcnt = atoi(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;   // number of actual shapes ARE
         int arecnt = atoi(tok);

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;   // number of islands ISL
         int islcnt = atoi(tok);
         // more to follow, but not needed.

         if (get_nextword(NULL, tok, 80, att, 255) == 0)
            continue;   // whether island is ON (0) or OFF (1)
         int islandIsOn = atoi(tok);

         int fill = TRUE;
         DataStruct *data = Graph_PolyStruct(xrflay, 0, 0);

         if (!fillpattern.CompareNoCase("CUTOUT"))
         {
            fill = FALSE;
            data->setGraphicClass(GR_CLASS_ROUTKEEPOUT);
            if (copper_cutout == 'V')
                data->setNegative(true);
         }

         if (strlen(cur_netname))  // here do netname
         {
            data->setGraphicClass(graphicClassEtch);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), valueTypeString, cur_netname.GetBuffer(0), SA_APPEND, NULL);
         }
         
         // here get PRP. This seems to follow FIL, but is not counted anywhere.
         while ((ifp_line = get_nextline(ifp)))
         {
            if ((lp = get_string(ifp_line, "\n")) == NULL)
               break;
            if (get_nextword(lp, tok, 80, att, 255) == 0)
               break;  
            if (!STRCMPI(tok, "PRP"))
               continue;

            Push_NextLine = TRUE;
            break;
         }

         get_vertex(wcnt, TRUE);

/* do not store the boundary - like in PADS in.*/
         if (FIL_only == TRUE )
         {
            BOOL f = FALSE,v = FALSE;
            if (fill == FALSE)
            {
               if (copper_cutout == 'N')
                  f = TRUE;
               if (copper_cutout == 'V')
               {
                  f = TRUE;
                  v = TRUE;
               }
            }
            CPoly *poly = Graph_Poly(NULL, 0, f, v, TRUE); // Filled & maybe Closed???
            poly->setFloodBoundary(true);

            for (int i=0; i<polycnt; i++)
            {
               MenPoly poly = polyarray.ElementAt(i);  
               Graph_Vertex(poly.x, poly.y, 0);
            }
         }
         else if (arecnt == 0)   // if no areacnt done, use the "area boundary"
         {
            BOOL f = fill,v = FALSE;
            if (fill == FALSE)
            {
               if (copper_cutout == 'N')
                  f = TRUE;
               if (copper_cutout == 'O')
                  f = FALSE;
               if (copper_cutout == 'V')
               {
                  f = TRUE;
                  v = TRUE;
               }
            }
            CPoly *poly = Graph_Poly(NULL, 0, f, v, TRUE); 

            for (int i=0; i<polycnt; i++)
            {
               MenPoly poly = polyarray.ElementAt(i);  
               Graph_Vertex(poly.x, poly.y, 0);
            }
         }

         polycnt = 0;
			int j=0;

         for (j=0; j<arecnt; j++)
         {
            if (!(ifp_line = get_nextline(ifp)))
            {
               // error
               CString tmp;
               tmp.Format("ARE or ARS expected at %ld", ifp_linecnt);
               // logical file end error - eof before $end???
               ErrorMessage(tmp, lp, MB_OK | MB_ICONHAND);
            }

            if ((lp = get_string(ifp_line,"\n")) == NULL)
               continue;
            if (get_nextword(lp, tok, 80, att, 255) == 0)
               continue;
            int is_ARE = FALSE;

            if (!STRCMPI(tok, "ARE"))
               is_ARE = TRUE;

            if (!is_ARE && (STRCMPI(tok, "ARS")))
            {
               fprintf(ferr, "ARE or ARS expected -> [%s] found at %ld\n", tok, ifp_linecnt);
               display_error++;
            }

            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;
            cur_netname = tok;

            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;
            xrflay = Graph_Level(tok, "", 0);

            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;
            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;
            wcnt = atoi(tok);

            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;   // aperture size 
            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;   // cutout shape 
            cutoutshape = atoi(tok); // 4 rectangle, 8 octagon, 1 polygon, 0 area cutout
                                     // it seems that 1 makes a ARS to follow
            int thermaltiescnt = 0;

            while (get_nextword(NULL, tok, 80, att, 255))   // optional 
            {
               if (!STRCMPI(tok, "H"))
               {
                  if (get_nextword(NULL, tok, 80, att, 255) == 0)
                     continue;
                  if (get_nextword(NULL, tok, 80, att, 255) == 0)
                     continue;
                  if (get_nextword(NULL, tok, 80, att, 255) == 0)
                     continue;
                  if (get_nextword(NULL, tok, 80, att, 255) == 0)
                     continue;
               }
               else if (!STRCMPI(tok, "T"))
               {
                  if (get_nextword(NULL, tok, 80, att, 255) == 0)
                     continue;
                  thermaltiescnt = atoi(tok);
               }
            }
            // more stuff, but not needed.

            // here is the actual shapes -- if ARS follows, ARS should be used and I should delete ARE
            get_vertex(wcnt, TRUE);

            if (wcnt && FIL_only == FALSE)
            {
               Graph_Poly(NULL, 0, TRUE, FALSE, TRUE); // Filled & Closed
               for (int i=0; i<polycnt; i++)
               {
                  MenPoly poly = polyarray.ElementAt(i);  
                  Graph_Vertex(poly.x, poly.y, 0.0);
               }
            }

            polycnt = 0;

            // here check what is next
            if (!(ifp_line = get_nextline(ifp)))
            {
               break;
            }
            else
            {
               if ((lp = get_string(ifp_line, "\n")) == NULL)
                  continue;
               if (get_nextword(lp, tok, 80, att, 255) == 0)
                  continue;   // ???
               Push_NextLine = TRUE;
            }

            // here could be a ARS ????
            if ((is_ARE) && (!STRCMPI(tok, "ARS")))// polygon cutout  which could be the ars statement
            {
               if (!(ifp_line = get_nextline(ifp)))
               {
                  CString tmp;
                  tmp.Format("ISL expected at %ld", ifp_linecnt);
                  ErrorMessage(tmp, lp, MB_OK | MB_ICONHAND);
               }

               if ((lp = get_string(ifp_line, "\n")) == NULL)
                  continue;
               if (get_nextword(lp, tok, 80, att, 255) == 0)
                  continue;   // ARS
               if (get_nextword(NULL, tok, 80, att, 255) == 0)
                  continue;   // signal
               if (get_nextword(NULL, tok, 80, att, 255) == 0)
                  continue;   // layer
               if (get_nextword(NULL, tok, 80, att, 255) == 0)
                  continue;   // solid
               if (get_nextword(NULL, tok, 80, att, 255) == 0)
                  continue;   // vercount
               int vercnt = atoi(tok);

               get_vertex(vercnt, TRUE);
               polycnt = 0;
            }
            bool flag = false;
            /* TSR 3644: TTI handling added. TODO: F & I in TTI line */

				
				


            for (polycnt=0; polycnt<thermaltiescnt; polycnt++)
            {
               if ((ifp_line = get_nextline(ifp)))
               {
                  if (get_nextword(ifp_line, tok, 80, att, 255) == 0)
                     continue;   

                  if (!STRCMPI(tok, "TTI"))
                  {
                     //x of instance
                     if (get_nextword(NULL, tok, 80, att, 255) == 0)
                        continue;
                     double instanceX = cnv_tok(tok);

                     //y of instance
                     if (get_nextword(NULL, tok, 80, att, 255) == 0)
                        continue;
                     double instanceY = cnv_tok(tok);

                     //width
                     if (get_nextword(NULL, tok, 80, att, 255) == 0)
                        continue;

                     int err;
                     double width = tttArray.ElementAt(atoi(tok));
                     width *= scale_factor;
                     int widthIndex = Graph_Aperture("", T_ROUND, width, 0.0, 0.0, 0.0, 0.0, 0, 0, FALSE, &err);

                     //rot
                     if (get_nextword(NULL, tok, 80, att, 255) == 0)
                        continue;
                     double degRotation = atof(tok);
                     
                     //xLength
                     if (get_nextword(NULL, tok, 80, att, 255) == 0)
                        continue;
                     double xLength = cnv_tok(tok);
                     
                     //yLength
                     if (get_nextword(NULL, tok, 80, att, 255) == 0)
                        continue;
                     double yLength = cnv_tok(tok);
                     
                     //pattern
                     if (get_nextword(NULL, tok, 80, att, 255) == 0)
                        continue;
                     int pattern = atoi(tok);

                     if(!pattern)
                        continue;

                     double cosR = cos(DegToRad(degRotation));
                     double sinR = sin(DegToRad(degRotation));

                     if (FIL_only == TRUE || ignore_Thermal_Tie == TRUE)
                        continue;

                     // Horizontal +ve segment (right)
                     if (pattern == 1 || pattern == 3 || pattern == 5 || pattern == 7 ||   
                        pattern == 9  || pattern == 11 || pattern == 13 || pattern == 15) 
                     {
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE);
                        Graph_Vertex(instanceX, instanceY, 0.0);
                        Graph_Vertex(instanceX + xLength*cosR, instanceY + xLength*sinR, 0.0);
                     }

                     // Horizontal -ve segment (left)
                     if (pattern ==  4 || pattern ==  5 || pattern ==  6 || pattern ==  7 ||   
                         pattern == 12 || pattern == 13 || pattern == 14 || pattern == 15) 
                     {                                                                         
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE);
                        Graph_Vertex(instanceX, instanceY, 0.0);
                        Graph_Vertex(instanceX - xLength*cosR, instanceY - xLength*sinR, 0.0);
                     }

                     // Vertical +ve segment (top)
                     if (pattern ==  2 || pattern ==  3 || pattern ==  6 || pattern ==  7 ||   
                         pattern == 10 || pattern == 11 || pattern == 14 || pattern == 15) 
                     {                       
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE);
                        Graph_Vertex(instanceX, instanceY, 0.0);
                        Graph_Vertex(instanceX - yLength*sinR, instanceY + yLength*cosR, 0.0);
                     }

                     // Vertical -ve segment (bottom)       
                     if (pattern ==  8 || pattern ==  9 || pattern == 10 || pattern == 11 ||    
                         pattern == 12 || pattern == 13 || pattern == 14 || pattern == 15) 
                     {                                                                                              
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE);
                        Graph_Vertex(instanceX, instanceY, 0.0);
                        Graph_Vertex(instanceX + yLength*sinR, instanceY - yLength*cosR, 0.0);

                     }
                  }
                  else if (flag == false)
                  {
                     CString tmp;
                     tmp.Format("TTI expected at %ld", ifp_linecnt);
                     ErrorMessage(tmp,lp, MB_OK | MB_ICONHAND);
							
							fprintf(ferr,"TTI expected at %ld\n", ifp_linecnt);
							display_error++;
							flag = true;   //added for case 1122 so that we display message box once only.
                  }
						else if (flag == true)
						{
							
							CString tmp;
                     display_error++;
							fprintf(ferr,"TTI expected at %ld\n", ifp_linecnt);
							
							//write data to log file instead. case 1122

						}

               }
               
            }

         }
			   
			
					
         for (j=0; j<islcnt; j++)
         {
            if (!(ifp_line = get_nextline(ifp)))
            {
               CString tmp;
               tmp.Format("ISL or ISS expected at %ld", ifp_linecnt);
               ErrorMessage(tmp, lp, MB_OK | MB_ICONHAND);
            }

            if ((lp = get_string(ifp_line, "\n")) == NULL)
               continue;

            if (get_nextword(lp, tok, 80, att, 255) == 0)
               continue;   // ISL

            int is_ISL = FALSE;

            if (!STRCMPI(tok, "ISL"))
               is_ISL = TRUE;

            //if the tok is either ISS or ISL, the message shouldn't appear on the log file.
            if (!is_ISL && STRCMPI(tok, "ISL") && STRCMPI(tok, "ISS"))
            {
               fprintf(ferr, "ISL or ISS expected -> [%s] found at %ld\n", tok, ifp_linecnt);
               display_error++;
            } 

            if (get_nextword(NULL, tok, 80, att, 255) == 0)
               continue;

            wcnt = atoi(tok);          
            
            get_vertex(wcnt, TRUE);   // here is the actual shapes

            if (FIL_only == TRUE || ignore_Islands == TRUE || islandIsOn < 1)
            {
               polycnt = 0;
               continue;
            }

            Graph_Poly(NULL, 0, TRUE, FALSE, TRUE); // Filled & Closed

            for (int i=0; i<polycnt; i++)
            {
               MenPoly poly = polyarray.ElementAt(i);  
               Graph_Vertex(poly.x, poly.y, 0.0);
            }

            polycnt = 0;

            if (!(ifp_line = get_nextline(ifp)))    // here check what is next
            {
               break;
            }
            else
            {
               if ((lp = get_string(ifp_line, "\n")) == NULL)
                  continue;

               if (get_nextword(lp, tok, 80, att, 255) == 0)
                  continue;   // ???

               Push_NextLine = TRUE;
            }

            // now here check is ISS follows, maybe dependend on ARS
            if ((is_ISL) && (!STRCMPI(tok, "ISS"))) // polygon cutout  which could be the ars statement
            {
               if (!(ifp_line = get_nextline(ifp)))
               {
                  // error
                  CString tmp;
                  tmp.Format("ISS expected at %ld", ifp_linecnt);
                  // logical file end error - eof before $end???
                  ErrorMessage(tmp, lp, MB_OK | MB_ICONHAND);
               }

               if ((lp = get_string(ifp_line, "\n")) == NULL)
                  continue;
               if (get_nextword(lp, tok, 80, att, 255) == 0)
                  continue;   // ISS

               if (STRCMPI(tok, "ISS"))
               {
                  fprintf(ferr, "ISS expected -> [%s] found at %ld\n", tok, ifp_linecnt);
                  display_error++;
               }

               if (get_nextword(NULL, tok, 80, att, 255) == 0)
                  continue;
               wcnt = atoi(tok);
               get_vertex(wcnt, TRUE);

               polycnt = 0;
            }

         }
      }
      else
      {
         fprintf(ferr, "Unknown TRACE Section [%s] at %ld\n", tok, ifp_linecnt);
         display_error++;
      }
   }

   //if (via == FALSE)  // this means that the trace file was loaded as part of a neutral file.
   if (TRUE) // do it always
   {
      // the layers in a neutral file have the same sequence as the XRF layers, so do electrical stackup 
      // for 1 == TOP to n == BOTTOM
      // neutral file reader updates the electricalstacknumber.
      LayerStruct *l = NULL;
      int stackcnt = 0;

      for (i=0; i<255; i++)
      {
         if (strlen(xrflayer[i]) == 0)
            continue;
         int lindex = Graph_Level(xrflayer[i],"", 0);

         l = doc->FindLayer(lindex);
         l->setElectricalStackNumber(++stackcnt);
         if (!techFileProcessed)
            l->setPhysicalStackNumber(stackcnt);

         if (stackcnt == 1)
            l->setLayerType(LAYTYPE_SIGNAL_TOP);
         else if (!STRNICMP(l->getName(), "SIGNAL_", 7))
               l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }

      // here make bottom
      if (l)
         l->setLayerType(LAYTYPE_SIGNAL_BOT);
   }

	
   return 1;
}

/******************************************************************************
* do_netlist
*/
static int do_netlist(FileStruct *fl)
{
   setFileReadProgress("Processing netlist");

   if (fl != NULL)
      do_netlist(fl->getNetList());

   return 1;
}

//*****************************************************************************

// This is an external interface for using the netlist reader from this importer.
// This is a class (obviously) but it mostly uses the static global functions and
// variables already present in the reader.
//
// Declaration of class is in SchematicNetlistIn.h.
// It would be nice to put in MentorIn.h, but can't include MentorIn.h in
// SchematicNetlistIn.cpp (or anyplace other than MentorIn.cpp) because it defines
// and populates and references a lot of static data, does not work without some
// of rest of what is in MentorIn.cpp.

CMentorBoardstationNetlistReader::CMentorBoardstationNetlistReader(CString filename)
{
   if ((ifp = fopen(filename, "rb")) == NULL)    // not rt because the is a Ctr Z allowed in text
   {
      ErrorMessage("Can not open input file %s", filename);
   }

   ifp_linecnt = 0;

   ///InitializeMembers();

}

CMentorBoardstationNetlistReader::~CMentorBoardstationNetlistReader()
{
   if (ifp != NULL)
      fclose(ifp);

   ///FreeMembers();
}

void CMentorBoardstationNetlistReader::ReadNetlist(CNetList &netlistToPopulate)
{
   do_netlist(netlistToPopulate);
}


/******************************************************************************
* do_netlist
* This is used by external interface as well as this importer
*/
static int do_netlist(CNetList &netlistRef)
{
   char *lp;
   char tok[80];
   char att[255];
   int res;

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {
      if (strlen(ifp_line) < 2)
         continue;
      if (ifp_line[strlen(ifp_line)-1] == '\n')
         ifp_line[strlen(ifp_line)-1] = '\0';
      CString save_string = ifp_line;

      if ((lp = get_string(ifp_line, "\n")) == NULL)
         continue;
      if ((res = get_nextnetword(lp, tok, 80, att, 255, 0)) == 0)
         continue;
      if (tok[0] == '#')   // remark
         continue;

      if (STRCMPI(tok, "NET"))   // not NET, then skip
         continue;

      if ((res = get_nextnetword(NULL, tok, 80, att, 255, 1)) == 0)
         continue;
      CString nname = get_netname(tok);
      nname.MakeUpper();   // Mentor netname are not case sensitive

      NetStruct *net = netlistRef.addNet(nname);
      while (res = get_nextnetword(NULL, tok, 80, att, 255, 0))
      {
         if (res == 1)
         {
            CString cname = tok;
            cname.MakeUpper();   // Mentor component are not case sensitive

            // comp pin combination was 'J1'-'98' 'J1-98' J1-98 J1-'98'
            if ((res = get_nextnetword(NULL,tok,80,att,255, 0)) == 0)
               continue;   // Netname
            CString pname = tok;
            pname.MakeUpper();

            if (strlen(nname) && strlen(cname) && strlen(pname))
            {
               // if the comp name is in compToBeRemove then don't write to netlist
               CString insertRefname;
               if (compToBeRemove.Lookup(cname, insertRefname))
                  continue;
               net->addCompPin(cname, pname); /// add_comppin(fl, net, cname, pname);
            }
            else
            {
               if (ferr != NULL)
               {
                  fprintf(ferr, "Netlist error Net [%s] Comp [%s] Pin [%s] at %ld\n", nname, cname, pname, ifp_linecnt);
                  fprintf(ferr, " -> %s\n", save_string);
               }
               display_error++;
            }
         }
         else if (net)
         {
            ///doc->SetUnknownAttrib(&net->getAttributesRef(), get_attrmap(tok), att, attributeUpdateOverwrite, NULL);
            net->attributes();  // makes sure attrib ref won't be null, net needs a getDefinedAttributesRef().
            CString kwStr( get_attrmap(tok) );
            if (!kwStr.IsEmpty())
            {
               int kw = netlistRef.getCamCadData().getDefinedAttributeKeywordIndex(kwStr, valueTypeString);
               netlistRef.getCamCadData().setAttribute(*net->getAttributesRef(), kw, att);
            }
         }
      }
   }
   return 1;
}

/******************************************************************************
* do_package

GATE '/I$210' C166 ' ' 0 '1':'1' 1 '2':'2' 1 
GATE '/I$219' IC18 ' ' 0 'GRDA':'1' 0 '3.3V_1':'2' 0 '1Y0':'3' 0 '1Y1':'4' 0  -
       '1Y2':'5' 0 'GRD_1':'6' 0 'GRD_2':'7' 0 '1Y3':'8' 0 '1Y4':'9' 0  -
       '3.3V_2':'10' 0 'G':'11' 0 'FBOUT':'12' 0 'FBIN':'13' 0 '3.3V_3':'14' 0  -
       '1Y5':'15' 0 '1Y6':'16' 0 '1Y7':'17' 0 'GRD_3':'18' 0 'GRD_4':'19' 0  -
       '1Y8':'20' 0 '1Y9':'21' 0 '3.3V_4':'22' 0 'VCCA':'23' 0 'CLK':'24' 0 
GATE '/I$220' R277 ' ' 0 '1':'1' 1 '2':'2' 1 
*/
static int do_package(FileStruct *fl)
{
   char *lp;
   char tok[80];
   char att[255];
   int res;

   setFileReadProgress("Processing packages");

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {
      if (strlen(ifp_line) < 2)
         continue;
      if (ifp_line[strlen(ifp_line)-1] == '\n')
         ifp_line[strlen(ifp_line)-1] = '\0';
      CString save_string = ifp_line;

      if ((lp = get_string(ifp_line, "\n")) == NULL)
         continue;
      if ((res = get_nextnetword(lp, tok, 80, att, 255, 0)) == 0)
         continue;
      if (tok[0] == '#')   // remark
         continue;

      if (!STRCMPI(tok,"GATE"))
      {
         // get_nextnetname allows '-5V'
         if ((res = get_nextnetword(NULL, tok, 80, att, 255, 1)) == 0)
            continue;   // Netname
         CString instname = get_instname(tok); // this is done because i have to flatten instance names
         instname.MakeUpper();      // Mentor inst are not case sensitive

         if ((res = get_nextnetword(NULL, tok, 80, att, 255, 1)) == 0)
            continue;   // Netname
         CString compname = get_netname(tok);
         compname.MakeUpper();      // Mentor comp are not case sensitive

         DataStruct *data = datastruct_from_refdes(doc, fl->getBlock(), compname);
         if (data == NULL) 
         {
            fprintf(ferr, "Error: Can not find Component [%s] in [%s] at %ld\n", compname, "package file", ifp_linecnt);
            display_error++;
         }
         else
         {
            doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_GATEINSTANCENAME, instname, SA_APPEND,NULL); // x, y, rot, heigh
         }
      }
   }
   return 1;
}

/******************************************************************************
* check_create
*/
static void check_create()
{
   WORD refNameKeyword = (WORD)doc->IsKeyWord(ATT_REFNAME, TRUE);

   if (comppincnt)
   {
      for (int i=0; i<comppincnt; i++)
      {
         CompPin *cpin = comppin[i];

         if (cpin->getPadStackName().IsEmpty())
         {
            if (strlen(component_default_padstack) > 0)           
            {
               cpin->setPadStackName(component_default_padstack);

               if ((i + 1) == comppincnt)  // if the pin is last one then set the  component_default_padstack to null
                  component_default_padstack[0] = '\0';
            }
            else
            {
               cpin->setPadStackName(board_default_padstack);
            }
         }

         // do pinnr here
         CString pinnr;
         pinnr.Format("%d", i+1);

         CString p = cpin->getPinName();

         if (p.IsEmpty())
            p = pinnr;

         DataStruct *data = Graph_Block_Reference(cpin->getPadStackName(), p, pcbFileNum, 
            cpin->getX(), cpin->getY(), DegToRad(0.0), 0, 1.0, Graph_Level("0", "", 1), TRUE, blockTypePadstack);

         data->getInsert()->setInsertType(insertTypePin);

         int pnr = atoi(pinnr);
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, TRUE), VT_INTEGER, &pnr, attributeUpdateOverwrite, NULL);

         if (cpin->getAttrib() != NULL)
         {
            // inherit all instances, not just one
            cpin->getAttrib()->setValueFromString(doc->getCamCadData(), refNameKeyword, data->getInsert()->getRefname());

            Attrib* attrib = NULL;

            if (data->getAttributesRef()->Lookup(refNameKeyword, attrib))
            {
               attrib->addInstance(cpin->getAttrib());
            }
            else
            {
               data->getAttributesRef()->SetAt(refNameKeyword, cpin->getAttrib());
            }

            cpin->setAttrib(NULL);
         }

         cpin->setPadStackName("");
         cpin->setPinName("");

         //free(cpin->padstack);
         //cpin->padstack = NULL;
         //free(cpin->pinname);
         //cpin->pinname = NULL;
      }

      comppincnt = 0;
   }

   if (cur_create == CREATE_COMPONENT)
   {
      if (centroid.placed)
      {
         BlockStruct *block = doc->GetCentroidGeometry();
         if (cur_b)
         {
            DataStruct *data = Graph_Block_Reference(block->getBlockNumber(), NULL, centroid.x, centroid.y, 
                  DegToRad(centroid.rotation), 0, 1.0, -1);
            data->getInsert()->setInsertType(insertTypeCentroid);
         }
      }

		CheckAttribOnCompSpecificLayerForGeom();
   }

	if (cur_create == CREATE_DRAWING && curDrawingInsertBoard == true)
	{
		FileStruct *drwfile = Graph_File_Start(cur_b->getName(), Type_Mentor_Layout);
		drwfile->setBlockType(blockTypeDrawing);
		drwfile->setShow(false);
		
		doc->RemoveBlock(drwfile->getBlock());
		drwfile->setBlock(cur_b);
		cur_b->setFileNumber(drwfile->getFileNumber());
		cur_b->setFlags(BL_FILE);
	}

   if (cur_create > CREATE_UNKNOWN)
   {
      Graph_Block_Off();
      CheckTopBottomLayerMappingInserts();

      cur_b = NULL;
      cur_create = CREATE_UNKNOWN;
   }

   // Always reset layerToDataMap & componentSpecificLayerOnMap here just in case it was not reset anywhere else
   layerToDataMap.RemoveAll();
   componentSpecificLayerOnMap.RemoveAll();
   isThruholeBlindBuried = FALSE;
	curDrawingInsertBoard = false;

   polycnt = 0;
   return;
}

/******************************************************************************
* CheckAttribOnCompSpecificLayerForGeom
*/
static void CheckAttribOnCompSpecificLayerForGeom()
{
   if (cur_create == CREATE_COMPONENT && cur_b != NULL)
   {
		// Check the geometry for any attribute that is on layers in componentSpecificLayerOnMap
      if (cur_b->getAttributes())
			CheckAttributeOnComponentSpecificLayer(cur_b->getAttributes());

		POSITION pos = cur_b->getDataList().GetHeadPosition();
		while (pos)
		{
			DataStruct *data = cur_b->getDataList().GetNext(pos);
			if (data == NULL)
				continue;

			if (data->getAttributes())
				CheckAttributeOnComponentSpecificLayer(data->getAttributes());
		}
	}
}

/******************************************************************************
* CheckComponentSpecificLayer
*/
static void CheckAttributeOnComponentSpecificLayer(CAttributes *attributesRef)
{
   if (attributesRef)
   {      
      for (POSITION pos = attributesRef->GetStartPosition();pos != NULL;)
      {
         Attrib* attrib;
         WORD keyword;

         attributesRef->GetNextAssoc(pos, keyword, attrib);

			AttribIterator attribIterator(attrib);
         Attrib tmpAttrib(attrib->getCamCadData());

			while (attribIterator.getNext(tmpAttrib))
			{
				LayerStruct *layer = doc->getLayerAt(tmpAttrib.getLayerIndex());

				CString layerName;
				if (!componentSpecificLayerOnMap.Lookup(layer->getName(), layerName))
					continue;

				CString genericName;
				BOOL isTop = FALSE;
				if (!IsSpecificTopBottomMappingLayer(layerName, genericName, isTop))
					continue;

				// Change all the attribute on the specific to the Top or Bottom layer corresponding to the specific layer
				// layer_1 goes to layer_TOP
				// laeyr_2 goes to layer_BOT

				CString topBottomLayer;
				if (isTop)
					topBottomLayer.Format("%s_TOP", genericName);
				else
					topBottomLayer.Format("%s_BOT", genericName);

				int layerNum = Graph_Level(topBottomLayer, "", FALSE);
				attrib->updateLayerOfInstanceLike(tmpAttrib, layerNum);
			}
      }
   }
}

/******************************************************************************
* CheckTopBottomLayerMappingInserts
*/
static int CheckTopBottomLayerMappingInserts()
{
   CMapPtrToPtr dataToBeRemove;
   dataToBeRemove.RemoveAll();

   if (cur_create == CREATE_BOARD)
      int i = 0;

   // Loop through the layerToDataMap and make sure Top & Bottom layer mapping are done correctly
   POSITION pos = layerToDataMap.GetStartPosition();
   while (pos)
   {
      CString layerName;
      void *voidPtr = NULL;
      layerToDataMap.GetNextAssoc(pos, layerName, voidPtr);
      MENLayerData *layerData = (MENLayerData*)voidPtr;

      CString genericName;
      CString layerName_1;
      CString layerName_2;
      BOOL isTop = FALSE;

      if (IsGenericTopBottomMappingLayer(layerName, layerName_1, layerName_2))
      {  
         // Layer is a generic layer of TOP & BOTTOM mapping layer

         if (isThruholeBlindBuried || cur_create == CREATE_BOARD)
         {
            // The component is a thruhole, blind, buried, or board component
            // so use layer_TOP and layer_Bottom instead of layer_1 and layer_2
            layerName_1.Format("%s_TOP", layerName);
            layerName_2.Format("%s_BOT", layerName);
         }

         // Check for the layerName_1 & layerName_2
         void *voidPtr_1 = NULL;
         void *voidPtr_2 = NULL;
         layerToDataMap.Lookup(layerName_1, voidPtr_1);
         layerToDataMap.Lookup(layerName_2, voidPtr_2);

         if (voidPtr_1)
         {
            // - There are some insert on layerName_1 so check each insert type to if it is already on layerName_1.
            // - If there is an insert type that is not on layerName_1, then add it to the layerName_1
            int layer_1 = Graph_Level(layerName_1, "", FALSE);
            MENLayerData *layerData_1 = (MENLayerData*)voidPtr_1;

            for (int i=0; i<layerData->dataCnt; i++)
            {     
               MENData *mentorData = (MENData*)layerData->dataArray[i];
               if (!mentorData->getData())
                  continue;

               // Check to see if the same data type is found in layerName_1
               BOOL found = FALSE;
               for (int k=0; k<layerData_1->dataCnt; k++)
               {
                  MENData *mentorData_1 = (MENData*)layerData_1->dataArray[k];
                  if (mentorData->getDataType() ==  mentorData_1->getDataType())
                  {
                     found = TRUE;
                     break;
                  }
               }

               // If not found then add it to layerName_1
               if (!found && mentorData->getDataList())
               {
                  DataStruct *copy = CopyTransposeEntity(mentorData->getData(), 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
                  mentorData->getDataList()->AddTail(copy);
                  copy->setLayerIndex(layer_1);

                  dataToBeRemove.SetAt(mentorData->getData(), mentorData->getDataList());
               }                 
            }
         }
         else if (!voidPtr_1)
         {
            // There is no insert on layerName_1 so add the inserts on generic layer to this layer
            int layer_1 = Graph_Level(layerName_1, "", FALSE);

            for (int i=0; i<layerData->dataCnt; i++)
            {
               MENData *mentorData = (MENData*)layerData->dataArray[i];
               if (mentorData->getData())
               {
                  DataStruct *copy = CopyTransposeEntity(mentorData->getData(), 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
                  mentorData->getDataList()->AddTail(copy);
                  copy->setLayerIndex(layer_1);

                  dataToBeRemove.SetAt(mentorData->getData(), mentorData->getDataList());
               }  
            }
         }

         if (voidPtr_2)
         {
            // - There are some insert on layerName_2 so check each insert type to if it is already on layerName_2.
            // - If there is an insert type that is not on layerName_2, then add it to the layerName_2
            int layer_2 = Graph_Level(layerName_2, "", FALSE);
            MENLayerData *layerData_2 = (MENLayerData*)voidPtr_2;

            for (int i=0; i<layerData->dataCnt; i++)
            {     
               MENData *mentorData = (MENData*)layerData->dataArray[i];
               if (!mentorData->getData())
                  continue;

               // Check to see if the same data type is found in layerName_2
               BOOL found = FALSE;
               for (int k=0; k<layerData_2->dataCnt; k++)
               {
                  MENData *mentorData_2 = (MENData*)layerData_2->dataArray[k];
                  if (mentorData->getDataType() ==  mentorData_2->getDataType())
                  {
                     found = TRUE;
                     break;
                  }
               }

               // If not found then add it to layerName_2
               if (!found && mentorData->getDataList())
               {
                  DataStruct *copy = CopyTransposeEntity(mentorData->getData(), 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
                  mentorData->getDataList()->AddTail(copy);
                  copy->setLayerIndex(layer_2);

                  dataToBeRemove.SetAt(mentorData->getData(), mentorData->getDataList());
               }  
            }
         }
         else if (!voidPtr_2)
         {
            // There is no insert on layerName_2 so add the inserts on generic layer to this layer
            int layer_2 = Graph_Level(layerName_2, "", FALSE);

            for (int i=0; i<layerData->dataCnt; i++)
            {
               MENData *mentorData = (MENData*)layerData->dataArray[i];
               if (mentorData->getData() && mentorData->getDataList())
               {
                  DataStruct *copy = CopyTransposeEntity(mentorData->getData(), 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
                  mentorData->getDataList()->AddTail(copy);
                  copy->setLayerIndex(layer_2);

                  dataToBeRemove.SetAt(mentorData->getData(), mentorData->getDataList());
               }  
            }
         }
      }
      else if (IsSpecificTopBottomMappingLayer(layerName, genericName, isTop) && (isThruholeBlindBuried || cur_create == CREATE_BOARD))
      {
         // - The layer is specific layer of the Top & Bottome mapping layer set  
         // - The component is a thruhole, blind, buried, or board component
         // so use layer_TOP and layer_Bottom instead of layer_1 and layer_2

         CString topBottomLayer;
         if (isTop)
            topBottomLayer.Format("%s_TOP", genericName);
         else
            topBottomLayer.Format("%s_BOT", genericName);

         // Check for the insert on the layer
         void *voidPtr = NULL;
         layerToDataMap.Lookup(topBottomLayer, voidPtr);

         if (voidPtr)
         {
            // There are already some insert on both specific layers
            // so check each insert on the generic layer to see if it is in the specific layers.
            // If there is an insert that is not on either specific layers, then add it to the specific layer(s)

            int layerNum = Graph_Level(topBottomLayer, "", FALSE);
            MENLayerData *topBottomlayerData = (MENLayerData*)voidPtr;

            for (int i=0; i<layerData->dataCnt; i++)
            {     
               MENData *mentorData = (MENData*)layerData->dataArray[i];
               if (!mentorData->getData())
                  continue;

               // Check to see if the same data type is found in either layer_TOP or layer_BOT
               BOOL found = FALSE;
               for (int k=0; k<topBottomlayerData->dataCnt; k++)
               {
                  MENData *topBotMentorData = (MENData*)topBottomlayerData->dataArray[k];
                  if (mentorData->getDataType() ==  topBotMentorData->getDataType())
                  {
                     found = TRUE;
                     break;
                  }
               }

               // If not found then change it to either layer_TOP or layer_BOT
               if (!found && mentorData->getDataList())
                  mentorData->getData()->setLayerIndex(layerNum);
            }
         }
         else if (!voidPtr)
         {
            // There is no insert on specific layer_1 so change the inserts on generic layer to this layer
            int layerNum = Graph_Level(topBottomLayer, "", FALSE);

            for (int i=0; i<layerData->dataCnt; i++)
            {
               MENData *mentorData = (MENData*)layerData->dataArray[i];
               if (mentorData->getData())
                  mentorData->getData()->setLayerIndex(layerNum);
            }
         }
      }
      else if (explodePowerSignalLayer && (!layerName.CompareNoCase("POWER") || !layerName.CompareNoCase("SIGNAL")))
      {
         // The generic layer is power or signal so copy the inserts on these layers onto all the specific layers
         // of either POWER or SIGNAL

         int count = 0;
         CString genericLayerName;

         if (!layerName.CompareNoCase("POWER"))
         {
            count = GetNumberOfPowerSpecificLayers();
            genericLayerName = "POWER";
         }
         else
         {
            count = GetNumberOfSignalSpecificLayers();
            genericLayerName = "SIGNAL";
         }

         if (count == 0)
            continue;

         for (int i=0; i<layerData->dataCnt; i++)
         {
            MENData *mentorData = (MENData*)layerData->dataArray[i];
            if (!mentorData->getData())
               continue;

            for (int k=1; k<=count; k++)
            {
               // Check for the specific SIGNAL layer
               CString specificLayerName;
               specificLayerName.Format("%s_%d", genericLayerName, k);

               void *voidPtr;
               BOOL hasInsert = FALSE;

               if (layerToDataMap.Lookup(specificLayerName, voidPtr))
               {
                  // There is some insert on this specific layer
                  MENLayerData *layData = (MENLayerData*)voidPtr;

                  // Check to see if the same insert on this layer is in the same block
                  BOOL found = FALSE;
                  for (int k=0; k<layData->dataCnt; k++)
                  {
                     MENData *menData = (MENData*)layData->dataArray[k];
                     if (menData->getDataList() == mentorData->getDataList())
                     {
                        hasInsert = TRUE;
                        break;
                     }
                  }
               }

               if (hasInsert)
                  continue;

               // There is no insert on this specific layer so add teh insert to this layer
               int layerNum = Graph_Level(specificLayerName, "", FALSE);

               DataStruct *copy = CopyTransposeEntity(mentorData->getData(), 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
               mentorData->getDataList()->AddTail(copy);
               copy->setLayerIndex(layerNum);

               dataToBeRemove.SetAt(mentorData->getData(), mentorData->getDataList());
            }
         }
      }
   }

   // - If there are insert in layers specified in componentSpecificLayerOnMap
   // then change all the insert on the specific layer_1 to layer_TOP and layer_2 to layer_BOT
   pos = componentSpecificLayerOnMap.GetStartPosition();
   while (pos)
   {
      CString layerName;
      componentSpecificLayerOnMap.GetNextAssoc(pos, layerName, layerName);

      CString genericName;
      BOOL isTop = FALSE;
      if (!IsSpecificTopBottomMappingLayer(layerName, genericName, isTop))
         continue;

      void *voidPtr = NULL;
      if (!layerToDataMap.Lookup(layerName, voidPtr))
         continue;

      // Change all the insert on the specific to the Top or Bottom layer corresponding to the specific layer
      // layer_1 goes to layer_TOP
      // laeyr_2 goes to layer_BOT

      CString topBottomLayer;
      if (isTop)
         topBottomLayer.Format("%s_TOP", genericName);
      else
         topBottomLayer.Format("%s_BOT", genericName);

      int layerNum = Graph_Level(topBottomLayer, "", FALSE);
      MENLayerData *layerData = (MENLayerData*)voidPtr;

      for (int i=0; i<layerData->dataCnt; i++)
      {
         MENData *mentorData = (MENData*)layerData->dataArray[i];
         if (mentorData->getData())
            mentorData->getData()->setLayerIndex(layerNum);
      }
   }

   // Remove all the unwanted data
   pos = dataToBeRemove.GetStartPosition();
   while (pos)
   {
      void *voidDataPtr = NULL;
      void *voidDataListPtr = NULL;
      dataToBeRemove.GetNextAssoc(pos, voidDataPtr, voidDataListPtr);

      // Remove the data from dataList
      if (voidDataPtr && voidDataListPtr)
      {
         DataStruct *data = (DataStruct*)voidDataPtr;
         CDataList *dataList = (CDataList*)voidDataListPtr;

         RemoveOneEntityFromDataList(doc, dataList, data, NULL);
      }
   }
   dataToBeRemove.RemoveAll();


   // Clean up the layerToDataMap
   pos = layerToDataMap.GetStartPosition();
   while (pos)
   {
      CString layerName;
      void *voidPtr = NULL;
      layerToDataMap.GetNextAssoc(pos, layerName, voidPtr);

      MENLayerData *layerData = (MENLayerData*)voidPtr;

      // Clean up the layerData->dataArray
      for (int i=0; i<layerData->dataCnt; i++)
      {
         MENData *mentorData = (MENData*)layerData->dataArray[i];
         delete mentorData;
      }
      layerData->dataArray.RemoveAll();
      delete layerData;
   }

   isThruholeBlindBuried = FALSE;

   return 1;
}

/******************************************************************************
* AddDataToLayer
*/
static int AddDataToLayer(CString layerName, CDataList *dataList, DataStruct *data, int dataType)
{
   if (data != NULL)
   {
      MENData *mentorData = new MENData(data,dataList,dataType);

      void *voidPtr = NULL;

      if (layerToDataMap.Lookup(layerName, voidPtr))
      {

         MENLayerData *layerData = (MENLayerData*)voidPtr;
         layerData->dataArray.SetAtGrow(layerData->dataCnt++, mentorData);
      }
      else
      {
         MENLayerData *layerData = new MENLayerData;
         layerToDataMap.SetAt(layerName, layerData);
         layerData->layerName = layerName;
         layerData->dataCnt = 0;
         layerData->dataArray.SetSize(20, 20);
         layerData->dataArray.SetAtGrow(layerData->dataCnt++, mentorData);
      }
   }

   return 1;
}

/******************************************************************************
* update_mirrorlayers
*/
static int update_mirrorlayers(int stackupfound)
{
   int   l;
   LayerStruct *lp;

   // update layers.
   // the problem is that here pad_2, signal_2 is solder, but wrongly stacked up.
/*
$$attribute( "ARTWORK_DEFINITION_IDENTIFIER", "", , , , [0, 0]);
$$attribute( "ARTWORK_24_LAYER_DEFINITION", "DAM_1, PASTE_MASK_1, BOARD_OUTLINE");
$$attribute( "ARTWORK_25_LAYER_DEFINITION", "DAM_1, PASTE_MASK_2, BOARD_OUTLINE");
$$attribute( "ARTWORK_01_LAYER_DEFINITION", "SIGNAL_1, PAD_1, DAM_1");
$$attribute( "ARTWORK_02_LAYER_DEFINITION", "PAD_2, SIGNAL_2, DAM_1");
$$attribute( "ARTWORK_03_LAYER_DEFINITION", "SIGNAL_3, DAM_1");
$$attribute( "ARTWORK_04_LAYER_DEFINITION", "SIGNAL_4, DAM_1");
$$attribute( "ARTWORK_05_LAYER_DEFINITION", "SIGNAL_5, DAM_1");
$$attribute( "ARTWORK_06_LAYER_DEFINITION", "SIGNAL_6, DAM_1");
$$attribute( "ARTWORK_07_LAYER_DEFINITION", "SOLDER_MASK_1, DAM_1");
$$attribute( "ARTWORK_08_LAYER_DEFINITION", "SOLDER_MASK_2, DAM_1");
$$attribute( "ARTWORK_09_LAYER_DEFINITION", "SILKSCREEN_PCB_1, DAM_1");
*/

/* here 6 routing layers, but 8 total
$$attribute( "ARTWORK_DEFINITION_IDENTIFIER", "", , , , [0, 0]);
$$attribute( "ARTWORK_01_LAYER_DEFINITION", "panel_outline, pad_1, signal_1");
$$attribute( "ARTWORK_02_LAYER_DEFINITION", "panel_outline, signal_3");
$$attribute( "ARTWORK_04_LAYER_DEFINITION", "panel_outline, signal_4");
$$attribute( "ARTWORK_05_LAYER_DEFINITION", "panel_outline, signal_5");
$$attribute( "ARTWORK_07_LAYER_DEFINITION", "panel_outline, signal_6");
$$attribute( "ARTWORK_08_LAYER_DEFINITION", "panel_outline, pad_2, signal_2");
$$attribute( "ARTWORK_09_LAYER_DEFINITION", "panel_outline, solder_mask_1");
$$attribute( "ARTWORK_10_LAYER_DEFINITION", "panel_outline, solder_mask_2");
$$attribute( "ARTWORK_11_LAYER_DEFINITION", "panel_outline, paste_mask_1");
$$attribute( "ARTWORK_12_LAYER_DEFINITION", "panel_outline, paste_mask_2");
$$attribute( "ARTWORK_13_LAYER_DEFINITION", "panel_outline, silkscreen_1, [clip_silkscreen=0.01]");
$$attribute( "ARTWORK_14_LAYER_DEFINITION", "panel_outline, silkscreen_2, [clip_silkscreen=0.01]");
$$attribute( "ARTWORK_15_LAYER_DEFINITION", "panel_outline, drill");
$$attribute( "ARTWORK_06_LAYER_DEFINITION", "panel_outline, power_3");
$$attribute( "ARTWORK_03_LAYER_DEFINITION", "panel_outline, power_1, power_2");
*/
   if ((number_of_routing_layers && pad2_stackup_layer) && (number_of_routing_layers > pad2_stackup_layer))
   {
      // here adjust these layers.
      for (int j = 0; j< doc->getMaxLayerIndex(); j++)
      {
         lp = doc->getLayerArray()[j];

         if (lp == NULL) // could have been deleted.
            continue; 

         if (lp->getElectricalStackNumber() == pad2_stackup_layer)
         {
            lp->setElectricalStackNumber(number_of_routing_layers);
            lp->setPhysicalStackNumber(number_of_routing_layers);

            if (!STRNICMP(lp->getName(),"POWER_", 6))
            {
               lp->setLayerType(LAYTYPE_POWERNEG);
            }
            else if (!STRNICMP(lp->getName(),"SIGNAL_", 7))
            {
               lp->setLayerType(LAYTYPE_SIGNAL_BOT);
            }
         }
         else if (lp->getElectricalStackNumber() > pad2_stackup_layer)
         {
            lp->setElectricalStackNumber(lp->getElectricalStackNumber()-1);
            lp->setPhysicalStackNumber(lp->getElectricalStackNumber()-1);

            if (!STRNICMP(lp->getName(),"POWER_", 6))
            {
               lp->setLayerType(LAYTYPE_POWERNEG);
            }
            else if (!STRNICMP(lp->getName(),"SIGNAL_", 7))
            {
               lp->setLayerType(LAYTYPE_SIGNAL_INNER);
            }
         }
      }
   }

   l = Graph_Level("REFLOC_1", "", 0);

   if (lp = doc->FindLayer(l))
      lp->setLayerType(LAYTYPE_TOP);

   l = Graph_Level("REFLOC_2", "", 0);

   if (lp = doc->FindLayer(l))
      lp->setLayerType(LAYTYPE_BOTTOM);

   Graph_Level_Mirror("REFLOC_1", "REFLOC_2", "");

	int i=0;
   for (i=0; i<mentorLayersCnt; i++)
   {
      MENLayer *mentorLayer = mentorLayers[i];

      if (!STRCMPI(mentorLayer->name, "SOLDER_MASK_1") || !STRCMPI(mentorLayer->name, "SOLDER_MASK_2"))
      {
         l = Graph_Level("SOLDER_MASK_1", "", 0);

         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_MASK_TOP);

         l = Graph_Level("SOLDER_MASK_2", "", 0);

         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_MASK_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "SOLDER_MASK_TOP") || !STRCMPI(mentorLayer->name, "SOLDER_MASK_BOT"))
      {
         l = Graph_Level("SOLDER_MASK_TOP", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_MASK_TOP);

         l = Graph_Level("SOLDER_MASK_BOT", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_MASK_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "PASTE_MASK_1") || !STRCMPI(mentorLayer->name, "PASTE_MASK_2"))
      {
         l = Graph_Level("PASTE_MASK_1", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PASTE_TOP);

         l = Graph_Level("PASTE_MASK_2", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PASTE_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "PASTE_MASK_TOP") || !STRCMPI(mentorLayer->name, "PASTE_MASK_BOT"))
      {
         l = Graph_Level("PASTE_MASK_TOP", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PASTE_TOP);

         l = Graph_Level("PASTE_MASK_BOT", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PASTE_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "SILKSCREEN_1") || !STRCMPI(mentorLayer->name, "SILKSCREEN_2"))
      {
         l = Graph_Level("SILKSCREEN_1", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_SILK_TOP);

         l = Graph_Level("SILKSCREEN_2", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_SILK_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "SILKSCREEN"))
      {
         l = Graph_Level("SILKSCREEN", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_SILK_TOP);
      }
      else if (!STRCMPI(mentorLayer->name, "SILKSCREEN_TOP") || !STRCMPI(mentorLayer->name, "SILKSCREEN_BOT"))
      {
         l = Graph_Level("SILKSCREEN_TOP", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_SILK_TOP);

         l = Graph_Level("SILKSCREEN_BOT", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_SILK_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "PAD_1") || !STRCMPI(mentorLayer->name, "PAD_2"))
      {
         l = Graph_Level("PAD_1", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PAD_TOP);

         l = Graph_Level("PAD_2", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PAD_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "PAD_TOP") || !STRCMPI(mentorLayer->name, "PAD_BOT"))
      {
         l = Graph_Level("PAD_TOP", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PAD_TOP);

         l = Graph_Level("PAD_BOT", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PAD_BOTTOM);
      }
      else if (!STRCMPI(mentorLayer->name, "PAD"))
      {
         l = Graph_Level("PAD","", 0);

         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PAD_ALL);
      }
      else if (!STRCMPI(mentorLayer->name, "POWER"))
      {
         l = Graph_Level("POWER", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_POWERNEG);
      }
      else if (!STRCMPI(mentorLayer->name, "SIGNAL"))
      {
         l = Graph_Level("SIGNAL", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PAD_ALL);
      }
      else if (!STRCMPI(mentorLayer->name, "SOLDER_MASK"))
      {
         l = Graph_Level("SOLDER_MASK", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_MASK_ALL);
      }
      else if (!strcmp(mentorLayer->name, "PASTE_MASK"))
      {
         l = Graph_Level("PASTE_MASK", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_PASTE_ALL);
      }
      else if (!STRCMPI(mentorLayer->name, "ALL"))
      {
         l = Graph_Level("ALL", "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_ALL);
      }
      else if (!STRCMPI(mentorLayer->name, prim_boardoutline))
      {
         l = Graph_Level(prim_boardoutline, "", 0);
         if (lp = doc->FindLayer(l))
            lp->setLayerType(LAYTYPE_BOARD_OUTLINE);
      }
      else
      {
         CString ws = mentorLayer->name;

         if (STRNICMP(mentorLayer->name, "SIGNAL_", 7) && STRNICMP(mentorLayer->name, "POWER_", 6))
         {
            l = Graph_Level(mentorLayer->name, "", 0);
            lp = doc->FindLayer(l);

            if (ws.Right(2) == "_2" || ws.Right(4) == "_BOT")
               lp->setLayerType(LAYTYPE_BOTTOM);
            else if (ws.Right(2) == "_1" || ws.Right(4) == "_TOP")
               lp->setLayerType(LAYTYPE_TOP);
         } 
         else
         {
            if (!stackupfound)
            {
               // if no stackup was done, need to assign electrical layers to SIGNAL_???
               if (!STRNICMP(mentorLayer->name, "SIGNAL_", 7))
               {
                  l = Graph_Level(mentorLayer->name, "", 0);
                  lp = doc->FindLayer(l);
                  int stacknr = atoi(mentorLayer->name.Mid(7, 1));

                  if (lp->getElectricalStackNumber() == 0 && stacknr)
                  {
                     lp->setElectricalStackNumber(stacknr);
                     lp->setPhysicalStackNumber(stacknr);
                     if (stacknr == 1)
                        lp->setLayerType(LAYTYPE_SIGNAL_TOP);
                     else if (stacknr == number_of_routing_layers)
                        lp->setLayerType(LAYTYPE_SIGNAL_BOT);
                     else
                        lp->setLayerType(LAYTYPE_SIGNAL_INNER);
                  }
               }
            } // no stackup create section found
         }
      }
   }

   // now attributes after mirror layers
   for (int j = 0; j< doc->getMaxLayerIndex(); j++)
   {
      lp = doc->getLayerArray()[j];
      if (lp == NULL)    // could have been deleted.
         continue;

      for (i=0; i<layer_attr_cnt; i++)
      {
         if (lp->getName().CompareNoCase(layer_attr[i].name) == 0)
         {
            Graph_Level(layer_attr[i].name, "", 0);      
            if (layer_attr[i].mirror > -1)
            {
               Graph_Level(layer_attr[layer_attr[i].mirror].name, "", 0);
               Graph_Level_Mirror(layer_attr[i].name, layer_attr[layer_attr[i].mirror].name , "");
            }

            CString genericName;
            BOOL isTop = FALSE;
            if (IsSpecificTopBottomMappingLayer(lp->getName(), genericName, isTop))
            {
               // This specific layer is top/bottom mapping layer so we need to assign the layer type given in the
               //  setting file for the specific layer to the corresponding layer_TOP and layer_BOT

               CString topBottomLayer;
               if (isTop)
                  topBottomLayer.Format("%s_TOP", genericName);
               else
                  topBottomLayer.Format("%s_BOT", genericName);

               int layerNum = Graph_Level(topBottomLayer, "", FALSE);
               LayerStruct *layer = doc->getLayerArray()[layerNum];
               layer->setLayerType(layer_attr[i].attr);
            }

            lp->setLayerType(layer_attr[i].attr);
            break;
         }
      }
   }

   // here test all power layers
   for (i=0; i<powernetcnt; i++)
   {
      CString  lname;
      lname.Format("POWER_%d", i+1);
      lp = doc->FindLayer_by_Name(lname);

      if (lp == NULL)
      {
         fprintf(ferr, "Power Layer [%s] Net [%s] does not have a STACKUP (electrical stackup number) defined!\n", 
            lname, powernetarray[i]->name);
         display_error++;
      }
      else if (lp->getElectricalStackNumber() == 0)
      {
         fprintf(ferr, "Power Layer [%s] Net [%s] does not have a STACKUP (electrical stackup number) defined!\n", 
            lname, powernetarray[i]->name);
         display_error++;
      }
      else
      {
         // assign net to layer
         doc->SetUnknownAttrib(&lp->getAttributesRef(), ATT_NETNAME, powernetarray[i]->name, SA_APPEND,NULL); // x, y, rot, height
      }
   }

   return 1;
}

/******************************************************************************
* RemoveGenericLayers
*/
static int RemoveGenericLayers()
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (!layer)
         continue;

      CString layerName_1;
      CString layerName_2;
      if (IsGenericTopBottomMappingLayer(layer->getName(), layerName_1, layerName_2))
         DeleteLayer(doc, layer, TRUE);
   }

   return 1;
}

/******************************************************************************
* DeleteUnusedPowerSignalLayers
   This function delete all POWER or SIGNAL layers that has no Artwork number
   or Electricalstacknum, and also all inserts that are on those layers
*/
static int DeleteUnusedPowerSignalLayers()
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (!layer)
         continue;

      if (explodePowerSignalLayer && (!layer->getName().CompareNoCase("POWER") || !layer->getName().CompareNoCase("SIGNAL")))
      {
         DeleteLayer(doc, layer, TRUE);
      }
      else if (layer->getName().Find("POWER_") > -1 || layer->getName().Find("SIGNAL_") > -1)
      {
         int index = layer->getName().Find("_");
         CString tmpString = layer->getName().Right(layer->getName().GetLength() - index - 1);

         // If the POWER_ or SIGNAL_ does not ended with a number, then skip
         if (!IsNumber(tmpString))
            continue;

         if (layer->getArtworkStackNumber() != 0 || layer->getElectricalStackNumber() != 0)
            continue;

         DeleteLayer(doc, layer, TRUE);
      }
   }

   return 1;
}

/******************************************************************************
* IsNumber
*/
static BOOL IsNumber(CString value)
{
   for (int i=0; i<value.GetLength(); i++)
   {
      int asciiValue = (int)value[i];
      if (asciiValue < 48 || asciiValue > 57)
         return FALSE;
   }

   return TRUE;
}

/******************************************************************************
* getLayerColors
*/
static void getLayerColors(CString filename)
{
	CStdioFile file;
	
	if (!file.Open(filename, CFile::modeRead))
		return;

	CString line;
	while (file.ReadString(line))
	{
		line.Trim();
		if (line.IsEmpty())
			continue;

		int colorIndex, r, g, b;
		int tokPos = 0;
		CString tok;

		tok = line.Tokenize(" \t,", tokPos);	// color index
		colorIndex = atoi(tok);

		tok = line.Tokenize(" \t,", tokPos);	// =
		if (tok != "=")
			continue;

		tok = line.Tokenize(" \t,", tokPos);	// red
		r = atoi(tok);

		tok = line.Tokenize(" \t,", tokPos);	// green
		g = atoi(tok);

		tok = line.Tokenize(" \t,", tokPos);	// blue
		b = atoi(tok);

		if (colorIndex >= 0)
			mentorLayerColors.SetAtGrow(colorIndex, RGB(r, g, b));
	}

	file.Close();
}

/******************************************************************************
* do_layers
*/
static int do_layers()
{
   GetLayersFromFile();
   SortLayerName();
   CreateLayer();

   return 1;
}

/******************************************************************************
* do_default_layers
*/
static int do_default_layers()
{
   GetDefaultLayers();
   SortLayerName();
   CreateLayer();

   return 1;
}

/******************************************************************************
* GetLayersFromFile
*/
static void GetLayersFromFile()
{
   char *lp;
   char tok[80];
   char att[255];

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {
      if (strlen(ifp_line) < 2)
         continue;
      if (ifp_line[strlen(ifp_line)-1] == '\n')
         ifp_line[strlen(ifp_line)-1] = '\0';
      if ((lp = get_string(ifp_line, "\n")) == NULL)
         continue;

      if (get_nextword(lp, tok, 80, att, 255) == 0)      // layer anme - unique name that identifies the layer
         continue;
      if (tok[0] == '#')   
         continue;
      CString layerName = tok;

      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // stacking no. - unique number in the layer stacking order
         continue;
      int stackNum = atoi(tok);

      // The following parameters are ignored
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // type
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // color - color index of the graphic objects
         continue;
      int colorIndex = atoi(tok);

      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // width - currently unimplemented base on reference manual V8.6_6
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // fill - currently unimplemented base on reference manual V8.6_6
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // pattern - the number of fill pattern for graphic objects
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // transparency - the number of transparency of the fill pattern
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // path - currently unimplemented base on reference manual V8.6_6
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // space - currently unimplemented base on reference manual V8.6_6
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // style - the line drawing style
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // text - currently unimplemented base on reference manual V8.6_6
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // pen - associate a plotter pen to the layer
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // hilite - color of all highlighted objects on the layer
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // protect - color of all protected objects on the layer
         continue;
      if (get_nextword(NULL, tok, 80, att, 255) == 0)    // select - color of selected objects
         continue;

      CString topLayerName;
      if (get_nextword(NULL, tok, 80, att, 255) > 0)     // top - name of the top mapping layer in a user-defined layer set
         topLayerName = tok;

      CString bottomLayerName;
      if (get_nextword(NULL, tok, 80, att, 255) > 0)     // bottom - name of the bottome mapping layer in a user-defined layer set
         bottomLayerName = tok;

		MENLayer *pMentorLayer = SetMentorLayer(layerName, topLayerName, bottomLayerName);
		pMentorLayer->setColorIndex(colorIndex);
   }

   hasLayerFile = TRUE;
   return;
}

/******************************************************************************
* SetMentorLayer
*/
static MENLayer *SetMentorLayer(CString layerName, CString topUserDefLayer, CString bottomUserDefLayer)
{
   MENLayer *mentorLayer = new MENLayer;
   mentorLayers.SetAtGrow(mentorLayersCnt++, mentorLayer);
   mentorLayer->name = layerName;
   mentorLayer->topUserDefLayer = topUserDefLayer;
   mentorLayer->bottomUserDefLayer = bottomUserDefLayer;

	return mentorLayer;
}

/******************************************************************************
* GetDefaultLayers
*/
static void GetDefaultLayers()
{
	SetMentorLayer("BREAKOUT", "", "");
	SetMentorLayer("BREAKOUT_1", "", "");
	SetMentorLayer("BREAKOUT_2", "", "");

	SetMentorLayer("COMPONENT_BODY_OUTLINE", "", "");
	SetMentorLayer("COMPONENT_BODY_OUTLINE_1", "", "");
	SetMentorLayer("COMPONENT_BODY_OUTLINE_2", "", "");

	SetMentorLayer("DRAWING", "", "");
	SetMentorLayer("DRAWING_1", "", "");
	SetMentorLayer("DRAWING_2", "", "");

	SetMentorLayer("GLUE_MASK", "", "");
	SetMentorLayer("GLUE_MASK_1", "", "");
	SetMentorLayer("GLUE_MASK_2", "", "");

	SetMentorLayer("PAD", "", "");
	SetMentorLayer("PAD_1", "", "");
	SetMentorLayer("PAD_2", "", "");

	SetMentorLayer("PASTE_MASK", "", "");
	SetMentorLayer("PASTE_MASK_1", "", "");
	SetMentorLayer("PASTE_MASK_2", "", "");

	SetMentorLayer("PIN_ID", "", "");
	SetMentorLayer("PIN_ID_1", "", "");
	SetMentorLayer("PIN_ID_2", "", "");

	SetMentorLayer("PLACE", "", "");
	SetMentorLayer("PLACE_1", "", "");
	SetMentorLayer("PLACE_2", "", "");

	SetMentorLayer("PROBE", "", "");
	SetMentorLayer("PROBE_1", "", "");
	SetMentorLayer("PROBE_2", "", "");

	SetMentorLayer("PROBE_SYMBOL", "", "");
	SetMentorLayer("PROBE_SYMBOL_1", "", "");
	SetMentorLayer("PROBE_SYMBOL_2", "", "");

	SetMentorLayer("SILKSCREEN", "", "");
	SetMentorLayer("SILKSCREEN_1", "", "");
	SetMentorLayer("SILKSCREEN_2", "", "");

	SetMentorLayer("SOLDER_MASK", "", "");
	SetMentorLayer("SOLDER_MASK_1", "", "");
	SetMentorLayer("SOLDER_MASK_2", "", "");

	SetMentorLayer("THERMAL", "", "");
	SetMentorLayer("THERMAL_1", "", "");
	SetMentorLayer("THERMAL_2", "", "");

	SetMentorLayer("TRIM_PATH", "", "");
	SetMentorLayer("TRIM_PATH_1", "", "");
	SetMentorLayer("TRIM_PATH_2", "", "");
}

/******************************************************************************
* CreateLayer
*/
static void CreateLayer()
{
	CTypedMapStringToPtrContainer<MENLayer*> menAssociationMap(10, false);
	int menLyrKey = doc->RegisterKeyWord("Mentor_Layer", 0, valueTypeString);

	// create layer
   for (int i=0; i<mentorLayersCnt; i++)
   {
      MENLayer *mentorLayer = mentorLayers[i];
      CString layerName_1;
      CString layerName_2;

		int colorIndex = mentorLayer->getColorIndex();
		COLORREF layerColor = -1;
		if (colorIndex >= 0 && colorIndex < mentorLayerColors.GetSize())
			layerColor = mentorLayerColors[colorIndex];

      if (IsGenericTopBottomMappingLayer(mentorLayer->name, layerName_1, layerName_2))
      {
         // Create the generic layer
         int layerNum = Graph_Level(mentorLayer->name, "", FALSE);
			LayerStruct *layer = doc->FindLayer(layerNum);
			if (layerColor != -1)
			{
				layer->setColor(layerColor);
			    layer->setOriginalColor(layerColor);
			}


         // Create the specific layer_1
         layerNum = Graph_Level(layerName_1, "", FALSE);
         layer = doc->FindLayer(layerNum);
			layer->setAttrib(doc->getCamCadData(), menLyrKey, valueTypeString, (void*)layerName_1.GetBuffer(0), attributeUpdateOverwrite, NULL);
         layer->setFlagBits(LY_NEVERMIRROR);
			if (layerColor != -1)
			{
				layer->setColor(layerColor);
			    layer->setOriginalColor(layerColor);
			}

         // Create the specific layer_2
         layerNum = Graph_Level(layerName_2, "", FALSE);
         layer = doc->FindLayer(layerNum);
			layer->setAttrib(doc->getCamCadData(), menLyrKey, valueTypeString, (void*)layerName_2.GetBuffer(0), attributeUpdateOverwrite, NULL);
         layer->setFlagBits(LY_MIRRORONLY);
			if (layerColor != -1)
			{
				layer->setColor(layerColor);
			    layer->setOriginalColor(layerColor);
			}
         

         // Create the Top & Bottom mirroring layers and set them to mirror each others
         CString layerTop = mentorLayer->name + "_TOP";
			layerNum = Graph_Level(layerTop, "", FALSE);
         layer = doc->FindLayer(layerNum);
			layer->setAttrib(doc->getCamCadData(), menLyrKey, valueTypeString, (void*)layerName_1.GetBuffer(0), attributeUpdateOverwrite, NULL);

         CString layerBot = mentorLayer->name + "_BOT";
			layerNum = Graph_Level(layerBot, "", FALSE);
         layer = doc->FindLayer(layerNum);
			layer->setAttrib(doc->getCamCadData(), menLyrKey, valueTypeString, (void*)layerName_2.GetBuffer(0), attributeUpdateOverwrite, NULL);

			Graph_Level_Mirror(layerTop, layerBot, "");


         // Add the layers to array
         MENLayer *mentorLayer = new MENLayer;
         mentorLayers.SetAtGrow(mentorLayersCnt++, mentorLayer);
         mentorLayer->name = layerTop;
         mentorLayer->topUserDefLayer = "";
         mentorLayer->bottomUserDefLayer = "";
			menAssociationMap.SetAt(layerName_1, mentorLayer);

         mentorLayer = new MENLayer;
         mentorLayers.SetAtGrow(mentorLayersCnt++, mentorLayer);
         mentorLayer->name = layerBot;
         mentorLayer->topUserDefLayer = "";
         mentorLayer->bottomUserDefLayer = "";
			menAssociationMap.SetAt(layerName_2, mentorLayer);
      }
      else 
      {
         // Create the layer
         int layerNum = Graph_Level(mentorLayer->name, "", FALSE);
         LayerStruct *layer = doc->FindLayer(layerNum);
			if (layerColor != -1)
			{
				layer->setColor(layerColor);
			    layer->setOriginalColor(layerColor);
	
				MENLayer *assocLayer = NULL;
				if (menAssociationMap.Lookup(mentorLayer->name, assocLayer))
					assocLayer->setColorIndex(colorIndex);
			}
      }
   }

   menAssociationMap.RemoveAll();
   return;
}

/******************************************************************************
* SortLayerName
*/
static void SortLayerName()
{
   for (int i=0; i<mentorLayersCnt-1; i++)
   {
      for (int k=i+1; k<mentorLayersCnt; k++)
      {
         MENLayer *current = mentorLayers[i];
         MENLayer *next = mentorLayers[k];

         if (current->name > next->name)
         {
            mentorLayers[i] = next;
            mentorLayers[k] = current;
         }
      }
   }
}

/******************************************************************************
* IsGenericTopBottomMappingLayer
*/
static BOOL IsGenericTopBottomMappingLayer(CString layerName, CString &layerName_1, CString &layerName_2)
{
   layerName.MakeUpper();
   layerName.Trim();

   if (layerName == "BREAKOUT"      || layerName == "COMPONENT_BODY_OUTLINE"  || layerName == "DRAWING" ||
       layerName == "GLUE_MASK"     || layerName == "PAD"                     || layerName == "PASTE_MASK" ||
       layerName == "PIN_ID"        || layerName == "PLACE"                   || layerName == "PROBE" ||
       layerName == "PROBE_SYMBOL"  || layerName == "SILKSCREEN"              || layerName == "SOLDER_MASK" ||
       layerName == "THERMAL"       || layerName == "TRIM_PATH")
   {
      layerName_1 = layerName + "_1";
      layerName_2 = layerName + "_2";
      return TRUE;
   }
   else
   {
      for (int i=0; i<mentorLayersCnt; i++)
      {
         MENLayer *layer = mentorLayers[i];
         if (!layer->getName().CompareNoCase(layerName))
         {
            if (layer->bottomUserDefLayer != "" && layer->topUserDefLayer != "")
            {
               layerName_1 = layer->topUserDefLayer;
               layerName_2 = layer->bottomUserDefLayer;
               return TRUE;
            }
         }
      }
   }

   layerName_1 = "";
   layerName_2 = "";
   return FALSE;
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

   if (layerName == "BREAKOUT_1"                || layerName == "BREAKOUT_2" ||
       layerName == "COMPONENT_BODY_OUTLINE_1"  || layerName == "COMPONENT_BODY_OUTLINE_2" ||
       layerName == "DRAWING_1"                 || layerName == "DRAWING_2" ||
       layerName == "GLUE_MASK_1"               || layerName == "GLUE_MASK_2" ||
       layerName == "PAD_1"                     || layerName == "PAD_2" ||
       layerName == "PASTE_MASK_1"              || layerName == "PASTE_MASK_2" ||
       layerName == "PIN_ID_1"                  || layerName == "PIN_ID_2" ||
       layerName == "PLACE_1"                   || layerName == "PLACE_2" ||
       layerName == "PROBE_1"                   || layerName == "PROBE_2" ||
       layerName == "PROBE_SYMBOL_1"            || layerName == "PROBE_SYMBOL_2" ||
       layerName == "SILKSCREEN_1"              || layerName == "SILKSCREEN_2" ||
       layerName == "SOLDER_MASK_1"             || layerName == "SOLDER_MASK_2" ||
       layerName == "THERMAL_1"                 || layerName == "THERMAL_2" ||
       layerName == "TRIM_PATH_1"               || layerName == "TRIM_PATH_2")
   {
      int temp = layerName.ReverseFind('_');
      genericName = layerName.Left(temp);

      if (layerName.Right(2) == "_1")
         isTop = TRUE;
      else
         isTop = FALSE;

      return TRUE;
   }
   else
   {
      for (int i=0; i<mentorLayersCnt; i++)
      {
         MENLayer *layer = mentorLayers[i];
         if (!layer->bottomUserDefLayer.CompareNoCase(layerName))
         {
            genericName = layer->getName();
            isTop = FALSE;
            return TRUE;
         }
         else if(!layer->topUserDefLayer.CompareNoCase(layerName))
         {
            genericName = layer->getName();
            isTop = TRUE;
            return TRUE;
         }
      }
   }

   return FALSE;
}


/******************************************************************************
* GetNumberOfSignalSpecificLayers
	- Need to find the highest signal number since they might not be consecutive
*/
static int GetNumberOfSignalSpecificLayers()
{
   int count = 0;
   for (int i=0; i<mentorLayersCnt; i++)
   {
      CString layerName = mentorLayers[i]->getName();
      if (layerName.Find("SIGNAL_", 0) > -1)
		{
			int index = layerName.Find("_");
			CString signalNum = layerName.Right(layerName.GetLength() - (index + 1));

			if (IsNumber(signalNum) && atoi(signalNum) > count)
				count = atoi(signalNum);
		}
   }

   return count;
}

/******************************************************************************
* GetNumberOfPowerSpecificLayers
*/
static int GetNumberOfPowerSpecificLayers()
{
   int count = 0;
   for (int i=0; i<mentorLayersCnt; i++)
   {
      MENLayer *layer = mentorLayers[i];
      if (layer->getName().Find("POWER_", 0) > -1)
         count++;
   }

   return count;
}

/******************************************************************************
* do_geom_ascii
*/
static int do_geom_ascii()
{
   int   stackupfound = FALSE;

   setFileReadProgress("Processing geometries");

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {
      int code;
      char  *lp;

      if (strlen(ifp_line) < 2)
         continue;
      if (ifp_line[0] != '$') 
         continue;

      if ((lp = get_string(ifp_line, "(")) == NULL) 
         continue;

      if (!STRNICMP(lp, "$abort_enable", strlen("$abort")))
         continue;
      if (!STRNICMP(lp, "$$snap", strlen("$$snap")))     
         continue;
      if (!STRNICMP(lp, "$$delete", strlen("$$delete")))
         continue;

      if ((code = is_command(lp,command_lst, SIZ_OF_COMMAND)) < 0)
      {
#ifdef _DEBUG
         fprintf(ferr, "DEBUG: Unknown MENTOR Section [%s] in [%s] at %ld\n", lp, cur_filename, ifp_linecnt);
         display_error++;
#endif
         // set cur_create to nothing. Must be before is_command, because 
         // is command may be some create_
         if (!strncmp(lp, "$$create",8))
           cur_create = CREATE_UNKNOWN;

         // logical file end error - eof before $end???
         prt_skip();     
      }
      else
      {
         (*command_lst[code].function)();
      }
      if (cur_create == CREATE_STACKUP)
         stackupfound = TRUE;
   }

   check_create();
   update_mirrorlayers(stackupfound);
   return 1;
}

/******************************************************************************
* do_technology
*/
static int do_technology()
{
   layer_phys_cnt = 0;

   clean_layerstackup(doc, true, true);

   setFileReadProgress("Processing technologies");

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {
      int code;
      char  *lp;

      if (strlen(ifp_line) < 2)  
         continue;
      if (ifp_line[0] != '$')    
         continue;

      if ((lp = get_string(ifp_line,"(")) == NULL)
         continue;

      if (!STRNICMP(lp,"$abort_enable",strlen("$abort")))
         continue;
      if (!STRNICMP(lp,"$$snap",strlen("$$snap")))       
         continue;
      if (!STRNICMP(lp,"$$delete",strlen("$$delete")))   
         continue;

      if ((code = is_command(lp,technology_lst,SIZ_OF_TECHNOLOGY)) < 0)
      {
#ifdef _DEBUG
         fprintf(ferr,"DEBUG: Unknown MENTOR Section [%s] in [%s] at %ld\n", lp, cur_filename, ifp_linecnt);
         display_error++;
#endif
      }
      else
      {
         (*technology_lst[code].function)();
      }
   }

   // assign layers from the define_physical_layer record
   //for (int i=0;i<layer_phys_cnt;i++)
   //{
   //}

   techFileProcessed = true;

   return 1;
}

/******************************************************************************
* do_testpoints
*/
int do_testpoints()
{
   CTestProbeDataList TestProbeDataList;
   char  att[MAX_ATT], tok[MAX_TOK];
   TTestPoints	t_testpoint;
   int parse_start = false;
   CString tokline; 
   int refIndex = 0;

   setFileReadProgress("Processing testpoints");
   
   // Use default scale
   double pre_scale = scale_factor;
   do_units(mentor_defaultunits);
   double scale = scale_factor;
   scale_factor = pre_scale;

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {
      // start parsing after headline: # X-Loc     Y-Loc     Net Name        Side      Ins/Map   Prev. Via    Fixture,Probe
      if(!parse_start) 
      {
         CString line = ifp_line;
         if(line.Find("Fixture,Probe") > -1)
            parse_start = true;
      }      
      else 
      {
         if (get_nextword(ifp_line,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //X Position
         t_testpoint.x = atof(tok) * scale;

         if (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //Y Position
         t_testpoint.y = atof(tok) * scale;

         if (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //Net name
         t_testpoint.netname = get_netname(tok);
         
         if (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //Side
         tokline= tok;
         t_testpoint.pcbside = (tokline.MakeUpper().Find("BOT") > -1)?testSurfaceBottom:testSurfaceTop;

         if (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //Insert/Map or 
         if (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //name

         if (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //Probe number
         t_testpoint.refname.Format("TP%d",refIndex++);
         
         TestProbeDataList.Add(t_testpoint);
     
      }
   }

   //Build Maps for Via
   TestProbeDataList.BuildViaListMap(doc,currentPCBFile);

   //place access and probe
   CStringArray logmessage;
   TestProbeDataList.MarkViaAndComPinProbeTest(doc,currentPCBFile,&logmessage);

   //out error messages
   for(int i = 0; i < logmessage.GetCount(); i++)
      fprintf(ferr, logmessage.GetAt(i));
   logmessage.RemoveAll();

   return true;
}

//****************************************************************************
int get_testsets(char *neutralfmt, CString &refname, CString &Fixture)
{
   char *startp, *endp;
   int len;

   //get fixture 
   startp = strstr(neutralfmt,"(") + 1;
   endp = strchr(neutralfmt,',');
   if(!endp || !startp)
	   return 0;
   len = endp - startp;
   *endp = '\0';
   Fixture = startp;

   //get refname
   startp = startp + len +1;
   endp = strchr(startp,')');
   if(!endp || !startp)
	   return 0;
   *endp = '\0';
   refname = startp;

   return 1;
}

/******************************************************************************
* do_pins
*/
int do_pins()
{
   char  att[MAX_ATT], tok[MAX_TOK];
   int parse_start = false;

   CCompToPinRefListMap CompToPinList(doc, page_unit);
   CMapStringToInt propertyTypeMap;
 
   setFileReadProgress("Processing pins");

   InitPropertyType(propertyTypeMap);

   // here do a while loop
   while ((ifp_line = get_nextline(ifp)))
   {

      // start parsing after headline: # PIN  Reference-Pin_no  Properties
      if(!parse_start) 
      {
         CString line = ifp_line;
         if(line.Find("Reference-Pin_no") > -1)
            parse_start = true;
      }
      else
      {
         if (get_nextword(ifp_line,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //PIN 

         if (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) == 0)   return 0; //PIN Name and Number         
         CSupString superRefDes = tok;

         CStringArray refDesArray;
         int parmNum = superRefDes.ParseQuote(refDesArray,"-");
         CString compName = (parmNum)?refDesArray.GetAt(0):"";
         CString pinNumber = (parmNum > 1)?refDesArray.GetAt(1):"";

         while (get_nextword(NULL,tok,MAX_TOK,att,MAX_ATT) != 0) //Property
         {
            //Look up Pin Type
            int propertyType = -1;
            propertyTypeMap.Lookup(tok,propertyType);
            
            //Parse Property
            superRefDes = att;
            int NumParm = superRefDes.ParseQuote(refDesArray,",");
            if(NumParm) //remove first and last quotes
            {
               refDesArray[0].TrimLeft('"');
               refDesArray[NumParm - 1].TrimRight('"');
            }

            //handle property
            switch(propertyType)
            {
            case InitPropertyType_PINREF:
               {
                  CPinRef *pinref = NULL;
                  if(!CompToPinList.LookupPinRef(compName,pinNumber,pinref) || !pinref)
                     pinref = new CMentorPinRef(page_unit);

                  CompToPinList.AddPinRefs(compName,pinNumber,pinref,refDesArray);
               }
               break;
            case InitPropertyType_PINTYPE:
               break;

            default: //unknown
               break;
            }/*CompName*/            
         }/*while*/                 

      }/*if*/
   }/*while*/ 

   CStringArray logmessage;

   //update Pin reference to CAMCAD
   CompToPinList.UpdatePinRefAttributeToCAMCAD(currentPCBFile,logmessage);

   //out error messages
   for(int i = 0; i < logmessage.GetCount(); i++)
      fprintf(ferr, logmessage.GetAt(i));
   logmessage.RemoveAll();

   return true;
}

static void InitPropertyType(CMapStringToInt &propertyTypeMap)
{
   propertyTypeMap.SetAt("PINREF",InitPropertyType_PINREF);       
   propertyTypeMap.SetAt("PINTYPE",InitPropertyType_PINTYPE);         
}


//****************************************************************************

static void changeInsertBlockRef(BlockStruct *block, int topBlockNum, int botBlockNum)
{
	if (block)
	{
		POSITION dataPos = block->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = block->getDataList().GetNext(dataPos);
			if (data)
			{	
				InsertStruct *insert = data->getInsert();
				if (insert && (insert->getBlockNumber() == topBlockNum))
				{
					insert->setBlockNumber(botBlockNum);
					// Dean says don't do the following. I'm saving it for a little while
					// just in case the issue turns out different.
					//insert->setPlacedBottom(true);
					//insert->setMirrorFlags(MIRROR_FLIP | MIRROR_LAYERS);
				}
			}
		}
	}
}

//****************************************************************************

static bool blockReferencesBlockNum(BlockStruct *block, int refBlockNum)
{
	// If the block contains an insert that references refBlockNum
	// return true, otherwise return false. Checks this blocks data
	// inserts, does not drill down further, but if turns out such
	// is needed, just make this recursive.

	if (block != NULL)
	{			
		POSITION dataPos = block->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = block->getDataList().GetNext(dataPos);
			if (data)
			{	
				InsertStruct *insert = data->getInsert();
				if (insert && (insert->getBlockNumber() == refBlockNum))
				{
					return true;
				}
			}
		}
	}

	return false;
}

//****************************************************************************

static void updateMirroredInserts()
{
	for (int i=0; i < doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);
		if (block && block->getBlockType() == blockTypePcb)
		{
			POSITION dataPos = block->getDataList().GetHeadPosition();
			while (dataPos)
			{
				DataStruct *data = block->getDataList().GetNext(dataPos);
				if (data && data->getDataType() == dataTypeInsert)
				{	
					InsertStruct *insert = data->getInsert();
					if (insert != NULL && insert->getInsertType() == insertTypePcbComponent && 
						insert->getGraphicMirrored())
					{
						int blockNum = insert->getBlockNumber();
						BlockStruct *insertedBlock = doc->getBlockAt(blockNum);
						if (insertedBlock != NULL)
						{
							CString blockName = insertedBlock->getName();
							if (blockName.Find(bottomSuffix) < 0)
							{
								CString mirroredName = blockName + bottomSuffix;
								BlockStruct *mirroredBlock = Graph_Block_Exists(doc, mirroredName, block->getFileNumber(), blockTypePcbComponent);
								if (mirroredBlock != NULL)
								{
									insert->setBlockNumber(mirroredBlock->getBlockNumber());
								}
							}
						}
					}
				}
			}
		}
	}
}

//****************************************************************************

static void prepareEntitiesForMirrorInsert(int topBlockNum, int botBlockNum)
{
	// Limit loop to blocks existing now, will be adding blocks, don't
	// need to process the new blocks.
	int blockCount = doc->getMaxBlockIndex();
	for (int i=0; i < blockCount; i++)
	{
		BlockStruct *block = doc->getBlockAt(i);
		if (block)
		{
			BlockStruct *copyBlock = NULL;

			CString blockName = block->getName();

			if ((blockName.Find(bottomSuffix) < 0) && blockReferencesBlockNum(block, topBlockNum))
			{
				CString copyName = blockName + bottomSuffix;
				copyBlock = Graph_Block_Exists(doc, copyName, block->getFileNumber(), block->getBlockType());
				if (copyBlock == NULL) 
				{
					copyBlock = Graph_Block_On(GBO_APPEND, copyName, block->getFileNumber(), 0, block->getBlockType());
					Graph_Block_Copy(block, 0, 0, 0, 0, 1, 0, TRUE);
					Graph_Block_Off();
				}

				if (copyBlock)
				{
					changeInsertBlockRef(copyBlock, topBlockNum, botBlockNum);
				}
			}
		}
	}
}

//****************************************************************************

static DataStruct *getBestPadData(BlockStruct *block, int layer)
{
	// We want a pad that matches the layer, so we get the right
	// aperture. But if there isn't one, get any pad.

	if (block)
	{
		POSITION dataPos = block->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = block->getDataList().GetNext(dataPos);
			if (data  && data->getLayerIndex() == layer)
			{	
				return data;
			}
		}
	}

	return block->getDataList().GetHead();
}

/****************************************************************************/
/*
*/
static int  get_bottom_layer_phys_stacknum()
{
   int num = 0;

   for (int i=1;i<=layer_phys_cnt;i++)
   {
      if (layer_phys[i].stacknum > num)
         num = layer_phys[i].stacknum;
   }

   return num;
}

//****************************************************************************

static int process_asymmetric_padstacks()
{
	// If padstack is asymmetric then we have to make special (separate) top side
	// and bottom side padstack entities, to be used, respecitvely, by top and
	// bottom side inserts of this geometry.

	// Process top side rules, get matching bottom rule, check for symmetry
	for (int i = 0; i < padrulecnt; i++)
	{
		CMentorPadRule *rule1 = padrulearray.GetAt(i);
		if (rule1->getTopSide() == 1)
		{
			CMentorPadRule *rule2 = get_padrule(rule1->getPadname(), /*bottom*/ 0);
			if (rule2 != NULL)
			{
				// This test could be improved, it is not really checking for symmetry.
				// It is only checking to see if the same layer is used throughout,
				// which is the situation in case 1417, for which this is being done.
				// If there are other cases of assymetry that need to be trapped, this
				// test will need attention.
				if (rule1->getStartLayer() == rule2->getStartLayer() &&  // same starts
					rule1->getEndLayer() == rule2->getEndLayer() &&       // same ends
					rule1->getStartLayer() == rule1->getEndLayer())       // same start & end
				{
					// For bottom side padstacks...
					// Make two padstacks from one. The padstack for placement on the top side will
					// have only a bottom side pad. The padstack for placement on the bottom will
					// have only a top side pad, which then will mirror (elsewhere) to the bottom
					// by virtue of the bottom side insert of the component.
					// The padstack that already exists is made into the top side padstack.
					// The bottom side (mirrored) padstack is created new.
					// For top side padstacks... the top side stack gets a top pad, the
					// bottom side stack gets a bottom pad, so when mirrored it goes to top.

					CString padstackname = rule1->getPadname();

					BlockStruct *padstackBlock = doc->Find_Block_by_Name(padstackname, -1, blockTypePadstack);
					if (padstackBlock)
					{
						int topPadstackBlockNum = padstackBlock->getBlockNumber();

						CString thisLayerName = "PAD_TOP";
						CString otherLayerName = "PAD_BOT";

                  // case dts0100434438 - Problem is layer name "Bottom" is not reliable data to check.
                  // Bottom layer in case data is named "ph_4btm".
                  // Use max stack number instead, to recognize bottom layer.
						///////if (layer_phys[rule1->getStartLayer()].physname.CompareNoCase("Bottom") == 0)
                  if (layer_phys[rule1->getStartLayer()].stacknum == get_bottom_layer_phys_stacknum())
						{
							thisLayerName = "PAD_BOT";
							otherLayerName = "PAD_TOP";
						}

						// Adjust padstack for top side
						// Make it have only one pad
						int newlayer = Graph_Level(thisLayerName, "", false);
						DataStruct *data1 = getBestPadData(padstackBlock, newlayer);
						if (data1)
						{
							data1->setLayerIndex(newlayer);
							padstackBlock->getDataList().RemoveAll();
							padstackBlock->getDataList().AddHead(data1);

							// Make padstack for bottom side
							CString otherPadstackname = padstackname + bottomSuffix;
							BlockStruct *newPadstackBlock = Graph_Block_On(GBO_APPEND, otherPadstackname, -1, blockTypePadstack);
							Graph_Block_Copy(padstackBlock, 0, 0, 0, 0, 1, 0, TRUE);
							Graph_Block_Off();
							newPadstackBlock->setBlockType(blockTypePadstack);
							DataStruct *data2 = newPadstackBlock->getDataList().GetHead();
							// case dts0100545197 - Problem is The padstack s25c_tp.a should be on the PAD_TOP.
 							// Originally, the original Padstack and new Padstack were on the same layer. Now we
							// let new Padstack is on the mirror layer.
							newlayer = Graph_Level(otherLayerName, "", false);
							data2->setLayerIndex(newlayer);
							int botPadstackBlockNum = newPadstackBlock->getBlockNumber();
							// Dean says don't do the following. I'm saving it for a little while
							// in case he changes his mind when he actually sees the result.
							//data2->getInsert()->setPlacedBottom(true);
							//data2->getInsert()->setMirrorFlags(MIRROR_FLIP | MIRROR_LAYERS);

							// Adjust components that use these pads
							prepareEntitiesForMirrorInsert(topPadstackBlockNum, botPadstackBlockNum);
						}
					}

				}
			}
		}
	}

	return 1;
}

/****************************************************************************/
/*
   this is the first pass thru the PRT file.
*/
static int loop_geom(List *lst, int siz_of_lst)
{
   // here do a while loop
   cur_create = CREATE_UNKNOWN;

   while ((ifp_line = get_nextline(ifp)))
   {
      int code;
      char  *lp;

      if (strlen(ifp_line) < 2)
         continue;
      if (ifp_line[0] != '$') 
         continue;

      if ((lp = get_string(ifp_line,"(")) == NULL) 
         continue;

      // set cur_create to nothing. Must be before is_command, because 
      // is command may be some create_
      if (!strncmp(lp,"$$create",8))
           cur_create = CREATE_UNKNOWN;

      if ((code = is_command(lp, lst, siz_of_lst)) < 0)
      {
        //
         int t = 0;
      }
      else
      {
         (*lst[code].function)();
      }
   }

   cur_create = CREATE_UNKNOWN;
   return 1;
}

/****************************************************************************/
/*
   a polygon can stretch over multiple lines. Read until a end_of_polygon is 
   found or a line starting $$ or // - that would be an error
*/
static int get_koos(int typ)
{
   char     *lp;
   CString	tmp;
   MenPoly  p;
   int      end_of_polygon = FALSE;

   // first get the complete line. I need to check if this line contains the
   // ] end_of_polygon character
   if ((lp = get_string(NULL,"\n")) == NULL)
      return polycnt;

   tmp.Empty();
   tmp.Append(lp);
   if(tmp.Find(']') != -1)
      end_of_polygon = TRUE;

   if ((lp = get_string(tmp.GetBuffer(0),"]")) == NULL)
      return polycnt;

   while (*lp && *lp != '[')
      lp++;

   if (*lp != '[')
   {
      CString  w;
      w.Format("'[' expected at %ld",ifp_linecnt);
      ErrorMessage("Error in coo read", w);
      return 0;
   }


   do
   {
      // the string is like [xxx,xxx] or [ xx , xx ]
      tmp.Empty();
      tmp.Append(lp);

      if(tmp.Find(']') != -1)
         end_of_polygon = TRUE;

      tmp.Replace('[',' ');
      tmp.Replace(',',' ');
      tmp.Replace(']',' ');

      if (lp = get_string(tmp.GetBuffer(0)," "))
      {
         if (!is_number(lp))
         {
            CString  w;
            w.Format("Number  expected - [%s] found at %ld",lp, ifp_linecnt);
            ErrorMessage("Error in coo read", w);
            return 0;
         }
         p.x = cnv_tok(lp);
         lp = get_string(NULL," ");
         if (!is_number(lp))
         {
            CString  w;
            w.Format("Number  expected - [%s] found at %ld",lp, ifp_linecnt);
            ErrorMessage("Error in coo read", w);
            return 0;
         }

         p.y = cnv_tok(lp);
         p.type = typ;
         p.fill = 0;
         polyarray.SetAtGrow(polycnt,p);
         polycnt++;
      }

      while ((lp = get_string(NULL," ")) != NULL)
      {
         if (lp[0] == ')') // this is the end of the polygon
            break;
         if (!is_number(lp))
         {
            CString  w;
            w.Format("Number  expected - [%s] found at %ld",lp, ifp_linecnt);
            ErrorMessage("Error in coo read", w);
            return 0;
         }

         p.x = cnv_tok(lp);
         lp = get_string(NULL," ");
         if (!is_number(lp))
         {
            CString  w;
            w.Format("Number  expected - [%s] found at %ld",lp, ifp_linecnt);
            ErrorMessage("Error in coo read", w);
            return 0;
         }

         p.y = cnv_tok(lp);
         p.type = typ;
         p.fill = 0;
         polyarray.SetAtGrow(polycnt,p);
         polycnt++;
      }
      if (!end_of_polygon)
      {
         if (!get_nextline(tmp,ifp))
         {
            CString  w;
            w.Format("Continuation of coo read expected at %ld",ifp_linecnt);
            ErrorMessage("Error in coo read", w);
            return 0;
         }

         lp = get_string(tmp.GetBuffer(0), "\n");      
         end_of_polygon = FALSE;
      }
   } while (!end_of_polygon);

   return polycnt;
}

/****************************************************************************/
/*
*/
static int prt_skip()
{
   // do nothing
   return 1;
}

/******************************************************************************
* prt_page
*/
static int prt_page()
{
   char *lp;
   char tmp[80];

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   if ((lp = get_string(NULL, "(,"))   == NULL)
      return 0;
   if ((lp = get_string(NULL, ",")) == NULL)
      return 0;
   if ((lp = get_string(NULL, ",")) == NULL) 
      return 0;
   if ((lp = get_string(NULL, ",")) == NULL) 
      return 0; // UNITS

   strcpy(tmp, lp);
   clean_name(tmp);
   if (!STRCMPI(tmp, "@inches"))
   {
      scale_factor = Units_Factor(UNIT_INCHES, page_unit);
   }
   else if (!STRCMPI(tmp,"@mils"))
   {
      scale_factor = Units_Factor(UNIT_MILS, page_unit);
   }
   else if (!STRCMPI(tmp,"@mm"))
   {
      scale_factor = Units_Factor(UNIT_MM, page_unit);
   }
   else if (!STRCMPI(tmp,"@cm"))
   {
      scale_factor = Units_Factor(UNIT_MM, page_unit) * 10;
   }
   else
   {
      // unknown units
      CString t;
      t.Format("Unknown UNITS [%s] in [%s] at %ld ", tmp, cur_filename, ifp_linecnt);
      ErrorMessage(t,"Unit read error", MB_OK | MB_ICONHAND);
   }

   return 1;
}

/******************************************************************************
* update_layer
*/
int update_layer(CString &layerName)
{
	layerName.MakeUpper();
   for (int i=0; i<mentorLayersCnt; i++)
   {
      if (!strcmp(mentorLayers[i]->name, layerName))
		{
         return i;      
		}
   }

   MENLayer *mentorLayer = new MENLayer;
   mentorLayers.SetAtGrow(mentorLayersCnt++, mentorLayer);
   mentorLayer->name = layerName;
   mentorLayer->topUserDefLayer = "";
   mentorLayer->bottomUserDefLayer = "";

   return mentorLayersCnt - 1;
}

/******************************************************************************
* get_comppin
*/
static int  get_comppin(const char *pinname)
{
   if (pinname != NULL)
   {
      for (int i=0;i < comppincnt;i++)
      {
         if (comppin[i]->getPinName() == pinname)
         {
            return i;
         }
      }
   }

   CompPin* cpin = new CompPin(pinname);
   comppin.SetAtGrow(comppincnt++, cpin);

   return comppincnt -1;
}

/****************************************************************************/
/*
*/
static int get_drillindex(const char *pref, double size, int layernum, int unplated)
{
   MEN4Drill   p;

   if (size == 0) return -1;

   for (int i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size && p.unplated == unplated)
      {
         return p.blocknumber;   
      }
   }

   CString name;
   name.Format("$$_%s_%d_$$",pref, drillcnt);

   int widthindex = Graph_Tool(name, 0, size, 0, 0, 0, 0L);
   BlockStruct *widthblock = doc->getWidthBlock(widthindex);
   int blocknum = widthblock != NULL ? widthblock->getBlockNumber() : -1;

   if (unplated && widthblock != NULL)
   {
      doc->SetAttrib(&widthblock->getAttributesRef(),doc->IsKeyWord(DRILL_NONPLATED, TRUE), VT_NONE, NULL, attributeUpdateOverwrite, NULL);
   }

   p.d = size;
   p.unplated = unplated;
   p.blocknumber = blocknum;
   drillarray.SetAtGrow(drillcnt++, p);  

   return p.blocknumber;
}

/******************************************************************************
* electrical_layer
*/
static int electrical_layer(const char *l)
{
   if (!STRNICMP(l, "PAD_", 4))
      return TRUE;
   if (!STRNICMP(l, "SIGNAL_", 7))
      return TRUE;
   if (!STRNICMP(l, "POWER_", 6))
      return TRUE;

   return FALSE;

}

/******************************************************************************
* prt_attribute
*/
static int prt_attribute()
{
   char *lp;
   char layername[80], keyword[80];
   CString cvalue;
   char *value;
   double drillsize;
   int graphic_class = 0, cnt = polycnt;

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   if ((lp = get_string(NULL, "(, \t"))   == NULL) 
      return 0;
   CpyStr(keyword,_strupr(lp),80);

   clean_layer(keyword);
   strcpy(layername, keyword);
   make_otherlayer(layername);

   strcpy(keyword, get_attrmap(keyword));

   lp = get_string(NULL," \t,"); // must have white space
   cvalue.Empty();
   cvalue.Append(lp);
   value = cvalue.GetBuffer(0);
   clean_text(value);

   // under some circumstances, an empty poly end a sequence of initial, term.
   // but example COMPONENT HEIGHT, it does not.
   int empty_poly = TRUE;

   if (strlen(value) == 0)
      empty_poly = TRUE;


   // move the keepout to the correct layers. PAD, SIGNAL, "" and same as attributes are ALL
   if (strstr(keyword, "_KEEPOUT"))
   {
      if (!STRCMPI(keyword, "SIGNAL"))
         strcpy(layername, "ALL");
      else if (!STRCMPI(keyword, "PAD"))
         strcpy(layername, "ALL");
      else if (!STRCMPI(keyword, value))
         strcpy(layername, "ALL");
      else if (strlen(value)) // this says keepout on which layer
         strcpy(layername, _strupr(value));
      else
         strcpy(layername, "ALL");
   }
   else if (strstr(keyword, "_KEEPIN"))
   {
      if (!STRCMPI(keyword, "SIGNAL"))
         strcpy(layername, "ALL");
      else if (!STRCMPI(keyword, "PAD"))
         strcpy(layername, "ALL");
      else if (!STRCMPI(keyword, value))  // if routing_keepout routing_keepout, it is on all
         strcpy(layername, "ALL");
      else if (strlen(value)) // this says keepout on which layer
         strcpy(layername, _strupr(value));
      else
         strcpy(layername, "ALL");
   }

   if (!STRCMPI(keyword, "PLUNGE_POINT"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "BOARD_ROUTING_LAYERS"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "COMPONENT_DEFAULT_PADSTACK"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "BOARD_DEFAULT_PADSTACK"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "DEFAULT_PAD_SIZE"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "COMPONENT_HEIGHT"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "FIXED_COMPONENT_LOCATION"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "COMPONENT_PIN_DEFINITION"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "DRILL_DEFINITION_UNPLATED"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "TERMINAL_SURFACE_DEFINITION"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "TERMINAL_THRUHOLE_DEFINITION"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "TERMINAL_THRUVIA_DEFINITION"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "DRILL_DEFINITION_PLATED") || !STRCMPI(keyword, "DRILL_DEFINITION"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "POWER_NET_NAMES"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "TERMINAL_BLIND_DEFINITION"))
      empty_poly = FALSE;
   if (!STRCMPI(keyword, "TERMINAL_BURIED_VIA_DEFINITION"))
      empty_poly = FALSE;
      
   if (empty_poly)
   {
      // no value 
      if (polycnt)
      {
         update_layer((CString)layername);
         graphic_class = get_graphic_class(keyword);

			if (!STRCMPI(keyword, "COMPONENT_PLACEMENT_OUTLINE"))
			{
				// For attribute "COMPONENT_PLACEMENT_OUTLINE" place it on the layer name give in value

				CString lyrName = value;
				lyrName.Trim();
				if (strlen(lyrName) > 0)
				{
					// When value is not empty, then it is the layer name so use it
					update_layer(lyrName);
					int laynr = Graph_Level(lyrName, "", 0);
					if (cur_create == CREATE_PIN || cur_create == CREATE_VIA )
						write_polypin(lyrName, DATA_ATTRIBUTE, TRUE);
					else
						write_poly(laynr, 0, 0, TRUE, graphic_class, DATA_ATTRIBUTE, TRUE);
				}
				else
				{
					// Assume it is generic layer "PLACE", so insert to "PLACE_1" & "PLACE_2"
					int laynr = Graph_Level("PLACE_1", "", 0);
					if (cur_create == CREATE_PIN || cur_create == CREATE_VIA )
						write_polypin(lyrName, DATA_ATTRIBUTE, FALSE);
					else
						write_poly(laynr, 0, 0, TRUE, graphic_class, DATA_ATTRIBUTE, FALSE);

					laynr = Graph_Level("PLACE_2", "", 0);
					if (cur_create == CREATE_PIN || cur_create == CREATE_VIA )
						write_polypin(lyrName, DATA_ATTRIBUTE, TRUE);
					else
						write_poly(laynr, 0, 0, TRUE, graphic_class, DATA_ATTRIBUTE, TRUE);
				}
			}
			else
			{
				int laynr = Graph_Level(layername, "", 0);

				if (cur_create == CREATE_PIN || cur_create == CREATE_VIA )
					write_polypin(layername, DATA_ATTRIBUTE, TRUE);
				else
				{
					// this should always be closed except some -
					int close = TRUE;
					if (!STRCMPI(keyword, "MILLING_PATH"))
						close = FALSE;

					write_poly(laynr, 0, 0, close, graphic_class, DATA_ATTRIBUTE, TRUE);
				}
			}
      }
   }

   lp = get_string(NULL, ",");   // must have white space

   // here for stackup
   if (cur_create == CREATE_STACKUP &&
       !STRNICMP(keyword, "ARTWORK_", strlen("ARTWORK_")))
   {
      char tmp[256], *lp;
      int stacknumber;
      strcpy(tmp,keyword);
      if ((lp = get_string(tmp, "_")) == NULL)  
         return 0;
      if ((lp = get_string(NULL, "_")) == NULL)
         return 0;
      stacknumber = atoi(lp);
      if (stacknumber == 0)               
         return 0;
      strncpy(tmp,_strupr(value),255);   // layers can be mixed string
      tmp[255] = '\0';

      lp = get_string(tmp, " \n,\"");
      while (lp)
      {
         LayerStruct *l;

         // $$attribute( "ARTWORK_10_LAYER_DEFINITION", "silkscreen_1, FILM_TITLE, [no_thermal], [clip_silkscreen=0.01]");
         // [ layers are instructions, not layers.
         if (lp[0] == '[') 
         {
            lp = get_string(NULL, " \n,\"");
            continue;
         }

         int lindex = Graph_Level(_strupr(lp), "", 0);
         if ((l = doc->FindLayer(lindex)) == NULL)
            return 0;
         l->setArtworkStackNumber(stacknumber);

         if(electrical_layer(lp) && l->getElectricalStackNumber() == 0)
         {
            // artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
            // electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
            //physicalstacknumber;     // physical manufacturing stacking of layers, 
                                       // signaltop / dialectric / signal_1 / dia / power_1 .... / solder
   
            if (strstr(value,"PAD_1"))
            {
               if (!STRCMPI(lp,"PAD_1"))
                  l->setLayerType(LAYTYPE_PAD_TOP);
               else
                  l->setLayerType(LAYTYPE_SIGNAL_TOP);
               component_layer = stacknumber;
               l->setElectricalStackNumber(stacknumber);
               l->setPhysicalStackNumber(stacknumber);
            }
            else if (strstr(value, "PAD_2"))
            {
               if (!STRCMPI(lp, "PAD_2"))
                  l->setLayerType(LAYTYPE_PAD_BOTTOM);
               else
                  l->setLayerType(LAYTYPE_SIGNAL_BOT);
               solder_layer = stacknumber;
               pad2_stackup_layer = stacknumber;
               l->setElectricalStackNumber(stacknumber);
               l->setPhysicalStackNumber(stacknumber);
            }
            else if (!STRNICMP(lp, "SIGNAL_", 7))  // electrical layers are between top and bottom
            {
               l->setLayerType(LAYTYPE_SIGNAL_INNER);
               l->setElectricalStackNumber(stacknumber);
               l->setPhysicalStackNumber(stacknumber);
            }
            else if (!STRNICMP(lp, "POWER_", 6) )  // electrical layers are between top and bottom
            {
               l->setLayerType(LAYTYPE_POWERNEG);
               l->setElectricalStackNumber(stacknumber);
               l->setPhysicalStackNumber(stacknumber);
            }
            else
            {
               // nothing
            }
         } // only electrical layers
         lp = get_string(NULL, " \n,\"");
      }
   }
   else if (!STRCMPI(keyword, "DRILL_DEFINITION_UNPLATED"))
   {  
      // value is toolname
      // @mark
      //if ((lp = get_string(NULL,","))   == NULL) return 0;
      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;
         // [x,y]
         polycnt = 0;
         get_koos(0);
      }

      if (polycnt)
      {
         // here check if I need to defined a drill hole block
         // now place it.
         int drilllayernum = Graph_Level("DRILLHOLE", "", 0); 
         int blocknum = get_drillindex("UNPLATED", cnv_tok(value), drilllayernum, 1);
         if (blocknum >= 0)
         {
            MenPoly  p1 = polyarray.ElementAt(0);
            DataStruct *data = Graph_Block_Reference(blocknum, NULL, p1.x, p1.y, 0.0, 0 , 1.0, 0);
            data->getInsert()->setInsertType(insertTypeDrillHole);
         }
      }
      polycnt = 0;
   }
   else if (!STRCMPI(keyword, "DRILL_DEFINITION_PLATED") || !STRCMPI(keyword, "DRILL_DEFINITION"))
   {  
      // value is toolname
      // @mark
      //if ((lp = get_string(NULL,","))   == NULL) return 0;
      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n",cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;
         // [x,y]
         polycnt = 0;
         get_koos(0);
      }

      if (polycnt)
      {
         // here check if I need to defined a drill hole block
         // now place it.
         int drilllayernum = Graph_Level("DRILLHOLE", "", 0);
         int blocknum = get_drillindex("DRILL", cnv_tok(value), drilllayernum, 0);
         if (blocknum >= 0)
         {
            MenPoly  p1 = polyarray.ElementAt(0);
            DataStruct *data = Graph_Block_Reference(blocknum, NULL, p1.x, p1.y, 0.0, 0 , 1.0, 0);
            data->getInsert()->setInsertType(insertTypeDrillHole);
         }
      }
      polycnt = 0;
   }
   else if (!STRCMPI(keyword, "TERMINAL_DRILL_SIZE"))
   {
      // scale
      if ((lp = get_string(NULL, ",")) == NULL)
         return 0;
      // nothing
      if ((lp = get_string(NULL, ",")) == NULL) 
         return 0;
      // [x,y]
      if ((lp = get_string(NULL, " \t[")) == NULL) 
         return 0;
      drillsize = cnv_tok(lp);
         
      if (drillsize < smallestdrill)
      {
         int output_units_accuracy = GetDecimals(page_unit); 
         fprintf(ferr, "TERMINAL_DRILL_SIZE [%lf] too small at %ld -> changed to %1.*lf.\n", 
            drillsize, ifp_linecnt, output_units_accuracy, smallestdrill);
         display_error++;
         drillsize = smallestdrill;
      }
      
      // need to put it into Graph_Circle
      int drilllayernum = Graph_Level("DRILLHOLE", "",0);
      int blocknum = get_drillindex("DRILL", drillsize, drilllayernum, 0);
      if (blocknum >= 0)
         Graph_Block_Reference(blocknum, NULL, 0.0, 0.0, 0.0, 0, 1.0, drilllayernum);
   }
   else if (!STRCMPI(keyword, "COMPONENT_FIDUCIAL"))
   {  
      // value is pinname
      // @mark
      //if ((lp = get_string(NULL,","))   == NULL) return 0;
      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;
         // [x,y]
         polycnt = 0;
         get_koos(0);
      }
      MenPoly  p1 = polyarray.ElementAt(0);
      
      CString  bname = "$COMPONENT_FIDUCIAL";

      // just make sure, it's there.
      BlockStruct *block = Graph_Block_Exists(doc, bname, pcbFileNum);
      if ( block == NULL)
      {
         int err;
         CString appname = "FIDAPP";
         int layerIndex = Graph_Level("COMPONENT_FIDUCIAL", "", 0);

         block = Mentor_Graph_Block_On(GBO_APPEND, bname, pcbFileNum, 0);
         Graph_Aperture(appname, T_UNDEFINED, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
         Graph_Block_Reference(appname, NULL, pcbFileNum, 0, 0, 0.0, 0, 1.0, layerIndex, TRUE);
         Graph_Block_Off();
         block->setBlockType(BLOCKTYPE_FIDUCIAL);
      }
   
      DataStruct *data = Graph_Block_Reference(block->getBlockNumber(), NULL, p1.x, p1.y, 0.0, 0, 1.0, -1);
      data->getInsert()->setInsertType(insertTypeFiducial);
      polycnt = 0;
   }
   else if (!STRCMPI(keyword, "COMPONENT_INSERT_CENTER"))
   {  
      // value is pinname
      // @mark
      //if ((lp = get_string(NULL,","))   == NULL) return 0;
      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;
         // [x,y]
         polycnt = 0;
         get_koos(0);
      }
      MenPoly  p1 = polyarray.ElementAt(0);
      centroid.placed = TRUE;
      centroid.x = p1.x;
      centroid.y = p1.y;
      polycnt = 0;
   }
   else if (!STRCMPI(keyword, "COMPONENT_PIN_DEFINITION"))
   {  
      // value is pinname
      // @mark
      //if ((lp = get_string(NULL,","))   == NULL) return 0;
      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL)
            return 0;
         // [x,y]
         polycnt = 0;
         get_koos(0);
      }

      int ptr = get_comppin(_strupr(value));
      MenPoly  p1 = polyarray.ElementAt(0);
      comppin[ptr]->setOrigin(p1.x,p1.y);
      polycnt = 0;
   }
	else if (!STRCMPI(keyword, "COMPONENT_PLACEMENT_OUTLINE"))
	{
      // @mark
      if ((lp = get_string(NULL, ",")) == NULL) 
         return 0;

      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;

         polycnt = 0;
         get_koos(0);

         if (polycnt)
         {
				graphic_class = get_graphic_class(keyword);
				
				CString lyrName = value;
				if (strlen(lyrName) > 0)
				{
					// When value is not empty, then it is the layer name so use it
					update_layer(lyrName);
					int laynr = Graph_Level(lyrName, "", 0);
					write_poly(laynr, 0, 0, TRUE, graphic_class, DATA_ATTRIBUTE, TRUE);
				}
				else
				{
					// Assume it is generic layer "PLACE", so insert to "PLACE_1" & "PLACE_2"
					int laynr = Graph_Level("PLACE_1", "", 0);
					write_poly(laynr, 0, 0, TRUE, graphic_class, DATA_ATTRIBUTE, FALSE);

					laynr = Graph_Level("PLACE_2", "", 0);
					write_poly(laynr, 0, 0, TRUE, graphic_class, DATA_ATTRIBUTE, TRUE);
				}
         }
         polycnt = 0;
		}
	}
   else if (!STRCMPI(keyword, "COMPONENT_THERMAL_OUTLINE") ||
       !STRCMPI(keyword, "BOARD_PLACEMENT_OUTLINE") ||
       !STRCMPI(keyword, "BOARD_PLACEMENT_REGION") ||
       !STRCMPI(keyword, "COMPONENT_BODY_OUTLINE") ||
       !STRCMPI(keyword, "BOARD_PLACEMENT_OUTLINE") ||
       !STRCMPI(keyword, "BOARD_ROUTING_OUTLINE") ||
       !STRCMPI(keyword, "ROUTING_KEEPOUT") ||
       !STRCMPI(keyword, "BOARD_PLACEMENT_KEEPOUT") ||
       !STRCMPI(keyword, "VIA_KEEPOUT") || !STRCMPI(keyword, "TESTPOINT_KEEPOUT") ||
       !STRCMPI(keyword, "TRACE_KEEPOUT"))
   {                                                      
      // @mark
      if ((lp = get_string(NULL, ",")) == NULL) 
         return 0;
      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;

			polycnt = 0;
         get_koos(0);
         if (polycnt)
         {
            update_layer((CString)layername);
            graphic_class = get_graphic_class(keyword);

            int laynr = Graph_Level(layername, "", 0);
            int filled = 0;
            if(GR_CLASS_ROUTKEEPOUT == graphic_class)
               filled = 1;
            write_poly(laynr, 0, filled, 1, graphic_class, DATA_ATTRIBUTE, TRUE);
         }
         polycnt = 0;
      }
   }
   else if (!STRCMPI(keyword, "MILLING_PATH"))
   {     
      // @mark
      if ((lp = get_string(NULL, ",")) == NULL) 
         return 0;
      if (strstr(lp, "@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL)
            return 0;
         // [x,y]
         polycnt = 0;
         get_koos(0);
         if (polycnt)
         {
            update_layer((CString)layername);
            graphic_class = get_graphic_class(keyword);

            int laynr = Graph_Level(layername, "", 0);
            write_poly(laynr, 0 ,0, 0, graphic_class, DATA_ATTRIBUTE, TRUE);  // not closed
         }
         polycnt = 0;
      }
   }
   else if (!STRCMPI(keyword, "PLUNGE_POINT"))
   {                                                      
/*
      // @mark
      if ((lp = get_string(NULL,","))  == NULL) return 0;
      if (strstr(lp,"@mark") != NULL)
      {
         // here was an initialize function
         if (polycnt != 1)
         {
            fprintf(ferr,"@mark without a preceeding initial, term in [%s] at %ld\n",cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
      }
      else
      {
         // scale
         if ((lp = get_string(NULL,","))  == NULL) return 0;
         // [x,y]
         polycnt = 0;
         get_koos(0);
         if (polycnt)
         {
            update_layer(layername);
            graphic_class = get_graphic_class(keyword);

            int laynr = Graph_Level(layername,"", 0);
            write_poly(laynr, 0 ,0, 0, graphic_class, TRUE);   // not closed
         }
         polycnt = 0;
      }
*/
#ifdef _DEBUG
      fprintf(ferr, "Debug: PLUNGE_POINT not implemented at %ld\n", ifp_linecnt);
#endif
   }
   else if (!STRCMPI(keyword, "TERMINAL_THRUHOLE_DEFINITION"))
   {
      isThruholeBlindBuried = TRUE;
   }  
   else if (!STRCMPI(keyword, "TERMINAL_THRUVIA_DEFINITION"))
   {
      isThruholeBlindBuried = TRUE;
   }  
   else if (!STRCMPI(keyword, "TERMINAL_BLIND_DEFINITION"))
   {
      isThruholeBlindBuried = TRUE;
      if (cur_b)
         doc->SetAttrib(&cur_b->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE, NULL,attributeUpdateOverwrite, NULL);
   }
   else if (!STRCMPI(keyword, "TERMINAL_BURIED_VIA_DEFINITION"))
   {
      isThruholeBlindBuried = TRUE;
      if (cur_b) // first geometry is loaded and than technology
         doc->SetAttrib(&cur_b->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE, NULL, attributeUpdateOverwrite, NULL); 
   }
   else if (!STRCMPI(keyword, "COMPONENT_SPECIFIC_LAYER_ON"))
   {
      int curPos = 0;
      CString layerList = value;
      CString layerName = layerList.Tokenize(", ", curPos);
      while (layerName != "")
      {
         componentSpecificLayerOnMap.SetAt(layerName, layerName);
         layerName = layerList.Tokenize(", ", curPos);      
      }
   }
   else if (!STRCMPI(keyword, "POWER_NET_NAMES"))
   {
      char  *tmp, *lp;
      tmp = STRDUP(value);
      NetStruct   *n;

      if (cur_create == CREATE_BOARD)
      {
         if (lp = get_string(tmp, ", \t'"))
         {
            do
            {
               if (STRCMPI(lp, "NO_POWER_LAYERS")) // if not equal to NO_POWER_LAYERS than do it.
               {
                  MEN4Powernet *c = new MEN4Powernet;
                  powernetarray.SetAtGrow(powernetcnt++, c);  
                  c->name = get_netname(lp);
                  c->name.MakeUpper();    // Mentor netname are not case sensitive

                  n = add_net(currentPCBFile, c->name); // do this on the corrected c->name not lp.
                  doc->SetUnknownAttrib(&n->getAttributesRef(), ATT_POWERNET, "", attributeUpdateOverwrite, NULL); //  
               }
            }while (lp = get_string(NULL, ", \t'"));
         }
      } 
      free(tmp);
   }  
   else if (!STRCMPI(keyword, "TERMINAL_SURFACE_DEFINITION"))
   {
      if (cur_b)
      {
         doc->SetAttrib(&cur_b->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, attributeUpdateOverwrite, NULL);  
      }
   }
   else if (!STRCMPI(keyword, "COMPONENT_PADSTACK_OVERRIDE"))
   {
      // value is pinname,padstack
      if ((lp = get_string(value, ",")) != NULL)
      {
         CString pinname = _strupr(lp);
         lp = get_string(NULL, " \t\n");

         CString  pin = lp;
         pin.MakeUpper();
         int ptr = get_comppin(pinname.Trim());
         comppin[ptr]->setPadStackName(pin.Trim());
      }
   }
   else if (!STRCMPI(keyword, "COMPONENT_DEFAULT_PADSTACK"))
   {
      strcpy(component_default_padstack,_strupr(value));
   }
   else if (!STRCMPI(keyword, "BOARD_DEFAULT_PADSTACK"))
   {
      strcpy(board_default_padstack,_strupr(value));
   }
   else if (!STRCMPI(keyword, "COMPONENT_LAYOUT_TYPE"))
   {
      if (cur_b && strlen(value))
      {
         if(!STRCMPI(value, "surface"))
            doc->SetAttrib(&cur_b->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
               VT_NONE, NULL, attributeUpdateOverwrite, NULL); //  
         else
            doc->SetAttrib(&cur_b->getAttributesRef(), doc->RegisterKeyWord(keyword, TRUE, valueTypeString),
               valueTypeString, value, attributeUpdateOverwrite, NULL); //  
      }
   }
   else if (!STRCMPI(keyword, "COMPONENT_LAYOUT_SURFACE"))
   {
      if (cur_b && strlen(value))
      {
         doc->SetAttrib(&cur_b->getAttributesRef(), doc->RegisterKeyWord(keyword, TRUE, valueTypeString),
               valueTypeString, _strupr(value), attributeUpdateOverwrite, NULL); //  
      }
   }
   else if (!STRCMPI(keyword, "BOARD_ROUTING_LAYERS"))
   {
      if ((lp = get_string(NULL, ",")) == NULL)
         return 0;
      if ((lp = get_string(NULL, ",")) == NULL) 
         return 0;
      if ((lp = get_string(NULL, "[,"))   == NULL)
         return 0;
      if ((lp = get_string(NULL, "[,"))   == NULL)
         return 0;
      number_of_routing_layers = atoi(lp);
   }
   else if (!STRCMPI(keyword, "COMPONENT_HEIGHT"))
   {
      double height;
      if (strstr(lp, "@mark"))
      {
         if (polycnt != 1)
         {
            // here is an error
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
         MenPoly  p1 = polyarray.ElementAt(0);
         height = p1.x;
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;   
         // nothing
         if ((lp = get_string(NULL, ",")) == NULL)
            return 0;
         // [x,y
         if ((lp = get_string(NULL, " \t[")) == NULL)
            return 0;
      
         height = cnv_tok(lp);
      }

      if (cur_b)
         doc->SetAttrib(&cur_b->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &height, attributeUpdateOverwrite, NULL);
   }
   else if (!STRCMPI(keyword, "COMPONENT_INSERT_ANGLE"))
   {
      double angle;
      if (strstr(lp, "@mark"))
      {
         if (polycnt != 1)
         {
            // here is an error
            fprintf(ferr, "@mark without a preceeding initial, term in [%s] at %ld\n", cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
         MenPoly  p1 = polyarray.ElementAt(0);
         angle = p1.x;
      }
      else
      {
         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;   
         // nothing
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;
         // [x,y
         if ((lp = get_string(NULL, " \t[")) == NULL) 
            return 0;
      
         angle = atof(lp);
      }

      centroid.placed = TRUE;
      centroid.rotation = angle;
   }
   else if (!STRCMPI(keyword, "FIXED_COMPONENT_LOCATION"))
   {
      // $$initial([2,270], , @nosnap );
      // $$terminal([-0.64685,-0.88585] );
      // $$attribute( "FIXED_COMPONENT_LOCATION", "MTG1,mtg000db001", @mark, @scale , "");
      double x = 0, y = 0, rot = 0;
      int mir = 0;

      if (strstr(lp, "@mark")) 
      {
         if (polycnt != 2)
         {
            // here is an error
            fprintf(ferr,"@mark without a preceeding initial, term in [%s] at %ld\n",cur_filename, ifp_linecnt);
            display_error++;
            return 0;
         }
         /*TSR 3657: When the rotation and side of the component are defined on separate lines, 
         we put them in the polyarray and then load them here. The problem was that when we put them in polyarray,
         we multiplied them by scale factor. Before: rot = p1.y;. Now: rot = p1.y/scale_factor;*/ 

         // side and roation: $$initial([2,270], , @nosnap );
         MenPoly  p1 = polyarray.ElementAt(0);  
         mir = (p1.x/scale_factor) > 1; //unconvert: it was converted to a point scale before being saved in polyaray
         rot = p1.y/scale_factor;  //unconvert, it was converted to a point scale before being saved in polyaray
         
         // x, y: $$terminal([-0.64685,-0.88585] );
         p1 = polyarray.ElementAt(1);
         x = p1.x;
         y = p1.y;
      }
      else
      {
         // $$attribute( "FIXED_COMPONENT_LOCATION", "C38,C_50V_X7R_0805", , @scale , , [1, 90, 24.4475, 194.6275]);

         // scale
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;   
         // nothing
         if ((lp = get_string(NULL, ",")) == NULL) 
            return 0;
         // [side,rot,x,y
         if (lp = get_string(NULL, " \t["))
         {
            mir = (atoi(lp) > 1);
            if (lp = get_string(NULL, ",")   )
            {
               rot = atof(lp);
               if (lp = get_string(NULL, ","))
               {
                  x = cnv_tok(lp);
                  if (lp = get_string(NULL, ","))
                  {
                     y = cnv_tok(lp);
                  }
               }
            }
         }
      }

      if (mir)
      {
         rot = 360 - rot;
      }
      while (rot < 0)   rot += 360;
      while (rot >=360) rot -= 360;

      CString  refname, bname;
      refname = get_string(value, ",");
      refname.MakeUpper();
      bname = get_string(NULL, ",");
      bname.MakeUpper();
      BlockStruct * block = Graph_Block_Exists(doc, bname, pcbFileNum);
      if (block)
      {
         if (strlen(bname))
         {
            DataStruct *data = Graph_Block_Reference(block->getBlockNumber(), refname, x, y, DegToRad(1.0 * rot), mir, 1.0, -1);
            data->getInsert()->setInsertType(insertTypePcbComponent);
            doc->SetUnknownAttrib(&data->getAttributesRef(), "FIXED", "", attributeUpdateOverwrite, NULL); 
         }
      }
      else
      {
         fprintf(ferr, "FIXED_COMPONENT_LOCATION %s Geometry [%s] does not exist.\n", refname, bname);
         display_error++;
      }
   }
   else
   {
      // unknown attribute
      if (cur_b->getAttributesRef())   // must be string, because a mentor example is "POINTER", "0.28" and the next "POINTER", ""
         doc->SetUnknownAttrib(&cur_b->getAttributesRef(), keyword, value, attributeUpdateOverwrite, NULL);
   }
   
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int brd_check_attribute()
{
   char     *lp;
   char     keyword[80];

   if ((lp = get_string(NULL,"(, \t")) == NULL) 
      return 0;
   CpyStr(keyword,lp,80);
   clean_name(keyword);

	// Only update_layer if it is "SIGNAL_" or "POWER_" something
	// so that the layer is there for the second read of the file
   CString value = get_string(NULL, "(, \t");
   clean_name(value.GetBuffer(0));
	value.MakeUpper();
	if (value.Find("SIGNAL_", 0) > -1 || value.Find("POWER_", 0) > -1)
		update_layer(value);

   if (cur_create == CREATE_PIN)
   {
      if (!STRCMPI(keyword,"TERMINAL_SURFACE_DEFINITION"))
      {
         padlistarray[padlistcnt-1]->smd = TRUE;
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int prt_pin()
{
   char     pinname[80];
   char     padstack[80];
   char     *lp;

   if (cur_create == CREATE_UNKNOWN)   
      return 0;

   if ((lp = get_string(NULL,"\", \t"))   == NULL) 
      return 0;   // pinname
   strcpy(pinname, _strupr(lp));
   clean_name(pinname);
   if ((lp = get_string(NULL,"\", \t"))   == NULL) 
      return 0;   // padstack
   strcpy(padstack, _strupr(lp));
   clean_name(padstack);
   if ((lp = get_string(NULL,","))  == NULL)
      return 0;   // ??

   polycnt = 0;
   get_koos(0);
   int ptr = get_comppin(pinname);
   MenPoly  p1 = polyarray.ElementAt(0);
   comppin[ptr]->setOrigin(p1.x,p1.y);
   comppin[ptr]->setPadStackName(padstack);

   return 1;
}

/******************************************************************************
* prt_create_pin
*/
static int prt_create_pin()
{
   char  *lp;

   check_create();

   if ((lp = get_string(NULL, "(\")")) == NULL)
      return 0;
   strcpy(cur_name, _strupr(lp)); 

   cur_b = Mentor_Graph_Block_On(GBO_APPEND, cur_name, pcbFileNum, 0, blockTypePadstack);

	// Case 1788, sometimes these things are fiducials
   int pt = get_part(cur_name);

	if (pt > -1)
   {
      switch (partarray[pt]->parttype)
      {
         case PART_FIDUCIALTOP:
         case PART_FIDUCIALBOT:
         case PART_FIDUCIAL:
            cur_b->setBlockType(BLOCKTYPE_FIDUCIAL);
         break;
         default:
				break;
      }
   }
	else
	{
		// Standard behavior, before case 1788
      cur_b->setBlockType(BLOCKTYPE_PADSTACK);
	}

   cur_create = CREATE_PIN;
   cur_padlistptr = get_padlistptr(cur_name);

   return 1;
}


/******************************************************************************
* prt_create_panel
*/
static int prt_create_panel()
{
   char  *lp;
   check_create();

   if ((lp = get_string(NULL,"(\")")) == NULL)  
      return 0;
   strcpy(cur_name,_strupr(lp));
   cur_create = CREATE_PANEL;
 
   // find block
   cur_b = Graph_Block_Exists(doc, cur_name, -1);
	cur_b->setBlockType(BLOCKTYPE_PANEL);

   // graph entities to this file
   FileStruct *file = doc->Find_File(cur_b->getFileNumber());
   doc->PrepareAddEntity(file);

   return 1;
}


/****************************************************************************/
/*
*/
static int prt_create_drawing()
{
   char  *lp;

   check_create();

   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   strcpy(cur_name,_strupr(lp));
   cur_create = CREATE_DRAWING;

   // find block
	cur_b = Mentor_Graph_Block_On(GBO_APPEND, cur_name, pcbFileNum, 0L, blockTypeDrawing);

	return 1;
}


/****************************************************************************/
/*
   PANTHEON has more than 1 create_board in a file.
*/
static int prt_create_board()
{
   char *lp;

   check_create();

   // no need to get name, got it in brd_create_board
   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   strcpy(cur_name,_strupr(lp));

   if (STRCMPI(cur_name, cur_boardname))
   {
      currentPCBFile = Graph_File_Start(cur_name, Type_Mentor_Layout);
      currentPCBFile->setBlockType(blockTypePcb);
      currentPCBFile->getBlock()->setBlockType(blockTypePcb);
   }

   cur_create = CREATE_BOARD;
   cur_b = Mentor_Graph_Block_On(GBO_APPEND, cur_name, pcbFileNum, 0);
   cur_b->setBlockType(BLOCKTYPE_PCB);

   // graph entities to this file
   FileStruct *file = doc->Find_File(cur_b->getFileNumber());
   doc->PrepareAddEntity(file);
   return 1;
}

/****************************************************************************/
/*
*/
static int brd_create_board()
{
   char  *lp;
   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   strcpy(cur_boardname, _strupr(lp));
   return 1;
}

/****************************************************************************/
/*
*/
static int find_create_board()
{
   char  *lp;
   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   strcpy(cur_boardname,_strupr(lp));
   return 1;
}

/****************************************************************************/
/*
*/
static int update_part(const char *p, int typ)
{

   for (int i=0;i<partcnt;i++)
   {
      if (partarray[i]->name.CompareNoCase(p) == 0)
      {
         if (typ > 0)
            partarray[i]->parttype = typ;
         return i;
      }
   }

   MEN4Part *c = new MEN4Part;
   partarray.SetAtGrow(partcnt,c);  
   partcnt++;
   c->name = p;
   c->parttype = typ;

   return partcnt-1;
}

/****************************************************************************/
/*
   here allow wildcard * and ? 
*/
static int get_part(const char *p)
{

   for (int i=0;i<partcnt;i++)
   {
      if (wildcard_compare(p,partarray[i]->name, 1) == 0)
      {
         return i;
      }
   }

   return -1;
}

/******************************************************************************
* brd_check_smdpin
*/
static int brd_check_smdpin()
{
   char     *lp;
   CString  padname;

   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   padname = lp;
   padname.MakeUpper();

   if (get_padlistptr(lp) > -1)
   {
      fprintf(ferr,"Upper/Lower Case matching padname [%s]\n", lp);
      display_error++;
      padname = "UPPER_";
      padname += _strupr(lp);
   }

   MEN4Padlist *c = new MEN4Padlist;
   padlistarray.SetAtGrow(padlistcnt++, c);  
   c->name = padname;
   c->smd = FALSE;
   c->via = FALSE;
   c->layer_elem_cnt;
   c->complex_cnt;

   cur_create = CREATE_PIN;

   return 1;
}

/****************************************************************************/
/*
*/
static int brd_check_path()
{
   char  *lp;
   char  lay[80];

   if ((lp = get_string(NULL," ,\"")) == NULL) 
      return 0;
   strcpy(lay, _strupr(lp));
   clean_layer(lay);

   int lindex = update_layer((CString)lay);
   if (lindex < 0)
      return 1;

   if (cur_create == CREATE_PIN)
   {
      //if ((lp = get_string(NULL," ,\"")) == NULL) 
      //   return 0;
      //strcpy(lay, _strupr(lp));
      //clean_layer(lay);

      //int lindex = update_layer((CString)lay);
      //if (lindex < 0)
      //   return 1;

      MEN4Padlist *padlist = padlistarray[padlistcnt-1];
      CString countStr;
      CString layer;
      layer.Format("%d", lindex);

      int count = 0;
      if (padlist->layer_elem_cnt.Lookup(layer, countStr))
         count = atoi(countStr);

      countStr.Format("%d", ++count);
      padlist->layer_elem_cnt.SetAt(layer, countStr);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int brd_check_circle()
{
   char  *lp;
   char  lay[80];

   if ((lp = get_string(NULL," ,\"")) == NULL) 
      return 0;
   strcpy(lay, _strupr(lp));
   clean_layer(lay);
   int lindex = update_layer((CString)lay);
   if (lindex < 0)
      return 1;

   if (cur_create == CREATE_PIN)
   {
      //if ((lp = get_string(NULL," ,\"")) == NULL) 
      //   return 0;
      //strcpy(lay, _strupr(lp));
      //clean_layer(lay);
      //int lindex = update_layer((CString)lay);
      //if (lindex < 0)
      //   return 1;

      MEN4Padlist *padlist = padlistarray[padlistcnt-1];
      CString countStr;
      CString layer;
      layer.Format("%d", lindex);

      int count = 0;
      if (padlist->layer_elem_cnt.Lookup(layer, countStr))
         count = atoi(countStr);

      countStr.Format("%d", ++count);
      padlist->layer_elem_cnt.SetAt(layer, countStr);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int brd_check_polygon()
{
   char  *lp;
   char  lay[80];

   if ((lp = get_string(NULL," ,\"")) == NULL)  
      return 0;
   strcpy(lay, _strupr(lp));
   clean_layer(lay);
   int lindex = update_layer((CString)lay);
   if (lindex < 0)
      return 1;

   if (cur_create == CREATE_PIN)
   {
      //if ((lp = get_string(NULL," ,\"")) == NULL)  
      //   return 0;
      //strcpy(lay, _strupr(lp));
      //clean_layer(lay);
      //int lindex = update_layer((CString)lay);
      //if (lindex < 0)
      //   return 1;

      MEN4Padlist *padlist = padlistarray[padlistcnt-1];
      CString countStr;
      CString layer;
      layer.Format("%d", lindex);

      int count = 0;
      if (padlist->layer_elem_cnt.Lookup(layer, countStr))
         count = atoi(countStr);

      countStr.Format("%d", ++count);
      padlist->layer_elem_cnt.SetAt(layer, countStr);
   }

   return 1;
}

/****************************************************************************/
/*
*/   
static int brd_create_panel()
{
   char  *lp;
   FileStruct *panelfile = NULL;

   if ((lp = get_string(NULL, "(\")")) == NULL)
      return 0;
   strcpy(cur_name,_strupr(lp));

   panelfile = Graph_File_Start(cur_name, Type_Mentor_Layout);
   panelfile->setBlockType(blockTypePanel);
   panelfile->getBlock()->setBlockType(currentPCBFile->getBlockType());

   panelfile->setShow(false); 
   return 1;
}

/****************************************************************************/
/*
*/
static int brd_create_drawing()
{
   char  *lp;
   if ((lp = get_string(NULL, "(\")")) == NULL)
      return 0;
   strcpy(cur_name,_strupr(lp));

	BlockStruct *block = Mentor_Graph_Block_On(GBO_APPEND, cur_name, pcbFileNum, 0L, blockTypeDrawing);
	Graph_Block_Off();

   //FileStruct *drwfile = NULL;
   //drwfile = Graph_File_Start(cur_name, Type_Mentor_Layout);
   //drwfile->blocktype  = BLOCKTYPE_DRAWING;
   //drwfile->show = FALSE;
   cur_b = NULL;

   return 1;
}

/****************************************************************************/
/*
   always generate to make sure that I know the block type, when an add is done.
*/  
static int brd_create_generic_part()
{
   char  *lp;
   CString  u;

   if ((lp = get_string(NULL, "(\")")) == NULL)
      return 0;
   u = lp;
   u.MakeUpper();
   cur_b = Mentor_Graph_Block_On(GBO_APPEND, u, pcbFileNum, 0);
   cur_b->setBlockType(BLOCKTYPE_MECHCOMPONENT);

   // this part can have pins.
   int pt = get_part(u);

   if (pt > -1)
   {
      switch (partarray[pt]->parttype)
      {
         case PART_TOOL:
            cur_b->setBlockType(BLOCKTYPE_TOOLING);
         break;
         case PART_FIDUCIALTOP:
         case PART_FIDUCIALBOT:
         case PART_FIDUCIAL:
            cur_b->setBlockType(BLOCKTYPE_FIDUCIAL);
         break;
         default:
            cur_b->setBlockType(BLOCKTYPE_MECHCOMPONENT);
         break;
      }
   }

   Graph_Block_Off();
   cur_b = NULL;

   return 1;
}

BlockTypeTag getBlockTypeByPartType(int partType)
{
   switch (partType)
   {
   case PART_TOOL:
      return blockTypeTooling;
   case PART_FIDUCIALTOP:
   case PART_FIDUCIALBOT:
   case PART_FIDUCIAL:
      return blockTypeFiducial;
   }

   return blockTypePcbComponent;
}

/****************************************************************************/
/*
   always generate to make sure that I know the block type, when an add is done.
*/
static int brd_create_component_part()
{
   char *lp;
   CString  u;

   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   u = lp;
   u.MakeUpper();
   // this part can have pins.
   int pt = get_part(u);

   BlockTypeTag blockType = (pt < 0)?blockTypePcbComponent:getBlockTypeByPartType(partarray[pt]->parttype);
   cur_b = Mentor_Graph_Block_On(GBO_APPEND, u, pcbFileNum, 0, blockType);

   if (pt > -1)
   {
      switch (partarray[pt]->parttype)
      {
         case PART_TOOL:
            cur_b->setBlockType(BLOCKTYPE_TOOLING);
         break;
         case PART_FIDUCIALTOP:
         case PART_FIDUCIALBOT:
         case PART_FIDUCIAL:
            cur_b->setBlockType(BLOCKTYPE_FIDUCIAL);
         break;
         default:
            cur_b->setBlockType(BLOCKTYPE_PCBCOMPONENT);
         break;
      }
   }

   Graph_Block_Off();
   cur_b = NULL;

   return 1;
}

/****************************************************************************/
/*
*/
static int prt_create_stackup()
{
   char  *lp;

   check_create();

   if ((lp = get_string(NULL, "(\")")) == NULL)
      return 0;
   strcpy(cur_name,_strupr(lp));
   cur_b = Mentor_Graph_Block_On(GBO_APPEND, cur_name, currentPCBFile->getFileNumber(),0);
   cur_create = CREATE_STACKUP;

   return 1;
}

/****************************************************************************/
/*
*/
static int prt_create_generic_part()
{
   char  *lp;

   check_create();

   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   strcpy(cur_name,_strupr(lp));

   // here is -1 to find the first name
   cur_b = Mentor_Graph_Block_On(GBO_APPEND, cur_name, pcbFileNum, 0);
   cur_b->setBlockType(BLOCKTYPE_GENERICCOMPONENT);

   // this part can have pins.
   int pt = get_part(cur_name);

   if (pt > -1)
   {
      switch (partarray[pt]->parttype)
      {
         case PART_TOOL:
            cur_b->setBlockType(BLOCKTYPE_TOOLING);
         break;
         case PART_FIDUCIALTOP:
         case PART_FIDUCIALBOT:
         case PART_FIDUCIAL:
            cur_b->setBlockType(BLOCKTYPE_FIDUCIAL);
         break;
         default:
            cur_b->setBlockType(BLOCKTYPE_MECHCOMPONENT);
         break;
      }
   }

   cur_create = CREATE_GENERIC_PART;

   return 1;
}


/****************************************************************************/
/*
*/
static int prt_create_via()
{
   char  *lp;

   check_create();

   if ((lp = get_string(NULL, "(\")")) == NULL)
      return 0;
   strcpy(cur_name,_strupr(lp));
   cur_b = Mentor_Graph_Block_On(GBO_APPEND, cur_name, pcbFileNum, 0, blockTypePadstack);
   cur_create = CREATE_VIA;
   cur_padlistptr = get_padlistptr(cur_name);
   padlistarray[cur_padlistptr]->via = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
static int prt_create_component()
{
   char  *lp;
   CString  t;

   check_create();
   comppincnt = 0;
   if ((lp = get_string(NULL,"(\")")) == NULL)
      return 0;
   t = lp;
   t.MakeUpper();
   // this part can have pins.
   int pt = get_part(t);
   BlockTypeTag blockType = (pt < 0)?blockTypePcbComponent:getBlockTypeByPartType(partarray[pt]->parttype);
   cur_b = Mentor_Graph_Block_On(GBO_APPEND, t, pcbFileNum, 0, blockType);

   if (pt > -1)
   {
      switch (partarray[pt]->parttype)
      {
         case PART_TOOL:
            cur_b->setBlockType(BLOCKTYPE_TOOLING);
         break;
         case PART_FIDUCIALTOP:
         case PART_FIDUCIALBOT:
         case PART_FIDUCIAL:
            cur_b->setBlockType(BLOCKTYPE_FIDUCIAL);
         break;
         default:
            cur_b->setBlockType(BLOCKTYPE_PCBCOMPONENT);
            int lay = Graph_Level("REFLOC_1", "", 0);
            doc->SetUnknownAttrib(&cur_b->getAttributesRef(), ATT_REFNAME, "", attributeUpdateOverwrite, NULL);
         break;
      }
   }

   strcpy(cur_name,t);
   cur_create = CREATE_COMPONENT;
   centroid.placed= FALSE;
   centroid.x = centroid.y = centroid.rotation = 0.0;

   return 1;
}

/****************************************************************************/
/*
*/
static int get_padformindex(int f,double sizeA, double sizeB, double offsetx, double offsety)
{
   MenPadform* p;
   double   rotation = 0;

   for (int i=0;i < padFormArray.GetSize();i++)
   {
      p = padFormArray.GetAt(i);

      if (p != NULL &&
          p->form == f &&
          fabs(p->getSizeA()   - sizeA  ) < SMALLNUMBER &&
          fabs(p->getSizeB()   - sizeB  ) < SMALLNUMBER &&
          fabs(p->offsetx - offsetx) < SMALLNUMBER &&
          fabs(p->offsety - offsety) < SMALLNUMBER)
      {
         return i;   
      }
   }

   CString  name;

   while (true)
   {
      name.Format("PADSHAPE_%d",padFormArray.GetSize());

      if (Graph_Block_Exists(doc, name, pcbFileNum))
      {
         padFormArray.Add(NULL);
      }
      else
      {
         break;
      }
   }

   int   err;
   Graph_Aperture(name, f, sizeA, sizeB, offsetx, offsety, DegToRad(rotation), 0, BL_APERTURE, TRUE, &err);

   p = new MenPadform;

   p->form    = f;
   p->setSizeA(sizeA);
   p->setSizeB(sizeB);
   p->offsetx = offsetx;
   p->offsety = offsety;

   padFormArray.Add(p);  
   //padformcnt++;

   return padFormArray.GetSize() - 1;
}

/******************************************************************************
* write_polypin
*/
static int write_polypin(const char *llay, int dataType, BOOL clearPolyArray)
{
   int smd = FALSE;
   if (cur_padlistptr > -1)
      smd = padlistarray[cur_padlistptr]->smd;

   char lay[80];
   strcpy(lay, llay);
   make_padlayer(lay, smd);
   int laynr = Graph_Level(lay, "", 0);

   // here now assign it to an aperture and link that aperture to that shape.
   if (!is_complex_attached(laynr))
   {
		CString pshapename;
		pshapename.Format("%s %s", cur_name, lay);
		BlockStruct *subblock = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
		subblock->setBlockType(BLOCKTYPE_PADSHAPE);
		write_poly(Graph_Level("0", "", TRUE), 0, 0, 1, 0, dataType, clearPolyArray, TRUE);
		Graph_Block_Off();

      CString pxshapename;
      pxshapename.Format("PADSHAPE_%s", pshapename);
      Graph_Complex(pxshapename, 0, subblock, 0.0, 0.0, 0.0);
      DataStruct *data = Graph_Block_Reference(pxshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

      CString layer;
      layer.Format("%d", laynr);

      MEN4Padlist *padlist = padlistarray[cur_padlistptr];
      padlist->complex_cnt.SetAt(layer, "1");

      // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
      CDataList *dataList = GetCurrentDataList();
      AddDataToLayer(lay, dataList, data, dataType);
   }
	else
	{
		CString pshapename;
		pshapename.Format("%s %s", cur_name, lay);
		BlockStruct *block = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
		block->setBlockType(BLOCKTYPE_PADSHAPE);
		write_poly(laynr, 0, 0, 1, 0, dataType, clearPolyArray);
		Graph_Block_Off();
	}

   return 1;
}

/******************************************************************************
* make_otherlayer
*/
static void make_otherlayer(char *lay)
{
   //if (cur_create == CREATE_PIN || cur_create == CREATE_VIA)
   // return;

   //if (!STRCMPI(lay, "SILKSCREEN"))  
   //{
   // strcpy(lay, "SILKSCREEN_1");
   //}
   //else if (!STRCMPI(lay, "ASSEMBLY"))
   //{
   // strcpy(lay, "ASSEMBLY_1");
   //}
   //else if (!STRCMPI(lay, "COMPONENT_PLACEMENT_OUTLINE"))
   //{
   // strcpy(lay, "COMPONENT_PLACEMENT_OUTLINE_1");
   //}
   //else if (!STRCMPI(lay, "COMPONENT_BODY_OUTLINE"))
   //{
   // strcpy(lay, "COMPONENT_BODY_OUTLINE_1");
   //}

   //if (cur_create == CREATE_BOARD || cur_create == CREATE_PANEL)
   // return;

   return;
}

/******************************************************************************
* make_padlayer
*/
static void make_padlayer(char *lay, int smd)
{

   //if (!strcmp(lay, "PAD"))
   //{
   // if (smd)
   // {
   //    strcpy(lay, "PAD_1");
   //    return;
   // }
   //}
   //else if (!strcmp(lay, "SOLDER_MASK"))
   //{
   // if (smd)
   // {
   //    strcpy(lay, "SOLDER_MASK_1");
   //    return;
   // }
   //}
   //else if (!strcmp(lay, "PASTE_MASK"))
   //{
   // if (smd)
   // {
   //    strcpy(lay, "PASTE_MASK_1");
   //    return;
   // }
   //}

   //if (smd)  // on SMD pins the layers are MIRROR-ONLY and NEVER-MIRROR
   //{
   // if (!STRCMPI(lay, "PASTE_MASK_1"))  strcpy(lay, "PASTE_MASK_TOP");
   // if (!STRCMPI(lay, "PASTE_MASK_2"))  strcpy(lay, "PASTE_MASK_BOT");
   // if (!STRCMPI(lay, "SOLDER_MASK_1")) strcpy(lay, "SOLDER_MASK_TOP");
   // if (!STRCMPI(lay, "SOLDER_MASK_2")) strcpy(lay, "SOLDER_MASK_BOT");
   // if (!STRCMPI(lay, "PAD_1"))         strcpy(lay, "PAD_TOP");
   // if (!STRCMPI(lay, "PAD_2"))         strcpy(lay, "PAD_BOT");
   //}

   return;
}

/****************************************************************************/
/*
*/
static int prt_circle()
{
   double   x1,y1,r;
   char     *lp;
   char     lay[80];
   int      laynr, orig_laynr;
   double   w = template_path_width;

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   if ((lp = get_string(NULL,",")) == NULL) 
      return 0;
   strcpy(lay, _strupr(lp));
   clean_layer(lay);

   make_otherlayer(lay);

   orig_laynr = update_layer((CString)lay);

   if ((lp = get_string(NULL,",")) == NULL) 
      return 0;
   x1 = cnv_tok(lp);
   if ((lp = get_string(NULL,",")) == NULL) 
      return 0;
   y1 = cnv_tok(lp);
   if ((lp = get_string(NULL,",")) == NULL)  
      return 0;
   r = cnv_tok(lp);

   // Pantheon 
   // $$circle("SIGNAL", 0.0, 0.0, 0.02, 0.0, "", @radius) ;
   if ((lp = get_string(NULL,",)")) != NULL)  
   {
      w = cnv_tok(lp);
      // ???
      if ((lp = get_string(NULL," \t,)")) != NULL)  
      {
         // ???
         if ((lp = get_string(NULL," \t,)")) != NULL)  
         {
            if (!STRCMPI(lp,"@radius"))
               r = r * 2;  // convert radius to diameter
         }
      }
   }

   if (cur_create == CREATE_PIN || cur_create == CREATE_VIA )
   {
      CString  pshapename;

      int      smd = FALSE;
      if (cur_padlistptr > -1)
      {
         smd = padlistarray[cur_padlistptr]->smd;
      }

      make_padlayer(lay, smd);
      update_layer((CString)lay);
      laynr = Graph_Level(lay, "", 0);

      if (!is_pad_complex(orig_laynr))
      {
         int pindex = get_padformindex(T_ROUND,r,0,x1,y1);
         pshapename.Format("PADSHAPE_%d",pindex);
         DataStruct *data = Graph_Block_Reference(pshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

         // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
         CDataList *dataList = GetCurrentDataList();
         AddDataToLayer(lay, dataList, data, DATA_CIRCLE);
      }
      else
      {
         pshapename.Format("%s %s",cur_name,lay);
         BlockStruct *geomblock = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
         geomblock->setBlockType(BLOCKTYPE_PADSHAPE);
         Graph_Circle(Graph_Level("0", "", 1), x1, y1, r/2, 0L, 0, FALSE, TRUE);
         Graph_Block_Off();

         // here now assign it to an aperture and link that aperture to that shape.
         if (!is_complex_attached(laynr))
         {
            CString  pxshapename;
            pxshapename.Format("PADSHAPE_%s", pshapename);
            Graph_Complex(pxshapename, 0, geomblock, 0.0, 0.0, 0.0);
            DataStruct *data = Graph_Block_Reference(pxshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

            CString layer;
            layer.Format("%d", laynr);

            MEN4Padlist *padlist = padlistarray[cur_padlistptr];
            padlist->complex_cnt.SetAt(layer, "1");

            // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
            CDataList *dataList = GetCurrentDataList();
            AddDataToLayer(lay, dataList, data, DATA_CIRCLE);
         }
      }
   }
   else
   {
      int   widthindex, err;

      make_otherlayer(lay);
      update_layer((CString)lay);
      laynr = Graph_Level(lay, "", 0);

      if (w == 0)
         widthindex = 0;   // make it small width.
      else
         widthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

      DataStruct *data = Graph_Circle(laynr, x1, y1, r/2, 0L, widthindex, FALSE, FALSE); 

      // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
      CDataList *dataList = GetCurrentDataList();
      AddDataToLayer(lay, dataList, data, DATA_CIRCLE);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int prt_arc()
{
   char     *lp;
   char     lay[80];
   int      laynr;

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   if ((lp = get_string(NULL,",")) == NULL)
      return 0;
   strcpy(lay,get_layerrename(_strupr(lp)));
   clean_layer(lay);

   if (cur_create == CREATE_PIN || cur_create == CREATE_VIA)
   {
      CString  pshapename;

      int      smd = FALSE;

      if (cur_padlistptr > -1)
      {
         smd = padlistarray[cur_padlistptr]->smd;
      }
      make_padlayer(lay, smd);
      update_layer((CString)lay);
      laynr = Graph_Level((CString)lay, "", 0);

      if (!is_complex_attached(laynr))
      {
			pshapename.Format("%s %s", cur_name, lay);
			BlockStruct *subblock = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
			subblock->setBlockType(BLOCKTYPE_PADSHAPE);

			// This will be a complex so insert them on floating layer
			do_arc("0", FALSE);

			Graph_Block_Off();

			CString  pxshapename;
         pxshapename.Format("PADSHAPE_%s", pshapename);
         Graph_Complex(pxshapename, 0, subblock, 0.0, 0.0, 0.0);
         DataStruct *data = Graph_Block_Reference(pxshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

         CString layer;
         layer.Format("%d", laynr);

         MEN4Padlist *padlist = padlistarray[cur_padlistptr];
         padlist->complex_cnt.SetAt(layer, "1");

         // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
         CDataList *dataList = GetCurrentDataList();
         AddDataToLayer(lay, dataList, data, DATA_ARC);
      }
		else
		{
			pshapename.Format("%s %s", cur_name, lay);
			BlockStruct *b = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
			b->setBlockType(BLOCKTYPE_PADSHAPE);
			do_arc(lay);
			Graph_Block_Off();
		}
   }
   else
   {
      make_otherlayer(lay);
      update_layer((CString)lay);
      do_arc(lay);
   }
   return 1;
}


/****************************************************************************/
/*

$$text( "BOARD_OUTLINE", "151-0800-00 REV A", 0.675, -4.4, 0.1, @TC, 
0, 1.00, 0.01, "leroy", "None", 0.0, 0.0 );

*/
static int prt_text()
{
   double   x,y;
   double   h,w;
   double   r = 0;
   char     *lp;
   char     lay[80];
   int      laynr, mirror = 0;
   CString	cvalue;
   char     *value = NULL;
   DbFlag   just;
   CString  original_lay;

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   if ((lp = get_string(NULL,",")) == NULL) 
      return 0;

   int refNameKeyword = doc->IsKeyWord(ATT_REFNAME, TRUE);

   strcpy(lay, _strupr(lp));
   clean_layer(lay);
   original_lay = lay;
   make_otherlayer(lay);

   update_layer((CString)lay);

   lp = get_string(NULL," \t,"); // must have white space and must have a quote. Empty text is ""
   cvalue.Empty();
   cvalue.Append(lp);
   value = cvalue.GetBuffer(0);
   clean_text(value);

   if ((lp = get_string(NULL,",")) == NULL)  // x
      return 0;   

   x = cnv_tok(lp);

   if ((lp = get_string(NULL,",")) == NULL)  // y     
      return 0;   

   y = cnv_tok(lp);

   if ((lp = get_string(NULL,",")) == NULL) // height
      return 0;   

   h = cnv_tok(lp);

   if ((lp = get_string(NULL,",")) == NULL) // justification
      return 0;   

   just = get_textjust(lp);

   if ((lp = get_string(NULL,",")) == NULL) // rotation
      return 0; 

   r = atof(lp);

   if ((lp = get_string(NULL,",")) == NULL) // aspect ratio
      return 0;    

   w = h * atof(lp) * 0.8; // Adjust space automatically

   if ((lp = get_string(NULL,",")) != NULL)              // ?? 
   {
   }

   if ((lp = get_string(NULL,",")) != NULL)              // "std"
   {
   }

   if ((lp = get_string(NULL,",")) != NULL)              // "none" 
   {
   }

   if ((lp = get_string(NULL,",)")) != NULL)             // x ?
   {
   }

   if ((lp = get_string(NULL,",)")) != NULL)             // y ?
   {
   }

   // these layers are default mirror, but can be overwritten by NOMirror
   if (!STRCMPI(lay, "SILKSCREEN_2"))  mirror = TRUE;
   if (!STRCMPI(lay, "SOLDER_MASK_2")) mirror = TRUE;
   if (!STRCMPI(lay, "PASTE_MASK_2"))  mirror = TRUE;

   while ((lp = get_string(NULL,",)")) != NULL)                // mirror
   {
      CString t;
      t = lp;  // can have blanks
      t.TrimLeft();
      t.TrimRight();

      if (!STRCMPI(t,"@mirror"))
         mirror = 1;
      else
      if (!STRCMPI(t,"@nomirror"))
         mirror = 0;
   }

   // w / 0.8 is the char width plus its space
   //normalize_text(&x, &y, just, r, mirror, h, w/0.8*strlen(value));
   HorizontalPositionTag textAlignment = getTextAlignment(just);
   VerticalPositionTag   lineAlignment = getLineAlignment(just);

   // this is just a placeholder text
   if (STRCMPI(value,"^$ref") == 0)   // this will allow multiple reference designators
   {
      CString layerName = lay;
      CString layerName_1;
      CString layerName_2;

      // If the layer is a generic Top & Bottom mirror layer, then always use the TOP layer of the generic layer
      if (IsGenericTopBottomMappingLayer(layerName, layerName_1, layerName_2))
         layerName.Format("%s_TOP", lay);

      laynr = Graph_Level(layerName, "", 0);

      // place an attribute
      if (cur_b != NULL)
      {
         CAttributeMapWrap blockAttributeMap(cur_b->getAttributesRef());
         blockAttributeMap.addInstance(doc->getCamCadData(), refNameKeyword, valueTypeString, "REF", x, y,
               DegToRad(r), h, w/0.8, 1,0, TRUE, 0L, laynr, 0, 0,textAlignment,lineAlignment);
      }
   }
   else if (!STRNICMP(value, "^$pinref", 8)) // this will allow multiple pin reference designators
   {
      // lay for refname must be on top or bottom, so that mirror works
      if (!strstr(lay, "_1") && !strstr(lay, "_2"))
      {
         CString ws;
         ws.Format("%s_1", lay);
         laynr = Graph_Level(ws, "", 0);
      }
      else
      {
         laynr = Graph_Level(lay, "", 0);
      }

      // Get the pin number from the string, ex: "^$pinref,1,p" <== pinName is 1
      CString pinName = value;
      int index1 = pinName.Find(',', 0);
      int index2 = pinName.Find(',', index1 + 1);
      if (index2 > -1)
         pinName = pinName.Mid(index1 + 1, index2 - index1 -1);
      else
         pinName = pinName.Right(pinName.GetLength() - index1);
      pinName.Trim();

      int ptr = get_comppin(pinName);
      CompPin *cpin = comppin[ptr];

      // Add the attrib to the attrib link list
      if (ptr > -1)
      {
         Attrib* attrib = doc->getCamCadData().constructAttribute();
         attrib->setProperties(x - cpin->getX(), y - cpin->getY(), DegToRad(r), h, w/0.8, 0, 
            TRUE, TRUE, FALSE, FALSE, FALSE,intToHorizontalPositionTag(textAlignment),
            intToVerticalPositionTag(lineAlignment), laynr, 0L);
         attrib->setValueType(valueTypeString);
         attrib->setStringValueIndex(doc->RegisterValue("REF"));

         if (cpin->getAttrib() != NULL)
         {
            cpin->getAttrib()->addInstance(attrib);
         }
         else
         {
            cpin->setAttrib(attrib);
         }
      }
   }
   else if (!STRNICMP(value,"^$",2))
   {
      CString att;
      att.Format("%s_%s", lay, value);

      CString layerName = lay;
      CString layerName_1;
      CString layerName_2;

      // If the layer is a generic Top & Bottom mirror layer, then always use the TOP layer of the generic layer
      if (IsGenericTopBottomMappingLayer(layerName, layerName_1, layerName_2))
         layerName.Format("%s_TOP", lay);

      laynr = Graph_Level(layerName, "", 0);

      // place an attribute
      if (cur_b)
      {
         int visible = FALSE;
         doc->SetUnknownVisAttrib(&cur_b->getAttributesRef(), get_attrmap(att), "REF", x, y, DegToRad(r),
               h, w/0.8, 1, 0, visible, attributeUpdateOverwrite, 0L, laynr, 0,textAlignment,lineAlignment); 
      }
   }
   else if (cur_create == CREATE_PIN || cur_create == CREATE_VIA)
   {
      CString  pshapename;

      int      smd = FALSE;

      if (cur_padlistptr > -1)
      {
         smd = padlistarray[cur_padlistptr]->smd;
      }
      make_padlayer(lay, smd);
      update_layer((CString)lay);
      laynr = Graph_Level(lay, "", 0);

      if (!is_complex_attached(laynr))
      {
			pshapename.Format("%s %s", cur_name, lay);
			BlockStruct *subblock = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
			subblock->setBlockType(BLOCKTYPE_PADSHAPE);
			DataStruct *data = Graph_Text(Graph_Level("0", "", TRUE), value, x, y, h, w, DegToRad(r), 0, FALSE, mirror, 0, FALSE, -1, 0,
				textAlignment,lineAlignment); 
			Graph_Block_Off();

         CString  pxshapename;
         pxshapename.Format("PADSHAPE_%s", pshapename);
         Graph_Complex(pxshapename, 0, subblock, 0.0, 0.0, 0.0);
         data = Graph_Block_Reference(pxshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

         CString layer;
         layer.Format("%d", laynr);

         MEN4Padlist *padlist = padlistarray[cur_padlistptr];
         padlist->complex_cnt.SetAt(layer, "1");

         // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
         CDataList *dataList = GetCurrentDataList();
         AddDataToLayer(lay, dataList, data, DATA_TEXT);
      }
		else
		{
			pshapename.Format("%s %s", cur_name, lay);
			BlockStruct *block = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
			block->setBlockType(BLOCKTYPE_PADSHAPE);
			DataStruct *data = Graph_Text(laynr, value, x, y, h, w, DegToRad(r), 0, FALSE, mirror, 0, FALSE, -1, 0,
				textAlignment,lineAlignment); 
			Graph_Block_Off();

			// Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
			CDataList *dataList = GetCurrentDataList();
			AddDataToLayer(lay, dataList, data, DATA_TEXT);
		}
   }
   else
   {
      make_otherlayer(lay);
      update_layer((CString)lay);
      laynr = Graph_Level(lay, "", 0);

      DataStruct *data = Graph_Text(laynr, value, x, y, h, w, DegToRad(r), 0, FALSE, mirror, 0, FALSE, -1, 0,
         textAlignment,lineAlignment);

      // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
      CDataList *dataList = GetCurrentDataList();
      AddDataToLayer(lay, dataList, data, DATA_TEXT);
   }

   return 1;
}

/******************************************************************************
* is_poly_aperture
   return TRUE is a aperture
*/
static int is_poly_aperture(int *form, double *sizea, double *sizeb, double *offsetx, double *offsety)
{
   if (polycnt != 5) return   0;

   // write into a poly buffer and than later decide if this is an aperture or complex
   CPoly *poly = new CPoly;
   poly->setClosed(true);

   CPnt     *pnt;
   int      res = 0;

   for (int i=0;i<polycnt;i++)
   {
      MenPoly p = polyarray.ElementAt(i);
      pnt = new CPnt;
      pnt->bulge = 0.0;
      pnt->x = (DbUnit)p.x;
      pnt->y = (DbUnit)p.y;
      poly->getPntList().AddTail(pnt);
   }

   // loop through poly
   double cx, cy, radius;
   double   sizeA, sizeB, llx, lly, uux, uuy;

   if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
   {
      *sizea = radius;
      *sizeb = radius;
      *offsetx = cx;
      *offsety = cy;
      *form = T_ROUND;
      res = 1;
   }
   else if (PolyIsRectangle(poly, &llx, &lly, &uux, &uuy))
   {
      *sizea = (uux - llx);
      *sizeb = (uuy - lly);
      *offsetx = llx + (uux - llx)/2;
      *offsety = lly + (uuy - lly)/2;

      if (((uux - llx)) == (uuy - lly))
         *form = T_SQUARE;
      else
         *form = T_RECTANGLE;
      res = 1;
   }
   else if (PolyIsOblong(poly, &sizeA, &sizeB, &cx, &cy))
   {  
      // oblong
      int r = 0;
   }
   else
   {
      // complex
   }

   delete poly;

   return res;
}

/****************************************************************************/
/*
   return TRUE is a aperture. This is if a path is only 2 points. That results 
   in a oval.
*/
static int is_path_aperture(double width, int *form, double *sizea, double *sizeb, 
                            double *offsetx, double *offsety, double *rot)
{
   double len, left, right;

   if (polycnt != 2) 
      return   0;
   MenPoly p0 = polyarray.ElementAt(0);
   MenPoly p1 = polyarray.ElementAt(1);

   Point2   tr, ll;  // top right, lowleft
   ll.x = p0.x;
   ll.y = p0.y;
   tr.x = p0.x;
   tr.y = p0.y;

   if (p1.x < ll.x)  ll.x = p1.x;
   if (p1.y < ll.y)  ll.y = p1.y;

   if (p1.x > tr.x)  tr.x = p1.x;
   if (p1.y > tr.y)  tr.y = p1.y;

   *rot = atan2(tr.y - ll.y, tr.x - ll.x);

   len = sqrt((tr.x - ll.x)*(tr.x - ll.x) + (tr.y - ll.y)*(tr.y - ll.y));
   left = sqrt((ll.x*ll.x) + (ll.y*ll.y));
   right= sqrt((tr.x*tr.x) + (tr.y*tr.y));
   *form = T_OBLONG;
   *offsety = 0;
   *offsetx = len/2 - left;
   *sizea = len+width;
   *sizeb = width;

   // if both offsets are not on zero, it is probably not an aperture, but a 
   // complex pad.
   if (fabs(*offsetx) > SMALLNUMBER &&
       fabs(*offsety) > SMALLNUMBER)
       return 0;

   return 1;
}

/******************************************************************************
* prt_path
*/
static int prt_path()
{
   char *lp;

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   char lay[80];
   if ((lp = get_string(NULL, ",)")) == NULL) 
      return 0;
   strcpy(lay, _strupr(lp));

   clean_layer(lay);
   make_otherlayer(lay);
   int graphic_class = get_graphic_class(lay);
   
   if (strstr(lay, "_KEEPOUT") || strstr(lay, "_KEEPIN"))  // generic keepout and keepin are ALL layers
      strcpy(lay, "ALL");

   int orig_laynr = update_layer((CString)lay);
   int laynr = Graph_Level(lay, "", 0);

   double width = 0;
   if ((lp = get_string(NULL, ",")) != NULL)  
      width  = cnv_tok(lp);

   write_poly(laynr, width, 0, 0, graphic_class, DATA_PATH, TRUE);

   if ((lp = get_string(NULL, ",")) == NULL)
      return 0;

   polycnt = 0;
   get_koos(0);

   // here need to close polygon

   if (cur_create == CREATE_PIN || cur_create == CREATE_VIA)
   {
      CString pshapename;
      int form;      
      double sizea, sizeb, offsetx, offsety, rot;

      int smd = FALSE;
      if (cur_padlistptr > -1)
         smd = padlistarray[cur_padlistptr]->smd;
      
      make_padlayer(lay, smd);
      update_layer((CString)lay);
      laynr = Graph_Level(lay, "", 0);

      // here need to do test if polygon is a rectangle.
      if (!is_pad_complex(orig_laynr) && is_path_aperture(width, &form, &sizea, &sizeb, &offsetx, &offsety, &rot))
      {
         int pindex = get_padformindex(form, sizea, sizeb, offsetx, offsety);
         pshapename.Format("PADSHAPE_%d", pindex);
         polycnt = 0;
         DataStruct *data = Graph_Block_Reference(pshapename, NULL, pcbFileNum, 0.0, 0.0, rot, 0, 1.0, laynr, TRUE);

         // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
         CDataList *dataList = GetCurrentDataList();
         AddDataToLayer(lay, dataList, data, DATA_PATH);
      }
      else
      {
         if (!is_complex_attached(laynr))
         {
				// here is a complex padstack, because it is not rectangle or square
				pshapename.Format("%s %s", cur_name, lay);
				BlockStruct *subblock = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
				subblock->setBlockType(BLOCKTYPE_PADSHAPE);

				int closed = 0;
				if (polycnt > 1)
				{
					MenPoly p1 = polyarray.ElementAt(0);
					MenPoly p2 = polyarray.ElementAt(polycnt-1);
					closed = (fabs(p1.x - p2.x) < SMALLNUMBER  && fabs(p1.y - p2.y) < SMALLNUMBER);
				}

				// This is going to be a complex so make sure the last parameter is FALSE and use floating layer 0
				write_poly(Graph_Level("0", "", 1), width, 0, closed, 0, DATA_PATH, TRUE, FALSE);
				Graph_Block_Off();

				CString pxshapename;
            pxshapename.Format("PADSHAPE_%s", pshapename);
            Graph_Complex(pxshapename, 0, subblock, 0.0, 0.0, 0.0);
            DataStruct *data = Graph_Block_Reference(pxshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

            CString layer;
            layer.Format("%d", laynr);

            MEN4Padlist *padlist = padlistarray[cur_padlistptr];
            padlist->complex_cnt.SetAt(layer, "1");

            // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
            CDataList *dataList = GetCurrentDataList();
            AddDataToLayer(lay, dataList, data, DATA_PATH);
         }
			else
			{
				// here is a complex padstack, because it is not rectangle or square
				pshapename.Format("%s %s", cur_name, lay);
				BlockStruct *block = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
				block->setBlockType(BLOCKTYPE_PADSHAPE);

				int closed = 0;
				if (polycnt > 1)
				{
					MenPoly p1 = polyarray.ElementAt(0);
					MenPoly p2 = polyarray.ElementAt(polycnt-1);
					closed = (fabs(p1.x - p2.x) < SMALLNUMBER  && fabs(p1.y - p2.y) < SMALLNUMBER);
				}

				write_poly(Graph_Level("0", "", 1), width, 0, closed, 0, DATA_PATH, TRUE, TRUE);
				Graph_Block_Off();
			}
      }
   }
   else
   {
      make_otherlayer(lay);
      update_layer((CString)lay);
      laynr = Graph_Level(lay,"",0);
      graphic_class = get_graphic_class(lay);
      write_poly(laynr, width,0, 0, graphic_class, DATA_PATH, TRUE);
   }
   return 1;
}

/******************************************************************************
* prt_polygon
*/
static int prt_polygon()
{
   char     *lp;
   char     lay[80];
   int      orig_laynr, laynr, graphic_class = 0;

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   if ((lp = get_string(NULL,",)")) == NULL)
      return 0;
   strcpy(lay, _strupr(lp));
   clean_layer(lay);
   make_otherlayer(lay);

   orig_laynr = update_layer((CString)lay);

   if (cur_create == CREATE_PIN || cur_create == CREATE_VIA)
   {
      int      smd = FALSE;

      if (cur_padlistptr > -1)
      {
         smd = padlistarray[cur_padlistptr]->smd;
      }
      make_padlayer(lay, smd);
   }
   else
   {
      make_otherlayer(lay);
   }

   update_layer((CString)lay);

   laynr = Graph_Level(lay, "", 0);
   graphic_class = get_graphic_class(lay);

// there coould be terms before poly.
/*
   write_poly(laynr, 0.0 ,1, 1, graphic_class, TRUE); // maybe rest from initial , initial is not filled ??
*/
   if ((lp = get_string(NULL, ",")) == NULL) 
      return 0;

// 
// polycnt = 0;
   get_koos(0);

   if (polycnt == 0)
      return 0;

   if (cur_create == CREATE_PIN || cur_create == CREATE_VIA)
   {
      CString  pshapename;
      int      form;    
      double   sizea,sizeb,offsetx,offsety;
      int      smd = FALSE;

      MenPoly p1 = polyarray.ElementAt(0);
      MenPoly p2 = polyarray.ElementAt(polycnt-1);

      // make sure it is closed
      if (p1.x != p2.x || p1.y != p2.y)
      {
         polyarray.SetAtGrow(polycnt,p1);
         polycnt++;
      }
      laynr = Graph_Level(lay,"",0);

      if (!is_pad_complex(orig_laynr) && is_poly_aperture(&form,&sizea,&sizeb,&offsetx,&offsety))
      {
         int pindex = get_padformindex(form, sizea, sizeb, offsetx, offsety);
         polycnt = 0;

         CString pshapename;
         pshapename.Format("PADSHAPE_%d", pindex);
         DataStruct *data = Graph_Block_Reference(pshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

         // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
         CDataList *dataList = GetCurrentDataList();
         AddDataToLayer(lay, dataList, data, DATA_POLYGON);
      }
      else
      {
         if (!is_complex_attached(laynr))
         {
				// This will be a complex so insert them on floating layer
				int floatinglaynr = Graph_Level("0", "", TRUE);

				pshapename.Format("%s %s", cur_name, lay);
				BlockStruct *subblock = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
				subblock->setBlockType(BLOCKTYPE_PADSHAPE);
				write_poly(floatinglaynr, 0.0, 1,1, 0, DATA_POLYGON, TRUE, FALSE);
				Graph_Block_Off();

            CString pxshapename;
            pxshapename.Format("PADSHAPE_%s", pshapename);
            Graph_Complex(pxshapename, 0, subblock, 0.0, 0.0, 0.0);
            DataStruct *data = Graph_Block_Reference(pxshapename, NULL, pcbFileNum, 0.0, 0.0, 0.0, 0, 1.0, laynr, TRUE);

            CString layer;
            layer.Format("%d", laynr);

            MEN4Padlist *padlist = padlistarray[cur_padlistptr];
            padlist->complex_cnt.SetAt(layer, "1");

            // Add to layerToDataMap for correctly creating insert on TOP & BOTTOM layers mapping
            CDataList *dataList = GetCurrentDataList();
            AddDataToLayer(lay, dataList, data, DATA_POLYGON);
         }
			else
			{
				pshapename.Format("%s %s", cur_name, lay);
				BlockStruct *b = Mentor_Graph_Block_On(GBO_APPEND, pshapename, pcbFileNum, 0);
				b->setBlockType(BLOCKTYPE_PADSHAPE);
				write_poly(laynr, 0.0, 1,1, 0, DATA_POLYGON, TRUE);
				Graph_Block_Off();
			}
      }
   }
   else
   {
      graphic_class = get_graphic_class(lay);
      write_poly(laynr, 0.0 ,1, 1, graphic_class, DATA_POLYGON, TRUE);
   }
   return 1;
}

/******************************************************************************
* prt_initial
*/
static int prt_initial()
{
   if (cur_create == CREATE_UNKNOWN)
      return 0;

   polycnt = 0;
   get_koos(POLYTYPE_INIT);
   return 1;
}

/****************************************************************************/
/*
*/
static int prt_template_path_width()
{
   char  *lp;

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   if ((lp = get_string(NULL,",")) == NULL) 
      return 0;   
   template_path_width = cnv_tok(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int prt_terminal()
{
   if (cur_create == CREATE_UNKNOWN)
      return 0;

   get_koos(POLYTYPE_TERM);
   return 1;
}

/****************************************************************************/
/*
   $$define_physical_layer(   1, "PHYSICAL_1", "PAD_1", "SIGNAL_1");

   $$define_physical_layer(   1, "ph_1top_G", "PAD_1", "SIGNAL_1");
   $$define_physical_layer(   2, "ph_2G", "SIGNAL_3");
   $$define_physical_layer(   3, "ph_3V", "SIGNAL_4");
   $$define_physical_layer(   4, "ph_4H", "SIGNAL_5");
   $$define_physical_layer(   5, "ph_5P", "SIGNAL_6");
   $$define_physical_layer(   6, "ph_6G", "SIGNAL_7");
   $$define_physical_layer(   7, "ph_7V", "SIGNAL_8");
   $$define_physical_layer(   8, "ph_8H", "SIGNAL_9");
   $$define_physical_layer(   9, "ph_9G", "SIGNAL_10");
   $$define_physical_layer(  10, "ph_10btm_G", "PAD_2", "SIGNAL_2");

   PHYSICAL is not required to be in any certain order !
   $$define_physical_layer(   1, "PHYSICAL_1", "SIGNAL_1", "PAD_1");
   $$define_physical_layer(   2, "PHYSICAL_2", "SIGNAL_2");
   $$define_physical_layer(   3, "PHYSICAL_3", "POWER_1");
   $$define_physical_layer(   4, "PHYSICAL_4", "SIGNAL_5");
   $$define_physical_layer(   5, "PHYSICAL_10", "SIGNAL_3");
   $$define_physical_layer(   6, "PHYSICAL_11", "SIGNAL_4", "PAD_2");

*/
static int prt_define_physical_layer()
{
   char  *lp;
   int   stacknum = -1, physnum = -1;

   if ((lp = get_string(NULL," \t,")) == NULL)  // stackup number
      return 0;      
   physnum = stacknum = atoi(lp);
   if ((lp = get_string(NULL," \t,")) == NULL) // physical_xxx 
      return 0;      

/*
   if (!STRNICMP(lp,"PHYSICAL_", strlen("PHYSICAL_")))
   {
      int res = sscanf(lp,"PHYSICAL_%d", &physnum);
      if (res != 1)  physnum = stacknum;
   }
*/
   if (stacknum >= MAX_LAYERS)
   {
      fprintf(ferr,"Too many layers\n");
      display_error++;  
   }
   layer_phys[stacknum].stacknum = stacknum;
   layer_phys[stacknum].physname = lp;
   layer_phys[stacknum].logicalname = "";

   while (lp = get_string(NULL," \t,);")) 
   {
      LayerStruct *l;
      if (physnum < 1)  
         continue;
      if ((l = doc->FindLayer_by_Name(lp)) != NULL)
      {
         l->setElectricalStackNumber(physnum);
         l->setPhysicalStackNumber(physnum);
      }
      if (strlen(layer_phys[stacknum].logicalname))
         layer_phys[stacknum].logicalname += ",";
      layer_phys[stacknum].logicalname += lp;
   }

   if (stacknum > layer_phys_cnt)
      layer_phys_cnt = stacknum;

   return 1;
}

/******************************************************************************
* Create_LayerrangePADSTACKData
*/
static int Create_LayerrangePADSTACKData(const char *pname, CDataList *DataList, int layerPtr1, int layerPtr2, int top)
{
   int typ = 0;   
   BOOL created = FALSE;
   CMapStringToPtr dataExistedMap;

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);
      switch(data->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());   
               
            if ( (block->getFlags() & BL_TOOL))
            {
               // always do the drill
               DataStruct *copy = Graph_Block_Reference(block->getBlockNumber(), data->getInsert()->getRefname(), 
                                       data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                       data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex()); 
               copy->getInsert()->setInsertType(data->getInsert()->getInsertType());
            }
            else
            {
               LayerStruct *layer = doc->FindLayer(data->getLayerIndex());

               if (!layer->getName().CompareNoCase("POWER") || !layer->getName().CompareNoCase("SIGNAL"))
               {
                  CString dataLayerName = layer->getName() + "_";
                  BOOL found = FALSE;

                  for (int i=layerPtr1; i<=layerPtr2; i++)
                  {
                     int curPos = 0;
                     CString layerName = layer_phys[i].logicalname.Tokenize(",", curPos);

                     while (layerName.Trim() != "")  // only signal, 
                     {
                        // Look for "POWER_" or "SIGNAL_"
                        if (layerName.Find(dataLayerName, 0) < 0)
                        {
                           layerName = layer_phys[i].logicalname.Tokenize(",", curPos);
                           continue;
                        }

                        // Skip if the layer already has data inserted
                        void *voidPtr;
                        if (dataExistedMap.Lookup(layerName, voidPtr))
                        {
                           layerName = layer_phys[i].logicalname.Tokenize(",", curPos);
                           continue;
                        }
                        // Layer found so copy the data
                        int lay = Graph_Level(layerName, "", 0);
                        DataStruct *copy = Graph_Block_Reference(block->getBlockNumber(), data->getInsert()->getRefname(),  
                                                data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                                data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), lay);
                        copy->getInsert()->setInsertType(data->getInsert()->getInsertType());
                        dataExistedMap.SetAt(layerName, copy);
                        found = TRUE;

                        // Get next physical layer name
                        layerName = layer_phys[i].logicalname.Tokenize(",", curPos);
                     }
                  }

                  if (!found)
                  {
                     fprintf(ferr, "Need to expand padstack [%s] Layer [%s] to [%s] [%s]\n",
                           pname, layer->getName(), layer_phys[layerPtr1].logicalname, layer_phys[layerPtr2].logicalname);
                     fprintf(ferr, "Padstack [%s] Layer [%s] defined is not in define_pin_rule [%s] [%s]\n",
                           pname, layer->getName(), layer_phys[layerPtr1].logicalname, layer_phys[layerPtr2].logicalname);
                     display_error++;
                  }
               }
               else if (!layer->getName().CompareNoCase("PAD") || !layer->getName().CompareNoCase("SOLDER_MASK") ||
                        !layer->getName().CompareNoCase("PASTE_MASK"))
               {
                  CString layerName = layer->getName();

                  if (layerPtr1 == 1)
                  {
                     // Only add to PAD_TOP, SOLDER_MASK_TOP, or PASTE_MASK_TOP
                     // if the range of physical touch the first layer
                     layerName += "_TOP";
                  }
                  else if (layerPtr2 == layer_phys_cnt)
                  {
                     // Only add to PAD_BOT, SOLDER_MASK_BOT, or PASTE_MASK_BOT
                     // if the range of physical touch the last layer
                     layerName += "_BOT";
                  }
                  else
                  {
                     // Skip
                     continue;
                  }

                  int lay = Graph_Level(layerName, "", 0);
                  DataStruct *copy = Graph_Block_Reference(block->getBlockNumber(), data->getInsert()->getRefname(), 
                                          data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                          data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), lay);
                  copy->getInsert()->setInsertType(data->getInsert()->getInsertType());
                  dataExistedMap.SetAt(layerName, copy);
               }
               else if (layer->getName().Find("PAD_") > -1 || layer->getName().Find("SOLDER_MASK_") > -1 ||
                        layer->getName().Find("PASTE_MASK_") > -1)
               {
                  // If the layer is PAD_BOT, PAD_TOP, SOLDER_MASK_TOP, SOLDER_MASK_BOT,
                  // PASTE_MASK_TOP, or PAST_MASK_BOT
                  // Remove the data if it is already there and insert another at this layer
                  void *voidPtr;
                  if (dataExistedMap.Lookup(layer->getName(), voidPtr))
                  {
                     DataStruct *dataToRemove = (DataStruct*)voidPtr;
                     RemoveOneEntityFromDataList(doc, DataList, dataToRemove, NULL);
                  }

                  int lay = Graph_Level(layer->getName(), "", 0);
                  DataStruct *copy = Graph_Block_Reference(block->getBlockNumber(), data->getInsert()->getRefname(), 
                                          data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                          data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), lay);
                  copy->getInsert()->setInsertType(data->getInsert()->getInsertType());
                  dataExistedMap.SetAt(layer->getName(), copy);
               }
               else
               {
                  for (int i=layerPtr1; i<=layerPtr2; i++)
                  {
                     int curPos = 0;
                     CString layerName = layer_phys[i].logicalname.Tokenize(",", curPos);

                     while (layerName.Trim() != "")  // only signal, 
                     {
                        // Check to see if layer is in the physical layer list
                        if (layerName.CompareNoCase(layer->getName()))
                        {
                           layerName = layer_phys[i].logicalname.Tokenize(",", curPos);
                           continue;
                        }

                        // Remove the data if it is already there and insert another at this layer
                        void *voidPtr;
                        if (dataExistedMap.Lookup(layerName, voidPtr))
                        {
                           DataStruct *dataToRemove = (DataStruct*)voidPtr;
                           RemoveOneEntityFromDataList(doc, DataList, dataToRemove, NULL);
                        }
                  
                        // Layer found so copy the data
                        int lay = Graph_Level(layerName, "", 0);
                        DataStruct *copy = Graph_Block_Reference(block->getBlockNumber(), data->getInsert()->getRefname(), 
                                                data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                                data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), lay);
                        copy->getInsert()->setInsertType(data->getInsert()->getInsertType());
                        dataExistedMap.SetAt(layerName, copy);

                        // Get next physical layer name
                        layerName = layer_phys[i].logicalname.Tokenize(",", curPos);
                     }
                  }
               }

/*               if (layer->getElectricalStackNumber() == 0) // if the layer is non electrical layer then copy
               {
                  DataStruct *copy = Graph_Block_Reference(block->name, data->getInsert()->refname, block->filenum, 
                                          data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                          data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex(), TRUE);
                  copy->getInsert()->getInsertType() = data->getInsert()->getInsertType();
                  continue;
               }        

               // either the padstack has to be exploded from SIGNAL to SIGNAL_2 etc...
               // or the padstack def was already exploded and only the needed layer has top be copied.
               if (layer->getName().CompareNoCase("POWER") == 0)
                  continue;
               if (!layer->getName().CompareNoCase("SIGNAL"))
               {
                  int found = 0;
                  for (int i=l1ptr; i<=l2ptr; i++)
                  {
                     char tmp[255];
                     strcpy(tmp, layer_phys[i].logicalname);
                     char *lphys = strtok(tmp, ",");

                     while (lphys)  // only signal, 
                     {
                        if (STRNICMP(lphys, "PAD", 3))      // not PAD_ do it
                        {
                           int lay = Graph_Level(lphys, "", 0);
                           DataStruct *copy = Graph_Block_Reference(block->name, data->getInsert()->refname, block->filenum, 
                                                   data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                                   data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), lay, TRUE);
                           copy->getInsert()->getInsertType() = data->getInsert()->getInsertType();
                           found++;
                        }
                        lphys = strtok(NULL, ",");
                     }
                  }
                  // layer is pad is not in span of tech
                  if (!found)
                  {
                     fprintf(ferr, "Need to expand padstack [%s] Layer [%s] to [%s] [%s]\n",
                           pname, layer->getName(), layer_phys[l1ptr].logicalname, layer_phys[l2ptr].logicalname);
                     fprintf(ferr, "Padstack [%s] Layer [%s] defined is not in define_pin_rule [%s] [%s]\n",
                           pname, layer->getName(), layer_phys[l1ptr].logicalname, layer_phys[l2ptr].logicalname);
                     display_error++;
                  }

               }
               else if (!layer->getName().CompareNoCase("PAD"))
               {
                  fprintf(ferr,"Need to expand padstack [%s] Layer [%s] to [%s] [%s]\n",
                        pname, layer->getName(), layer_phys[l1ptr].logicalname, layer_phys[l2ptr].logicalname);
                  display_error++;

                  CString elecLayerName = layer_phys[l1ptr].logicalname;
                  int index = elecLayerName.Find(',');
                  CString layerName = elecLayerName.Right(elecLayerName.GetLength() - index - 1);

                  LayerStruct *tmpLayer = doc->FindLayer_by_Name(layerName);
                  
                  data->getLayerIndex() = tmpLayer->num;

                  if (l1ptr != l2ptr)
                  {
                     CString elecLayerName = layer_phys[l2ptr].logicalname;
                     int index = elecLayerName.Find(',');
                     CString layerName = elecLayerName.Right(elecLayerName.GetLength() - index - 1);

                     LayerStruct *tmpLayer = doc->FindLayer_by_Name(layerName);

                     DataStruct *copy = Graph_Block_Reference(block->name, data->getInsert()->refname, block->filenum, 
                                             data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                                             data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), tmpLayer->num, TRUE);
                     copy->getInsert()->getInsertType() = data->getInsert()->getInsertType();
                     created = TRUE;
                  }
               }
               else
               {
                  // here check if this layer is part of the spanning signal layer layer.
                  int found = FALSE;
                  for (int i=1; i<=layer_phys_cnt; i++)
                  {
                     char  tmp[255];
                     strcpy(tmp,layer_phys[i].logicalname);
                     char *lphys = strtok(tmp, ",");
                     while (lphys)
                     {
                        if (layer->getName().CompareNoCase(lphys) == 0)
                        {
                           found = TRUE;
                           break;
                        }
                        lphys = strtok(NULL, ",");
                     }
                  }

                  if (!found) // this is not a physical layer !
                     continue;   

                  // this layer is a physical layer, now check if it is in the spann
                  found = 0;
                  for (i=l1ptr; i<=l2ptr; i++)
                  {
                     char  tmp[255];
                     strcpy(tmp,layer_phys[i].logicalname);
                     char *lphys = strtok(tmp, ",");
                     while (lphys)
                     {
                        if (layer->getName().CompareNoCase(lphys) == 0)
                        {
                           found = TRUE;
                           break;
                        }
                        lphys = strtok(NULL, ",");
                     }
                  }

                  // layer is pad is not in span of tech
                  if (!found)
                  {
                     fprintf(ferr, "Padstack [%s] Layer [%s] defined is not in define_pin_rule [%s] [%s]\n",
                           pname, layer->getName(), layer_phys[l1ptr].logicalname, layer_phys[l2ptr].logicalname);
                     display_error++;
                  }
                  else
                  {
                     // copy it !
                     if (!created)
                     {
                        DataStruct *copy = Graph_Block_Reference(block->name, data->getInsert()->refname, 
                              block->filenum, data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), 
                              data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex(), TRUE);
                        copy->getInsert()->getInsertType() = data->getInsert()->getInsertType();
                     }
                  }
               } // not drill
*/


            }
         }
      }
   }

   return typ;

} /* end Create_LayerrangePadstackData */

/****************************************************************************/
/*
*/
static int  get_layer_phys_ptr(const char *l)
{

   for (int i=1;i<=layer_phys_cnt;i++)
   {
      if (layer_phys[i].physname.CompareNoCase(l) == 0)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
$$define_rule_pin( "lf1_netcon_sig3", "PHYSICAL_3", "PHYSICAL_3" , @NOTOP );
$$define_rule_pin( "lf1_netcon_sig3", "PHYSICAL_3", "PHYSICAL_3" , @TOP );
$$define_rule_pin( "lf1_netcon_sig5", "PHYSICAL_1", "PHYSICAL_1" , @TOP );
$$define_rule_pin( "lf1_netcon_sig5", "PHYSICAL_6", "PHYSICAL_6" , @NOTOP );
$$define_rule_pin( "lf1_xg400", "PHYSICAL_5", "PHYSICAL_5" , @TOP );
$$define_rule_pin( "lf1_xg400", "PHYSICAL_5", "PHYSICAL_5" , @NOTOP );
$$define_rule_pin( "lf2_xg400", "PHYSICAL_4", "PHYSICAL_4" , @TOP );
$$define_rule_pin( "lf2_xg400", "PHYSICAL_4", "PHYSICAL_4" , @NOTOP );
$$define_rule_pin( "pad_dk_gnd_2_5", "PHYSICAL_2", "PHYSICAL_5" , @TOP );
$$define_rule_pin( "pad_dk_gnd_2_5", "PHYSICAL_5", "PHYSICAL_2" , @NOTOP );
$$define_rule_pin( "pad_dk_gnd_mov", "PHYSICAL_1", "PHYSICAL_2" , @TOP );
$$define_rule_pin( "pad_dk_gnd_mov", "PHYSICAL_6", "PHYSICAL_5" , @NOTOP );
$$define_rule_pin( "pad_via0.6_buried", "PHYSICAL_2", "PHYSICAL_5" , @TOP );
$$define_rule_pin( "pad_via0.6_buried", "PHYSICAL_5", "PHYSICAL_2" , @NOTOP );
*/
static int prt_define_rule_pin()
{
   char *lp;
   if ((lp = get_string(NULL," \t,")) == NULL)  
      return 0;      // 
   CString padname = _strupr(lp);

   if ((lp = get_string(NULL," \t,")) == NULL)  
      return 0;      // 
   int startLayer = get_layer_phys_ptr(lp);

   if ((lp = get_string(NULL," \t,")) == NULL) 
      return 0;      // 
   int endLayer = get_layer_phys_ptr(lp);

   if ((lp = get_string(NULL," \t,")) == NULL) 
      return 0;      // 
   int top = !STRCMPI(lp,"@TOP");

	CMentorPadRule *padrule = new CMentorPadRule(padname, startLayer, endLayer, top);
	padrulearray.SetAtGrow(padrulecnt++, padrule);

   BlockStruct *block = Graph_Block_Exists(doc, padname, pcbFileNum);
   if (block)
   {
      if (!top)   
         return 0;

      if (startLayer < 1)
         return 0;
      if (endLayer < 1)
         return 0;

      if (startLayer > endLayer)
      {
         int tmpLayer = startLayer;
         startLayer = endLayer;
         endLayer = startLayer;
      }

      CString pin_padname;
      pin_padname.Format("%s_pin (%d:%d)", padname, startLayer, endLayer);

      BlockStruct *pinBlock = Mentor_Graph_Block_On(GBO_APPEND, pin_padname, pcbFileNum, 0, blockTypePadstack);

      doc->SetAttrib(&pinBlock->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE,NULL,attributeUpdateOverwrite, NULL);
      Create_LayerrangePADSTACKData(padname, &(block->getDataList()), startLayer, endLayer, top);

      Graph_Block_Off();
   }

   return 1;
}

/******************************************************************************
* prt_define_rule_via

   $$define_rule_via( "via_0.6_buried", "PHYSICAL_5", "PHYSICAL_2");
   $$define_rule_via( "via_dummy_buried", "PHYSICAL_2", "PHYSICAL_1");
   $$define_rule_via( "via_mic_0.3_bur", "PHYSICAL_2", "PHYSICAL_1");
   $$define_rule_via( "via_mic_0.3_bur", "PHYSICAL_6", "PHYSICAL_5");
*/
static int prt_define_rule_via()
{
   char     *lp;
   if ((lp = get_string(NULL, " \t,")) == NULL) 
      return 0;      // 
   CString padname = _strupr(lp);

   if ((lp = get_string(NULL, " \t,")) == NULL) 
      return 0;      // 
   int startlayer = get_layer_phys_ptr(lp);

   if ((lp = get_string(NULL, " \t,")) == NULL) 
      return 0;      // 
   int endlayer = get_layer_phys_ptr(lp);

   BlockStruct *block = Graph_Block_Exists(doc, padname, pcbFileNum);
   if (block)
   {
      int s = startlayer;
      int e = endlayer;
      if (s < 1)
         return 0;
      if (e < 1)
         return 0;
      if (s > e)
      {
         s = endlayer;
         e = startlayer;
      }

      CString via_padname;
      via_padname.Format("%s (%d:%d)", padname, s, e);

      BlockStruct *b1 = Mentor_Graph_Block_On(GBO_APPEND, via_padname, pcbFileNum, 0, blockTypePadstack);
      Create_LayerrangePADSTACKData(padname, &(block->getDataList()), s, e, 1);
      
      doc->SetAttrib(&b1->getAttributesRef(), doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE,NULL, attributeUpdateOverwrite, NULL); 
      Graph_Block_Off();

   }

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
      case 1:  *rot = 0;   *mir = 0;
         break;
      case 2:  *rot = 0;   *mir = 1;   
         break;
      case 4:  *rot = 180; *mir = 0;
         break;
      case 3:  *rot = 180; *mir = 1;   
         break;
      case 7:  *rot = 270; *mir = 0;
         break;
      case 5:  *rot = 270; *mir = 1;
         break;
      case 6:  *rot = 90;  *mir = 0;
         break;
      case 8:  *rot = 90;  *mir = 1;
         break;
      case 9:  *rot = 135; *mir = 0;
         break;
      case 13: *rot = 135; *mir = 1;
         break;
      case 15: *rot = 225; *mir = 0;
         break;
      case 11: *rot = 225; *mir = 1;
         break;
      case 12: *rot = 135; *mir = 0;
         break;
      case 16: *rot = 135; *mir = 1;
         break;
      case 14: *rot = 45;  *mir = 0;
         break;
      case 10: *rot = 45;  *mir = 1;
         break;
   }

   return;
}

/****************************************************************************/
/*
*/
static int prt_add()
{

   if (cur_create == CREATE_UNKNOWN)
      return 0;

   char* lp;
   char geom_name[80];
   char refname[80];

   if ((lp = get_string(NULL,",")) == NULL) 
      return 0;
   strcpy(geom_name,_strupr(lp));
   clean_name(geom_name);

   // here make it, because it does not have to be in sequence
	BlockStruct* block = Graph_Block_Exists(doc, geom_name, pcbFileNum);
	if (block == NULL)
	{
		block = Mentor_Graph_Block_On(GBO_APPEND, geom_name, pcbFileNum, 0);
		Graph_Block_Off();
	}

   if ((lp = get_string(NULL,",")) == NULL)  
      return 0;
   double x = cnv_tok(lp);

   if ((lp = get_string(NULL,",")) == NULL) 
      return 0;
   double y = cnv_tok(lp);

   if ((lp = get_string(NULL,",")) == NULL)  // refname
      return 0;   
   strcpy(refname,_strupr(lp));
   clean_name(refname);

   int mir,rot;
   if ((lp = get_string(NULL,",")) == NULL) // rotation
      return 0;   
   cnv_addrot(atoi(lp),&rot,&mir);

   if ((lp = get_string(NULL,",")) == NULL)  // scale numerator
      return 0;   
   double scale_numerator = atof(lp);

   if ((lp = get_string(NULL,",")) == NULL) // scale denominator  
      return 0;   
   double scale_denominator = atof(lp);

   // here still need to do mirror and rotate
   // pass filenum 0, so that it only looks for name.

   // mentor allows same refnames for add.
   // this is no good.
   int cnt = 0;
   CString newRefname = refname;
   DataStruct* data = NULL;
   while (data = datastruct_from_refdes(doc, currentPCBFile->getBlock(), newRefname))
   {
      newRefname.Format("%s_%d", refname, ++cnt);
   }

   data = Graph_Block_Reference(block->getBlockNumber(), newRefname, x, y, DegToRad(1.0 * rot), mir, 
         (scale_numerator / scale_denominator),Graph_Level("0", "", 1));

   if (cnt)
   {
#ifdef _DEBUG
      fprintf(ferr,"DEBUG: $$add Part Reference name [%s] already used -> changed to [%s]\n", refname, newRefname);
      display_error++;
#endif
   }

   if (scale_numerator != scale_denominator)
   {
#ifdef _DEBUG
      fprintf(ferr, "DEBUG: Scaled insertion of [%s] in [%s] detected at %ld.\n", block->getName(), cur_name, ifp_linecnt);
      display_error++;
#endif
   }

   // here inherit generic part if it is one.
   if (block->getBlockType() == BLOCKTYPE_TOOLING)
   {
      data->getInsert()->setInsertType(insertTypeDrillTool); 
   }
   else if (block->getBlockType() == BLOCKTYPE_FIDUCIAL)
   {
      data->getInsert()->setInsertType(insertTypeFiducial); 
      int pt = get_part(geom_name);

      if (pt > -1)
      {
         switch (partarray[pt]->parttype)
         {
         case PART_FIDUCIALTOP:
            break;
         case PART_FIDUCIALBOT:
            if (data->getInsert()->getMirrorFlags() == 0)
            {
               data->getInsert()->setPlacedBottom(true);
            }
            break;
         default:
            break;
         }
      }
   }
   else if (block->getBlockType() == BLOCKTYPE_GENERICCOMPONENT)
   {
      data->getInsert()->setInsertType(insertTypeGenericComponent); 
   }
   else if (block->getBlockType() == BLOCKTYPE_PCB)
   {
      data->getInsert()->setInsertType(insertTypePcb); 
		curDrawingInsertBoard = true;
   }
   else if (get_padlistptr(geom_name) > -1) // geometry can be defined later than call.
   {
      // if it came from a create_pin, it must be a mechanical and not electrical pin.
      data->getInsert()->setInsertType(insertTypeMechanicalPin); 
   }

   if (cur_create == CREATE_BOARD)
   {
      boardLevelGeometryMap.addBoardLevelInsert(data);
   }

   return 1;
}

/******************************************************************************
* get_smallestdrill
*/
static double get_smallestdrill(int unit)
{
   // if it's using globla variable page_unit, it was using the variable before it gets initialized, so pass the unit.
   double d;
   d = 1.0;

   int output_units_accuracy = GetDecimals(unit); 
   for (int dd=0;dd<output_units_accuracy;dd++)
      d *= 0.1;
   return d;
}

/******************************************************************************
* FilterGraphicClass
*/
void FilterGraphicClass(int graphicClass)
{
	for (int i=0; i < doc->getMaxBlockIndex(); i++)
	{
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if (data && data->getGraphicClass() == graphicClass)
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
      }
	}
}

/******************************************************************************
* FilterInsertType
*/
void FilterInsertType(int insertType)
{
	for (int i=0; i < doc->getMaxBlockIndex(); i++)
	{
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
		 if (data && data->getInsert() && data->getInsert()->getInsertType() == insertType)
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
      }
	}
}

/******************************************************************************
* Mentor_Graph_Block_On
*/
BlockStruct *Mentor_Graph_Block_On(int method, CString geomName, int fileNum, DbFlag flag, BlockTypeTag blockType)
{
	
	BlockStruct *tmpBlock = Graph_Block_Exists(doc, geomName, currentPCBFile->getFileNumber(), blockType);

	if (tmpBlock && (tmpBlock->getBlockType() == blockTypePcb || tmpBlock->getBlockType() == blockTypePanel))
	{
   	tmpBlock = Graph_Block_On(method, geomName, currentPCBFile->getFileNumber(), flag);
		return tmpBlock;
	}
	else if (blockType == blockTypePcb || blockType == blockTypePanel)
	{
   	tmpBlock = Graph_Block_On(method, geomName, currentPCBFile->getFileNumber(), flag);
		tmpBlock->setBlockType(blockType);
		return tmpBlock;
	}

	if (useNeutralFile)
	{
		if (!replacedOriginalNeutralGeom.Lookup(geomName, geomName))
		{
			// Original Neutral geometry definition has not been replaced with Boardstation definition, yet,
			// so remove the definition on the original Neutral geometry
			BlockStruct *tmpBlock = Graph_Block_Exists(doc, geomName, pcbFileNum);
			if (tmpBlock && tmpBlock->getBlockType() != blockTypePcb)
			{
				POSITION pos = tmpBlock->getDataList().GetHeadPosition();
				while (pos)
				{
					DataStruct *data = tmpBlock->getDataList().GetNext(pos);
					delete data;
				}

				tmpBlock->getDataList().RemoveAll();
				if (tmpBlock->getAttributesRef())
					tmpBlock->getAttributesRef()->empty();
			}

			replacedOriginalNeutralGeom.SetAt(geomName, geomName);
		}
	}

   return Graph_Block_On(method, geomName, fileNum, flag, blockType);
}

//_____________________________________________________________________________
MENData::MENData(DataStruct* data,CDataList* dataList,int dataType)
: m_data(data)
, m_dataList(dataList)
, m_dataType(dataType)
{
}

//-----------------------------------------------------------------------------
// CBoardLevelGeometry
//-----------------------------------------------------------------------------
CBoardLevelGeometry::CBoardLevelGeometry(const CString name, BlockStruct* originalBlock)
   : m_name(name)
   , m_boardInsertList(false)
{
   reset();
   m_originalBlock = originalBlock;
}

CBoardLevelGeometry::~CBoardLevelGeometry()
{
   reset();
}

void CBoardLevelGeometry::reset()
{
   m_originalBlock = NULL;
   m_derivedBlock = NULL;
   m_boardInsertList.empty();
}

void CBoardLevelGeometry::addBoardLevelInsert(DataStruct* boardLeverInsertData)
{
   m_boardInsertList.AddTail(boardLeverInsertData);
}

void CBoardLevelGeometry::derivedSpecialVersionOfBoardLevelGeometry()
{
   if (checkForContentOnSpecialLayer())
   {
      CString blockName = m_originalBlock->getName() + "_$$ADD";
      m_derivedBlock = Graph_Block_On(    GBO_APPEND, blockName, m_originalBlock->getFileNumber(), 0L, m_originalBlock->getBlockType());
      Graph_Block_Copy(m_originalBlock, 0.0, 0.0, 0.0, 0, 1, -1, TRUE, FALSE);
      Graph_Block_Off();
      
      for (CDataListIterator dataList(*m_derivedBlock); dataList.hasNext();)
      {
         DataStruct* data = dataList.getNext();
         LayerStruct* layer = doc->getLayerAt(data->getLayerIndex());
         if (layer == NULL)
            continue;

         CString layerName = layer->getName();
         CString newLayerName, newMirrorLayerName;
         if (layerName.GetLength() > 2 && (layerName.Right(2) == "_1" || layerName.Right(2) == "_2"))
         {
            int temp = layerName.ReverseFind('_');
            newLayerName.Format("%s_%s", layerName.Left(temp), layerName.Right(2) == "_1"?"TOP":"BOT");
            newMirrorLayerName.Format("%s_%s", layerName.Left(temp), layerName.Right(2) == "_1"?"BOT":"TOP");

            LayerStruct* newLayer = doc->Add_Layer(newLayerName);
            if (newLayer != NULL)
            {
               newLayer->setElectricalStackNumber(layer->getElectricalStackNumber());
               newLayer->setPhysicalStackNumber(layer->getPhysicalStackNumber());
               newLayer->setArtworkStackNumber(layer->getArtworkStackNumber());
               newLayer->setLayerType(layer->getLayerType());
               newLayer->setColor(layer->getColor());
               newLayer->setOriginalColor(layer->getOriginalColor()); 
               newLayer->setLayerFlags(0);

               LayerStruct* newMirrorLayer = doc->Add_Layer(newMirrorLayerName);
               if (newMirrorLayer != NULL)
               {
                  newLayer->setMirroredLayerIndex(newMirrorLayer->getLayerIndex());
                  newMirrorLayer->setMirroredLayerIndex(newLayer->getLayerIndex());
               }

               doc->CopyAttribs(&newLayer->getAttributesRef(), layer->getAttributesRef());

               int menLyrKey = doc->RegisterKeyWord("Mentor_Layer", 0, valueTypeString);
			      newLayer->setAttrib(doc->getCamCadData(), menLyrKey, valueTypeString, (void*)layerName.GetBuffer(0), attributeUpdateOverwrite, NULL);

               data->setLayerIndex(newLayer->getLayerIndex());
            }
         }
      }

      for (POSITION pos = m_boardInsertList.GetHeadPosition(); pos != NULL;)
      {
         DataStruct* data = m_boardInsertList.GetNext(pos);
         if (data != NULL && data->getDataType() == dataTypeInsert)
         {
            data->getInsert()->setBlockNumber(m_derivedBlock->getBlockNumber());
         }
      }
   }
}

bool CBoardLevelGeometry::checkForContentOnSpecialLayer()
{
   bool retval = false;
   for (CDataListIterator dataList(*m_originalBlock); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();
      LayerStruct* layer = doc->getLayerAt(data->getLayerIndex());
      if (layer == NULL)
         continue;

      CString layerName = doc->getLayerAt(data->getLayerIndex())->getName();
      if (layerName.GetLength() > 2 && (layerName.Right(2) == "_1" || layerName.Right(2) == "_2"))
      {
         retval = true;
         break;
      }
   }

   return retval;
}

//-----------------------------------------------------------------------------
// CBoardLevelGeometryMap
//-----------------------------------------------------------------------------
CBoardLevelGeometryMap::CBoardLevelGeometryMap()
   : m_boardLevelGeometryMap(nextPrime2n(20),true)
{
}

CBoardLevelGeometryMap::~CBoardLevelGeometryMap()
{
   m_boardLevelGeometryMap.empty();
}

void CBoardLevelGeometryMap::addBoardLevelInsert(DataStruct* boardLeverInsertData)
{
   if (boardLeverInsertData == NULL || boardLeverInsertData->getDataType() != dataTypeInsert)
      return;

   BlockStruct* block = doc->getBlockAt(boardLeverInsertData->getInsert()->getBlockNumber());
   if (block == NULL)
      return;

   CBoardLevelGeometry* boardLevelGeometry = NULL;
   if (!m_boardLevelGeometryMap.Lookup(block->getName(), boardLevelGeometry))
   {
      boardLevelGeometry = new CBoardLevelGeometry(block->getName(), block);
      m_boardLevelGeometryMap.SetAt(block->getName(), boardLevelGeometry);
   }
   boardLevelGeometry->addBoardLevelInsert(boardLeverInsertData);
}

void CBoardLevelGeometryMap::derivedSpecialVersionOfBoardLevelGeometries()
{
   for (POSITION pos = m_boardLevelGeometryMap.GetStartPosition(); pos != NULL;)
   {
      CString key;
      CBoardLevelGeometry* boardLevelGeometry = NULL;
      m_boardLevelGeometryMap.GetNextAssoc(pos, key, boardLevelGeometry);
      if (boardLevelGeometry != NULL)
      {
         boardLevelGeometry->derivedSpecialVersionOfBoardLevelGeometry();
      }
   }
}

void CBoardLevelGeometryMap::empty()
{
   m_boardLevelGeometryMap.empty();
}

/******************************************************************************
* CMentorPinRef
*/
CMentorPinRef::CMentorPinRef(int sysPageUnit)
: CPinRef(sysPageUnit)
{  
   m_displaymode = Mode_Physical;
   m_rightreadingflag = false;
}

VerticalPositionTag CMentorPinRef::getTextVerticalAlignment(CString verticalPos)
{
   if(verticalPos.IsEmpty())
      return verticalPositionCenter;

   switch(verticalPos.GetAt(0))
   {
   case 'T':
      return verticalPositionTop;
   case 'B':
      return verticalPositionBottom;
   case 'C':
      return verticalPositionCenter;
   }

   return verticalPositionCenter;
}

HorizontalPositionTag CMentorPinRef::getTextHorizontalAlignment(CString horizontalPos)
{
   if(horizontalPos.GetLength() < 2)
      return horizontalPositionCenter;

   switch(horizontalPos.GetAt(1))
   {
   case 'L':
      return horizontalPositionLeft;
   case 'R':
      return horizontalPositionRight;
   case 'C':
      return horizontalPositionCenter;
   }

   return horizontalPositionCenter;
}

PinRefDisplayMode CMentorPinRef::getDisplayMode(CString displaymode)
{
   if(displaymode.IsEmpty())
      return Mode_Unknown;

   switch(displaymode.GetAt(0))
   {
   case 'P':
      return Mode_Physical;
   case 'L':
      return Mode_Logical;
   case 'U':
      return Mode_Userdef;
   case 'H':
      return Mode_Hidden;
   }

   return Mode_Unknown;
}

/* 
ex. # PIN  Reference-Pin_no  Properties
PIN CR1-K (PINREF,"IN,-0.03305,-0.06515,270,CC,0.025,0.025,0.005,std,0,U,v")

//Definition of PINREF in BoardStation Importer's pins.pins
Unit (One of the following: IN, CM, MM, ML, TM, TN) 
X,Y coordinate location 
Orientation 
Text justification (Vertical+Horizontal)(T-Top, C-Centre, B-Bottom, L-Left, R-Right) 
Text height 
Character width 
Stroke width 
Font (Font name) 
Right-reading flag (If zero, no right reading; if one, right reading) 
Pinref Display mode (P-Physical, L-Logical, U-User defined, H-Hidden) 
User defined name (if exists) 
Pinref layer (Only if overriden on multiple layers) 
*/
bool CMentorPinRef::setPinRef(CString CompName, CString PinNumber, CStringArray &propertyArray)
{
   setComponentName(CompName);
   setPinNumber(PinNumber);
   
   int ParamNum = propertyArray.GetCount();
   if( ParamNum > PinRefField_DisplayMode)
   {
      // Position, Size
      double scalefactor = getScaleFactor(propertyArray[PinRefField_Unit]);
      setX(atof(propertyArray[PinRefField_X]) * scalefactor);
      setY(atof(propertyArray[PinRefField_Y]) * scalefactor);
      setWidth(atof(propertyArray[PinRefField_Height]) * scalefactor);
      setHeight(atof(propertyArray[PinRefField_Width]) * scalefactor);
      setPen(atof(propertyArray[PinRefField_Pen]) * scalefactor);

      //Rotation
      setRotationType(Rotation_RightReading);
      m_rightreadingflag = atoi(propertyArray[PinRefField_RightReading])?true:false;       
      setRotationDegrees(atof(propertyArray[PinRefField_Orientation]));

      // Alignment
      setVerticalPosition(getTextVerticalAlignment(propertyArray[PinRefField_Align]));
      setHorizontalPosition(getTextHorizontalAlignment(propertyArray[PinRefField_Align]));

      //Font
      setFontName(propertyArray[PinRefField_FontName]);      
      m_displaymode = getDisplayMode(propertyArray[PinRefField_DisplayMode]);
      
      // Set diaplay mode
      bool m_isVisible = isVisible();
      CString m_UserdefString = "";
      switch(m_displaymode)
      {
      case Mode_Physical:  //Layout Pin name.
      case Mode_Logical:   //Schematic Pin name.
         m_UserdefString = PinNumber;
         break;
      case Mode_Userdef:
         if (ParamNum > PinRefField_DefName)
            m_UserdefString = propertyArray[PinRefField_DefName];
         break;
      case Mode_Hidden:
         m_isVisible = false;
         break;
      }

      setVisible(m_isVisible);
      setUserdefString(m_UserdefString);

      //layer name (?)
      setLayerName((ParamNum > PinRefField_Layer)?propertyArray[PinRefField_Layer]:"");
      return true;
   }

   return false;
}

// When force right reading text is set, text will always appear right side up (From Documentation) 
// This only affects text that is orentated between 91 degrees and 270 degrees 
// (final, fully resolved angles is considered right reading) 
// because text outside this range displays right side up by default. 
// So, If resolved angle is outside range 271-90 then subtract 180.
double CMentorPinRef::getAdjustedAngleDegree(double componentAngle, double pinAngle)
{
   double refAngle = getRotationDegrees();
   double fullAngle = componentAngle + pinAngle + refAngle;

   //need to consider angles of component, pin and refname
   if(m_rightreadingflag && (fullAngle > 90 && fullAngle < 271))
      refAngle = refAngle - 180;

   return refAngle;
}

double CMentorPinRef::getAdjustedAngleRadians(double componentAngle, double pinAngle)
{
   double refAngle = getRotationDegrees();
   double fullAngle = componentAngle + pinAngle + refAngle;

   //need to consider angles of component, pin and refname
   if(m_rightreadingflag && (fullAngle > 90 && fullAngle < 271))
      refAngle = refAngle - 180;

   return degreesToRadians(refAngle);
}
