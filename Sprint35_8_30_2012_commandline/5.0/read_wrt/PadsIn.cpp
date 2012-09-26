// $Header: /CAMCAD/4.6/read_wrt/PadsIn.cpp 125   6/11/07 5:27p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"
#include "geomlib.h"
#include "pcbutil.h"
#include "pcbutil2.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "logwrite.h"
#include "lyrmanip.h"
#include "menlib.h"  // just for date test
#include "padsin.h"
#include "Attribute.h"
#include "Variant.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)

#define LineStatusUpdateInterval 200

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg  *progress;
extern char *testaccesslayers[];

static void InitMem();
static void FreeMem();
static int load_padssettings(const CString fname);
static void AssignPanelOutline();
static int do_assign_layers();
static int set_boundary_to_hidden(BlockStruct *Block);
static int loop_pads(List *c_list, int siz_of_command);
static char *get_nextline(char *string,int n,FILE *fp);
static int pads_skip();
static int nextsect(char *n);
static double cnv_unit(double x, char unitflag);
static double cnv_unit(double x);
static DataStruct *get_pieces(double x, double y, const char *name,const char *linetype, const char *shapename);
static int get_text(double x, double y, const char *name, double *height);
static int get_label(CAttributes** map, double x, double y, const char *name);
static int do_padstacklayers();
static void GetSilkcreenLayers(CString &silkscreenTop, CString &silkscreenBottom);
static int do_text_layer();
static void processAssemblyOptions(CCEtoODBDoc *doc, FileStruct *pcbFile);
static void assignUserMirrorLayers();

static int get_ptypeptr(const char *n);

static TypeStruct * FindPadsPartType(FileStruct *pcbfile, CString partname);

static int PageUnits;
static FileStruct *file = NULL;
static FILE *ferr;
static FILE *ifp;                            /* File pointers.    */
static int display_error = 0;
static int display_log;
static CFileReadProgress*  fileReadProgress = NULL;

static CCEtoODBDoc *doc;
static char ifp_line[MAX_LINE];

static int PushTok = FALSE;
static int eof_found;
static char unit_flag = ' ';           /* I = inch M = metric A = mil B = basic */
static char heightunit_flag = ' ';     /* I = inch M = metric A = mil B = basic */
static int title_found = FALSE;                                       
static int file_version = 2;
static int layer_mode = 30;
static int layer_assign = FALSE;    // since PADS Version 3 there is the complete Layerassign defined.
static PADSAdef layer_attr[MAX_LAYERS];    /* Array of layers from pdif.in   */
static int layer_attr_cnt;

static CPartArray partarray;     // this is the device - mapping
static int partcnt;

static CPourArray pourarray;  
static int pourcnt;

static CPincopperArray pincopperarray; 
static int pincoppercnt;

static CAttrArray attrarray;  
static int attrcnt;

static CMiscLayerArray misclayerarray; 
static int misclayercnt;

static CViadefArray viadefarray; 
static int viadefcnt;

static CPadsignoredArray padsignoredarray;   
static int padsignoredcnt;

static CCompPinignoredArray comppinignoredarray;   
static int comppinignoredcnt;

static CTerminalArray terminalarray;
static int terminalcnt;

static CPaddefArray paddefarray;
//static int paddefcnt;

static int unroutelayer;

static int maxlayer; // number of layers
static int drilllayernum;
static int max_copper_layer;

static CString compoutline[30];  // allow upto 30 different layers for component outline
static int compoutlinecnt;

static PADSRefnamesize refnamesize;

static char keep_unrout_layer;
static bool ignoreStrangePadshapes;

static char non_electrical_pins;
static int elim_double_vias;

static CString unnamednet[30];
static int unnamednetcnt;

static CString panelOutlineLayer;

static int ComponentSMDrule;     // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static CAttribmapArray attribmaparray;
static int attribmapcnt = 0;
static int PCB_SECTION_FOUND;

static bool polycrack = true;

static CAttrCopyList AttrCopyList;

static CMapStringToString UserMirrorLayerMap; // from .mirror_layer in settings file

static CTypeChanger MakeInto_CommandProcessor;

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
   if ((s_inputFileLineNumber % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
   {
      fileReadProgress->updateProgress();
   }

   ++s_inputFileLineNumber;

   //if (s_inputFileLineNumber == 1674)
   //{
   //   int iii = 3;
   //}
}

static void PropagateDeviceAttributes(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
	if (doc != NULL && pcbFile != NULL && pcbFile->getBlock() != NULL)
	{
		WORD dollarDeviceAttrKW = (WORD)doc->IsKeyWord(ATT_TYPELISTLINK, 0);

	   BlockStruct *pcbBlock = pcbFile->getBlock();

      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct *data = insertIterator.getNext();

			if (data->getAttributesRef() != NULL)
			{
				Attrib *devAttr = NULL;
				if (data->getAttributesRef()->Lookup(dollarDeviceAttrKW, devAttr) && devAttr != NULL)
				{
#if CamCadMajorMinorVersion > 406  //  > 4.6
               TypeStruct *devType = FindType(pcbFile, devAttr->getStringValue());
#else
					TypeStruct *devType = FindType(pcbFile, doc->ValueArray[devAttr->getStringValueIndex()]);
#endif
					if (devType != NULL && devType->getAttributesRef() != NULL)
					{
						data->getAttributesRef()->CopyNew(*devType->getAttributesRef());
					}
				}
			}
      }
	}
}

