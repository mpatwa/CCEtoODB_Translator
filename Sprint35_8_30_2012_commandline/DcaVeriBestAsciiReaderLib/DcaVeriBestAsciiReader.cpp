// $Header: /CAMCAD/DcaLib/DcaVeriBestAsciiReader.cpp 1     6/18/07 1:23p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.

   PAD is only the outer layer.
   PAD in SMD is PAD_TOP or PAD_BOT
   PAD in thru is PAD_1 and PAD_2


   Library for reading vb99 databases

   if a V14 cor file is found -> take this.

   the following files are expected
   inch     cel  // shape definitions
   inch     clr  // clearance file -> not yet supported
   inch     cor  // correlation file
   inch     des  // design
   inch     nnn  // keyin netlist -> can be changed in TEC with .NETLISTEX...
   inch     pad  // padgeometry definition
   inch     pkg  // package file
   inch     stk  // padstack
   inch     tec  // technology, layerstacking

   Not implemented:
   Default trace width
   Text justifications

   Triangle graphic (padstack) are treated as Rectangle.
   Complex shape
   Ellipse
   Rotated pins   = only 90 degree rotation allowed
   local PART_DEF under PART_INSTANCE in design file.

   .PART_INSTANCE UNUSED will be skipped.

*/           

#include "StdAfx.h"

//#ifdef EnableDcaVeriBestAsciiReader

//#error testing 123
 
#include "DcaVeriBestAsciiReader.h"
#include "DcaFile.h"
#include "DcaPoint2.h"
#include "DcaLib.h"
#include "DcaCamCadData.h"
#include "DcaFileType.h"
#include "DcaLayerType.h"
#include "DcaAttributes.h"
#include "DcaUnits.h"
#include "DcaDataType.h"
#include "DcaInsert.h"
#include "DcaLegacyGraph.h"
#include "DcaTMatrix.h"
#include "DcaDataListIterator.h"
#include "DcaTestAccess.h"
#include "DcaPoly.h"
#include "DcaGeomLib.h"
#include "DcaWriteFormat.h"

//#include "ccdoc.h"
//#include "dbutil.h"
//#include "graph.h"             
//#include "geomlib.h"
//#include "pcbutil.h"
//#include "format_s.h"
//#include <math.h>
//#include <string.h>
//#include "gauge.h"
//#include "attrib.h"
//#include "vb99in.h"
//#include "CCEtoODB.h"

#define LineStatusUpdateInterval 200

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CProgressDlg *progress;
//static CFileReadProgress*  fileReadProgress = NULL;
static CDcaOperationProgress* fileReadProgress = NULL;
//extern LayerTypeInfoStruct layertypes[];
extern char *testaccesslayers[];

/* Static Variables *********************************************************/
//static CCAMCADDoc        *doc;
static FileStruct       *file = NULL;
static FileStruct       *fl = NULL;          // this is the current filepointer, either from file or
                                             // from an already loaded PCB file.
static PageUnitsTag     PageUnits;
static double           unit_faktor;
static FILE             *cur_fp;             // this the current file pointer
//static long              fpcnt;
static FILE             *ferr;               /* scanner error file */
static int              display_error = 0;
static TypeStruct       *curtype;
static BlockStruct      *curblock;
static DataStruct       *lastpininsert;

static char             buf[MAX_LONGLINE];
static char             cur_line[MAX_LONGLINE];    // this is the current line
static int              cur_filenum = 0;
static CString          cur_filename;             // this is the current open file
static CString          cur_version;
static int              cur_ident;
static int              cur_status = FILE_UNKNOWN;
static int              cur_cellinstance = 0;
static int              cur_compcnt = 0;  // needed for unnamed components
static int              cur_fill = FALSE;
static bool             isInGeneratedData;

static int              NumberOfLayers   = 0;
static int              usecorfile = FALSE;
static Point2           last_pinkoo;
static char             last_padstack[80];
static int              last_padrotation;
static int              SMDPADMIRROR = FALSE;
static int              OLDNETLIST = FALSE;
static int              derived;    // used to copy component for pin_name in part_def in design file.
static vb99Via          vb99via;
static int              polycnt =0 ;
static Point2           *poly_l;         // .bulge is the radius
static CDcaVeriBestAsciiReaderGlobal G;

static VB99Attr             *attr;
static int                  attrcnt;

// here is how SMD pin recognition works.
// remember data pointer - if no intpad rename top and bottom to SMD_TOP and SMD_BOT
// and mark as SMD pad.
static CString          topPadName;
static CString          bottomPadName;
static CString          innerPadName;
static CString          thermalPadName;
static CString          clearancePadName;
static CString          topSoldermaskPadName;
static CString          bottomSoldermaskPadName;
static CString          holeName;
static CString          topSolderPastePadName;
static CString          bottomSolderPastePadName;
static CString          viaFromLayerName;
static CString          viaToLayerName;

static CDcaVBGeomMap       VBGeomList;       
static CDcaVBGeom*         curVBGeom;

static CDrillArray      drillarray;
static int              drillcnt;

static VBAdef           layer_attr[MAX_LAYERS];       /* Array of layers from pdif.in   */
static int              layer_attr_cnt;

static Localpad         *localpad;
static int              localpadcnt;

static CString          unnamednet[30];
static int              unnamednetcnt;

static CurPad           *curpad;
static int              curpad_cnt;

static CAttribmapArray  attribmaparray;
static int              attribmapcnt = 0;

static CPinNetArray     pinToUniqueNetZeroArray;
static int              pinToUniqueNetZeroCnt;

// Variable user for loading setting file
static int              testpadtop; // force a testpad in cel file to a top definition.
static CString          creator = "";
static CString          compoutline;
static int              correct_padstackuniterror;
static int              explode_drawing_cell;
static int              use_generated_data;
static CString          netlistsource;
static CString          USE_TECHNOLOGY;
static int              USE_CELL_FANOUT_VIAS;
static BOOL             USE_UNIQUE_NETZERO;
static bool             do_normalize_bottom_build_geometries;
static CString          padshapePrefix;

// Control for reporting unknown keywords.
// This came about with case dts0100386334, users want to know about unknown pad shapes.
static bool reportUnknownCommands = false;
static CString unknownCommandMessage;


/* Function Prototypes *********************************************************/
static void load_vb99settings(const CString fname);
static void do_jobprefs(CString pathName);
static void do_padstack(CString pathName);
static void do_cell(CString pathName);
static void do_layout(CString pathName);
static void do_pdb(CString pathName);
static void do_netlist(CString pathName);
static void do_netprops(CString pathName);
static void do_assign_layers();
static void clean_original_name();
static CString Get_Next_Refname(CAttributes* map);
//static int get_layertype(const char *layerType);
static int start_command(List *lst,int siz_lst,int start_indent);
static int go_command(List *lst,int siz_lst,int start_indent);
static int tok_search(char *lp, List *,int);
static char *strword();
static double get_units(char *lp);
static double cnv_unit(double);
static char *get_keyword(char *buf,int *ident);
static void clean_blank(char *n);
static char *clean_level(char *n);      // eliminate of blanks.
static char *clean_quote(char *n);     // eliminate of blanks and quotes.
static void check_buf(char *b);
static int write_poly();
static void global_init();
static void global_free();
static int read_techfile();
static int read_padfile();
static int read_netfile(FileStruct *file);
static int write_arc();
static int write_circle();
static int write_text(const char *tmp);
static int overwrite_pad();            // I do not think we need this function anymore ?????
static int update_localpad(char *pn);
static BOOL IsCompExist(CString compname);
static void ReassignCompPinNamesIfDuplicate(BlockStruct *compBlock);
static void AssignCompPinToUniqueNetZero(CString compName);
static void AssignLayoutPinNetAssignments(CString compName);
static BlockStruct* getLayerSpecificViaBlock(const CString& fromLayerName,const CString& toLayerName);

static int   keyin_netlist;
static int   netprops_netlist;
static int   layout_netlist;
extern CString GetLogfilePath(CString logFileName);

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

static void setInputFile(FILE* file)
{
   delete fileReadProgress;
   fileReadProgress = new CDcaFileReadProgress(file);

   resetInputFileLineNumber();
}

static CCamCadData* s_camCadData = NULL;

static CCamCadData& getCamCadData()
{
   return *s_camCadData;
}

static void setCamCadData(CCamCadData* camCadData)
{
   s_camCadData = camCadData;
}

static CDcaLegacyGraph* s_legacyGraph = NULL;

static CDcaLegacyGraph& getLegacyGraph()
{
   if (s_legacyGraph == NULL)
   {
      s_legacyGraph = new CDcaLegacyGraph(getCamCadData());
   }

   return *s_legacyGraph;
}

static void releaseLegacyGraph()
{
   delete s_legacyGraph;

   s_legacyGraph = NULL;
}

static int graphLayer(const char* layerName,const char* prefix,BOOL floatingFlag)
{
   CString fullLayerName;

   if (prefix != NULL)
   {
      fullLayerName = prefix;
   }

   fullLayerName += layerName;

   int layerIndex = getCamCadData().getDefinedLayerIndex(fullLayerName,floatingFlag != 0,layerTypeUnknown);

   return layerIndex;   
}

static void graphLayerMirror(const char* layerName1,const char* layerName2)
{
   LayerStruct& layer1 = getCamCadData().getDefinedLayer(layerName1);
   LayerStruct& layer2 = getCamCadData().getDefinedLayer(layerName2);

   layer1.mirrorWithLayer(layer2);
}

char* getStringToken(char* line=NULL)
{
   static char* currentString = NULL;
   static CString currentStringBuffer;
   const char* delimeter = " \t\n";

   bool findEndQuote = false;

   if (line != NULL)
      currentString = line;

   while (*currentString && strchr(delimeter, *currentString))
      *currentString++;

   if (*currentString == NULL)
      return NULL;

   currentStringBuffer.Empty();

   // check if quotes
   if (*currentString == '\"') // go until hit end quote
   {
      findEndQuote = true;
      *currentString++; // skip first quote
   }


   while (*currentString && 
         (
            (findEndQuote && *currentString != '\"') ||           // Go until find valid end-quote
            (!findEndQuote && !strchr(delimeter, *currentString))  // Go until find delimiter
         ))
   {
      // append the token
      currentStringBuffer += *currentString++;
   }

   if (*currentString)
      *currentString++;

   return currentStringBuffer.GetBuffer(0);
}

static bool isDbFlagGlobal(DbFlag flags)
{
   return (flags & BL_GLOBAL) != 0;
}

static CDcaOperationProgress* s_progress = NULL;

static CDcaOperationProgress& getProgress()
{
   return *s_progress;
}

static void setProgress(CDcaOperationProgress* progress)
{
   s_progress = progress;
}

//_____________________________________________________________________________
CAttributes* CDcaVeriBestAsciiReaderGlobal::getAttributes()
{
   CAttributes* attributes = NULL;

   if (m_attributes != NULL)
   {
      if (*m_attributes == NULL)
      {
         *m_attributes = new CAttributes();
      }

      attributes = *m_attributes;
   }

   return attributes;
}

CAttributes** CDcaVeriBestAsciiReaderGlobal::getAttributesPtr()
{
   return m_attributes;
}

void CDcaVeriBestAsciiReaderGlobal::setAttributesPtr(CAttributes** attributes)
{
   m_attributes = attributes;
}

void CDcaVeriBestAsciiReaderGlobal::clearAttributesPtr()
{
   m_attributes = NULL;
}

/******************************************************************************
* ReadVB99   (Camcad HKP VBASCII in)
*/
void DcaReadVB99(CCamCadData& camCadData,const CString& pathname,const CString& userPath,CDcaOperationProgress& progress /*, FormatStruct* Format, int pageunits)*/)
{
   setCamCadData(&camCadData);
   setProgress(&progress);

   file = NULL;
   //doc = Doc;
   display_error = 0;
   PageUnits = getCamCadData().getPageUnits();

   CFilePath vbLogFilePath(GetLogfilePath("vbascii.log"));
   //vbLogFilePath.pushLeaf("vbascii.log");
   CString vbLogFile = vbLogFilePath.getPath();
   //CString vbLogFile = getApp().getUserPath() + "vbascii.log";

   if ((ferr = fopen(vbLogFile,"wt")) == NULL)  
   {
      CString  t;
      t.Format("Error open [%s] file", vbLogFile);
      ErrorMessage(t, "Error");
      return;
   }

   CString  projectname = "VB ASCII Project";   // is the last directory entry e:/www/xxx/vccvv/project
   char     pname[OFS_MAXPATHNAME], *lp;
   strcpy(pname, pathname);

   lp = strtok(pname,"/\\:");
   while (lp)
   {
      projectname = lp;
      lp = strtok(NULL,"/\\:");
   }

   // pathname must have the last '\' for all functions below
   CString validPathname = pathname;

   if (pathname.GetLength() < 1 || pathname.Right(1) != "\\")
      validPathname.Format("%s\\", pathname);

   fprintf(ferr,"Projectname: %s\n",projectname);
   CTime t;
   t = t.GetCurrentTime();
   fprintf(ferr,"%s\n",t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));

   global_init();

   // if a PCB file is alread loaded
   // here find a file->getBlockType() which is marked as the BLOCKTYPE_PCB
   // the return fill be a NULL file, because this only updates
   // an existing file and does not create a new one.

   CStdioFileWriteFormat traceWriteFormat("c:\\traceWriteFormat.txt");
   getLegacyGraph().setTraceWriteFormat(&traceWriteFormat);

   //fl = file = Graph_File_Start(projectname, Type_VB99_Layout);
   //fl = file = camCadData.getFileList().addNewFile(projectname,fileTypeVb99Layout);
   fl = file = getLegacyGraph().graphFileStart(projectname,fileTypeVb99Layout);

   //file->setBlockType(blockTypePcb);
   //file->getBlock()->setBlockType(file->getBlockType());
   cur_filenum = file->getFileNumber();

   // settings must be loaded after fileinit.
   //CString settingsFile = getApp().getUserPath() + "vbascii.in";
   CFilePath settingsFilePath(userPath);
   settingsFilePath.pushLeaf("vbascii.in");
   CString settingsFile = settingsFilePath.getPath();
   load_vb99settings(settingsFile);

   //graphLayer("0","",1);

   keyin_netlist = 0;
   netprops_netlist = 0;
   layout_netlist = 0;

   if (netlistsource.CompareNoCase("netprops.hkp") && netlistsource.CompareNoCase("layout.hkp"))
      keyin_netlist = 1;

   if (!netlistsource.CompareNoCase("netprops.hkp"))
      netprops_netlist = 1;

   if (!netlistsource.CompareNoCase("layout.hkp"))
      layout_netlist = 1;

   resetInputFileLineNumber();                  
   G.push_tok = 0;
   NumberOfLayers = 0; // updated in job pref

   // jobprefs.hkp
   do_jobprefs(validPathname);

   // padstack.hkp
   do_padstack(validPathname);

   // cell.hkp
   do_cell(validPathname);

   // layout.hkp
   do_layout(validPathname);

   // netclass.hkp
   // netprops.hkp

   // pdb.hkp
   do_pdb(validPathname);

   // netlist
   if (keyin_netlist)   
      do_netlist(validPathname);
   else if (netprops_netlist)
      do_netprops(validPathname);

   //COperationProgress operationProgress;
   CDcaOperationProgress operationProgress;
   operationProgress.setLength(0.);

   operationProgress.updateStatus("Generate NC net");
   //generate_NCnet(doc,file,&operationProgress);  // this function generates the NC (non connect) net.
   camCadData.generateNoConnectNets(file->getFileNumber(),&operationProgress);

   // mirror padstack layers.
   graphLayerMirror(PAD_TOP, PAD_BOT);

   // here assign other layers
   do_assign_layers();

   operationProgress.updateStatus("Generating padstack access flags");
   getCamCadData().generatePadstackAccessFlag(true,&operationProgress);

   if (false) // for debugging, keep all geometries - knv
   {
   operationProgress.updateStatus("Purging unused blocks");
   getCamCadData().purgeUnusedBlocks(); 
   }

   operationProgress.updateStatus("Generating pin locations");
   getCamCadData().generatePinLocations(NULL,&operationProgress);

   operationProgress.updateStatus("Processing device types");
   operationProgress.setLength(fl->getTypeList().GetCount());

   // Check for device types that are derived and copy the attribute from the original device type
   //WORD orignialTypeKey = (WORD)doc->RegisterKeyWord("ORIGINAL_DEVICE", 0, VT_STRING);
   int orignialTypeKey = getCamCadData().getDefinedAttributeKeywordIndex("ORIGINAL_DEVICE",valueTypeString);
   
   for (POSITION pos = fl->getTypeList().GetHeadPosition();pos != NULL;)
   {
      TypeStruct *type = fl->getTypeList().GetNext(pos);

      operationProgress.incrementProgress();

      if (type == NULL || type->getAttributesRef() == NULL)
         continue;

      CAttribute *attrib = NULL;

      if (type->getAttributesRef()->Lookup(orignialTypeKey, attrib) && attrib != NULL)
      {
         CString originalTypeName = attrib->getStringValue();
         TypeStruct *originalType = fl->getTypeList().getType(originalTypeName);

         if (originalType != NULL)
         {
            type->attributes().CopyAll(originalType->attributes());
            //doc->CopyAttribs(&type->getAttributesRef(), originalType->getAttributesRef());
         }
      }
   }

   operationProgress.updateStatus("Processing type attributes");
   //RefreshTypetoCompAttributes(doc, SA_OVERWRITE);
   getCamCadData().refreshTypetoCompAttributes();

   operationProgress.updateStatus("Cleaning up names");
   clean_original_name();

   if (do_normalize_bottom_build_geometries)
   {
      operationProgress.updateStatus("Normalizing bottom built geometries");
      //doc->OnToolsNormalizeBottomBuildGeometries();
      getCamCadData().normalizeBottomBuiltGeometries();
   }

   t = t.GetCurrentTime();
   fprintf(ferr,"%s\n",t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));

   fprintf(ferr,"Logfile closed\n");
   fclose(ferr);

   operationProgress.updateStatus("Freeing memory");
   global_free();

   delete fileReadProgress;
   fileReadProgress = NULL;

   //if (display_error)
   //   Logreader(vbLogFile);

   setCamCadData(NULL);
   setProgress(NULL);
   releaseLegacyGraph();
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