/******************************************************************************
* ReadPADS
*/
void ReadPADS(const char *fullPath, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   doc = Doc;
   PageUnits = pageunits;

   // CAD file
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(fullPath, drive, dir, fname, ext);
   CString fileName = fname;
   fileName += ext;

   ifp = fopen(fullPath, "rt");
   if (!ifp)
   {
      ErrorMessage(fullPath, "Error opening file");
      return;
   }

   if (!getApp().SilentRunning)
      fileReadProgress = new CFileReadProgress(ifp);

   // log file
   CString padsLogFile = GetLogfilePath("pads.log");
   ferr = fopen(padsLogFile, "wt");
   if (!ferr)
   {
      ErrorMessage("Error open file", padsLogFile);
      return;
   }
   log_set_filename(fullPath);

   InitMem();

   // init globals
   display_error = FALSE;
   display_log = FALSE;
   PCB_SECTION_FOUND = FALSE;
   resetInputFileLineNumber();
   PushTok = FALSE;
   eof_found = FALSE;
   title_found = FALSE;
   unroutelayer = 0;
   max_copper_layer = 0;

   file = Graph_File_Start(fileName, fileTypePadsLayout);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(blockTypePcb);

   Graph_Level("0", "", TRUE); // init floating layer
   drilllayernum = Graph_Level("DRILLHOLE", "", FALSE);

   CString settingsFile( getApp().getImportSettingsFilePath("pads.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nPADS Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_padssettings(settingsFile);

   int res = loop_pads(start_lst, SIZ_START_LST);

   generate_NCnet(doc, file); // this function generates the NC (non connect) net.

   if (res > -1)
   {
      COperationProgress *operationProgress = getApp().SilentRunning ? NULL : new COperationProgress;

      if (operationProgress != NULL)
         operationProgress->updateStatus("Optimizing Database",0);

      //progress->SetStatus("Optimizing Database");
      double accuracy = get_accuracy(doc);

      if (!elim_double_vias)  
      {
         //progress->SetStatus("Elim Vias");
         EliminateDuplicateVias(doc, file->getBlock());
      }
   
      // this can happen on strange via placements.
      EliminateSinglePointPolys(doc);                
      BreakSpikePolys(file->getBlock()); 

      set_boundary_to_hidden(file->getBlock());

		// Case 1779, the "if" is temporary, to make a release build demo for QA
		// I expect to be backing this out in near future.
		// Default is "true", which gives backward compat behavior.
		// Can be turned off with secret control in pads.in, .POLYCRACK N
      if (polycrack)
			Crack(doc, file->getBlock(), TRUE, operationProgress);   

      EliminateOverlappingTraces(doc, file->getBlock(), TRUE, accuracy);               

      // here do padstack layers
      do_padstacklayers();

      if (unroutelayer)
      {
         int layerNum = Graph_Level("UNROUT", "", FALSE);
         LayerStruct *layer = doc->getLayerArray()[layerNum];
         layer->setVisible(false); // do not show
      }
   
      if (!keep_unrout_layer)
      {
         int layerNum = Graph_Level("UNROUT", "", FALSE);
         LayerStruct *layer = doc->getLayerArray()[layerNum];
         DeleteLayer(doc, layer, TRUE);
      }

		AssignPanelOutline();

      // here assign other layers
      do_assign_layers();
		do_text_layer();
      assignUserMirrorLayers();

      update_smdpads(doc);
      update_smdrule_geometries(doc, ComponentSMDrule);
      update_smdrule_components(doc, file, ComponentSMDrule,operationProgress);
      RefreshInheritedAttributes(doc, attributeUpdateOverwrite);  // smd pins are after the pin instance is done.
		PropagateDeviceAttributes(doc, file);

      if (operationProgress != NULL)
         operationProgress->updateStatus("Optimizing Padstacks",0);
      OptimizePadstacks(doc, pageunits, 0,operationProgress); // this can create unused blocks

      generate_PADSTACKACCESSFLAG(doc, 1);

      MakeInto_CommandProcessor.Apply(doc, file);  // Do before generate_PINLOC, it may add pins (for testpoints)

      generate_PINLOC(doc, file, 1);   // this function generates the PINLOC argument for all pins.

		// Case 1817
		doc->OnToolsNormalizeBottomBuildGeometries();

      // Case 2033
      processAssemblyOptions(doc, file);

      // Case dts0100375577
      AttrCopyList.Apply(doc, file);

      doc->purgeUnusedBlocks(); // use after optimize_PADSTACKS

      if (operationProgress != NULL)
      {
         delete operationProgress;
         operationProgress = NULL;
      }

   }  
   else
   {
		// aborted due to unrecoverable syntax error
      ErrorMessage("Unrecoverable Syntax Error in PADS ASCII file!", "PADS File Load Aborted!");
   }

   FreeMem();
   MakeInto_CommandProcessor.Reset();

   fclose(ifp);
   fclose(ferr); 

   if (!PCB_SECTION_FOUND)
   {
      //*PCB*        GENERAL PARAMETERS OF THE PCB DESIGN
      ErrorMessage("The *PCB section is missing. This section contains important information about the technology and layers.\n\
The imported file may not have the correct electrical layer assignment!",
            "Missing *PCB* GENERAL PARAMETERS OF THE PCB DESIGN");
   }

   if (display_error && display_log)
      Logreader(padsLogFile);
}

/******************************************************************************
* InitMem
*/
static void InitMem()
{
   partarray.SetSize(100,100);
   partcnt = 0;

   pourarray.SetSize(100,100);
   pourcnt = 0;

   pincopperarray.SetSize(100,100);
   pincoppercnt = 0;

   attrarray.SetSize(100,100);
   attrcnt = 0;

   misclayerarray.SetSize(100,100);
   misclayercnt = 0;

   terminalarray.SetSize(100,100);
   terminalcnt = 0;

   viadefarray.SetSize(100,100);
   viadefcnt = 0;

   padsignoredarray.SetSize(100,100);
   padsignoredcnt = 0;

   comppinignoredarray.SetSize(100,100);
   comppinignoredcnt = 0;

   paddefarray.SetSize(0,100);
   //paddefcnt = 0;
}

/******************************************************************************
* FreeMem
*/
static void FreeMem()
{
	delete fileReadProgress;
	fileReadProgress = NULL;

   pincopperarray.RemoveAll();
	
	int i=0;
   for (i=0;i<pourcnt;i++)
      delete pourarray[i];
   pourarray.RemoveAll();

   for (i=0;i<attrcnt;i++)
      delete attrarray[i];
   attrarray.RemoveAll();

   for (i=0;i<misclayercnt;i++)
      delete misclayerarray[i];
   misclayerarray.RemoveAll();

   for (i=0;i<attribmapcnt;i++)
      delete attribmaparray[i];
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   for (i=0;i<partcnt;i++)
      delete partarray[i];
   partarray.RemoveAll();

   for (i=0;i<viadefcnt;i++)
      delete viadefarray[i];
   viadefarray.RemoveAll();

   for (i=0;i<padsignoredcnt;i++)
      delete padsignoredarray[i];
   padsignoredarray.RemoveAll();

   for (i=0;i<comppinignoredcnt;i++)
      delete comppinignoredarray[i];
   comppinignoredarray.RemoveAll();

   terminalarray.RemoveAll();

   paddefarray.empty();   // pad with drill and padstack array

   AttrCopyList.RemoveAll();
   UserMirrorLayerMap.RemoveAll();
}

/******************************************************************************
* clean_rot
*/
static double clean_rot(double rot)
{
   rot = round(rot * 10) / 10;
   while (rot < 0)      
      rot += 360;
   while (rot >= 360)
      rot -= 360;

   return rot;
}

/******************************************************************************
* char *get_attribmap
*/
static const char *get_attribmap(const char *c)
{
   for (int i=0; i<attribmapcnt; i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   return c;
}

/****************************************************************************
* CheckName
*/
static int CheckName(CString *SS,CString *BN)
{
   int loc, loc2, pos, retVal;
	   
   retVal = 0;
   loc = -1;
   loc2 = -1;
   pos = -1;
	
   CString SearchStr = *SS;
   CString BlockName = *BN;
   CString subStr = "";

   CString arg1, arg2;

   SearchStr.MakeLower();
   BlockName.MakeLower();

   loc = SearchStr.Find('*');
   if (loc == -1) 
   {
	   // No '*' in SearchString
	   if (SearchStr.Compare(BlockName) == 0)
		   return 1;
   }
   else
   {
      // if * is first char
      if ((loc == 0) && (SearchStr.GetLength() > 1))
	  {
         // get location of next '*'
         SearchStr = SearchStr.Right(SearchStr.GetLength() - 1);
         loc2 = SearchStr.Find('*');
		 if (loc2 > -1)
		 {
		    // another wildcard found
		    if (loc2 == 0)
			{
				return CheckName(&SearchStr, &BlockName);
			}
			else
			{
               // need to search for subStr in BlockName
			   subStr = SearchStr.Left(loc2);
			   SearchStr = SearchStr.Right(SearchStr.GetLength() - loc2);
			   pos = BlockName.Find(subStr);
			   if (pos > -1) // found subStr
			   {
			      arg1 = SearchStr.GetString();
			      arg2 = BlockName.Right(BlockName.GetLength() - (pos + subStr.GetLength()));
				  return CheckName(&arg1, &arg2);
			   }
			}
		 }
		 else
		 {
            // no more wildcards, check against right of BlockName
		    if (SearchStr.Compare(BlockName.Right(SearchStr.GetLength())) == 0)
				return 1;
		 }

	  }
	  else if ((loc > 0) && (SearchStr.GetLength() > 1))
	  {
         // if * is not 1st char
         // check against left of blockname, SearchString does not start with '*'
         subStr = SearchStr.Left(loc);
         SearchStr = SearchStr.Right(SearchStr.GetLength() - loc);
         if (subStr.Compare(BlockName.Left(subStr.GetLength())) == 0)
         {
			arg1 = SearchStr.GetString();
            arg2 = BlockName.Right(BlockName.GetLength() - (subStr.GetLength()));
            return CheckName(&arg1, &arg2);
         }
	  }
	  else if ((loc == 0) && (SearchStr.GetLength() == 1))
		  return 1; // in recursive call last '*' match with remaining BlockName



   }
   return retVal;
}

/****************************************************************************
* is_geom
*/
static int is_geom(const char *s, int geomtype)
{
   for (int i=0; i<attrcnt; i++)
   {
      CString SearchString, BlockName;
	  SearchString = attrarray[i]->name;
	  BlockName = s;
	  if (attrarray[i]->atttype == geomtype && CheckName(&SearchString, &BlockName))
		  return 1;
   }
   return 0;
}

/******************************************************************************
* is_attrib
*/
static int is_attrib(const char *s,int atttype)
{
   for (int i=0; i<attrcnt; i++)
   {
      if (attrarray[i]->atttype == atttype && !STRICMP(attrarray[i]->name,s))
         return 1;
   }

   return 0;
}

/******************************************************************************
* get_mappeddeviceindex
*/
static DeviceTypeTag get_mappeddeviceindex(const char *s)
{
   for (int i=0; i<attrcnt; i++)
   {
      if (attrarray[i]->atttype == DEVICE_ATT && !STRICMP(attrarray[i]->name,s))
         return attrarray[i]->devicetype;
   }

   return deviceTypeUnknown;
}

/******************************************************************************
* update_attr
*/
static int update_attr(char *lp,int typ)
{
   PADSAttr *attr = new PADSAttr;
   attrarray.SetAtGrow(attrcnt, attr);  
   attrcnt++;  
   attr->name = lp;
   attr->atttype = typ;
   attr->devicetype = deviceTypeUndefined;
   return attrcnt-1;
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
//   return   LAYTYPE_UNKNOWN; 
//}

/******************************************************************************
* load_padssettings
*/
static int load_padssettings(const CString fname)
{
   FILE *fp;
   char line[255];
   char *lp;

   unnamednetcnt = 0;
   compoutlinecnt = 0;
   refnamesize.height = 0.07 * Units_Factor(UNIT_INCHES, PageUnits);
   refnamesize.linewidth = 0;
   non_electrical_pins = 'n';
   keep_unrout_layer = FALSE;
   ignoreStrangePadshapes = TRUE;
   elim_double_vias = FALSE;
   ComponentSMDrule = 0;
   layer_attr_cnt = 0;
   panelOutlineLayer = "";
   AttrCopyList.RemoveAll();
   UserMirrorLayerMap.RemoveAll();
   MakeInto_CommandProcessor.Reset();

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "Pads Settings", MB_OK | MB_ICONHAND);
      return 0;
   }

   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;


      if (lp[0] == '.')
      {
         // here do it.
         if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            ComponentSMDrule = atoi(lp);
         }
         else if (!STRICMP(lp, ".NON_ELECTRICAL_PINS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            non_electrical_pins = tolower(lp[0]);           
         }
         else if (!STRICMP(lp, ".UNNAMEDNET"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 

            CString unnamedNet = lp;
            unnamedNet.TrimLeft();
            unnamedNet.TrimRight();

            if (unnamednetcnt < 30)
            {
               unnamednet[unnamednetcnt] = unnamedNet;
               unnamednetcnt++;
            }
            else
            {
               fprintf(ferr, "Too many unnamed nets defined.\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString attr1 = lp;
            attr1.MakeUpper();
            attr1.TrimLeft();
            attr1.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString attr2 = lp;
            attr2.MakeUpper();
            attr2.TrimLeft();
            attr2.TrimRight();

            PADSAttribmap  *attribMap = new PADSAttribmap;
            attribmaparray.SetAtGrow(attribmapcnt++, attribMap);
            attribMap->attrib = attr1;
            attribMap->mapattrib = attr2;
         }
         else if (!STRICMP(lp, ".COMPOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString compOutline = lp;
            compOutline.TrimLeft();
            compOutline.TrimRight();

            if (compoutlinecnt < 30)
            {
               compoutline[compoutlinecnt] = compOutline;
               compoutlinecnt++;
            }
            else
            {
               fprintf(ferr, "Too many compoutlines defined.\n");
               display_error++;
            }

         }
         else if (!STRCMPI(lp, ".DISPLAYLOG"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "y", 1))
               display_log = TRUE;
         }
         else if (!STRCMPI(lp, ".POLYCRACK"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "n", 1))
               polycrack = false;
         }
         else if (!STRCMPI(lp, ".IGNORESTRANGEPADSHAPES"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "n", 1))
               ignoreStrangePadshapes = FALSE;
         }
         else if (!STRCMPI(lp, ".UNROUTLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "y", 1))
               keep_unrout_layer = TRUE;
         }
         else if (!STRICMP(lp, ".LOGICDEVICETYPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            int ptr = update_attr(lp, DEVICE_ATT);
            PADSAttr *attr =  attrarray.GetAt(ptr);

            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;

            attr->devicetype = stringToDeviceTypeTag(lp);
            if (attr->devicetype == deviceTypeUnknown || attr->devicetype == deviceTypeUndefined)
            {
               fprintf(ferr, "Unknown Devicetype [%s] in [%s] at %ld\n", lp, fname, getInputFileLineNumber());
               fprintf(ferr, "Allowed Devicetypes :\n");
               fprintf(ferr, "\t%s\n", getDevceTypeListAsDelimitedValueString(",\n\t"));
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".FIDUCIALATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, FID_ATT);
         }
         else if (!STRICMP(lp, ".FIDUCIALGEOM"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, FID_GEOM);
         }
         else if (!STRICMP(lp, ".HEIGHTATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp,HEIGHT_ATT);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if( islower( lp[0] ) )
               heightunit_flag =  _toupper( lp[0] );
            else
               heightunit_flag = lp[0];
         }
         else if (!STRICMP(lp, ".SMDATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, SMD_ATT);
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue; 
            CString olayer = lp;
            olayer.TrimLeft();
            olayer.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);
            if (layer_attr_cnt < MAX_LAYERS)
            {
               layer_attr[layer_attr_cnt].layerName = olayer;
               layer_attr[layer_attr_cnt].layerType = laytype;
               layer_attr_cnt++;
            }
            else
            {
               fprintf(ferr, "Too many layer attributes\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".MIRRORLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString lay1 = lp;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString lay2 = lp;

            UserMirrorLayerMap.SetAt(lay1, lay2);
         }
         else if (!STRICMP(lp, ".PANELOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            panelOutlineLayer = lp;
         }
         else if (!STRICMP(lp, ".TOOLINGGEOM"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, TOOL_GEOM);
         }
         else if (!STRICMP(lp, ".COPY_ATT_FROM_TO"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString from = lp;
                        
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString to = lp;

            AttrCopyList.AddAttrPair(from, to);
         }
         else if (MakeInto_CommandProcessor.ParseCommand(line)) // send whole text line, not just command
         {
            // Already did what it needed to do
         }

      }
   }

   fclose(fp);
   return 1;
}

///////////////////////////////////////////////////////////////////////

bool CAttrCopyList::AddAttrPair(CString from, CString to)
{
   if (!from.IsEmpty() && !to.IsEmpty())
   {
      this->Add( new CAttrCopyPair(from, to) );
      return true;
   }

   return false;
}

//----------------------------------------------------------------------

bool CAttrCopyList::Apply(CCEtoODBDoc *doc, FileStruct *file)
{
   // CAttrCopyLIst - List of from->to attrib names for copying.
   // Apply to all PCB Component inserts.
   // If an insert has teh "from" attrib, then create the "to" attrib
   // using teh same value. If "to" attrib already exists, overwrite it.
   // If "from" attrib does not exist, do no create the "to" attrib.


   if (doc != NULL && file != NULL)
   {
      BlockStruct *pcbBlock = file->getBlock();
      if (pcbBlock != NULL)
      {
         POSITION pos = pcbBlock->getHeadDataInsertPosition();
         while (pos != NULL)
         {
            DataStruct *data = pcbBlock->getNextDataInsert(pos);

            if (data->isInsertType(insertTypePcbComponent) && data->getAttributesRef() != NULL)
            {
               for (int i = 0; i < AttrCopyList.GetCount(); i++)
               {
                  CAttrCopyPair *acp = AttrCopyList.GetAt(i);
                  CString fr = acp->GetFromName();
                  CString to = acp->GetToName();
                  Attrib *attrib =  is_attvalue(doc, data->getAttributesRef(), fr, 0);
                  CString value;
                  if (attrib != NULL)
                  {
                     switch (attrib->getValueType())
                     {
                     case VT_STRING:
                     case VT_EMAIL_ADDRESS:
                     case VT_WEB_ADDRESS:
#if CamCadMajorMinorVersion > 406  //  > 4.6
                        value = attrib->getStringValue();
#else
                        value = doc->ValueArray[attrib->getStringValueIndex()];
#endif
                        break;
                     case VT_UNIT_DOUBLE:
                        {
                           int decimals = GetDecimals(doc->getSettings().getPageUnits());
                           value.Format("%+.*lf", decimals, attrib->getDoubleValue());
                        }
                        break;
                     case VT_DOUBLE:
                        value.Format("%+.2lf", attrib->getDoubleValue());
                        break;   
                     case VT_INTEGER:
                        value.Format("%+d", attrib->getIntValue());
                        break;
                     case VT_NONE:
                        value = "";
                        break;
                     }

                     doc->SetUnknownAttrib(&data->getAttributesRef(), to, value, attributeUpdateOverwrite, NULL); 
                  }
               }
            }
         }
      }
      return true;
   }
   return false;
}

///////////////////////////////////////////////////////////////////////

/******************************************************************************
* get_padslayertype
*/
static int get_padslayertype(int ptr)
{
   PADSMiscLayer *miscLayer = misclayerarray[ptr];       
   if (!miscLayer->layer_type.CompareNoCase("ROUTING"))
   {
      if (miscLayer->stacknumber == 1)
         return LAYTYPE_SIGNAL_TOP;
      else if (miscLayer->stacknumber == maxlayer)
         return LAYTYPE_SIGNAL_BOT;
      else
         return LAYTYPE_UNKNOWN; // this could already been marked as a plane
   }
   else if (!miscLayer->layer_type.CompareNoCase("SOLDER_MASK"))
   {
      // here need to find if it is top or bottom
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_MASK_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_MASK_BOTTOM;
      else
         return LAYTYPE_MASK_ALL;
   }
   else if (!miscLayer->layer_type.CompareNoCase("PASTE_MASK"))
   {
      // here need to find if it is top or bottom;
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_PASTE_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_PASTE_BOTTOM;
      else
         return LAYTYPE_PASTE_ALL;
   }
   else if (!miscLayer->layer_type.CompareNoCase("SILK_SCREEN"))
   {
      // here need to find if it is top or bottom
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_SILK_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_SILK_BOTTOM;
      else
         return LAYTYPE_UNKNOWN;
   }
   else if (!miscLayer->layer_type.CompareNoCase("DRILL"))
   {
      return LAYTYPE_DRILL;
   }
   else if (!miscLayer->layer_type.CompareNoCase("ASSEMBLY"))
   {
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_BOTTOM;
      else
         return LAYTYPE_UNKNOWN;
   }
   else
   {
      return LAYTYPE_UNKNOWN;
   }
}

/******************************************************************************
* AssignPanelOutline
*/
static void AssignPanelOutline()
{
	LayerStruct* layer = doc->FindLayer_by_Name(panelOutlineLayer);
	if (layer == NULL)
		return;

   for (int i=0; i< doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);      
      if (block == NULL)
         continue;

      POSITION dataPos = block->getHeadDataPosition();
      while (dataPos)
      {
         DataStruct* data = block->getNextData(dataPos);
			if (data == NULL)
				continue;

         if (data->getDataType() == T_POLY && data->getLayerIndex() == layer->getLayerIndex())
         {
            data->setGraphicClass(graphicClassPanelOutline);           
         }
      }
   }

}

static int getLayerStackNumber(CString layerName)
{
   for (int i = 0; i < misclayercnt; i++)
   {
      PADSMiscLayer *miscLayer = misclayerarray[i];
      if (layerName.Compare(miscLayer->layer_name) == 0)
         return miscLayer->stacknumber;
   }

   return -1; // not found
}

static void assignCadLayerNumberComment(CString layerName)
{
   LayerStruct *layer = doc->FindLayer_by_Name(layerName);
   int cadLayerNumber = getLayerStackNumber(layerName);
   if (layer != NULL && cadLayerNumber > 0)
   {
      CString buf;
      buf.Format("%d", cadLayerNumber);
      layer->setComment(buf);
   }
}

/******************************************************************************
*/
static void assignUserMirrorLayers()
{
   // Assign mirror pairs from .mirror_layer commands in settings file.
   // One of the layers named in the .mirror_layer command must already
   // exist, otherwise command is ignored. I.e. we don't create entirely
   // unused layers.

   POSITION pos = UserMirrorLayerMap.GetStartPosition();
   while (pos != NULL)
   {
      CString lay1name;
      CString lay2name;
      UserMirrorLayerMap.GetNextAssoc(pos, lay1name, lay2name);

      if (!lay1name.IsEmpty() && !lay2name.IsEmpty())
      {
         // If one or the other exist then set up the mirror pair
         LayerStruct *lay1 = doc->FindLayer_by_Name(lay1name);
         LayerStruct *lay2 = doc->FindLayer_by_Name(lay2name);

         if (lay1 != NULL && lay2 != NULL)
         {
            // both exist, just mirorr them
            Graph_Level_Mirror(lay1name, lay2name, "");
         }
         else if (lay1 != NULL)
         {
            // Lay1 exists but not Lay2, create Lay2
            Graph_Level_Mirror(lay1name, lay2name, "");
            lay2 = doc->FindLayer_by_Name(lay2name);
            if (lay2 != NULL) // jic
            {
               lay2->setComment("Created by .MIRRORLAYER");
               lay2->setLayerType(getOppositeSideLayerType(lay1->getLayerType()));
            }
         }
         else if (lay2 != NULL)
         {
            // Lay2 exists but not Lay1, create Lay1
            Graph_Level_Mirror(lay1name, lay2name, "");
            lay1 = doc->FindLayer_by_Name(lay1name);
            if (lay1 != NULL) // jic
            {
               lay1->setComment("Created by .MIRRORLAYER");
               lay1->setLayerType(getOppositeSideLayerType(lay2->getLayerType()));
            }
         }
      }
   }
}

static void AssignPadsDotInLayerTypes()
{
   for (int i=0; i<doc->getLayerCount(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

      if (layer == NULL)
      {
         continue;
      }

      for (int j=0; j<layer_attr_cnt; j++)
      {
         if (layer->getName().CompareNoCase(layer_attr[j].layerName) == 0)
         {
            layer->setLayerType(layer_attr[j].layerType);
            break;
         }
      }
   }
}

/******************************************************************************
* do_assign_layers
*  make layers more understandable for PADS users.
*/
static int do_assign_layers()
{
   LayerStruct *layer;

   layer = doc->FindLayer(drilllayernum);

   if (layer != NULL)
   {
      layer->setComment("Drill");
      layer->setLayerType(LAYTYPE_DRILL);
   }

   layer = doc->FindLayer_by_Name("BOARD_OUTLINE");

   if (layer != NULL)
   {
      layer->setComment("Board_Outline");
      layer->setLayerType(LAYTYPE_BOARD_OUTLINE);
   }

   layer = doc->FindLayer_by_Name("ALL");

   if (layer != NULL)
   {
      layer->setComment("All");
      layer->setLayerType(LAYTYPE_ALL);
   }

   int layerNum = Graph_Level("PADLAYER_TOP", "", 0);
   layer = doc->FindLayer(layerNum);
   layer->setComment("Pad Top");
   layer->setLayerType(LAYTYPE_PAD_TOP);

   layerNum = Graph_Level("PADLAYER_INNER", "", 0);
   layer = doc->FindLayer(layerNum);
   layer->setComment("Pad Inner");
   layer->setLayerType(LAYTYPE_PAD_INNER);

   layerNum = Graph_Level("PADLAYER_BOT", "" ,0);
   layer = doc->FindLayer(layerNum);
   layer->setComment("Pad Bottom");
   layer->setLayerType(LAYTYPE_PAD_BOTTOM);

   Graph_Level_Mirror("PADLAYER_TOP", "PADLAYER_BOT", "");

   // switch padlayer off
	int i=0;
   for (i=2; i<maxlayer; i++)
   {
      CString layerName;
      layerName.Format("PADLAYER_%d", i); // case 413831 reverted to pre case 376877,  case 376877 used:  layerName.Format("%d", i);

      layer = doc->FindLayer_by_Name(layerName);

      if (layer != NULL)
      {
         layer->setLayerType(LAYTYPE_PAD_INNER);
         layer->setElectricalStackNumber(i);
      }
   }

   // Do this now to init all that will be init's.
   AssignPadsDotInLayerTypes();

   // tech section was missing, make "1" as the SIGNAL_TOP and "max" as SIGNAL_BOTTOM
   if (maxlayer == 0 && max_copper_layer > 0)
   {
      for (i=1;i <= max_copper_layer; i++)
      {
         CString layerName;
         layerName.Format("%d", i);

         layer = doc->FindLayer_by_Name(layerName);

         if (layer == NULL)
            continue;

         if (layer->getLayerType() != 0)
            continue;

         if (i > 16)
         {
            fprintf(ferr, "Missing PCB header -> Suspicious Copper Layer [%d] detected\n", i);
            display_error++;
         }

         layer->setLayerType(LAYTYPE_SIGNAL);
         layer->setElectricalStackNumber(i);
      }
   }

   if (!layer_assign)
   {
      CString layer1, layer2;
      layer1.Format("DECAL_%d", 1);
      layer2.Format("DECAL_%d", maxlayer);

      int layerNum = Graph_Level(layer1, "", 0);
      layer = doc->FindLayer(layerNum);
      layer->setComment("Component_Outline_Top");
      layer->setLayerType(LAYTYPE_SILK_TOP);
   
      layerNum = Graph_Level(layer2, "", 0);
      layer = doc->FindLayer(layerNum);
      layer->setComment("Component_Outline_Bottom");
      layer->setLayerType(LAYTYPE_SILK_BOTTOM);
   
      // map decal layers
      Graph_Level_Mirror(layer1, layer2, "");

      // needed to map copper on decals
      layer1.Format("%d", 1);
      layer2.Format("%d", maxlayer);
      Graph_Level_Mirror(layer1, layer2, "");
   }
   else
   {
      int misclayertop = -1;
      int misclayerbot = -1;

      for (i=0; i<misclayercnt; i++)
      {
         PADSMiscLayer *miscLayer = misclayerarray[i];

         CString numericLayerName;
         numericLayerName.Format("%d", miscLayer->stacknumber);

         layer = doc->FindLayer_by_Name(numericLayerName);

         if (layer != NULL)
         {
            layer->setName( miscLayer->layer_name);
            layer->setComment(numericLayerName);

            int layerType = get_padslayertype(i);

            if (layerType != 0)
            {
               layer->setLayerType(layerType);
            }
         }
      
         if (miscLayer->stacknumber == 1)
         {
            misclayertop = i;
         }
         else if (miscLayer->stacknumber == maxlayer)
         {
            misclayerbot = i;
         }
         else
         {
            CString layerName;
            layerName.Format("DECAL_%d", miscLayer->stacknumber);

            LayerStruct *origLayer = doc->FindLayer_by_Name(layerName);

            if (origLayer != NULL && layer != NULL)
            {
               MoveLayer(doc, origLayer, layer, TRUE);
            }
         }
      }

      if (misclayertop > -1 && misclayerbot > -1)
      {
         PADSMiscLayer *layerTop = misclayerarray[misclayertop];
         PADSMiscLayer *layerBot = misclayerarray[misclayerbot];

         if (strlen(layerTop->associated_assembly) && strlen(layerBot->associated_assembly))
         {
            Graph_Level_Mirror(layerTop->associated_assembly, layerBot->associated_assembly, "");
            assignCadLayerNumberComment(layerTop->associated_assembly);
            assignCadLayerNumberComment(layerBot->associated_assembly);
         }

         if (strlen(layerTop->associated_silk_screen) && strlen(layerBot->associated_silk_screen))
         {
            Graph_Level_Mirror(layerTop->associated_silk_screen, layerBot->associated_silk_screen, "");
            assignCadLayerNumberComment(layerTop->associated_silk_screen);
            assignCadLayerNumberComment(layerBot->associated_silk_screen);
         }

         if (strlen(layerTop->associated_paste_mask) && strlen(layerBot->associated_paste_mask))
         {
            Graph_Level_Mirror(layerTop->associated_paste_mask, layerBot->associated_paste_mask, "");
            assignCadLayerNumberComment(layerTop->associated_paste_mask);
            assignCadLayerNumberComment(layerBot->associated_paste_mask);
         }

         if (strlen(layerTop->associated_solder_mask) && strlen(layerBot->associated_solder_mask))
         {
            Graph_Level_Mirror(layerTop->associated_solder_mask, layerBot->associated_solder_mask, "");
            assignCadLayerNumberComment(layerTop->associated_solder_mask);
            assignCadLayerNumberComment(layerBot->associated_solder_mask);
         }

         if (strlen(layerTop->layer_name) && strlen(layerBot->layer_name))
         {
            Graph_Level_Mirror(layerTop->layer_name, layerBot->layer_name, "");
            assignCadLayerNumberComment(layerTop->layer_name);
            assignCadLayerNumberComment(layerBot->layer_name);
         }

         // here now copy and merge the DECAL_%d layers to the correct Silk etc...
         // move DECAL_1 to associated_silk_screen on top
         // move DECAL_%d to associated_silk_screen on bottom

         LayerStruct* origLayer = doc->FindLayer_by_Name("DECAL_1");

         if (origLayer != NULL)
         {
            LayerStruct* newLayer = doc->FindLayer_by_Name(layerTop->associated_silk_screen);

            if (newLayer != NULL)
            {
               MoveLayer(doc, origLayer, newLayer, TRUE);
            }
         }

         CString layerName;
         layerName.Format("DECAL_%d", maxlayer);

         origLayer = doc->FindLayer_by_Name(layerName);

         if (origLayer != NULL)
         {
            LayerStruct* newLayer = doc->FindLayer_by_Name(layerBot->associated_silk_screen);

            if (newLayer != NULL)
            {
               MoveLayer(doc, origLayer, newLayer, TRUE);
            }
         }
      }
   }

	// Do this again to make sure any layer that is not assigned a layer type 
   // will get the layer type form pads.in
   for (i=0; i<doc->getLayerCount(); i++)
   {
		layer = doc->getLayerArray()[i];

      if (layer == NULL)
      {
         continue;
      }

		if (layer->getLayerType() == layerTypeUnknown)
		{
			// If no layer type then try to get it from misclayerarray
			for (int j=0; j<misclayercnt; j++)
			{
				PADSMiscLayer *miscLayer = misclayerarray[j];

				if (layer->getName().CompareNoCase(miscLayer->layer_name) == 0)
				{
					int layerType = get_padslayertype(j);

					if (layerType)
               {
						layer->setLayerType(layerType);
               }

					break;
				}
			}
		}

		if (layer->getLayerType() == layerTypeUnknown)
		{
			for (int j=0; j<layer_attr_cnt; j++)
			{
				if (layer->getName().CompareNoCase(layer_attr[j].layerName) == 0)
				{
					layer->setLayerType(layer_attr[j].layerType);

					break;
				}
			}
		}

      // Only keep the layer type drill we made up front, that is where the drills really go.
      // Turn any others into documentation layers.
      if (layer->getLayerType() == layerTypeDrill && layer->getLayerIndex() != drilllayernum)
         layer->setLayerType(layerTypeDocumentation);
   }

   return 1;
}

/******************************************************************************
* set_boundary_to_hidden
*/
static int set_boundary_to_hidden(BlockStruct *Block)
{
   POSITION dataPos = Block->getDataList().GetHeadPosition();

   while (dataPos) 
   {
      DataStruct *data = Block->getDataList().GetNext(dataPos);
      if (data->getDataType() != T_POLY)
         continue;
      
      CPoly *boundarypoly = NULL;
      int filled = FALSE;

      POSITION polyPos = data->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = data->getPolyList()->GetNext(polyPos);
         if (poly->isFloodBoundary())
            boundarypoly = poly;
         else if (poly->isFilled())
            filled = TRUE;
      }

      // hide a boundary if a filled has been detected inside.
      // that means a pour was flooded.
      if (boundarypoly && filled)
         boundarypoly->setHidden(true);
   }

   return 1;
}

/******************************************************************************
* do_padstacklayers
*/
static int do_padstacklayers()
{
   CString lname;
   int i;
   LayerStruct *l;

   // -2 is top
   lname.Format("PADLAYER_TOP");
   i = Graph_Level(lname, "", 0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_TOP);

   // -1 is inner
   lname.Format("PADLAYER_INNER");
   i = Graph_Level(lname, "", 0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_INNER);

   // 0 is bottom
   lname.Format("PADLAYER_BOT");
   i = Graph_Level(lname, "", 0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   Graph_Level_Mirror("PADLAYER_TOP", "PADLAYER_BOT", "");

   return 1;
}

/******************************************************************************
* write_single_padstack
*/
static int write_single_padstack(int i)
{
   CString  name;
   int smd = 0;
   int index = i;
   name.Format("PADSTACK_%d", index);

   while (Graph_Block_Exists(doc, name, file->getFileNumber()))
   {
      name.Format("PADSTACK_%d", ++index);
   }

   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, name, file->getFileNumber(),0);
   curblock->setBlockType(blockTypePadstack);

   PADS_Paddef& paddef = *(paddefarray.getAt(i));
   paddef.setPadstackName(name);
   DbFlag padstackflag = 0;      

   for (int ii=0; ii<MAX_PADSTACK_SIZE; ii++)
   {
      CString lname;
      if (paddef.padstack[ii] > -1)
      {
         if (ii == 0)
         {
            lname = "PADLAYER_TOP";
            padstackflag |= BL_ACCESS_TOP;
         }
         else if (ii == 1)
         {
            lname = "PADLAYER_INNER";
         }
         else if (ii == 2)
         {
            lname = "PADLAYER_BOT";
            padstackflag |= BL_ACCESS_BOTTOM;
         }
         else if (ii == 3)
         {
            lname = "PADLAYER_TOP";
            padstackflag |= BL_ACCESS_TOP;
         }
         else if (ii == maxlayer+2)
         {
            lname = "PADLAYER_BOT";
            padstackflag |= BL_ACCESS_BOTTOM;
         }
         else
            lname.Format("PADLAYER_%d", ii-2); // case 413831, revert back to pre case 376877 (was: lname.Format("%d", ii-2); 
            
         int layernum = Graph_Level(lname, "", 0);
         if (ii > 2)
            LayerStruct *l = doc->FindLayer(layernum);

         BlockStruct *block = doc->Find_Block_by_Num(paddef.padstack[ii]);
         DataStruct *data = Graph_Block_Reference(block->getName(), NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);

         if (ii == 0)      // -2 top 
            smd |= 1;
         else if (ii == 1) // -1 inner
            smd |= 2;
         else if (ii == 2) // 0  bottom
            smd |= 4;
      }
   }     

   if (paddef.getDrillSize() > 0.)
   {
      int drillayer = Graph_Level("DRILLHOLE", "", 0);
      BlockStruct *block = Graph_FindTool(paddef.getDrillSize(), 0, TRUE,paddef.getPlatedFlag());

      // must be filenum 0, because apertures are global.
      DataStruct* toolData = Graph_Block_Reference(block->getName(), NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, drillayer, TRUE);
      toolData->getInsert()->setInsertType(insertTypeDrillHole);
   }

   Graph_Block_Off();
   curblock->setFlagBits(padstackflag);

   if (smd == 1)
      doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, attributeUpdateOverwrite, NULL);
   
   //paddefarray.SetAt(i, paddef);
   return 1;
}

/******************************************************************************
* is_command
*/
static int is_command(char *l, List *c_list, int siz_of_command)
{
   for (int i=0; i<siz_of_command; i++)
   {
      if (!_strnicmp(c_list[i].token, l, strlen(c_list[i].token)))
         return i;
   }
   return -1;
}

/******************************************************************************
* loop_pads
*/
static int loop_pads(List *c_list, int siz_of_command)
{
   // here do a while loop
   while (get_nextline(ifp_line, MAX_LINE, ifp) && !eof_found)
   {
      CString tmp = ifp_line;

      char *lp = strtok(ifp_line, " \t\n");
      if (lp == NULL)
         continue;

      int code = is_command(lp, c_list, siz_of_command);
      if (code < 0)
      {
         fprintf(ferr, "Unknown PADS Section [%s] at %ld\n", lp, getInputFileLineNumber());
         display_error++;
         pads_skip();
      }
      else
      {
         if ((*c_list[code].function)() < 0)
            return -1;
      }
   }

   return 1;
}

/******************************************************************************
* *get_nextline
*/
static char *get_nextline(char *string, int n, FILE *fp)
{
   static CString last_line;
   static char *last_res;
   long last_linecnt;
   char *res;
   bool getAnotherLine = false;

   if (PushTok)
   {
      strcpy(string, last_line);
      res = last_res;
      PushTok = FALSE;
   }
   else
   {
      int remark = TRUE;
      while (remark)
      {
         
			incrementInputFileLineNumber();
         res = fgets(string, MAX_LINE, fp);
						
			if (res == NULL)
         {
            eof_found = TRUE;
            return NULL;
         }
         last_line = string;
         last_line.TrimLeft();
         last_line.TrimRight();
         last_linecnt = getInputFileLineNumber();

         CString l = last_line.Left(4);
         l.TrimRight(); 
         if (l.GetLength() == 0)
            continue;

			if (l != "*REM")
            remark = FALSE;			

         last_res = res;
      }
   }
   
   // clean right 
   CString tmp = last_line;
   tmp.TrimRight();
   strcpy(string, tmp);

   return res;
}

/******************************************************************************
* pads_skip
*/
static int pads_skip()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = strtok(ifp_line," \t\n");
      if (lp == NULL)
         continue;
      if (nextsect(lp))
         break;
   }

   return 1;
}
    
/******************************************************************************
* int do_misclayer_layer
*  From 
*  {
*     {
*     }
*  }  to blacket closed
*
*  short    layertype;              // see dbutil.h dbutil.cpp
*  short    artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
*  short    electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
*  short    physicalstacknumber;    // phusycal manufacturing stacking of layers, 
*                                   // signaltop / dialectric / signal_1 / dia / power_1 .... / solder
*  
*  unsigned long attr;     // layer attributes
*/
static int  do_misclayer_layer(int layernr)
{
   int bracket_cnt = 0;
   LayerStruct *ll;
   
   layer_assign = TRUE;

   PADSMiscLayer *miscLayer = new PADSMiscLayer;
   misclayerarray.SetAtGrow(misclayercnt++, miscLayer);
   miscLayer->stacknumber = layernr;
   miscLayer->component = FALSE;    // this must be set to make the associated layers active
   miscLayer->layer_name.Format("Layer_%d", layernr);
   miscLayer->associated_silk_screen = "";
   miscLayer->associated_paste_mask = "";
   miscLayer->associated_solder_mask = "";
   miscLayer->associated_assembly = "";
   miscLayer->layer_type = "";

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = get_string(ifp_line," \t\n");
      if (lp == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "COMPONENT")) 
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         if (!STRCMPI(lp, "Y"))
            miscLayer->component = TRUE;
      }
      else if (!STRCMPI(lp,"ASSOCIATED_SILK_SCREEN")) 
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_silk_screen = lp;
         miscLayer->associated_silk_screen.TrimLeft();
         miscLayer->associated_silk_screen.TrimRight();
      }
      else if (!STRCMPI(lp, "ASSOCIATED_ASSEMBLY"))   
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_assembly = lp;
         miscLayer->associated_assembly.TrimLeft();
         miscLayer->associated_assembly.TrimRight();
      }
      else if (!STRCMPI(lp, "ASSOCIATED_PASTE_MASK")) 
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_paste_mask = lp;
         miscLayer->associated_paste_mask.TrimLeft();
         miscLayer->associated_paste_mask.TrimRight();
      }
      else if (!STRCMPI(lp, "ASSOCIATED_SOLDER_MASK"))   
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_solder_mask = lp;
         miscLayer->associated_solder_mask.TrimLeft();
         miscLayer->associated_solder_mask.TrimRight();
      }
      else if (!STRCMPI(lp, "LAYER_NAME"))   // layername are comments.
      {
         CString layer_name;
         CString lname;
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         layer_name = lp;
         layer_name.TrimLeft();
         layer_name.TrimRight();

         lname.Format("%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            ll->setComment(layer_name);

         // here now check if a decal layer was 
         lname.Format("DECAL_%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            ll->setComment(layer_name);
         miscLayer->layer_name = layer_name;
      }
      else if (!STRCMPI(lp, "LAYER_THICKNESS"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         double h = cnv_unit(atof(lp));
         if (h > 0)
         {
            CString lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               doc->SetAttrib(&ll->getAttributesRef(), doc->IsKeyWord(LAYATT_THICKNESS, TRUE), VT_UNIT_DOUBLE, &h, attributeUpdateOverwrite, NULL); 
         }
      }
      else if (!STRCMPI(lp, "LAYER_TYPE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         miscLayer->layer_type = lp;
      }
      else if (!STRCMPI(lp, "PLANE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;

         if (!STRCMPI(lp, "CAM"))
         {
            CString lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               ll->setLayerType(LAYTYPE_POWERNEG);
         }
         else if (!STRCMPI(lp, "MIXED"))
         {
            CString lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               ll->setLayerType(LAYTYPE_SPLITPLANE);
         }
      }
      else if (!STRCMPI(lp, "NET"))
      {
         // Need to pass false for the optional value in get_string function because sometimes net name starts with "\" - Joanne
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL)
            continue;

         CString lname;
         lname.Format("%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            doc->SetUnknownAttrib(&ll->getAttributesRef(), LAYATT_NETNAME, lp, SA_APPEND, NULL); // x, y, rot, height

         NetStruct *net = add_net(file,lp);
         if (net)
            doc->SetUnknownAttrib(&net->getAttributesRef(), ATT_POWERNET, "", attributeUpdateOverwrite, NULL); //  
      }
      else if (!STRCMPI(lp, "COPPER_THICKNESS"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         double h = cnv_unit(atof(lp));
         if (h > 0)
         {
            CString  lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               doc->SetAttrib(&ll->getAttributesRef(),doc->IsKeyWord(LAYATT_COPPER_THICKNESS, TRUE), VT_UNIT_DOUBLE, &h, attributeUpdateOverwrite, NULL); 
         }
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString val = lp;

         CString  lname;
         lname.Format("%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            doc->SetUnknownAttrib(&ll->getAttributesRef(), get_attribmap(key), val, attributeUpdateOverwrite, NULL); // x, y, rot, height
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattributepart_values
*/
static int  do_miscattributepart_values(const char *refname)
{
   int   bracket_cnt = 0;

   // if this is a via, find it and assign cur_netname
   DataStruct *data = datastruct_from_refdes(doc, file->getBlock(), refname);

   if (data == NULL)
   {
      fprintf(ferr, "PART [%s] not found at %ld\n", refname, getInputFileLineNumber());
      display_error++;
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = get_string(ifp_line," \t\n");

      if (lp == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (data)
         {
            if (is_attrib(key, HEIGHT_ATT))
            {
               char save_unit = unit_flag;
               unit_flag = heightunit_flag;

               double h = cnv_unit(atof(val));
               doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE,&h,attributeUpdateOverwrite, NULL);
               unit_flag = save_unit;
            }
            else
            {
               doc->SetUnknownAttrib(&data->getAttributesRef(), get_attribmap(key), val, attributeUpdateOverwrite, NULL);
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattributeparttype_values
*/
static int  do_miscattributeparttype_values(const char *typname)
{
   int bracket_cnt = 0;

	// When get_string() is called below, typname changes !!!  (the dangers of globals used in func calls !)
	// What is passed in is a pointer into the input buffer. The buffer and content changes with I/O operations in the loop.
	// So save a local copy.
	CString TypName = typname;

   TypeStruct *basetype = AddType(file, TypName);

   if (basetype == NULL)
   {
      fprintf(ferr, "PARTTYPE [%s] not found at %ld\n", TypName, getInputFileLineNumber());
      display_error++;
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = get_string(ifp_line," \t\n");

      if (lp == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
                       
      if (!STRCMPI(lp,"{"))   //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (basetype)
         {
            if (is_attrib(key, HEIGHT_ATT))
            {
               char save_unit = unit_flag;
               unit_flag = heightunit_flag;

               double h = cnv_unit(atof(val));
               doc->SetAttrib(&basetype->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE,&h,attributeUpdateOverwrite, NULL);
               unit_flag = save_unit;
            }
            else
            {
               doc->SetUnknownAttrib(&basetype->getAttributesRef(), get_attribmap(key), val, attributeUpdateOverwrite, NULL);
            }
         }
      }
   }

	// Propogate attributes from basetype to alternate (aka derived) types
	if (basetype != NULL)
	{
		int typeindx = get_ptypeptr(TypName);
		if (typeindx > -1) // jic, but it should always exist by now
		{
			PADSPart *padspart = partarray[typeindx];

			for (int i = 0; i < padspart->decalcnt; i++)
			{
				CString originaldecalname = padspart->originaldecalname[i];
				CString decalname = padspart->decalname[i];

				CString derived_original;
				derived_original.Format("%s_%s", TypName, originaldecalname);

				TypeStruct *derivedOriginalType = FindType(file, derived_original);
            if (derivedOriginalType != NULL && derivedOriginalType->getAttributesRef() != NULL &&
               basetype->getAttributesRef() != NULL)
				{
               derivedOriginalType->getAttributesRef()->CopyNew(*basetype->getAttributesRef());
				}

				CString derived_decal;
				derived_decal.Format("%s_%s", TypName, decalname);

				if (derived_original.Compare(derived_decal) != 0)
				{
					TypeStruct *derivedtype = FindType(file, derived_decal);
					if (derivedtype != NULL)
					{
						derivedtype->getAttributesRef()->CopyNew(*basetype->getAttributesRef());
					}
				}
			}
		}
	}


   return 1;
}

/******************************************************************************
* int do_miscattributedecal_values
*/
static int do_miscattributedecal_values(const char *decalname)
{
   int bracket_cnt = 0;
   BlockStruct *block;

   block = Graph_Block_Exists(doc, decalname, -1);

   if (block == NULL)
   {
      fprintf(ferr, "DECAL [%s] not found at %ld\n", decalname, getInputFileLineNumber());
      display_error++;
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = get_string(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (block)
         {
            if (is_attrib(key, HEIGHT_ATT))
            {
               char save_unit = unit_flag;
               unit_flag = heightunit_flag;
   
               double h = cnv_unit(atof(val));
               doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &h, attributeUpdateOverwrite, NULL); 
               unit_flag = save_unit;
            }
            else
            {
               doc->SetUnknownAttrib(&block->getAttributesRef(), get_attribmap(key), val, attributeUpdateOverwrite, NULL); //  
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattributenet_values
*/
static int do_miscattributenet_values(const char *netname)
{
   int bracket_cnt = 0;

   NetStruct *net = add_net(file, netname);

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = get_string(ifp_line, "\"\t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         
         if (!STRCMPI(lp, "DFT."))
            if ((lp = get_string(NULL, "\"\n")) == NULL) //keywords are quoted, spaces acceptable
                    continue;
         
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();


         if ((lp = get_string(NULL, " \"\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();
         doc->SetUnknownAttrib(&net->getAttributesRef(), get_attribmap(key), val, attributeUpdateOverwrite, NULL);
         
      }
   }

   return 1;
}

static int do_miscAttributePcb_values(const CString pcbName)
{
   int bracket_cnt = 0;

   if (file == NULL)
   {
      fprintf(ferr, "PCB file not found at %ld\n", getInputFileLineNumber());
      display_error++;
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = get_string(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "ASSEMBLY_OPTIONS"))
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (file->getBlock() != NULL)
         {
            doc->SetUnknownAttrib(&file->getBlock()->getAttributesRef(), get_attribmap(key), val, attributeUpdateOverwrite, NULL); //  
         }
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattribute_values
*/
static int  do_miscattribute_values()
{
   int bracket_cnt = 0;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = get_string(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  
      {
         // do nothing
      }
      else
      if (!STRCMPI(lp, "}"))  
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "PARTTYPE"))  
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         do_miscattributeparttype_values(lp);
      }
      else if (!STRCMPI(lp, "PART"))   
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         do_miscattributepart_values(lp);
      }
      else if (!STRCMPI(lp, "NET")) 
      {
         // Need to pass false for the optional value in get_string function because sometimes net name starts with "\" - Joanne
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL)
            continue;
         do_miscattributenet_values(lp);
      }
      else if (!STRCMPI(lp, "DECAL"))  
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         do_miscattributedecal_values(lp);
      }
      else if (!STRCMPI(lp, "PCB"))  
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         do_miscAttributePcb_values(lp);
      }
      else
      {
/*
         fprintf(ferr,"Attribute association [%s] not implemented at %ld\n",
            lp, getInputFileLineNumber());
         display_error++;
*/
      }
   }

   return 1;
}

/******************************************************************************
* int do_misclayer
*  From 
*  {
*     {
*     }
*  }  to blacket closed
*/
static int do_misclayer()
{
   int bracket_cnt = 0;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "LAYER"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         int laynr = atoi(lp);
         do_misclayer_layer(laynr);
      }
   }

   return 1;
}

/******************************************************************************
* int misc_skip
   From 
   {
      {
      }
   }  to blacket closed
*/
static int misc_skip(const char *l)
{
   int bracket_cnt = 0;

   if (!strcmp(l, "{"))
      bracket_cnt++;
   if (!strcmp(l, "}"))
      bracket_cnt--;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
   }

   return 1;
}

/******************************************************************************
* pads_misc
*/
static int pads_misc()
{
   // is done here because this writes all padstack layers.
   // misc can update comments to layers.

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = strtok(ifp_line, " \t\n");
      if (lp == NULL)
         continue;
      if (nextsect(lp))
         break;
      
      if (!STRCMPI(lp, "LAYER"))
         do_misclayer();
      else if (!STRCMPI(lp, "ATTRIBUTES"))
         misc_skip(lp);
      else if (!STRCMPI(lp, "ATTRIBUTE"))
      {
         if ((lp = strtok(NULL, "\n")) == NULL)
            continue;
         CString tmp = lp;
         tmp.TrimLeft();
         tmp.TrimRight();
         if (!tmp.Compare("VALUES"))
            do_miscattribute_values();
         else
            misc_skip(lp);
      }
      else
         misc_skip(lp);
   }

   return 1;
}
    
/******************************************************************************
* pads_title_pads
*/
static int pads_title_pads()
{
   title_found = TRUE;
   file_version = 2;
   unit_flag = 'P';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pad5_title_pads
*/
static int pad5_title_pads()
{
   title_found = TRUE;
   file_version = 5;
   unit_flag = 'A';  // units are in mil
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pads_title_inch
*/
static int pads_title_inch()
{
   title_found = TRUE;
   file_version = 2;
   unit_flag = 'I';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pad3_title_inch
*/
static int pad3_title_inch()
{
   title_found = TRUE;
   file_version = 3;
   unit_flag = 'I';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pads_title_basic
*/
static int pads_title_basic()
{
   title_found = TRUE;
   file_version = 2;
   unit_flag = 'B';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pad4_title_basic
*/
static int pad4_title_basic()
{
   title_found = TRUE;
   file_version = 4;
   unit_flag = 'B';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pad6_title_basic
*/
static int pad6_title_basic()
{
   title_found = TRUE;
   file_version = 6;
   unit_flag = 'B';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad7_title_basic
*/
static int pad7_title_basic()
{
   title_found = TRUE;
   file_version = 7;
   unit_flag = 'B';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwrunknown_title
*/
static int pwrunknown_title()
{
   title_found = TRUE;

   file_version = 11;   // i do not know exactly what to do here
   if (strstr(ifp_line, "BASIC"))
      unit_flag = 'B';
   else if (strstr(ifp_line, "MILS"))
      unit_flag = 'A';
   else if (strstr(ifp_line, "INCHES"))
      unit_flag = 'I';
   else if (strstr(ifp_line, "METRIC"))
      unit_flag = 'M';
   else
   {
      fprintf(ferr, "Unknown header line [%s]\n", ifp_line);
      display_error++;
      return 0;
   }

   if (strstr(ifp_line, "250L"))  // layer_mode default is 30  - Joanne
      layer_mode = 250;


   // here get version 
   char *lp = strtok(ifp_line, "V");
	int real_file_version = 0;
   if (lp = strtok(NULL, "-"))
   {  
      if (atof(lp) > 1)
		{
         file_version = 10 * (int)atof(lp);
			real_file_version = (int)(10 * atof(lp));
		}
   }

   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr1_title_basic
*/
static int pwr1_title_basic()
{
   title_found = TRUE;
   file_version = 11;
   unit_flag = 'B';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr15_title_basic
*/
static int pwr15_title_basic()
{
   title_found = TRUE;
   file_version = 15;
   unit_flag = 'B';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr2005_1_title_basic
*/
int pwr2005_0_title_basic()
{
   title_found = TRUE;
   file_version = 20050;
   unit_flag = 'B';

		// pads 2005 is supported.  case 1384
	//CString msg = "New version 2005 of PADS file is detected, currently CAMCAD might not import the file properly.  This will be resolved in an upcoming version.\n";
	//ErrorMessage(msg, "New PADS File Version!");

   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr2007_0_title_basic
*/
int pwr2007_0_title_basic()
{
   title_found = TRUE;
   file_version = 20070;
   unit_flag = 'B';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr2009_0_title_basic
*/
int pwr2009_0_title_basic()
{
   title_found = TRUE;
   file_version = 20090;
   unit_flag = 'B';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad3_title_basic
*/
static int pad3_title_basic()
{
   title_found = TRUE;
   file_version = 3;
   unit_flag = 'B';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pad6_title_inch
*/
static int pad6_title_inch()
{
   title_found = TRUE;
   file_version = 6;
   unit_flag = 'I';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad7_title_inch
*/
static int pad7_title_inch()
{
   title_found = TRUE;
   file_version = 7;
   unit_flag = 'I';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr1_title_inch
*/
static int pwr1_title_inch()
{
   title_found = TRUE;
   file_version = 11;
   unit_flag = 'I';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr15_title_inch
*/
static int pwr15_title_inch()
{
   title_found = TRUE;
   file_version = 15;
   unit_flag = 'I';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad4_title_inch
*/
static int pad4_title_inch()
{
   title_found = TRUE;
   file_version = 4;
   unit_flag = 'I';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pwr2007_0_title_inch
*/
int pwr2007_0_title_inch()
{
   title_found = TRUE;
   file_version = 20070;
   unit_flag = 'I';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr2009_0_title_inch
*/
int pwr2009_0_title_inch()
{
   title_found = TRUE;
   file_version = 20090;
   unit_flag = 'I';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad6_title_metric
*/
static int pad6_title_metric()
{
   title_found = TRUE;
   file_version = 6;
   unit_flag = 'M';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad7_title_metric
*/
static int pad7_title_metric()
{
   title_found = TRUE;
   file_version = 7;
   unit_flag = 'M';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr1_title_metric
*/
static int pwr1_title_metric()
{
   title_found = TRUE;
   file_version = 11;
   unit_flag = 'M';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr15_title_metric
*/
static int pwr15_title_metric()
{
   title_found = TRUE;
   file_version = 15;
   unit_flag = 'M';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad4_title_metric
*/
static int pad4_title_metric()
{
   title_found = TRUE;
   file_version = 4;
   unit_flag = 'M';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pad3_title_metric
*/
static int pad3_title_metric()
{
   title_found = TRUE;
   file_version = 3;
   unit_flag = 'M';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pads_title_metric
*/
static int pads_title_metric()
{
   title_found = TRUE;
   file_version = 2;
   unit_flag = 'M';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pwr2007_0_title_metric
*/
int pwr2007_0_title_metric()
{
   title_found = TRUE;
   file_version = 20070;
   unit_flag = 'M';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr2009_0_title_metric
*/
int pwr2009_0_title_metric()
{
   title_found = TRUE;
   file_version = 20090;
   unit_flag = 'M';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad6_title_mil
*/
static int pad6_title_mil()
{
   title_found = TRUE;
   file_version = 6;
   unit_flag = 'A';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad7_title_mil
*/
static int pad7_title_mil()
{
   title_found = TRUE;
   file_version = 7;
   unit_flag = 'A';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr1_title_mil
*/
static int pwr1_title_mil()
{
   title_found = TRUE;
   file_version = 11;
   unit_flag = 'A';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr15_title_mil
*/
static int pwr15_title_mil()
{
   title_found = TRUE;
   file_version = 15;
   unit_flag = 'A';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pad4_title_mil
*/
static int pad4_title_mil()
{
   title_found = TRUE;
   file_version = 4;
   unit_flag = 'A';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pads_title_mil
*/
static int pads_title_mil()
{
   title_found = TRUE;
   file_version = 2;
   unit_flag = 'A';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pad3_title_mil
*/
static int pad3_title_mil()
{
   title_found = TRUE;
   file_version = 3;
   unit_flag = 'A';
   return loop_pads(pcb_lst, SIZ_PCB_LST);
}

/******************************************************************************
* pwr2007_0_title_mil
*/
int pwr2007_0_title_mil()
{
   title_found = TRUE;
   file_version = 20070;
   unit_flag = 'A';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pwr2009_0_title_mil
*/
int pwr2009_0_title_mil()
{
   title_found = TRUE;
   file_version = 20090;
   unit_flag = 'A';
   return loop_pads(pcb6_lst, SIZ_PCB6_LST);
}

/******************************************************************************
* pads_end
*/
static int pads_end()
{
   eof_found = TRUE; // logical end 
   return 1;
}

/******************************************************************************
* pads_pcb
*/
static int pads_pcb()
{
   PCB_SECTION_FOUND = TRUE;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      
      if (!STRCMPI(lp, "REFNAMESIZE"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;   // y
         refnamesize.height = cnv_unit(atof(lp)) * TEXT_HEIGHTFACTOR;

         if ((lp = strtok(NULL," \t\n")) == NULL)
            continue;   // y
         refnamesize.linewidth = cnv_unit(atof(lp));
      }
      else if (!STRCMPI(lp, "MAXIMUMLAYER"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         maxlayer = atoi(lp); 
         
         for (int ii=1; ii<=maxlayer; ii++)
         {
            CString lname;
            lname.Format("%d", ii);
            int i = Graph_Level(lname, "", 0);

            LayerStruct *layer = doc->FindLayer(i);
            if (layer == NULL)   
               continue;
            
            if (ii == 1)
               layer->setLayerType(LAYTYPE_SIGNAL_TOP);
            else if (ii == maxlayer)
               layer->setLayerType(LAYTYPE_SIGNAL_BOT);
            else
               layer->setLayerType(LAYTYPE_SIGNAL_INNER);

            layer->setElectricalStackNumber(ii);
         }
      }
      else
      {
         // unknown
      }
   }

   int i = Graph_Level("PADLAYER_TOP", "", 0);
   LayerStruct *layer = doc->FindLayer(i);
   if (layer)
      layer->setElectricalStackNumber(1);

   i = Graph_Level("PADLAYER_BOT", "", 0);
   layer = doc->FindLayer(i);
   if (layer)
      layer->setElectricalStackNumber(maxlayer);

   Graph_Level_Mirror("PADLAYER_TOP", "PADLAYER_BOT", "");


   return 1;
}

/******************************************************************************
* pads_matrix
*/
static int pads_matrix()
{
   pads_skip();   
   return 1;
}

/******************************************************************************
* pads_reuse
*/
static int pads_reuse()
{
   ErrorMessage("Error: This is not a PADS *PCB* file!\nFile can not be read.", "PADS ASCII Read Error");
   return -2;
}

/******************************************************************************
* add_test_to_via
*/
static int add_test_to_via(double x, double y, int side) // side 0 = bottom, 1 = top
{
   CString testval = "";
   if (side == 0)
      testval = testaccesslayers[ATT_TEST_ACCESS_BOT];
   else if (side == 1)
      testval = testaccesslayers[ATT_TEST_ACCESS_TOP];

	double tolerance = doc->convertToPageUnits(pageUnitsMils, 0.1);  // using 1/10 mil, aka 1/10000 inch as tolerance

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      if (fabs(data->getInsert()->getOriginX() - x) < tolerance && fabs(data->getInsert()->getOriginY() - y) < tolerance)
      {
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TEST, TRUE), VT_STRING, testval.GetBuffer(0) ,attributeUpdateOverwrite, NULL);
         return 1;
      }
   }
   return 0;
}

/******************************************************************************
* add_test_to_pin
*/
static int add_test_to_pin(const char *comppin, int side)
{
   CString testval = "";
   if (side == 0)
      testval = testaccesslayers[ATT_TEST_ACCESS_BOT];
   else if (side == 1)
      testval = testaccesslayers[ATT_TEST_ACCESS_TOP];

   char comp_pin[255];
   strcpy(comp_pin, comppin);

   char *lp;
   if ((lp = strtok(comp_pin, ".")) == NULL)
      return 0;
   CString comp = lp;

   if ((lp = strtok(NULL, ".")) == NULL)
      return 0;
   CString pin = lp;

   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         if (!STRCMPI(compPin->getRefDes(), comp) && !STRCMPI(compPin->getPinName(), pin))
         {
            doc->SetAttrib(&compPin->getAttributesRef(), doc->IsKeyWord(ATT_TEST, TRUE),
                  VT_STRING, testval.GetBuffer(0),attributeUpdateOverwrite, NULL); 
            return 1;
         }
      }
   }

   return 0;
}

/******************************************************************************
* pads_testpoint
      *REMARK* TEST POINTS ON VIAS
      *REMARK* VIA XLOC YLOC SIDE SIGNAME SYMBOLNAME
      VIA   7000    3450    0   +5V        TESTVIATHRU
*/
static int pads_testpoint()
{
   if (!elim_double_vias)  
      EliminateDuplicateVias(doc, file->getBlock());
   elim_double_vias = TRUE;

   generate_NCnet(doc, file); // this function generates the NC (non connect) net.

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char  *lp;

      if ((lp = strtok(ifp_line," \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString tstname = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      // side
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int side = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString signame = lp;
      if (signame.CompareNoCase(".NONE.") == 0)
         signame = NET_UNUSED_PINS;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;   // signame
      CString symbolname = lp;

      // here now loop thru the file list and
      // find the matching via and flag it for test/
      if (tstname.CompareNoCase("VIA") == 0)
      {
         if ((add_test_to_via(x, y, side)) == 0)
         {
            fprintf(ferr, "TEST VIA could not be found at %ld\n", getInputFileLineNumber());
            display_error++;
         }
      }
      else
      {
         if ((add_test_to_pin(symbolname, side)) == 0)
         {
            fprintf(ferr, "TEST PIN [%s] could not be found at %ld\n", symbolname, getInputFileLineNumber());
            display_error++;
         }
      }
   }
   return 1;
}

/******************************************************************************
* pads_text
*/
static int pads_text()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;
		
		if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
		
      if (nextsect(lp))
         break;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;   
      double rot = atof(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      if (!STRCMPI(lp, "0"))
         lp = "ALL";
      int level = Graph_Level(lp, "", 0);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;   
      double height = cnv_unit(atof(lp)) * TEXT_HEIGHTFACTOR;
      double width = height * TEXT_WIDTHFACTOR;

      if ((lp = strtok(NULL," \t\n")) == NULL)
         continue;   
      double penwidth = cnv_unit(atof(lp));

      int mirror = FALSE;
      if ((lp = strtok(NULL," \t\n")) != NULL) // mirror
         if (lp[0] == 'M') mirror = TRUE;

      int ta = 0; // CAMCAD textAlignment is Pads HJUST
      if ((lp = strtok(NULL," \t\n")) != NULL) // HJUST
      {
         switch (lp[0])
         {
            case 'L':
            case 'l':
               ta = 0;
               break;
            case 'C':
            case 'c':
               ta = 1;
               break;
            case 'R':
            case 'r':
               ta = 3;
               break;
         }
      }

      int la = 0; // CAMCAD lineAlignment is Pads VJUST
      if ((lp = strtok(NULL," \t\n")) != NULL) // VJUST
      {
         switch (lp[0])
         {
            case 'D':
            case 'd':
               la = 0;
               break;
            case 'C':
            case 'c':
               la = 1;
               break;
            case 'U':
            case 'u':
               la = 3;
               break;
         }
      }

      incrementInputFileLineNumber();    // here get prosa
      fgets(ifp_line, MAX_LINE, ifp);
		
		// Case 1668
		if (file_version >= 20050)
		{
			// Skip font name in current line, get next line
			incrementInputFileLineNumber();
			fgets(ifp_line, MAX_LINE, ifp);
		}

      CString tmp = ifp_line;
      tmp.TrimRight();
      strcpy(ifp_line, tmp);
      if ((lp = strtok(ifp_line, "\n")) == NULL)
         continue;

      if (strlen(lp))
         Graph_Text(level, lp, x, y, height, width, DegToRad(rot), 0 , TRUE, mirror, 0, FALSE, -1, 0, ta, la); // prop flag, mirror flag, oblique angle in deg
   }

   return 1;
}

/******************************************************************************
* pads_lines
*/
static int pads_lines()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char *lp;
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString name = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString linetype = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int pieces = atoi(lp);

      int flags = 0;
      if (file_version >= 20070)
      {
         // There is a new field "flags" is added in version 2007.0, currently we see a need to use it
         // so we are just going to read it and leave it alone.
         if ((lp = strtok(NULL, " \t\n")) != NULL)
            flags = atoi(lp);
      }
   
      // if COPPER the text is not there, but the signal name maybe 
      int textcnt= 0;
      if (linetype.CompareNoCase("COPPER")) // check if not equal
      {
         if ((lp = strtok(NULL, " \t\n")) != NULL) 
            textcnt = atoi(lp);
      }

      char *sigstr = "";
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         sigstr = lp;

		int i=0;
      for (i=0; i<pieces; i++)
      {        
			DataStruct* data = get_pieces(x, y, name, linetype, ""); // yes, we want a name
         if (data != NULL && strlen(sigstr))
         {
            data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING, (void *)sigstr, SA_APPEND, NULL);
         }
      }

      double h;
      for (i=0; i<textcnt; i++)
         get_text(x, y, name, &h);
   }

   return 1;
}

/******************************************************************************
* is_layer_compoutline
*/
static int is_layer_compoutline(const char *l)
{
   for (int i=0; i<compoutlinecnt; i++)
   {
      if (compoutline[i].CompareNoCase(l) == 0)
         return 1;
   }
   return 0;
}

/******************************************************************************
* *get_pieces
*/
DataStruct *get_pieces(double x, double y, const char *name, const char *linetype, const char *shapename)
{
#if CamCadMajorMinorVersion > 406  //  > 4.6
   CCamCadData& camCadData = doc->getCamCadData();
#endif

   get_nextline(ifp_line, MAX_LINE, ifp);

   char *lp;
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return NULL;

   if (!STRCMPI(lp, ".REUSE."))  // skip the reuse line and 
   {
      get_nextline(ifp_line, MAX_LINE, ifp);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         return NULL;
   }
   CString type = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return NULL;
   int numcoo = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return NULL;
   double width = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return NULL;   
   int layer = atoi(lp);

   CString restrictions = ""; // only for keepout
   int pinnr = 0;
   if ((lp = strtok(NULL, " \t\n")) != NULL) // keepout restrictions
   {
      if (!STRCMPI(type, "KPTCLS") || !STRCMPI(type, "KPTCIR"))
      {
         restrictions = lp;
         if (!STRCMPI(restrictions, "H"))
            width = 0; // for this the width is the height restriction
      }
      else
         pinnr = atoi(lp)+1;
   }

   if (numcoo == 0)  
   {
      return NULL;
   }
   else if (numcoo > 32000)   
   {
      fprintf(ferr, "Bad number of Coordinates %d at %ld\n", numcoo, getInputFileLineNumber());
      display_error++;
      return NULL;
   }

   CString layername = "";
   if (!STRICMP(linetype, "BOARD"))
   {
      layername = "BOARD_OUTLINE";
   }
   else if (!STRCMPI(linetype, "DECAL"))
   {
      // if layer is copper, use 1...n layer
      if (!STRNICMP(type, "COP", 3))
      {
         layername.Format("%d", layer);
         if (layer > max_copper_layer)
            max_copper_layer = layer;
      }
      else
      {
         // case 413831 reverted to pre case 376877
         // case 376877 used:  
         ///if (layer == 0 || layer == 1) // layer 0 and 1 in decal are the same
         ///   layername = "1"; 
         ///else
         ///   layername.Format("%d", layer); 
         if (layer == 0 || layer == 1) // layer 0 and 1 in decal is the same
            layername = "DECAL_1";
         else
            layername.Format("DECAL_%d", layer);
      }
   }
   else
   {
      if (layer == 0)      // layer 0 is all
         layername = "ALL";
      else
         layername.Format("%d", layer);
   }

   int level = Graph_Level(layername, "", 0);
   int err;
   int widthindex = 0; 
   // if (width > 0)
   //    widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   int closed = FALSE;
   int filled = FALSE;
   int voidpoly = FALSE;
   if (!type.CompareNoCase("CLOSED") || !type.CompareNoCase("CIRCLE") || !type.CompareNoCase("CIRCUT"))
   {
      closed = TRUE;
   }
   else if (!type.CompareNoCase("COPCIR") || !type.CompareNoCase("COPCLS"))
   {
      closed = TRUE;
      filled = TRUE;
      width = 0; // COPPER elements are always a line width of 0, width from pads is hatch width

   }
   else if (!type.CompareNoCase("KPTCLS") || !type.CompareNoCase("KPTCIR"))
   {
      closed = TRUE;
      width = 0;
   }
   else if (!type.CompareNoCase("BRDCLS") || !type.CompareNoCase("BRDCIR"))   // board outline cutout
   {
      closed = TRUE;
      width = 0;
      voidpoly = TRUE;
   }

   if (file_version == 40)  // - Pads reader version 4.0 added new piece types, but we are not supporting these features, yet - Joanne
   {
      if (!type.CompareNoCase("ARWLN1") || !type.CompareNoCase("ARWLN2") || !type.CompareNoCase("ARWHD1") ||
         !type.CompareNoCase("ARWHD2") || !type.CompareNoCase("EXTLN1") || !type.CompareNoCase("EXTLN2") ||
         !type.CompareNoCase("BASPNT"))
      {

         fprintf(ferr, "Doesn't support [%s] Piece type.\n", type);
         display_error++;
      }

   }

   widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
    
   if (pinnr)  // copper can be attached to a pin.
   {
      if (!STRCMPI(linetype, "DECAL"))
      {
         PADSPincopper *pinCopper = new PADSPincopper;
         pincopperarray.SetAtGrow(pincoppercnt++, pinCopper);
         pinCopper->pinnr = pinnr;
         pinCopper->layer = layer;

         CString pingraphic;
         pingraphic.Format("%s %d", shapename, pinnr);
         pinCopper->pinblock = Graph_Block_On(GBO_APPEND, pingraphic, file->getFileNumber(), 0);
      }
   }

   DataStruct *data = Graph_PolyStruct(level, 0, 0);
   if (STRCMPI(linetype, "DECAL") == 0 && is_layer_compoutline(layername))
      data->setGraphicClass(GR_CLASS_COMPOUTLINE);
   if (strlen(name))
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NAME, 1), VT_STRING, (void *)name, SA_APPEND, NULL);
   if (!STRICMP(linetype, "BOARD"))
      data->setGraphicClass(GR_CLASS_BOARDOUTLINE);

   // need a function to shrink a polygon by width.
   CPoly *lastpoly = Graph_Poly(NULL, widthindex, filled , voidpoly, closed); // fillflag , negative, closed
   Point2 pointFirst;
   Point2 point;

   for (int i=0; i<numcoo; i++)
   {
      //incrementInputFileLineNumber();     // here get prosa
      get_nextline(ifp_line, MAX_LINE, ifp);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;

      point.x = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;

      point.y = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
      {
         double ab = atof(lp) / 10;
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;

         double aa = atof(lp) / 10;
         if(aa >= 360)
         {
            fprintf(ferr, "PADS Syntax error at %ld\n", getInputFileLineNumber());
            display_error++;
            aa = 0;           
         }
         point.bulge = tan(DegToRad(aa) / 4);
      }
      else
      {
         point.bulge = 0;
      }

      if (!type.CompareNoCase("CIRCLE") || !type.CompareNoCase("CIRCUT") || !type.CompareNoCase("COPCIR") ||
         !type.CompareNoCase("KPTCIR") || !type.CompareNoCase("BRDCIR"))
         point.bulge = 1;

      // there can be an error in PADS to have the arc start as the last element.
      if (i == numcoo-1 && !closed)
         point.bulge = 0;
      
      Graph_Vertex(point.x + x, point.y + y, point.bulge);
      if (i==0)
         pointFirst = point;
   }

   if (closed)
   {
      if (!type.CompareNoCase("COPCIR") || !type.CompareNoCase("CIRCUT") || !type.CompareNoCase("CIRCLE") ||
         !type.CompareNoCase("KPTCIR") || !type.CompareNoCase("BRDCIR"))
         Graph_Vertex(pointFirst.x + x, pointFirst.y + y, 0.0);
      else if (point.x != pointFirst.x || point.y != pointFirst.y)
         Graph_Vertex(pointFirst.x + x, pointFirst.y + y, 0.0);
   }
   else
   {
      // check if it was OPEN but should be closed
      if (point.x == pointFirst.x && point.y == pointFirst.y)
         lastpoly->setClosed(true);
   }

   // here multiply keepout 
   if (!STRICMP(linetype, "KEEPOUT"))
   {
      if (strlen(restrictions) == 0) // generic
      {
         data->setGraphicClass(GR_CLASS_ALLKEEPOUT);
      }
      else
      {
         int first = TRUE;
         for (int r=0; r<(int)strlen(restrictions); r++)
         {
            int gclass = 0;
            switch (restrictions[r])
            {
            case 'P':   // = placement       
               gclass = GR_CLASS_PLACEKEEPOUT;
               break;
            case 'H':   // = componment height  
               gclass = GR_CLASS_PLACEKEEPOUT;
               break;
            case 'R':   // = trace              
               gclass = GR_CLASS_ROUTKEEPOUT;
               break;
            case 'C':   // = copper and plane  
               gclass = GR_CLASS_ROUTKEEPOUT;
               break;
            case 'V':   // = via             
               gclass = GR_CLASS_VIAKEEPOUT;
               break;
            case 'T':   // = test point         
               gclass = graphicClassTestPointKeepOut;
               break;
            default: 
               fprintf(ferr, "Unknown restriction [%c] in KEEPOUT at %ld\n", restrictions[r], getInputFileLineNumber());
               display_error++;
               continue; // unknown
               break;
            }        

            if (first)
            {
               data->setGraphicClass(gclass);
            }
            else
            {
#if CamCadMajorMinorVersion > 406  //  > 4.6
               DataStruct *newData = camCadData.getNewDataStruct(*data);
#else
               DataStruct *newData = new DataStruct(*data);
#endif
               newData->setGraphicClass(gclass);
               AddEntity(newData);
            }
            first = FALSE;
         }
      }
   }

   // if ((!STRICMP(linetype, "COPPER") || !type.CompareNoCase("COPCIR") || !type.CompareNoCase("COPCLS")) && !(layer > max_copper_layer))
   // {
   //    data->setGraphicClass(GR_CLASS_ETCH);
   // }

   if (pinnr)
   {
      if (!STRCMPI(linetype, "DECAL"))
         Graph_Block_Off();
   }

   return data;
}

/******************************************************************************
* get_terminal
*/
static int get_terminal(const char *name)
{
   get_nextline(ifp_line, MAX_LINE, ifp);

   char *lp;
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   if (lp[0] != 'T')                            
      return 0;
   lp[0] = ' ';

   double x = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double y = cnv_unit(atof(lp));

   // Currently we are ignoring the field "nmx", "nmy".
   lp = strtok(NULL, " \t\n"); // swallow nmx
   lp = strtok(NULL, " \t\n"); // swallow nmy

   // Case dts0100422405
   // If there is another field then we have pads 2007 format and it is the pin refname
   CString refname;
   if ((lp = strtok(NULL, " \t\n")) != NULL)
   {
      refname = lp;
   }

   PADSTerminal terminal(refname, x, y, -1, false);

   terminalarray.SetAtGrow(terminalcnt++, terminal);  

   return 1;
}

/******************************************************************************
* get_padstackindex
*/
static int get_padstackindex(double drillsize ,bool platedFlag,int *pad_stackarray)
{
	int i=0;
   for (i=0; i<paddefarray.GetCount(); i++)
   {
      PADS_Paddef& paddef = *(paddefarray.getAt(i));

		// Case 1660,  Logic in comparision was backwards. Should be ">", not "<".
		// Ended up finding wrong pad.
      if (fabs(paddef.getDrillSize() - drillsize) > 0.001)
      {
         continue;
      }

      if (paddef.getPlatedFlag() != platedFlag)
      {
         continue;
      }

      bool found = true;

      for (int k=0; k<MAX_PADSTACK_SIZE; k++)
      {
         if (paddef.padstack[k] != pad_stackarray[k])
         {
            found = false;
            break;
         }
      }
      
      if (found)
      {
         return i;
      }
   }

   PADS_Paddef& paddef = *(paddefarray.getAt(paddefarray.GetCount()));
   paddef.setDrillSize(drillsize);
   paddef.setPlatedFlag(platedFlag);
   paddef.typ = 0;

   for (i=0; i<MAX_PADSTACK_SIZE; i++)
   {
      paddef.padstack[i] = pad_stackarray[i];

      if (i == 0 && pad_stackarray[i] > -1)  // top
         paddef.typ |= 0x1;

      if (i == 1 && pad_stackarray[i] > -1)  // inner
         paddef.typ |= 0x3;

      if (i == 2 && pad_stackarray[i] > -1)  // bottom
         paddef.typ |= 0x2;
   }

   if (paddef.getDrillSize() > 0.)
      paddef.typ |= 0x4;

   paddef.setPadstackName("");
     
   write_single_padstack(paddefarray.GetCount() - 1);

   return paddefarray.GetCount() - 1;
}

/******************************************************************************
* get_stack
*/
static int get_stack(const char *name)
{
   //incrementInputFileLineNumber();     // here get prosa
   get_nextline(ifp_line, MAX_LINE, ifp);
   
   char *lp;

   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return -1;  // PAD

   // if this is not the PAD keyword -- error
   if (STRCMPI(lp, "PAD"))
      return -1;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return -1;

   int pinnumber = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return -1;

   int padcnt = atoi(lp);

   int pad_stack[MAX_PADSTACK_SIZE]; // -2 .. 30

	int i=0;
   for (i=0; i<MAX_PADSTACK_SIZE; i++)
      pad_stack[i] = -1;

   double drill = 0;
   bool platedFlag = true;

   for (i=0; i<padcnt; i++)
   {
      double sizeA = 0, sizeB = 0, offset = 0;
      int rotation = 0;

      //incrementInputFileLineNumber();     // here get prosa
      get_nextline(ifp_line, MAX_LINE, ifp);
      // here make a padstack
      lp = strtok(ifp_line, " \t\n");
      int layerNum = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
      {
         fprintf(ferr, "Syntax Error: Size expected at %ld\n", getInputFileLineNumber());           
         display_error++;
         return -1;
      }

      sizeA = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) == NULL)
      {
         fprintf(ferr, "Syntax Error: Padform expected at %ld\n", getInputFileLineNumber());           
         display_error++;
         return -1;
      }

      CString formName = lp;
      int form = T_UNDEFINED;

      if (!strcmp(lp, "R"))   // round
      {
         // nothing
         form = T_ROUND;
      }
      else if (!strcmp(lp, "S")) // square
      {
         // nothing
         form = T_SQUARE;
      }
      else if (!strcmp(lp, "A")) // annular
      {
         form = T_DONUT;
         lp = strtok(NULL, " \t\n");
         sizeB = cnv_unit(atof(lp));
      }
      else if (!strcmp(lp, "O")) // odd
      {
         form = T_TARGET;     // as per Dino
      }
      else if (!strcmp(lp, "OF"))   // oval finger
      {
         form = T_OBLONG;
         lp = strtok(NULL, " \t\n");
         rotation = atoi(lp);
         lp = strtok(NULL, " \t\n");
         sizeB = sizeA;
         sizeA = cnv_unit(atof(lp));
         lp = strtok(NULL, " \t\n");
         offset = cnv_unit(atof(lp));
      }
      else if (!strcmp(lp, "RF"))   // rectangle finger
      {
         form = T_RECTANGLE;
         lp = strtok(NULL, " \t\n");
         rotation = atoi(lp);
         lp = strtok(NULL, " \t\n");
         sizeB = sizeA;
         sizeA = cnv_unit(atof(lp));
         lp = strtok(NULL, " \t\n");
         offset = cnv_unit(atof(lp));
      }
      else if (!strcmp(lp, "RT"))   // round thermal
      {
         if (ignoreStrangePadshapes) continue;
         form = T_ROUND;
      }
      else if (!strcmp(lp, "RA"))   // round ???
      {
         if (ignoreStrangePadshapes) continue;
         form = T_ROUND;
      }
      else if (!strcmp(lp, "ST"))   // square thermal
      {
         if (ignoreStrangePadshapes) continue;
         form = T_SQUARE;
/*
         lp = strtok(NULL," \t\n");
         rotation = atoi(lp);
         lp = strtok(NULL," \t\n");
         sizeB = sizeA;
         sizeA = cnv_unit(atof(lp));
         lp = strtok(NULL," \t\n");
         offset = cnv_unit(atof(lp));
*/
      }
      else
      {
         fprintf(ferr, "Unknown DECAL padform [%s] at %ld -> skipped\n", lp, getInputFileLineNumber()); 
         continue;
      }

      if (i == 0) // get drill
      {
         lp = strtok(NULL, " \t\n");
         drill = cnv_unit(atof(lp));

         lp = strtok(NULL, " \t\n");

         if (lp != NULL && *lp == 'N')
         {
            platedFlag = false;
         }
      }
   
      if (sizeA == 0)
         continue;   // dummy stack

      // here get padfrom index
      BlockStruct *block = Graph_FindAperture(form, sizeA, sizeB, offset, 0, DegToRad(rotation), 0, TRUE, FALSE);
      pad_stack[layerNum + 2] = block->getBlockNumber();
   }

   // here now check Paddef index of already defined
   int psindex = get_padstackindex(drill,platedFlag, &pad_stack[0]);

   // The pinnumber == 0 is a special setting, it means all pins except 
   // those explicitly listed. Spec says "except explicitly listed later".
   // LATER! But DR 672509 presents data where PAD 0 is in the middle of
   // the comp (a QFP) and so the ones explicitly defined earlier were
   // being wiped out, which made comp wrong. So we honor all those
   // explicitly set already as well as later.
   if (pinnumber == 0)
   {
      for (i=0; i<terminalcnt; i++)
      {
         PADSTerminal terminal = terminalarray.ElementAt(i);
         if (!terminal.GetExplicitlySet())
         {
            // This is a default setting, so do not set ExplicitlySet
            terminal.SetPadstackIndex(psindex);
            terminalarray.SetAtGrow(i, terminal);
         }
      }
   }
   else
   {
      PADSTerminal terminal = terminalarray.ElementAt(pinnumber-1);
      terminal.SetPadstackIndex(psindex);
      terminal.SetExplicitlySet(true);  // Yes, this specific pin is explicitly set.
      terminalarray.SetAtGrow(pinnumber - 1, terminal);     
   }

   return 1;
}

/******************************************************************************
* is_pad_layertype
*/
static int is_pad_layertype(const char *lname, int ltype)
{
   for (int i=0; i<layer_attr_cnt; i++)
   {
      if (!STRCMPI(layer_attr[layer_attr_cnt].layerName, lname))
      {
         return (layer_attr[layer_attr_cnt].layerType == ltype);
      }
   }
   return 0;
}

/******************************************************************************
* get_viastack
   layer 1 top is +3 offset

   // -2 .. 250 is 0..252 with 
    0 is top, 1 is mid 2 is bottom 3 is exploded top .. 252 is exploded maxlayer
*/
static void get_viastack(const char *padstackName, int padCount, double drillSize, int startDrillLevelNum, int endDrillLevelNum)
{
   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, padstackName, file->getFileNumber(), 0);
   curblock->setBlockType(blockTypePadstack);

   bool blindFlag = false;
   if (startDrillLevelNum != 1 || endDrillLevelNum != maxlayer)
   {
      blindFlag = true;
      doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE, NULL, attributeUpdateOverwrite, NULL);
   }

   CArray<int,int> innerPads;
   CArray<BOOL,BOOL> innerPadNegative;
   int maxInnerPads = 0;
   BOOL *used = new BOOL[endDrillLevelNum + 1];
	
	int i=0;
   for (i=0; i<=endDrillLevelNum; i++)
      used[i] = FALSE;     

   // for each line of via definition in PADS file
   for (i=0; i<padCount; i++)
   {
      get_nextline(ifp_line, MAX_LINE, ifp);

      char *tok = strtok(ifp_line, " \t\n");
      int levelNum = atoi(tok);

      tok = strtok(NULL, " \t\n");
      double sizeA = cnv_unit(atof(tok));
      double sizeB = 0;

      tok = strtok(NULL, " \t\n");
      CString shapeName = tok;
      int shape = T_UNDEFINED;
      int rotation = 0;
      int spokeNum = 0;
      BOOL Negative = FALSE;

      if (!shapeName.CompareNoCase("R")) // round
      {
         shape = T_ROUND;
      }
      else if (!shapeName.CompareNoCase("RA"))  // round antipad
      {
         if (ignoreStrangePadshapes) continue;
         shape = T_ROUND;
         Negative = TRUE;
      }
      else if (!shapeName.CompareNoCase("A")) // round -- do not make a donut here!
      {
         shape = T_ROUND;
         tok = strtok(NULL, " \t\n");
         sizeB = cnv_unit(atof(tok));
      }
      else if (!shapeName.CompareNoCase("S")) // square
      {
         shape = T_SQUARE;
      }
      else if (!shapeName.CompareNoCase("SA"))  // square antipad
      {
         if (ignoreStrangePadshapes) continue;
         shape = T_SQUARE;
         Negative = TRUE;
      }
      else if (!shapeName.CompareNoCase("RT"))  // round thermal
      {
         if (ignoreStrangePadshapes) continue;
         shape = T_THERMAL;
         tok = strtok(NULL, " \t\n");
         sizeA = cnv_unit(atof(tok));

         tok = strtok(NULL, " \t\n");
         rotation = atoi(tok);

         tok = strtok(NULL, " \t\n");
         double spokeWidth = cnv_unit(atof(tok));
         sizeB = sizeA - spokeWidth;

         tok = strtok(NULL, " \t\n");
         spokeNum = atoi(tok);
      }
      else if (!shapeName.CompareNoCase("ST"))  // square thermal
      {
         if (ignoreStrangePadshapes) continue;
         shape = T_SQUARE;
      }
      else
      {
         fprintf(ferr, "Unknown VIA padform [%s] at %ld -> skipped\n", shapeName, getInputFileLineNumber()); 
         continue;
      }

      if (fabs(sizeA) < SMALLNUMBER)
         continue;

      BlockStruct *apertureGeometry = Graph_FindAperture(shape, sizeA, sizeB, 0, 0, DegToRad(rotation), 0, TRUE, FALSE);
      apertureGeometry->setSpokeCount(spokeNum);

      if (levelNum == -2) // TOP
         levelNum = 1;

      if (levelNum == 0) // BOTTOM
         levelNum = maxlayer;

      CString layerName;
      if (levelNum == 1)
         layerName = "PADLAYER_TOP";
      else if (levelNum == maxlayer)
         layerName = "PADLAYER_BOT";
      else if (levelNum == -1)
         layerName = "PADLAYER_INNER";
      else
         layerName.Format("PADLAYER_%d", levelNum); // case 413831 reverted to pre case 376877,  case 376877 used:    layerName.Format("%d", levelNum);

      int layerNum = Graph_Level(layerName, "", 0);
      bool blindElectricalLayerFlag = (blindFlag && levelNum <= maxlayer);

      if (blindElectricalLayerFlag)
      {
         if (levelNum == -1) // INNER
         {
            innerPads.SetAtGrow(maxInnerPads, apertureGeometry->getBlockNumber());
            innerPadNegative.SetAtGrow(maxInnerPads, Negative);
            maxInnerPads++;
            continue;
         }

         if (levelNum < startDrillLevelNum || levelNum > endDrillLevelNum)
            continue;

         used[levelNum] = TRUE;
      }

      DataStruct *padInsert = Graph_Block_Reference(apertureGeometry->getName(), NULL, file->getFileNumber(), 0, 0, 0, 0, 1, layerNum, TRUE);
      padInsert->setNegative(Negative);

      if (!blindElectricalLayerFlag)
      {
         if (is_pad_layertype(layerName, LAYTYPE_POWERNEG))
            padInsert->getInsert()->setInsertType(insertTypeClearancePad);
         if (is_pad_layertype(layerName, LAYTYPE_DRILL))
            padInsert->getInsert()->setInsertType(insertTypeDrillSymbol);
      }
   }

   if (blindFlag) // add inner pads to undefined inner layers between startDrillLevelNum and endDrillLevelNum
   {
      for (int i=startDrillLevelNum; i<=endDrillLevelNum; i++)
      {
         if (used[i])
            continue;

         for (int j=0; j<maxInnerPads; j++)
         {
            CString layerName;
            if (i == 1)
               layerName = "PADLAYER_TOP";
            else if (i == maxlayer)
               layerName = "PADLAYER_BOT";
            else
               layerName.Format("%d", i);

            int layerNum = Graph_Level(layerName, "", 0);

            DataStruct *padInsert = Graph_Block_Reference(doc->getBlockAt(innerPads[j])->getName(), NULL, file->getFileNumber(), 0, 0, 0, 0, 1, layerNum, TRUE);
            padInsert->setNegative(innerPadNegative[j]);
         }
      }
   }

   if (drillSize > 0)
   {
      BlockStruct *drillGeometry = Graph_FindTool(drillSize, 0, TRUE,true);
      DataStruct* toolData = Graph_Block_Reference(drillGeometry->getName(), NULL, file->getFileNumber(), 0, 0, 0, 0, 1, drilllayernum, TRUE);
      toolData->getInsert()->setInsertType(insertTypeDrillHole);
   }

   Graph_Block_Off();
   delete[] used;
}

/******************************************************************************
* get_text
*/
static int get_text(double x, double y, const char *name, double *h)
{
   *h = 0.0;
   get_nextline(ifp_line, MAX_LINE, ifp);

   char *lp;
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   double x1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double y1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rot = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   if (!STRCMPI(lp, "0"))     // layer 0 is all
      lp = "ALL";

	//int level = -1;
	//if (atoi(lp) == 1)
	//	level = GetSilkcreenTopLayer();
	//else if (atoi(lp) == maxlayer)
	//	level = GetSilkscreenBottomLayer();

	//if (level < 0)
	//	level = Graph_Level(lp, "", 0);

	int level = Graph_Level(lp, "", 0);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double height = cnv_unit(atof(lp)) * TEXT_HEIGHTFACTOR;
   double width = height * TEXT_WIDTHFACTOR;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double penwidth = cnv_unit(atof(lp));

   int mirror = FALSE;
   if ((lp = strtok(NULL, " \t\n")) != NULL)
   {
      if (lp[0] == 'M')
         mirror = TRUE;
   }

   int ta = 0; // CAMCAD textAlignment is Pads HJUST
   if ((lp = strtok(NULL," \t\n")) != NULL) // HJUST
   {
      switch (lp[0])
      {
         case 'L':
         case 'l':
            ta = 0;
            break;
         case 'C':
         case 'c':
            ta = 1;
            break;
         case 'R':
         case 'r':
            ta = 3;
            break;
      }
   }

   int la = 0;
   if ((lp = strtok(NULL," \t\n")) != NULL) // HJUST
   {
      switch (lp[0])
      {
         case 'D':
         case 'd':
            ta = 0;
            break;
         case 'C':
         case 'c':
            ta = 1;
            break;
         case 'U':
         case 'u':
            ta = 3;
            break;
      }
   }

   incrementInputFileLineNumber();    // here get prosa
   fgets(ifp_line, MAX_LINE, ifp);

   // may be an empty text line
   if ((lp = strtok(ifp_line, "\n")) == NULL)
      return 1;
   
	// Case 1668
	// If version 2005, current line is font, get text from next line
	CString b4 = ifp_line;
	if (file_version >= 20050)
	{
		// Skip font name in current line, get next line
		incrementInputFileLineNumber();
		fgets(ifp_line, MAX_LINE, ifp);
		lp = strtok(ifp_line, "\n");
	}

   if (lp == NULL || strlen(lp) == 0)
      return 1;

   if (!STRNICMP(lp, "ZHEIGHT", 7))
      *h = atof(&lp[8]);
   else
      Graph_Text(level, lp, x1 + x, y1 + y, height, width, DegToRad(rot), 0, TRUE, mirror, 0, FALSE, -1, 0, ta, la);

   return 1;
}

/******************************************************************************
* get_label
*     VISIBLE_LX_LY_LORI_LHEIGTH_LWIDTH_LLEVEL_MIRRORED_HJUST_VJUST_[RIGHT_READING]
*/
static int get_label(CAttributes** map, double x, double y, const char *name)
{
   char *lp;
   char tmp[80];  // temp label name

   get_nextline(ifp_line, MAX_LINE, ifp);
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   CString visible = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double x1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double y1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rot = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   if (!STRCMPI(lp,"0"))
      lp = "ALL";
   else
   {
      int lev = atoi(lp);
      sprintf(tmp, "DECAL_%d", lev);
      lp = tmp;
   }
   int level = Graph_Level(lp, "", 0);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double height = cnv_unit(atof(lp)) * TEXT_HEIGHTFACTOR;
   double width = height * TEXT_WIDTHFACTOR;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double penwidth = cnv_unit(atof(lp));

   int mirror = FALSE;
   if ((lp = strtok(NULL, " \t\n")) != NULL) 
   {
      if (lp[0] == 'M')
         mirror = TRUE;
   }

   incrementInputFileLineNumber();
   fgets(ifp_line, MAX_LINE, ifp);

	// Case 1668
	if (file_version >= 20050)
	{
		// Skip font name in current line, get next line
		incrementInputFileLineNumber();
		fgets(ifp_line, MAX_LINE, ifp);
	}

   // may be an empty text line
   if ((lp = strtok(ifp_line,"\n")) == NULL)
      return 1;
   if (lp == NULL || strlen(lp) == 0)
      return 1;
   CString key = get_attribmap(lp);

   if (!STRCMPI(lp, "Part Type"))
   {
      // do not overwrite the DEVICE attribute.
   }
   else if (!key.CompareNoCase(ATT_TYPELISTLINK))
   {
      // do not overwrite the DEVICE attribute.
   }
   else if (!key.CompareNoCase(ATT_REFNAME))
	{
      CString namestr(name);
		WORD keyword = doc->RegisterKeyWord(key, 0, VT_STRING);
		Attrib *attrib = NULL;
		if ((*map) != NULL && (*map)->Lookup(keyword, attrib))
		{
         CString attribval( attrib->getStringValue() );

			if (!attribval.CompareNoCase("DECAL"))
			{
            // Don't need a visible refname attrib if name is constant "DECAL"
            if (namestr.CompareNoCase("DECAL") != 0)
               doc->SetUnknownVisAttrib(map, key, name, x1 + x, y1 + y, DegToRad(rot), height, width, 0, 0, TRUE, attributeUpdateOverwrite, 0L, level, 0, 0, 0);
			}
			else
			{ 
				Attrib *tmpAttrib = new Attrib(*attrib);
				tmpAttrib->setProperties(x1 + x, y1 + y, DegToRad(rot), height, width, 0, 0, TRUE, 0, 0, 0, horizontalPositionLeft, verticalPositionBaseline, level, 0L);
				attrib->addInstance(tmpAttrib);
			}
		}
		else
		{
         if (namestr.CompareNoCase("DECAL") != 0)
            doc->SetVisAttrib(map, keyword, VT_STRING, (void*)name, x1 + x, y1 + y, DegToRad(rot), height, width, 0, 0, TRUE, attributeUpdateOverwrite, 0L, level, 0, 0, 0);
		}
	}
   else
      doc->SetUnknownVisAttrib(map, key, "", x1 + x, y1 + y, DegToRad(rot), height, width, 0, 0, TRUE, attributeUpdateOverwrite, 0L, level, 0, 0, 0);

   return 1;
}

/******************************************************************************
* int in_pincopper
*/
static int in_pincopper(int pinnr)
{
   for (int i=0; i<pincoppercnt; i++)
   {
      if (pincopperarray[i]->pinnr == pinnr)
         return i;
   }
   return -1;
}

/******************************************************************************
* attach_pincopper
*/
static int attach_pincopper(int pinnr, DataStruct *pininstance)
{
   int pinptr = in_pincopper(pinnr);

   if (pinptr > -1)
   {
#ifdef _DEBUG
   PADSPincopper *p = pincopperarray[pinptr];
#endif

      BlockStruct *newpinshape = Graph_Block_On(GBO_APPEND, "~", file->getFileNumber(), 0);
      newpinshape->setBlockType(blockTypePadstack);

      // pininstance is the datastruct of the pin.
      // this has a padstack attached.
   
      // the pincopperarray->pinblock needs to be normalize to the pin koos
      POSITION insertPos = pincopperarray[pinptr]->pinblock->getDataList().GetHeadPosition();
      while (insertPos)
      {
         DataStruct *subData = pincopperarray[pinptr]->pinblock->getDataList().GetNext(insertPos);
         DataStruct *newData = CopyTransposeEntity(subData, -pininstance->getInsert()->getOriginX(), -pininstance->getInsert()->getOriginY(), 
               0.0, 0, 1.0, -1, TRUE);
         newpinshape->getDataList().AddTail(newData);
      }

      // now copy the existing padstack to the newpinshape
      BlockStruct *oldpin = doc->Find_Block_by_Num(pininstance->getInsert()->getBlockNumber());
      insertPos = oldpin->getDataList().GetHeadPosition();
      while (insertPos)
      {
         DataStruct *subData = oldpin->getDataList().GetNext(insertPos);
         DataStruct *newData = CopyTransposeEntity(subData, 0.0, 0.0, 0.0, 0, 1.0, -1, TRUE);
         newpinshape->getDataList().AddTail(newData);
      }

      // the current padname has to be added to the pincopper block
      Graph_Block_Off();

      CString name = pincopperarray[pinptr]->pinblock->getName();
      // remove old pincopperblock
      doc->RemoveBlock(pincopperarray[pinptr]->pinblock);

      // the pincopper block needs to be added to pininstance.
      pininstance->getInsert()->setBlockNumber(newpinshape->getBlockNumber());
      newpinshape->setName(name);
   }

   return 1;
}

/******************************************************************************
* pads_decal
*/
static int pads_decal()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString name = lp;
      
      pincoppercnt = 0;

      // check that name does not exist
      int exist_cnt = 0;
      while (TRUE)
      {
         if (Graph_Block_Exists(doc, name, file->getFileNumber()))
         {
            fprintf(ferr, "DECAL [%s] already defined\n", name);           
            display_error++;
            CString  tmp;
            tmp.Format("%s_%d", name, ++exist_cnt);
            name = tmp;
         }
         else
            break;
      }

      BlockStruct *block = Graph_Block_On(GBO_APPEND, name, file->getFileNumber(), 0);

	   if (is_geom(block->getName(), FID_GEOM) != 0)
		   block->setBlockType(blockTypeFiducial);
	   else if (is_geom(block->getName(), TOOL_GEOM) != 0)
		   block->setBlockType(blockTypeTooling);
	   else
		   block->setBlockType(blockTypePcbComponent);

      // pads V5 has no decal units
      if (file_version == 4)
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         CString unit = lp;
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double nmx = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double nmy = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double nmori = atof(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int pieces = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int termcnt = atoi(lp);    
      if (!termcnt)  // has not pins
         block->setBlockType(0); // this is not a PCB component.

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int stackcnt = atoi(lp);

      int textcnt= 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         textcnt = atoi(lp);

		int i=0;
      for (i=0; i<pieces; i++)
         get_pieces(0,0, "", "DECAL", name); // no piecename needed, but shapename is needed

      double height;
      for (i=0; i<textcnt; i++)
      {
         get_text(0, 0, name, &height);
         if (height > 0)
         {
            double h = cnv_unit(height);
            doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &h, attributeUpdateOverwrite, NULL); 
         }

      }

      terminalcnt = 0;
      for (i=0; i<termcnt; i++)
         get_terminal(name);

      for (i=0; i<stackcnt; i++)
      {
         if (get_stack(name) < 0)            
         {
            CString tmp;
            tmp.Format("Big Syntax error at line %ld -> Padstack records expected!", getInputFileLineNumber());
            fprintf(ferr, "%s\n", tmp);
            display_error++;
            tmp += "\nNeed to generate a new PADS ASCII file.";
            ErrorMessage(tmp, "Invalid PADS ASCII file!");
            return -1;
         }  
      }

      // here write all terminals
      for (i=0; i<terminalcnt; i++)
      {
         // here place pin
         PADSTerminal terminal = terminalarray.ElementAt(i);
         CString pinnr;
         pinnr.Format("%d", i+1);
         
         // i = ignore
         // f = fiducial
         // n = normal
         // non_electrical_pins
         PADS_Paddef& paddef = *(paddefarray.getAt(terminal.GetPadstackIndex()));

         // also now need to watch out that it does not appear in the netlist.
         if (non_electrical_pins == 'i' && (paddef.typ & 3) == 0 && in_pincopper(atoi(pinnr)) < 0)
         {
            PADSPadsIgnored *padIgnored = new PADSPadsIgnored;
            padsignoredarray.SetAtGrow(padsignoredcnt, padIgnored);
            padsignoredcnt++;
            padIgnored->decalname = name;
            padIgnored->pinnr = atoi(pinnr);
            padIgnored->pinname = pinnr;
            continue;
         }

         DataStruct *data = Graph_Block_Reference(paddef.getPadstackName(), pinnr, file->getFileNumber(), terminal.GetX(), terminal.GetY(), DegToRad(0.0), 0 , 1.0, -1, TRUE);
         if (non_electrical_pins == 'f' && (paddef.typ & 3) == 0)
            data->getInsert()->setInsertType(insertTypeFiducial);
         else
            data->getInsert()->setInsertType(insertTypePin);

         attach_pincopper(atoi(pinnr), data);

         // do pinnr here
         if (strlen(pinnr))
         {
            int pnr = atoi(pinnr);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, TRUE), VT_INTEGER, &pnr, attributeUpdateOverwrite, NULL);
         }
      }

      for (i=0; i<pincoppercnt; i++)
         delete pincopperarray[i];

      Graph_Block_Off();
   }

   return 1;
}

/******************************************************************************
* pad6_decal
*/
static int pad6_decal()
{
   char oldunit =  unit_flag;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char  *lp;
      CString unit;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString name = lp;

      pincoppercnt = 0;    
      unit_flag = oldunit;

      // check that name does not exist
      int exist_cnt = 0;
      while (TRUE)
      {
         if (Graph_Block_Exists(doc, name, file->getFileNumber()))
         {
            fprintf(ferr, "DECAL [%s] already defined\n", name);           
            display_error++;
            CString  tmp;
            tmp.Format("%s_%d", name,++exist_cnt);
            name = tmp;
         }
         else
            break;
      }

      BlockStruct *block = Graph_Block_On(GBO_APPEND, name, file->getFileNumber(), 0);

	   if (is_geom(block->getName(), FID_GEOM) != 0)
		   block->setBlockType(blockTypeFiducial);
	   else if (is_geom(block->getName(), TOOL_GEOM) != 0)
		   block->setBlockType(blockTypeTooling);
	   else
		   block->setBlockType(blockTypePcbComponent);

      char decalunit = ' ';

      if (unit_flag != 'P')
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;

         decalunit = lp[0];

         if (oldunit == 'B')
         {
            // if oldunits are in BASIC format, do nothing
         }
         else if (file_version >= 15)
         {
            if (lp[0] == 'I')
               unit_flag = 'A';
            else if (lp[0] == 'M')
               unit_flag = 'M';
         }
         else
         {
            if (oldunit == 'M' && lp[0] == 'I')
               unit_flag = 'A';
            else if (oldunit != 'B' && lp[0] == 'M')
               unit_flag = 'M';
         }
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      double nmx;
      double nmy;
      double nmori;

      if (file_version < 30)
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;

         nmx = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;

         nmy = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;

         nmori = atof(lp);
      }
      else
      {
         nmx = 0;
         nmy = 0;
         nmori = 0;
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int pieces = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int termcnt = atoi(lp);

      if (termcnt == 0) // this is not a pcb component.
         block->setBlockType(0);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int stackcnt = atoi(lp);

      int textcnt= 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         textcnt = atoi(lp);

      // pads power version 3
      int labelcnt = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         labelcnt = atoi(lp);

      if (pieces < 0)
      {
         fprintf(ferr, "Bad number of Drawing Pieces %d at %ld\n", pieces, getInputFileLineNumber());
         display_error++;
         continue;
      }

      if (textcnt < 0)
      {
         fprintf(ferr, "Bad number of Text Pieces %d at %ld\n", textcnt, getInputFileLineNumber());
         display_error++;
         continue;
      }

      if (labelcnt < 0)
      {
         fprintf(ferr, "Bad number of Label Pieces %d at %ld\n", labelcnt, getInputFileLineNumber());
         display_error++;
         continue;
      }

		int i=0; 
      for (i=0; i<pieces; i++)
         get_pieces(0, 0, "", "DECAL", name);   // no piecename needed, but shapename
   
      double height;

      for (i=0; i<textcnt; i++)
      {
         char save_unit = unit_flag;
         get_text(0, 0, name, &height);

         if (height > 0)
         {
            if (decalunit == 'I')
               unit_flag = 'A';
            else if (decalunit == 'M')
               unit_flag = 'M';

            double h = cnv_unit(height);
            doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &h, attributeUpdateOverwrite, NULL); // x, y, rot, height
         }

         unit_flag = save_unit;
      }

      for (i=0; i<labelcnt; i++)
		{
			int res = get_label(&block->getAttributesRef(), 0, 0, "DECAL"); // no name needed

			if (res < 1)
			{
            CString  tmp;
            tmp.Format("Label syntax error at line %ld -> Label records expected!", getInputFileLineNumber());

            fprintf(ferr, "%s\n", tmp);
            display_error++;
            ErrorMessage(tmp, "Invalid PADS ASCII file!");
            return -1;
			}
		}

		terminalcnt = 0;
      for (i=0; i<termcnt; i++)
         get_terminal(name);

      for (i=0; i<stackcnt; i++)
      {
         int res = get_stack(name);

         if (res < 0)
         {
            CString  tmp;
            tmp.Format("Big Syntax error at line %ld -> Padstack records expected!", getInputFileLineNumber());
            fprintf(ferr, "%s\n", tmp);
            display_error++;
            tmp += "\nNeed to generate a new PADS ASCII file.";
            ErrorMessage(tmp, "Invalid PADS ASCII file!");
            return -1;
         }  
      }

      // here write all terminals
      for (i=0; i<terminalcnt; i++)
      {
         // here place pin
         PADSTerminal terminal= terminalarray.ElementAt(i);

         CString pinrefname( terminal.GetRefname() );
         if (pinrefname.IsEmpty())
            pinrefname.Format("%d", i+1);
         
         

         // i = ignore
         // f = fiducial
         // n = normal
         // non_electrical_pins
         PADS_Paddef& paddef = *(paddefarray.getAt(terminal.GetPadstackIndex()));

         // also now need to watch out that it does not appear in the netlist.
         if (non_electrical_pins == 'i' && (paddef.typ & 3) == 0 && in_pincopper(i+1) < 0)
         {
            PADSPadsIgnored *padsIgnored = new PADSPadsIgnored;
            padsignoredarray.SetAtGrow(padsignoredcnt, padsIgnored);
            padsignoredcnt++;
            padsIgnored->decalname = name;
            padsIgnored->pinnr = i+1;
            padsIgnored->pinname = pinrefname;
            continue;
         }

         DataStruct *data = Graph_Block_Reference(paddef.getPadstackName(), pinrefname, file->getFileNumber(), terminal.GetX(), terminal.GetY(), DegToRad(0.0), 0 , 1.0, -1, TRUE);
         if (non_electrical_pins == 'f' && (paddef.typ & 3) == 0)
            data->getInsert()->setInsertType(insertTypeFiducial);
         else
            data->getInsert()->setInsertType(insertTypePin);

         attach_pincopper(i+1, data); // was attach_pincopper(atoi(pinnr), data);

         // do pinnr here
         if (!pinrefname.IsEmpty())
         {
            int pnr = i+1;
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, TRUE), VT_INTEGER, &pnr, attributeUpdateOverwrite, NULL);
         }
      }

      for (i=0; i<pincoppercnt; i++)
         delete pincopperarray[i];

      Graph_Block_Off();
   }

   unit_flag = oldunit;
   return 1;
}

/******************************************************************************
* get_ptypeptr
*/
static int get_ptypeptr(const char *n)
{
   for (int i=0; i<partcnt; i++)
   {
#ifdef _DEBUG
PADSPart *cc = partarray[i];
#endif
      if (!strcmp(partarray[i]->ptypename,n))
         return i;
   }
   return -1;
}

/******************************************************************************
* update_alphapin
*/
static int update_alphapin(BlockStruct *block, int pinnr, const char *pinname)
{
   int found = 0;

   DataStruct *data = get_pindata_from_comppinnr(doc, block, pinnr);
   if (data)
   {
      data->getInsert()->setRefname(STRDUP(pinname));
   }
   else
   {
      PADSPadsIgnored *padIgnored = new PADSPadsIgnored;
      padsignoredarray.SetAtGrow(padsignoredcnt++, padIgnored);
      padIgnored->decalname = block->getName();
      padIgnored->pinnr = pinnr;
      padIgnored->pinname = pinname;
   }

   return found;
}

/******************************************************************************
* int update_comppin_ignore
   here I make a list of components and ignored pins, so that the later 
   netlist check is fast.
*/
static int update_comppin_ignore(const char *compName, const char *decal)
{
   if (padsignoredcnt == 0)
      return 0;

   // c = compname
   // s = shape
   DataStruct *data = datastruct_from_refdes(doc, file->getBlock(), compName);
   if (data == NULL)
      return 0;

   BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
   if (block == NULL)
      return 0;

   for (int i=0; i<padsignoredcnt; i++)   // this has all decals and pinne
   {
      PADSPadsIgnored *padIgnored = padsignoredarray[i];
      if (padIgnored->decalname.Compare(block->getName()) == 0)
      {
         // add to the comppinignoredcnt
         PADSCompPinIgnored *compPinIgnored = new PADSCompPinIgnored;
         comppinignoredarray.SetAtGrow(comppinignoredcnt++, compPinIgnored);  
         compPinIgnored->compname = compName;
         compPinIgnored->geomname = block->getName();
         compPinIgnored->pinnr = padIgnored->pinnr;
         compPinIgnored->pinname = padIgnored->pinname;
         compPinIgnored->already_reported = 0;
      }
   }

   return 1;
}

/******************************************************************************
* set_parttype_altused
*  If only 1 alt is used per type, I can optimize this so that it does not get renamed.
*  I can use the original type and delete the other ones.
*
*/
static int set_parttype_altused(const char *ptype, int alt)
{
   int p = get_ptypeptr(ptype);
 
   return 1;
}

/******************************************************************************
* pads_part
*/
static int pads_part()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString name = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString ptype = lp;  // if TOOL_126@TOOL_126A, than tool_126A is the shape and do not use the type
      CString origptype = lp;

      CString value = "";
      CString tolerance = "";

      int found = ptype.Find(",");
      if (found > -1)
      {
         CString typeLeft = ptype.Left(found);
         value = ptype.Right(strlen(ptype) - (found + 1));

         // here seperate tolerance is string end with a %
         int found1 = value.Find(",");
         if (found1 > -1)
         {
            CString valueLeft = value.Left(found1);
            tolerance = value.Right(strlen(value) - (found1 + 1));
            value = valueLeft;
         }

         found1 = value.Find("@");
         if (found1 > -1)
         {
            CString valueLeft, valueRight;
            valueLeft = value.Left(found1);
            valueRight = value.Right(strlen(value)-(found1+1));
            value = valueLeft;
            typeLeft += "@";
            typeLeft += valueRight;
         }
         ptype = typeLeft;
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double rot = atof(lp);

      int mir = 0;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      mir = (lp[0] == 'M');

      double nmx;
      double nmy;
      double nmrot;
      if (file_version < 30)
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         nmx = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         nmy = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         nmrot = atof(lp);
      }
      else
      {
         nmx = 0;
         nmy = 0;
         nmrot = 0;
      }

      int alt = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         alt = atoi(lp);

      int CLSTID = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         CLSTID = atoi(lp);

      int CLSTATTR = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         CLSTATTR = atoi(lp);

      int BROTHERID = 0;   
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         BROTHERID = atoi(lp);

      int labelcnt = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         labelcnt = atoi(lp);

      if (mir)
         rot = 360 - rot;
      rot = clean_rot(rot);   

      DataStruct *data;
      int pointer;
      CString original_decal  = ""; // which original DECAL was used.
      char tmp[255];
      strcpy(tmp, origptype);
      if (lp = strchr(tmp, '@'))
      {
         CString shape;
         CString typ;
         strcpy(tmp, origptype);
         if (lp = strtok(tmp, "@"))    // seperate type
            typ = lp;
         if (lp = strtok(NULL, "\n"))  // and shape
            shape = lp;
         original_decal = shape;

         ptype = typ;
         origptype = typ;
         pointer = get_ptypeptr(origptype);
         if (pointer > -1)
         {
            CString newdecal;    
            newdecal.Format("%s_%s", typ,shape);

            BlockStruct *block = Graph_Block_Exists(doc, newdecal, file->getFileNumber());

            // only needs to create a new shape if there are alpha pins
            if (pointer > -1 && partarray[pointer]->alphapins == 0)
            {
               newdecal = shape;
            }
            else if (block == NULL) // this means a type@decal combination is not 
                                    // yet defined. a decal will get the pinnames from the type.
            {
               BlockStruct *block1 = Graph_Block_Exists(doc, shape, file->getFileNumber());
               if (block1 == NULL)
               {
                  CString tmp;
                  tmp.Format("PartDecal [%s] is used in Part [%s], but is not defined in DECAL.", shape, origptype);
                  ErrorMessage(tmp, "PADS read Error");
                  fprintf(ferr, "%s\n", tmp);
                  display_error++;
                  continue;
               }

					BlockStruct *block2 = Graph_Block_On(GBO_APPEND, newdecal, file->getFileNumber(), 0);
					block2->setBlockType(block1->getBlockType());
					Graph_Block_Copy(block1, 0, 0, 0, 0, 1, -1, TRUE);
					Graph_Block_Off();
					doc->SetUnknownAttrib(&block2->getAttributesRef(), ATT_DERIVED, shape, attributeUpdateOverwrite, NULL);

               // update new decal
               int pinnr = 0;
               char *tmp = STRDUP(partarray[pointer]->pinname);
               char *tmp1 = strtok(tmp,",");
               while (tmp1)
               {
                  pinnr++;
                  update_alphapin(block2, pinnr, tmp1);
                  tmp1 = strtok(NULL,",");
               }
               free(tmp);

               // update the type with the new shape
               CString typname;  
               typname.Format("%s_%s", origptype, newdecal);
               TypeStruct *newtype = AddType(file,typname);
               TypeStruct *origtypeptr = FindType(file, origptype);
               if (origtypeptr != NULL)
                  doc->CopyAttribs(&newtype->getAttributesRef(), origtypeptr->getAttributesRef());
               newtype->setBlockNumber( block2->getBlockNumber()); // b->num; pads can have multiple Decals assigned to a type.
            }
   
            data = Graph_Block_Reference(newdecal, name , file->getFileNumber(), x, y, DegToRad(rot), mir , 1.0, -1, TRUE);
				if (is_geom(newdecal, FID_GEOM) != 0)
					data->getInsert()->setInsertType(insertTypeFiducial);
				else if (is_geom(newdecal, TOOL_GEOM) != 0)
					data->getInsert()->setInsertType(insertTypeDrillTool);
				else
					data->getInsert()->setInsertType(insertTypePcbComponent);

            // check which shape this parttype has. - if it has a different on, make a new type
            // first check if and alt fits, otherwise add a new one.
            int found = FALSE;
            for (int i=0; i<partarray[pointer]->decalcnt; i++)
            {
               if (newdecal.CompareNoCase(partarray[pointer]->decalname[i]) == 0)
               {
                  found = TRUE;
                  alt = i;
               }
            }

            if (!found)
            {
               if (partarray[pointer]->decalcnt < MAX_ALTDECAL)
               {
                  partarray[pointer]->decalname[partarray[pointer]->decalcnt] = newdecal;
                  partarray[pointer]->originaldecalname[partarray[pointer]->decalcnt] = shape;
                  alt = partarray[pointer]->decalcnt++;
               
                  CString typname;
                  typname.Format("%s_%s", origptype, newdecal);
                  TypeStruct *newtype = AddType(file,typname);

                  TypeStruct *origtypeptr = FindType(file, origptype);
                  if (origtypeptr != NULL)
                     doc->CopyAttribs(&newtype->getAttributesRef(), origtypeptr->getAttributesRef());

                  doc->SetUnknownAttrib(&newtype->getAttributesRef(), ATT_DERIVED, origptype, attributeUpdateOverwrite, NULL); //  

                  BlockStruct *block = Graph_Block_Exists(doc, newdecal, -1);
                  if (block)
                     newtype->setBlockNumber( block->getBlockNumber());
               }
               else
               {
                  fprintf(ferr, "Too many alternate shapes in type@shape [%s@%s] section\n", typ, newdecal);
                  origptype = "";
               }
            }
         }
         else
         {
            fprintf(ferr,"Component Decal [%s] not found at %ld\n",origptype, getInputFileLineNumber());
            display_error++;
         }
      }
      else
      {
         pointer = get_ptypeptr(origptype);

         if (pointer > -1)
         {
            original_decal = partarray[pointer]->originaldecalname[alt];
            data = Graph_Block_Reference(partarray[pointer]->decalname[alt], name , file->getFileNumber(), x, y,
                  DegToRad(rot), mir , 1.0, -1, TRUE);
            if (is_geom(partarray[pointer]->decalname[alt], FID_GEOM) != 0)
               data->getInsert()->setInsertType(insertTypeFiducial);
			else if (is_geom(partarray[pointer]->decalname[alt], TOOL_GEOM) != 0)
               data->getInsert()->setInsertType(insertTypeDrillTool);
			else
               data->getInsert()->setInsertType(insertTypePcbComponent);
         }
         else
         {
            fprintf(ferr, "Component Decal [%s] not found at %ld\n", origptype, getInputFileLineNumber());
            display_error++;
         }
      }  // if ptye is type@shape

      if (pointer == -1)
         continue;

      update_comppin_ignore(name, original_decal);

      if (partarray[pointer]->fiducial)
         data->getInsert()->setInsertType(insertTypeFiducial);

      if (partarray[pointer]->smdflag)
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, attributeUpdateOverwrite, NULL);

      if (partarray[pointer]->compheight > 0)
      {
         double compHeight = partarray[pointer]->compheight;
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &compHeight, attributeUpdateOverwrite, NULL);
      }

      if (file_version < 30)
      {
         int laynr = Graph_Level("DECAL_1", "", 0);
         doc->SetVisAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, name.GetBuffer(0), nmx ,nmy, 
               DegToRad(nmrot), refnamesize.height, refnamesize.height * .8, 1, 0, TRUE, attributeUpdateOverwrite, 0L, laynr, 0, 0, 0);
      }

      if (strlen(origptype))
      {
         set_parttype_altused(origptype, alt);
         if (alt == 0) // is primary, was: partarray[pointer]->decalcnt == 1)
         {
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TYPELISTLINK, TRUE), VT_STRING,
                  origptype.GetBuffer(0),attributeUpdateOverwrite, NULL);
         }
         else
         {
            CString typname;
            typname.Format("%s_%s", origptype, partarray[pointer]->decalname[alt]);

            if (FindType(file, typname) == NULL) // a type may not exist if it was modified in the component list
            {
               TypeStruct *newtype = AddType(file, typname);
               doc->SetUnknownAttrib(&newtype->getAttributesRef(), ATT_DERIVED, origptype, attributeUpdateOverwrite, NULL);  
               
               TypeStruct *origtypeptr = FindType(file, origptype);
               if (origtypeptr != NULL)
                  doc->CopyAttribs(&newtype->getAttributesRef(), origtypeptr->getAttributesRef());

               BlockStruct *block = Graph_Block_Exists(doc, original_decal, -1);
               if (block)
                  newtype->setBlockNumber( block->getBlockNumber());

            }
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TYPELISTLINK, TRUE),
                  VT_STRING, typname.GetBuffer(0),attributeUpdateOverwrite, NULL);
         }
      }

      if (strlen(value))
      {
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_VALUE,0), VT_STRING,   value.GetBuffer(0),attributeUpdateOverwrite, NULL);
      }
      if (strlen(tolerance))
      {
         double ptol;
         double mtol;
         double tol;
         CString  t;

         check_tolerance(tolerance, &mtol, &ptol, &tol);

         if (tol > -1)
         {
            double tt = tol;
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, TRUE), VT_DOUBLE, &tt, attributeUpdateOverwrite, NULL);
         }
         if (mtol > -1)
         {
            double tt = -mtol;
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE), VT_DOUBLE, &tt, attributeUpdateOverwrite, NULL);
         }           
         if (ptol > -1)
         {
            double tt = ptol;
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE), VT_DOUBLE,&tt,attributeUpdateOverwrite, NULL);
         }
      }

      // Case 2033 - Always set LOADED=TRUE on incoming pcb components
      if (data->getInsert()->getInsertType() == insertTypePcbComponent)
      {
         doc->SetAttrib(&data->getDefinedAttributes(), doc->IsKeyWord(ATT_LOADED, TRUE), VT_STRING,
                  "TRUE",attributeUpdateOverwrite, NULL);
      }

      for (int i=0; i<labelcnt; i++)
         get_label(&data->getAttributesRef(), 0, 0, name);  // no name needed

   }

   return 1;
}

/******************************************************************************
* get_gates
*/
static int get_gates()
{
   char  string[MAX_LINE];
   char  *lp;
   get_nextline(string, MAX_LINE, ifp);

   if ((lp = strtok(string, " \t\n")) == NULL)  // must be G or S
   {
      fprintf(ferr, "Syntax Error at %ld\n", getInputFileLineNumber());
      display_error++;
      return 0;
   }

   lp = strtok(NULL, " \t\n");
   lp = strtok(NULL, " \t\n");
   int gatecnt = lp != NULL ? atoi(lp) : 0;

   lp = strtok(NULL, "\n");   // just to make sure that this is the end of the line

   for (int i=0; i<gatecnt; i++)
   {
      if ((lp = strtok(NULL, " \t\n")) == NULL)
      {
         get_nextline(string, MAX_LINE, ifp);
         lp = strtok(string, " \t\n"); 
      }
   }

   return 1;
}

/******************************************************************************
* update_parttypepin
*/
static int update_parttypepin(const char *parttypename, int pinnr, const char *pinname)
{
   int pointer = get_ptypeptr(parttypename);
   if (pointer < 0)
      return 0;

   int found = 0;
   for (int i=0; i<MAX_ALTDECAL; i++)
   {
      if (strlen(partarray[pointer]->decalname[i]) == 0)
         continue;
      found = 0;

      CString newdecal = partarray[pointer]->decalname[i];
      BlockStruct *block = Graph_Block_Exists(doc, newdecal, -1);
      if (block)
         update_alphapin(block, pinnr, pinname);
   }

   return 1;
}

/******************************************************************************
* get_alpha
*  here look for pinnr and update pinname
* DR 608691 This code used to do a bunch of trimleft and trimright on buffers, such
* would seem to make it tolerant of variable number of space delimiters.
* But we got some data where there were only the space delimiters, and no data
* between the delimiters. Because this parser is keeping sync with the file on
* field by field basis, this caused an out of sync situation. These blank lines
* were supposed to be counted as data, but they were thrown out.
* SINCE this happened only with a V5.0 file, it is being changed only for
* the V5.0 file. It is not clear if all should get treated this way or not.
* But since there are no reports of this sort of thing happening with other
* data samples, we'll be conservative and fix specifically what is broke. For now.
*
* IF it turns out we need to undo the "5.0" fix here for some reason, it is not a great loss.
* We asked the experts on PADS team if this is valid PADS data, and they say it is not.
* So we really aren't obligated to be able to read it.
*   From: Ershov, Vasily 
*   Sent: Tuesday, December 13, 2011 5:58 AM
*   To: Tomala, Radoslaw
*   Cc: Faltersack, Rick
*   Subject: RE: Is this correct PADS data ?
*
*   Radek, Rick, 
* 
*   No, this is not a valid PADS data.  Pin mapping table must have  all 38 pin numbers listed (separated by spaces).
*   Pin number can't be empty.  
* 
*   Thanks, Vasily
*/

static int get_alpha(int cnt, const char *parttypename, int partptr)
{
   if (file_version != 50) // All except V5.0
   {
      // This is the original parsing, before DR 608691.

      if (cnt == 0)
         return 0;

      char string[MAX_LINE];
      CString tmp;
      char *lp;

      do 
      {
         incrementInputFileLineNumber();    // here get prosa
         fgets(string, MAX_LINE, ifp);

         // Case 1668, removed font detection code here, spec says it does not belong

         tmp = string;
         tmp.TrimLeft();
         tmp.TrimRight();
         strcpy(string, tmp);
      } while (strlen(tmp) == 0);   // empty line

      lp = strtok(string, " \t\n"); 
      update_parttypepin(parttypename, 1, lp);
      partarray[partptr]->pinname = lp;

      for (int i=1; i<cnt; i++)
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
         {
            do
            {
               incrementInputFileLineNumber();    // here get prosa
               fgets(string, MAX_LINE, ifp);

               // Case 1668, removed font detection code here, spec says it does not belong

               tmp = string;
               tmp.TrimLeft();
               tmp.TrimRight();
               strcpy(string, tmp);
            } while (strlen(tmp) == 0);   // empty line

            lp = strtok(string, " \t\n"); 
         }
         update_parttypepin(parttypename, i+1, lp);
         partarray[partptr]->pinname += ",";
         partarray[partptr]->pinname += lp;
      }
   }
   else
   {
      // For V5.0, only, so far.

      if (cnt == 0)
         return 0;

      char string[MAX_LINE];

      int pinnr = 0;
      int found = 0;
      while (found != cnt)
      {
         incrementInputFileLineNumber();    // here get prosa
         fgets(string, MAX_LINE, ifp);

         // Replace space delimiters with vertical bar. But count them.
         CString str(string);
         int delcnt = 0;
         int indx = 0;
         while ((indx = str.Find(" ")) >= 0)
         {
            delcnt++;
            str.SetAt(indx, '|');
         }

         found += delcnt; // There is a trailing delim so don't need +1

         CSupString supstr(str);
         CStringArray fields;
         supstr.ParseQuote(fields, "|");

         for (int i = 0; i < fields.GetCount(); i++)
         {
            CString field( fields.GetAt(i) );
            field.Trim();

            if (!field.IsEmpty())
            {
               pinnr++;
               update_parttypepin(parttypename, pinnr, field);
               if (i > 0)
                  partarray[partptr]->pinname += ",";
               partarray[partptr]->pinname += field;
            }
            // else ignore. Trailing delimiter gives one false last field from CSupString.ParseQuote.
         }
      }


   }

   return 1;
}

/******************************************************************************
* get_sig
*/
static int get_sig()
{
   char  string[MAX_LINE];

   incrementInputFileLineNumber();    
   fgets(string, MAX_LINE, ifp);
   return 1;
}

/******************************************************************************
* pads_type
*
*  This is allowed in PADS. Therefore, we need to keep the value with the type
*
*  PCAPALT,22UF_50V PCAPALT I UND  0   0   0   2     0
*  A B 
*
*  PCAPALT,47UF_16V PCAPALT I UND  0   0   0   2     0
*  A B 
*
*  PCAPALT,4.7UF_50V PCAPALT I UND  0   0   0   2     0
*  A B 
*/
static int pads_type()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      int altdecalcnt = 0;
      int gatecnt = 0;
      int sigcnt = 0;
      int attcnt = 0;
      int alphacnt = 0;
      int flg = 0;

      char tmp[MAX_LINE];
      char *lp;
      strcpy(tmp, ifp_line);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;

      int count;
      int attfound = FALSE;
      if (file_version < 30)
      {
         // if the line has more than 8 entries, there is an attribute Flag
         count = 1;  // already got one
         attfound = FALSE;
         while ((lp = strtok(NULL, " \t\n")) != NULL)
            count++;
         if (count > 8)
            attfound = TRUE;
      }

      CString value = "";
      CString tolerance = "";

      if ((lp = strtok(tmp, " \t\n")) == NULL)
         continue;
      CString nname = lp;
      CString origname = lp;

      PADSPart *part = new PADSPart;
      partarray.SetAtGrow(partcnt++, part);  
      part->altused = 0;
      part->ptypename = origname;
      part->pinname = "";
      part->smdflag = 0;
      part->fiducial = 0;
      part->alphapins = 0;
      part->compheight = 0;

      // here seperate value from name
      int found = nname.Find(",");
      if (found > -1)
      {
         CString nnameLeft = nname.Left(found);
         value = nname.Right(strlen(nname) - (found + 1));

         int found1 = value.Find(",");
         if (found1 > -1)
         {
            CString valueLeft = value.Left(found1);
            tolerance = value.Right(strlen(value) - (found1 + 1));
            value = valueLeft;
         }
         nname = nnameLeft;
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString decalname = lp;

      if ((lp = strtok(NULL, "\n")) == NULL)
         continue;
      strcpy(tmp, lp);

      for (int decalCnt=0; decalCnt<MAX_ALTDECAL; decalCnt++)
         part->decalname[decalCnt] = "";

      TypeStruct *type = AddType(file, origname);
      if ((lp = strtok(decalname.GetBuffer(0), ":")))
      {
         part->decalname[0] = lp;
         part->originaldecalname[0] = lp;
			part->primaryDecalname = lp;
         altdecalcnt++;
      }

      while ((lp = strtok(NULL, ":")))
      {
         if (altdecalcnt < MAX_ALTDECAL)
         {
            part->decalname[altdecalcnt] = lp;
            part->originaldecalname[altdecalcnt] = lp;
            altdecalcnt++;
         }
         else
         {
            CString tmp;
            tmp.Format("Type [%s]\n", origname);
            ErrorMessage("Too many Alternate Decal", tmp);
         }
      }
      part->decalcnt = altdecalcnt;

      if (file_version > 2 && file_version < 20070)
      {
         // The units field has been removed from Version 2007.0

         if ((lp = strtok(tmp, " \t\n")) == NULL)
            continue; // unit
         lp = strtok(NULL, " \t\n"); // type
      }
      else
      {
         lp = strtok(tmp, " \t\n"); // type
      }

      if (lp) // type
      {
         part->typ = lp;

         attcnt = 0;
         if (attfound)
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            attcnt = atoi(lp);
         }

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         gatecnt = atoi(lp);
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         sigcnt = atoi(lp);
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         alphacnt = atoi(lp);
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         flg = atoi(lp);

         if (strcmp(part->typ, "UND")) // undefined - ignored
         {        
            if (is_attrib(part->typ, SMD_ATT))  
            {
               part->smdflag = TRUE;
            }
            else
            {
               DeviceTypeTag deviceType = get_mappeddeviceindex(part->typ);
               if (deviceType == deviceTypeUnknown || deviceType == deviceTypeUndefined)
               {
                  doc->SetAttrib(&type->getAttributesRef(), doc->RegisterKeyWord("PADS TYPE", 0, VT_STRING),
                     VT_STRING, part->typ.GetBuffer(0), attributeUpdateOverwrite, NULL);
               }
               else
               {
                  doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_DEVICETYPE, 0),
                     VT_STRING, deviceTypeTagToValueString(deviceType).GetBuffer(0), attributeUpdateOverwrite, NULL);
               }
            }
         }
      }

      if (strlen(value))
         doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 0), VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);

      if (strlen(tolerance))
      {
         double tol = atof(tolerance);
         doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, 0), VT_DOUBLE, &tol, attributeUpdateOverwrite, NULL);
      }

      if (alphacnt)
      {
         part->alphapins = TRUE;
         for (int k=0; k<altdecalcnt; k++)
         {
            BlockStruct *block = Graph_Block_Exists(doc, part->decalname[k], -1);
            if (block == NULL)
            {
               fprintf(ferr, "Syntax Error in PADS file at %ld\n", getInputFileLineNumber());
               display_error++;
            }
            else
            {
               CString newdecal;    
               newdecal.Format("%s_%s", origname, part->decalname[k]);
               BlockStruct *block1 = Graph_Block_On(GBO_APPEND, newdecal, file->getFileNumber(),0);
               if (is_geom(block1->getName(), FID_GEOM) != 0)
                  block1->setBlockType(blockTypeFiducial);
               else if (is_geom(block1->getName(), TOOL_GEOM) != 0)
                  block1->setBlockType(blockTypeTooling);
               else
                  block1->setBlockType(blockTypePcbComponent);
               Graph_Block_Copy(block, 0, 0, 0, 0, 1, -1, TRUE);
               Graph_Block_Off();
               part->decalname[k] = newdecal;
            }
         } // loop through all decals.
      }
      
		int i=0;
      for (i=0; i<attcnt; i++)
      {
         get_nextline(ifp_line, MAX_LINE, ifp);
         if ((lp = strtok(ifp_line,":")))
         {
            CString keyword = lp;
            keyword.TrimLeft();
            keyword.TrimRight();
            keyword.MakeUpper();

            if (strlen(keyword))
            {
               lp = strtok(NULL, "\n");
               // here check how an attribute is used.
               // SMD needs to get to the component
               // FIDUCIAL needs to get to the component.
               if (is_attrib(keyword, SMD_ATT))
               {
                  if (lp && lp[0] == '1') // SMD:1 is the correct SMD flag
                     part->smdflag = TRUE;
               }
               else if (is_attrib(keyword, FID_ATT))
               {
                  part->fiducial = TRUE;
                  doc->SetUnknownAttrib(&type->getAttributesRef(), keyword, lp, SA_APPEND, NULL);
               }
               else if (is_attrib(keyword, HEIGHT_ATT))
               {
                  char save_unit = unit_flag;
                  unit_flag = heightunit_flag;
                  if (lp && strlen(lp))
                  {
                     double compHeight = cnv_unit(atof(lp));
                     doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &compHeight, attributeUpdateOverwrite, NULL);
                     part->compheight = compHeight;
                  }
                  unit_flag = save_unit;
               }
               else
               {
                  doc->SetUnknownAttrib(&type->getAttributesRef(), get_attribmap(keyword), lp, SA_APPEND, NULL);
               }
            }
         } // lp == NULL
      }

      for (i=0;i <gatecnt; i++)
         get_gates();

      for (i=0; i<sigcnt; i++)
         get_sig();

      get_alpha(alphacnt, origname, partcnt-1);
   
		// Make primary without decal name appendage
      if (altdecalcnt > 0)
      {
         BlockStruct *block = Graph_Block_Exists(doc, part->decalname[0], -1);
         if (block)
            type->setBlockNumber( block->getBlockNumber());     
      }

      // Add part types with alternate decal references
		if (altdecalcnt > 1)
      {
         // make this type not used, not make an alternate entry.
         // make type_decal for each and copy the contens on the end of the read.
#ifdef _DEBUG
         fprintf(ferr, "DEBUG: PARTTYPE [%s] has %d DECALS - this would not make a unique CAMCAD %s\n", origname, altdecalcnt, ATT_TYPELISTLINK); 
         display_error++;
#endif

         TypeStruct *origtype = AddType(file, origname);
			// Skip i=0, we don't want an instance of type with primary decal name appended, we only want the alternates
         for (i=1;i <altdecalcnt; i++)
         {
            CString typname;
            typname.Format("%s_%s", origname, part->decalname[i]);
            TypeStruct *newtype = AddType(file,typname);
            doc->SetUnknownAttrib(&newtype->getAttributesRef(), ATT_DERIVED, origname, SA_APPEND, NULL); 

            BlockStruct *block = Graph_Block_Exists(doc, part->decalname[i], -1);
            if (block)
               newtype->setBlockNumber( block->getBlockNumber()); // b->num; pads can have multiple Decals assigned to a type.
            doc->CopyAttribs(&newtype->getAttributesRef(), origtype->getAttributesRef());

         }

         for (i=0; i<altdecalcnt; i++)
            doc->SetUnknownAttrib(&origtype->getAttributesRef(), ATT_ALTERNATESHAPE, part->decalname[i], SA_APPEND, NULL);
      }
   }
   return 1;
}

/******************************************************************************
* pads_route
*/
static int pads_route()
{
   pads_skip();   
   return 1;
}

/******************************************************************************
* check_ignored_pin
*/
static int check_ignored_pin(const char *comp, const char *pinname)
{
   for (int i=0; i<comppinignoredcnt; i++)
   {
      PADSCompPinIgnored *compPinIgnored = comppinignoredarray[i];
      if (!compPinIgnored->compname.Compare(comp) && !compPinIgnored->pinname.Compare(pinname))
      {
         if (!compPinIgnored->already_reported)
         {
            fprintf(ferr, "Component [%s] Pin [%s] in Netlist, but ignored due to settings in pads.in\n", comp, pinname);
            compPinIgnored->already_reported = TRUE;
            display_error++;
         }
         return 1;
      }
   }
   return 0;
}

/******************************************************************************
* already_comppin
*/
static int already_comppin(FileStruct *filePtr, const char *net_name, const char *comp, const char *pin)
{
   POSITION netPos = filePtr->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = filePtr->getNetList().GetNext(netPos);
      if (net->getNetName().Compare(net_name))
         continue;
   
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         if (compPin->getRefDes().Compare(comp) == 0 && compPin->getPinName().Compare(pin) == 0)
             return 1;
      }
   }

   // here need to check if pin is ignored
   if (check_ignored_pin(comp, pin))
      return 1;

   return 0;
}