/****************************************************************************/
/*
*/
static int update_attr(const char *lp,int typ)
{
   if (attrcnt < MAX_ATTR)
   {
      if ((attr[attrcnt].name = STRDUP(lp)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      attr[attrcnt].atttype = typ;
      attrcnt++;
   }
   else
   {
      // error
   }
   return attrcnt-1;
}

/****************************************************************************
* is_attrib
*/
static int is_attrib(const char *s, int atttype)
{
   for (int i=0; i<attrcnt; i++)
   {
      CString SearchString, BlockName;
     SearchString = attr[i].name;
     BlockName = s;
     if (attr[i].atttype == atttype && CheckName(&SearchString, &BlockName))
        return 1;
   }
   return 0;
}

/******************************************************************************
* load_vb99settings
*/
static void load_vb99settings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   unnamednetcnt = 0;
   testpadtop = FALSE;
   creator = "";
   compoutline = "";
   correct_padstackuniterror = FALSE;
   explode_drawing_cell = FALSE;
   use_generated_data = TRUE;
   netlistsource = "layout.hkp";
   USE_TECHNOLOGY = "(Default)";
   USE_CELL_FANOUT_VIAS = FALSE;
   USE_UNIQUE_NETZERO = TRUE;
   do_normalize_bottom_build_geometries = false;
   reportUnknownCommands = false; // not currently a user control, but might be some day
   padshapePrefix.Empty();

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      MessageBox(NULL, tmp, "VBASCII Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = getStringToken(line)) == NULL)
         continue;

      if (lp[0] == '.')
      {
         // here do it.
         if (!STRICMP(lp, ".UNNAMEDNET"))
         {
            if ((lp = getStringToken()) == NULL)
               continue; 
            CString c = lp;
            c.TrimLeft();
            c.TrimRight();

            if (unnamednetcnt < 30)
            {
               unnamednet[unnamednetcnt] = c;
               unnamednetcnt++;
            }
            else
            {
               fprintf(ferr, "Too many unnamed nets defined.\n");
               display_error++;
            }

         }
         else if (!STRICMP(lp, ".COMPOUTLINE"))
         {
            if ((lp = getStringToken()) == NULL)
               continue; 
            compoutline = lp;
         }
         else if (!STRICMP(lp, ".USETECHNOLOGY"))
         {
            if ((lp = getStringToken()) == NULL)
               continue; 
            USE_TECHNOLOGY = lp;
         }
         else if (!STRICMP(lp, ".TESTPAD"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            testpadtop = !STRCMPI(lp, "TOP");
         }
         else if (!STRICMP(lp, ".CORRECT_PADSTACKUNITERROR"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               correct_padstackuniterror = TRUE;
         }
         else if (!STRICMP(lp, ".USE_GENERATED_DATA"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               use_generated_data = FALSE;
         }
         else if (!STRICMP(lp, ".EXPLODE_DRAWING_CELL"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               explode_drawing_cell = TRUE;
         }
         else if (!STRICMP(lp, ".USE_CELL_FANOUT_VIAS"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               USE_CELL_FANOUT_VIAS = TRUE;
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = getStringToken()) == NULL)
               continue; 
            CString olayer = lp;
            olayer.TrimLeft();
            olayer.TrimRight();

            if ((lp = getStringToken()) == NULL)
               continue; 
            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);

            if (layer_attr_cnt < MAX_LAYERS)
            {
               layer_attr[layer_attr_cnt].name = olayer;
               layer_attr[layer_attr_cnt].attr = laytype;
               layer_attr_cnt++;
            }
            else
            {
               fprintf(ferr, "Too many layer attributes\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            CString a1 = lp;
            a1.MakeUpper();
            a1.TrimLeft();
            a1.TrimRight(); 

            if ((lp = getStringToken()) == NULL)
               continue;
            CString a2 = lp;
            a2.MakeUpper();
            a2.TrimLeft();
            a2.TrimRight();

            VBAttribmap *c = new VBAttribmap;
            attribmaparray.SetAtGrow(attribmapcnt++, c);
            c->attrib = a1;
            c->mapattrib = a2;
         }
         else if (!STRICMP(lp, ".MAKE_INTO_TESTPOINT"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            update_attr(lp, TEST_ATTR);
         }
         else if (!STRICMP(lp, ".USE_UNIQUE_NETZERO"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               USE_UNIQUE_NETZERO = FALSE;
         }
         else if (!STRICMP(lp, ".NORMALIZE_BOTTOM_BUILD_GEOM"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               do_normalize_bottom_build_geometries = true;
         }
         else if (!STRICMP(lp, ".PADSHAPEPREFIX"))
         {
            if ((lp = getStringToken()) == NULL)
               continue;
            padshapePrefix = lp;
         }
      }
   }

   fclose(fp);
}

/******************************************************************************
* do_jobprefs
*/
static void do_jobprefs(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "jobprefs.hkp";

   if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
   {
      CString tmp;

      tmp.Format("Can not open file [%s]", cur_filename);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return;
   }

   setInputFile(cur_fp);
   G.push_tok = 0;
   G.clearAttributesPtr();

   go_command(jobprefs_lst, SIZ_JOBPREFS_LST, 0);
   G.clearAttributesPtr();

   fclose(cur_fp);
}

/******************************************************************************
* do_padstack
*/
static void do_padstack(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "padstack.hkp";

   if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", cur_filename);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return;
   }
   
   setInputFile(cur_fp);
   G.clearAttributesPtr();

   getProgress().updateStatus(cur_filename);

   go_command(pad_lst, SIZ_PAD_LST, 0);
   G.clearAttributesPtr();

   fclose(cur_fp);
}

/******************************************************************************
* do_cell
*/
static void do_cell(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "cell.hkp";

   if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", cur_filename);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return;
   }
   
   setInputFile(cur_fp);
   G.clearAttributesPtr();

   getProgress().updateStatus(cur_filename);

   go_command(cel_lst,SIZ_CEL_LST,0);
   G.clearAttributesPtr();

   fclose(cur_fp);
}

/******************************************************************************
* do_layout
*/
static void do_layout(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "layout.hkp";

   if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", cur_filename);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return;
   }
   
   setInputFile(cur_fp);
   G.push_tok = 0;
   G.clearAttributesPtr();

   getProgress().updateStatus(cur_filename);

   go_command(des_lst,SIZ_DES_LST,0);
   G.clearAttributesPtr();

   fclose(cur_fp);
}

/******************************************************************************
* do_pdb
   this goes into the Tyeplist (DeviceList)
*/
static void do_pdb(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "pdb.hkp";

   if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", cur_filename);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return;
   }

   setInputFile(cur_fp);
   G.push_tok = 0;
   G.clearAttributesPtr();

   getProgress().updateStatus(cur_filename);

   go_command(pdb_lst,SIZ_PDB_LST,0);
   G.clearAttributesPtr();

   fclose(cur_fp);
}

/******************************************************************************
* do_netlist
   first look for netlist.aug
   next look for netlst.kyn
*/
static void do_netlist(CString pathName)
{
   if (strlen(netlistsource))
   {
      cur_filename = pathName;
      cur_filename += netlistsource;
   }
   else
   {
      cur_filename = pathName;
      cur_filename += "netlist.aug";
   }

   if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
   {
      cur_filename = pathName;
      cur_filename += "netlist.kyn";

      if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
      {
         CString tmp;
         tmp.Format("Can not open file [%s]", cur_filename);
         fprintf(ferr, tmp);
         ErrorMessage(tmp);
         return;
      }
   }

   setInputFile(cur_fp);
   G.push_tok = 0;

   getProgress().updateStatus(cur_filename);
   
   read_netfile(file);
   
   fclose(cur_fp);
}

/******************************************************************************
* do_netprops
*/
static void do_netprops(CString pathName)
{

   cur_filename = pathName;
   cur_filename += "netprops.hkp";

   if ((cur_fp = fopen(cur_filename, "rt")) == NULL)  
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", cur_filename);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return;
   }

   setInputFile(cur_fp);
   G.push_tok = 0;
   G.clearAttributesPtr();

   getProgress().updateStatus(cur_filename);

   go_command(netprops_lst,SIZ_NETPROPS_LST,0);
   G.clearAttributesPtr();

   fclose(cur_fp);
}

/******************************************************************************
* go_command
  return -1 for EOF
  return 1  for ident match
  return 0  for ???
*/
static int go_command(List *lst, int siz_lst, int start_ident)
{
   char buf[MAX_LINE];
   char *tok;

   while (TRUE)
   {
      if (!G.push_tok)
      {
         if ((fgets(buf,MAX_LINE, cur_fp)) == NULL)  return -1;
         //printf("%s %8ld\n",cur_filename,getInputFileLineNumber());

         // if line has ! it is a remark !
         if (strlen(buf) && buf[0] == '!')
         {
            // remark;
            buf[0] = '\0';
         }
         
         check_buf(buf);
         
         strcpy(cur_line, buf);
         incrementInputFileLineNumber();
         if ((tok = get_keyword(buf,&cur_ident)) == NULL)  continue;
      }
      else
      {
         // reload strtok
         strcpy(buf, cur_line);
         tok = get_keyword(buf, &cur_ident);
      }

      if (tok[0] == '!')   continue;   // remark 

      if (cur_ident <= start_ident)
      {
       CString keyword = tok;
       if (!keyword.CompareNoCase("DISPLAY_WIDTH"))
       {
          char  *lp;

          lp = strtok(NULL,WORD_DELIMETER);
          G.diameter = cnv_unit(atof(lp));
      
       }
         G.push_tok = TRUE;
         return 1;
      }
      G.push_tok = FALSE;

      int i;
      if ((i = tok_search(tok, lst, siz_lst)) >= 0)
      {
         (*lst[i].function)();
      }
      else
      {
#ifdef _DEBUG
         fprintf(ferr,"DEBUG: Unknown Command [%s] in [%s] at %ld -> skipped til", tok, cur_filename, getInputFileLineNumber());
         display_error++;
#endif

         int res = vb99_null(cur_ident);

#ifdef _DEBUG
            fprintf(ferr, " %ld\n", getInputFileLineNumber());
#endif
            
         if (res < 0)
            return -1; // skip until next on same level.
      }
   }

   return 0;
}

/****************************************************************************/
/*
*/
static const char *get_attribmap(const char *c)
{
   int      i;
   static   CString  tmp;

   for (i=0;i<attribmapcnt;i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return   attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   tmp = c;
   tmp.MakeUpper();
   return tmp.GetBuffer(0);
}

                
/****************************************************************************/
/*
*/
static int get_drillindex(double size, int layernum)
{
   if (size == 0)
      return -1;

   vb99Drill p;
   for (int i=0; i<drillcnt; i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size)
         return i;         
   }

   CString name = "";
   name.Format("DRILL_%d", drillcnt);
   //BlockStruct  *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
   //curblock->blocktype = BLOCKTYPE_PADSHAPE;

   getCamCadData().getDefinedToolIndex(size,name);
   //Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   p.d = size;
   drillarray.SetAtGrow(drillcnt++, p);  

   return drillcnt -1;
}
     

/******************************************************************************
* do_assign_layers
   make layers more understandable for vb99 users.
   do mirror layers
*/
static void do_assign_layers()
{
   LayerStruct* lp = getCamCadData().getLayerArray().getLayer("INSERTION_TOP");
   int j = 0;

   if (lp != NULL)
   {
      graphLayerMirror("INSERTION_TOP", "INSERTION_BOTTOM");
   }

   graphLayerMirror("ASSEMBLY_TOP", "ASSEMBLY_BOTTOM");
   graphLayerMirror("SILKSCREEN_TOP", "SILKSCREEN_BOTTOM");
   graphLayerMirror(PAD_TOP_SOLDERMASK, PAD_BOT_SOLDERMASK);
   graphLayerMirror(PAD_TOP_SOLDERPASTE, PAD_BOT_SOLDERPASTE);
	
   for (j = 0;j < getCamCadData().getLayerArray().GetSize();j++)
   {
      lp = getCamCadData().getLayer(j);

      if (lp == NULL)   continue; // could have been deleted.

      // special for SMD PagetMaxLayerIndex()ds
      if (!lp->getName().CompareNoCase("DRILLHOLE"))
      {
         lp->setLayerType(layerTypeDrill);
      }

      // special for SMD pads.
      if (!lp->getName().CompareNoCase(SMD_TOP))
      {
         lp->setFlagBits(LY_NEVERMIRROR); 
         lp->setLayerType(layerTypePadTop);
      }

      if (!lp->getName().CompareNoCase(SMD_BOT))
      {
         lp->setFlagBits(LY_MIRRORONLY); 
         lp->setLayerType(layerTypePadBottom);
      }

      // special for SMD pads.
      if (!lp->getName().CompareNoCase(SMDPASTE_TOP))
      {
         lp->setFlagBits(LY_NEVERMIRROR); 
         lp->setLayerType(layerTypePasteTop);
      }

      if (!lp->getName().CompareNoCase(SMDPASTE_BOT))
      {
         lp->setFlagBits(LY_MIRRORONLY); 
         lp->setLayerType(layerTypePasteBottom);
      }

      // special for SMD pads.
      if (!lp->getName().CompareNoCase(SMDMASK_TOP))
      {
         lp->setFlagBits(LY_NEVERMIRROR); 
         lp->setLayerType(layerTypeMaskTop);
      }

      if (!lp->getName().CompareNoCase(SMDMASK_BOT))
      {
         lp->setFlagBits(LY_MIRRORONLY); 
         lp->setLayerType(layerTypeMaskBottom);
      }

      graphLayerMirror(PAD_TOP, PAD_BOT);

      if (!lp->getName().CompareNoCase(PAD_TOP))
      {
         lp->setLayerType(layerTypePadTop);
         lp->setElectricalStackNumber(1);
      }

      if (!lp->getName().CompareNoCase(PAD_BOT))
      {
         lp->setLayerType(layerTypePadBottom);
         lp->setElectricalStackNumber(NumberOfLayers);
      }

      if (!lp->getName().CompareNoCase(PAD_INT))
      {
         lp->setLayerType(layerTypePadInner);
      }

      if (!lp->getName().CompareNoCase("DRILL_HOLE"))
      {
         lp->setLayerType(layerTypeDrill);
      }

      if (!lp->getName().CompareNoCase("PLACEMENT_OUTLINE"))
      {
         lp->setLayerType(layerTypeComponentOutline);
      }

      if (!lp->getName().CompareNoCase("ASSEMBLY_TOP"))
      {
         lp->setLayerType(layerTypeTop);
      }

      if (!lp->getName().CompareNoCase("INSERTION_TOP"))
      {
         lp->setLayerType(layerTypeTop);
      }

      if (!lp->getName().CompareNoCase("ASSEMBLY_BOTTOM"))
      {
         lp->setLayerType(layerTypeBottom);
      }

      if (!lp->getName().CompareNoCase("INSERTION_BOTTOM"))
      {
         lp->setLayerType(layerTypeBottom);
      }

      if (!lp->getName().CompareNoCase("SILKSCREEN_TOP"))
      {
         lp->setLayerType(layerTypeSilkTop);
      }

      if (!lp->getName().CompareNoCase("SILKSCREEN_BOTTOM"))
      {
         lp->setLayerType(layerTypeSilkBottom);
      }

      if (!lp->getName().CompareNoCase(PAD_TOP_SOLDERMASK))
      {
         lp->setLayerType(layerTypeMaskTop);
      }

      if (!lp->getName().CompareNoCase(PAD_BOT_SOLDERMASK))
      {
         lp->setLayerType(layerTypeMaskBottom);
      }

      if (!lp->getName().CompareNoCase(PAD_TOP_SOLDERPASTE))
      {
         lp->setLayerType(layerTypePasteTop);
      }

      if (!lp->getName().CompareNoCase(PAD_BOT_SOLDERPASTE))
      {
         lp->setLayerType(layerTypePasteBottom);
      }

      if (!STRNICMP(lp->getName(),"PadLayer",strlen("PadLayer")))
      {
         int lnr = atoi(lp->getName().Right((int)strlen(lp->getName()) - (int)strlen("PadLayer")));
         if (lnr == 1)
         {
            lp->setLayerType(layerTypePadTop);
            lp->setElectricalStackNumber(1);
            CString  padlayerbottom;
            padlayerbottom.Format("PadLayer%d", NumberOfLayers);
            graphLayerMirror(lp->getName(), padlayerbottom);
         }
         else
         if (lnr == NumberOfLayers)
         {
            lp->setLayerType(layerTypePadBottom);
            lp->setElectricalStackNumber(NumberOfLayers);
            graphLayerMirror("PadLayer1", lp->getName());
         }
         else
         {
            // inner layers
            lp->setLayerType(layerTypePadInner);
            lp->setElectricalStackNumber(lnr);
         }
      }
   }

   // here attributes from vbascii.in
   for (j=0;j < getCamCadData().getLayerArray().GetSize(); j++)
   {
      LayerStruct *l = getCamCadData().getLayer(j);

      if (l == NULL) continue; // could have been deleted.

      CString  lname = l->getName();

      for (int i=0;i < layer_attr_cnt;i++)
      {
         if (lname.CompareNoCase(layer_attr[i].name) == 0)
         {
            l->setLayerType(layer_attr[i].attr);

            break;
         }
      }
   }
}


/******************************************************************************
* clean_original_name
*/
static void clean_original_name()
{
   for (int blockIndex=0;blockIndex < getCamCadData().getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = getCamCadData().getBlock(blockIndex);

      if (block != NULL && !block->getOriginalName().IsEmpty())
      {
         // here now check if a block with the original name exist. If not - change the real name and
         // kill the original name
         //if (Graph_Block_Exists(doc, block->getOriginalName(), -1) == NULL)
         if (getLegacyGraph().graphBlockExists(block->getOriginalName(),-1) == NULL)
         {
            block->setName(block->getOriginalName());
            block->setOriginalName("");
         }
      }
   }
}

/****************************************************************************/
/*
*/
void global_init()
{
   if ((attr = (VB99Attr  *)calloc(MAX_ATTR,sizeof(VB99Attr))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   attrcnt = 0;

   attribmaparray.SetSize(100,100);
   attribmapcnt = 0;

   if ((poly_l = (Point2 *)calloc(MAX_POLY, sizeof(Point2))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   if ((localpad = (Localpad *)calloc(MAX_LOCALPAD,sizeof(Localpad))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   localpadcnt = 0;

   if ((curpad = (CurPad *)calloc(MAX_CURPAD,sizeof(CurPad))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   curpad_cnt = 0;

   G.push_tok = FALSE;
   G.clearAttributesPtr();
   G.dialeccnt = 0;
   G.name.Empty();
   G.name_1.Empty();
   G.pinname.Empty();
   G.symbol.Empty();
   G.ref_des.Empty();
   G.level.Empty();
   G.geomtyp = apertureUndefined;
   G.lyrnr = G.dialeccnt = G.just = G.push_tok = G.smd = G.pincnt = G.notplated= 0;
   G.thickness = G.textwidth = 0.0;
   G.mirror = G.sidebot = G.padstackshape = G.comp_mirror = G.fanout_trace = 0;
   G.xdim = G.ydim = G.xoff = G.yoff = 0.0;
   G.diameter = G.radius = G.height = G.rotation = G.chamfer = G.thermal_clearance = G.tie_leg_width = 0.0;
   G.comp_x = G.comp_y = G.comp_rotation = 0.0;
   G.startangle = G.deltaangle = 0.0;
   G.text_type.Empty();
   G.refdes_Count = 0;

   lastpininsert = NULL;

   VBGeomList.Empty();
   if (curVBGeom != NULL)
   {
      delete curVBGeom;
      curVBGeom = NULL;
   }
}

void global_free()
{
   int   i;

   for (i=0;i<attribmapcnt;i++)
   {
      delete attribmaparray[i];
   }
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   free(poly_l);
   polycnt = 0;

   for (i=0;i<localpadcnt;i++)
   {
      free(localpad[i].curpad);
   }
   free(localpad);
   localpadcnt = 0;

   free(curpad);

   for (int i=0;i<attrcnt;i++)
      free(attr[i].name);
   free(attr);

   VBGeomList.Empty();
   if (curVBGeom != NULL)
   {
      delete curVBGeom;
      curVBGeom = NULL;
   }
}

/****************************************************************************/
/*
  check if text does not only consist of space
*/
static int is_text(char *n)
{
   if (n == NULL) 
   {
      // this is allows
      //fprintf(ferr,"Empty text at %ld in %s\n",getInputFileLineNumber(),cur_filename);
      //display_error++;
      return FALSE;
   }

   int len = (int)strlen(n);

   for (int i = 0;i < len;i++)
   {
      if (!isspace(n[i]))  return TRUE;
   }

   return FALSE;
}

/****************************************************************************/
/*
  first advanve to begin of "
  than get the word
*/
char *strword()
{
   char  *lp;

   lp = strtok(NULL, "\"");     // get the first "
   if (is_text(lp))
   {
      // why this stament ??? text can start with blank. " Blank"
      // if (!isspace(lp[0])) return lp;
      return lp;  
   }
   lp = strtok(NULL, "\"");     // get until next "

   return lp;
}

/****************************************************************************/
/*
  calc conversion factor to get units to inch
*/
double get_units(char *lp)
{
   PageUnitsTag fromUnits = PageUnits;

   if (STRCMPI(lp,"IN") == 0)
   {
      fromUnits = pageUnitsInches;
   }
   else if (STRCMPI(lp,"TH") == 0)
   {
      fromUnits = pageUnitsMils;
   }
   else if (STRCMPI(lp,"MM") == 0)
   {
      fromUnits = pageUnitsMilliMeters;
   }
   else if (STRCMPI(lp,"UM") == 0)
   {
      fromUnits = pageUnitsMicroMeters;
   }
   else if ((STRCMPI(lp,"NM") == 0) || (STRCMPI(lp,"1NM") == 0))
   {
      fromUnits = pageUnitsNanoMeters;
   }
   else
   {
      fprintf(ferr,"Unknown UNITS [%s]\n",lp);
      display_error++;
   }

   double units = getUnitsFactor(fromUnits, PageUnits);
 
   return units;
}

/****************************************************************************/
/*
  convert from units to inch
*/
/****************************************************************************/
/*
   Converts from PADS any units to INCH units.
*/
static double   cnv_unit(double x)
{
   return(x * unit_faktor);
}

/******************************************************************************
* get_keyword
  vb99 keyword is ...XXX
  ident is the number of ...
  the return is the pointer to the keyword.
*/
char *get_keyword(char *buf, int *ident)
{
   unsigned int i = 0;
   while (i < strlen(buf) && isspace(buf[i]))
      i++;

   *ident = 0;
   while (i < strlen(buf) && buf[i] == '.')
   {
      *ident = *ident + 1;
      i++;
   }

   if (*ident == 0)
      return NULL;

   char *tok = strtok(&(buf[i]), " \t");
   return tok;
}

/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(char *lp, List *tok_lst,int tok_size)
{
   int      i;

   for (i = 0; i < tok_size; ++i)
   {
      // this must look for the complete token, because
      // PAD and PADSTACK is a difference
      if (!STRICMP(lp,tok_lst[i].token))  
         return(i);
   }
   return(-1);
}

/****************************************************************************/
/*
*/
int vb99_number_layers()
{
   char *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   NumberOfLayers = atoi(lp);

   return 1;
}


/****************************************************************************/
/*
*/
int vb99_padstack()
{
   char *lp;

   lp = strword();
   G.symbol.Format("XPAD_%s", lp);  // padstacks can have the same name as a package

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_padrot()
{
   char *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.rotation = atof(lp);

   if (G.rotation >= 360)
      G.rotation -= 360;
   if (G.rotation < 0)
      G.rotation += 360;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_pinname()
{
   char *lp;

   lp = strword();
   G.pinname = lp;

   return 1;
}

/******************************************************************************
* vb99_pin_options
*/
int vb99_pin_options()
{
   char *lp = strword();
   G.name_1 = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int part_cell_xy()
{

   polycnt = 0;
   vb99_xy();

   if (polycnt)
   {
      G.comp_x = poly_l[0].x;
      G.comp_y = poly_l[0].y;
   }
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
  The syntax is
      ...XY (x y)
            (x y)

*/
static int vb99_xy()
{
   char  *lp;
   char  buf[MAX_LONGLINE];
   int   id = 0;
   int   try_cnt = 0;

   while (TRUE)
   {
      if (try_cnt)
      {
         if ((fgets(buf,MAX_LONGLINE,cur_fp)) == NULL)  return -1;
         check_buf(buf);
         strcpy(cur_line,buf);
         incrementInputFileLineNumber();
         // check if this is a keyword. If yes, you are done.
         if ((lp = get_keyword(buf,&id)) != NULL && id > 0)
         {
            G.push_tok = TRUE;
            break;
         }
         else
         {
            strcpy(buf,cur_line);   // just reset this here.
            lp = strtok(buf,"(");
         }
      }

      // here do it also for XY () () ()
      while (TRUE)
      {
         if (polycnt < MAX_POLY)
         {
            if ((lp = strtok(NULL," (,\t")) == NULL)
            {
               break;
            }
            poly_l[polycnt].x = cnv_unit(atof(lp));
            if ((lp = strtok(NULL," \t)")) == NULL)
            {
               long l = getInputFileLineNumber();
               break;
            }
            poly_l[polycnt].y = cnv_unit(atof(lp));
            poly_l[polycnt].bulge = 0.0;
            polycnt++;
         }
         else
         {
            fprintf(ferr,"Too many polylines\n");
            display_error++;
            return -1;
         }
      }
      try_cnt++;
   }

   return 1;
}
/****************************************************************************/
/*
  The syntax is
      ...XYR (x y r) r = radius
             (x y r)

   negative radius = ccw
   positive radius = cw 

*/
static int vb99_xyr()
{
   char  *lp;
   char  buf[MAX_LONGLINE];
   int   id = 0;
   int   try_cnt = 0;

   while (TRUE)
   {
      if (try_cnt)
      {
         if ((fgets(buf,MAX_LONGLINE,cur_fp)) == NULL)  return -1;
         check_buf(buf);
         strcpy(cur_line,buf);
         incrementInputFileLineNumber();
         // check if this is a keyword. If yes, you are done.
         if ((lp = get_keyword(buf,&id)) != NULL && id > 0)
         {
            G.push_tok = TRUE;
            break;
         }
         else
         {
            strcpy(buf,cur_line);   // just reset this here.
            lp = strtok(buf,"(");
         }
      }

      // here do it also for XYR () () ()
      while (TRUE)
      {
         if (polycnt < MAX_POLY)
         {
            if ((lp = strtok(NULL," (,\t")) == NULL)
            {
               break;
            }
            poly_l[polycnt].x = cnv_unit(atof(lp));
            if ((lp = strtok(NULL," \t)")) == NULL)
            {
               long l = getInputFileLineNumber();
               break;
            }
            poly_l[polycnt].y = cnv_unit(atof(lp));
            if ((lp = strtok(NULL," \t)")) == NULL)
            {
               long l = getInputFileLineNumber();
               break;
            }
            // this is the radius, not a bulge.
            poly_l[polycnt].bulge = cnv_unit(atof(lp));
            polycnt++;
         }
         else
         {
            fprintf(ferr,"Too many polylines\n");
            display_error++;
            return -1;
         }
      }
      try_cnt++;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int graph_side()
{
   char *lp;

   lp = strword();

   // opp_side -- if it is mount side, the layer does not need to get changed
   if (!STRCMPI(lp,"MNT_SIDE"))
      return 1;

   if (cur_status == FILE_SOLDER_MASK)
   {
      if (curVBGeom != NULL)
      {
         curVBGeom->SetSoldermaskSide(lp);
         return 1;
      }
      else
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = PAD_BOT_SOLDERMASK;
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = PAD_BOT_SOLDERMASK;
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"TOP"))
      {
         G.level = PAD_TOP_SOLDERMASK;
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
   }
   else
   if (cur_status == FILE_ASSEMBLY_OUTL)
   {
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = "ASSEMBLY_BOT";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = "ASSEMBLY_BOT";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"TOP"))
      {
         G.level ="ASSEMBLY_TOP";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
   }
   else
   if (cur_status == FILE_INSERTION_OUTL)
   {
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = "INSERTION_BOT";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = "INSERTION_BOT";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"TOP"))
      {
         G.level ="INSERTION_TOP";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
   }
   else
   if (cur_status == FILE_SILK_ARTWORK)
   {
      if (curVBGeom != NULL)
      {
         curVBGeom->SetSilkscreenSide(lp);
         return 1;
      }
      else
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = "SILKSCREEN_BOT";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = "SILKSCREEN_BOTTOM";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
      else
      if (!STRCMPI(lp,"TOP"))
      {
         G.level ="SILKSCREEN_TOP";
         G.lyrnr = graphLayer(G.level,"",0);
         return 1;
      }
   }

   fprintf(ferr,"Side [%s] not evaluated in [%s] at %ld\n", lp, cur_filename, getInputFileLineNumber());
   display_error++;

   return 1;
}

/****************************************************************************/
/*
   This can be USER_LVL_%d
   or the layer comment name. In this case get is back to USER_LVL name
   and on the end all gets changed.
*/
static int vb99_lyr()
{
   char* lp;
   lp = strword();
   
   G.level = lp;

   if (strlen(G.level) == 0)
   {
      G.lyrnr = getCamCadData().getDefinedFloatingLayer().getLayerIndex(); 
   }
   else
   {
      if (!G.level.CompareNoCase("ASSEMBLY"))
      {
         G.level = "ASSEMBLY_TOP";
      }
      else if (!G.level.CompareNoCase("SILKSCREEN_MNT_SIDE"))
      {
         G.level = "SILKSCREEN_TOP";
      }
      else if (!G.level.CompareNoCase("SILKSCREEN_OPP_SIDE"))
      {
         G.level = "SILKSCREEN_BOTTOM";
      }

      LayerStruct* layer;

      for (int layerIndex = 0;layerIndex < getCamCadData().getLayerArray().GetSize();layerIndex++)
      {
         layer = getCamCadData().getLayer(layerIndex);

         if (layer != NULL && !layer->getComment().IsEmpty() && layer->getComment().CompareNoCase(G.level) == 0)
         {
            G.level = layer->getName();
         }
      }

      G.lyrnr = graphLayer(G.level,"",0); 
   }

   return 1;
}

/****************************************************************************/
/*
  Skip this one to the next level.
*/
static int vb99_skip()
{
   // Skip is called on purpose, while vb99_null may be called due to unrecognized
   // commands and keywords. If being skipped on purpose, then do not issue
   // the "unknown" log file message.

   bool savedSetting = reportUnknownCommands;
   reportUnknownCommands = false;

   int retval = (vb99_null(cur_ident) < 0) ? -1 : 1; 

   reportUnknownCommands = savedSetting;

   return retval;
}


/****************************************************************************/
/*
*/
static int vb99_units()
{
   char *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   unit_faktor = get_units(lp);
   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_units()
{
   char *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   if (!cur_version.CompareNoCase("VB99"))
   {
      if (correct_padstackuniterror && STRCMPI(lp,"TH")) // if not th
      {
         lp = "TH";  // there is an error in VB99 padstack - the pads are always in TH units.
      }
   }

   unit_faktor = get_units(lp);
   return 1;
}

/****************************************************************************/
/*
  return -1 for EOF
  return 1  for ident match
  return 0  for ???
*/
static int start_command(List *lst,int siz_lst,int start_ident)
{
   char  buf[MAX_LINE];
   char  *lp;
   int   i;

   while (TRUE)
   {
      if (!G.push_tok)
      {
         if ((fgets(buf,MAX_LINE,cur_fp)) == NULL)  return -1;

         //printf("%s %8ld\n",cur_filename,getInputFileLineNumber());

         check_buf(buf);
         strcpy(cur_line,buf);
         incrementInputFileLineNumber();
         if ((lp = get_keyword(buf,&cur_ident)) == NULL)  continue;
      }
      else
      {
         // reload strtok
         strcpy(buf,cur_line);
         lp = get_keyword(buf,&cur_ident);
      }

      if (cur_ident <= start_ident)
      {
         G.push_tok = TRUE;
         return 1;
      }
      G.push_tok = FALSE;

      if ((i = tok_search(lp,lst,siz_lst)) >= 0)
      {
         (*lst[i].function)();
      }
      else
      {
         G.push_tok = TRUE;
         return 1;
      }
   }
   return 0;
}

/****************************************************************************/
/*
  return -1 for EOF
  return 1  for ident match
  return 0  for ???
*/
static int vb99_null(int start_ident)
{
   char     *lp;
   CString  tmp;

   if (reportUnknownCommands)
   {
      if (unknownCommandMessage.IsEmpty())
         unknownCommandMessage = "Unknown command";

      fprintf(ferr, "%s: %s  in [%s] at %ld\n", unknownCommandMessage, cur_line, cur_filename, getInputFileLineNumber());
   }

   while (TRUE)
   {
      if (!G.push_tok)
      {
         if ((fgets(buf,MAX_LONGLINE,cur_fp)) == NULL)  return -1;
         incrementInputFileLineNumber();
         check_buf(buf);
         clean_blank(buf);
         // XY poly may not have a . in the beginning
         if (buf[0] != '.')
            continue;
         strcpy(cur_line,buf);
         if ((lp = get_keyword(buf,&cur_ident)) == NULL)  continue;
      }
      else
      {
         // reload strtok
         strcpy(buf,cur_line);
         lp = get_keyword(buf,&cur_ident);
      }

      if (cur_ident <= start_ident)
      {
         G.push_tok = TRUE;
         return 1;
      }
      G.push_tok = FALSE;
   }

  return 0;
}

/****************************************************************************/
/*
  here do NAME, ROUTING ... etc.
*/
static int laystack_layer_num()
{
   int   id = cur_ident;
   char  *lp;
   int   physnr;

   lp = strtok(NULL,WORD_DELIMETER);
   physnr = atoi(lp);

   go_command(lay_lst,SIZ_LAY_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_name()
{
   char *lp;

   lp = strword();
   G.name = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_layer()
{
   char *lp;

   lp = strword();
   G.name = clean_level(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static void clean_blank(char *n)
{
   // eliminame leading and trailing blanks
   while (strlen(n) > 0 && isspace(n[strlen(n)-1]))   n[strlen(n)-1] = '\0';
   STRREV(n);
   while (strlen(n) > 0 && isspace(n[strlen(n)-1]))   n[strlen(n)-1] = '\0';
   STRREV(n);
   return;
}

/****************************************************************************/
/*
*/
static char *clean_quote(char *n)
{
   CString  w;
   w = n;
   w.TrimLeft();
   w.TrimRight();
   strcpy(n,w);

   if (strlen(n) && n[strlen(n)-1] == '"')   n[strlen(n)-1] = '\0';
   STRREV(n);
   if (strlen(n) && n[strlen(n)-1] == '"')   n[strlen(n)-1] = '\0';
   STRREV(n);

   return n;
}

/****************************************************************************/
/*
  elimitade blank: I have seen examples with
   "Trace Level 1" and "Trace Level  1"
*/
char *clean_level(char *n)
{
   unsigned int   i;
   int   t = 0;
   char  tmp[80];

   for (i= 0 ;i<strlen(n);i++)
   {
      if (!isspace(n[i]))
         tmp[t++] = n[i];
   }
   tmp[t] = '\0';
   strcpy(n,tmp);
   return n;
}

/****************************************************************************/
/*
*/
static int pad_pad()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();

   G.name.Format("%s %s", padshapePrefix, lp);   // can have the same name as a PADSTACK
   G.name.Trim();

   //if (G.name.Find("1.160") >= 0)
   //{
   //   int iii = 3;
   //}

   G.geomtyp = apertureUndefined;
   G.xdim = 0;
   G.ydim = 0;
   G.xoff = 0;
   G.yoff = 0;
   G.diameter = 0;

   bool savedSetting = reportUnknownCommands;
   reportUnknownCommands = true; // case dts0100386334, want to know about skipped pad shape commands
   unknownCommandMessage = "Unrecognized PAD";
   go_command(padform_lst,SIZ_PADFORM_LST,id);
   reportUnknownCommands = savedSetting;

   // here make an aperture
   if (G.geomtyp == T_TRIANGLE)
   {
      fprintf(ferr,"Here need to make a triangle aperture\n");
      //Graph_Aperture(G.name, apertureRound, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0,BL_APERTURE,TRUE,&err);
      getCamCadData().getDefinedAperture(G.name,apertureRound,G.xdim,G.ydim,G.xoff,G.yoff,0.,-1,BL_APERTURE,0);
   }
   else if (G.geomtyp == apertureComplex)
   {
      CString  usr_name;
      usr_name.Format("SHAPE_%s", G.name);
      //Graph_Complex(G.name, 0, usr_name, 0.0, 0.0, 0.0);
      //getCamCadData().getDefinedComplexAperture(G.name,usr_name);  // didn't work - knv
      getLegacyGraph().graphComplex(-1,G.name,0,usr_name,0.,0.,0.);
   }
   else
   {
      //Graph_Aperture(G.name, G.geomtyp, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0,BL_APERTURE, TRUE, &err);
      getCamCadData().getDefinedAperture(G.name,G.geomtyp,G.xdim,G.ydim,G.xoff,G.yoff,0.,-1,BL_APERTURE);
   }

   //if (G.name.Find("1.160") >= 0)
   //{
   //   int iii = 3;

   //   CStdioFileWriteFormat debugWriteFormat("c:\\ppad_pad-debug.txt");
   //   debugWriteFormat.setNewLineMode(true);

   //   getCamCadData().getBlockDirectory().dump(debugWriteFormat);
   //}

   return 1;
}

/******************************************************************************
* pad_hole
*/
static int pad_hole()
{
   char *lp = strword();
   G.name.Format(".HOLE_%s", lp);   // can have same name as padstack

   G.geomtyp = apertureUndefined;
   G.xdim = 0;
   G.ydim = 0;
   G.xoff = 0;
   G.yoff = 0;

   int id = cur_ident;
   go_command(hole_lst, SIZ_HOLE_LST, id);
   
   int layernum = graphLayer("DRILLHOLE", "", 0);
   getCamCadData().getDefinedToolIndex(G.xdim,G.name);
   //Graph_Tool(G.name, 0, G.xdim, 0, 0, 0, 0L);

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_smd()
{
   char  *lp;

   G.smd = FALSE;
   lp = strword();

   if (!STRCMPI(lp, "MOUNTING_HOLE"))
   {
      curblock->setBlockType(blockTypeTooling);
   }
   else
   if (!STRCMPI(lp, "FIDUCIAL"))
   {
      G.smd = TRUE;
      curblock->setBlockType(blockTypeFiducial);
   }
   else
   if (!STRCMPI(lp, "PIN_SMD"))
      G.smd = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_technology()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();

   if (!STRCMPI(lp,USE_TECHNOLOGY))
      go_command(padstack_lst,SIZ_PADSTACK_LST,id);
   else
   {
#ifdef _DEBUG
      fprintf(ferr, "DEBUG: Padstack Technology [%s] skipped.\n", lp);
      display_error++;
#endif
      vb99_skip();
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pdb_name()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
   {
      //doc->SetUnknownAttrib(&curtype->getAttributesRef(),ATT_NAME, name, SA_OVERWRITE, NULL);
      curtype->attributes().setAttribute(getCamCadData(),standardAttributeName,name);
   }

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
static int pdb_prop()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  key;
   
   key = lp;
   key.TrimLeft();
   key.TrimRight();
   key.MakeUpper();

   lp = strword();   // get ,

   lp = strword();
   CString  val;
   
   val = lp;
   val.TrimLeft();
   val.TrimRight();

   lp = strword();   // get ,
   lp = strword();   // attrib type

   if (strlen(key))
   {
      //doc->SetUnknownAttrib(&curtype->getAttributesRef(),get_attribmap(key), val, SA_OVERWRITE, NULL);
      int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex(key,valueTypeString);
      curtype->attributes().setAttribute(getCamCadData(),keywordIndex,val);
   }

   vb99_skip();

   return 1;
}


/****************************************************************************/
/*
*/
static int pdb_refprefix()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
   {
      //doc->SetUnknownAttrib(&curtype->getAttributesRef(),"VB_REFPREFIX", name, SA_OVERWRITE, NULL);
      int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex("VB_REFPREFIX",valueTypeString);
      curtype->attributes().setAttribute(getCamCadData(),keywordIndex,name);
   }

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
static int pdb_label()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
   {
      //doc->SetUnknownAttrib(&curtype->getAttributesRef(),"VB_LABEL", name, SA_OVERWRITE, NULL);
      int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex("VB_LABEL",valueTypeString);
      curtype->attributes().setAttribute(getCamCadData(),keywordIndex,name);
   }

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
static int pdb_desc()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
   {
      //doc->SetUnknownAttrib(&curtype->getAttributesRef(),"VB_DESC", name, SA_OVERWRITE, NULL);
      int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex("VB_DESC",valueTypeString);
      curtype->attributes().setAttribute(getCamCadData(),keywordIndex,name);
   }

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
static int netprops_pins()
{
   int   id = cur_ident;

   go_command(netprops_pins_lst,SIZ_NETPROPS_PINS_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int netprops_pin_section()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();   // count 

   go_command(netprops_pins_lst,SIZ_NETPROPS_PINS_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int netprops_pins_refpin()
{
   int      id = cur_ident;
   char     *lp;
   CString  compname, pinname;

   lp = strword();
   compname = lp;
   int index = compname.ReverseFind('-');
   if (index < 0) 
   {
      fprintf(ferr, "Unknown Ref_pin [%s] in [%s] at %ld\n", compname, cur_filename, getInputFileLineNumber());
      display_error++;
      return 1;
   }

   pinname = compname.Right((int)strlen(compname) - index - 1);
   compname.Delete(index, (int)strlen(compname) - index);

   while ((index = compname.ReverseFind('-')) >= 0)
   {
      if (IsCompExist(compname))
         break;

      pinname = compname.Right((int)strlen(compname) - index) + pinname;
      compname.Delete(index, (int)strlen(compname) - index);
   }  

   // "U22-27"
   // netname is in G.name
   //NetStruct *n = add_net(fl, G.name);
   //add_comppin(fl, n, compname, pinname);

   NetStruct* net = fl->getNetList().addNet(G.name);
   net->addCompPin(compname,pinname);
   
   return 1;
}

/******************************************************************************
* IsCompExist
*/
static BOOL IsCompExist(CString compname)
{
   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() != dataTypeInsert)
         continue;
      if (data->getInsert()->getInsertType() != insertTypePcbComponent)
         continue;
      if (data->getInsert()->getInsertType() != insertTypeTestPoint)
         continue;

      if (!compname.CompareNoCase(data->getInsert()->getRefname()))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* pad_padstack
*/
static int pad_padstack()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.name.Format("XPAD_%s", lp); // padstacks can have the same name as a package
   G.smd = FALSE;
   G.notplated = FALSE;
   G.padstackshape = 0;

   curblock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, G.name, -1, 0);
   curblock->setBlockType(blockTypePadstack);
   curblock->setOriginalName(lp);

   topPadName = "";
   bottomPadName = "";
   innerPadName = "";
   thermalPadName = "";
   topSoldermaskPadName = "";
   bottomSoldermaskPadName = "";
   topSolderPastePadName = "";
   bottomSolderPastePadName = "";
   clearancePadName = "";
   holeName = "";

   go_command(pad_padstack_lst, SIZ_PAD_PADSTACK_LST, id);

   if (innerPadName.IsEmpty() && holeName.IsEmpty()) // no internal pad .. must be an SMD
   {
      if (topPadName == bottomPadName) // if top and bottom is the same
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topPadName);
         padname.Trim();

         int layerindex = graphLayer(PAD_TOP, "", 0);   // use a standart layer
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();
   
         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
         bottomPadName = "";
      }
      else if (!topPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topPadName);
         padname.Trim();

         int layerindex = graphLayer(SMD_TOP, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      // still in SMD (no inner layers).
      if (!bottomPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomPadName);
         padname.Trim();

         int layerindex = graphLayer(SMD_BOT, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      // true smd if top and bot to it can be mirrored.
      if (!topPadName.IsEmpty())
      {
         //doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);
         curblock->attributes().setAttribute(getCamCadData(),standardAttributeSmd);
      }
      else
      {
         //doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);
         curblock->attributes().setAttribute(getCamCadData(),standardAttributeBlind);
      }

      if (topSoldermaskPadName == bottomSoldermaskPadName) // if top and bottom is the same
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSoldermaskPadName);
         padname.Trim();

         int layerindex = graphLayer(PAD_TOP_SOLDERMASK, "", 0); // use a standart layer
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();
   
         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
         bottomSoldermaskPadName = "";
      }
      else if (!topSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSoldermaskPadName);
         padname.Trim();

         int layerindex = graphLayer(SMDMASK_TOP, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSoldermaskPadName);
         padname.Trim();

         int layerindex = graphLayer(SMDMASK_BOT, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (topSolderPastePadName == bottomSolderPastePadName)   // if top and bottom is the same
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSolderPastePadName);
         padname.Trim();

         int layerindex = graphLayer(PAD_TOP_SOLDERPASTE,"", 0); // use a standart layer
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();
   
         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
         bottomSolderPastePadName = "";
      }
      else if (!topSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSolderPastePadName);
         padname.Trim();

         int layerindex = graphLayer(SMDPASTE_TOP, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSolderPastePadName);
         padname.Trim();

         int layerindex = graphLayer(SMDPASTE_BOT, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

   }
   else // innerPad exists
   {
      if (!topPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topPadName);
         padname.Trim();

         int layerindex = getLegacyGraph().graphLevel(PAD_TOP, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomPadName);
         padname.Trim();

         int layerindex = getLegacyGraph().graphLevel(PAD_BOT, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!innerPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, innerPadName);
         padname.Trim();

         int layerindex = getLegacyGraph().graphLevel(PAD_INT, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!holeName.IsEmpty())
      {
         int layernum = getLegacyGraph().graphLevel("DRILLHOLE", "", 0);

         getLegacyGraph().graphBlockReference(holeName, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
      }

      if (!topSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSoldermaskPadName);
         padname.Trim();

         int layerindex = getLegacyGraph().graphLevel(PAD_TOP_SOLDERMASK,"",0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!topSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSolderPastePadName);
         padname.Trim();

         int layerindex = getLegacyGraph().graphLevel(PAD_TOP_SOLDERPASTE, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSoldermaskPadName);
         padname.Trim();

         int layerindex = getLegacyGraph().graphLevel(PAD_BOT_SOLDERMASK, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSolderPastePadName);
         padname.Trim();

         int layerindex = getLegacyGraph().graphLevel(PAD_BOT_SOLDERPASTE, "", 0);
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, padname, -1, 0);
         getLegacyGraph().graphBlockOff();

         getLegacyGraph().graphBlockReference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
   }

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
static int pad_custom_drill_symbol()
{
   char  *lp;
   int   id = cur_ident;
   int   oldstat = cur_status;

   lp = strword();
   G.name = lp;

   curblock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,G.name,-1,0);
   curblock->setBlockType(0);

   cur_status = 0;

   G.level = "DRILLSYMBOL";
   G.lyrnr = getLegacyGraph().graphLevel(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
  Drawing_cells are non electrical Parts.
*/
static int cel_drawing_cell()
{
   int   id = cur_ident;
   char  *lp;

   //here update last curpad struct
   overwrite_pad();

   lastpininsert = NULL;
   if ((lp = strword()) != NULL)
      G.name = lp;
   else
      G.name = "";

   curblock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,G.name,-1,0);
   G.setAttributesPtr(&curblock->getAttributesRef());

   go_command(celpart_lst,SIZ_CELPART_LST,id);

   getLegacyGraph().graphBlockOff();

   if (explode_drawing_cell)
   {
      // delete this definition, so that the definition is used from LAYOUT.HKP
      //doc->RemoveBlock(curblock);
      delete curblock;
      curblock = NULL;
   }

   lastpininsert = NULL;
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int cel_filetype()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
 
   if (STRCMPI(lp,"CELL_LIBRARY"))
   {
      // NOT the right file type
      CString  tmp;

      tmp.Format("Filetype [%s] found - Filetype [CELL_LIBRARY] expected");

      ErrorMessage(tmp, "Wrong Filetype in CELL.HKP");
      return -2;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int des_filetype()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
 
   if (STRCMPI(lp,"LAYOUT"))
   {
      // NOT the right file type
      CString  tmp;

      tmp.Format("Filetype [%s] found - Filetype [LAYOUT] expected");

      ErrorMessage(tmp, "Wrong Filetype in LAYOUT.HKP");
      return -2;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int netprops_filetype()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
 
   if (STRCMPI(lp,"NETPROPERTIES_FILE"))
   {
      // NOT the right file type
      CString  tmp;

      tmp.Format("Filetype [%s] found - Filetype [NETPROPERTIES_FILE] expected");

      ErrorMessage(tmp, "Wrong Filetype in NETPROPS.HKP");
      return -2;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pdb_filetype()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
 
   if (STRCMPI(lp,"ASCII_PDB"))
   {
      // NOT the right file type
      CString  tmp;

      tmp.Format("Filetype [%s] found - Filetype [ASCII_PDB] expected");

      ErrorMessage(tmp, "Wrong Filetype in PDB.HKP");
      return -2;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int jobprefs_filetype()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
 
   if (STRCMPI(lp,"JOB_PREFERENCES"))
   {
      // NOT the right file type
      CString  tmp;

      tmp.Format("Filetype [%s] found - Filetype [JOB_PREFERENCES] expected");

      ErrorMessage(tmp, "Wrong Filetype in JOBPREFS.HKP");
      return -2;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pad_filetype()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
 
   if (STRCMPI(lp,"PADSTACK_LIBRARY"))
   {
      // NOT the right file type
      CString  tmp;

      tmp.Format("Filetype [%s] found - Filetype [PADSTACK_LIBRARY] expected");

      ErrorMessage(tmp, "Wrong Filetype in PADSTACK.HKP");
      return -2;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cel_version()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   cur_version = lp;

 /*
   if (!STRCMPI(lp,"VB99"))   return 1;

   int res = sscanf(lp,"%d.%d.%d",&v1, &v2, &v3);
   
   if (res != 3)
   {
      fprintf(ferr,"CELL FILE VERSION Scan error at %ld\n",getInputFileLineNumber());
      display_error++;
   }

   if (v1 != 1)
   {
      CString tmp;
      tmp.Format("Cell File Version %d not supported!\n\
Unsupported Revision may cause software problems.\n",v1);
      MessageBox(NULL, tmp,"VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
static int des_version()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   cur_version = lp;

/*
   int res = sscanf(lp,"%d.%d.%d",&v1, &v2, &v3);
   
   if (res != 3)
   {
      fprintf(ferr,"LAYOUT FILE VERSION Scan error at %ld\n",getInputFileLineNumber());
      display_error++;
   }

   if (v1 != 1)
   {
      CString tmp;
      tmp.Format("Layout File Version %d not supported!\n\
Unsupported Revision may cause software problems.\n",v1);
      MessageBox(NULL, tmp,"VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
static int netprops_version()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   cur_version = lp;

/*
   int res = sscanf(lp,"%d.%d.%d",&v1, &v2, &v3);
   
   if (res != 3)
   {
      fprintf(ferr,"LAYOUT FILE VERSION Scan error at %ld\n",getInputFileLineNumber());
      display_error++;
   }

   if (v1 != 1)
   {
      CString tmp;
      tmp.Format("Layout File Version %d not supported!\n\
Unsupported Revision may cause software problems.\n",v1);
      MessageBox(NULL, tmp,"VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
static int pdb_version()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   cur_version = lp;

/*
   int res = sscanf(lp,"%d.%d.%d",&v1, &v2, &v3);
   
   if (res != 3)
   {
      fprintf(ferr,"LAYOUT FILE VERSION Scan error at %ld\n",getInputFileLineNumber());
      display_error++;
   }

   if (v1 != 1)
   {
      CString tmp;
      tmp.Format("Layout File Version %d not supported!\n\
Unsupported Revision may cause software problems.\n",v1);
      MessageBox(NULL, tmp,"VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
static int jobprefs_version()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   cur_version = lp;
/*
   if (!STRCMPI(lp,"VB99"))   return 1;

   int res = sscanf(lp,"%d.%d.%d",&v1, &v2, &v3);
   
   if (res != 3)
   {
      fprintf(ferr,"JOBPREFS FILE VERSION Scan error at %ld\n",getInputFileLineNumber());
      display_error++;
   }

   if (v1 != 1)
   {
      CString tmp;
      tmp.Format("Jobprefs File Version %d not supported!\n\
Unsupported Revision may cause software problems.\n",v1);
      MessageBox(NULL, tmp,"VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
static int pad_version()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   cur_version = lp;
/* 
   if (!STRCMPI(lp,"VB99"))   return 1;

   int res = sscanf(lp,"%d.%d.%d",&v1, &v2, &v3);
   
   if (res != 3)
   {
      fprintf(ferr,"PADSATCK LIBRARY VERSION Scan error at %ld\n",getInputFileLineNumber());
      display_error++;
   }

   if (v1 != 1)
   {
      CString tmp;
      tmp.Format("Padstack Library Version %d not supported!\n\
Unsupported Revision may cause software problems.\n",v1);
      MessageBox(NULL, tmp,"VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
static int cel_package_cell()
{
   int   id = cur_ident;
   char  *lp;

   //here update last curpad struct
   overwrite_pad();

   lastpininsert = NULL;
   lp = strword();
   G.name = lp;

   G.pincnt = 0;

   // Must make it upper so it won't be case sensitive. So it can be found later in des_part()
   CString geomname = G.name; 
   geomname.MakeUpper();

   curblock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, geomname, -1, 0);

   if (is_attrib(curblock->getName(), TEST_ATTR) != 0)
   {
      curblock->setBlockType(blockTypeTestPoint);
   }
   else
   {
      curblock->setBlockType(blockTypePcbComponent);
   }

   G.setAttributesPtr(&curblock->getAttributesRef());

   // used to track all pin names to make sure there are no duplicates
   // if duplicates exist, rename the pin to "<PinName> [i]" where "i" is the pin count + 1
   // used in celpart_pin
   G.m_duplicatePinMapCount.RemoveAll();

   // Add geom to VBGeomList
   VBGeomList.AddGeom(geomname, curblock->getBlockNumber());

   go_command(celpart_lst, SIZ_CELPART_LST, id);

   if (G.packageGroup.CompareNoCase("TEST_POINT") == 0)
   {
      curblock->setBlockType(blockTypeTestPoint);
   }

   G.m_duplicatePinMapCount.RemoveAll();
   //here update last curpad struct
   overwrite_pad();

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
static int cel_mechanical_cell()
{
   int   id = cur_ident;
   char  *lp;

   //here update last curpad struct
   overwrite_pad();

   lastpininsert = NULL;
   lp = strword();
   G.name = lp;

   G.pincnt = 0;

   curblock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, G.name,-1,0);
   curblock->setBlockType(blockTypeMechComponent);
   G.setAttributesPtr(&curblock->getAttributesRef());

   go_command(celpart_lst,SIZ_CELPART_LST,id);

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
static int des_part()
{
   int id = cur_ident;
   CString geomname;
   char *lp;

   lp = strword();
   geomname = lp;

   // Must make it upper so it won't be case sensitive in order to match teh geometry created in cel_package_cell()
   geomname.MakeUpper();

   G.name_1 = ""; // part number
   G.comp_mirror = 0;   // mirror
   G.sidebot = 0;
   G.comp_rotation = 0;
   G.fanout_trace = 0;
   polycnt = 0;
   G.pincnt = 0;
   derived = FALSE;

   // Reset the map
   pinToUniqueNetZeroArray.RemoveAll();
   pinToUniqueNetZeroCnt = 0;

   // make sure 
   BlockStruct *block = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,geomname,-1,0);
   getLegacyGraph().graphBlockOff();

   DataStruct *d = getLegacyGraph().graphBlockReference(geomname, NULL, 0, 0.0,0.0, 0.0,0 , 1.0,getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);

   //if (d->getEntityNumber() == 8104)
   //{
   //   int iii = 3;
   //}

   G.setAttributesPtr(&d->getAttributesRef());

   if (block->getBlockType() == blockTypeTestPoint)
   {   
      d->getInsert()->setInsertType(insertTypeTestPoint);
      //doc->SetUnknownAttrib(&d->getAttributesRef(),ATT_TEST,"",SA_OVERWRITE, NULL);
      d->attributes().setAttribute(getCamCadData(),standardAttributeName,"");
   }
   else if (block->getBlockType() == blockTypePcbComponent)
   {
      d->getInsert()->setInsertType(insertTypePcbComponent);
   } 
   else
   {
      // bad mojo here...
     d->getInsert()->setInsertType(insertTypeUnknown);
   }
   
   curVBGeom = new CDcaVBGeom(geomname, block->getBlockNumber());

   go_command(part_instance_lst,SIZ_PART_INSTANCE_LST,id);

   CDcaVBGeom* vbGeom = VBGeomList.FindGeom(geomname);

   if (vbGeom != NULL && curVBGeom != NULL)
   {
      if (vbGeom->IsDifference(curVBGeom))
      {
         CDcaVBGeom* derivedVBGeom = vbGeom->CheckForDerivedGeom(curVBGeom);
         if (derivedVBGeom == NULL)
         {
            CString newGeomName = vbGeom->GetDerivedGeomName();
            BlockStruct* newBlock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, newGeomName, -1, 0);
            newBlock->setBlockType(block->getBlockType());
            getLegacyGraph().graphBlockCopy(block, 0.0, 0.0, 0.0, 0, 1, -1, TRUE);
            getLegacyGraph().graphBlockOff();

            POSITION pos = newBlock->getHeadDataPosition();
            while (pos != NULL)
            {
               DataStruct* data = newBlock->getNextData(pos);

               if (data == NULL)
                  continue;

               if (data->getDataType() == dataTypePoly)
               {
                  LayerStruct* layer = getCamCadData().getLayer(data->getLayerIndex());

                  if (layer == NULL)
                     continue;

                  if (curVBGeom->GetSoldermaskSide() == sideOptionOppSide || curVBGeom->GetSilkscreenSide() == sideOptionOppSide)
                  {
                     if (layer->getName().CompareNoCase("SILKSCREEN_TOP") == 0 )
                     {
                        if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() != layer->getLayerIndex())
                           data->setLayerIndex(layer->getMirroredLayerIndex());
                        else
                        {
                           data->setLayerIndex(getLegacyGraph().graphLevel("SILKSCREEN_BOTTOM", "", FALSE));
                        }
                     }
                     else if (layer->getName().CompareNoCase("SILKSCREEN_BOTTOM") == 0)
                     {
                        if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() != layer->getLayerIndex())
                           data->setLayerIndex(layer->getMirroredLayerIndex());
                        else
                        {
                           data->setLayerIndex(getLegacyGraph().graphLevel("SILKSCREEN_TOP", "", FALSE));
                        }
                     }
                     else if (layer->getName().CompareNoCase(PAD_TOP_SOLDERMASK) == 0)
                     {
                        if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() != layer->getLayerIndex())
                           data->setLayerIndex(layer->getMirroredLayerIndex());
                        else
                        {
                           data->setLayerIndex(graphLayer(PAD_BOT_SOLDERMASK, "", FALSE));
                        }
                     }
                     else if (layer->getName().CompareNoCase(PAD_BOT_SOLDERMASK) == 0)
                     {
                        if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() != layer->getLayerIndex())
                           data->setLayerIndex(layer->getMirroredLayerIndex());
                        else
                        {
                           data->setLayerIndex(graphLayer(PAD_TOP_SOLDERMASK, "", FALSE));
                        }
                     }
                  }
               }
               else if (data->getDataType() == dataTypeInsert)
               {
                  InsertStruct* pinInsert = data->getInsert();
                  if (pinInsert->getInsertType() != insertTypePin)
                     continue;

                  CDcaVBPin* vbPin = curVBGeom->FindPin(pinInsert->getRefname());
                  if (vbPin != NULL)
                  {
                     if (vbPin->GetPinOption() == sideOptionOppSide)
                     {
                        pinInsert->setMirrorFlags(MIRROR_FLIP | MIRROR_LAYERS);
                        pinInsert->setPlacedBottom(true);
                     }
                     else if (vbPin->GetPinOption() == sideOptionNone)
                     {
                        pinInsert->setMirrorFlags(0);
                        pinInsert->setPlacedBottom(false);
                     }
                  }
               }
            }

            //POSITION pos = newBlock->getHeadDataInsertPosition();
            //while (pos != NULL)
            //{
            // DataStruct* pinData = newBlock->getNextDataInsert(pos);
            // if (pinData == NULL || pinData->getInsert() == NULL)
            //    continue;

            // InsertStruct* pinInsert = pinData->getInsert();
            // if (pinInsert->getInsertType() != insertTypePin)
            //    continue;

            // CDcaVBPin* vbPin = vbGeom->FindPin(pinInsert->getRefname());
            // if (vbPin != NULL)
            // {
            //    if (sideOptionOppSide)
            //    {
            //       pinInsert->setMirrorFlags(MIRROR_FLIP | MIRROR_LAYERS);
            //       pinInsert->setPlacedBottom(true);
            //    }
            //    else if (sideOptionNone)
            //    {
            //       pinInsert->setMirrorFlags(0);
            //       pinInsert->setPlacedBottom(false);
            //    }
            // }
            //}

            block = newBlock;
            curVBGeom->SetName(newGeomName);
            curVBGeom->SetGeomNum(newBlock->getBlockNumber());
            vbGeom->AddDerivedGeom(curVBGeom);
         }
         else
         {
            block = getCamCadData().getBlock(derivedVBGeom->GetGeomNum());
         }

         d->getInsert()->setBlockNumber(block->getBlockNumber());
      }

      delete curVBGeom;
      curVBGeom = NULL;
   }


   G.clearAttributesPtr();
   cur_compcnt++;
   int odd_smd = 0;

   d->getInsert()->setRefname(STRDUP(G.ref_des));
   d->getInsert()->setOriginX(G.comp_x);
   d->getInsert()->setOriginY(G.comp_y);
   d->getInsert()->setAngle(degreesToRadians(G.comp_rotation));

   //if (d->getInsert()->getRefname().CompareNoCase("CL9017") == 0)
   //{
   //   int iii = 3;
   //}

   if (G.sidebot)
   {
      d->getInsert()->setPlacedBottom(true);
   }

   if (G.comp_mirror)
   {
      G.comp_rotation = G.comp_rotation - 180;
      while (G.comp_rotation < 0)
         G.comp_rotation += 360;
      while (G.comp_rotation >= 360)
         G.comp_rotation -= 360;

      d->getInsert()->setAngle(degreesToRadians(G.comp_rotation));
      d->getInsert()->setMirrorFlagBits(MIRROR_FLIP | MIRROR_LAYERS);
      d->getInsert()->setPlacedBottom(true);
   }

   if (G.sidebot && !G.comp_mirror)
   {
      //fprintf(ferr, "Component [%s] placed on BOTTOM layer and not mirrored\n",G.name);
      //display_error++;
      odd_smd = TRUE;
   }

   if (strlen(G.name_1))
   {
      CString typeName = G.name_1;

      int n = 0;
      TypeStruct* existingType = fl->getTypeList().findType(typeName);

      while (existingType != NULL)
      {
         if (existingType->getBlockNumber() == -1 || existingType->getBlockNumber() == block->getBlockNumber()) // use this one
         {
            existingType->setBlockNumber(block->getBlockNumber());
            break;
         }

         typeName.Format("%s_%d", G.name_1, ++n);
         existingType = fl->getTypeList().findType(typeName);
      }

      //TypeStruct* type = AddType(fl, typeName);
      TypeStruct* type = fl->getTypeList().addType(typeName);
      type->setBlockNumber( block->getBlockNumber());

      if (typeName.CompareNoCase(G.name_1)) // if we changed the name
      {
         //int orignialTypeKey = doc->RegisterKeyWord("ORIGINAL_DEVICE", 0, VT_STRING);
         //doc->SetAttrib(&type->getAttributesRef(), orignialTypeKey, VT_STRING, G.name_1.GetBuffer(0), SA_OVERWRITE, NULL);

         int originalDeviceKeywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex("ORIGINAL_DEVICE",valueTypeString);
         type->attributes().setAttribute(getCamCadData(),originalDeviceKeywordIndex,G.name_1);

         fprintf(ferr, "Component [%s] used Device Name [%s].  Changed to Device Name [%s] because of duplication.\n", G.ref_des, G.name_1, typeName);
         display_error++;
      }         

      //int keyword = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);
      //doc->SetAttrib(&d->getAttributesRef(), keyword, VT_STRING, typeName.GetBuffer(0), SA_OVERWRITE, NULL);
      d->attributes().setAttribute(getCamCadData(),standardAttributeTypeListLink,typeName);
   }

   if (odd_smd)
   {
      //RemoveAttrib(doc->IsKeyWord(ATT_SMDSHAPE, TRUE), &d->getAttributesRef());

      int smdInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeSmd),keyWordSectionIn);
      d->attributes().removeAttribute(smdInKeywordIndex);

      //doc->SetAttrib(&d->getAttributesRef(),doc->RegisterKeyWord("SMD_BOTTOM", 0, VT_NONE), VT_NONE, NULL, SA_OVERWRITE, NULL);
      int smdBottomKeywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex("SMD_BOTTOM",valueTypeNone);
      d->attributes().setAttribute(getCamCadData(),smdBottomKeywordIndex);
   }

   if (G.fanout_trace)
   {
      fprintf(ferr, "Component [%s] has fanout traces!\n", G.ref_des);
      display_error++;
   }

   ReassignCompPinNamesIfDuplicate(block);

   // Assign comp pin to unique net zero
   AssignCompPinToUniqueNetZero(d->getInsert()->getRefname());

   if (layout_netlist)
      AssignLayoutPinNetAssignments(d->getInsert()->getRefname());

   // Free the map
   pinToUniqueNetZeroArray.RemoveAll();
   pinToUniqueNetZeroCnt = 0;

   G.ref_des = "";      // refdes
   polycnt = 0;
   return 1;
}

/******************************************************************************
* ReassignCompPinNamesIfDuplicate
*/
static void ReassignCompPinNamesIfDuplicate(BlockStruct *compBlock)
{
   CTMatrix mat;
   mat.translate(-G.comp_x, -G.comp_y);
   mat.rotateDegrees(-G.comp_rotation);

   // recalculate all the pin locations so that they are relative to the component's origin
   for (int i=0; i<pinToUniqueNetZeroCnt; i++)
   {
      VBPinNet pinNet = pinToUniqueNetZeroArray[i];
      mat.transform(pinNet.m_pinLoc);
      if (G.comp_mirror)
         pinNet.m_pinLoc.x = -pinNet.m_pinLoc.x;
      pinToUniqueNetZeroArray.SetAtGrow(i, pinNet);
   }

   // make sure the pin names match up based on the locations of the pins so the nets are correctly assigned
   POSITION pos = compBlock->getHeadDataInsertPosition();
   while (pos)
   {
      DataStruct *pinData = compBlock->getNextDataInsert(pos);

      if (pinData->getDataType() != dataTypeInsert)
         continue;

      InsertStruct *pinInsert = pinData->getInsert();
      bool pinMirror = pinInsert->getGraphicMirrored();

      if (pinInsert->getInsertType() != insertTypePin)
         continue;

      for (int i=0; i<pinToUniqueNetZeroCnt; i++)
      {
         VBPinNet pinNet = pinToUniqueNetZeroArray[i];
         bool compPinMirror = (pinNet.pinOption.Compare("OPP_SIDE") == 0);

         // check if the location is the same
         if (pinInsert->getOrigin2d().fpeq(pinNet.m_pinLoc) && pinMirror == compPinMirror)
         {
            pinNet.pinName = pinInsert->getRefname();
            pinToUniqueNetZeroArray.SetAtGrow(i, pinNet);
            break;
         }
      }
   }

}

/******************************************************************************
* AssignCompPinToUniqueNetZero
*/
static void AssignCompPinToUniqueNetZero(CString compName)
{
   // Skip the assignment if USE_UNIQUE_NETZERO is FALSE
   if (!USE_UNIQUE_NETZERO)
      return;

   for (int i=0; i<pinToUniqueNetZeroCnt; i++)
   {
      VBPinNet pinNet = pinToUniqueNetZeroArray[i];

      // Only add the comp pin to netname that starts with "(NET0)-"
      if (pinNet.netName.Left((int)strlen("(NET0)-")).MakeUpper() == "(NET0)-")
      {
         //NetStruct *net = add_net(fl, pinNet.netName);
         //add_comppin(fl, net, compName, pinNet.pinName);

         NetStruct* net = fl->getNetList().addNet(pinNet.netName);
         net->addCompPin(compName,pinNet.pinName);
      }
   }
}

/******************************************************************************
* AssignLayoutPinNetAssignments
*/
static void AssignLayoutPinNetAssignments(CString compName)
{
   // Skip the assignment if USE_UNIQUE_NETZERO is FALSE
   for (int i=0; i<pinToUniqueNetZeroCnt; i++)
   {
      VBPinNet pinNet = pinToUniqueNetZeroArray[i];

      // Only add the comp pin to netname that starts with "(NET0)-"
      if (pinNet.netName.Left((int)strlen("(NET0)")).MakeUpper() != "(NET0)")
      {
         //NetStruct *net = add_net(fl, pinNet.netName);
         //add_comppin(fl, net, compName, pinNet.pinName);

         NetStruct* net = fl->getNetList().addNet(pinNet.netName);
         net->addCompPin(compName,pinNet.pinName);
      }
   }
}

/******************************************************************************
* part_pin
*/
static int part_pin()
{
   char *lp = strword();
   CString pinName = lp;

   // Here set the netname to "(Net0)-" temperary until it get the real net name
   VBPinNet pinNet;
   pinNet.pinName = pinName;
   pinNet.netName = "";
   pinNet.m_pinLoc.x = 0.0;
   pinNet.m_pinLoc.y = 0.0;
   pinNet.pinOption = "";
   pinToUniqueNetZeroArray.SetAtGrow(pinToUniqueNetZeroCnt++, pinNet);

   int id = cur_ident;
   go_command(part_pin_lst, SIZ_PART_PIN_LST, id);

   if (curVBGeom != NULL)
   {
      pinNet = pinToUniqueNetZeroArray[pinToUniqueNetZeroCnt-1];
      curVBGeom->AddPin(pinNet.pinName, pinNet.pinOption);
   }

   return 1;
}

/******************************************************************************
* part_silkscreen_outline
*/
static int part_silkscreen_outline()
{
   int id = cur_ident;
   int oldstat = cur_status;
   cur_status = FILE_SILK_ARTWORK;
   go_command(part_graph_lst,SIZ_PART_GRAPH_LST,id);

   cur_status = oldstat;
   return 1;
}

/******************************************************************************
* part_solder_mask
*/
static int part_solder_mask()
{
   int id = cur_ident;
   int oldstat = cur_status;
   cur_status = FILE_SOLDER_MASK;
   go_command(part_graph_lst,SIZ_PART_GRAPH_LST,id);

   cur_status = oldstat;
   return 1;
}

/******************************************************************************
* part_pin_option
*/
static int part_pin_option()
{
   char *lp = strword();
   CString pinOption = lp;

   VBPinNet pinNet = pinToUniqueNetZeroArray[pinToUniqueNetZeroCnt-1];
   pinNet.pinOption = pinOption;
   pinToUniqueNetZeroArray.SetAt(pinToUniqueNetZeroCnt-1, pinNet);

   return 1;
}

/******************************************************************************
* part_pin_net
*/
static int part_pin_net()
{
   char *lp = strword();
   CString netName = lp;

   VBPinNet pinNet = pinToUniqueNetZeroArray[pinToUniqueNetZeroCnt-1];
   pinNet.netName = netName;
   pinToUniqueNetZeroArray.SetAt(pinToUniqueNetZeroCnt-1, pinNet);

   return 1;
}

/******************************************************************************
* part_pin_xy
*/
static int part_pin_xy()
{
   polycnt = 0;
   vb99_xy();

   if (polycnt)
   {
      VBPinNet pinNet = pinToUniqueNetZeroArray[pinToUniqueNetZeroCnt-1];
      pinNet.m_pinLoc.x = poly_l[0].x;
      pinNet.m_pinLoc.y = poly_l[0].y;
      pinToUniqueNetZeroArray.SetAt(pinToUniqueNetZeroCnt-1, pinNet);
   }
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_mechanical()
{
   int      id = cur_ident;
   CString  geomname;
   char     *lp;

   lp = strword();
   geomname = lp;

   //BlockStruct *b = Graph_Block_Exists(doc, geomname, -1);
   BlockStruct* b = getLegacyGraph().graphBlockExists(geomname,-1);

   G.name = ""; // refdes
   G.name_1 = "";
   G.comp_mirror = 0; // top
   G.sidebot = 0;
   G.comp_rotation = 0;
   polycnt = 0;

   // make sure 
   DataStruct *d;
   if (!b)
   {
      getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,geomname,-1,0);
      getLegacyGraph().graphBlockOff();
      d = getLegacyGraph().graphBlockReference(geomname, NULL, 0,0.0,0.0, 0.0,0 , 1.0,getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
      G.setAttributesPtr(&d->getAttributesRef());

      getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,geomname,-1,0);
      go_command(part_def_instance_lst,SIZ_PART_DEF_INSTANCE_LST,id);
      getLegacyGraph().graphBlockOff();
      G.clearAttributesPtr();
      G.comp_x = 0;
      G.comp_y = 0;
      G.comp_rotation = 0;
   }
   else
   {
      d = getLegacyGraph().graphBlockReference(geomname, NULL, 0,0.0,0.0, 0.0,0 , 1.0,getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
      G.setAttributesPtr(&d->getAttributesRef());

      go_command(part_instance_lst,SIZ_PART_INSTANCE_LST,id);
      G.clearAttributesPtr();
   }

   d->getInsert()->setRefname(STRDUP(G.ref_des));
   d->getInsert()->setOriginX(G.comp_x);
   d->getInsert()->setOriginY(G.comp_y);
   d->getInsert()->setAngle(degreesToRadians(G.comp_rotation));
   

   if (G.sidebot)
   {
      d->getInsert()->setPlacedBottom(true);
   }

   if (G.comp_mirror)
   {
      G.comp_rotation = G.comp_rotation - 180;
      while (G.comp_rotation < 0)   G.comp_rotation += 360;
      while (G.comp_rotation >= 360)   G.comp_rotation -= 360;

      d->getInsert()->setAngle(degreesToRadians(G.comp_rotation));
      d->getInsert()->setMirrorFlagBits(MIRROR_FLIP | MIRROR_LAYERS);
   }

   polycnt = 0;

   // make sure that Mechanical does not have a ATT_TYPELIST 
   //CAttribute *a;
   int typeListLineInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeTypeListLink),keyWordSectionIn);
   d->attributes().deleteAttribute(typeListLineInKeywordIndex);

   //if (a =  is_attvalue(doc, d->getAttributesRef(), ATT_TYPELISTLINK, 1))
   //{
   //   int keyindex = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);
   //   RemoveAttrib(keyindex, &d->getAttributesRef());
   //}

   return 1;
}

/****************************************************************************/
/*
*/
static int des_cell()
{
   int      id = cur_ident;
   CString  geomname;
   char     *lp;

   lp = strword();
   geomname = lp;

   //BlockStruct *b = Graph_Block_Exists(doc, geomname, -1);
   BlockStruct* b = getLegacyGraph().graphBlockExists(geomname,-1);

   G.name = ""; // refdes
   G.name_1 = "";
   G.comp_mirror = 0; // top
   G.sidebot = 0;
   G.comp_rotation = 0;
   polycnt = 0;

   // make sure 
   DataStruct *d;
   if (!b)
   {
      getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,geomname,-1,0);
      getLegacyGraph().graphBlockOff();
      d = getLegacyGraph().graphBlockReference(geomname, NULL, 0,0.0,0.0, 0.0,0 , 1.0,getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
      G.setAttributesPtr(&d->getAttributesRef());

      getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,geomname,-1,0);
      go_command(part_def_instance_lst,SIZ_PART_DEF_INSTANCE_LST,id);
      getLegacyGraph().graphBlockOff();
      G.clearAttributesPtr();
      G.comp_x = 0;
      G.comp_y = 0;
      G.comp_rotation = 0;
   }
   else
   {
      d = getLegacyGraph().graphBlockReference(geomname, NULL, 0,0.0,0.0, 0.0,0 , 1.0,getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
      G.setAttributesPtr(&d->getAttributesRef());

      go_command(part_instance_lst,SIZ_PART_INSTANCE_LST,id);
      G.clearAttributesPtr();
   }

   d->getInsert()->setRefname(STRDUP(G.ref_des));
   d->getInsert()->setOriginX(G.comp_x);
   d->getInsert()->setOriginY(G.comp_y);
   d->getInsert()->setAngle(degreesToRadians(G.comp_rotation));
   

   if (G.sidebot)
   {
      d->getInsert()->setPlacedBottom(true);
   }
   if (G.comp_mirror)
   {
      G.comp_rotation = G.comp_rotation - 180;
      while (G.comp_rotation < 0)   G.comp_rotation += 360;
      while (G.comp_rotation >= 360)   G.comp_rotation -= 360;

      d->getInsert()->setAngle(degreesToRadians(G.comp_rotation));
      d->getInsert()->setMirrorFlagBits(MIRROR_FLIP | MIRROR_LAYERS);
   }
   polycnt = 0;

   // make sure that Mechanical does not have a ATT_TYPELIST 
   //CAttribute *a;
   int typeListLineInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeTypeListLink),keyWordSectionIn);
   d->attributes().deleteAttribute(typeListLineInKeywordIndex);

   //if (a =  is_attvalue(doc, d->getAttributesRef(), ATT_TYPELISTLINK, 1))
   //{
   //   int keyindex = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);
   //   RemoveAttrib(keyindex, &d->getAttributesRef());
   //}

   return 1;
}

/****************************************************************************/
/*
*/
static int via_instance_netname()
{
   if (USE_CELL_FANOUT_VIAS == false)
      vb99_name();
   else
      vb99_skip();

   return 1;
}
/****************************************************************************/
/*
*/
static int part_instance_via()
{
   if (USE_CELL_FANOUT_VIAS == false)
      net_via();
   else
      vb99_skip();

   return 1;
}
/****************************************************************************/
/*
*/
static int net_via()
{
   int id = cur_ident;
   
   polycnt = 0;
   viaFromLayerName.Empty();
   viaToLayerName.Empty();

   go_command(via_instance_lst,SIZ_VIA_INSTANCE_LST,id);

   // here write via
   if (polycnt)
   {
      // make sure via exist
      CString viaBlockName;
      if (viaFromLayerName.IsEmpty() || viaToLayerName.IsEmpty())
      {
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, G.symbol, -1, 0);
         getLegacyGraph().graphBlockOff();
         viaBlockName = G.symbol;
      }
      else
      {
         BlockStruct* viaBlock = getLayerSpecificViaBlock(viaFromLayerName, viaToLayerName);

         if (viaBlock != NULL)
         {
            viaBlockName = viaBlock->getName();
         }
      }

      if (!viaBlockName.IsEmpty())
      {
         DataStruct* data = getLegacyGraph().graphBlockReference(viaBlockName, NULL, -1,poly_l[0].x, poly_l[0].y, 0.0, 0, 1.0, getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
         data->getInsert()->setInsertType(insertTypeVia);

         if (strlen(G.name))
         {
            //add_net(fl,G.name);  // just incase no netlist is loaded
            //doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),VT_STRING, G.name.GetBuffer(0), SA_APPEND, NULL);

            NetStruct* net = fl->getNetList().addNet(G.name);
            net->attributes().setAttribute(getCamCadData(),standardAttributeNetName,G.name);
         }
      }
   }

   polycnt = 0;
   viaFromLayerName.Empty();
   viaToLayerName.Empty();

   return 1;
}

/******************************************************************************
* getLayerSpecificViaBlock
*/
BlockStruct* getLayerSpecificViaBlock(const CString& fromLayerName,const CString& toLayerName)
{
   //BlockStruct* viaBlock = doc->Find_Block_by_Name(G.symbol, -1, blockTypePadstack);
   BlockStruct* viaBlock = getCamCadData().getBlockDirectory().getAt(G.symbol,-1,blockTypePadstack);

   if (viaBlock == NULL)
      return NULL;

   CString newBlockName;
   newBlockName.Format("%s_%s_%s", viaBlock->getName(), fromLayerName, toLayerName);
   //BlockStruct* newViaBlock = Graph_Block_Exists(doc, newBlockName, -1, blockTypePadstack);
   BlockStruct* newViaBlock = getLegacyGraph().graphBlockExists(newBlockName,-1,blockTypePadstack);

   if (newViaBlock != NULL)
      return newViaBlock;

   // Create block from the original block
   //LayerStruct* fromLayer = doc->FindLayer_by_Name(fromLayerName);
   //LayerStruct* toLayer = doc->FindLayer_by_Name(toLayerName);
   LayerStruct* fromLayer = getCamCadData().getLayerArray().getLayer(fromLayerName);
   LayerStruct* toLayer   = getCamCadData().getLayerArray().getLayer(toLayerName);

   // The layer name of the top and bottom layer.  "Lyr_" is a reserved prefix for VB Ascii layer name.
   CString startLayer = "Lyr_1";
   CString endLayer;
   endLayer.Format("Lyr_%d", NumberOfLayers);

   if (fromLayerName.CompareNoCase(startLayer) == 0 && toLayerName.CompareNoCase(endLayer) == 0)
   {
      // Check to see if via span from top to bottom, if it is then use original block
      newViaBlock = viaBlock;
   }
   else
   {
      InsertStruct* topPad = NULL;
      InsertStruct* innerPad = NULL;
      InsertStruct* bottomPad = NULL;
      InsertStruct* drill = NULL;
      DataStruct* topSolder = NULL;
      DataStruct* topPaste = NULL;
      DataStruct* bottomSolder = NULL; 
      DataStruct* bottomPaste = NULL;  

      // Find the top, inner and bottom pads
      for (CDataListIterator datalist(*viaBlock, dataTypeInsert); datalist.hasNext();)
      {
         DataStruct* data = datalist.getNext();
         if (data == NULL)
            continue;

         LayerStruct* layer = getCamCadData().getLayer(data->getLayerIndex());

         if (layer == NULL)
            continue;

         CString layerName = layer->getName();

         if (topPad == NULL && (layerName.CompareNoCase(PAD_TOP) == 0 || layerName.CompareNoCase(SMD_TOP) == 0))
         {
            topPad = data->getInsert();
         }
         else if (innerPad == NULL && layerName.CompareNoCase(PAD_INT) == 0)
         {
            innerPad = data->getInsert();
         }
         else if (bottomPad == NULL && (layerName.CompareNoCase(PAD_BOT) == 0 || layerName.CompareNoCase(SMD_BOT) == 0))
         {
            bottomPad = data->getInsert();
         }
         else if (topSolder == NULL && (layerName.CompareNoCase(PAD_TOP_SOLDERMASK) == 0 || layerName.CompareNoCase(SMDMASK_TOP) == 0))
         {
            topSolder = data;
         }
         else if (bottomSolder == NULL && (layerName.CompareNoCase(PAD_BOT_SOLDERMASK) == 0 || layerName.CompareNoCase(SMDMASK_BOT) == 0))
         {
            bottomSolder = data;
         }
         else if (topPaste == NULL && (layerName.CompareNoCase(PAD_TOP_SOLDERPASTE) == 0 || layerName.CompareNoCase(SMDPASTE_TOP) == 0))
         {
            topPaste = data;
         }
         else if (bottomPaste == NULL && (layerName.CompareNoCase(PAD_BOT_SOLDERPASTE) == 0 || layerName.CompareNoCase(SMDPASTE_BOT) == 0))
         {
            bottomPaste = data;
         }
         else if (drill == NULL && layerName.CompareNoCase("DRILLHOLE") == 0)
         {
            drill = data->getInsert();
         }
      }

      newViaBlock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, newBlockName, -1, blockTypePadstack);
      newViaBlock->setBlockType(blockTypePadstack);
      //doc->CopyAttribs(&newViaBlock->getAttributesRef(), viaBlock->getAttributesRef());
      newViaBlock->attributes().CopyAll(viaBlock->attributes());

      // Insert drill onto drill layer
      if (drill != NULL)
      {
         int layerIndex = graphLayer("DRILLHOLE", "", FALSE);

         BlockStruct* drillBlock = getCamCadData().getBlock(drill->getBlockNumber());

         getLegacyGraph().graphBlockReference(drillBlock->getName(), "", drillBlock->getFileNumber(), drill->getOriginX(), drill->getOriginY(),
               drill->getAngle(), drill->getMirrorFlags() != 0, drill->getScale(), layerIndex, 
               (drillBlock->getFlags() & BL_GLOBAL) != 0, drillBlock->getBlockType());
      }

      // Insert soldermask and paste top if "fromLayer" is the starting layer
      if (fromLayerName.CompareNoCase(startLayer) == 0)
      {
         if (topSolder != NULL)
         {
            InsertStruct* insert = topSolder->getInsert();
            BlockStruct* padBlock = getCamCadData().getBlock(insert->getBlockNumber());

            getLegacyGraph().graphBlockReference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags() != 0, insert->getScale(), topSolder->getLayerIndex(),
                  (padBlock->getFlags() & BL_GLOBAL) != 0, padBlock->getBlockType());
         }

         if (topPaste != NULL)
         {
            InsertStruct* insert = topPaste->getInsert();
            BlockStruct* padBlock = getCamCadData().getBlock(insert->getBlockNumber());

            getLegacyGraph().graphBlockReference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags() != 0, insert->getScale(), topPaste->getLayerIndex(),
                  (padBlock->getFlags() & BL_GLOBAL) != 0, padBlock->getBlockType());
         }
      }

      // Insert soldermask and paste bottom if "toLayer" is the end layer
      if (toLayerName.CompareNoCase(endLayer) == 0)
      {
         if (bottomSolder != NULL)
         {
            InsertStruct* insert = bottomSolder->getInsert();
            BlockStruct* padBlock = getCamCadData().getBlock(insert->getBlockNumber());

            getLegacyGraph().graphBlockReference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags() != 0, insert->getScale(), bottomSolder->getLayerIndex(),
                  (padBlock->getFlags() & BL_GLOBAL) != 0, padBlock->getBlockType());
         }

         if (bottomPaste != NULL)
         {
            InsertStruct* insert = bottomPaste->getInsert();
            BlockStruct* padBlock = getCamCadData().getBlock(insert->getBlockNumber());

            getLegacyGraph().graphBlockReference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags() != 0, insert->getScale(), bottomPaste->getLayerIndex(),
                  (padBlock->getFlags() & BL_GLOBAL) != 0, padBlock->getBlockType());
         }
      }

      // Insert top pad onto fromLayer
      if (topPad != NULL && fromLayer != NULL)
      {
         BlockStruct* padBlock = getCamCadData().getBlock(topPad->getBlockNumber());

         getLegacyGraph().graphBlockReference(padBlock->getName(), "", padBlock->getFileNumber(), topPad->getOriginX(), topPad->getOriginY(),
               topPad->getAngle(), topPad->getMirrorFlags() != 0, topPad->getScale(), fromLayer->getLayerIndex(), 
               (padBlock->getFlags() & BL_GLOBAL) != 0, padBlock->getBlockType());
      }

      // Insert inner pad to all layes between fromLayer and toLayer
      if (innerPad != NULL && fromLayer != NULL && toLayer != NULL &&
         fromLayer->getElectricalStackNumber() > 0 && toLayer->getElectricalStackNumber() > 0)
      {
         BlockStruct* padBlock = getCamCadData().getBlock(innerPad->getBlockNumber());

         for (int i=fromLayer->getElectricalStackNumber()+1; i<toLayer->getElectricalStackNumber(); i++)
         {
            CString layerName;
            layerName.Format("LYR_%d", i);
            int layerIndex = graphLayer(layerName, "", FALSE);

            getLegacyGraph().graphBlockReference(padBlock->getName(), "", padBlock->getFileNumber(), innerPad->getOriginX(),
                  innerPad->getOriginY(), innerPad->getAngle(), innerPad->getMirrorFlags() != 0, innerPad->getScale(),
                  layerIndex, (padBlock->getFlags() & BL_GLOBAL) != 0, padBlock->getBlockType());            
         }


         //for (int i=0; i<doc->getMaxLayerIndex(); i++)
         //{
         // LayerStruct* layer = doc->getLayerAt(i);
         // if (layer == NULL)
         //    continue;

         // if (layer->getElectricalStackNumber() > fromLayer->getElectricalStackNumber() &&
         //     layer->getElectricalStackNumber() < toLayer->getElectricalStackNumber())
         // {
         //    Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), innerPad->getOriginX(),
         //          innerPad->getOriginY(), innerPad->getAngle(), innerPad->getMirrorFlags(), innerPad->getScale(),
         //          layer->getLayerIndex(), padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());            
         // }        
         //}
      }

      // Insert bottom pad to toLayer
      if (bottomPad != NULL && toLayer != NULL)
      {
         BlockStruct* padBlock = getCamCadData().getBlock(bottomPad->getBlockNumber());

         getLegacyGraph().graphBlockReference(padBlock->getName(), "", padBlock->getFileNumber(), bottomPad->getOriginX(), bottomPad->getOriginY(),
               bottomPad->getAngle(), bottomPad->getMirrorFlags() != 0, bottomPad->getScale(), toLayer->getLayerIndex(), 
               (padBlock->getFlags() & BL_GLOBAL) != 0, padBlock->getBlockType());
      }

      getLegacyGraph().graphBlockOff();
   }

   return newViaBlock;
}

/****************************************************************************/
/*
*/
static int net_testpoint()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.sidebot = TRUE;
   G.rotation = 0;
   G.clearAttributesPtr();
   G.ref_des = "";   // REF_DES text name

   char* lp = strword();
   CString cellName = lp;

   go_command(testpoint_instance_lst,SIZ_TESTPOINT_INSTANCE_LST,id);

   // here write via
   if (polycnt)
   {
      // G.name = NetName
      // G.symbol = Padstack

      // Make sure the geometry exists
      BlockStruct* cellBlock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,cellName,-1,0);
      getLegacyGraph().graphBlockOff();

      // Get the pin name of the first pin because it should only has one pin
      CString pinName;
      for (CDataListIterator dataList(*cellBlock, insertTypePin); dataList.hasNext();)
      {
         InsertStruct* insert = dataList.getNext()->getInsert();
         pinName = insert->getRefname();
      }

      // If cannot find pin then add pin
      if (pinName.IsEmpty())
      {  
         BlockStruct* padstack =  getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,G.symbol,-1,0);
         getLegacyGraph().graphBlockOff();

         BlockStruct* cellBlock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,cellName,-1,0);
         DataStruct* pinData = getLegacyGraph().graphBlockReference(G.symbol, "1", -1, 0.0, 0.0, 0.0, 0, 1.0, getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
         pinData->getInsert()->setInsertType(insertTypePin);
         getLegacyGraph().graphBlockOff();
      }

      int mirror = FALSE;
      TestAccessTag testaccessindex = testAccessTop;

      if (G.sidebot)
      {
         mirror = TRUE;
         testaccessindex = testAccessBottom;
      }

      DataStruct* testPointData = getLegacyGraph().graphBlockReference(cellName, G.ref_des, -1, poly_l[0].x,poly_l[0].y,
                                     degreesToRadians(G.rotation), mirror != 0, 1.0, getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
      testPointData->getInsert()->setInsertType(insertTypeTestPoint);
      //testPointData->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttributeRefName), valueTypeString, G.ref_des.GetBuffer(0), attributeUpdateOverwrite, NULL);
      //testPointData->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttributeTest), valueTypeString, TestAccessTagToString(testaccessindex), attributeUpdateOverwrite, NULL);
      testPointData->attributes().setAttribute(getCamCadData(),standardAttributeRefName,G.ref_des);
      testPointData->attributes().setAttribute(getCamCadData(),standardAttributeTest,TestAccessTagToString(testaccessindex));

      if (strlen(G.name))
      {
         //NetStruct* net = add_net(fl,G.name);  // just incase no netlist is loaded
         //CompPinStruct* comppin = net->addCompPin(G.ref_des, pinName);

         NetStruct* net = fl->getNetList().addNet(G.name);
         net->addCompPin(G.ref_des,pinName);
      }
   }
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int part_instance_trace()
{
   if (USE_CELL_FANOUT_VIAS == false)
   {
      net_trace();
   }
   else
   {
      G.fanout_trace = TRUE;
      vb99_skip();
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int part_instance_shape_name()
{
   char  *lp;

   lp = strword();
   //G.symbol = lp;
   G.symbol.Format("XPAD_%s", lp);  // - Joanne: When the padstack for test point was created, the padstack name had
                                    // had "XPAD" in front of it.  So we need to add "XPAD" here also.

   return 1;
}

/****************************************************************************/
/*
*/
static int via_instance_layer_pair()
{
   //char  *lp;

   //lp = strword();   // start layer
   //lp = strword();   // end layer

   viaFromLayerName = strtok(NULL,WORD_DELIMETER); //strword();
   viaToLayerName = strtok(NULL,WORD_DELIMETER);   //strword();

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_diameter()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.diameter = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_radius_Rect()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_radius()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_circlediameter()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp)) / 2;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_circleradius()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_startangle()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER); // in degrees
   G.startangle = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_deltaangle()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER); // in degrees
   G.deltaangle = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_height()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.height = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_top()
{
   char  *lp;
  
   lp = strword();
   topPadName = lp;

   return 1;
}

/******************************************************************************
* padstack_holename
*/
static int padstack_holename()
{
   char *tok = strword();
   holeName.Format(".HOLE_%s", tok); // can have same name as padstack

   G.xoff = 0;
   G.yoff = 0;

   int id = cur_ident;
   go_command(pad_holename_lst, SIZ_PAD_HOLENAME_LST, id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_topsoldermask()
{
   char  *lp;
  
   lp = strword();
   topSoldermaskPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_topsolderpaste()
{
   char  *lp;
  
   lp = strword();
   topSolderPastePadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_bot()
{
   char  *lp;

   lp = strword();
   bottomPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_botsoldermask()
{
   char  *lp;

   lp = strword();
   bottomSoldermaskPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_botsolderpaste()
{
   char  *lp;

   lp = strword();
   bottomSolderPastePadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_int()
{
   char  *lp;

   lp = strword();
   innerPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_thermal()
{
   char  *lp;

   lp = strword();
   thermalPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_clearance()
{
   char  *lp;

   lp = strword();
   clearancePadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_shapename()
{
   char  *lp;

   if ((lp = strword()) != NULL)
      G.name = lp;
   else
      G.name = "";

   curblock->setName(G.name);

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_description()
{
   char  *lp;

   if ((lp = strword()) != NULL)
   {
      //doc->SetUnknownAttrib(&curblock->getAttributesRef(), "DESCRIPTION", lp, SA_OVERWRITE, NULL); //  
      int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex("DESCRIPTION",valueTypeString);
      curblock->attributes().setAttribute(getCamCadData(),keywordIndex,lp);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_mount_type()
{
   char  *lp;

   if ((lp = strword()) != NULL)
   {
      if (!STRCMPI(lp,"SURFACE"))
      {
         //doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),VT_NONE,NULL,/* this makes a "real" char* */ SA_OVERWRITE, NULL); //
         curblock->attributes().setAttribute(getCamCadData(),standardAttributeSmd);
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int update_curpad(const char *name, int laynr, double x, double y, double rot, int smd)
{        
   // first check if layer already has a padstack
   int   i;

   for (i=0;i<curpad_cnt;i++)
   {
      if (curpad[i].layerptr == laynr)
      {
         curpad[i].rotation = rot;
         curpad[i].layerptr = laynr;   // padstack has lyr number, not level number
         strcpy(curpad[i].padgeomname, name);
         curpad[i].x = x;
         curpad[i].y = y;
         curpad[i].smd = smd;
         return i;
      }
   }

   if (curpad_cnt < MAX_CURPAD)
   {
      curpad[curpad_cnt].rotation = rot;
      curpad[curpad_cnt].layerptr = laynr;   // padstack has lyr number, not level number
      strcpy(curpad[curpad_cnt].padgeomname, name);
      curpad[curpad_cnt].x = x;
      curpad[curpad_cnt].y = y;
      curpad[curpad_cnt].smd = smd;
      curpad_cnt++;
   }
   else
   {
      fprintf(ferr,"Too many padstack overwrites at %ld\n",getInputFileLineNumber());
      display_error++;
      return -1;
   }
   return curpad_cnt-1;
}

/****************************************************************************/
/*
   expand the pincode into the pad array
*/
static int init_curpad(const char *p)
{
   DataStruct  *np;

   if (NumberOfLayers == 0)
   {
      fprintf(ferr, "No technology file loaded - padlayers for [%s] can not be expanded correctly\n", p);
      return 0;
   }

   //BlockStruct *b = doc->Find_Block_by_Name(p, -1);
   BlockStruct* b = getCamCadData().getBlock(p,-1);

   if (b == NULL) return 1;

   int smd = FALSE;

   //CAttribute *a =  is_attvalue(doc, b->getAttributesRef(), ATT_SMDSHAPE, 2);

   CString smdKeyword = StandardAttributeTagToName(standardAttributeSmd);
   int smdOutKeywordIndex = getCamCadData().getKeyWordArray().Lookup(smdKeyword,keyWordSectionOut);
   CAttribute* a = b->attributes().lookupAttribute(smdOutKeywordIndex);

   if (a)   smd = TRUE;

   // loop throu file
   POSITION pos = b->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = b->getDataList().GetNext(pos);

      if (np->getDataType() == dataTypeInsert)
      {
         // get padshape name and layer
         BlockStruct *block = getCamCadData().getBlock(np->getInsert()->getBlockNumber());
         LayerStruct *l = getCamCadData().getLayer(np->getLayerIndex());
      
         int fromlayer, tolayer;

         if (!STRCMPI(l->getName(),SMD_TOP))
         {
            fromlayer = 1;
            tolayer   = 1;
         }
         else
         if (!STRCMPI(l->getName(),SMD_BOT))
         {
            fromlayer = NumberOfLayers;
            tolayer   = NumberOfLayers;
         }
         else
         if (!STRCMPI(l->getName(),PAD_TOP))
         {
            fromlayer = 1;
            tolayer   = 1;
         }
         else
         if (!STRCMPI(l->getName(),PAD_BOT))
         {
            fromlayer = NumberOfLayers;
            tolayer   = NumberOfLayers;
         }
         else  
         if (!STRCMPI(l->getName(),PAD_INT))
         {
            fromlayer = 2;
            tolayer   = NumberOfLayers-1;
         }
         else              
         {
            update_curpad(block->getName(), np->getLayerIndex(), np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), 
                          radiansToDegrees(np->getInsert()->getAngle()), smd);
            continue;
         }

         // here now make the layers correct
         for (int i=fromlayer;i<=tolayer;i++)
         {
            CString  tmp;
            tmp.Format("PadLayer%d",i);
            int   lyrnr = graphLayer(tmp,"", 0);
            update_curpad(block->getName(), lyrnr, np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), radiansToDegrees(np->getInsert()->getAngle()), smd);
         }
      }
   }

   return 0;
}

/****************************************************************************/
/*
*/
static int jobprefs_userlayer()
{
   int   id = cur_ident;

   go_command(userlayer_lst,SIZ_USERLAYER_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int netprops_netname()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.name = lp;

   go_command(netprops_netname_lst,SIZ_NETPROPS_NETNAME_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int pdb_typenumber()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.name = lp;
   G.name.MakeUpper();

   //curtype = AddType(fl, G.name);
   curtype = fl->getTypeList().addType(G.name);

   go_command(pdb_typenumber_lst,SIZ_PDB_TYPENUMBER_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int netprops_netclass()
{
   char  *lp;

   lp = strword();

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
static int jobprefs_layerstackup()
{
   int   id = cur_ident;

   go_command(layerstackup_lst,SIZ_LAYERSTACKUP_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int layerstackup_dialectric()
{
   int      oldstat = cur_status;
   int      id = cur_ident;
   int      stacknr;
   char     *lp;
   CString  ul;
   LayerTypeTag ltyp;

   lp = strword();
   stacknr = atoi(lp);

   G.level = "";  // layerdescription
   G.lyrnr = 0;   // conductive layer number
   G.thickness = -1;
   G.name_1 = ""; // plane netname
      
   go_command(layercontens_lst,SIZ_LAYERCONTENS_LST,id);
   ul = G.level;
   if (!G.level.CompareNoCase("Solder_Mask"))
   {
      if (stacknr == 1)
      {
         G.level = PAD_TOP_SOLDERMASK;
         ltyp = layerTypeMaskTop;
      }
      else
      {
         G.level = PAD_BOT_SOLDERMASK;
         ltyp = layerTypeMaskBottom;
      }
   }
   else
   {
      ul.Format("DIELECTRIC_%d", ++G.dialeccnt);
      ltyp = layerTypeDielectric;
   }

   int lnr = graphLayer(ul, "", 0);
   LayerStruct *l = getCamCadData().getLayer(lnr);
   l->setComment(G.level);
   l->setLayerType(ltyp);
   l->setArtworkStackNumber(stacknr);
      
   if (G.thickness > 0)
   {
      //doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_THICKNESS, 0),VT_UNIT_DOUBLE, &G.thickness, SA_OVERWRITE, NULL); // 
      l->attributes().setAttribute(getCamCadData(),standardAttributeLayerThickness,G.thickness);
   }

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
   short    artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
   short    electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
   short    physicalstacknumber;    // physical manufacturing stacking of layers, 
                                    // signaltop / dialectric / signal_1 / dia / power_1 .... / solder
*/
static int layerstackup_plane()
{
   int      oldstat = cur_status;
   int      id = cur_ident;
   int      stacknr;
   char     *lp;
   CString  ul;

   lp = strword();
   stacknr = atoi(lp);

   G.level = "";  // layerdescription
   G.lyrnr = 0;   // conductive layer number
   G.thickness = -1;
   G.name_1 = "";

   go_command(layercontens_lst,SIZ_LAYERCONTENS_LST,id);

   ul.Format("LYR_%d", G.lyrnr); // this is also used in plane_net

   int lnr = graphLayer(ul, "", 0);
   LayerStruct *l = getCamCadData().getLayer(lnr);
   l->setComment(G.level);
   l->setElectricalStackNumber(G.lyrnr);
   l->setArtworkStackNumber(stacknr);

   // Case 1585, if plane_type (saved in cur_status) is "positive", then set
   // layer type in same fashion as SIGNAL_PLANE. It would have been nice to
   // just call layerstackup_signal, but we don't know which way it will
   // until we're into it.
   if (cur_status == FILE_POS_PLANE)
   {
      if (G.lyrnr == 1)
         l->setLayerType(layerTypeSignalTop);
      else
      if (G.lyrnr == NumberOfLayers)
         l->setLayerType(layerTypeSignalBottom);
      else
         l->setLayerType(layerTypeSignalInner);
   }
   else
   {
      // How it was before case 1585
      l->setLayerType(layerTypePowerNegative);
   }
   
      
   if (G.thickness > 0)
   {
      //doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_THICKNESS, 0),VT_UNIT_DOUBLE,&G.thickness,SA_OVERWRITE, NULL); // x, y, rot, height
      l->attributes().setAttribute(getCamCadData(),standardAttributeLayerThickness,G.thickness);
   }

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int layerstackup_signal()
{
   int      oldstat = cur_status;
   int      id = cur_ident;
   int      stacknr;
   char     *lp;
   CString  ul;

   lp = strword();
   stacknr = atoi(lp);
   
   G.level = "";  // layerdescription
   G.lyrnr = 0;   // conductive layer number
   G.thickness = -1;
   go_command(layercontens_lst,SIZ_LAYERCONTENS_LST,id);

   ul.Format("LYR_%d", G.lyrnr);

   int lnr = graphLayer(ul, "", 0);
   LayerStruct *l = getCamCadData().getLayer(lnr);
   l->setComment(G.level);

   if (G.lyrnr == 1)
      l->setLayerType(layerTypeSignalTop);
   else
   if (G.lyrnr == NumberOfLayers)
      l->setLayerType(layerTypeSignalBottom);
   else
      l->setLayerType(layerTypeSignalInner);
   l->setElectricalStackNumber(G.lyrnr);
   l->setArtworkStackNumber(stacknr);

   if (G.thickness > 0)
   {
      //doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_THICKNESS, 0),VT_UNIT_DOUBLE,&G.thickness,SA_OVERWRITE, NULL); // x, y, rot, height
      l->attributes().setAttribute(getCamCadData(),standardAttributeLayerThickness,G.thickness);
   }

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int layercontens_description()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.level = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int layercontens_plane_type()
{
   char     *lp;

   if (lp = strword())
   {
      if (!STRCMPI(lp,"POSITIVE"))
      {
         cur_status = FILE_POS_PLANE;
      }
      else if (!STRCMPI(lp,"NEGATIVE"))
      {
         cur_status = FILE_NEG_PLANE;
      }
      else
      {
         cur_status = FILE_UNKNOWN;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int layercontens_plane_net()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();   // this needs to be implemented in a netname array, because multiple netnames
                     // can appear here

   if (G.lyrnr && lp && strlen(lp))
   {
      CString  ul;
      ul.Format("LYR_%d", G.lyrnr);
      int lnr = graphLayer(ul, "", 0);
      LayerStruct *l = getCamCadData().getLayer(lnr);

      //doc->SetUnknownAttrib(&l->getAttributesRef(),ATT_NETNAME,lp, SA_APPEND, NULL); // x, y, rot, height
      curtype->attributes().setAttribute(getCamCadData(),standardAttributeNetName,lp);

      // update layer as a powerplane - powernet
      //NetStruct *n = add_net(file,lp);
      NetStruct* n = fl->getNetList().addNet(lp);

      //doc->SetAttrib(&n->getAttributesRef(),doc->IsKeyWord(ATT_POWERNET, 0),VT_NONE, NULL, SA_OVERWRITE, NULL); //
      n->attributes().setAttribute(getCamCadData(),standardAttributePowerNet);
   }

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
static int layercontens_thickness()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.thickness = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int layercontens_resistivity()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();

   return 1;
}

/****************************************************************************/
/*
*/
static int layercontens_conductivenr()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.lyrnr = atoi(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int userlayer_layername()
{
   int   id = cur_ident;
   CString  user_layer_name, ul;
   char  *lp;

   lp = strword();
   user_layer_name = lp;

   G.lyrnr = 0;
   // get user_layer_number
   go_command(userlayernum_lst,SIZ_USERLAYERNUM_LST,id);

   ul.Format("USER_LVL_%d", G.lyrnr);

   int lnr = graphLayer(ul, "", 0);
   LayerStruct *l = getCamCadData().getLayer(lnr);
   l->setComment(user_layer_name);

   return 1;
}

/****************************************************************************/
/*
*/
static int userlayer_layernumber()
{
   char  *lp;

   lp = strword();
   G.lyrnr = atoi(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_via()
{
   int   id = cur_ident;
   
   polycnt = 0;

   go_command(via_instance_lst,SIZ_VIA_INSTANCE_LST,id);

   if (USE_CELL_FANOUT_VIAS)
   {
      // here write via
      if (polycnt)
      {
         // make sure via exist
         getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,G.symbol,-1,0);
         getLegacyGraph().graphBlockOff();

         // G.symbol = Padstack
         DataStruct *d = getLegacyGraph().graphBlockReference(G.symbol, NULL, -1, poly_l[0].x,poly_l[0].y, 
            0.0, 0, 1.0, getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);
         d->getInsert()->setInsertType(insertTypeVia);
      }
   }
   polycnt = 0;

   return 1;
}

static int celpart_package_group()
{
   char* lp = strword();
   G.packageGroup = lp;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_pin()
{
   int   id = cur_ident;
   char  *lp;

   //here update last curpad struct
   overwrite_pad();

   G.name_1 = ""; // pin options
   polycnt = 0;
   G.rotation = 0;
   G.pincnt++;
   lp = strtok(NULL, WORD_DELIMETER);
   CString tok = clean_quote(lp);
   CString pinName = tok;

   WORD count = 0;
   if (G.m_duplicatePinMapCount.Lookup(tok, count))
      pinName.Format("%s[%d]", tok, ++count);
   G.m_duplicatePinMapCount.SetAt(tok, count);

   G.pinname = pinName;

   go_command(celpin_lst, SIZ_CELPIN_LST, id);

   CDcaVBGeom* vbGeom = VBGeomList.GetLastGeom();
   if (vbGeom != NULL)
      vbGeom->AddPin(G.pinname, G.name_1);


   // make sure pin exist
   BlockStruct *b = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, G.symbol, -1, 0);
   getLegacyGraph().graphBlockOff();

   init_curpad(G.symbol);

   // here place pin
   if (!polycnt)
   {
      poly_l[0].x = 0;
      poly_l[0].y = 0;
   }

   int mir = FALSE;
   int rot = 0;
   if (strstr(G.name_1, "OPP_SIDE"))
   {
      mir = TRUE;
      rot = 180;
   }

   DataStruct *d = getLegacyGraph().graphBlockReference(G.symbol, G.pinname, 0, poly_l[0].x, poly_l[0].y,
         degreesToRadians(G.rotation + rot), mir != 0, 1.0, getCamCadData().getDefinedFloatingLayer().getLayerIndex(), TRUE);

   InsertTypeTag btype = getDefaultInsertTypeForBlockType(b->getBlockType()); // this could be fiducial or mounting hole

   if (btype != insertTypeUnknown)
   {
      d->getInsert()->setInsertType(btype);
      G.pinname = "";
   }
   else
   {
      d->getInsert()->setInsertType(insertTypePin);
   }

   lastpininsert = d;

   // do pinnr here
   if (strlen(G.pinname))
   {
      int   pnr = atoi(G.pinname);
      if (pnr)
      {
         //doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, TRUE),VT_INTEGER, &pnr, SA_OVERWRITE, NULL); //  
         d->attributes().setAttribute(getCamCadData(),standardAttributePinNumber,pnr);
      }
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int update_pinname(BlockStruct *b, int pinnr, const char *pinname)
{
   int      found = 0;

   CString  pinnumber;
   pinnumber.Format("%d",pinnr);

   DataStruct *np;
   POSITION pos = b->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = b->getDataList().GetNext(pos);
      
      if (np->getDataType() == dataTypeInsert)        
      {
         if (np->getInsert()->getInsertType() == insertTypePin)
         {
            // pinname is in refname if a insert

            // these are absolute instructions.
            //CAttribute *a =  is_attvalue(doc, np->getAttributesRef(), ATT_COMPPINNR, 0);
            CAttribute* a = np->attributes().lookupAttribute(standardAttributePinNumber);

            if (a)
            {
               if (a->getValueType() == valueTypeInteger)
               {
                  // int   pt = a->getIntValue();
                  if (pinnr == a->getIntValue())
                  {
                     np->getInsert()->setRefname(STRDUP(pinname));
                     found++;
                     break;
                  }
               }
               else if (a->getValueType() == valueTypeString)
               {
                  CString l = a->getStringValue();

                  if (pinnumber.CompareNoCase(l) == 0)
                  {
                     np->getInsert()->setRefname(STRDUP(pinname));
                     found++;
                     break;
                  }
               }
            }
         }
      }
   } 

   if (!found)
   {
      fprintf(ferr,"Update error : Geometry [%s] Pinnr [%d] Pinname [%s]\n",
         b->getName(), pinnr, pinname);

      display_error++;
   }

   return found;
}

/******************************************************************************
* vb99_mh
*/
static int vb99_mh()
{
   int id = cur_ident;
   
   G.symbol = "";
   polycnt = 0;
   G.diameter = 0;
   go_command(mh_instance_lst, SIZ_MH_INSTANCE_LST, id);

   if (!strlen(G.symbol))
      G.symbol.Format("MH_%lg", G.diameter);

   //if (Graph_Block_Exists(doc, G.symbol, -1) == NULL)
   if (getLegacyGraph().graphBlockExists(G.symbol,-1) == NULL)      
   {
      int layerNum = graphLayer("DRILLHOLE", "", 0);

      CString drillName;
      drillName.Format("DRILL_%d", get_drillindex(G.diameter, layerNum));
      getLegacyGraph().graphBlockReference(drillName, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerNum, TRUE);

      BlockStruct *block = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend, G.symbol, -1, 0);
      block->setBlockType(blockTypeTooling);
      getLegacyGraph().graphBlockReference(drillName, NULL, 0, 0, 0, 0.0, 0 , 1.0, layerNum, TRUE);
      getLegacyGraph().graphBlockOff();
   }

   DataStruct *data = getLegacyGraph().graphBlockReference(G.symbol, NULL, 0, poly_l[0].x, poly_l[0].y, 0.0, 0 , 1.0, -1, TRUE);
   data->getInsert()->setInsertType(insertTypeDrillTool);

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_silk_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SILK_ARTWORK;

   G.level = "SILKSCREEN_TOP";
   G.lyrnr = graphLayer(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_trace()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   if (USE_CELL_FANOUT_VIAS == false)
   {
      vb99_skip();
   }
   else
   {
      cur_status = CELL_FANOUT;

      go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

      cur_status = oldstat;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_fiducial()
{
   int   id = cur_ident;
   
   G.sidebot = FALSE;
   G.symbol = "";
   polycnt = 0;
   go_command(fiducial_instance_lst,SIZ_FIDUCIAL_INSTANCE_LST,id);

   int      mir = 0;

   if (!strlen(G.symbol))
      G.symbol.Format("FIDC_%lg",G.radius*2);

   // just make sure, it's there.
   //if (Graph_Block_Exists(doc, G.symbol, -1) == NULL)
   if (getLegacyGraph().graphBlockExists(G.symbol,-1) == NULL)      
   {
      BlockStruct *b;
      b = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,G.symbol,-1,0);

      //int err;
      CString  appname;
      appname.Format("FIDAPP_%lg",G.radius*2);

      //Graph_Aperture(appname, T_TARGET, G.radius*2, G.radius, 0.0, 0.0, 0.0, 0,BL_APERTURE, TRUE, &err);
      getCamCadData().getDefinedAperture(appname,apertureTarget,G.radius*2,G.radius,0.,0.,0.,-1,BL_APERTURE,0);
      getLegacyGraph().graphBlockReference(appname, NULL, 0, 0, 0, 0.0, 0 , 1.0, -1, TRUE);
      getLegacyGraph().graphBlockOff();
      b->setBlockType(blockTypeFiducial);
   }

   DataStruct *d = getLegacyGraph().graphBlockReference(G.symbol, NULL, 0, poly_l[0].x,poly_l[0].y,0.0, mir != 0 , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeFiducial);
   if (G.sidebot) d->getInsert()->setPlacedBottom(true);

   polycnt = 0;

   return 1;
   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_gluespot()
{
   int   id = cur_ident;
   
   G.sidebot = FALSE;
   G.symbol = "";
   polycnt = 0;

   go_command(gluespot_lst,SIZ_GLUESPOT_LST,id);

   int mir = 0;

   //BlockStruct *gluepoint = generate_Gluepoint_Geom(doc);
   BlockStruct* gluePointGeometry = &(getCamCadData().getDefinedGluePointGeometry());

   DataStruct* d = getLegacyGraph().graphBlockReference(gluePointGeometry->getName(), NULL, 0, poly_l[0].x,poly_l[0].y, 0.0, mir != 0 , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeGluePoint);

   if (G.sidebot) d->getInsert()->setPlacedBottom(true);

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_placement_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PLACEMENT_OUTLINE;

   G.level = "PLACEMENT_OUTLINE";
   G.lyrnr = graphLayer(G.level, "", 0);
   G.height = 0;
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   if (G.height > 0 && curblock)
   {
      //doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE, &G.height, SA_OVERWRITE, NULL);
      curblock->attributes().setAttribute(getCamCadData(),standardAttributeComponentHeight,G.height);
   }

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_assembly_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = CELL_ASSEMBLY_OUTLINE;

   G.level = "ASSEMBLY_TOP";
   G.lyrnr = graphLayer(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_insertion_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = CELL_INSERTION_OUTLINE;

   G.level = "INSERTION_TOP";
   G.lyrnr = graphLayer(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
   Outlines and polies are not in sequence
*/
static int write_poly()
{
   int widthindex;
   int i;
   int fill = 0;
   int voidtype = 0;
   int closed = 0;
   int poured = 0;
   int hidenFlag = 0;

   if (polycnt == 0)
      return 1;

   //if (G.diameter == 0)
   //{
   //   widthindex = Graph_Aperture("", apertureRound, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   //}
   //else
   //{
   //   widthindex = Graph_Aperture("", apertureRound, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   //}

   getCamCadData().getDefinedAperture("",apertureRound,G.diameter,0.,0.,0.,0.,-1,BL_WIDTH,0,&widthindex);

   DataStruct *curdata = getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);

   // here need to do TRACE_OBS
   //                 VIA_OBS
   //                 PLAC_OBS

   closed = (poly_l[0].x == poly_l[polycnt-1].x && poly_l[0].y == poly_l[polycnt-1].y);

   if (cur_status == FILE_SILK_ARTWORK)
   {
   }
   else if (cur_status == FILE_PLACEMENT_OUTLINE)
   {
      curdata->setGraphicClass(graphicClassComponentOutline);

   }
   else if (cur_status == FILE_POS_PLANE)
   { 
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(graphicClassEtch);

         //add_net(fl, G.name);
         NetStruct* net = fl->getNetList().addNet(G.name);

         //doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         int netNameInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeNetName),keyWordSectionIn);
         curdata->attributes().setAttribute(getCamCadData(),netNameInKeywordIndex,G.name,attributeUpdateAppend);
      }
   }
   else if (cur_status == FILE_SHAPE)
   {
      closed = 1;
      fill = cur_fill;
   }
   else if (cur_status == FILE_UNKNOWN)
   {
      if (closed)
         fill = cur_fill;
   }
   else if (cur_status == FILE_PLANE_OUTL)
   {
      closed = 1;
      fill = 1;
      
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(graphicClassEtch);

         //add_net(fl, G.name);
         NetStruct* net = fl->getNetList().addNet(G.name);

         //doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         int netNameInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeNetName),keyWordSectionIn);
         curdata->attributes().setAttribute(getCamCadData(),netNameInKeywordIndex,G.name,attributeUpdateAppend);
      }
   }
   else if (cur_status == FILE_PLANE_HOLE)
   {
      closed = 1;
      fill = 1;
      voidtype = 1;
      
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(graphicClassEtch);

         //add_net(fl, G.name);
         NetStruct* net = fl->getNetList().addNet(G.name);

         //doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),valueTypeString,G.name.GetBuffer(0), attributeUpdateAppend,NULL);
         int netNameInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeNetName),keyWordSectionIn);
         curdata->attributes().setAttribute(getCamCadData(),netNameInKeywordIndex,G.name,attributeUpdateAppend);
      }
   }
   else if (cur_status == FILE_ROUTE_OUTL)
   {
      curdata->setGraphicClass(graphicClassRouteKeepIn);
   }
   else if (cur_status == FILE_BOARD_OUTL)
   {
      curdata->setGraphicClass(graphicClassBoardOutline);
   }
   else if (cur_status == FILE_ASSM_OUTL)
   {
      curdata->setGraphicClass(graphicClassComponentOutline);
   }
   else if (cur_status == FILE_ROUTE_OBS)
   {
      curdata->setGraphicClass(graphicClassRouteKeepOut);
   }
   else if (cur_status == FILE_PLACE_OBS)
   {
      curdata->setGraphicClass(graphicClassPlaceKeepOut);
   }
   else if (cur_status == FILE_VIA_OBS)
   {
      curdata->setGraphicClass(graphicClassViaKeepOut);
   }
   else if (cur_status == CELL_FANOUT)
   {
      curdata->setGraphicClass(graphicClassTraceFanout);
   }
   else if (cur_status == FILE_TRACE)
   {
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(graphicClassEtch);

         //add_net(fl, G.name);
         NetStruct* net = fl->getNetList().addNet(G.name);

         //doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         int netNameInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeNetName),keyWordSectionIn);
         curdata->attributes().setAttribute(getCamCadData(),netNameInKeywordIndex,G.name,attributeUpdateAppend);
      }
   }
   else if (cur_status == FILE_CONDUCTIVE_AREA)
   {
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(graphicClassEtch);

         //add_net(fl, G.name);
         NetStruct* net = fl->getNetList().addNet(G.name);

         //doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         int netNameInKeywordIndex = getCamCadData().getKeyWordArray().Lookup(StandardAttributeTagToName(standardAttributeNetName),keyWordSectionIn);
         curdata->attributes().setAttribute(getCamCadData(),netNameInKeywordIndex,G.name,attributeUpdateAppend);
      }

      closed = 1;
      fill = 0;

      if (!use_generated_data)
         fill = cur_fill;
      else
         hidenFlag = TRUE;

      poured = TRUE; // this is only the boundary, no voids etc.. They are in the Generated_Data section as lines and arcs.
   }
   else if (cur_status == FILE_SOLDER_MASK)
   {
      // Case 1584, apply fill code to solder mask items
      if (closed)
         fill = cur_fill;
   }

   CPoly *ppoly = getLegacyGraph().graphPoly(NULL,  widthindex, fill, voidtype, closed);   // fillflag , negative, closed

   if (poured)
      ppoly->setFloodBoundary(true);
   if (hidenFlag)
      ppoly->setHidden(true);

   if (isInGeneratedData && use_generated_data)
   {
      ppoly->setHatchLine(true);
   }

   CPnt* lastvertex = NULL;
   for (i=0;i<polycnt;i++)
   {
      if (fabs(poly_l[i].bulge) > 0)   // this is the radius and center point
      {
         // if bulge < 0 it is counter clockwise, > 0 is clockwise

         if (lastvertex && i < (polycnt-1))
         {
            double   r, sa, da;

            if (poly_l[i].bulge < 0)   
            {
               // CCW
               ArcCenter2( poly_l[i-1].x, poly_l[i-1].y, 
                        poly_l[i+1].x, poly_l[i+1].y,
                        poly_l[i].x, poly_l[i].y,
                        &r, &sa, &da, FALSE );
            }
            else
            {
               // CW
               ArcCenter2( poly_l[i-1].x, poly_l[i-1].y, 
                        poly_l[i+1].x, poly_l[i+1].y,
                        poly_l[i].x, poly_l[i].y,
                        &r, &sa, &da, TRUE );
            }

            if (fabs(r - fabs(poly_l[i].bulge)) > 0.01)
            {
               // here is some calc error
#ifdef _DEBUG
               fprintf(ferr, "DEBUG: Arc calculation error in [%s] at %ld\n", cur_filename, getInputFileLineNumber());
               display_error++;
#endif
            }
            else
            {
               lastvertex->bulge = (DbUnit)tan(da/4);
            }
         }
      }
      else
      {
         lastvertex = getLegacyGraph().graphVertex(poly_l[i].x, poly_l[i].y, poly_l[i].bulge);
      }
   }

   cur_fill = 0;
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int des_line()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   
   polycnt = 0;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   G.lyrnr = graphLayer(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_sym()
{
   int   id = cur_ident;
   vb99_null(id);
   return 1;
}

/****************************************************************************/
/*
*/
static int des_arc()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   DbFlag flg = 0;

   cur_status = FILE_TRACE;

   polycnt = 0;
   G.diameter = 0;
   go_command(grapharc_lst,SIZ_GRAPHARC_LST,id);

   write_arc();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_circle()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   DbFlag flg = 0;

   cur_status = FILE_TRACE;

   polycnt = 0;
   G.diameter = 0;
   cur_fill = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = graphLayer(G.level,"",0);
   write_circle();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_graphic()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   cur_fill  = FALSE;
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}
/****************************************************************************/
/*
*/
static int vb99_drillgraphic()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   cur_fill  = FALSE;

   G.level = "DRILLDRAWING_THRU"; // has not an own layer, this is the layername that is used in VB
   G.lyrnr = graphLayer(G.level,"",0);

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_route_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_ROUTE_OUTL;
   G.level = "ROUTE_OUTL";
   G.lyrnr = graphLayer("ROUTE_OUTL","",0);
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_generated_data()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   if (use_generated_data)
   {
      isInGeneratedData = true;
      go_command(generated_data_lst,SIZ_GENERATED_DATA_LST,id);
   }
   else
   {
      vb99_skip();
   }

   cur_status = oldstat;
   isInGeneratedData = false;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_assembly_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_ASSEMBLY_OUTL;
   G.level = "ASSEMBLY_TOP";
   G.lyrnr = graphLayer("ASSEMBLY_TOP","",0);
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_drill_graphic()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = 0;
   G.level = "DRILLSYMBOL";
   G.lyrnr = graphLayer("DRILLSYMBOL","",0);
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_silkscreen_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SILK_ARTWORK;

   G.level = "SILKSCREEN_TOP";
   G.lyrnr = graphLayer(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_insertion_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_INSERTION_OUTL;
   G.level = "INSERTION_TOP";
   G.lyrnr = graphLayer("INSERTION_TOP","",0);
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_solder_mask()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SOLDER_MASK;
   G.level = "SOLDER_MASK";
   G.lyrnr = graphLayer("SOLDER_MASK","",0);
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_board_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_BOARD_OUTL;

   G.level = "BOARD_OUTL";
   G.lyrnr = graphLayer("BOARD_OUTL","",0);
   LayerStruct *l = getCamCadData().getLayer(G.lyrnr);
   l->setLayerType(layerTypeBoardOutline);

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int net_trace()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_TRACE;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int net_conductive_area()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_CONDUCTIVE_AREA;
   G.diameter = 0; // reset linewidth to 0

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_net()
{
   int      id = cur_ident;
   int      oldstat = cur_status;
   char     *lp;

   lp = strword();
   G.name = lp;
   if (!STRCMPI(lp,"Unconnected_Net"))
      G.name = "";

   cur_status = FILE_NET;

   go_command(net_lst,SIZ_NET_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_horzjust()
{
   char     *lp;
   lp = strword();

   if (!STRCMPI(lp,"LEFT"))
   {
      G.just |= GRTEXT_W_L;
   }
   else if (!STRCMPI(lp,"CENTER"))
   {
      G.just |= GRTEXT_W_C;
   }
   else if (!STRCMPI(lp,"RIGHT"))
   {
      G.just |= GRTEXT_W_R;
   }
   
   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_vertjust()
{
   char     *lp;
   lp = strword();

   if (!STRCMPI(lp,"TOP"))
   {
      G.just |= GRTEXT_H_T;
   }
   else if (!STRCMPI(lp,"CENTER"))
   {
      G.just |= GRTEXT_H_C;
   }
   else if (!STRCMPI(lp,"BOTTOM"))
   {
      G.just |= GRTEXT_H_B;
   }
   
   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_textwidth()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.textwidth = cnv_unit(atof(lp));
   
   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_textoption()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   if (strstr(lp,"MIRRORED"))
      G.mirror = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_font()
{
   // not implemented
   return 1;
}

/****************************************************************************/
/*
*/
static int des_boardorg()
{
   int   id = cur_ident;

   fprintf(ferr,"BOARD ORIGIN not implemented\n");
   go_command(board_org_lst,SIZ_BOARD_ORG_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_square()
{
   int   id = cur_ident;

   G.geomtyp = apertureSquare;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_octagon()
{
   int   id = cur_ident;

   G.geomtyp = apertureOctagon;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_custom()
{
   int   id = cur_ident;

   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_radius_corner_rect()
{
   int   id = cur_ident;
   double x, y;
   double radius;
   
   //if (G.name.Find("1.160") >= 0)
   //{
   //   int iii = 3;

   //   CStdioFileWriteFormat debugWriteFormat("c:\\padform_radius_corner_rect-debug.txt");
   //   debugWriteFormat.setNewLineMode(true);

   //   getCamCadData().getBlockDirectory().dump(debugWriteFormat);
   //}

   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   radius = G.radius;
   x = G.xdim / 2;
   y = G.ydim / 2;

   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-x + radius, y, 0);
   getLegacyGraph().graphVertex(x - radius, y, -0.5);
   getLegacyGraph().graphVertex(x, y - radius, 0);
   getLegacyGraph().graphVertex(x, -y + radius, -0.5);
   getLegacyGraph().graphVertex(x - radius, -y, 0);
   getLegacyGraph().graphVertex(-x + radius, -y, -0.5);
   getLegacyGraph().graphVertex(-x, -y + radius, 0);
   getLegacyGraph().graphVertex(-x , y - radius, -0.5);
   getLegacyGraph().graphVertex(-x + radius, y, 0); 

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
int padform_octagonal_finger()
{
   int   id = cur_ident;
   double x, y;
   double offset;
   

   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.;
   y = G.ydim / 2.;
   

   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
#ifdef FIRST_TRY
   // The docs on hand make it seem like the definition should be like this.
   // I.e. the chamfered end should be to the right.
   // However, the result does not match the expectation set by a gerber image
   // of what we are after. Info in case folder, case dts0100386334.
   offset = G.ydim / 3.34;;
   getLegacyGraph().graphVertex(-x, y, 0.);
   getLegacyGraph().graphVertex(x - offset,  y, 0.);
   getLegacyGraph().graphVertex(x,  y - offset, 0.);
   getLegacyGraph().graphVertex(x, -y + offset, 0.);
   getLegacyGraph().graphVertex(x - offset, -y, 0.);
   getLegacyGraph().graphVertex(-x , -y, 0.);
   getLegacyGraph().graphVertex(-x,   y, 0.);
#else
   // Seems it needs to be defined with the chamfer at the top.
   offset = G.xdim / 3.34;
   getLegacyGraph().graphVertex(-x, -y, 0.);
   getLegacyGraph().graphVertex(-x,  y - offset, 0.);
   getLegacyGraph().graphVertex(-x + offset,  y, 0.);
   getLegacyGraph().graphVertex( x - offset,  y, 0.);
   getLegacyGraph().graphVertex( x, y - offset, 0.);
   getLegacyGraph().graphVertex( x , -y, 0.);
   getLegacyGraph().graphVertex(-x,  -y, 0.);
#endif

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
int padform_elongated_octagon()
{
   int   id = cur_ident;
   double x, y;
   double offset;
   

   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.;
   y = G.ydim / 2.;
   

   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);

   offset = min(G.xdim / 3.34, G.ydim / 3.34);
   getLegacyGraph().graphVertex(-x, -y + offset, 0.);
   getLegacyGraph().graphVertex(-x,  y - offset, 0.);
   getLegacyGraph().graphVertex(-x + offset,  y, 0.);
   getLegacyGraph().graphVertex( x - offset,  y, 0.);
   getLegacyGraph().graphVertex( x, y - offset, 0.);
   getLegacyGraph().graphVertex( x , -y + offset, 0.);
   getLegacyGraph().graphVertex( x - offset,  -y, 0.);
   getLegacyGraph().graphVertex(-x + offset, -y, 0.);
   getLegacyGraph().graphVertex(-x, -y + offset, 0.);

   getLegacyGraph().graphBlockOff();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_square_donut()
{
   int   id = cur_ident;
   double x, y;
   

   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  
   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);

   // Outer
   x = G.xdim / 2.;
   y = x;
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-x, -y, 0.);
   getLegacyGraph().graphVertex(-x,  y, 0.);
   getLegacyGraph().graphVertex( x,  y, 0.);
   getLegacyGraph().graphVertex( x, -y, 0.);
   getLegacyGraph().graphVertex(-x, -y, 0.);

   // Hole
   x = G.xdim / 3.5;
   y = x;
   getLegacyGraph().graphPoly(NULL,  0, 1, 1, 1);
   getLegacyGraph().graphVertex(-x, -y, 0.);
   getLegacyGraph().graphVertex(-x,  y, 0.);
   getLegacyGraph().graphVertex( x,  y, 0.);
   getLegacyGraph().graphVertex( x, -y, 0.);
   getLegacyGraph().graphVertex(-x, -y, 0.);

   getLegacyGraph().graphBlockOff();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_square_thermal()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  
   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);

   x = G.xdim / 2.; // overall width defines corner xy's.
   y = x;

   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   double tie_leg_width = G.tie_leg_width;
   if (tie_leg_width <= 0.0)
      tie_leg_width = G.xdim * .20; // default to 20% of width for gap
   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = G.xdim * .15; // default to 15% of width for thickness

   double leg = x - (tie_leg_width / 2.);
   double thk = thermal_clearance;

   if (leg > x) // two legs would be greater than width, not good, punt
      leg = G.xdim / 2.35;

   if (thk > x) // two thicknesses would be greater than width, not good, punt
      thk = leg / 2.35; // off the wall, makes a nice aspect ratio

   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-x, -y, 0.);
   getLegacyGraph().graphVertex(-x, -y + leg, 0.);
   getLegacyGraph().graphVertex(-x + thk, -y + leg, 0.);
   getLegacyGraph().graphVertex(-x + thk, -y + thk, 0.);
   getLegacyGraph().graphVertex(-x + leg, -y + thk, 0.);
   getLegacyGraph().graphVertex(-x + leg, -y, 0.);
   getLegacyGraph().graphVertex(-x, -y, 0.);

   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-x,  y, 0.);
   getLegacyGraph().graphVertex(-x + leg,  y, 0.);
   getLegacyGraph().graphVertex(-x + leg,  y - thk, 0.);
   getLegacyGraph().graphVertex(-x + thk,  y - thk, 0.);
   getLegacyGraph().graphVertex(-x + thk,  y - leg, 0.);
   getLegacyGraph().graphVertex(-x,  y - leg, 0.);
   getLegacyGraph().graphVertex(-x,  y, 0.);

   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( x,  y, 0.);
   getLegacyGraph().graphVertex( x,  y - leg, 0.);
   getLegacyGraph().graphVertex( x - thk,  y - leg, 0.);
   getLegacyGraph().graphVertex( x - thk,  y - thk, 0.);
   getLegacyGraph().graphVertex( x - leg,  y - thk, 0.);
   getLegacyGraph().graphVertex( x - leg,  y, 0.);
   getLegacyGraph().graphVertex( x,  y, 0.);

   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( x, -y, 0.);
   getLegacyGraph().graphVertex( x - leg, -y, 0.);
   getLegacyGraph().graphVertex( x - leg, -y + thk, 0.);
   getLegacyGraph().graphVertex( x - thk, -y + thk, 0.);
   getLegacyGraph().graphVertex( x - thk, -y + leg, 0.);
   getLegacyGraph().graphVertex( x, -y + leg, 0.);
   getLegacyGraph().graphVertex( x, -y, 0.);

   getLegacyGraph().graphBlockOff();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_square_thermal_45()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.; // overall width defines corner xy's.
   y = x;

   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   double tie_leg_width = G.tie_leg_width;
   if (tie_leg_width <= 0.0)
      tie_leg_width = G.xdim * .20; // default to 20% of width for gap
   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = G.xdim * .15; // default to 15% of width for thickness

   ///double leg = x - (tie_leg_width / 2.);
   double thk = thermal_clearance;

   // Images from Expedition GUI make it look like the pad segments have chamfers
   // at 45 degrees on the ends. The Thermal clearance then is the hypoteneuse
   // of a right triangle that defines the endpoints of the inside corners of
   // the segments. We get the length of a leg of this triangle, then use it
   // and the thickness to determine offsets for the bounding poly points.
   // The ends are chamfered using same style as octoagon finger.

   // c = sqrt(a^2 + b^2)
   // if a == b (right triangle) then
   // c = sqrt(2(a^2))
   // c^2 = 2(a^2)
   // (c^2)/2 = a^2
   // a = sqrt((c^2)/2)
   // We are given c as "tie leg width", get a
   double a = sqrt( (tie_leg_width * tie_leg_width) / 2.0 );

   double chamfer = thk / 3.34;
     
   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);

   // Top
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-x + thk + a, y, 0.);
   getLegacyGraph().graphVertex( x - thk - a, y, 0.);
   getLegacyGraph().graphVertex( x - thk - a + chamfer, y - chamfer, 0.);
   getLegacyGraph().graphVertex( x - thk - a + chamfer, y - thk + chamfer, 0.);
   getLegacyGraph().graphVertex( x - thk - a, y - thk, 0.);
   getLegacyGraph().graphVertex(-x + thk + a, y - thk, 0.);
   getLegacyGraph().graphVertex(-x + thk + a - chamfer, y - thk + chamfer, 0.);
   getLegacyGraph().graphVertex(-x + thk + a - chamfer, y - chamfer, 0.);
   getLegacyGraph().graphVertex(-x + thk + a, y, 0.);

   // Bottom
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-x + thk + a, -y, 0.);
   getLegacyGraph().graphVertex( x - thk - a, -y, 0.);
   getLegacyGraph().graphVertex( x - thk - a + chamfer, -y + chamfer, 0.);
   getLegacyGraph().graphVertex( x - thk - a + chamfer, -y + thk - chamfer, 0.);
   getLegacyGraph().graphVertex( x - thk - a, -y + thk, 0.);
   getLegacyGraph().graphVertex(-x + thk + a, -y + thk, 0.);
   getLegacyGraph().graphVertex(-x + thk + a - chamfer, -y + thk - chamfer, 0.);
   getLegacyGraph().graphVertex(-x + thk + a - chamfer, -y + chamfer, 0.);
   getLegacyGraph().graphVertex(-x + thk + a, -y, 0.);

   // Left
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-x, -y  + thk + a, 0.);
   getLegacyGraph().graphVertex(-x,  y - thk - a, 0.);
   getLegacyGraph().graphVertex(-x + chamfer,       y - thk - a + chamfer, 0.);
   getLegacyGraph().graphVertex(-x + thk - chamfer, y - thk - a + chamfer, 0.);
   getLegacyGraph().graphVertex(-x + thk,  y - thk - a, 0.);
   getLegacyGraph().graphVertex(-x + thk, -y + thk + a, 0.);
   getLegacyGraph().graphVertex(-x + thk - chamfer, -y + thk + a - chamfer, 0.);
   getLegacyGraph().graphVertex(-x + chamfer, -y + thk + a - chamfer, 0.);
   getLegacyGraph().graphVertex(-x, -y  + thk + a, 0.);

   // Right
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-(-x), -y  + thk + a, 0.);
   getLegacyGraph().graphVertex(-(-x),  y - thk - a, 0.);
   getLegacyGraph().graphVertex(-(-x + chamfer),       y - thk - a + chamfer, 0.);
   getLegacyGraph().graphVertex(-(-x + thk - chamfer), y - thk - a + chamfer, 0.);
   getLegacyGraph().graphVertex(-(-x + thk),  y - thk - a, 0.);
   getLegacyGraph().graphVertex(-(-x + thk), -y + thk + a, 0.);
   getLegacyGraph().graphVertex(-(-x + thk - chamfer), -y + thk + a - chamfer, 0.);
   getLegacyGraph().graphVertex(-(-x + chamfer), -y + thk + a - chamfer, 0.);
   getLegacyGraph().graphVertex(-(-x), -y  + thk + a, 0.);


   getLegacyGraph().graphBlockOff();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_round_thermal()
{
   int   id = cur_ident;
   
   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  

   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   ///double tie_leg_width = G.tie_leg_width;
   ///if (tie_leg_width <= 0.0)
   ///   tie_leg_width = G.xdim * .20; // default to 20% of width for gap
   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = G.xdim * .15; // default to 15% of width for thickness

   double thk = thermal_clearance;

   double outerRadius = G.xdim / 2.;
   double innerRadius = outerRadius - thk;

   double bulge = tan(degreesToRadians(60.) / 4.);  // bulge = tan(da/4)
   const double sin75 = sin(degreesToRadians(75.));
   const double cos75 = cos(degreesToRadians(75.));
   const double sin15 = sin(degreesToRadians(15.));
   const double cos15 = cos(degreesToRadians(15.));

   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);

   // Make the segment ends parallel, rather than tapered like the standard camcad thermal

   double startInnerX = sin15 * innerRadius;
   double startInnerY = cos15 * innerRadius;
   double startOuterX = startInnerX;
   double startOuterY = startInnerY + thk;

   double endInnerX = sin75 * innerRadius;
   double endInnerY = cos75 * innerRadius;
   double endOuterX = endInnerX + thk;
   double endOuterY = endInnerY;

   double cx, cy, sa, da;
   int res = ArcFromStartEndRadius(startOuterX, startOuterY, endOuterX, endOuterY,outerRadius, FALSE, TRUE, &cx, &cy, &sa, &da);

   double bulge2 = tan(da/4.);

   // top left arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( -(startOuterX), +(startOuterY),  bulge2);
   getLegacyGraph().graphVertex( -(endOuterX),   +(endOuterY),    0.);
   getLegacyGraph().graphVertex( -(endInnerX),   +(endInnerY),   -bulge);
   getLegacyGraph().graphVertex( -(startInnerX), +(startInnerY),  0.);
   getLegacyGraph().graphVertex( -(startOuterX), +(startOuterY),  0.);

   // top right arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( +(startOuterX), +(startOuterY), -bulge2);
   getLegacyGraph().graphVertex( +(endOuterX),   +(endOuterY),    0.);
   getLegacyGraph().graphVertex( +(endInnerX),   +(endInnerY),    bulge);
   getLegacyGraph().graphVertex( +(startInnerX), +(startInnerY),  0.);
   getLegacyGraph().graphVertex( +(startOuterX), +(startOuterY),  0.);

   // bottom left arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( -(startOuterX), -(startOuterY), -bulge2);
   getLegacyGraph().graphVertex( -(endOuterX),   -(endOuterY),    0.);
   getLegacyGraph().graphVertex( -(endInnerX),   -(endInnerY),    bulge);
   getLegacyGraph().graphVertex( -(startInnerX), -(startInnerY),  0.);
   getLegacyGraph().graphVertex( -(startOuterX), -(startOuterY),  0.);

   // bottom right arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( +(startOuterX), -(startOuterY),  bulge2);
   getLegacyGraph().graphVertex( +(endOuterX),   -(endOuterY),    0.);
   getLegacyGraph().graphVertex( +(endInnerX),   -(endInnerY),   -bulge);
   getLegacyGraph().graphVertex( +(startInnerX), -(startInnerY),  0.);
   getLegacyGraph().graphVertex( +(startOuterX), -(startOuterY),  0.);

   getLegacyGraph().graphBlockOff();

   return 1;
}



/****************************************************************************/
/*
*/
int padform_2_web_round_thermal()
{
   int   id = cur_ident;
   
   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  

   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   double tie_leg_width = G.tie_leg_width;
   if (tie_leg_width <= 0.0)
      tie_leg_width = G.xdim * .20; // default to 20% of width for gap
   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = G.xdim * .15; // default to 15% of width for thickness

   double thk = thermal_clearance;
   double outerRadius = G.xdim / 2.;
   double innerRadius = outerRadius - thk;

   double c = innerRadius;
   double a = tie_leg_width / 2.;
   double b = sqrt( (c * c) - (a * a) ); 

   //double bulge = tan(degreesToRadians(150.) / 4.);  // bulge = tan(da/4)
   //const double sin75 = sin(degreesToRadians(75.));
   //const double cos75 = cos(degreesToRadians(75.));

   // Make the segment ends parallel, rather than tapered like the standard camcad thermal

   double innerX = b;  //sin75 * innerRadius;
   double innerY = a;  //cos75 * innerRadius;
   double outerX = innerX + thk;
   double outerY = innerY;

   double cx, cy, sa, daOuter, daInner;
   int res = ArcFromStartEndRadius(innerX, innerY, -innerX, innerY,innerRadius, FALSE, TRUE, &cx, &cy, &sa, &daInner);
   res = ArcFromStartEndRadius(outerX, outerY, -outerX, outerY,outerRadius, FALSE, TRUE, &cx, &cy, &sa, &daOuter);

   double bulgeInner = tan(daInner/4.);
   double bulgeOuter = tan(daOuter/4.);

   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);

   // top arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( +(outerX), +(outerY), bulgeOuter);
   getLegacyGraph().graphVertex( -(outerX), +(outerY), 0.);
   getLegacyGraph().graphVertex( -(innerX), +(innerY), -bulgeInner);
   getLegacyGraph().graphVertex( +(innerX), +(innerY), 0.);
   getLegacyGraph().graphVertex( +(outerX), +(outerY), 0.);

   // top arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex( +(outerX), -(outerY), -bulgeOuter);
   getLegacyGraph().graphVertex( -(outerX), -(outerY), 0.);
   getLegacyGraph().graphVertex( -(innerX), -(innerY), bulgeInner);
   getLegacyGraph().graphVertex( +(innerX), -(innerY), 0.);
   getLegacyGraph().graphVertex( +(outerX), -(outerY), 0.);

/*
   // This makes ends that are on vector from center, makes a sort of wedge shape.
   // This is the way the camcad built-in thermal does it.
   // But pictures from Expedition show parallel ends, like is done above.

   // top arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(+(sin75 * outerRadius), +(cos75 * outerRadius), bulge);
   Graph_Vertex(-(sin75 * outerRadius), +(cos75 * outerRadius), 0.);
   Graph_Vertex(-(sin75 * innerRadius), +(cos75 * innerRadius), -bulge);
   Graph_Vertex(+(sin75 * innerRadius), +(cos75 * innerRadius), 0.);
   Graph_Vertex(+(sin75 * outerRadius), +(cos75 * outerRadius), 0.);

   // bottom arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(+(sin75 * outerRadius), -(cos75 * outerRadius), -bulge);
   Graph_Vertex(-(sin75 * outerRadius), -(cos75 * outerRadius), 0.);
   Graph_Vertex(-(sin75 * innerRadius), -(cos75 * innerRadius), bulge);
   Graph_Vertex(+(sin75 * innerRadius), -(cos75 * innerRadius), 0.);
   Graph_Vertex(+(sin75 * outerRadius), -(cos75 * outerRadius), 0.);
*/
   getLegacyGraph().graphBlockOff();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_2_web_round_thermal_45()
{
   int   id = cur_ident;
   
   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  

   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   ///double tie_leg_width = G.tie_leg_width;
   ///if (tie_leg_width <= 0.0)
   ///   tie_leg_width = G.xdim * .20; // default to 20% of width for gap
   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = G.xdim * .15; // default to 15% of width for thickness

   double thk = thermal_clearance;

   double outerRadius = G.xdim / 2.;
   double innerRadius = outerRadius - thk;

   // Pad is built camcad Thermal style, thickness from cad is used
   // but not tie leg width. Pattern from Apertur2.cpp.

   // Orignially 60° arcs, therfore 30° between them, coalescing 4 segs into 2 segs (made
   // from massaging standard camcad thermal)
   const double bulge = tan(degreesToRadians(60. + 30. + 60.)/4.); // bulge = tan(da/4) ; da = 150°
   const double sin30 = 0.5; 
   const double cos30 = 0.866;

   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);

   // top/left arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(+(sin30 * outerRadius), +(cos30 * outerRadius), bulge);
   getLegacyGraph().graphVertex(-(cos30 * outerRadius), -(sin30 * outerRadius), 0.);
   getLegacyGraph().graphVertex(-(cos30 * innerRadius), -(sin30 * innerRadius), -bulge);
   getLegacyGraph().graphVertex(+(sin30 * innerRadius), +(cos30 * innerRadius), 0.);
   getLegacyGraph().graphVertex(+(sin30 * outerRadius), +(cos30 * outerRadius), 0.);

   // bottom/right arc
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);
   getLegacyGraph().graphVertex(-(sin30 * outerRadius), -(cos30 * outerRadius), bulge);
   getLegacyGraph().graphVertex(+(cos30 * outerRadius), +(sin30 * outerRadius), 0.);
   getLegacyGraph().graphVertex(+(cos30 * innerRadius), +(sin30 * innerRadius), -bulge);
   getLegacyGraph().graphVertex(-(sin30 * innerRadius), -(cos30 * innerRadius), 0.);
   getLegacyGraph().graphVertex(-(sin30 * outerRadius), -(cos30 * outerRadius), 0.);


   getLegacyGraph().graphBlockOff();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_chamfered_rectangle()
{
   int   id = cur_ident;
   double x, y;
   

   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
 
   x = G.xdim / 2.;
   y = G.ydim / 2.;
   double offset = G.chamfer;

   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);

   getLegacyGraph().graphVertex(-x, -y + offset, 0.);
   getLegacyGraph().graphVertex(-x,  y - offset, 0.);
   getLegacyGraph().graphVertex(-x + offset,  y, 0.);
   getLegacyGraph().graphVertex( x - offset,  y, 0.);
   getLegacyGraph().graphVertex( x,  y - offset, 0.);
   getLegacyGraph().graphVertex( x, -y + offset, 0.);
   getLegacyGraph().graphVertex( x - offset, -y, 0.);
   getLegacyGraph().graphVertex(-x + offset, -y, 0.);
   getLegacyGraph().graphVertex(-x, -y + offset, 0.);

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
int padform_round_finger()
{
   int   id = cur_ident;
   double x, y;
   double offset;
   

   G.geomtyp = apertureComplex;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,usr_name, -1, 0);

   G.diameter = 0;
   G.level = getCamCadData().getDefinedFloatingLayer().getName();
   G.lyrnr = graphLayer(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.;
   y = G.ydim / 2.;
   
   getLegacyGraph().graphPolyStruct(G.lyrnr, 0L, 0);
   getLegacyGraph().graphPoly(NULL,  0, 1, 0, 1);

   // NOTE there was no legitimate sample of ROUND_FINGER cad data provided
   // as of this writing. Test data was contrived and hand built, and is therefore
   // suspect. In other words, this implementation is based on speculation and
   // has not been legitimately tested because no legitimate data sample has
   // been provided.

#define DEFINE_ROUND_END_UP
#ifdef DEFINE_ROUND_END_UP
   offset = x; // xdim is diameter, so x is radius
   getLegacyGraph().graphVertex(-x, -y, 0.);
   getLegacyGraph().graphVertex(-x,  y - offset, -1.);
   getLegacyGraph().graphVertex( x,  y - offset,  0.);
   getLegacyGraph().graphVertex( x, -y, 0.);
   getLegacyGraph().graphVertex(-x , -y, 0.);
#else
   // Define round end to right
   // This is what the docs suggest, but round-end-up is what seems to work
   offset = y; // ydim is diameter, so y is radius
   getLegacyGraph().graphVertex(-x,  y, 0.);
   getLegacyGraph().graphVertex( x - offset,  y, -1.);
   getLegacyGraph().graphVertex( x - offset, -y,  0.);
   getLegacyGraph().graphVertex(-x, -y, 0.);
   getLegacyGraph().graphVertex(-x,  y, 0.);
#endif

   getLegacyGraph().graphBlockOff();

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_round()
{
   int   id = cur_ident;

   G.geomtyp = apertureRound;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_round_donut()
{
   int   id = cur_ident;

   G.geomtyp = apertureDonut;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_thermal()
{
   int   id = cur_ident;

   G.geomtyp = apertureThermal;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int hole_option()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_rectangle()
{
   int   id = cur_ident;

   G.geomtyp = apertureRectangle;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padform_oblong()
{
   int   id = cur_ident;

   G.geomtyp = apertureOblong;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int padgeom_width()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.xdim = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
static int padgeom_height()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.ydim = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int padgeom_chamfer()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.chamfer = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
static int padgeom_tie_leg_width()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.tie_leg_width = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
static int padgeom_thermal_clearance()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.thermal_clearance = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
static int padform_offset()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.xoff = cnv_unit(atof(lp));
   lp = strtok(NULL,WORD_DELIMETER);
   G.yoff = cnv_unit(atof(lp));

   // Case 1583, Apparantly vbascii offsets are expressed in an opposite notion from
   // camcad conventions, so flip the sign on the offsets.
   G.xoff = -G.xoff;
   G.yoff = -G.yoff;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_rot()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.rotation = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int part_cell_rot()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.comp_rotation = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int part_cell_mir()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   
   if (STRCMPI(lp,"LYR_1"))   // if not on lyr_1
      G.comp_mirror = TRUE;
   else
      G.comp_mirror = FALSE;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_mir()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   
   if (!STRCMPI(lp,"YES"))
      G.mirror = TRUE;
   else
      G.mirror = FALSE;

   return 1;
}

/****************************************************************************/
/*
   MNT_SIDE
   OPP_SIDE
*/
static int vb99_side()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   
   if (STRCMPI(lp,"MNT_SIDE"))   // if not top
   {
      G.sidebot = TRUE;
      if (cur_status == FILE_SILK_ARTWORK)
      {
         G.level = "SILKSCREEN_BOTTOM";
         G.lyrnr = graphLayer("SILKSCREEN_BOTTOM", "" , 0);
      }
   }
   else
   {
      G.sidebot = FALSE;
      if (cur_status == FILE_SILK_ARTWORK)
      {
         G.level = "SILKSCREEN_TOP";
         G.lyrnr = graphLayer("SILKSCREEN_TOP", "" , 0);
      }
   }
   return 1;
}

/****************************************************************************/
/*
   TOP BOTTOM
*/
static int vb99_facement()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   
   if (STRCMPI(lp,"TOP"))  // if not top
   {
      G.sidebot = TRUE;
   }
   else
   {
      G.sidebot = FALSE;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_fill()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   cur_fill  = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
static int vb99_shape_options()
{
   char  *lp;

   if ((lp = strtok(NULL,WORD_DELIMETER)) == NULL) return 1;

   if (!STRCMPI(lp,"FILLED"))
      cur_fill  = TRUE;

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int graph_polyline_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
int   generated_plane_lyr()
{
   int   id = cur_ident;
   char     *lp;
   int   oldstat = cur_status;

   // it is allowed that text has no prosa
   lp = strword();
   G.level = lp;
   G.lyrnr = graphLayer(G.level,"",0); 

   go_command(plane_lyr_lst,SIZ_PLANE_LYR_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int   plane_lyr_plane_type()
{
   char     *lp;

   if (lp = strword())
   if (!STRCMPI(lp,"POSITIVE"))
   {
      cur_status = FILE_POS_PLANE;
   }
   else
   {
      cur_status = FILE_UNKNOWN;
   }
   return 1;
}

/****************************************************************************/
/*
*/
int   plane_lyr_plane_net()
{
   int   id = cur_ident;
   char     *lp;

   lp = strword(); // netname
   G.name = lp;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
int graph_polyline_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   cur_fill = 0;
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   // always close it
   if (polycnt)
   {
      if ((poly_l[0].x != poly_l[polycnt-1].x) || (poly_l[0].y != poly_l[polycnt-1].y))
      {
         if (polycnt < MAX_POLY)
         {
            poly_l[polycnt].x = poly_l[0].x;
            poly_l[polycnt].y = poly_l[0].y;
            poly_l[polycnt].bulge = poly_l[0].bulge;
            polycnt++;
         }
         else
         {
            fprintf(ferr,"Too many polylines\n");
            display_error++;
            return -1;
         }
      }
   }

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int graph_circle_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0;
   cur_fill = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = graphLayer(G.level,"",0);
   write_circle();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int graph_circle_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0;
   cur_fill = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = graphLayer(G.level,"",0);
   write_circle();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XYR
       LVL
       WDT
*/
static int graph_polyarc_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XYR
       LVL
       WDT
*/
static int graph_polyarc_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   // always close it
   if (polycnt)
   {
      if ((poly_l[0].x != poly_l[polycnt-1].x) || (poly_l[0].y != poly_l[polycnt-1].y))
      {
         if (polycnt < MAX_POLY)
         {
            poly_l[polycnt].x = poly_l[0].x;
            poly_l[polycnt].y = poly_l[0].y;
            poly_l[polycnt].bulge = poly_l[0].bulge;
            polycnt++;
         }
         else
         {
            fprintf(ferr,"Too many polylines\n");
            display_error++;
            return -1;
         }
      }
   }

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XYR
       LVL
       WDT
*/
static int graph_rect_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   if (polycnt)
   {
      double   x1 = poly_l[0].x, y1 = poly_l[0].y, x2 = poly_l[1].x, y2 = poly_l[1].y;

      polycnt = 5;
      poly_l[0].x = x1;
      poly_l[0].y = y1;
      poly_l[0].bulge = 0;
      poly_l[1].x = x2;
      poly_l[1].y = y1;
      poly_l[1].bulge = 0;
      poly_l[2].x = x2;
      poly_l[2].y = y2;
      poly_l[2].bulge = 0;
      poly_l[3].x = x1;
      poly_l[3].y = y2;
      poly_l[3].bulge = 0;
      poly_l[4].x = x1;
      poly_l[4].y = y1;
      poly_l[4].bulge = 0;

      write_poly();
   }

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XYR
       LVL
       WDT
*/
static int graph_rect_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   if (polycnt)
   {
      double   x1 = poly_l[0].x, y1 = poly_l[0].y, x2 = poly_l[1].x, y2 = poly_l[1].y;

      polycnt = 5;
      poly_l[0].x = x1;
      poly_l[0].y = y1;
      poly_l[0].bulge = 0;
      poly_l[1].x = x2;
      poly_l[1].y = y1;
      poly_l[1].bulge = 0;
      poly_l[2].x = x2;
      poly_l[2].y = y2;
      poly_l[2].bulge = 0;
      poly_l[3].x = x1;
      poly_l[3].y = y2;
      poly_l[3].bulge = 0;
      poly_l[4].x = x1;
      poly_l[4].y = y1;
      poly_l[4].bulge = 0;

      write_poly();
   }

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int poly_line()
{
   int   id = cur_ident;
   
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  text consists of
       XY
       LVL
       WDT
*/
static int graph_text()
{
   int      id = cur_ident;
   char     *lp;
   CString  prosa;

   //cur_filename, fpcnt

   // it is allowed that text has no prosa
   lp = strword();
   prosa = lp;

   polycnt = 0;
   G.mirror = 0;
   G.rotation = 0;
   G.text_type = "";
   G.textwidth = 0;  // according to VB ASCII desc. this is ignored.
   G.just = 0;

   go_command(text_lst,SIZ_TEXT_LST,id);

   G.textwidth = strlen(prosa) * G.height * (6.0 / 8.0);

   write_text(prosa);

   return 1;
}

/****************************************************************************/
/*
*/
static int text_text_type()
{
   int   id = cur_ident;
   char  *lp;

   // it is allowed that text has no prosa
   lp = strword();
   G.text_type = lp;

   return 1;
}

/****************************************************************************/
/*
  text consists of
       XY
       LVL
       WDT
*/
static int text_display_attr()
{
   int   id = cur_ident;

   go_command(graphtext_lst,SIZ_GRAPHTEXT_LST,id);

   return 1;
}

/****************************************************************************/
/*
  arc consists of
       PRI
       SEC
       XY
       ST_ANG
       SW_ANG
       LVL      
*/
static int poly_arc()
{
   int   id = cur_ident;
   
   go_command(grapharc_lst,SIZ_GRAPHARC_LST,id);

   int      p1 = polycnt;
   double   x1,y1,x2,y2,bulge;
   double   center_x = poly_l[polycnt-1].x;
   double   center_y = poly_l[polycnt-1].y;

   double   startangle = degreesToRadians(G.startangle);
   double   deltaangle = degreesToRadians(G.deltaangle);
   double   radius = G.radius;

   if (polycnt && ((polycnt -2) < MAX_POLY))
   {
      // check if arc has a G.deltangle of 360. If yes, it is a circle
      if (fabs(G.deltaangle - 360) < 1)
      {
         poly_l[polycnt-1].x = center_x-radius;
         poly_l[polycnt-1].y = center_y;
         poly_l[polycnt-1].bulge = 1;

         poly_l[polycnt].x = center_x+radius;
         poly_l[polycnt].y = center_y;
         poly_l[polycnt].bulge = 1;
         polycnt++;

         poly_l[polycnt].x = center_x-radius;
         poly_l[polycnt].y = center_y;
         poly_l[polycnt].bulge = 0;
         polycnt++;
      }
      else
      {  
         bulge = tan(deltaangle/4);
         x1 = center_x + radius * cos(startangle);
         y1 = center_y + radius * sin(startangle);
         x2 = center_x + radius * cos(startangle+deltaangle);
         y2 = center_y + radius * sin(startangle+deltaangle);

         poly_l[polycnt-1].x = x1;
         poly_l[polycnt-1].y = y1;
         poly_l[polycnt-1].bulge = bulge;
   
         poly_l[polycnt].x = x2;
         poly_l[polycnt].y = y2;
         poly_l[polycnt].bulge = 0;
         polycnt++;
      }
   }
   else
   {
      fprintf(ferr,"Too many polylines\n");
      display_error++;
      return -1;
   }

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  circle consists of
       XY
       DIA
       LVL
*/
static int poly_circle()
{
   int   id = cur_ident;
   DbFlag   flg = 0L;
   
   //polycnt = 0;
   cur_fill = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   if (cur_status != FILE_UNKNOWN)
   {
      write_circle();
   }
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int write_arc()
{
   int widthindex;

   //if (G.diameter == 0)
   //{
   //   widthindex = Graph_Aperture("", apertureRound, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   //}
   //else
   //{
   //   widthindex = Graph_Aperture("", apertureRound, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   //}

   getCamCadData().getDefinedAperture("",apertureRound,G.diameter,0.,0.,0.,0.,-1,BL_WIDTH,0,&widthindex);

   double   startangle = degreesToRadians(G.startangle);
   double   deltaangle = degreesToRadians(G.deltaangle);

   if (fabs(G.deltaangle - 360) < 1)
   {
      getLegacyGraph().graphCircle(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 0L, widthindex , FALSE, TRUE); 
   }
   else
   {
      getLegacyGraph().graphArc(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, startangle, deltaangle, 0L, widthindex , FALSE);
   }

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int write_circle()
{
   int widthindex;

   //if (G.diameter == 0)
   //{
   //   widthindex = Graph_Aperture("", apertureRound, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   //}
   //else
   //{
   //   widthindex = Graph_Aperture("", apertureRound, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   //}

   getCamCadData().getDefinedAperture("",apertureRound,G.diameter,0.,0.,0.,0.,-1,BL_WIDTH,0,&widthindex);

   DataStruct *d = getLegacyGraph().graphCircle(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 0L, widthindex , FALSE, cur_fill); 

   if (cur_status == FILE_ROUTE_OUTL)
   {
      d->setGraphicClass(graphicClassRouteKeepIn);
   }
   else if (cur_status == FILE_BOARD_OUTL)
   {
      d->setGraphicClass(graphicClassBoardOutline);
   }
   else if (cur_status == FILE_ASSM_OUTL)
   {
      d->setGraphicClass(graphicClassComponentOutline);
   }
   else if (cur_status == FILE_ROUTE_OBS)
   {
      d->setGraphicClass(graphicClassRouteKeepOut);
   }
   else if (cur_status == FILE_PLACE_OBS)
   {
      d->setGraphicClass(graphicClassPlaceKeepOut);
   }
   else if (cur_status == FILE_VIA_OBS)
   {
      d->setGraphicClass(graphicClassViaKeepOut);
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int write_text(const char *tmp)
{

   double   x, y, len;
   bool prop = true;   // veribest text is proportional
                           // also a text H125 W125 seems to be H125 W100

   x = poly_l[0].x;
   y = poly_l[0].y;
   len = G.textwidth;

   if (len < G.height)
   {
      len = strlen(tmp) * G.height*TEXT_CORRECT;
   }

   HorizontalPositionTag textAlign = horizontalPositionCenter;
   VerticalPositionTag lineAlign = verticalPositionBaseline;

   if (G.just & GRTEXT_W_C)
   {
      textAlign = horizontalPositionCenter;
   }
   else if (G.just & GRTEXT_W_R)
   {
      textAlign = horizontalPositionRight;
   }

   if (G.just & GRTEXT_H_T)
   {
      lineAlign = verticalPositionTop;
   }
   else if (G.just & GRTEXT_H_C)
   {
      lineAlign = verticalPositionCenter;
   }
   else if (G.just & GRTEXT_H_B)
      lineAlign = verticalPositionBottom;

   //normalize_text(&x, &y, G.just, G.rotation, G.mirror, G.height, len);

   if (!G.text_type.CompareNoCase("PARTNO"))
   {
      G.name_1 = tmp;
      G.name_1.MakeUpper();
      int laynr = graphLayer(G.level,"",0); 

      if (G.getAttributesPtr() != NULL)
      {
         if (polycnt != 0)
         {
            //doc->SetUnknownVisAttrib(G.AttribMap, ATT_TYPELISTLINK, G.name_1, x, y, degreesToRadians(G.rotation), G.height, 
            //   G.height*TEXT_CORRECT, prop, G.mirror, TRUE, SA_OVERWRITE, 0,laynr, 0, textAlign, lineAlign);
            CAttribute* attribute = G.getAttributes()->setAttribute(getCamCadData(),standardAttributeTypeListLink,G.name_1);
            attribute->getDefinedAttributeNodeList().addNewNode(x,y,degreesToRadians(G.rotation),G.height,G.height*TEXT_CORRECT,
               getCamCadData().getSmallWidthIndex(),laynr,true,prop,false,false,0,textAlign,lineAlign);
         }
         else
         {
            //doc->SetUnknownAttrib(G.AttribMap, ATT_TYPELISTLINK, G.name_1,SA_OVERWRITE, NULL); // x, y, rot, height
            G.getAttributes()->setAttribute(getCamCadData(),standardAttributeTypeListLink,G.name_1);
         }
      }
   }
   else if (!G.text_type.CompareNoCase("REF_DES"))
   {
      G.ref_des = tmp;
      G.ref_des.MakeUpper();

      if (!G.ref_des.CompareNoCase("REF DES") || !G.ref_des.CompareNoCase("REFDES") )
         return 1;

      if (G.comp_mirror)
      {
         if (!STRCMPI(G.level,"ASSEMBLY_BOTTOM"))
            G.level = "ASSEMBLY_TOP";
         else if (!STRCMPI(G.level,"ASSEMBLY_TOP"))
            G.level = "ASSEMBLY_BOTTOM";
         else if (!STRCMPI(G.level,"SILKSCREEN_BOTTOM"))
            G.level = "SILKSCREEN_TOP";
         else if (!STRCMPI(G.level,"SILKSCREEN_TOP"))
            G.level = "SILKSCREEN_BOTTOM";

         G.mirror = 0;
      }

      int laynr = graphLayer(G.level,"",0); 

      if (G.getAttributesPtr() != NULL)
      {
         CString keyword = Get_Next_Refname(G.getAttributes());
         
         if (polycnt)
         {
            x = x - G.comp_x;
            y = y - G.comp_y;

            double xoff = x, yoff = y;

            if (G.comp_mirror)
            {
               // Need to subtract by 360 because in VBAscii when a component is mirror,
               // the text rotation is given in clockwise instead of counter clockwise.
               // And we need to subtract another 180 to account for mirroring.
               G.rotation = (360 - G.rotation) - (360 - G.comp_rotation) - 180;

               Rotate(x, y, 360-G.comp_rotation-180, &xoff, &yoff);
               xoff = -xoff;
            }
            else
            {
               G.rotation = G.rotation - G.comp_rotation;
               Rotate(x, y, 360-G.comp_rotation, &xoff, &yoff);
            }

            while (G.rotation >= 360)
               G.rotation -= 360;
            while (G.rotation < 0)
               G.rotation += 360;

            //doc->SetUnknownVisAttrib(G.AttribMap, keyword, G.ref_des, xoff, yoff, degreesToRadians(G.rotation), G.height, 
            //   G.height*TEXT_CORRECT, prop, G.mirror, TRUE, SA_OVERWRITE, 0,laynr, 0, textAlign, lineAlign);

            int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex(keyword,valueTypeString);
            CAttribute* attribute = G.getAttributes()->setAttribute(getCamCadData(),keywordIndex,G.ref_des);

            if (attribute == NULL)
            {
               int iii = 3;
            }

            //CString nameValue = attribute->getNameValue();

            attribute->getDefinedAttributeNodeList().addNewNode(xoff,yoff,degreesToRadians(G.rotation),G.height,G.height*TEXT_CORRECT,getCamCadData().getSmallWidthIndex(),laynr,
               true,prop,false,false,0,textAlign,lineAlign);
         }
         else
         {
            //doc->SetUnknownAttrib(G.AttribMap, keyword, G.ref_des, SA_RETURN, NULL); // SA_RETURN for refname, so that it does not overwrite the placed one.
            int keywordIndex = getCamCadData().getKeyWordArray().getDefinedKeywordIndex(keyword,valueTypeString);
            curtype->attributes().setAttribute(getCamCadData(),keywordIndex,G.ref_des);
         }
      }
   }
   else if (!G.text_type.CompareNoCase("USER_TYPE"))
   {
      if (polycnt && strlen(tmp))
      {
         int laynr = graphLayer(G.level,"",0); 
         getLegacyGraph().graphText(laynr,tmp, x, y,G.height,G.height*TEXT_CORRECT, degreesToRadians(G.rotation), 0L, prop, G.mirror, 0, FALSE, -1, 0, textAlign, lineAlign); 
      }
   }
   else if (!G.text_type.CompareNoCase("DRILL_DRAWING"))
   {
      if (polycnt && strlen(tmp))
      {
         int laynr = graphLayer(G.level,"",0); 
         getLegacyGraph().graphText(laynr,tmp, x, y,G.height,G.height*TEXT_CORRECT, degreesToRadians(G.rotation), 0L, prop, G.mirror, 0, FALSE, -1, 0, textAlign, lineAlign); 
      }
   }
   else
   {
      fprintf(ferr, "Unknown Text_Type [%s] in [%s] at %ld\n", G.text_type, cur_filename, getInputFileLineNumber());
      display_error++;
   }

   polycnt = 0;
   return 1;
}

/******************************************************************************
* Get_Next_Refname
*/
CString Get_Next_Refname(CAttributes* map)
{
   CString refname;
   int count = 0;

   if (map != NULL)
   {      
      for (POSITION pos = map->GetStartPosition();pos != NULL;)
      {
         WORD keywordIndex;
         CAttribute* attrib;

         map->GetNextAssoc(pos, keywordIndex, attrib);

         //CString tmpKeyword = doc->getKeyWordArray()[keyword]->cc;
         CString tmpKeyword = getCamCadData().getKeyWordArray().getAt(keywordIndex)->getCCKeyword();

         if (tmpKeyword.Find("REFNAME") > -1)
         {
            count++;
         }
      }
   }

   if (count)
      refname.Format("%s_%d", "REFNAME_", count);
   else
      refname = "REFNAME";

   return refname;
}
 
/****************************************************************************/
/*
  Check if a line ends with \n, meaning that the whole line was read.
*/
void check_buf(char *b)
{
   if (strlen(b) == 0)  return;

   if (b[strlen(b)-1] != '\n')
   {
      fprintf(ferr,"Error, Line at %ld in [%s] not completely read.\n",getInputFileLineNumber(),cur_filename);  
      fprintf(ferr,"The max number of characters per line is %ld.\n", strlen(b));  
      display_error++;
   }
   else
   {
      b[strlen(b)-1] = '\0';
   }

   return;
}

/****************************************************************************/
/*
*/
static int overwrite_pad()
{
   int   ptr;
   char  padname[80];

   return 1; // I do not think that this is needed anymore for VB99 and 2000.
             // you can not overwrite a padstack anymore according to the VB Ascii spec.

   if (curpad_cnt == 0)  return 1;

   ptr = update_localpad(padname);

   if (lastpininsert)
   {
      //lastpininsert->getInsert()->setBlockNumber(doc->Get_Block_Num(padname, -1, 0));
      BlockStruct& padBlock = getCamCadData().getBlockDirectory().getDefinedBlock(padname,-1,blockTypeUnknown);
      lastpininsert->getInsert()->setBlockNumber(padBlock.getBlockNumber());
   }

   curpad_cnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int update_localpad(char *pn)
{
   int   i, ii, iii;

   for (i=0;i<localpadcnt;i++)
   {
      int found = TRUE;

      // to find an already defined pad, not only every layer must match,
      // but also the total number of layers.
      if (curpad_cnt != localpad[i].curpadcnt) 
      {
         continue;
      }

      int equal = 0; // there can be just 1 layer the same
      for (ii=0;ii<localpad[i].curpadcnt;ii++)
      {

         for (iii=0;iii<curpad_cnt;iii++)
         {
            // if contens is equal
            if (localpad[i].curpad[ii].layerptr == curpad[iii].layerptr &&
                !STRCMPI(localpad[i].curpad[ii].padgeomname, curpad[iii].padgeomname))
            {
                equal++;
            }
         }
      }

      if (equal != curpad_cnt)   found = FALSE;

      if (found)
      {
         sprintf(pn,"XPAD_%d",i);
         return i;
      }
   }

   sprintf(pn,"XPAD_%d",localpadcnt);
   if (localpadcnt < MAX_LOCALPAD)
   {
      localpad[localpadcnt].curpadcnt = curpad_cnt;
      if ((localpad[localpadcnt].curpad = (CurPad *)calloc(curpad_cnt,sizeof(CurPad))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

      memcpy(localpad[localpadcnt].curpad,curpad,curpad_cnt * sizeof(CurPad));

      BlockStruct *curblock = getLegacyGraph().graphBlockOn(graphBlockOnMethodAppend,pn,-1,0);
      curblock->setBlockType(blockTypePadstack);

      int   xpadflg = 0;
      int   smd = TRUE;
      // also watch out for rotation
      for (i=0;i<curpad_cnt;i++)
      {
         if (strlen(curpad[i].padgeomname))
         {
            if (!curpad[i].smd)  smd = FALSE;

            getLegacyGraph().graphBlockReference(curpad[i].padgeomname, NULL, 0, 0.0, 0.0, 0.0, 0 , 1.0, curpad[i].layerptr, TRUE);

            LayerStruct *l = getCamCadData().getLayer(curpad[i].layerptr);
            // here do SMT detect
            if (!STRNICMP(l->getName(),"PadLayer",strlen("PadLayer")))
            {
               int lnr = atoi(l->getName().Right((int)strlen(l->getName()) - (int)strlen("PadLayer")));

               if (lnr == 1)
                  xpadflg |= 1;
               else if (lnr == NumberOfLayers)
                  xpadflg |= 2;
               else
                  xpadflg |= 4;
            }
         }
      }

      // here check for SMD
      if (xpadflg == 1 || smd)      // Top SMD
      {
         //doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),VT_NONE, NULL, SA_OVERWRITE, NULL); //  
         curblock->attributes().setAttribute(getCamCadData(),standardAttributeSmd);
      }
      else if (xpadflg == 2)    // Bottom only
      {
         //doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE),VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height
         curblock->attributes().setAttribute(getCamCadData(),standardAttributeBlind);
      }

      getLegacyGraph().graphBlockOff();

      localpadcnt++;
   }
   else                        
   {
      fprintf(ferr,"Too many local pads\n");
      display_error++;
      return -1;
   }

   return localpadcnt-1;
}

/****************************************************************************/
/*
*/
static int read_netfile(FileStruct *fl)
{
   char     buf[MAX_NETLINE];
   char     ttmp[MAX_NETLINE];
   char     *lp;
   int      netlist = FALSE;
   CString  netname;
   char     compname[80];
   char     pinname[80];
   NetStruct   *n = NULL;

   resetInputFileLineNumber();
   G.push_tok = 0;

   while (fgets(buf,MAX_NETLINE,cur_fp))
   {
      strcpy(ttmp,buf);
      check_buf(buf);
      incrementInputFileLineNumber();
      if ((lp = strtok(buf,WORD_DELIMETER)) == NULL)  continue;

      if (lp[0] == '.')                      continue;
      if (lp[0] == ';')                      continue;

      if (lp[0] == '%')
      {
         if (!STRCMPI(lp,"%PART"))
            netlist = FALSE;
         if (!STRCMPI(lp,"%NET"))
            netlist = TRUE;
         continue;
      }
      if (netlist)
      {
         if (lp[0] != '*')
         {
            if (lp[0] == '\\')
               lp = strtok(ttmp,"\\");
            else
            {
               fprintf(ferr," Use Tecfile .OLDNETLIST y for this type of netlist.\n");
               display_error++;
            }
            netname = lp;
            netname.TrimLeft();
            netname.TrimRight();

            //n = add_net(fl,netname);
            n = fl->getNetList().addNet(netname);
         }

         // some netlist are \C1\-\1\
         // others are C1-1
         while ((lp = strtok(NULL,"\\ \t\n")) != NULL)
         {
            strcpy(compname,lp);

            if ((lp = strtok(NULL,"\\ \t-")) == NULL)
            {
               fprintf(ferr,"Error in Netlist file [%s] at %ld\n", cur_filename,getInputFileLineNumber());
               display_error++;
               return -1;
            }

            strcpy(pinname, lp);

            //add_comppin(fl, n, compname, pinname);
            n->addCompPin(compname,pinname);
         }
      }
   }

   return 1;
}


/******************************************************************************
//* get_layertype 
*/
//static int get_layertype(const char *layerType)
//{
//   for (int i=0;i<MAX_LAYTYPE;i++)
//   {
//      if (!_STRICMP(layerTypeToString(i), layerType))
//         return i;
//   }
//
//   return LAYTYPE_UNKNOWN; 
//}

static ESideOption GetSideOption(CString sideOption)
{
   ESideOption sideOptionTag = sideOptionNone;

   if (sideOption.CompareNoCase("NONE") == 0)
      sideOptionTag = sideOptionNone;
   else if (sideOption.CompareNoCase("MNT_SIDE") == 0)
      sideOptionTag = sideOptionMountSide;
   else if (sideOption.CompareNoCase("OPP_SIDE") == 0)
      sideOptionTag = sideOptionOppSide;
   else if (sideOption.CompareNoCase("TOP") == 0)
      sideOptionTag = sideOptionTop;
   else if (sideOption.CompareNoCase("BOTTOM") == 0)
      sideOptionTag = sideOptionBottom;

   return sideOptionTag;
}

/******************************************************************************/
// CDcaVBPin
/******************************************************************************/
CDcaVBPin::CDcaVBPin(CString name, CString pinOption)
{
   m_name = name;
   m_pinOption = GetSideOption(pinOption);
}

CDcaVBPin::CDcaVBPin(const CDcaVBPin& other)
{
   if (&other != this)
   {
      m_name = other.m_name;
      m_pinOption = other.m_pinOption;
   }
}

CDcaVBPin::~CDcaVBPin()
{
}


/******************************************************************************/
// CDcaVBGeom
/******************************************************************************/
CDcaVBGeom::CDcaVBGeom(CString name, int geomNum)
{
   m_name = name;
   m_originalName = name;
   m_geomNum = geomNum;
   m_pinList.empty();
   m_deriveGeomList.empty();
   m_silkscreenSide = sideOptionNone;
   m_soldermaskSide = sideOptionNone;
}

CDcaVBGeom::~CDcaVBGeom()
{
   m_pinList.empty();
   m_deriveGeomList.empty();
}

void CDcaVBGeom::SetSilkscreenSide(const CString side)
{ 
   m_silkscreenSide = GetSideOption(side);
}

void CDcaVBGeom::SetSoldermaskSide(const CString side)   
{ 
   m_soldermaskSide = GetSideOption(side);   
}

CDcaVBPin* CDcaVBGeom::AddPin(CString pinName, CString pinOption)
{
   CDcaVBPin* pin = FindPin(pinName);
   if (pin == NULL)
   {
      pin = new CDcaVBPin(pinName, pinOption);
      m_pinList.AddTail(pin);
   }

   return pin;
}

CDcaVBPin* CDcaVBGeom::FindPin(CString pinName)
{
   POSITION pos = m_pinList.GetHeadPosition();
   while (pos)
   {
      CDcaVBPin* pin = m_pinList.GetNext(pos);
      if (pin == NULL)
         continue;

      if (pin->GetName().CompareNoCase(pinName) == 0)
         return pin;
   }

   return NULL;
}

bool CDcaVBGeom::IsDifference(CDcaVBGeom* otherVBGeom)
{
   if (m_originalName.CompareNoCase(otherVBGeom->m_originalName) != 0)
      return true;

   POSITION pos = otherVBGeom->m_pinList.GetHeadPosition();
   while (pos)
   {
      CDcaVBPin* otherPin = otherVBGeom->m_pinList.GetNext(pos);
      if (otherPin == NULL)
         continue;

      CDcaVBPin* geomPin = FindPin(otherPin->GetName());
      if (geomPin == NULL)
         return true;

      if (otherPin->GetPinOption() != geomPin->GetPinOption())
         return true;
   }

   if (m_silkscreenSide != otherVBGeom->m_silkscreenSide)
      return true;
   if (m_soldermaskSide != otherVBGeom->m_soldermaskSide)
      return true;

   return false;
}

void CDcaVBGeom::AddDerivedGeom(CDcaVBGeom* otherVBGeom)
{
   CDcaVBGeom* vbGeom = new CDcaVBGeom(otherVBGeom->m_name, otherVBGeom->m_geomNum);

   vbGeom->m_name = otherVBGeom->m_name;
   vbGeom->m_originalName = otherVBGeom->m_originalName;
   vbGeom->m_geomNum = otherVBGeom->m_geomNum;
   vbGeom->m_silkscreenSide = otherVBGeom->m_silkscreenSide;
   vbGeom->m_soldermaskSide = otherVBGeom->m_soldermaskSide;
   vbGeom->m_pinList.empty();
   vbGeom->m_deriveGeomList.empty();

   POSITION pos = otherVBGeom->m_pinList.GetHeadPosition();
   while (pos != NULL)
   {
      CDcaVBPin* otherPin = otherVBGeom->m_pinList.GetNext(pos);
      if (otherPin == NULL)
         continue;

      CDcaVBPin* newPin = new CDcaVBPin(*otherPin);
      vbGeom->m_pinList.AddTail(newPin);
   }

   m_deriveGeomList.AddTail(vbGeom);
}

CDcaVBGeom* CDcaVBGeom::CheckForDerivedGeom(CDcaVBGeom* otherVBGeom)
{
   POSITION pos = m_deriveGeomList.GetHeadPosition();
   while (pos != NULL)
   {
      CDcaVBGeom* vbGeom = m_deriveGeomList.GetNext(pos);
      if (vbGeom == NULL)
         continue;

      if (!vbGeom->IsDifference(otherVBGeom))
         return vbGeom;
   }

   return NULL;
}

CString CDcaVBGeom::GetDerivedGeomName()
{
   CString derivedName;
   derivedName.Format("%s_%d", this->m_name, this->m_deriveGeomList.GetCount()+1);
   return derivedName;
}


/******************************************************************************/
// CDcaVBGeomMap
/******************************************************************************/
CDcaVBGeomMap::CDcaVBGeomMap()
{
   lastAddedGeom = NULL;
   m_geomMap.empty();
}

CDcaVBGeomMap::~CDcaVBGeomMap()
{
   lastAddedGeom = NULL;
   m_geomMap.empty();
}

CDcaVBGeom* CDcaVBGeomMap::AddGeom(CString name, int geomNum)
{
   CDcaVBGeom* vbGeom = FindGeom(name);
   if (vbGeom == NULL)
   {
      vbGeom = new CDcaVBGeom(name, geomNum);
      m_geomMap.SetAt(vbGeom->GetName(), vbGeom);
   }
   lastAddedGeom = vbGeom;

   return vbGeom;
}

CDcaVBGeom* CDcaVBGeomMap::FindGeom(CString name)
{
   CDcaVBGeom* vbGeom = NULL;
   m_geomMap.Lookup(name, vbGeom);

   return vbGeom;
}

CDcaVBGeom* CDcaVBGeomMap::GetLastGeom()
{
   return lastAddedGeom;
}

void CDcaVBGeomMap::Empty()
{
   m_geomMap.empty();
}

//#endif // #ifdef EnableDcaVeriBestAsciiReader