/******************************************************************************
* pads_signal
*  No vianames T1 is layer T2 is change
*/
static int pads_signal()
{
   char *lp;   
   long sigflg = 0;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString name = lp;

   if ((lp = strtok(NULL, " \t\n")))
   {
      if ((lp = strtok(NULL, " \t\n")))   
         sigflg = atol(lp);
   }

   NetStruct *net = add_net(file, name);
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      if ((lp = strtok(ifp_line, " .\t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString refdes = lp;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString pinname = lp;

      if (!already_comppin(file, name, refdes, pinname))
         add_comppin(file, net, refdes, pinname);

      if ((lp = strtok(NULL, " .\t\n")) == NULL)
         continue;      
      refdes = lp;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      pinname = lp;
   
      if (!already_comppin(file,name, refdes, pinname))
         add_comppin(file, net, refdes, pinname);
      
      int layer = -1;
      int lastlayer;
      int first = TRUE;
      while (layer != 31)
      {
         get_nextline(ifp_line, MAX_LINE, ifp);
         if ((lp = strtok(ifp_line, " \t\n")) == NULL)
            continue;
         double x = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double y = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         char viachar = lp[0];

         ++lp;
         layer = atoi(lp);
         int layernr;
         if (layer == 0)
         {
            layernr = Graph_Level("UNROUT", "", 0);
            unroutelayer = TRUE;
         }
         else if (layer < 31) // last layer in routes - not a real layer
         {
            layernr = Graph_Level(lp, "", 0);
         }

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double width = cnv_unit(atof(lp));

         int widthindex = 0;
//       if (width != 0)
//       {
            int err;
            widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
//       }

         if (first)
         {
            DataStruct *data = Graph_PolyStruct(layernr, 0, 0);
            if (layer > 0) // UNROUT is not an etch
               data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING,name.GetBuffer(0),SA_APPEND, NULL);
            Graph_Poly(NULL, widthindex, 0, 0, 0);
            lastlayer = layer;
         }
         first = FALSE;
         
         double bulge = 0;
         Graph_Vertex(x, y, bulge); 

         if (layer != lastlayer && layer != 31)
         {
            char *vianame = "STANDARDVIA";
         
            DataStruct *data = Graph_Block_Reference(vianame, NULL, file->getFileNumber(), x, y, DegToRad(0), 0 , 1.0,Graph_Level("0", "", 1), TRUE);
            data->getInsert()->setInsertType(insertTypeVia);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, name.GetBuffer(0), SA_APPEND, NULL);
            data = Graph_PolyStruct(layernr, 0, 0);

            if (layer > 0) // unrout is not an etch.
               data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING, name.GetBuffer(0), SA_APPEND, NULL);
            Graph_Poly(NULL,widthindex, 0, 0, 0);
            Graph_Vertex(x, y, bulge);
         }
         lastlayer = layer;
      }
   }
   return 1;
}

/******************************************************************************
* int is_unnamednet
*/
static int  is_unnamednet(const char *nn)
{
   for (int i=0; i<unnamednetcnt; i++)
   {
      if (!STRNICMP(nn, unnamednet[i],strlen(unnamednet[i])))
         return 1;
   }
   return 0;
}

/******************************************************************************
* is_viadef
*/
static int is_viadef(char *v)
{
   for (int i=0;i<viadefcnt;i++)
   {  
      if (viadefarray[i]->name.Compare(v) == 0)
         return 1;
   }
   return FALSE;
}

/******************************************************************************
* pad6_signal
*/
static int pad6_signal()
{
   char *lp; 

   // Related to (but not the cause of) case dts0100422405
   // PADS 2007 format case data has -2 in this data position, unsigned no longer works
   //unsigned long sigflg = 0;
   long sigflg = 0;

   int net_register = doc->IsKeyWord(ATT_NETNAME, 1);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString name = lp;

   if ((lp = strtok(NULL, " \t\n")))
   {
      if ((lp = strtok(NULL," \t\n"))) 
         sigflg = atol(lp);
   }

   NetStruct *net = add_net(file,name);
   if (is_unnamednet(name))
      net->setFlagBits(NETFLAG_UNNAMED);

   // only bit 1, 9, 18 are used in version 3
   if (file_version < 30)  // sigflag in Version 3 have changed and do not means power/gnd anymore.
   {
      int value  = ((sigflg >> 26) & 0xff) ;
      if (value)
      {
         int keyword = doc->IsKeyWord(ATT_POWERNET, TRUE);
         doc->SetAttrib(&net->getAttributesRef(), keyword, VT_NONE,  NULL, attributeUpdateOverwrite, NULL); 
         keyword = doc->IsKeyWord(NETATT_SIGNALSTACK, TRUE);
         doc->SetAttrib(&net->getAttributesRef(), keyword, VT_INTEGER, &value, attributeUpdateOverwrite, NULL);
      }
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      int layernr;
      CPnt *lastvertex;

      if ((lp = strtok(ifp_line, " .\t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString refdes = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString pinname = lp;

      if (!already_comppin(file,name,refdes,pinname))
      {
         add_comppin(file, net, refdes, pinname);
      }
      if ((lp = strtok(NULL," .\t\n")) == NULL) continue;      
      refdes = lp;
      if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
      pinname = lp;
   
      if (!already_comppin(file, name, refdes, pinname))
      {
         add_comppin(file, net, refdes, pinname);
      }

      // here I can get more info like linewidth etc...
      
      double lastx;
      double lasty;
      int layer = -1;
      int lastlayer;
      int first = TRUE;
      int lastwidthindex = -1;
      while (layer <= maxlayer)
      {
         get_nextline(ifp_line, MAX_LINE, ifp);

         if ((lp = strtok(ifp_line, " \t\n")) == NULL)
            continue;

         if (nextsect(lp))
            return 1; // this happens if not routes follow a COMP.PIN line.
         double x = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double y = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         layer   = atoi(lp);

         if (layer == 0)
         {
            unroutelayer = TRUE;
         }
         else if (layer <= maxlayer)
         {
            layernr = Graph_Level(lp, "", 0);
         }

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double width = cnv_unit(atof(lp));

         int widthindex = 0;
         if (width < 0)
         {
            // lost here...
            fprintf(ferr, "Syntax error in PADS ASCII file at line %ld\n", getInputFileLineNumber());
            display_error++;
            continue;
         }
         else if (width >= 0)
         {
            int err;
            widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         }

         if (first && layer > 0 && layer <= maxlayer)
         {
            DataStruct *data = Graph_PolyStruct(layernr, 0, 0);
            if (layer > 0) // UNROUT is not an etch
               data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(), net_register, VT_STRING,name.GetBuffer(0),SA_APPEND, NULL); 
            Graph_Poly(NULL, widthindex, 0, 0, 0);
            lastlayer = layer;
            lastwidthindex = widthindex;
            first = FALSE;
         }
         int via = FALSE;

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         int flg = atoi(lp);

         int arcdir = 0;
         int arcfound = FALSE;
         CString vianame;
         while ((lp = strtok(NULL, " \t\n")))   
         {
            if (!STRCMPI(lp, "CW"))
            {
               // here recalc bulge
               arcdir = 1;
               lastx = x;
               lasty = y;
               arcfound = TRUE;
               continue;
            }
            else if (!STRCMPI(lp, "CCW"))
            {
               // here recalc bulge
               arcdir = 2;
               lastx = x;
               lasty = y;
               arcfound = TRUE;
               continue;
            }
            else if (!STRCMPI(lp, "TEARDROP"))  
            {
               // need to deal with this later
               if ((lp = strtok(NULL, " \t\n")) == NULL)
                  continue;
            }
            else if (!STRCMPI(lp, "THERMAL"))   
            {
               // need to deal with this later
            }
            else
            {
               if (is_viadef(lp))
               {
                  vianame = lp;
                  via = TRUE;
               }
            }
         }

         if (arcfound)
            continue;

         if (!first)
         {
            if (arcdir == 1)  // cw
            {
               double rad, sa, da;
               ArcCenter2((double)lastvertex->x, (double)lastvertex->y, x, y, lastx, lasty, &rad, &sa, &da, TRUE);
               lastvertex->bulge = (DbUnit)tan(da / 4);
            }
            else if (arcdir == 2)   // ccw
            {
               double rad, sa, da;
               ArcCenter2((double)lastvertex->x, (double)lastvertex->y, x, y, lastx, lasty, &rad, &sa, &da, FALSE);
               lastvertex->bulge = (DbUnit)tan(da / 4);
            }
            lastvertex = Graph_Vertex(x, y, 0.0); // no via, no arc
         }

         if (via)
         {
            DataStruct *data = Graph_Block_Reference(vianame, NULL, file->getFileNumber(), x, y,
                  DegToRad(0), 0 , 1.0, Graph_Level("0", "", 1), TRUE);
            data->getInsert()->setInsertType(insertTypeVia);
            doc->SetAttrib(&data->getAttributesRef(), net_register, VT_STRING, name.GetBuffer(0), SA_APPEND, NULL);

            if (layer > 0 && layer <= maxlayer)
            {
               DataStruct *data = Graph_PolyStruct(layernr, 0, 0);
               data->setGraphicClass(GR_CLASS_ETCH);
               doc->SetAttrib(&data->getAttributesRef(), net_register, VT_STRING, name.GetBuffer(0), SA_APPEND, NULL); 
               Graph_Poly(NULL, widthindex, 0, 0, 0); 
               lastvertex = Graph_Vertex(x, y, 0.0); 
               lastlayer = layer;
               lastwidthindex = widthindex;
               first = FALSE;
            }
         }
         else if (widthindex != lastwidthindex) // if the width changes without a via.
         {
            if (!first)
            {
               Graph_Poly(NULL,widthindex, 0, 0, 0);  // fillflag , negative, closed
               lastvertex = Graph_Vertex(x, y, 0.0); // no via, no arc
            }
            lastlayer = layer;
            lastwidthindex = widthindex;
            first = FALSE;
         }

         if (layer == 0)
            first = TRUE;
         if (widthindex != lastwidthindex)
            first = TRUE;
         if (layer != lastlayer)
            first = TRUE;

         via = FALSE;
         lastlayer = layer;
         lastwidthindex = widthindex;
         lastx = x;
         lasty = y;
         arcdir = 0;
      }
   }
   return 1;
}

/******************************************************************************
* pads_connection
*/
static int pads_connection()
{
   pads_skip();   
   return 1;
}

/******************************************************************************
* do_polypieces
*/
static int do_polypieces(const char *pourtype, double xloc, double yloc, const char *name, const char *ownername, const char *signame)
{
   char     *lp;

   get_nextline(ifp_line, MAX_LINE, ifp);

   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   if (!STRCMPI(lp, ".REUSE."))  // skip the reuse line and 
   {
      get_nextline(ifp_line, MAX_LINE, ifp);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         return 0;
   }
   CString ptype = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int corners = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int arcs = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   // Spec in Pour section specified width is in Mils
   double width = 0;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int level = Graph_Level(lp,"",0);

   int widthindex = 0;
   int err;
// if (width != 0)
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *data = NULL;

   // here find an existing pourout pointer
   for (int i=0; i<pourcnt && data == NULL; i++)
   {
      if (!strcmp(pourarray[i]->name, ownername))
         data = pourarray[i]->data_adress;
   }

   if (data == NULL && strcmp(ownername, name)/*not same names*/)
   {
      fprintf(ferr, "POUR ownername [%s] not found\n", ownername);
      display_error++;
   }

   if (data == NULL)
   {
      data = Graph_PolyStruct(level, 0 ,0);
      data->setGraphicClass(GR_CLASS_ETCH);
      if (signame && strlen(signame))
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING, (void *)signame, SA_APPEND, NULL);
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NAME, 1), VT_STRING, (void *)name, SA_APPEND, NULL); 
   }

   PADSPour *pour = new PADSPour;
   pourarray.SetAtGrow(pourcnt++, pour);  
   pour->name = name;
   pour->data_adress = data;

   int fill = 0;
   int voidfill = 0;
   int close = 0;
   int seg = FALSE;
   int boundary = FALSE;
   if (!strcmp(pourtype, "POUROUT"))
   {
      close = TRUE;
      boundary = TRUE;
   }
   else if (!strcmp(pourtype, "HATOUT"))
   {
      fill = close = TRUE;
   }
   else if (!strcmp(pourtype, "VOIDOUT"))
   {
      fill = TRUE;
      close = TRUE;
      voidfill = TRUE;
   }
   else if (!strcmp(pourtype, "PADTHERM"))
   {
      fill = FALSE;
      close = FALSE;
      voidfill = FALSE;
      seg = TRUE;
   }
   else if (!strcmp(pourtype, "VIATHERM"))
   {
      fill = FALSE;
      close = FALSE;
      voidfill = FALSE;
      seg = TRUE;
   }
   else
   {
      fprintf(ferr, "Poly type [%s] unknown at %ld\n", pourtype, getInputFileLineNumber());
      display_error++;
   }

   CPoly *poly = Graph_Poly(data, widthindex, fill, voidfill, close);
   poly->setThermalLine(seg);
   poly->setFloodBoundary(boundary);

   if (!STRCMPI(ptype, "CIRCLE"))
   {
      if (corners + arcs == 2)
      {
         double   x1,y1,x2,y2;

         get_nextline(ifp_line, MAX_LINE, ifp);
         // just in case there is a error
         if ((lp = strtok(ifp_line, " \t\n")))
            x1 = cnv_unit(atof(lp));
         if ((lp = strtok(NULL, " \t\n")))      
            y1 = cnv_unit(atof(lp));

         get_nextline(ifp_line, MAX_LINE, ifp);
         // just in case there is a error
         if ((lp = strtok(ifp_line, " \t\n")))
            x2 = cnv_unit(atof(lp));
         if ((lp = strtok(NULL, " \t\n")))      
            y2 = cnv_unit(atof(lp));

         Graph_Vertex(x1 + xloc, y1 + yloc,1);
         Graph_Vertex(x2 + xloc, y2 + yloc,1);
         Graph_Vertex(x1 + xloc, y1 + yloc,0);
      }
      else
      {
         fprintf(ferr, "Circle with more than 2 corners at %ld\n", getInputFileLineNumber());
         display_error++;
         for (int i=0; i<corners+arcs; i++)
            get_nextline(ifp_line, MAX_LINE, ifp);
      }
   }
   else
   {
      CPnt *last_vertex = NULL;
      for (int i=0; i<corners+arcs; i++)
      {
         get_nextline(ifp_line, MAX_LINE, ifp);

         // just in case there is a error
         if ((lp = strtok(ifp_line ," \t\n")) == NULL)
            continue;
         double x = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double y = cnv_unit(atof(lp));

         double bulge = 0;
         if ((lp = strtok(NULL, " \t\n")))
         {
            // if arc, x,y is the center of an arc
            double a = atof(lp) / 10;
            double sa = DegToRad(a);
            lp = strtok(NULL, " \t\n");
            a = atof(lp) / 10;

            double da = DegToRad(a);
            if (fabs(a) >= 360)
            {
               fprintf(ferr, "PADS Syntax error at %ld\n", getInputFileLineNumber());
               da = 0;
            }
            bulge = tan(da / 4);                                   

            if (last_vertex)
            {
               last_vertex->bulge = (DbUnit)bulge;
            }
            else
            {
               fprintf(ferr, "Arc found without last vertex at %ld\n", getInputFileLineNumber());
               display_error++;
            }
         }
         else
         {
            last_vertex = Graph_Vertex(x + xloc, y + yloc, bulge);
         }
      }
   }
   return 1;
}

/******************************************************************************
* pads_pour
*/
static int pads_pour()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString name = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString pourtype = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double xloc = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double yloc = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int pieces = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString ownername = "";
      CString signame   = "";
      if ((lp = strtok(NULL, " \t\n")))   
      {
         ownername = lp;
         if ((lp = strtok(NULL," \t\n"))) 
            signame = lp;
      }

      for (int i=0; i<pieces; i++)
         do_polypieces(pourtype, xloc, yloc, name, ownername, signame);
   }
   return 1;
}

/******************************************************************************
* pads_null
*/
static int pads_null()
{
   pads_skip();   
   return 1;
}

/******************************************************************************
* pad6_via
*/
static int pad6_via()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char     *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (nextsect(lp))
         break;
      CString name = lp;
      
      PADSViadef *viadef = new PADSViadef;
      viadefarray.SetAtGrow(viadefcnt++, viadef);  
      viadef->name = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double drill = cnv_unit(atof(lp));
      
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int stacks = atoi(lp);

      int start_drill = 1;
      int end_drill = maxlayer;
      if ((lp = strtok(NULL, " \t\n")))
      {
         start_drill = atoi(lp);
         if ((lp = strtok(NULL, " \t\n")))   
            end_drill = atoi(lp);
      }

      get_viastack(name, stacks, drill, start_drill, end_drill);
   }
   return 1;
}

/******************************************************************************
* nextsect
*/
static int nextsect(char *n)
{
   if (n[0] != '*')
      return 0;

   for (int i=0; i<SIZ_PCB_LST; i++)
   {
      if (!_strnicmp(pcb_lst[i].token, n, strlen(pcb_lst[i].token)))
      {
         PushTok = TRUE;
         return(1);
      }
   }
   return 0;
}

/******************************************************************************
* cnv_units
*   Converts from PADS any units to INCH units.
*/
static double cnv_unit(double x, char unitflag)
{
   double factor;

   switch (unitflag)
   {
   case 'I': /* inch */
      factor = Units_Factor(UNIT_INCHES, PageUnits);
      break;
   case 'M': /* metric */
      factor = Units_Factor(UNIT_MM, PageUnits);
      break;
   case 'P': /* pads */

   case 'A': /* mil */
      factor = Units_Factor(UNIT_MILS, PageUnits);
      break;
   case 'B': /* basic */
      factor = Units_Factor(UNIT_INCHES, PageUnits);
      factor = factor / 38100000.0;
      break;
   }

   return x * factor;
}

/******************************************************************************
* cnv_unit
*   Converts from PADS any units to PageUnits.
*/
static double cnv_unit(double x)
{
   return cnv_unit(x, unit_flag);
}

//_____________________________________________________________________________
PADS_Paddef* CPaddefArray::getAt(int index)
{
   PADS_Paddef* padDef = NULL;

   if (index < GetCount())
   {
      padDef = GetAt(index);
   }

   if (padDef == NULL)
   {
      padDef = new PADS_Paddef();
      SetAtGrow(index,padDef);
   }

   return padDef;
}

void GetSilkcreenLayers(CString &silkscreenTop, CString &silkscreenBottom)
{
	silkscreenTop.Empty();
	silkscreenBottom.Empty();

	for (int i=0; i<misclayercnt; i++)
	{
		PADSMiscLayer *miscLayer = misclayerarray[i];
		if (miscLayer == NULL)
			continue;

		if ((miscLayer->layer_type.CompareNoCase("SILK_SCREEN") == 0) && (miscLayer->layer_name.Find("Top") > 0))	
			silkscreenTop = miscLayer->layer_name;
		else if ((miscLayer->layer_type.CompareNoCase("SILK_SCREEN") == 0) && (miscLayer->layer_name.Find("Bottom") > 0))
			silkscreenBottom = miscLayer->layer_name;		
	}
}

int do_text_layer()
{
	// If text on decal is on layer with electrical stack number 1 or max
	// then change to layer "Silkscreen Top" or "Silkcreen Bottom" respectively

	// Geometry of type decals are fiducial, tooling or PCB component

	CString silkscreenTop, silkscreenBottom;
	GetSilkcreenLayers(silkscreenTop, silkscreenBottom);

	int silkscreenTopIndex = -1;
	int silkscreenBottomIndex = -1;

	if (!silkscreenTop.IsEmpty())
		if (doc->FindLayer_by_Name(silkscreenTop) != NULL)
			silkscreenTopIndex = doc->FindLayer_by_Name(silkscreenTop)->getLayerIndex();
	if (!silkscreenBottom.IsEmpty())
		if (doc->FindLayer_by_Name(silkscreenBottom) != NULL)
			silkscreenBottomIndex = doc->FindLayer_by_Name(silkscreenBottom)->getLayerIndex();

	for (int i=0; i<doc->getMaxBlockIndex(); i++)
	{
		BlockStruct* block = doc->getBlockAt(i);
		if (block == NULL)
			continue;
			
		if (block->getBlockType() != blockTypeFiducial && 
			 block->getBlockType() != blockTypeTooling &&
			 block->getBlockType() != blockTypePcbComponent)
			continue;

		for (POSITION pos = block->getHeadDataPosition(); pos != NULL;)
		{
			DataStruct* data = block->getNextData(pos);
			if (data == NULL || data->getDataType() != dataTypeText)
				continue;

			LayerStruct* layer = doc->getLayerAt(data->getLayerIndex());
			if (layer->getElectricalStackNumber() == 1 && silkscreenTopIndex != -1)
				data->setLayerIndex(silkscreenTopIndex);
			else if (layer->getElectricalStackNumber() == doc->getMaxElectricalLayerNumber() && silkscreenBottomIndex != -1)
				data->setLayerIndex(silkscreenBottomIndex);			
		}
	}

	return 1;
}

static TypeStruct * FindPadsPartType(FileStruct *pcbfile, CString partname)
{
   // Try to find the part type for "library:name" or "name"
   int i=0;

   TypeStruct *parttype = FindType(pcbfile, partname);

   // If part type not found by whole name then try the "name" from the form "library:name"
   if (parttype == NULL && partname.Find(":") > -1)
   {
	   int start = partname.Find(":") + 1;
      CString subname( partname.Right(partname.GetLength() - start));
      parttype = FindType(pcbfile, subname);
   }
   return parttype;
}


static void processAssemblyOptions(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   // Case 2033
   // Process PADS ASSEMBLY_OPTIONS attribute values to produce CAMCAD build Variants.
   // The attribute value is a list of 0 or more "variances".
   // E.g. ASSEMBLY_OPTIONS Build01,U;Build02,I;Build03,S,Alt01

   if (doc != NULL && pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
      BlockStruct *pcbBlock = pcbFile->getBlock();
      int assemblyOptKw = doc->IsKeyWord("ASSEMBLY_OPTIONS", FALSE);
      int loadedKw = doc->IsKeyWord(ATT_LOADED, FALSE);

      // Add a "default" variant that is just a capture of the pads import "as is".
      // Allows users to "reset" from other variant options.

      CString variantName = "$$ Default $$";
      Attrib* attrib = NULL;
      if (pcbBlock->lookUpAttrib(assemblyOptKw, attrib) && attrib != NULL && attrib->getValueType() == VT_STRING)
      {
#if CamCadMajorMinorVersion > 406  //  > 4.6
         variantName = attrib->getStringValue();
#else
         variantName = doc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif

         int index = variantName.Find(",");
         if (index > -1)
            variantName = variantName.Left(index);
      }

#if CamCadMajorMinorVersion > 406  //  > 4.6
      pcbFile->getVariantList().AddVariant(doc->getCamCadData(), *pcbFile, variantName);
#else
      pcbFile->getVariantList().AddVariant(*doc, *pcbFile, variantName);
#endif

      int partTypeErrCnt = 0;    // a counter for how many part types are not found in this file

      for (CDataListIterator insertIterator(pcbBlock->getDataList(),dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct *data = insertIterator.getNext();

         if (data->getAttributesRef() != NULL)
         {
            Attrib *attrib = NULL;
            if (data->getAttributesRef()->Lookup(assemblyOptKw, attrib) && attrib != NULL)
            {
               if (attrib->getValueType() == VT_STRING)
               {
#if CamCadMajorMinorVersion > 406  //  > 4.6
                  CSupString optionsStr = attrib->getStringValue();
#else
                  CSupString optionsStr = doc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
                  if (!optionsStr.IsEmpty())
                  {
                     // optionsStr stores the option after ASSEMBLY_OPTIONS
                     CStringArray records;
                     optionsStr.ParseQuote(records, ";");

                     for (int i = 0; i < records.GetCount(); i++)
                     {
                        CSupString recordStr = records.GetAt(i);
                        CStringArray fields;
                        recordStr.ParseQuote(fields, ",");

                        // Only interested in I and U records, which should have field 
                        // count = 2. Ignore records of other sizes
                        if (fields.GetCount() == 2)
                        {
                           CString code = fields.GetAt(1);
                           CString refname = data->getInsert()->getRefname();
                           code.MakeUpper();

                           // If is Installed or Uninstalled
                           if (code == "I" || code == "U")
                           {
                              CString variantname = fields.GetAt(0);
                              
                              if (!variantname.IsEmpty() && !refname.IsEmpty())
                              {
                                 // Just add it, will return existing if it was already there
#if CamCadMajorMinorVersion > 406  //  > 4.6
                                 CVariant *v = pcbFile->getVariantList().AddVariant(doc->getCamCadData(), *pcbFile, variantname);
#else
                                 CVariant *v = pcbFile->getVariantList().AddVariant(*doc, *pcbFile, variantname);
#endif

                                 CVariantItem *vitem = v->FindItem(refname);
                                 if (vitem != NULL)
                                 {
                                    CString value("TRUE"); // assume "I"
                                    if (code == "U") 
                                       value = "FALSE";
#if CamCadMajorMinorVersion > 406  //  > 4.6
                                    vitem->setAttrib(doc->getCamCadData(), loadedKw, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
                                    vitem->SetAttrib(doc, loadedKw, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
                                 }
                              }
                           }
                           // OOPS!!! We use "S" but missing 3rd option      
                           else if ( code.CompareNoCase("S") == 0 )
                           {
                              fprintf(ferr, "Missing part type for %s while using S for the option", refname);
                           }
                        }
                        // If we have S records, we will have 3rd field. The 3rd field indicates the part type in file
                        else if (fields.GetCount() == 3)
                        {
                           CString code = fields.GetAt(1);
                           code.MakeUpper();

                           // If is substitution
                           if (code == "S")
                           {
                              // variantname stores the variantname. e.g. BPS3000MV_4.071.064
                              // refname store the part name. e.g. R49
                              CString variantname = fields.GetAt(0);                              
                              CString refname = data->getInsert()->getRefname();

                              if (!variantname.IsEmpty() && !refname.IsEmpty())
                              {

#if CamCadMajorMinorVersion > 406  //  > 4.6
                                 CVariant *v = pcbFile->getVariantList().AddVariant(doc->getCamCadData(), *pcbFile, variantname);
#else
                                 CVariant *v = pcbFile->getVariantList().AddVariant(*doc, *pcbFile, variantname);
#endif
                                 // parttype stores the part type that the refname will use. e.g. 1K or library:1K
                                 CSupString parttype = fields.GetAt(2);                                

                                 TypeStruct *devType = FindPadsPartType(pcbFile, parttype);

                                 // Now we found it 
                                 if ( devType == NULL )
                                 {
                                    fprintf(ferr, "Assembly Option Failed:  Component \"%s\", Variant \"%s\",  References undefined Part Type \"%s\"\n", refname, variantname, parttype);
                                    ++partTypeErrCnt;
                                 }
                                 else
                                 {
                                    CVariantItem *vitem = v->FindItem(refname);
                                    if (vitem != NULL)
                                    {
                                       doc->CopyAttribs(&vitem->getAttributesRef(), devType->getAttributesRef());
                                    }
                                 }
                              }                             
                           }
                        }
                     }
                  }
               }
            }
         }
      }

      if ( partTypeErrCnt > 0)
      {
         CString msg;
         msg.Format("%d Variant Substitution errors were encountered, see log file for details.", partTypeErrCnt);
         ErrorMessage(msg);
      }
   } 
}
