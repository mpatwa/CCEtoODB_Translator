
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

   Triangle graphic (padstack) are treated as Rectangle.
   Complex shape
   Ellipse
   Rotated pins   = only 90 degree rotation allowed
   local PART_DEF under PART_INSTANCE in design file.

   .PART_INSTANCE UNUSED will be skipped.

*/           

#include "stdafx.h"
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
#include "vb99in.h"
#include "CCEtoODB.h"
#include "FileGarbler.h"
#include "find.h"
#include <sys/stat.h>
#include <direct.h>
#include "RwUiLib.h"
#include <set>
using std::set;
// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)

#define LineStatusUpdateInterval 200

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress;
static CFileReadProgress*  fileReadProgress = NULL;
//extern LayerTypeInfoStruct layertypes[];
extern char *testaccesslayers[];

/* Static Variables *********************************************************/
static CCEtoODBDoc       *doc;
static FileStruct       *file = NULL;
static FileStruct       *fl = NULL;          // this is the current filepointer, either from file or
                                             // from an already loaded PCB file.
static int              PageUnits;
static double           unit_faktor;
static FILE             *cur_fp;             // this the current file pointer
static char *           OutFileBuffer;
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
static bool             cur_conductive_area_teardrop = false;
static bool             cur_conductive_area_plane = false;
static bool             isInGeneratedData;
static bool             thievingDataFlag = false;
static bool             planeShapeFlag = false;
static bool             apsHatchFlag = false;
static bool             apsTieLegFlag = false;
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
static Global           G;

static CString          contourComp;
static CString          contourDirection;
static CString          contourHole;

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
static bool             defaultTechnology;
static CVBGeomMap       VBGeomList;       
static CVBGeom*         curVBGeom;

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
static CVBNetContainerMap vbNetContainer;

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
int actual_plane = 0;
long cur_actual_plane_num = 0;


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
static CString Get_Next_PartNo(CAttributes* map);
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
static void ReadVB99Pcb(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits);

static void UpdateNegativeLayers();
static void UpdateNetZeroFlags(CNetList &netlist);

static int   keyin_netlist;
static int   netprops_netlist;
static int   layout_netlist;
static CString customDrillSymName;
static CString standardSymbolName;
static CString standardSymbolSize;
static CString CharacterSymbolName;
static CString CharacterSymbolSize;

static void DrawSquare();
static void DrawSmallSquare();
static void DrawCircle(double diameter);
static void DrawTriangle();
static void DrawInvertedTriangle();
static void DrawSmallTriangle();
static void DrawDiamond(double lwidth);
static void DrawHexagon();
static void DrawStarCircle();
static void DrawCircleStar();
static void DrawSquareStar();
static void DrawHollowX();
static void DrawHollowPlus();
static void DrawBowTie();
static void DrawDoubleCone();
static void DrawSquareSlash();
static void DrawCircleSlash();
static void DrawCenterVerticalLine();
static void DrawCenterHorizontalLine();
static void DrawShortCenterHorizontalLine();
static void DrawCustom();
#define kCos45                       0.7071067811865
#define kSin45                       0.7071067811865

#define kCos30                       0.8660254037844
#define kSin30                       0.5
double size = 0.0;
double x = 0.0, y = 0.0;
double halfSize = 0.0;
double xDrlsymCenter = 0.0, yDrlsymCenter = 0.0;
double xDelta = 0.0, yDelta = 0.0;
double pen = 0.0, width = 0.0, halfWidth = 0.0, doubleWidth = 0.0;
static CString Component_Options;
static VbHoleTypeTag vbHoleType = vbHoleTypeUndefined;
set<CString> skipCellList;
set<CString> padstacksWithDifferentReferenceNames;
CString VbHoleTypeTagToString(VbHoleTypeTag tagValue)
{
   const char* retval;

   switch (tagValue)
   {
   case vbHoleTypeMounting:  retval = "Mounting";  break;
   case vbHoleTypeShearing:  retval = "Shearing";  break;
   case vbHoleTypeTooling:   retval = "Tooling";   break;
   default:                  retval = "Unknown";   break;
   }

   return retval;
}

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
   fileReadProgress = new CFileReadProgress(file);

   resetInputFileLineNumber();
}

//_____________________________________________________________________________
static CDataList* s_polyarcShapeDataList = NULL;

CDataList* getPolyarcShapeDataList()
{
   if (s_polyarcShapeDataList == NULL)
   {
      s_polyarcShapeDataList = new CDataList(false);
   }

   return s_polyarcShapeDataList;
}

void releasePolyarchShapeDataList()
{
   delete s_polyarcShapeDataList;
   s_polyarcShapeDataList = NULL;
}

/******************************************************************************
* ReadVB99   (Camcad HKP VBASCII in)
*/
void ReadVB99(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   ReadVB99Pcb(pathname,Doc,Format,pageunits);
   FileStruct* panelSubFile = file;

   CVbPcbInstances& pcbInstances = CVbPcbInstances::getPcbInstances();

   if (pcbInstances.getCount() > 0)
   {
      panelSubFile->setBlockType(blockTypePanel);

      for (POSITION pos = pcbInstances.getHeadPosition();pos != NULL;)
      {
         CVbPcbInstance* pcbInstance = pcbInstances.getNext(pos);

         if (pcbInstance->getPcbInstanceData() == NULL)
         {
            CFilePath pcbDesignFilePath(pathname);
            CString localPath = pcbInstance->getLocalPath();
            CFilePath relativePcbDesignPath(localPath);

            while (! relativePcbDesignPath.getPath().IsEmpty())
            {
               CString directory = relativePcbDesignPath.popRoot();

               if (directory != ".")
               {
                  pcbDesignFilePath.pushLeaf(directory);
               }
            }

            //CString hkpPath = pcbInstance->getHkpPath();
            //pcbDesignFilePath.pushLeaf(hkpPath);

            CString pcbDesignPath = pcbDesignFilePath.getPath();

            ReadVB99Pcb(pcbDesignPath,Doc,Format,pageunits);
            FileStruct* pcbSubFile = file;
            pcbSubFile->setShow(false);

            pcbInstances.insertPcbInstances(*panelSubFile,*pcbSubFile,localPath);
         }
      }
   }

   CVbPcbInstances::release();
   releasePolyarchShapeDataList();
}

bool decryptHKP(CString m_strInputFileName)
{
   CString strErrorMessage;
   struct stat info;
   char nextChar;

   FILE *m_pInputFile;
   char *m_pFileBuffer;
   BOOL m_bForceDecryptedOutput = FALSE;
   BOOL m_bForceEncryptedOutput = FALSE;

   // Open the input file.
   if ( ( m_pInputFile = fopen( m_strInputFileName, "rb" ) ) == (FILE *)NULL )
   {
      CString tmp;

      tmp.Format("Can not open file [%s]", m_strInputFileName);
      fprintf(ferr, tmp);
      ErrorMessage(m_strInputFileName, "Can not open file.");
      return false;
   }

   // Determine the size of the input file.
   if ( stat( m_strInputFileName, &info ) != 0 )
   {
      CString tmp;

      tmp.Format("Can not stat file [%s]\n", m_strInputFileName);
      fprintf(ferr, tmp);
      ErrorMessage(m_strInputFileName, "Can not open file.");
      return false;
   }

   // Allocate a buffer to hold the entire file.
   size_t bufferSize = sizeof(char) * info.st_size;
   m_pFileBuffer = new char [ bufferSize ];
   if ( m_pFileBuffer == (char *)NULL )
   {
      CString tmp = "Memory allocation error";
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return false;
   }

   // Read the entire file in one shot.
   fread( m_pFileBuffer, sizeof(char), bufferSize, m_pInputFile );
   if ( ferror( m_pInputFile ) )
   {
      CString tmp;

      tmp.Format("Read error on file [%s]", m_strInputFileName);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return false;
   }

   // Read one more byte to make sure we read the entire file.
   fread( &nextChar, sizeof(nextChar), 1, m_pInputFile );
   if ( !feof( m_pInputFile ) )
   {
      CString tmp;

      tmp.Format("Read error on file [%s]", m_strInputFileName);
      fprintf(ferr, tmp);
      ErrorMessage(tmp);
      return false;
   }

   // Close the input file.
   fclose( m_pInputFile );
   m_pInputFile = (FILE *)NULL;

   // Initialize the decryption engine.
   fileGarbler::cDecrypt decryptionEngine;
   int headerLength = decryptionEngine.InitializeFromBuffer( bufferSize, m_pFileBuffer );

   // Determine if the input file is encrypted.
   if ( headerLength > 0 )
   {
      // The input file is encrypted. We need to decrypt the input file
      if ( m_bForceDecryptedOutput || !m_bForceEncryptedOutput )
      {
         // Allocate a new buffer without the encryption header.
         bufferSize = bufferSize - headerLength;
         char *tmpFileBuffer = new char [ bufferSize ];
         if (!tmpFileBuffer)
         {
            CString tmp = "Memory allocation error";
            fprintf(ferr, tmp);
            ErrorMessage(tmp);
            return false;
         }
         memcpy( tmpFileBuffer, m_pFileBuffer + headerLength, bufferSize );

         // Delete is causing the Release build to crash - get back to this later - sasharma
         // delete m_pFileBuffer;
         m_pFileBuffer = tmpFileBuffer;

         // Decrypt the entire file (less the header which we just removed).
         decryptionEngine.Hash( bufferSize, m_pFileBuffer );
      }
   }
   else
   {
      // The input file is not encrypted
      if ( m_bForceDecryptedOutput || !m_bForceEncryptedOutput )
      {
         char *tmpFileBuffer = new char [ bufferSize ];
         if (!tmpFileBuffer)
         {
            CString tmp = "Memory allocation error";
            fprintf(ferr, tmp);
            ErrorMessage(tmp);
            return false;
         }
         memcpy( tmpFileBuffer, m_pFileBuffer, bufferSize );

         // Delete is causing the Release build to crash - get back to this later - sasharma
         // delete m_pFileBuffer;
         m_pFileBuffer = tmpFileBuffer;
      }
   }

   // Copy the un-encrypted content into buffer
   OutFileBuffer = new char [ bufferSize + 1];
   memcpy(OutFileBuffer, m_pFileBuffer, bufferSize);
   OutFileBuffer[strlen(OutFileBuffer)] = EOF;

   return true;
}

void ReadVB99Pcb(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   file = NULL;
   doc = Doc;
   display_error = 0;
   PageUnits = pageunits;
   skipCellList.clear();
   padstacksWithDifferentReferenceNames.clear();
   CString vbLogFile = GetLogfilePath("vbascii.log");

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

   if (pathname[strlen(pathname)-1] != '\\')
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

   fl = file = Graph_File_Start(projectname, Type_VB99_Layout);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());
   cur_filenum = file->getFileNumber();

   // settings must be loaded after fileinit.
   CString settingsFile( getApp().getImportSettingsFilePath("vbascii.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nVB99 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_vb99settings(settingsFile);

   Graph_Level("0","",1);

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


   // Decrypt encrypted ASCII HKPs before reading in

   // jobprefs.hkp
   if(decryptHKP(validPathname + "JobPrefs.hkp"))
      do_jobprefs(validPathname);

   // padstack.hkp
   if(decryptHKP(validPathname + "Padstack.hkp"))
      do_padstack(validPathname);

   // cell.hkp
   if(decryptHKP(validPathname + "Cell.hkp"))
      do_cell(validPathname);

   // layout.hkp
   if(decryptHKP(validPathname + "Layout.hkp"))
      do_layout(validPathname);

   // pdb.hkp
   if(decryptHKP(validPathname + "PDB.hkp"))
      do_pdb(validPathname);

   // netlist
   // netclass.hkp
   // netprops.hkp

   if (keyin_netlist)   
   {
      do_netlist(validPathname);
   }
   else if (netprops_netlist)
   {
      if(decryptHKP(validPathname + "NetProps.hkp"))
         do_netprops(validPathname);
   }

   COperationProgress operationProgress;
   operationProgress.setLength(0.);

   operationProgress.updateStatus("Generate NC net");
   generate_NCnet(doc,file,&operationProgress);  // this function generates the NC (non connect) net.

   // mirror padstack layers.
   Graph_Level_Mirror(PAD_TOP, PAD_BOT, "");

   // here assign other layers
   do_assign_layers();

   // Fix negative geometries
   UpdateNegativeLayers();

   operationProgress.updateStatus("Generating padstack access flags");
   generate_PADSTACKACCESSFLAG(doc, 1,&operationProgress);

   operationProgress.updateStatus("Purging unused blocks");
   doc->purgeUnusedBlocks(); 

   operationProgress.updateStatus("Generating pin locations");
   doc->generatePinLocations(false,&operationProgress);

   operationProgress.updateStatus("Processing device types");
   operationProgress.setLength(fl->getTypeList().GetCount());

   // Check for device types that are derived and copy the attribute from the original device type
   WORD orignialTypeKey = (WORD)doc->RegisterKeyWord("ORIGINAL_DEVICE", 0, VT_STRING);
   
   for (POSITION pos = fl->getTypeList().GetHeadPosition();pos != NULL;)
   {
      TypeStruct *type = fl->getTypeList().GetNext(pos);

      operationProgress.incrementProgress();

      if (type == NULL || type->getAttributesRef() == NULL)
         continue;

      Attrib *attrib = NULL;

      if (type->getAttributesRef()->Lookup(orignialTypeKey, attrib) && attrib != NULL)
      {
#if CamCadMajorMinorVersion > 406  //  > 4.6
         CString originalTypeName = attrib->getStringValue();
#else
         CString originalTypeName = doc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
         TypeStruct *originalType = FindType(fl, originalTypeName);

         if (originalType != NULL)
            doc->CopyAttribs(&type->getAttributesRef(), originalType->getAttributesRef());
      }
   }

   operationProgress.updateStatus("Processing type attributes");
   RefreshTypetoCompAttributes(doc, SA_OVERWRITE);

   operationProgress.updateStatus("Cleaning up names");
   clean_original_name();

   if (do_normalize_bottom_build_geometries)
   {
      operationProgress.updateStatus("Normalizing bottom built geometries");
      doc->OnToolsNormalizeBottomBuildGeometries();
   }

   UpdateNetZeroFlags(file->getNetList());

   t = t.GetCurrentTime();
   fprintf(ferr,"%s\n",t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));

   fprintf(ferr,"Logfile closed\n");
   fclose(ferr);

   operationProgress.updateStatus("Freeing memory");
   global_free();

   delete fileReadProgress;
   fileReadProgress = NULL;

   if (display_error)
      Logreader(vbLogFile);
}

static void UpdateNetZeroFlags(CNetList &netlist)
{
   POSITION pos = netlist.GetHeadPosition();
   while(pos)
   {
      NetStruct *net = netlist.GetNext(pos);
      if(net)
      {
         CString netName = net->getNetName();
       
         //Set single comppin NetZero to "NETFLAG_UNUSEDNET and NETFLAG_SINGLEPINNET"
         if ((netName.Left(strlen(QTNETZERO)).MakeUpper() == QTNETZERO) &&
            net->getCompPinCount() == 1 && vbNetContainer.IsEmptyRoute(netName))
         {
            net->setFlagBits(NETFLAG_UNUSEDNET | NETFLAG_SINGLEPINNET);
         }//if
      }
   }/*while*/
}

/****************************************************************************
*/

static void UpdateNegativeLayers()
{
   // Case dts0100470139

   // Some geometries on a layer of type POWER_NEGATIVE should have the negative
   // flag set. The problem is that during processing many items on that layer
   // are encountered in advance of the PLANE_LYR definition which has the
   // LAYER_TYPE NEGATIVE. Since items are instantiated as we go, we do not
   // do not know they are to be negative at the time they are instantiated.

   // Items actually in the PLANE_LYR definition are not themselves negative, only
   // those that are outside it. So we need some way to tell after the fact which
   // items shold be negative and which positive. We can't tell by layer, they are
   // mixed on the same layer. So we need a flag.

   // Putting an attribute on the PLANE_LYR items and looking for it later, processing it,
   // thenremoving it, is a nuisance. So what we do is go ahead and set the Negative flag
   // on them. Then when we get here, we have Negative flags that are opposite of what
   // we ultimately want for items on the POWER_NEGATIVE layer. So we march through
   // all the items on POWER_NEGATIVE and flip their negative switches.


   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

      LayerStruct *layer = doc->FindLayer( data->getLayerIndex() );

      if (layer != NULL && layer->getLayerType() == layerTypePowerNegative)
      {
         data->setNegative( !data->isNegative() );
      }
   }
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
   padshapePrefix = "AP"; // Case dts0100452974, needs non-blank default, AP for Aperture

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp, "VBASCII Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         // here do it.
         if (!STRICMP(lp, ".UNNAMEDNET"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
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
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            compoutline = lp;
         }
         else if (!STRICMP(lp, ".USETECHNOLOGY"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            USE_TECHNOLOGY = lp;
         }
         else if (!STRICMP(lp, ".TESTPAD"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            testpadtop = !STRCMPI(lp, "TOP");
         }
         else if (!STRICMP(lp, ".CORRECT_PADSTACKUNITERROR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               correct_padstackuniterror = TRUE;
         }
         else if (!STRICMP(lp, ".USE_GENERATED_DATA"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               use_generated_data = FALSE;
         }
         else if (!STRICMP(lp, ".EXPLODE_DRAWING_CELL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               explode_drawing_cell = TRUE;
         }
         else if (!STRICMP(lp, ".USE_CELL_FANOUT_VIAS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               USE_CELL_FANOUT_VIAS = TRUE;
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
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
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString a1 = lp;
            a1.MakeUpper();
            a1.TrimLeft();
            a1.TrimRight(); 

            if ((lp = get_string(NULL, " \t\n")) == NULL)
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
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, TEST_ATTR);
         }
         else if (!STRICMP(lp, ".USE_UNIQUE_NETZERO"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'N')
               USE_UNIQUE_NETZERO = FALSE;
         }
         else if (!STRICMP(lp, ".NORMALIZE_BOTTOM_BUILD_GEOM"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (toupper(lp[0]) == 'Y')
               do_normalize_bottom_build_geometries = true;
         }
         else if (!STRICMP(lp, ".PADSHAPEPREFIX"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            padshapePrefix = lp;
         }
         else if (!STRICMP(lp, ".SKIP_CELL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString cellName = lp;
            cellName.TrimRight();
            cellName.TrimLeft();
            cellName.MakeUpper();
            skipCellList.insert(cellName);
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

   /*setInputFile(cur_fp);*/
   G.push_tok = 0;
   G.AttribMap = NULL;
   G.viaSpansSection = NULL;
   G.curViaSpan = NULL;

   go_command(jobprefs_lst, SIZ_JOBPREFS_LST, 0);
   G.AttribMap = NULL;

   /*fclose(cur_fp);*/
}

/******************************************************************************
* do_padstack
*/
static void do_padstack(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "padstack.hkp";

   //setInputFile(cur_fp);
   G.AttribMap = NULL;

   if (progress != NULL)
      progress->SetStatus(cur_filename);

   go_command(pad_lst, SIZ_PAD_LST, 0);
   G.AttribMap = NULL;

   /*fclose(cur_fp);*/
}

/******************************************************************************
* do_cell
*/
static void do_cell(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "cell.hkp";

   /*setInputFile(cur_fp);*/
   G.AttribMap = NULL;

   if (progress != NULL)
      progress->SetStatus(cur_filename);

   go_command(cel_lst,SIZ_CEL_LST,0);
   G.AttribMap = NULL;

   /*fclose(cur_fp);*/
}

/******************************************************************************
* do_layout
*/
static void do_layout(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "layout.hkp";

   /*setInputFile(cur_fp);*/
   G.push_tok = 0;
   G.AttribMap = NULL;
   G.layout_def = 0;

   if (progress != NULL)
      progress->SetStatus(cur_filename);

   go_command(des_lst,SIZ_DES_LST,0);
   G.AttribMap = NULL;
   G.layout_def = 0;

   /*fclose(cur_fp);*/
}

/******************************************************************************
* do_pdb
   this goes into the Tyeplist (DeviceList)
*/
static void do_pdb(CString pathName)
{
   cur_filename = pathName;
   cur_filename += "pdb.hkp";

   /*setInputFile(cur_fp);*/
   G.push_tok = 0;
   G.AttribMap = NULL;

   if (progress != NULL)
      progress->SetStatus(cur_filename);

   go_command(pdb_lst,SIZ_PDB_LST,0);
   G.AttribMap = NULL;

   /*fclose(cur_fp);*/
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

   /*setInputFile(cur_fp);*/
   G.push_tok = 0;

   if (progress != NULL)
      progress->SetStatus(cur_filename);
   
   read_netfile(file);
   
   /*fclose(cur_fp);*/
}

/******************************************************************************
* do_netprops
*/
static void do_netprops(CString pathName)
{

   cur_filename = pathName;
   cur_filename += "netprops.hkp";

   /*setInputFile(cur_fp);*/
   G.push_tok = 0;
   G.AttribMap = NULL;

   if (progress != NULL)
      progress->SetStatus(cur_filename);

   go_command(netprops_lst,SIZ_NETPROPS_LST,0);
   G.AttribMap = NULL;

   /*fclose(cur_fp);*/
}

char *getnextline(char * str, int num)
{
   int j = 0;
   char *cs;
   cs = str;

   while(--num > 0 && OutFileBuffer)
   {
      if(*OutFileBuffer == EOF || *OutFileBuffer == 10 || *OutFileBuffer == 13)
      {
         if(*OutFileBuffer == '\n')
         {
            //*cs++ = *OutFileBuffer;
            OutFileBuffer++;
            break;
         }
         else if(cs == str && *OutFileBuffer == EOF)
            return NULL;
         else if(*OutFileBuffer == EOF)
            break;
      }
      else
      {
         *cs++ = *OutFileBuffer;
      }
      OutFileBuffer++;
   }
   *cs = '\0';
   return str;
}

/******************************************************************************
* go_command
  return -1 for EOF
  return 1  for ident match
  return 0  for ???
*/
int go_command(List *lst, int siz_lst, int start_ident)
{
   char buf[MAX_LINE];
   char *tok;

   while (TRUE)
   {
      if (!G.push_tok)
      {
         //if ((fgets(buf,MAX_LINE, cur_fp)) == NULL)  return -1;
         if ((getnextline(buf,MAX_LINE)) == NULL)  return -1;
         //printf("%s %8ld\n",cur_filename,getInputFileLineNumber());

         // if line has ! it is a remark !
         if (strlen(buf) && buf[0] == '!')
         {
            // remark;
            buf[0] = '\0';
         }
         
         
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

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

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
   LayerStruct *lp;

   if ((lp = doc->FindLayer_by_Name("INSERTION_TOP")) != NULL)
      Graph_Level_Mirror("INSERTION_TOP", "INSERTION_BOTTOM", "");

   Graph_Level_Mirror("ASSEMBLY_TOP", "ASSEMBLY_BOTTOM", "");
   Graph_Level_Mirror("SILKSCREEN_TOP", "SILKSCREEN_BOTTOM", "");
   Graph_Level_Mirror(PAD_TOP_SOLDERMASK, PAD_BOT_SOLDERMASK, "");
   Graph_Level_Mirror(PAD_TOP_SOLDERPASTE, PAD_BOT_SOLDERPASTE, "");

   CString lyr;
   lyr.Format("LYR_%d", NumberOfLayers);
   Graph_Level_Mirror("LYR_1", lyr, "");

	int j = 0;
   for (j = 0; j< doc->getMaxLayerIndex(); j++)
   {
      lp = doc->getLayerArray()[j];
      if (lp == NULL)   continue; // could have been deleted.

      // special for SMD PagetMaxLayerIndex()ds
      if (!lp->getName().CompareNoCase("DRILLHOLE"))
      {
         lp->setLayerType(LAYTYPE_DRILL);
      }

      // special for SMD pads.
      if (!lp->getName().CompareNoCase(SMD_TOP))
      {
         lp->setFlagBits(LY_NEVERMIRROR); 
         lp->setLayerType(LAYTYPE_PAD_TOP);
      }

      if (!lp->getName().CompareNoCase(SMD_BOT))
      {
         lp->setFlagBits(LY_MIRRORONLY); 
         lp->setLayerType(LAYTYPE_PAD_BOTTOM);
      }


      // special for SMD pads.
      if (!lp->getName().CompareNoCase(SMDPASTE_TOP))
      {
         lp->setFlagBits(LY_NEVERMIRROR); 
         lp->setLayerType(LAYTYPE_PASTE_TOP);
      }

      if (!lp->getName().CompareNoCase(SMDPASTE_BOT))
      {
         lp->setFlagBits(LY_MIRRORONLY); 
         lp->setLayerType(LAYTYPE_PASTE_BOTTOM);
      }

      // special for SMD pads.
      if (!lp->getName().CompareNoCase(SMDMASK_TOP))
      {
         lp->setFlagBits(LY_NEVERMIRROR); 
         lp->setLayerType(LAYTYPE_MASK_TOP);
      }

      if (!lp->getName().CompareNoCase(SMDMASK_BOT))
      {
         lp->setFlagBits(LY_MIRRORONLY); 
         lp->setLayerType(LAYTYPE_MASK_BOTTOM);
      }

      Graph_Level_Mirror(PAD_TOP, PAD_BOT, "");
      if (!lp->getName().CompareNoCase(PAD_TOP))
      {
         lp->setLayerType(LAYTYPE_PAD_TOP);
         lp->setElectricalStackNumber(1);
      }

      if (!lp->getName().CompareNoCase(PAD_BOT))
      {
         lp->setLayerType(LAYTYPE_PAD_BOTTOM);
         lp->setElectricalStackNumber(NumberOfLayers);
      }

      if (!lp->getName().CompareNoCase(PAD_INT))
      {
         lp->setLayerType(LAYTYPE_PAD_INNER);
      }

      if (!lp->getName().CompareNoCase("DRILL_HOLE"))
      {
         lp->setLayerType(LAYTYPE_DRILL);
      }

      if (!lp->getName().CompareNoCase("PLACEMENT_OUTLINE"))
      {
         lp->setLayerType(LAYTYPE_COMPONENTOUTLINE);
      }

      if (!lp->getName().CompareNoCase("ASSEMBLY_TOP"))
      {
         lp->setLayerType(LAYTYPE_TOP);
      }

      if (!lp->getName().CompareNoCase("INSERTION_TOP"))
      {
         lp->setLayerType(LAYTYPE_TOP);
      }

      if (!lp->getName().CompareNoCase("ASSEMBLY_BOTTOM"))
      {
         lp->setLayerType(LAYTYPE_BOTTOM);
      }

      if (!lp->getName().CompareNoCase("INSERTION_BOTTOM"))
      {
         lp->setLayerType(LAYTYPE_BOTTOM);
      }

      if (!lp->getName().CompareNoCase("SILKSCREEN_TOP"))
      {
         lp->setLayerType(LAYTYPE_SILK_TOP);
      }

      if (!lp->getName().CompareNoCase("SILKSCREEN_BOTTOM"))
      {
         lp->setLayerType(LAYTYPE_SILK_BOTTOM);
      }

      if (!lp->getName().CompareNoCase(PAD_TOP_SOLDERMASK))
      {
         lp->setLayerType(LAYTYPE_MASK_TOP);
      }

      if (!lp->getName().CompareNoCase(PAD_BOT_SOLDERMASK))
      {
         lp->setLayerType(LAYTYPE_MASK_BOTTOM);
      }

      if (!lp->getName().CompareNoCase(PAD_TOP_SOLDERPASTE))
      {
         lp->setLayerType(LAYTYPE_PASTE_TOP);
      }

      if (!lp->getName().CompareNoCase(PAD_BOT_SOLDERPASTE))
      {
         lp->setLayerType(LAYTYPE_PASTE_BOTTOM);
      }
      if (!lp->getName().CompareNoCase("DRILLDRAWING_THRU"))
      {
         lp->setLayerType(LAYTYPE_DRILL_DRAWING_THRU);
      }

      if (!STRNICMP(lp->getName(),"PadLayer",strlen("PadLayer")))
      {
         int lnr = atoi(lp->getName().Right(strlen(lp->getName()) - strlen("PadLayer")));
         if (lnr == 1)
         {
            lp->setLayerType(LAYTYPE_PAD_TOP);
            lp->setElectricalStackNumber(1);
            CString  padlayerbottom;
            padlayerbottom.Format("PadLayer%d", NumberOfLayers);
            Graph_Level_Mirror(lp->getName(), padlayerbottom, "");
         }
         else
         if (lnr == NumberOfLayers)
         {
            lp->setLayerType(LAYTYPE_PAD_BOTTOM);
            lp->setElectricalStackNumber(NumberOfLayers);
            Graph_Level_Mirror("PadLayer1", lp->getName(), "");
         }
         else
         {
            // inner layers
            lp->setLayerType(LAYTYPE_PAD_INNER);
            lp->setElectricalStackNumber(lnr);
         }
      }
   }

   // here attributes from vbascii.in
   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      LayerStruct *l = doc->getLayerArray()[j];
      if (l == NULL) continue; // could have been deleted.

      CString  lname = l->getName();
      for (int i=0;i<layer_attr_cnt;i++)
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
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getOriginalName() && strlen(block->getOriginalName()))
      {
         // here now check if a block with the original name exist. If not - change the real name and
         // kill the original name
         if (Graph_Block_Exists(doc, block->getOriginalName(), -1) == NULL)
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
   G.AttribMap = NULL;
   G.dialeccnt = 0;
   G.name.Empty();
   G.name_1.Empty();
   G.pinname.Empty();
   G.symbol.Empty();
   G.ref_des.Empty();
   G.level.Empty();
   G.lyrnr = G.dialeccnt = G.push_tok = G.geomtyp = G.smd = G.pincnt = G.notplated= 0;
   G.just = VBTEXT_N_A;
   G.thickness = G.textwidth = 0.0;
   G.mirror = G.sidebot = G.padstackshape = G.comp_mirror = G.fanout_trace = 0;
   G.xdim = G.ydim = G.xoff = G.yoff = 0.0;
   G.diameter = G.radius = G.height = G.rotation = G.chamfer = G.thermal_clearance = G.tie_leg_width = 0.0;
   G.comp_x = G.comp_y = G.comp_rotation = 0.0;
   G.startangle = G.deltaangle = 0.0;
   G.text_type.Empty();
   G.refdes_Count = 0;
   G.netName.Empty();
   G.unigroup.Empty();
   G.layout_def = 0;

   lastpininsert = NULL;

   VBGeomList.Empty();
   if (curVBGeom != NULL)
   {
      delete curVBGeom;
      curVBGeom = NULL;
   }
   vbNetContainer.RemoveAll();

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
   vbNetContainer.Release();

}

/****************************************************************************/
/*
  check if text does not only consist of space
*/
static int is_text(char *n)
{
   unsigned int   i;

   if (n == NULL) 
   {
      // this is allows
      //fprintf(ferr,"Empty text at %ld in %s\n",getInputFileLineNumber(),cur_filename);
      //display_error++;
      return FALSE;
   }

   for (i=0;i<STRLEN(n);i++)
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
   int fromUnits = PageUnits;

   if (STRCMPI(lp,"IN") == 0)
   {
      fromUnits = UNIT_INCHES;
   }
   else if (STRCMPI(lp,"TH") == 0)
   {
      fromUnits = UNIT_MILS;
   }
   else if (STRCMPI(lp,"MM") == 0)
   {
      fromUnits = UNIT_MM;
   }
   else if (STRCMPI(lp,"UM") == 0)
   {
      fromUnits = UNIT_MICRONS;
   }
   else if ((STRCMPI(lp,"NM") == 0) || (STRCMPI(lp,"1NM") == 0))
   {
      fromUnits = UNIT_NANOMETERS;
   }
   else
   {
      fprintf(ferr,"Unknown UNITS [%s]\n",lp);
      display_error++;
   }

   double units = Units_Factor(fromUnits, PageUnits);
 
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
int tok_search(char *lp, List *tok_lst,int tok_size)
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
int vb99_celpin_padstack_negpadname()
{
   int   id = cur_ident;
   char *lp = strword();
   G.negPadName = lp;

   go_command(celpin_padstack_negpad_lst, SIZ_CELPAD_PADSTACK_NEGPAD_LST, id);

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_celpin_padstack_negpad_padlyr()
{
   char *lp = strword();
   G.negPadLayer = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_celpin_padstack_negpad_padrot()
{
   char *lp = strword();
   G.negPadRot = atof( lp );

   // Case 496016
   // We are getting weird data from elsewhere in Mentor for this case.
   // The spec says values are in degrees, but we are getting stuff like 220000.
   // This little klude assumes degree directly if value is less than 1000,
   // but "normalizes" values that are larger. Evidence elswhere is same datum
   // suggests that 220000 is supposed to be 22 degrees.
   if (G.negPadRot > 1000.)
      G.negPadRot /= 10000.;

   G.negPadRot = normalizeDegrees(G.negPadRot);

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_padstack()
{
   int   id = cur_ident;

   G.negPadName.Empty();
   G.negPadLayer.Empty();
   G.negPadRot = 0;

   char *lp;

   lp = strword();
   CString padstackname = lp;
   G.symbol.Format("XPAD_%s", lp);  // padstacks can have the same name as a package

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int part_pin_padstack()
{
   int   id = cur_ident;

   G.negPadName.Empty();
   G.negPadLayer.Empty();
   G.negPadRot = 0;

   char *lp;

   lp = strword();
   CString padstackname = lp;
   //G.symbol.Format("XPAD_%s", lp);  // padstacks can have the same name as a package

   go_command(celpin_padstack_lst, SIZ_CELPAD_PADSTACK_LST, id);

   if (G.negPadName.IsEmpty())
      G.symbol.Format("XPAD_%s", padstackname);
   else
      G.symbol.Format("XPAD_%s_%s", padstackname, G.negPadName);

   BlockStruct *blk = Graph_Block_Exists(doc, G.symbol, -1, blockTypeUndefined, true);
   
   if (blk == NULL)
   {
      CString originalName;
      originalName.Format("XPAD_%s", padstackname);
      BlockStruct *originalBlk = Graph_Block_Exists(doc, originalName, -1, blockTypeUndefined, true);

      if (originalBlk != NULL)
      {
         int layerIndx = Graph_Level(G.negPadLayer, "", 0);
         blk = Graph_Block_On(GBO_APPEND, G.symbol, -1, 0);
         Graph_Block_Copy(originalBlk, 0., 0., 0., 0, 1., 0, TRUE, FALSE);
         if(!G.negPadName.IsEmpty())
         {
            CString ccNegPadApName;
            ccNegPadApName.Format("%s %s", padshapePrefix, G.negPadName);
            ccNegPadApName.Trim();
            double normalizedRotation = normalizeDegrees(G.negPadRot - G.comp_rotation);
            Graph_Block_Reference(ccNegPadApName, "NegPad", -1, 0., 0., DegToRad(normalizedRotation), 0, 1., layerIndx, 0);
         }
         Graph_Block_Off();
      }
   }

   BlockStruct *padstackBlock = Graph_Block_Exists(doc, G.symbol, -1, blockTypeUndefined, true);
   if(NULL == padstackBlock)
   {
      set<CString>::iterator it = padstacksWithDifferentReferenceNames.find(padstackname);
      if(padstacksWithDifferentReferenceNames.end() == it) // We need to output message only once
      {
         fprintf(ferr,"Padstack %s not found even with case insensitive search.\n",padstackname);
         padstacksWithDifferentReferenceNames.insert(padstackname);
      }
   }
   else if((NULL != padstackBlock) && padstackBlock->getName().Compare(G.symbol))
   {
      set<CString>::iterator it = padstacksWithDifferentReferenceNames.find(padstackname);
      if(padstacksWithDifferentReferenceNames.end() == it) // We need to output message only once
      {
         fprintf(ferr,"Padstack %s not found with case sensitive search.\n",padstackname);
         padstacksWithDifferentReferenceNames.insert(padstackname);
      }
   }
   
   if (blk != NULL && pinToUniqueNetZeroCnt > 0)
   {
      VBPinNet pinNet = pinToUniqueNetZeroArray[pinToUniqueNetZeroCnt-1];
      pinNet.m_padstackBlkNum = blk->getBlockNumber();
      pinToUniqueNetZeroArray.SetAt(pinToUniqueNetZeroCnt-1, pinNet);
   }

   if (!strcmp(G.packageGroup,"RF"))
   {
      if(pinToUniqueNetZeroCnt > 0)
      {
         VBPinNet pinNet = pinToUniqueNetZeroArray[pinToUniqueNetZeroCnt-1];
         BlockStruct *block = Graph_Block_On(GBO_APPEND, G.name, -1, 0, blockTypePcbComponent);
         CString pstkName = padstackname;

         if (G.negPadName.IsEmpty())
         {
            pstkName.Format("XPAD_%s", padstackname);
         }
         
         DataStruct *d = Graph_Block_Reference(pstkName, pinNet.pinName, 0, pinNet.m_pinLoc.x - G.comp_x, pinNet.m_pinLoc.y - G.comp_y, 0.0/*degreesToRadians(G.comp_rotation)*/, 0, 1.0, Graph_Level("0","",1), TRUE, blockTypePadstack);
         d->getInsert()->setInsertType(insertTypePin);
         Graph_Block_Off();
      }
   }
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

   G.rotation = normalizeDegrees(G.rotation);

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
int part_cell_xy()
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
int vb99_xy()
{
   char  *lp;
   char  buf[MAX_LONGLINE];
   int   id = 0;
   int   try_cnt = 0;

   while (TRUE)
   {
      if (try_cnt)
      {
         //if ((fgets(buf,MAX_LONGLINE,cur_fp)) == NULL)  return -1;
         if ((getnextline(buf,MAX_LONGLINE)) == NULL)  return -1;
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
            if(G.layout_def)
            {
               poly_l[polycnt].x -= G.comp_x;
               poly_l[polycnt].y -= G.comp_y;
               Rotate(poly_l[polycnt].x, poly_l[polycnt].y, -1*G.comp_rotation, &poly_l[polycnt].x, &poly_l[polycnt].y);
            }
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
int vb99_xyr()
{
   char  *lp;
   char  buf[MAX_LONGLINE];
   int   id = 0;
   int   try_cnt = 0;

   while (TRUE)
   {
      if (try_cnt)
      {
         /*if ((fgets(buf,MAX_LONGLINE,cur_fp)) == NULL)  return -1;*/
         if ((getnextline(buf,MAX_LONGLINE)) == NULL)  return -1;
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
            if(G.layout_def)
            {
               poly_l[polycnt].x -= G.comp_x;
               poly_l[polycnt].y -= G.comp_y;
               Rotate(poly_l[polycnt].x, poly_l[polycnt].y, -1*G.comp_rotation, &poly_l[polycnt].x, &poly_l[polycnt].y);
            }
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
int graph_side()
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
      else if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = PAD_BOT_SOLDERMASK;
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = PAD_BOT_SOLDERMASK;
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"TOP"))
      {
         G.level = PAD_TOP_SOLDERMASK;
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
   }
   else if (cur_status == FILE_SOLDER_PASTE)
   {      
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = PAD_BOT_SOLDERPASTE;
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = PAD_BOT_SOLDERPASTE;
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"TOP"))
      {
         G.level = PAD_TOP_SOLDERPASTE;
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
   }
   else if ((cur_status == FILE_ASSEMBLY_OUTL)
      || (cur_status == CELL_ASSEMBLY_OUTLINE))
   {
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = "ASSEMBLY_BOTTOM";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = "ASSEMBLY_BOTTOM";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"TOP"))
      {
         G.level ="ASSEMBLY_TOP";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
   }
   else if ((cur_status == FILE_INSERTION_OUTL)
      || (cur_status == CELL_INSERTION_OUTLINE))
   {
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = "INSERTION_BOTTOM";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = "INSERTION_BOTTOM";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"TOP"))
      {
         G.level ="INSERTION_TOP";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
   }
   else if (cur_status == FILE_SILK_ARTWORK)
   {
      if (curVBGeom != NULL)
      {
         curVBGeom->SetSilkscreenSide(lp);
         return 1;
      }
      else if (!STRCMPI(lp,"OPP_SIDE"))
      {
         G.level = "SILKSCREEN_BOTTOM";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"BOTTOM"))
      {
         G.level = "SILKSCREEN_BOTTOM";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
      else if (!STRCMPI(lp,"TOP"))
      {
         G.level ="SILKSCREEN_TOP";
         G.lyrnr = Graph_Level(G.level,"",0);
         return 1;
      }
   }

   else if (cur_status == CELL_FANOUT)
   {
      if (!STRCMPI(lp,"OPP_SIDE"))
      {
         CString lyr;
         lyr.Format("LYR_%d", NumberOfLayers);
         G.level = lyr;
         G.lyrnr = Graph_Level(G.level,"",0);
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
int vb99_std_lyr()
{
   char     *lp;
   lp = strword();
   
   G.level = lp;

   if (strlen(G.level) == 0)
      G.lyrnr = Graph_Level("0","",1); 
   else
   {
      if (!G.level.CompareNoCase("ASSEMBLY"))
         G.level = "ASSEMBLY_TOP";
      else if (!G.level.CompareNoCase("ASSEMBLY_MNT_LYR"))
      {
         if(G.comp_mirror)
            G.level = "ASSEMBLY_BOTTOM";
         else
            G.level = "ASSEMBLY_TOP";
      }
      else if (!G.level.CompareNoCase("ASSEMBLY_OPP_LYR"))
      {
         if(G.comp_mirror)
            G.level = "ASSEMBLY_TOP";
         else
            G.level = "ASSEMBLY_BOTTOM";
      }
      else if (!G.level.CompareNoCase("ASSEMBLY_TOP"))
         G.level = "ASSEMBLY_TOP";
      else if (!G.level.CompareNoCase("ASSEMBLY_BOTTOM"))
         G.level = "ASSEMBLY_BOTTOM";   
      else if (!G.level.CompareNoCase("SILKSCREEN_MNT_LYR"))
      {
         if(G.comp_mirror)
            G.level = "SILKSCREEN_BOTTOM";
         else
            G.level = "SILKSCREEN_TOP";
      }
      else if (!G.level.CompareNoCase("SILKSCREEN_OPP_LYR"))
      {
         if(G.comp_mirror)
            G.level = "SILKSCREEN_TOP";
         else
            G.level = "SILKSCREEN_BOTTOM";
      }
      else if (!G.level.CompareNoCase("SILKSCREEN_TOP"))
         G.level = "SILKSCREEN_TOP";
      else if (!G.level.CompareNoCase("SILKSCREEN_BOTTOM"))
         G.level = "SILKSCREEN_BOTTOM";
      else if (!G.level.CompareNoCase("SOLDERMASK_MNT_LYR"))
      {
         if(G.comp_mirror)
            G.level = "PAD_BOT_SOLDERMASK";
         else
            G.level = "PAD_TOP_SOLDERMASK";
      }
      else if (!G.level.CompareNoCase("SOLDERMASK_OPP_LYR"))
      {
         if(G.comp_mirror)
            G.level = "PAD_TOP_SOLDERMASK";
         else
            G.level = "PAD_BOT_SOLDERMASK";
      }
      else if (!G.level.CompareNoCase("SOLDERMASK_TOP"))
         G.level = PAD_TOP_SOLDERMASK;
      else if (!G.level.CompareNoCase("SOLDERMASK_BOTTOM"))
         G.level = PAD_BOT_SOLDERMASK;
      else if (!G.level.CompareNoCase("SOLDERPASTE_MNT_LYR"))
      {
         if(G.comp_mirror)
            G.level = "PAD_BOT_SOLDERPASTE";
         else
            G.level = "PAD_TOP_SOLDERPASTE";
      }
      else if (!G.level.CompareNoCase("SOLDERPASTE_OPP_LYR"))
      {
         if(G.comp_mirror)
            G.level = "PAD_TOP_SOLDERPASTE";
         else
            G.level = "PAD_BOT_SOLDERPASTE";
      }
      else if (!G.level.CompareNoCase("SOLDERPASTE_TOP"))
         G.level = PAD_TOP_SOLDERPASTE;
      else if (!G.level.CompareNoCase("SOLDERPASTE_BOTTOM"))
         G.level = "SOLDERPASTE_BOT";  
      else if (!G.level.CompareNoCase("DRILLDRAWING_THRU"))
         G.level = "DRILLDRAWING_THRU";
      else if (!G.level.CompareNoCase("DRILL_DRAWING_LYR"))
         G.level = "DRILLDRAWING_THRU";

      // Case dts0100526291
      // If this is trace fanout, we would set the MNT_LYR to LYR_1
      if(cur_status == CELL_FANOUT || cur_status == FILE_PLACE_OBS || 
         cur_status == FILE_VIA_OBS || cur_status == FILE_ROUTE_OBS)
      {
         if (!G.level.CompareNoCase("MNT_LYR"))
            G.level = "LYR_1";
      }

      // Case dts0100456262
      // DO NOT scan the user layer comments when seeking a standard layer.
      // The cad data in the cited case has user layers with same name as
      // standard layers, but they are NOT the same layers. This code tended
      // to return the user layer when what was correct was the standard layer.
      // So for this case, laye rlookup for standard versus user layer has been
      // completely separated. See vb99_user_lyr() below.
      // This code is left in place to warn you to NOT DO THIS.
      // (Also note how inefficient it was,kept looping even after finding what it was after.)
      //
      // for (int j = 0; j< doc->getMaxLayerIndex(); j++)
      // {
      //    LayerStruct *lp = doc->getLayerArray()[j];
      //    if (lp == NULL)   continue; // could have been deleted.
      // 
      //    if (strlen(lp->getComment()) && lp->getComment().CompareNoCase(G.level) == 0)
      //    {
      //       G.level = lp->getName();
      //    }
      // }

      G.lyrnr = Graph_Level(G.level,"",0); 
   }
   return 1;
}

/****************************************************************************/

static LayerStruct *vb99GetUserLayerByOriginalName(CString name)
{
   for (int j = 0; j< doc->getMaxLayerIndex(); j++)
   {
      LayerStruct *lp = doc->getLayerArray()[j];
      if (lp != NULL)
      {
         if (lp->getComment().CompareNoCase(name) == 0)
         {
            return lp;
         }
      }
   }

   return NULL;
}

/****************************************************************************/
/*
   Case dts0100456262.
   User layers end up with made up names based on the layer number, these
   have the form USER_LVL_n where n is layer number from cad.
   The layer name from cad gets put on the Layer as a comment.
   So what we need to do here in response to USER_LYR reference in CAD
   data is scan the layers for a matching comment, then select that layer.
*/
int vb99_user_lyr()
{
   char     *chr;
   chr = strword();
   
   G.level = chr;

   if (strlen(G.level) == 0)
      G.lyrnr = Graph_Level("0","",1); 
   else
   {
      if (!G.level.CompareNoCase("ASSEMBLY"))
         G.level = "ASSEMBLY_TOP";
      else
      if (!G.level.CompareNoCase("SILKSCREEN_MNT_SIDE"))
         G.level = "SILKSCREEN_TOP";
      else
      if (!G.level.CompareNoCase("SILKSCREEN_OPP_SIDE"))
         G.level = "SILKSCREEN_BOTTOM";

      LayerStruct *lp = vb99GetUserLayerByOriginalName(G.level);
      if (lp != NULL)
         G.level = lp->getName();

      G.lyrnr = Graph_Level(G.level, "", 0); 
   }
   return 1;
}

/****************************************************************************/
/*
  Skip this one to the next level.
*/
int vb99_skip()
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
int vb99_units()
{
   char *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   unit_faktor = get_units(lp);
   return 1;
}

/****************************************************************************/
/*
*/
int padstack_units()
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
int start_command(List *lst,int siz_lst,int start_ident)
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
int vb99_null(int start_ident)
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
         //if ((fgets(buf,MAX_LONGLINE,cur_fp)) == NULL)  return -1;
         if ((getnextline(buf,MAX_LONGLINE)) == NULL)  return -1;
         incrementInputFileLineNumber();
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
int laystack_layer_num()
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
int vb99_name()
{
   char *lp;

   lp = strword();
   G.name = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_layer()
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
int pad_pad()
{
   int   err, id = cur_ident;
   char  *lp;

   lp = strword();

   G.name.Format("%s %s", padshapePrefix, lp);   // can have the same name as a PADSTACK
   G.name.Trim();

   G.geomtyp = T_UNDEFINED;
   G.xdim = 0;
   G.ydim = 0;
   G.xoff = 0;
   G.yoff = 0;
   G.diameter = 0;
   G.chamfer = 0;
   G.tie_leg_width = 0;
   G.thermal_clearance = 0;

   bool savedSetting = reportUnknownCommands;
   reportUnknownCommands = true; // case dts0100386334, want to know about skipped pad shape commands
   unknownCommandMessage = "Unrecognized PAD";
   go_command(padform_lst,SIZ_PADFORM_LST,id);
   reportUnknownCommands = savedSetting;

   // here make an aperture
   if (G.geomtyp == T_TRIANGLE)
   {
      fprintf(ferr,"Here need to make a triangle aperture\n");
      Graph_Aperture(G.name, T_ROUND, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0, 
            BL_APERTURE, TRUE, &err);

   }
   else
   if (G.geomtyp == T_COMPLEX)
   {
      CString  usr_name;
      usr_name.Format("SHAPE_%s", G.name);
      BlockStruct* block = doc->Find_Block_by_Name(usr_name, -1);
      CTMatrix matrix;
      matrix.translateCtm(G.xoff,G.yoff);
      block->transform(matrix);
      Graph_Complex(G.name, 0, usr_name, 0.0, 0.0, 0.0);
   }
   else
   {
      Graph_Aperture(G.name, G.geomtyp, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
   }

   return 1;
}

/******************************************************************************
* pad_hole
*/
int pad_hole()
{
   customDrillSymName.Empty();
   standardSymbolName.Empty();
   standardSymbolSize.Empty();
   CharacterSymbolName.Empty();
   CharacterSymbolSize.Empty();
   char *lp = strword();

   G.geomtyp = T_UNDEFINED;
   G.xdim = 0;
   G.ydim = 0;
   G.xoff = 0;
   G.yoff = 0;
   G.punched = 0;
   G.notplated = 1;

   int id = cur_ident;
   go_command(hole_lst, SIZ_HOLE_LST, id);
   

   int plated = 0;
   if (G.notplated == 0)
      plated = 1;

   int layernum = Graph_Level("DRILLHOLE", "", 0);
    int blockNum = 0;
   if(!customDrillSymName.IsEmpty())
   {      
      blockNum = doc->Get_Block_Num(customDrillSymName, -1, 0);
   }
   else if(!standardSymbolName.IsEmpty())
   {
      CString DrillName = standardSymbolName + "_" + standardSymbolSize;
      blockNum = doc->Get_Block_Num(DrillName, -1, 0);
   }
   else if(!CharacterSymbolName.IsEmpty())
   {
      CString DrillName = CharacterSymbolName + "_" + CharacterSymbolSize;
      blockNum = doc->Get_Block_Num(DrillName, -1, 0);
   }
   int display = 0;

   int widthIndex = -1;
   if(G.geomtyp == T_ROUND)
   {
      G.name.Format(".HOLE_%s", lp);   // can have same name as padstack
      widthIndex = Graph_Tool(G.name, 0, G.xdim, 0, blockNum, display , 0L, plated, G.punched);
   }
   else if((G.geomtyp == T_RECTANGLE) || (G.geomtyp == T_SQUARE))
   {
      //Add code to for complex drill hole
      G.name.Format(".HOLE_%s", lp);   // can have same name as padstack
      widthIndex = Graph_ComplexTool(G.name, 0, 0, blockNum, display , 0L, plated, G.punched,G.geomtyp,G.xdim,G.ydim);
   }
   else if(G.geomtyp == T_OBLONG)
   {
      G.name.Format(".HOLE_%s", lp);   // can have same name as padstack
      int apGeomNum = Graph_Aperture(G.name, T_OBLONG, G.xdim, G.ydim, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, NULL);
      widthIndex = Graph_ComplexTool(G.name, apGeomNum, 0, blockNum, display , 0L, plated, G.punched,G.geomtyp,G.xdim,G.ydim);
   }

   if(widthIndex != -1)
   {
      BlockStruct *widthBlock = doc->getWidthBlock(widthIndex);

      if ( widthBlock != NULL )
      {
         if (G.plusTol != 0.0)
         {
            double positiveTol = G.plusTol;
            doc->SetAttrib(&widthBlock->getAttributesRef(), doc->IsKeyWord(ATT_PLUSTOLERANCE, 0), VT_DOUBLE, &positiveTol, SA_APPEND, NULL);
         }

         if (G.minusTol != 0.0)
         {
            double negativeTol = G.minusTol;
            doc->SetAttrib(&widthBlock->getAttributesRef(), doc->IsKeyWord(ATT_MINUSTOLERANCE, 0), VT_DOUBLE, &negativeTol, SA_APPEND, NULL);
         }
      }
   }
   G.plusTol = 0.0;
   G.minusTol = 0.0;


   return 1;
}

/****************************************************************************/
/*
*/
int padstack_smd()
{
   char  *lp;

   G.smd = FALSE;
   lp = strword();

   if (!STRCMPI(lp, "MOUNTING_HOLE"))
   {
      curblock->setBlockType(BLOCKTYPE_TOOLING);
   }
   else
   if (!STRCMPI(lp, "FIDUCIAL"))
   {
      G.smd = TRUE;
      curblock->setBlockType(BLOCKTYPE_FIDUCIAL);
   }
   else
   if (!STRCMPI(lp, "PIN_SMD"))
      G.smd = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_technology()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();

   if (!STRCMPI(lp,USE_TECHNOLOGY))
   {
      go_command(padstack_lst,SIZ_PADSTACK_LST,id);
   }
   else if(!STRCMPI(lp,"(Default)"))
   {
      defaultTechnology = true;
      go_command(padstack_lst,SIZ_PADSTACK_LST,id);
      defaultTechnology = false;      
   }
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
int pdb_name()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
      doc->SetUnknownAttrib(&curtype->getAttributesRef(),ATT_NAME, name, SA_OVERWRITE, NULL);

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int pdb_prop()
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
      doc->SetUnknownAttrib(&curtype->getAttributesRef(),get_attribmap(key), val, SA_OVERWRITE, NULL);

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int pdb_topcell()
{
   char  *lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   doc->SetUnknownAttrib(&curtype->getAttributesRef(),get_attribmap("TopCell"), name, SA_OVERWRITE, NULL);
   return 1;
}

/****************************************************************************/
/*
*/
int pdb_botcell()
{
   char  *lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   doc->SetUnknownAttrib(&curtype->getAttributesRef(),get_attribmap("BottomCell"), name, SA_OVERWRITE, NULL);
   return 1;
}

/****************************************************************************/
/*
*/
int pdb_refprefix()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
      doc->SetUnknownAttrib(&curtype->getAttributesRef(),"VB_REFPREFIX", name, SA_OVERWRITE, NULL);

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int pdb_label()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
      doc->SetUnknownAttrib(&curtype->getAttributesRef(),"VB_LABEL", name, SA_OVERWRITE, NULL);

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int pdb_desc()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   CString  name;
   
   name = lp;
   name.TrimLeft();
   name.TrimRight();

   if (strlen(name))
      doc->SetUnknownAttrib(&curtype->getAttributesRef(),"VB_DESC", name, SA_OVERWRITE, NULL);

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int netprops_pins()
{
   int   id = cur_ident;

   go_command(netprops_pins_lst,SIZ_NETPROPS_PINS_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int netprops_pin_section()
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
int netprops_pins_refpin()
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

   pinname = compname.Right(strlen(compname) - index - 1);
   compname.Delete(index, strlen(compname) - index);

   while ((index = compname.ReverseFind('-')) >= 0)
   {
      if (IsCompExist(compname))
         break;

      pinname = compname.Right(strlen(compname) - index) + pinname;
      compname.Delete(index, strlen(compname) - index);
   }  

   // "U22-27"
   // netname is in G.name
   NetStruct *n = add_net(fl, G.name);

   add_comppin(fl, n, compname, pinname);
   
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

      if (data->getDataType() != T_INSERT)
         continue;
      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;
      if (data->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)
         continue;

      if (!compname.CompareNoCase(data->getInsert()->getRefname()))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* pad_padstack
*/
int pad_padstack()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.name.Format("XPAD_%s", lp); // padstacks can have the same name as a package
   G.smd = FALSE;
   G.notplated = FALSE;
   G.padstackshape = 0;

   curblock = Graph_Block_On(GBO_APPEND, G.name, -1, 0);
   curblock->setBlockType(BLOCKTYPE_PADSTACK);
   curblock->setOriginalName(lp);

   defaultTechnology = false;
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
      if (!topPadName.IsEmpty() && topPadName == bottomPadName) // if top and bottom is the same
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topPadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_TOP, "", 0);   // use a standart layer
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();
   
         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
         bottomPadName = "";
      }
      else if (!topPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topPadName);
         padname.Trim();

         int layerindex = Graph_Level(SMD_TOP, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      // still in SMD (no inner layers).
      if (!bottomPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomPadName);
         padname.Trim();

         int layerindex = Graph_Level(SMD_BOT, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      // true smd if top and bot to it can be mirrored.
      if (!topPadName.IsEmpty())
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);
      else
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);

      if (!topSoldermaskPadName.IsEmpty() && topSoldermaskPadName == bottomSoldermaskPadName) // if top and bottom is the same
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSoldermaskPadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_TOP_SOLDERMASK, "", 0); // use a standart layer
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();
   
         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
         bottomSoldermaskPadName = "";
      }
      else if (!topSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSoldermaskPadName);
         padname.Trim();

         int layerindex = Graph_Level(SMDMASK_TOP, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSoldermaskPadName);
         padname.Trim();

         int layerindex = Graph_Level(SMDMASK_BOT, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!topSolderPastePadName.IsEmpty() && topSolderPastePadName == bottomSolderPastePadName)   // if top and bottom are the same
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSolderPastePadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_TOP_SOLDERPASTE,"", 0); // use a standart layer
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();
   
         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
         bottomSolderPastePadName = "";
      }
      else if (!topSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSolderPastePadName);
         padname.Trim();

         int layerindex = Graph_Level(SMDPASTE_TOP, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSolderPastePadName);
         padname.Trim();

         int layerindex = Graph_Level(SMDPASTE_BOT, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
      if (!clearancePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, clearancePadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_CLEARANCE, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
   }
   else // innerPad exists
   {
      if (!topPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topPadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_TOP, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomPadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_BOT, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!innerPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, innerPadName);
         padname.Trim();        

         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();
         for (int i=2;i<=NumberOfLayers-1;i++)
         {
            CString  tmp;
            tmp.Format("PadLayer%d",i);
            int   lyrnr = Graph_Level(tmp,"", 0);
            LayerStruct *l = doc->FindLayer(lyrnr);
            l->setLayerType(LAYTYPE_PAD_INNER);
            Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, lyrnr, TRUE);
         }
      }

      if (!holeName.IsEmpty())
      {
         int layernum = Graph_Level("DRILLHOLE", "", 0);

         Graph_Block_Reference(holeName, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
      }

      if (!topSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSoldermaskPadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_TOP_SOLDERMASK,"",0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!topSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, topSolderPastePadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_TOP_SOLDERPASTE, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSoldermaskPadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSoldermaskPadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_BOT_SOLDERMASK, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }

      if (!bottomSolderPastePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, bottomSolderPastePadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_BOT_SOLDERPASTE, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
      if (!clearancePadName.IsEmpty())
      {
         CString padname;
         padname.Format("%s %s", padshapePrefix, clearancePadName);
         padname.Trim();

         int layerindex = Graph_Level(PAD_CLEARANCE, "", 0);
         Graph_Block_On(GBO_APPEND, padname, -1, 0);
         Graph_Block_Off();

         Graph_Block_Reference(padname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
   }


   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
int pad_custom_drill_symbol()
{
   char  *lp;
   int   id = cur_ident;
   int   oldstat = cur_status;

   lp = strword();
   G.name = lp;

   curblock = Graph_Block_On(GBO_APPEND,G.name,-1,0);
   curblock->setBlockType(blockTypeToolGraphic);
   doc->SetUnknownAttrib(&curblock->getAttributesRef(),"CUSTOM_SYMBOL", G.name, SA_OVERWRITE, NULL);

   cur_status = 0;

   G.level = "DRILLSYMBOL";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;
   doc->getWidthTable().Add(curblock);
   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
  Drawing_cells are non electrical Parts.
*/
int cel_drawing_cell()
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

   CString cellName = G.name;
   bool skipThisCell = false;
   set<CString>::iterator it = skipCellList.find(cellName.MakeUpper());
   if(it != skipCellList.end())
      skipThisCell = true;
   curblock = Graph_Block_On(GBO_APPEND,G.name,-1,0,blockTypeGenericComponent);
   G.AttribMap = &curblock->getAttributesRef();

   go_command(celpart_lst,SIZ_CELPART_LST,id);

   Graph_Block_Off();

   if (explode_drawing_cell || skipThisCell)
   {
      // delete this definition, so that the definition is used from LAYOUT.HKP
      doc->RemoveBlock(curblock);
   }

   lastpininsert = NULL;
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
int cel_filetype()
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
int des_filetype()
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
int netprops_filetype()
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
int pdb_filetype()
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
int jobprefs_filetype()
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
int pad_filetype()
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
int cel_version()
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
      ErrorMessage(tmp, "VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
int des_version()
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
      ErrorMessage(tmp, "VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
int netprops_version()
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
      ErrorMessage(tmp, "VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
int pdb_version()
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
      ErrorMessage(tmp, "VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
int jobprefs_version()
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
      ErrorMessage(tmp, "VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
int pad_version()
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
      ErrorMessage(tmp, "VBASCII Version Warning", MB_OK | MB_ICONHAND);
   }
*/

   return 1;
}

/****************************************************************************/
/*
*/
int cel_package_cell()
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

   curblock = Graph_Block_On(GBO_APPEND, geomname, -1, 0);
   if (is_attrib(curblock->getName(), TEST_ATTR) != 0)
      curblock->setBlockType(blockTypeTestPoint);
   else
      curblock->setBlockType(blockTypePcbComponent);
   G.AttribMap = &curblock->getAttributesRef();

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

   if (!G.packageGroup.IsEmpty())
   {
      doc->SetUnknownAttrib(&curblock->getAttributesRef(),"PACKAGE_GROUP", G.packageGroup.GetBuffer(), SA_OVERWRITE, NULL);
   }

   G.m_duplicatePinMapCount.RemoveAll();
   //here update last curpad struct
   overwrite_pad();

   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
int cel_mechanical_cell()
{
   int   id = cur_ident;
   char  *lp;

   //here update last curpad struct
   overwrite_pad();

   lastpininsert = NULL;
   lp = strword();
   G.name = lp;

   G.pincnt = 0;

   curblock = Graph_Block_On(GBO_APPEND, G.name,-1,0);
   curblock->setBlockType(BLOCKTYPE_MECHCOMPONENT);
   G.AttribMap = &curblock->getAttributesRef();

   go_command(celpart_lst,SIZ_CELPART_LST,id);

   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
int des_part()
{
   Component_Options.Empty();
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
   G.packageGroup.Empty();
   G.unigroup = "No Group";
   G.AttribMap = NULL;
   derived = FALSE;
   G.layout_def = 0;

   // Reset the map
   pinToUniqueNetZeroArray.RemoveAll();
   pinToUniqueNetZeroCnt = 0;

   // make sure 
   BlockStruct *block = Graph_Block_On(GBO_APPEND,geomname,-1,0);
   Graph_Block_Off();
   G.name = block->getName();

   CAttributes* attribMap = new CAttributes();
   G.AttribMap = &attribMap;

   if(!VBGeomList.FindGeom(geomname))
   {
      VBGeomList.AddGeom(geomname, block->getBlockNumber());
   }
   curVBGeom = new CVBGeom(geomname, block->getBlockNumber());
   
   go_command(part_instance_lst,SIZ_PART_INSTANCE_LST,id);
   if(Component_Options == "NOT_PLACED")
   {
      Component_Options.Empty();
      pinToUniqueNetZeroArray.RemoveAll();
      pinToUniqueNetZeroCnt = 0;

      G.packageGroup.Empty();
      G.AttribMap = NULL;
      G.ref_des = "";
      G.unigroup.Empty();
      polycnt = 0;
      return 1;
   }

   if(strcmp(G.unigroup,"No Group"))
   {      
      G.unigroup.MakeUpper();
      
      if(!VBGeomList.FindGeom(G.unigroup))
      {
         // Add a new compositeComp block. g.unigroup is the group name
         BlockStruct* block = Graph_Block_On(GBO_APPEND, G.unigroup, -1, 0);  
         block->setBlockType(blockTypeCompositeComp);
         Graph_Block_Off();

         // Add the compositeComp as the insert of the board
         DataStruct *d = Graph_Block_Reference(G.unigroup, G.unigroup, 0, 0.0,0.0, 0.0,0 , 1.0,Graph_Level("0","",1), TRUE);
         d->getInsert()->setInsertType(insertTypeCompositeComp);

         VBGeomList.AddGeom(G.unigroup, block->getBlockNumber());
      }

      // Add RF shape to the compositeComp block
      block = Graph_Block_On(GBO_APPEND, G.unigroup, -1, 0, blockTypeCompositeComp);    

      // Add insert to compositeComp block
      DataStruct *d = Graph_Block_Reference(G.name, G.ref_des, 0, G.comp_x, G.comp_y, degreesToRadians(G.comp_rotation), G.comp_mirror, 1.0, Graph_Level(G.level,"",1), TRUE, blockTypePcbComponent);
      d->getInsert()->setInsertType(insertTypePcbComponent);

      doc->CopyAttribs(&d->getAttributesRef(), attribMap);
      // Get the saved poly data from write_poly()
      BlockStruct* tmpPoly = Graph_Block_On(GBO_APPEND, "Temp_Poly", -1, 0); 
      Graph_Block_Off();

      BlockStruct* compPoly = Graph_Block_On(GBO_APPEND, G.name, -1, 0);
      // Copy the poly data to the component (insert in composite component).
      compPoly->getDataList().takeData(tmpPoly->getDataList());
      Graph_Block_Off();

      Graph_Block_Off();   // End of compositeComp block

      G.packageGroup.Empty(); // unigroup is only used in this function. Make sure to clean it before leaving this function
      G.AttribMap = NULL;
      G.ref_des = "";      // refdes
      G.unigroup.Empty();       // unigroup is only used in this function. Make sure to clean it before leaving this function
      polycnt = 0;
      G.layout_def = 0;
      return 1;
   }

   // Get the saved poly data from write_poly()
   BlockStruct* tmpPoly = Graph_Block_On(GBO_APPEND, "Temp_Poly", -1, 0); 
   Graph_Block_Off();

   geomname = block->getName();

   DataStruct *d = Graph_Block_Reference(geomname, G.ref_des, 0, 0.0,0.0, 0.0,0 , 1.0,Graph_Level("0","",1), TRUE);
   block->getDataList().takeData(tmpPoly->getDataList());

   doc->CopyAttribs(&d->getAttributesRef(), attribMap);
   if(NULL != attribMap)
   {
      delete attribMap;
      attribMap = NULL;
   }
   G.AttribMap = &d->getAttributesRef();
   if (block->getBlockType() == BLOCKTYPE_TESTPOINT)
   {   
      d->getInsert()->setInsertType(insertTypeTestPoint);
      doc->SetUnknownAttrib(&d->getAttributesRef(),ATT_TEST,"",
         SA_OVERWRITE, NULL);
   }
   else if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
   {
      d->getInsert()->setInsertType(insertTypePcbComponent);
   } 
   else
   {
      // bad mojo here...
     d->getInsert()->setInsertType(insertTypeUnknown);
   }
   
   CVBGeom* vbGeom = VBGeomList.FindGeom(geomname);
   if (vbGeom != NULL && curVBGeom != NULL)
   {
      if (vbGeom->IsDifference(curVBGeom))
      {
         CVBGeom* derivedVBGeom = vbGeom->CheckForDerivedGeom(curVBGeom);
         if (derivedVBGeom == NULL)
         {
            CString newGeomName = vbGeom->GetDerivedGeomName();
            BlockStruct* newBlock = Graph_Block_On(GBO_APPEND, newGeomName, -1, 0);
            newBlock->setBlockType(block->getBlockType());
            Graph_Block_Copy(block, 0.0, 0.0, 0.0, 0, 1, -1, TRUE);
            Graph_Block_Off();

            POSITION pos = newBlock->getHeadDataPosition();
            while (pos != NULL)
            {
               DataStruct* data = newBlock->getNextData(pos);
               if (data == NULL)
                  continue;

               if (data->getDataType() == dataTypePoly)
               {
                  LayerStruct* layer = doc->getLayerAt(data->getLayerIndex());
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
                           data->setLayerIndex(Graph_Level("SILKSCREEN_BOTTOM", "", FALSE));
                        }
                     }
                     else if (layer->getName().CompareNoCase("SILKSCREEN_BOTTOM") == 0)
                     {
                        if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() != layer->getLayerIndex())
                           data->setLayerIndex(layer->getMirroredLayerIndex());
                        else
                        {
                           data->setLayerIndex(Graph_Level("SILKSCREEN_TOP", "", FALSE));
                        }
                     }
                     else if (layer->getName().CompareNoCase(PAD_TOP_SOLDERMASK) == 0)
                     {
                        if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() != layer->getLayerIndex())
                           data->setLayerIndex(layer->getMirroredLayerIndex());
                        else
                        {
                           data->setLayerIndex(Graph_Level(PAD_BOT_SOLDERMASK, "", FALSE));
                        }
                     }
                     else if (layer->getName().CompareNoCase(PAD_BOT_SOLDERMASK) == 0)
                     {
                        if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() != layer->getLayerIndex())
                           data->setLayerIndex(layer->getMirroredLayerIndex());
                        else
                        {
                           data->setLayerIndex(Graph_Level(PAD_TOP_SOLDERMASK, "", FALSE));
                        }
                     }
                  }

                  //if (data->getLayerIndex() != doc->getLayer("ASSEMBLY_TOP")->getLayerIndex() && 
                  //    data->getLayerIndex() != doc->getLayer("ASSEMBLY_BOTTOM")->getLayerIndex())
                  //   continue;

                  //CPolyList* polyList = data->getPolyList();
                  //POSITION polyHead = polyList->GetHeadPosition();
                  //while(polyHead)
                  //{
                  //   CPoly *poly = polyList->GetNext(polyHead);
                  //   if(poly != NULL)
                  //   {
                  //      CPntList pntList = poly->getPntList();
                  //      if(&pntList != NULL)
                  //      {
                  //         POSITION pntHead = pntList.GetHeadPosition();
                  //         while(pntHead)
                  //         {
                  //            CPnt *pnt = pntList.GetNext(pntHead);
                  //            if(pnt != NULL)
                  //            {
                  //               //pnt->x = pnt->x - (DbUnit)G.comp_x;
                  //               //pnt->y = pnt->y - (DbUnit)G.comp_y;
                  //            }
                  //         }
                  //      }
                  //   }
                  //}
               }
               else if (data->getDataType() == dataTypeInsert)
               {
                  InsertStruct* pinInsert = data->getInsert();
                  if (pinInsert->getInsertType() != insertTypePin)
                     continue;

                  CVBPin* vbPin = curVBGeom->FindPin(pinInsert->getRefname());
                  if (vbPin != NULL)
                  {
                     if (vbPin->GetPadstackBlkNum() > -1)
                        pinInsert->setBlockNumber( vbPin->GetPadstackBlkNum() );

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

            // CVBPin* vbPin = vbGeom->FindPin(pinInsert->getRefname());
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
            block = doc->getBlockAt(derivedVBGeom->GetGeomNum());
         }

         d->getInsert()->setBlockNumber(block->getBlockNumber());
      }

      delete curVBGeom;
      curVBGeom = NULL;
   }


   G.AttribMap = NULL;
   cur_compcnt++;
   int odd_smd = 0;

   d->getInsert()->setRefname(STRDUP(G.ref_des));
   d->getInsert()->setOriginX(G.comp_x);
   d->getInsert()->setOriginY(G.comp_y);
   d->getInsert()->setAngle(DegToRad(G.comp_rotation));

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

      d->getInsert()->setAngle(DegToRad(G.comp_rotation));
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
      TypeStruct *existingType;
      while (existingType = FindType(fl, typeName))
      {
         if (existingType->getBlockNumber() == -1 || existingType->getBlockNumber() == block->getBlockNumber()) // use this one
         {
            existingType->setBlockNumber( block->getBlockNumber());
            break;
         }

         typeName.Format("%s_%d", G.name_1, ++n);
      }

      TypeStruct *type = AddType(fl, typeName);
      type->setBlockNumber( block->getBlockNumber());


      if (typeName.CompareNoCase(G.name_1)) // if we changed the name
      {
         int orignialTypeKey = doc->RegisterKeyWord("ORIGINAL_DEVICE", 0, VT_STRING);
         doc->SetAttrib(&type->getAttributesRef(), orignialTypeKey, VT_STRING, G.name_1.GetBuffer(0), SA_OVERWRITE, NULL);

         fprintf(ferr, "Component [%s] used Device Name [%s].  Changed to Device Name [%s] because of duplication.\n", G.ref_des, G.name_1, typeName);
         display_error++;
      }
         

      int keyword = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);
      doc->SetAttrib(&d->getAttributesRef(), keyword, VT_STRING, typeName.GetBuffer(0), SA_OVERWRITE, NULL);
      // Fix for dts0100511673 Assembly/Silkscreen Part numbers are not imported into VisEDOC
      keyword = doc->IsKeyWord(ATT_PARTNUMBER, TRUE);
      doc->SetAttrib(&d->getAttributesRef(), keyword, VT_STRING, typeName.GetBuffer(0), SA_OVERWRITE, NULL);
   }

   if (odd_smd)
   {
      RemoveAttrib(doc->IsKeyWord(ATT_SMDSHAPE, TRUE), &d->getAttributesRef());

      doc->SetAttrib(&d->getAttributesRef(),doc->RegisterKeyWord("SMD_BOTTOM", 0, VT_NONE), VT_NONE, NULL, SA_OVERWRITE, NULL);
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

   G.packageGroup.Empty(); // unigroup is only used in this function. Make sure to clean it before leaving this function
   G.AttribMap = NULL;
   G.ref_des = "";      // refdes
   G.unigroup.Empty();       // unigroup is only used in this function. Make sure to clean it before leaving this function
   polycnt = 0;
   G.layout_def = 0;
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

   for (int i=0; i<pinToUniqueNetZeroCnt; i++)
   {
      VBPinNet pinNet = pinToUniqueNetZeroArray[i];

      // Only add the comp pin to netname that starts with "(NET0)-"
      if (pinNet.netName.Left(strlen("(NET0)-")).MakeUpper() == "(NET0)-"
         && (USE_UNIQUE_NETZERO || !vbNetContainer.IsEmptyRoute(pinNet.netName)))
      {
         NetStruct *net = add_net(fl, pinNet.netName);
         add_comppin(fl, net, compName, pinNet.pinName);
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
      if (pinNet.netName.Left(strlen("(NET0)")).MakeUpper() != "(NET0)")
      {
         NetStruct *net = add_net(fl, pinNet.netName);
         add_comppin(fl, net, compName, pinNet.pinName);
      }
   }
}

/******************************************************************************
* part_pin
*/
int part_pin()
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
   pinNet.m_padstackBlkNum = -1; // not set
   pinToUniqueNetZeroArray.SetAtGrow(pinToUniqueNetZeroCnt++, pinNet);

   int id = cur_ident;
   go_command(part_pin_lst, SIZ_PART_PIN_LST, id);

   if (curVBGeom != NULL)
   {
      pinNet = pinToUniqueNetZeroArray[pinToUniqueNetZeroCnt-1];
      curVBGeom->AddPin(pinNet.pinName, pinNet.pinOption, pinNet.m_padstackBlkNum);
   }

   return 1;
}

/******************************************************************************
* part_silkscreen_outline
*/
int part_silkscreen_outline()
{
   int id = cur_ident;
   int oldstat = cur_status;
   cur_status = FILE_SILK_ARTWORK;

   G.level = "SILKSCREEN_TOP";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(part_graph_lst,SIZ_PART_GRAPH_LST,id);

   cur_status = oldstat;
   return 1;
}

/******************************************************************************
* part_solder_mask
*/
int part_solder_mask()
{
   int id = cur_ident;
   int oldstat = cur_status;
   cur_status = FILE_SOLDER_MASK;

   G.level = PAD_TOP_SOLDERMASK;
   G.lyrnr = Graph_Level(G.level,"",0);

   go_command(part_graph_lst,SIZ_PART_GRAPH_LST,id);

   cur_status = oldstat;
   return 1;
}

/******************************************************************************
* part_pin_option
*/
int part_pin_option()
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
int part_pin_net()
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
int part_pin_xy()
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

// panel
int panelInstanceOrigin_Xy()
{
   CPoint2d origin;
   origin.x = cnv_unit(atof(strtok(NULL,WORD_DELIMETER)));
   origin.y = cnv_unit(atof(strtok(NULL,WORD_DELIMETER)));

   CVbPcbInstances::getPcbInstances().getLast().setOrigin(origin);

   return 1;
}

// panel
int panelInstance_Rotation()
{
   double rotationDegrees = atof(strtok(NULL,WORD_DELIMETER));

   CVbPcbInstances::getPcbInstances().getLast().setRotationRadians(degreesToRadians(rotationDegrees));

   return 1;
}

// panel
int panelInstance_Facement()
{
   CString facement = strtok(NULL,WORD_DELIMETER);
   bool topFacement = (facement.CompareNoCase("top") == 0);

   CVbPcbInstances::getPcbInstances().getLast().setTopFacement(topFacement);

   return 1;
}

// panel
int panelInstance_LocalPath()
{
   CString path = strword();

   CVbPcbInstances::getPcbInstances().getLast().setLocalPath(path);

   return 1;
}

// panel
int panelInstance_SourcePath()
{
   CString path = strword();

   CVbPcbInstances::getPcbInstances().getLast().setSourcePath(path);

   return 1;
}

// panel
int panelInstance_HkpPath()
{
   CString path = strword();

   CVbPcbInstances::getPcbInstances().getLast().setHkpPath(path);

   return 1;
}

/****************************************************************************/
/*
*/
int des_mechanical()
{
   int      id = cur_ident;
   CString  geomname;
   char     *lp;

   lp = strword();
   geomname = lp;

   BlockStruct *b = Graph_Block_Exists(doc, geomname, -1);

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
      Graph_Block_On(GBO_APPEND,geomname,-1,0);
      Graph_Block_Off();
      d = Graph_Block_Reference(geomname, NULL, 0, 
         0.0,0.0, 0.0,0 , 1.0,Graph_Level("0","",1), TRUE);
      d->getInsert()->setInsertType(insertTypeMechanicalComponent);
      G.AttribMap = &d->getAttributesRef();

      Graph_Block_On(GBO_APPEND,geomname,-1,0);
      go_command(part_def_instance_lst,SIZ_PART_DEF_INSTANCE_LST,id);
      Graph_Block_Off();
      G.AttribMap = NULL;
      G.comp_x = 0;
      G.comp_y = 0;
      G.comp_rotation = 0;
   }
   else
   {
      d = Graph_Block_Reference(geomname, NULL, 0, 
         0.0,0.0, 0.0,0 , 1.0,Graph_Level("0","",1), TRUE);
      d->getInsert()->setInsertType(insertTypeMechanicalComponent);
      G.AttribMap = &d->getAttributesRef();
      
      Graph_Block_On(GBO_APPEND, geomname, -1, 0);
      go_command(part_instance_lst,SIZ_PART_INSTANCE_LST,id);
      Graph_Block_Off();
      G.AttribMap = NULL;
   }

   d->getInsert()->setRefname(STRDUP(G.ref_des));
   d->getInsert()->setOriginX(G.comp_x);
   d->getInsert()->setOriginY(G.comp_y);
   d->getInsert()->setAngle(DegToRad(G.comp_rotation));
   

   if (G.sidebot)
   {
      d->getInsert()->setPlacedBottom(true);
   }
   if (G.comp_mirror)
   {
      G.comp_rotation = G.comp_rotation - 180;
      while (G.comp_rotation < 0)   G.comp_rotation += 360;
      while (G.comp_rotation >= 360)   G.comp_rotation -= 360;

      d->getInsert()->setAngle(DegToRad(G.comp_rotation));
      d->getInsert()->setMirrorFlagBits(MIRROR_FLIP | MIRROR_LAYERS);
   }
   polycnt = 0;

   // make sure that Mechanical does not have a ATT_TYPELIST 
   Attrib *a;
   if (a =  is_attvalue(doc, d->getAttributesRef(), ATT_TYPELISTLINK, 1))
   {
      int keyindex = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);
      RemoveAttrib(keyindex, &d->getAttributesRef());
   }

   return 1;
}

/****************************************************************************/
/*
*/
int des_cell()
{
   int      id = cur_ident;
   CString  geomname;
   char     *lp;

   lp = strword();
   geomname = lp;

   BlockStruct *b = Graph_Block_Exists(doc, geomname, -1);

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
      Graph_Block_On(GBO_APPEND,geomname,-1,0L,blockTypeGenericComponent);
      Graph_Block_Off();
      d = Graph_Block_Reference(geomname, NULL, 0, 
         0.0,0.0, 0.0,0 , 1.0,Graph_Level("0","",1), TRUE, blockTypeGenericComponent);
      d->getInsert()->setInsertType(insertTypeGenericComponent);
      G.AttribMap = &d->getAttributesRef();

      BlockStruct* block = Graph_Block_On(GBO_APPEND,geomname,-1,0L,blockTypeGenericComponent);
      go_command(part_def_instance_lst,SIZ_PART_DEF_INSTANCE_LST,id);
      Graph_Block_Off();
      G.AttribMap = NULL;
      G.comp_x = 0;
      G.comp_y = 0;
      G.comp_rotation = 0;
      CString cellName = geomname;
      bool skipThisCell = false;
      set<CString>::iterator it = skipCellList.find(cellName.MakeUpper());
      if(it != skipCellList.end())
      {
         doc->RemoveBlock(block);
         return 1;
      }         
   }
   else
   {
      d = Graph_Block_Reference(geomname, NULL, 0, 
         0.0,0.0, 0.0,0 , 1.0,Graph_Level("0","",1), TRUE, blockTypeGenericComponent);
      d->getInsert()->setInsertType(insertTypeGenericComponent);
      G.AttribMap = &d->getAttributesRef();

      go_command(part_instance_lst,SIZ_PART_INSTANCE_LST,id);
      G.AttribMap = NULL;
   }

   d->getInsert()->setRefname(STRDUP(G.ref_des));
   d->getInsert()->setOriginX(G.comp_x);
   d->getInsert()->setOriginY(G.comp_y);
   d->getInsert()->setAngle(DegToRad(G.comp_rotation));
   

   if (G.sidebot)
   {
      d->getInsert()->setPlacedBottom(true);
   }
   if (G.comp_mirror)
   {
      G.comp_rotation = G.comp_rotation - 180;
      while (G.comp_rotation < 0)   G.comp_rotation += 360;
      while (G.comp_rotation >= 360)   G.comp_rotation -= 360;

      d->getInsert()->setAngle(DegToRad(G.comp_rotation));
      d->getInsert()->setMirrorFlagBits(MIRROR_FLIP | MIRROR_LAYERS);
   }
   polycnt = 0;

   // make sure that Mechanical does not have a ATT_TYPELIST 
   Attrib *a;
   if (a =  is_attvalue(doc, d->getAttributesRef(), ATT_TYPELISTLINK, 1))
   {
      int keyindex = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);
      RemoveAttrib(keyindex, &d->getAttributesRef());
   }

   return 1;
}

/****************************************************************************/
/*
*/
int via_instance_netname()
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
int part_instance_via()
{
   if (USE_CELL_FANOUT_VIAS == TRUE)
   {
      G.layout_def = 1;
      BlockStruct *block = Graph_Block_On(GBO_APPEND,G.name,-1,0);
      net_via();
      Graph_Block_Off();
      G.layout_def = 0;
   }
   else
      vb99_skip();

   return 1;
}
/****************************************************************************/
/*
*/
int net_via()
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
         Graph_Block_On(GBO_APPEND, G.symbol, -1, 0);
         Graph_Block_Off();
         viaBlockName = G.symbol;
      }
      else
      {
         if(viaToLayerName.CompareNoCase("LYR_4"))
            int jj = 0;
         BlockStruct* viaBlock = getLayerSpecificViaBlock(viaFromLayerName, viaToLayerName);

         if (viaBlock != NULL)
         {
            viaBlockName = viaBlock->getName();
         }
      }

      if (!viaBlockName.IsEmpty())
      {
         DataStruct* data = Graph_Block_Reference(viaBlockName, NULL, -1,
               poly_l[0].x, poly_l[0].y, 0.0, 0, 1.0, Graph_Level("0","",1), TRUE, blockTypePadstack);
         data->getInsert()->setInsertType(insertTypeVia);
         if (strlen(G.name))
         {
            vbNetContainer.SetHasViaAt(G.name,true);
            add_net(fl,G.name);  // just incase no netlist is loaded
            doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
               VT_STRING, G.name.GetBuffer(0), SA_APPEND, NULL);
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
   BlockStruct* viaBlock = doc->Find_Block_by_Name(G.symbol, -1, blockTypePadstack);
   if (viaBlock == NULL)
      return NULL;

   CString newBlockName;
   newBlockName.Format("%s_%s_%s", viaBlock->getName(), fromLayerName, toLayerName);
   BlockStruct* newViaBlock = Graph_Block_Exists(doc, newBlockName, -1, blockTypePadstack);
   if (newViaBlock != NULL)
      return newViaBlock;

   // Create block from the original block
   LayerStruct* fromLayer = doc->FindLayer_by_Name(fromLayerName);
   LayerStruct* toLayer = doc->FindLayer_by_Name(toLayerName);

   // The layer name of the top and bottom layer.  "Lyr_" is a reserved prefix for VB Ascii layer name.
   CString startLayer = "Lyr_1";
   CString endLayer;
   endLayer.Format("Lyr_%d", NumberOfLayers);

   if (fromLayerName.CompareNoCase(startLayer) == 0 && toLayerName.CompareNoCase(endLayer) == 0)
   {
      // Check to see if via span from top to bottom, if it is then use original block
      newViaBlock = viaBlock;
      // Fix for dts0100504781 - Via Span Info is not exported
      // Add via span attributes to newViaBlock
      CAttributes *attribMap = newViaBlock->getAttributes();
      Attrib *attrib = NULL;

      bool attribMapExists = false;

      if (newViaBlock->getAttributesRef())
         attribMapExists = true;
      else
         newViaBlock->attributes();

      if(NULL != G.viaSpansSection)
      {
         int numOfViaSpans = G.viaSpansSection->getNumOfElements();
         if ((!attribMapExists) || (!newViaBlock->getAttributes()->Lookup(doc->IsKeyWord(ATT_VIA_SPAN_FROM_LAYER, 0), attrib)))
         {
            bool found = false;
            for (int i=0; i<numOfViaSpans; i++)
            {
               CViaSpan *viaSpan = G.viaSpansSection->getElementAt(i);
               CString padstackName;
               if (viaSpan != NULL)
               {
                  padstackName.Format("XPAD_%s_LYR_%d_LYR_%d", viaSpan->getPadstackName(), viaSpan->getFromLayer(), viaSpan->getToLayer());
                  if (padstackName.CompareNoCase(newBlockName) == 0)
                  {
                     // Now add attributes to &newViaBlock->getAttributesRef()
                     found = true;
                     int viaSpanFromLayer = viaSpan->getFromLayer();
                     int viaSpanToLayer   = viaSpan->getToLayer();

                     doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_FROM_LAYER, 0), VT_INTEGER, &viaSpanFromLayer, SA_APPEND, NULL);
                     doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_TO_LAYER, 0), VT_INTEGER, &viaSpanToLayer, SA_APPEND, NULL);
                     doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_CAPACITANCE, 0), VT_STRING, viaSpan->getCapacitance().GetBuffer(), SA_APPEND, NULL);
                     doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_INDUCTANCE, 0), VT_STRING, viaSpan->getInductance().GetBuffer(), SA_APPEND, NULL);
                     doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_DELAY, 0), VT_STRING, viaSpan->getDelay().GetBuffer(), SA_APPEND, NULL);
                     doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_GRID, 0), VT_STRING, viaSpan->getGrid().GetBuffer(), SA_APPEND, NULL);
                     doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_ATTRIBUTES, 0), VT_STRING, viaSpan->getAttributes().GetBuffer(), SA_APPEND, NULL);

                     // Have added via span info for this span and padstack
                     //G.viaSpansStruct->getViaSpanList().RemoveAt(G.viaSpansStruct->getViaSpanList().FindIndex(i));
                     break;
                  }
               }
            }
         }
      }
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

         LayerStruct* layer = doc->getLayerAt(data->getLayerIndex());
         if (layer == NULL)
            continue;
         CString layerName = layer->getName();

         if (topPad == NULL && (layerName.CompareNoCase(PAD_TOP) == 0 || layerName.CompareNoCase(SMD_TOP) == 0))
         {
            topPad = data->getInsert();
         }
         else if (innerPad == NULL 
            && !STRNICMP(layerName,"PadLayer",strlen("PadLayer"))
            && (LAYTYPE_PAD_INNER == layer->getLayerType()))
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

      newViaBlock = Graph_Block_On(GBO_APPEND, newBlockName, -1, blockTypePadstack);
      newViaBlock->setBlockType(blockTypePadstack);
      doc->CopyAttribs(&newViaBlock->getAttributesRef(), viaBlock->getAttributesRef());

      // Fix for dts0100504781 - Via Span Info is not exported
      // Add via span attributes to newViaBlock
      bool found = false;
      if(NULL != G.viaSpansSection)
      {
         int numOfViaSpans = G.viaSpansSection->getNumOfElements();
         for (int i=0; i<numOfViaSpans; i++)
         {
            CViaSpan *viaSpan = G.viaSpansSection->getElementAt(i);
            if (viaSpan != NULL)
            {
               CString padstackName, originalBlockName;
               padstackName.Format("XPAD_%s_LYR_%d_LYR_%d", viaSpan->getPadstackName(), viaSpan->getFromLayer(), viaSpan->getToLayer());
               originalBlockName.Format("%s_LYR_%d_LYR_%d", viaSpan->getPadstackName(), viaSpan->getFromLayer(), viaSpan->getToLayer());
               newViaBlock->setOriginalName(originalBlockName);
               if (padstackName.CompareNoCase(newBlockName) == 0)
               {
                  // Now add attributes to &newViaBlock->getAttributesRef()
                  found = true;
                  int viaSpanFromLayer = viaSpan->getFromLayer();
                  int viaSpanToLayer   = viaSpan->getToLayer();

                  doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_FROM_LAYER, 0), VT_INTEGER, &viaSpanFromLayer, SA_APPEND, NULL);
                  doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_TO_LAYER, 0), VT_INTEGER, &viaSpanToLayer, SA_APPEND, NULL);
                  doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_CAPACITANCE, 0), VT_STRING, viaSpan->getCapacitance().GetBuffer(), SA_APPEND, NULL);
                  doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_INDUCTANCE, 0), VT_STRING, viaSpan->getInductance().GetBuffer(), SA_APPEND, NULL);
                  doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_DELAY, 0), VT_STRING, viaSpan->getDelay().GetBuffer(), SA_APPEND, NULL);
                  doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_GRID, 0), VT_STRING, viaSpan->getGrid().GetBuffer(), SA_APPEND, NULL);
                  doc->SetAttrib(&newViaBlock->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPAN_ATTRIBUTES, 0), VT_STRING, viaSpan->getAttributes().GetBuffer(), SA_APPEND, NULL);

                  // Have added via span info for this span and padstack
                  //G.viaSpansStruct->getViaSpanList().RemoveAt(G.viaSpansStruct->getViaSpanList().FindIndex(i));
                  break;
               }
            }
         }
      }

      // Insert drill onto drill layer
      if (drill != NULL)
      {
         int layerIndex = Graph_Level("DRILLHOLE", "", FALSE);

         BlockStruct* drillBlock = doc->getBlockAt(drill->getBlockNumber());
         Graph_Block_Reference(drillBlock->getName(), "", drillBlock->getFileNumber(), drill->getOriginX(), drill->getOriginY(),
               drill->getAngle(), drill->getMirrorFlags(), drill->getScale(), layerIndex, 
               drillBlock->getFlags() & BL_GLOBAL, drillBlock->getBlockType());
      }

      // Insert soldermask and paste top if "fromLayer" is the starting layer
      if (fromLayerName.CompareNoCase(startLayer) == 0)
      {
         if (topSolder != NULL)
         {
            InsertStruct* insert = topSolder->getInsert();
            BlockStruct* padBlock = doc->getBlockAt(insert->getBlockNumber());
            Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags(), insert->getScale(), topSolder->getLayerIndex(),
                  padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());
         }

         if (topPaste != NULL)
         {
            InsertStruct* insert = topPaste->getInsert();
            BlockStruct* padBlock = doc->getBlockAt(insert->getBlockNumber());
            Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags(), insert->getScale(), topPaste->getLayerIndex(),
                  padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());
         }
      }

      // Insert soldermask and paste bottom if "toLayer" is the end layer
      if (toLayerName.CompareNoCase(endLayer) == 0)
      {
         if (bottomSolder != NULL)
         {
            InsertStruct* insert = bottomSolder->getInsert();
            BlockStruct* padBlock = doc->getBlockAt(insert->getBlockNumber());
            Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags(), insert->getScale(), bottomSolder->getLayerIndex(),
                  padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());
         }

         if (bottomPaste != NULL)
         {
            InsertStruct* insert = bottomPaste->getInsert();
            BlockStruct* padBlock = doc->getBlockAt(insert->getBlockNumber());
            Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), insert->getOriginX(), insert->getOriginY(),
                  insert->getAngle(), insert->getMirrorFlags(), insert->getScale(), bottomPaste->getLayerIndex(),
                  padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());
         }
      }

      // Insert top pad onto fromLayer
      if (topPad != NULL && fromLayer != NULL)
      {
         BlockStruct* padBlock = doc->getBlockAt(topPad->getBlockNumber());
         Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), topPad->getOriginX(), topPad->getOriginY(),
               topPad->getAngle(), topPad->getMirrorFlags(), topPad->getScale(), fromLayer->getLayerIndex(), 
               padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());
      }

      // Insert inner pad to all layes between fromLayer and toLayer
      if (innerPad != NULL && fromLayer != NULL && toLayer != NULL &&
         fromLayer->getElectricalStackNumber() > 0 && toLayer->getElectricalStackNumber() > 0)
      {
         BlockStruct* padBlock = doc->getBlockAt(innerPad->getBlockNumber());

         for (int i=fromLayer->getElectricalStackNumber()+1; i<toLayer->getElectricalStackNumber(); i++)
         {
            CString layerName;
            layerName.Format("LYR_%d", i);
            int layerIndex = Graph_Level(layerName, "", FALSE);

            Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), innerPad->getOriginX(),
                  innerPad->getOriginY(), innerPad->getAngle(), innerPad->getMirrorFlags(), innerPad->getScale(),
                  layerIndex, padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());            
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
         BlockStruct* padBlock = doc->getBlockAt(bottomPad->getBlockNumber());
         Graph_Block_Reference(padBlock->getName(), "", padBlock->getFileNumber(), bottomPad->getOriginX(), bottomPad->getOriginY(),
               bottomPad->getAngle(), bottomPad->getMirrorFlags(), bottomPad->getScale(), toLayer->getLayerIndex(), 
               padBlock->getFlags() & BL_GLOBAL, padBlock->getBlockType());
      }

      Graph_Block_Off();
   }

   return newViaBlock;
}

/****************************************************************************/
/*
*/
int net_testpoint()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.sidebot = TRUE;
   G.testPointRotation = 0;
   G.AttribMap = NULL;
   G.ref_des = "";   // REF_DES text name

   char* lp = strword();
   CString cellName = lp;

   // Fix for dts0100490364 - In BSXE2007.1/FablinkXE, Assembly & Silk Ref Des for TestPoints is not been exported 
   // through ExportCCz. Need to set G.AttribMap to store the attributes of testpoint
   CAttributes* tempAttr = new CAttributes();
   G.AttribMap = &tempAttr;

   go_command(testpoint_instance_lst,SIZ_TESTPOINT_INSTANCE_LST,id);

   //if (G.ref_des.CompareNoCase("TP15") == 0)
   //{
   //   int iii = 3;
   //}

   // here write via
   if (polycnt)
   {
      // G.name = NetName
      // G.symbol = Padstack

      // Make sure the geometry exists
      BlockStruct* cellBlock = Graph_Block_On(GBO_APPEND,cellName,-1,0);
      Graph_Block_Off();

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
         pinName = "1";

         BlockStruct* padstack =  Graph_Block_On(GBO_APPEND,G.symbol,-1,0);
         Graph_Block_Off();

         BlockStruct* cellBlock = Graph_Block_On(GBO_APPEND,cellName,-1,0);
         DataStruct* pinData = Graph_Block_Reference(G.symbol, pinName, -1, 0.0, 0.0, 0.0, 0, 1.0, Graph_Level("0","",1), TRUE);
         pinData->getInsert()->setInsertType(insertTypePin);
         Graph_Block_Off();
      }

      int mirror = FALSE;
      int testaccessindex = ATT_TEST_ACCESS_TOP;

      if (G.sidebot)
      {
         mirror = TRUE;
         testaccessindex = ATT_TEST_ACCESS_BOT;
      }

      DataStruct* testPointData = Graph_Block_Reference(cellName, G.ref_des, -1, poly_l[0].x,poly_l[0].y, degreesToRadians(G.testPointRotation), mirror, 1.0, Graph_Level("0","",1), TRUE);
      testPointData->getInsert()->setInsertType(insertTypeTestPoint);

#if CamCadMajorMinorVersion > 406  //  > 4.6
      testPointData->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttributeRefName), valueTypeString, G.ref_des.GetBuffer(0), attributeUpdateOverwrite, NULL);
      testPointData->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttributeTest), valueTypeString, testaccesslayers[testaccessindex], attributeUpdateOverwrite, NULL);
#else
      testPointData->setAttrib(doc, doc->getStandardAttributeKeywordIndex(standardAttributeRefName), valueTypeString, G.ref_des.GetBuffer(0), attributeUpdateOverwrite, NULL);
      testPointData->setAttrib(doc, doc->getStandardAttributeKeywordIndex(standardAttributeTest), valueTypeString, testaccesslayers[testaccessindex], attributeUpdateOverwrite, NULL);
#endif

      //dts0100490364 - Now need to translate positions of visible attributes with respect to testpoint location
      for (POSITION pos = tempAttr->GetStartPosition();pos != NULL;)
      {
         Attrib* pAttrib;
         WORD keyword;
         tempAttr->GetNextAssoc(pos, keyword, pAttrib);
         AttribIterator attribIterator(pAttrib);
         Attrib attrib(pAttrib->getCamCadData());
         while (attribIterator.getNext(attrib))
         {
            if (pAttrib->isVisible())
            {
               double x = pAttrib->getCoordinate().x;
               double y = pAttrib->getCoordinate().y;

               x = x - poly_l[0].x;
               y = y - poly_l[0].y;

               double xoff = x, yoff = y;

               if (mirror)
               {
                  Rotate(x, y, 360-G.testPointRotation-180, &xoff, &yoff);
                  xoff = -xoff;
               }
               else
               {
                  Rotate(x, y, 360-G.testPointRotation, &xoff, &yoff);
               }

               pAttrib->setCoordinate(xoff, yoff);
               pAttrib->setRotationDegrees(0.0);
            }
         }
      }

      CAttributes* testPointAttr = testPointData->getAttributesRef();
      testPointAttr->CopyAll(*tempAttr);
      // dts0100490364 fix ends

      if (strlen(G.name))
      {
         NetStruct* net = add_net(fl,G.name);  // just incase no netlist is loaded
         CompPinStruct* comppin = net->addCompPin(G.ref_des, pinName);
      }
   }

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
int part_instance_trace()
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
int part_assembly_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = CELL_ASSEMBLY_OUTLINE;

   G.level = "ASSEMBLY_TOP";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;
   return 1;
}
/****************************************************************************/
/*
*/
int part_instance_shape_name()
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
int part_package_group()
{
   char* lp = strword();
   G.packageGroup = lp;

   if (G.AttribMap)
   {
      doc->SetUnknownAttrib(G.AttribMap, "PACKAGE GROUP", lp,
               SA_OVERWRITE, NULL);
   }
   return 1;
}

/****************************************************************************/
/*
*/
int part_conductive_area()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_CONDUCTIVE_AREA;
   cur_conductive_area_teardrop = false;
   cur_conductive_area_plane = false;
   G.diameter = 0; // reset linewidth to 0
   G.layout_def = 1;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_conductive_area_teardrop = false;
   cur_conductive_area_plane = false;

   cur_status = oldstat;
   G.layout_def = 0;
   return 1;
}

/****************************************************************************/
/*
*/
int part_obstruct()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   char  *lp;
   lp = strtok(NULL,WORD_DELIMETER);
   CString keepout_type(lp);

   G.keepout_type = keepout_type;

   if(!keepout_type.CompareNoCase("TRACE"))
      cur_status = FILE_ROUTE_OBS;
   else if(!keepout_type.CompareNoCase("VIA"))
      cur_status = FILE_VIA_OBS;
   else
      return 1;

   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;
   G.layout_def = 1;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);
   cur_status = oldstat;
   G.layout_def = 0;
   return 1;
}
/****************************************************************************/
/*
*/

int via_instance_layer_pair()
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
int vb99_diameter()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.diameter = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_radius_Rect()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_radius()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_circlediameter()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp)) / 2;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_circleradius()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_startangle()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER); // in degrees
   G.startangle = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_deltaangle()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER); // in degrees
   G.deltaangle = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_height()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.height = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_top()
{
   char  *lp;
  
   lp = strword();
   if((defaultTechnology && topPadName.IsEmpty()) || (false == defaultTechnology))
      topPadName = lp;

   return 1;
}

/******************************************************************************
* padstack_holename
*/
int padstack_holename()
{
   char *tok = strword();
   if((defaultTechnology && holeName.IsEmpty()) || (false == defaultTechnology))
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
int padstack_topsoldermask()
{
   char  *lp;
  
   lp = strword();
   if((defaultTechnology && topSoldermaskPadName.IsEmpty()) || (false == defaultTechnology))
      topSoldermaskPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_topsolderpaste()
{
   char  *lp;
  
   lp = strword();
   if((defaultTechnology && topSolderPastePadName.IsEmpty()) || (false == defaultTechnology))
      topSolderPastePadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_bot()
{
   char  *lp;

   lp = strword();
   if((defaultTechnology && bottomPadName.IsEmpty()) || (false == defaultTechnology))
      bottomPadName = lp;   

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_botsoldermask()
{
   char  *lp;

   lp = strword();
   if((defaultTechnology && bottomSoldermaskPadName.IsEmpty()) || (false == defaultTechnology))
      bottomSoldermaskPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_botsolderpaste()
{
   char  *lp;

   lp = strword();
   if((defaultTechnology && bottomSolderPastePadName.IsEmpty()) || (false == defaultTechnology))
      bottomSolderPastePadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_int()
{
   char  *lp;

   lp = strword();
   if((defaultTechnology && innerPadName.IsEmpty()) || (false == defaultTechnology))
      innerPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_thermal()
{
   char  *lp;

   lp = strword();
   if((defaultTechnology && thermalPadName.IsEmpty()) || (false == defaultTechnology))
      thermalPadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int padstack_clearance()
{
   char  *lp;

   lp = strword();
   if((defaultTechnology && clearancePadName.IsEmpty()) || (false == defaultTechnology))
      clearancePadName = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_shapename()
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
int celpart_description()
{
   char  *lp;

   if ((lp = strword()) != NULL)
   {
      doc->SetUnknownAttrib(&curblock->getAttributesRef(), "DESCRIPTION", lp, SA_OVERWRITE, NULL); //  
   }

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_mount_type()
{
   char  *lp;

   if ((lp = strword()) != NULL)
   {
      if (!STRCMPI(lp,"SURFACE"))
      {
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE,
            NULL,  // this makes a "real" char *
            SA_OVERWRITE, NULL); //
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

   BlockStruct *b = doc->Find_Block_by_Name(p, -1);
   if (b == NULL) return 1;

   int smd = FALSE;
   Attrib *a =  is_attvalue(doc, b->getAttributesRef(), ATT_SMDSHAPE, 2);
   if (a)   smd = TRUE;

   // loop throu file
   POSITION pos = b->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = b->getDataList().GetNext(pos);

      if (np->getDataType() == T_INSERT)
      {
         // get padshape name and layer
         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
         LayerStruct *l = doc->FindLayer(np->getLayerIndex());
      
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
                          RadToDeg(np->getInsert()->getAngle()), smd);
            continue;
         }

         // here now make the layers correct
         for (int i=fromlayer;i<=tolayer;i++)
         {
            CString  tmp;
            tmp.Format("PadLayer%d",i);
            int   lyrnr = Graph_Level(tmp,"", 0);
            update_curpad(block->getName(), lyrnr, np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), RadToDeg(np->getInsert()->getAngle()), smd);
         }
      }
   }

   return 0;
}

/****************************************************************************/
/*
*/
int jobprefs_userlayer()
{
   int   id = cur_ident;

   go_command(userlayer_lst,SIZ_USERLAYER_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int jobprefs_padstack_technology()
{
   char  *lp;
   int   id = cur_ident;

   lp = strword();
   
   if(strlen(lp) > 0)
   {
      USE_TECHNOLOGY = lp;
      USE_TECHNOLOGY.TrimLeft();
      USE_TECHNOLOGY.TrimRight();
   }

   return 1;
}

/****************************************************************************/
/*
*/
int netprops_netname()
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
int pdb_typenumber()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.name = lp;
   G.name.MakeUpper();

   curtype = AddType(fl, G.name);

   go_command(pdb_typenumber_lst,SIZ_PDB_TYPENUMBER_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int netprops_netclass()
{
   char  *lp;

   lp = strword();

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int jobprefs_layerstackup()
{
   int   id = cur_ident;

   go_command(layerstackup_lst,SIZ_LAYERSTACKUP_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int jobprefs_via_spans()
{
   int   id = cur_ident;

   FileStruct *currFile = doc->getFileList().FindByFileNumber(cur_filenum);
   G.viaSpansSection = currFile->GetViaSpansSection();

   go_command(via_spans_lst,SIZ_VIA_SPANS_LST,id);

   if ( !currFile->getBlock()->getAttributesRef() )
      currFile->getBlock()->attributes();

   // Fix for dts0100504781 -	Via Span Info is not exported
   doc->SetAttrib(&currFile->getBlock()->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPANS_GENERAL_CLEARANCE, 0), 
      VT_STRING, G.viaSpansSection->getGeneralClearance().GetBuffer(), SA_APPEND, NULL);
   doc->SetAttrib(&currFile->getBlock()->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPANS_SAME_NET_CLEARANCE, 0), 
      VT_STRING, G.viaSpansSection->getSameNetClearance().GetBuffer(), SA_APPEND, NULL);
   doc->SetAttrib(&currFile->getBlock()->getAttributesRef(), doc->IsKeyWord(ATT_VIA_SPANS_USE_MNT_OPP_PADS, 0), 
      VT_STRING, G.viaSpansSection->getUseMntOppPads().GetBuffer(), SA_APPEND, NULL);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_general_clearance()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.viaSpansSection->setGeneralClearance(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_same_net_clearance()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.viaSpansSection->setSameNetClearance(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_use_mnt_opp_pads()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.viaSpansSection->setUseMntOppPads(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack()
{
   int   id = cur_ident;
   char  *lp;

   CViaSpan *temp = new CViaSpan();
   G.viaSpansSection->addElement(temp);
   G.curViaSpan = temp;

   lp = strword();
   G.curViaSpan->setPadstackName(lp);
   
   go_command(via_spans_padstack_lst,SIZ_VIA_SPANS_PADSTACK_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack_fromLayer()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.curViaSpan->setFromLayer(atoi(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack_toLayer()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.curViaSpan->setToLayer(atoi(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack_capacitance()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.curViaSpan->setCapacitance(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack_inductance()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.curViaSpan->setInductance(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack_delay()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.curViaSpan->setDelay(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack_grid()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.curViaSpan->setGrid(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int via_spans_padstack_attributes()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.curViaSpan->setAttributes(lp);

   return 1;
}


/****************************************************************************/
/*
*/
int layerstackup_dialectric()
{
   int      oldstat = cur_status;
   int      id = cur_ident;
   int      stacknr;
   char     *lp;
   CString  ul;
   int      ltyp;

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
         ltyp = LAYTYPE_MASK_TOP;
      }
      else
      {
         G.level = PAD_BOT_SOLDERMASK;
         ltyp = LAYTYPE_MASK_BOTTOM;
      }
   }
   else
   {
      ul.Format("DIALECTRIC_%d", ++G.dialeccnt);
      ltyp = LAYTYPE_DIALECTRIC;
   }

   int lnr = Graph_Level(ul, "", 0);
   LayerStruct *l = doc->FindLayer(lnr);
   l->setComment(G.level);
   l->setLayerType(ltyp);
   l->setArtworkStackNumber(stacknr);
      
   if (G.thickness > 0)
   {
      doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_THICKNESS, 0),
            VT_UNIT_DOUBLE, &G.thickness, SA_OVERWRITE, NULL); //  
   }
   doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_DIELECTRIC_CONSTANT, 0),
            VT_STRING, G.lay_dielectric_const.GetBuffer(), SA_OVERWRITE, NULL);

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
int layerstackup_plane()
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

   int lnr = Graph_Level(ul, "", 0);
   LayerStruct *l = doc->FindLayer(lnr);
   l->setComment(G.level);
   l->setElectricalStackNumber(G.lyrnr);
   l->setArtworkStackNumber(stacknr);

   // Case 1585, if plane_type (saved in cur_status) is "positive", then set
   // layer type in same fashion as SIGNAL_PLANE. It would have been nice to
   // just call layerstackup_signal, but we don't know which way it will
   // until we're into it.
   if (cur_status == FILE_POS_PLANE)
   {
      // dts0100504777 - Plane layers are also tagged as signal layers.
      l->setLayerType(LAYTYPE_POWERPOS);
   }
   else
   {
      // How it was before case 1585
      l->setLayerType(LAYTYPE_POWERNEG);
   }
   
      
   if (G.thickness > 0)
   {
      doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_THICKNESS, 0),
            VT_UNIT_DOUBLE,
            &G.thickness,
            SA_OVERWRITE, NULL); // x, y, rot, height
   }
   if ( !G.lay_resistivity.IsEmpty() )
   {
      doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_RESISTIVITY, 0),
         VT_STRING, G.lay_resistivity.GetBuffer(), SA_OVERWRITE, NULL);
      G.lay_resistivity.Empty();
   }


   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int layerstackup_signal()
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

   int lnr = Graph_Level(ul, "", 0);
   LayerStruct *l = doc->FindLayer(lnr);
   l->setComment(G.level);

   if (G.lyrnr == 1)
      l->setLayerType(LAYTYPE_SIGNAL_TOP);
   else
   if (G.lyrnr == NumberOfLayers)
      l->setLayerType(LAYTYPE_SIGNAL_BOT);
   else
      l->setLayerType(LAYTYPE_SIGNAL_INNER);
   l->setElectricalStackNumber(G.lyrnr);
   l->setArtworkStackNumber(stacknr);

   if (G.thickness > 0)
   {
      doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_THICKNESS, 0),
            VT_UNIT_DOUBLE,
            &G.thickness,
            SA_OVERWRITE, NULL); // x, y, rot, height
   }
   if ( !G.lay_resistivity.IsEmpty() )
   {
      doc->SetAttrib(&l->getAttributesRef(),doc->IsKeyWord(LAYATT_RESISTIVITY, 0),
         VT_STRING, G.lay_resistivity.GetBuffer(), SA_OVERWRITE, NULL);
      G.lay_resistivity.Empty();
   }

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int layercontens_description()
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
int layercontens_plane_type()
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
int layercontens_plane_net()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();   // this needs to be implemented in a netname array, because multiple netnames
                     // can appear here

   if (G.lyrnr && lp && strlen(lp))
   {
      CString  ul;
      ul.Format("LYR_%d", G.lyrnr);
      int lnr = Graph_Level(ul, "", 0);
      LayerStruct *l = doc->FindLayer(lnr);

      doc->SetUnknownAttrib(&l->getAttributesRef(),ATT_NETNAME,lp, SA_APPEND, NULL); // x, y, rot, height

      // update layer as a powerplane - powernet
      NetStruct *n = add_net(file,lp);
      doc->SetAttrib(&n->getAttributesRef(),doc->IsKeyWord(ATT_POWERNET, 0),
            VT_NONE, NULL, SA_OVERWRITE, NULL); //
   }

   vb99_skip();

   return 1;
}

/****************************************************************************/
/*
*/
int layercontens_thickness()
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
int layercontens_resistivity()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   CString  layer_resistivity = lp;
   G.lay_resistivity = layer_resistivity;

   return 1;
}

/****************************************************************************/
/*
*/

int layercontens_dielectric()
{
   int   id = cur_ident;
   char  *lp;

   lp = strword();
   G.lay_dielectric_const = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int layercontens_conductivenr()
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
int userlayer_layername()
{
   int   id = cur_ident;
   CString  user_layer_name, ul;
   char  *lp;

   lp = strword();
   user_layer_name = lp;

   G.lyrnr = 0;
   // get user_layer_number
   go_command(userlayernum_lst,SIZ_USERLAYERNUM_LST,id);

   if(user_layer_name.IsEmpty())
      ul.Format("USER_LVL_%d", G.lyrnr);
   else
      ul = user_layer_name;

   int lnr = Graph_Level(ul, "", 0);
   LayerStruct *l = doc->FindLayer(lnr);
   l->setComment(user_layer_name);

   return 1;
}

/****************************************************************************/
/*
*/
int userlayer_layernumber()
{
   char  *lp;

   lp = strword();
   G.lyrnr = atoi(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_via()
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
         Graph_Block_On(GBO_APPEND,G.symbol,-1,0);
         Graph_Block_Off();

         // G.symbol = Padstack
         DataStruct *d = Graph_Block_Reference(G.symbol, NULL, -1, poly_l[0].x,poly_l[0].y, 
            0.0, 0, 1.0, Graph_Level("0","",1), TRUE);
         d->getInsert()->setInsertType(insertTypeVia);
      }
   }
   polycnt = 0;

   return 1;
}

int celpart_package_group()
{
   char* lp = strword();
   G.packageGroup = lp;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_pin()
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

   // make sure pin geom (padstack) exists
   BlockStruct *padstackBlk = NULL;
   padstackBlk = Graph_Block_Exists(doc, G.symbol, -1, blockTypeUndefined, true);
   CString padstackName = G.symbol.Right(G.symbol.GetLength() - 5);
   if(NULL == padstackBlk)
   {
      set<CString>::iterator it = padstacksWithDifferentReferenceNames.find(padstackName);
      if(padstacksWithDifferentReferenceNames.end() == it) // We need to output message only once
      {
         fprintf(ferr,"Padstack %s not found even with case insensitive search.\n",padstackName);
         padstacksWithDifferentReferenceNames.insert(padstackName);
      }
   }
   else if((NULL != padstackBlk) && padstackBlk->getName().Compare(G.symbol))
   {
      set<CString>::iterator it = padstacksWithDifferentReferenceNames.find(padstackName);
      if(padstacksWithDifferentReferenceNames.end() == it) // We need to output message only once
      {
         fprintf(ferr,"Padstack %s not found with case sensitive search.\n",padstackName);
         padstacksWithDifferentReferenceNames.insert(padstackName);
      }
   }

   if(NULL != padstackBlk) // This should never happen
   {
      CVBGeom* vbGeom = VBGeomList.GetLastGeom();
      if (vbGeom != NULL)
         vbGeom->AddPin(G.pinname, G.name_1, padstackBlk->getBlockNumber());

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

      DataStruct *d = Graph_Block_Reference(padstackBlk->getBlockNumber(), G.pinname, poly_l[0].x, poly_l[0].y,
         DegToRad(G.rotation + rot), mir, 1.0, Graph_Level("0", "", 1));

      int btype = default_inserttype(padstackBlk->getBlockType()); // this could be fiducial or mounting hole

      if (btype)
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
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, TRUE),
               VT_INTEGER, &pnr, SA_OVERWRITE, NULL); //  
         }
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
      
      if (np->getDataType() == T_INSERT)        
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {
            // pinname is in refname if a insert

            // these are absolute instructions.
            Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_COMPPINNR, 0);
            if (a)
            {
               if (a->getValueType() == VT_INTEGER)
               {
                  // int   pt = a->getIntValue();
                  if (pinnr == a->getIntValue())
                  {
                     np->getInsert()->setRefname(STRDUP(pinname));
                     found++;
                     break;
                  }
               }
               else
               if (a->getValueType() == VT_STRING)
               {
#if CamCadMajorMinorVersion > 406  //  > 4.6
                  CString l = a->getStringValue();
#else
                  CString l = doc->ValueArray[a->getStringValueIndex()];
#endif
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
int vb99_mh()
{
   int id = cur_ident;
   
   G.symbol = "";
   polycnt = 0;
   G.diameter = 0;
   go_command(mh_instance_lst, SIZ_MH_INSTANCE_LST, id);

   if (!strlen(G.symbol))
      G.symbol.Format("MH_%lg", G.diameter);

   if (Graph_Block_Exists(doc, G.symbol, -1) == NULL)
   {
      int layerNum = Graph_Level("DRILLHOLE", "", 0);

      CString drillName;
      drillName.Format("DRILL_%d", get_drillindex(G.diameter, layerNum));
      Graph_Block_Reference(drillName, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerNum, TRUE);

      BlockStruct *block = Graph_Block_On(GBO_APPEND, G.symbol, -1, 0);
      block->setBlockType(BLOCKTYPE_TOOLING);
      Graph_Block_Reference(drillName, NULL, 0, 0, 0, 0.0, 0 , 1.0, layerNum, TRUE);
      Graph_Block_Off();
   }

   DataStruct *data = Graph_Block_Reference(G.symbol, NULL, 0, poly_l[0].x, poly_l[0].y, 0.0, 0 , 1.0, -1, TRUE);
   
   if(curblock->getBlockType() == blockTypePcbComponent || curblock->getBlockType() == blockTypeTestPoint)  
      data->getInsert()->setInsertType(insertTypeMechanicalPin);
   else
      data->getInsert()->setInsertType(insertTypeDrillTool);

   int holeTypeKeywordIndex = doc->RegisterKeyWord("ExpeditionHoleType",0,valueTypeString);

   doc->SetAttrib(&data->getAttributesRef(),holeTypeKeywordIndex,VbHoleTypeTagToString(vbHoleType),SA_OVERWRITE, NULL); //  

   polycnt = 0;

   return 1;
}

int vbMountingHole()
{
   vbHoleType = vbHoleTypeMounting;

   return vb99_mh();
}

int vbShearingHole()
{
   vbHoleType = vbHoleTypeShearing;

   return vb99_mh();
}

int vbToolingHole()
{
   vbHoleType = vbHoleTypeTooling;

   return vb99_mh();
}

/****************************************************************************/
/*
*/
int celpart_silk_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SILK_ARTWORK;

   G.level = "SILKSCREEN_TOP";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_conductive_area()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   cur_status = FILE_CELL_CONDUCTIVE_AREA;

   G.diameter = 0; // reset linewidth to 0

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_obstruct()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   char  *lp;
   lp = strtok(NULL,WORD_DELIMETER);
   CString keepout_type(lp);

   G.keepout_type = keepout_type;

   if(!keepout_type.CompareNoCase("TRACE"))
      cur_status = FILE_ROUTE_OBS;
   else if(!keepout_type.CompareNoCase("VIA"))
      cur_status = FILE_VIA_OBS;
   else
      return 1;

   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;
   G.layout_def = 1;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);
   cur_status = oldstat;
   G.layout_def = 0;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_contour()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   cur_status = FILE_CONTOUR;

   CString contourType = strtok(NULL,WORD_DELIMETER);
   cur_status = FILE_CONTOUR;

   G.level = "LAYTYPE_ALL";
   G.lyrnr = Graph_Level("LAYTYPE_ALL","",0);
   LayerStruct *l = doc->FindLayer(G.lyrnr);
   l->setLayerType(LAYTYPE_ALL);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;
   return 1;
}

/****************************************************************************/
/*
*/
int celpart_placement_keepout()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PLACE_OBS;

   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;

   double height = G.height;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;
   return 1;
}

/****************************************************************************/
/*
*/
int celpart_trace()
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
      G.diameter = 0.;

      go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

      cur_status = oldstat;
   }

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_fiducial()
{
   int   id = cur_ident;
   
   G.sidebot = FALSE;
   G.rotation = 0.;
   G.symbol = "";
   polycnt = 0;
   go_command(fiducial_instance_lst,SIZ_FIDUCIAL_INSTANCE_LST,id);

   int      mir = 0;

   if (!strlen(G.symbol))
      G.symbol.Format("FIDC_%lg",G.radius*2);

   // just make sure, it's there.
   if (Graph_Block_Exists(doc, G.symbol, -1) == NULL)
   {
      BlockStruct *b;
      b = Graph_Block_On(GBO_APPEND,G.symbol,-1,0);

      int err;
      CString  appname;
      appname.Format("FIDAPP_%lg",G.radius*2);

      Graph_Aperture(appname, T_TARGET, G.radius*2, G.radius, 0.0, 0.0, 0.0, 0, 
         BL_APERTURE, TRUE, &err);
      Graph_Block_Reference(appname, NULL, 0, 0, 0, 0.0, 0 , 1.0, -1, TRUE);
      Graph_Block_Off();
      b->setBlockType(BLOCKTYPE_FIDUCIAL);
   }
   
   DataStruct *d = Graph_Block_Reference(G.symbol, NULL, 0, poly_l[0].x,poly_l[0].y,
               DegToRad(G.rotation), (G.sidebot) , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeFiducial);
   d->getInsert()->setPlacedBottom((G.sidebot));

   polycnt = 0;

   return 1;
   return 1;
}

/****************************************************************************/
/*
*/
int vb99_gluespot()
{
   int   id = cur_ident;
   
   G.sidebot = FALSE;
   G.symbol = "";
   polycnt = 0;

   go_command(gluespot_lst,SIZ_GLUESPOT_LST,id);

   int      mir = 0;

   BlockStruct *gluepoint = generate_Gluepoint_Geom(doc);
   DataStruct *d = Graph_Block_Reference(gluepoint->getName(), NULL, 0, poly_l[0].x,poly_l[0].y,
               0.0, mir , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeGluePoint);
   if (G.sidebot) d->getInsert()->setPlacedBottom(true);

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_placement_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PLACEMENT_OUTLINE;

   G.level = "PLACEMENT_OUTLINE";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.height = 0;
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   if (G.height > 0 && curblock)
      doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE, &G.height, SA_OVERWRITE, NULL);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_assembly_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = CELL_ASSEMBLY_OUTLINE;

   G.level = "ASSEMBLY_TOP";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int celpart_insertion_outline()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = CELL_INSERTION_OUTLINE;

   G.level = "INSERTION_TOP";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
   Outlines and polies are not in sequence
*/
int write_poly()
{
   int widthindex;
   int i, err;
   int fill = 0; //planeShapeFlag;
   int voidtype = 0;
   int closed = 0;
   int poured = 0;
   int hidenFlag = 0;

   if (polycnt == 0) 
      return 1;

   if(Component_Options == "NOT_PLACED")
      return 1;

   // This is not the composite component and the data is from layout.hkp. We will not create the poly

   // If the call comes from des_part, we would create a temp goemetry to store to data
   if (!G.unigroup.IsEmpty())
      Graph_Block_On(GBO_APPEND,"Temp_Poly", -1, 0);

   if (G.diameter == 0)
      widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   else
      widthindex = Graph_Aperture("", T_ROUND, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *curdata = Graph_PolyStruct(G.lyrnr, 0L, 0);

   if (actual_plane == 1)
   {
      cur_actual_plane_num = curdata->getEntityNumber();
      actual_plane = 0;
   }

   // here need to do TRACE_OBS   
   //                 VIA_OBS
   //                 PLAC_OBS

   closed = (poly_l[0].x == poly_l[polycnt-1].x && poly_l[0].y == poly_l[polycnt-1].y);

   if (cur_status == FILE_SILK_ARTWORK)
   {
   }
   else if (cur_status == FILE_PLACEMENT_OUTLINE)
   {
      curdata->setGraphicClass(GR_CLASS_COMPOUTLINE);
   }
   else if (cur_status == FILE_POS_PLANE)
   {
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(GR_CLASS_ETCH);
         add_net(fl, G.name);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_ETCH_TYPE, TRUE),valueTypeString,ETCH_TYPE_PLANE,attributeUpdateAppend,NULL);
      }
      fill = cur_fill;
   }
   else if (cur_status == FILE_NEG_PLANE)
   {
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(GR_CLASS_ETCH);
         curdata->setNegative(true);  // case dts0100470139, but look at post process UpdateNegativeLayers() for final result
         add_net(fl, G.name);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_ETCH_TYPE, TRUE),valueTypeString,ETCH_TYPE_PLANE,attributeUpdateAppend,NULL);
      }
      fill = cur_fill;
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
         curdata->setGraphicClass(GR_CLASS_ETCH);
         add_net(fl, G.name);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_ETCH_TYPE, TRUE),valueTypeString,ETCH_TYPE_PLANE_OUTLINE,attributeUpdateAppend,NULL);
      }
   }
   else if (cur_status == FILE_PLANE_HOLE)
   {
      closed = 1;
      fill = 1;
      voidtype = 1;
      
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(GR_CLASS_ETCH);
         add_net(fl, G.name);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_ETCH_TYPE, TRUE),valueTypeString,ETCH_TYPE_PLANE_HOLE,attributeUpdateAppend,NULL);
      }
   }
   else if (cur_status == FILE_ROUTE_OUTL)
   {
      curdata->setGraphicClass(GR_CLASS_ROUTKEEPIN);
   }
   else if (cur_status == FILE_BOARD_OUTL)
   {
      curdata->setGraphicClass(GR_CLASS_BOARDOUTLINE);
   }
   else if (cur_status == FILE_MANUFACTURING_OUTL)
   {
      curdata->setGraphicClass(GR_CLASS_MANUFACTURINGOUTL);
   }
   else if (cur_status == FILE_DOCUMENTATION)
   {
      curdata->setGraphicClass(GR_CLASS_DRAWING);
   }
   else if (cur_status == FILE_SCORING_LINE)
   {
      curdata->setGraphicClass(GR_CLASS_DRAWING);
   }
   else if (cur_status == FILE_SHEARING_LINE)
   {
      curdata->setGraphicClass(GR_CLASS_DRAWING);
   }
   else if (cur_status == FILE_PANEL_OUTL) // panel
   {
      curdata->setGraphicClass(GR_CLASS_PANELOUTLINE);
   }
   else if (cur_status == FILE_PANEL_BORDER) // panel
   {
      curdata->setGraphicClass(GR_CLASS_PANELBORDER);
   }
   else if (cur_status == FILE_ASSM_OUTL)
   {
      curdata->setGraphicClass(GR_CLASS_COMPOUTLINE);
   }
   else if (cur_status == FILE_ROUTE_OBS)
   {
      curdata->setGraphicClass(GR_CLASS_ROUTKEEPOUT);
   }
   else if (cur_status == FILE_PLACE_OBS)
   {
      curdata->setGraphicClass(GR_CLASS_PLACEKEEPOUT);
      double height = G.height;
      doc->SetAttrib(&curdata->getAttributesRef(), doc->IsKeyWord(ATT_HEIGHT, 1), VT_UNIT_DOUBLE, &height, SA_OVERWRITE, NULL);
   }
   else if (cur_status == FILE_VIA_OBS)
   {
      curdata->setGraphicClass(GR_CLASS_VIAKEEPOUT);
   }
   else if (cur_status == CELL_FANOUT)
   {
      curdata->setGraphicClass(GR_CLASS_TRACEFANOUT);
      add_net(fl, G.name);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
   }
   else if (cur_status == FILE_CONTOUR)
   {
      curdata->setGraphicClass(GR_CLASS_CONTOUR);
      doc->SetUnknownAttrib(&curdata->getAttributesRef(),"COMP", contourComp.GetBuffer(), SA_OVERWRITE, NULL);
      doc->SetUnknownAttrib(&curdata->getAttributesRef(),"DIRECTION", contourDirection.GetBuffer(), SA_OVERWRITE, NULL);
      doc->SetUnknownAttrib(&curdata->getAttributesRef(),"HOLE", contourHole.GetBuffer(), SA_OVERWRITE, NULL);

      CString temp;
      temp.Format(".HOLE_%s", contourHole);
      
      int holeGeomNum = doc->Get_Block_Num(temp.GetBuffer(), -1, 0);
      temp.Format("%d", holeGeomNum);
      doc->SetUnknownAttrib(&curdata->getAttributesRef(),"HOLE_GEOM", temp.GetBuffer(), SA_OVERWRITE, NULL);
   }
   else if (cur_status == FILE_TRACE)
   {
      curdata->setGraphicClass(GR_CLASS_ETCH);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_ETCH_TYPE, TRUE),valueTypeString,ETCH_TYPE_TRACE,attributeUpdateAppend,NULL);
      if (strlen(G.name))  // G.name is netname
      {
         vbNetContainer.SetHasTraceAt(G.name,true);
         add_net(fl, G.name);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
      }
   }
   else if (cur_status == FILE_CELL_CONDUCTIVE_AREA)
   {
      curdata->setGraphicClass(GR_CLASS_ETCH);
      if (!G.netName.IsEmpty())  // G.netName is netname
      {
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),valueTypeString,G.netName.GetBuffer(0),attributeUpdateAppend,NULL);
      }
      fill = cur_fill;
   }
   else if (cur_status == FILE_CONDUCTIVE_AREA)
   {
      if (strlen(G.name))  // G.name is netname
      {
         curdata->setGraphicClass(GR_CLASS_ETCH);
         add_net(fl, G.name);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),valueTypeString,G.name.GetBuffer(0),attributeUpdateAppend,NULL);
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_ETCH_TYPE, TRUE),valueTypeString,ETCH_TYPE_CONDUCTIVE,attributeUpdateAppend,NULL);
      }
      closed = 1;
      fill = 0;

      //  dts0100487384
      // The code commented out is pre-dst0100487384. It can deleted after this settles awhile and we get
      // no uproar from some other faction that liked it how it was.
      // The essence of it was prior to change, this importer imposed a relationship between "generated data" and
      // the "conductive area". The relationship was either/or, you could have one or the other but not both.
      // Evidence in this case suggests that is not a correct relationship, that these should be independent.
/////#define ORIGINAL_STUFF
#ifdef  ORIGINAL_STUFF
      if (!use_generated_data || cur_conductive_area_teardrop) // Case dts0100447290 says ignore use_generated_data flag and always honor SHAPE_OPTIONS (fill) for teardrops
         fill = cur_fill;
       else 
         hidenFlag = TRUE; 
#else
      // Case dts0100487384 fix.
      // The change boils down to just always honoring the fill flag and never setting hidden.
      fill = cur_fill;
#endif

      if (!cur_conductive_area_teardrop) // Case dts0100447290 says don't set Boundary on teardrops
         poured = TRUE; // this is only the boundary, no voids etc.. They are in the Generated_Data section as lines and arcs.
   }
   else if (cur_status == FILE_SOLDER_MASK)
   {
      // Case 1584, apply fill code to solder mask items
      if (closed)
         fill = cur_fill;
   } 

   if (apsHatchFlag || apsTieLegFlag)
      fill = 0;

   CPoly *ppoly = Graph_Poly(NULL,  widthindex, fill, voidtype, closed);   // fillflag , negative, closed
   if (poured)
      ppoly->setFloodBoundary(true);
   if (hidenFlag)
      ppoly->setHidden(true);

   if (isInGeneratedData && use_generated_data && !planeShapeFlag)
   {
      ppoly->setHatchLine(true);
   }

   /* NEED TO HIDE ONLY PLANE SHAPES */
   if (!apsHatchFlag && !apsTieLegFlag && cur_conductive_area_plane)
      getPolyarcShapeDataList()->AddTail(curdata);

   CPnt *lastvertex = NULL;

   for (i=0;i<polycnt;i++)
   {
      if (fabs(poly_l[i].bulge) > 0)   // this is the radius and center point
      {
         // if bulge < 0 it is counter clockwise, > 0 is clockwise

         if (lastvertex && i < (polycnt-1))
         {
            double r,sa,da;
            bool cwFlag = (poly_l[i].bulge >= 0.);

            ArcCenter2(poly_l[i-1].x, poly_l[i-1].y, 
                       poly_l[i+1].x, poly_l[i+1].y,
                       poly_l[i  ].x, poly_l[i  ].y,
                       &r, &sa, &da, cwFlag );

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
               double bulge = tan(da/4.);

               // Case dts0100461877 - be careful that deltas close to zero don't turn into full circles!
               if (fabs(da) > 1.0 && radiansApproximatelyEqual(fabs(da),TwoPi,.0001))
               {
                  bulge = (bulge < 0. ? -1. : 1.);

                  lastvertex->bulge = (DbUnit)bulge;

                  double dx = poly_l[i].x - poly_l[i-1].x;
                  double dy = poly_l[i].y - poly_l[i-1].y;
                  double  x = poly_l[i].x + dx;
                  double  y = poly_l[i].y + dy;

                  lastvertex = Graph_Vertex(x,y,bulge);
               }

               lastvertex->bulge = (DbUnit)bulge;
            }
         }
      }
      else
      {
         lastvertex = Graph_Vertex(poly_l[i].x, poly_l[i].y, poly_l[i].bulge);
      }
   }

   if (!G.unigroup.IsEmpty())
      Graph_Block_Off();

   cur_fill = 0;
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
int des_line()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   
   polycnt = 0;
   G.diameter = 0.;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_sym()
{
   int   id = cur_ident;
   vb99_null(id);
   return 1;
}

/****************************************************************************/
/*
*/
int des_arc()
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
int des_circle()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   DbFlag flg = 0;

   cur_status = FILE_TRACE;

   polycnt = 0;
   G.diameter = 0;
   cur_fill = 0;
   G.diameter = 0.;

   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);
   write_circle();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_graphic()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   cur_fill  = FALSE;
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}
/****************************************************************************/
/*
*/
int vb99_drillgraphic()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   cur_fill  = FALSE;

   G.level = "DRILLDRAWING_THRU"; // has not an own layer, this is the layername that is used in VB
   G.lyrnr = Graph_Level(G.level,"",0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_route_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_ROUTE_OUTL;
   G.level = "ROUTE_OUTL";
   G.lyrnr = Graph_Level("ROUTE_OUTL","",0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_generated_data()
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
int des_assembly_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_ASSEMBLY_OUTL;
   G.level = "ASSEMBLY_TOP";
   G.lyrnr = Graph_Level("ASSEMBLY_TOP","",0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

int des_panel()  // Panel
{
   file->setBlockType(blockTypePanel);

   return 1;
}

int des_panel_outl()  // Panel
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PANEL_OUTL;
   G.diameter = 0.;
   G.level = "PanelOutline";
   G.lyrnr = Graph_Level(G.level,"",0);
   LayerStruct *lp = doc->FindLayer(G.lyrnr);
   if (lp != NULL)
      lp->setLayerType(layerTypePanelOutline);

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);
   file->setBlockType(blockTypePanel);
   cur_status = oldstat;

   return 1;
}

int des_panel_border()  // Panel
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PANEL_BORDER;
   G.lyrnr = Graph_Level("0","",0);
   G.diameter = 0.;
   LayerStruct *lp = doc->FindLayer(G.lyrnr);
   if (lp != NULL)
      lp->setLayerType(LAYTYPE_UNKNOWN);

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

int des_panelBoardInstance()
{
   int   id = cur_ident;
   CString refDes = strtok(NULL,WORD_DELIMETER);

   CVbPcbInstances::getPcbInstances().addPanelInstance(refDes);

   go_command(panelInstance_lst,SIZ_PanelInstance_LST,id);

   return 1;
}

int panelInstance_Origin()
{
   int   id = cur_ident;

   go_command(panelInstanceOrigin_lst,SIZ_PanelInstanceOrigin_LST,id);

   return 1;
}

/****************************************************************************/
/**********************   Read in Contour information   *********************/
int des_contour()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   CString contourType = strtok(NULL,WORD_DELIMETER);
   cur_status = FILE_CONTOUR;

   G.level = "LAYTYPE_ALL";
   G.lyrnr = Graph_Level("LAYTYPE_ALL","",0);
   LayerStruct *l = doc->FindLayer(G.lyrnr);
   l->setLayerType(LAYTYPE_ALL);
   G.diameter = 0.;
   
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

int contour_comp()
{
   char* lp = strword();
   contourComp = lp;
   return 1;
}

int contour_direction()
{
   char* lp = strword();
   contourDirection = lp;
   return 1;
}

int contour_hole()
{
   char* lp = strword();
   contourHole = lp;
   return 1;
}

/****************************************************************************/
/*
*/
int des_drill_graphic()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = 0;
   G.level = "DRILLSYMBOL";
   G.lyrnr = Graph_Level("DRILLSYMBOL","",0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/

int des_obstruct()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   char  *lp;
   lp = strtok(NULL,WORD_DELIMETER);
   CString keepout_type(lp);

   G.keepout_type = keepout_type;

   if(!keepout_type.CompareNoCase("TRACE"))
      cur_status = FILE_ROUTE_OBS;
   else if(!keepout_type.CompareNoCase("VIA"))
      cur_status = FILE_VIA_OBS;
   else
      return 1;

   // If there is not layer definition, it would be layer_all
   G.level = "LAYTYPE_ALL";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);
   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_placement_keepout()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PLACE_OBS;

   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;

   double height = G.height;
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/

int des_silkscreen_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SILK_ARTWORK;

   G.level = "SILKSCREEN_TOP";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0;
   
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_insertion_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_INSERTION_OUTL;
   G.level = "INSERTION_TOP";
   G.lyrnr = Graph_Level("INSERTION_TOP","",0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_solder_mask()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SOLDER_MASK;
   G.level = PAD_TOP_SOLDERMASK;
   G.lyrnr = Graph_Level(G.level,"",0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/

int vb99_solder_paste()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SOLDER_PASTE;
   G.level = PAD_TOP_SOLDERPASTE;
   G.lyrnr = Graph_Level(G.level,"",0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_board_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_BOARD_OUTL;

   G.level = "BOARD_OUTL";
   G.lyrnr = Graph_Level("BOARD_OUTL","",0);
   LayerStruct *l = doc->FindLayer(G.lyrnr);
   l->setLayerType(LAYTYPE_BOARD_OUTLINE);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_manufacturing_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_MANUFACTURING_OUTL;

   G.lyrnr = Graph_Level("0","",0);
   LayerStruct *lp = doc->FindLayer(G.lyrnr);
   if (lp != NULL)
      lp->setLayerType(LAYTYPE_UNKNOWN);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;

}

/****************************************************************************/
/*
*/
int des_documentation()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_DOCUMENTATION;

   G.level = "DOCUMENTATION";
   G.lyrnr = Graph_Level("DOCUMENTATION","",0);
   LayerStruct *l = doc->FindLayer(G.lyrnr);
   l->setLayerType(LAYTYPE_DOCUMENTATION);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_scoring_line()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SCORING_LINE;

   G.level = "SCORING_LINE";
   G.lyrnr = Graph_Level("SCORING_LINE","",0);
   LayerStruct *l = doc->FindLayer(G.lyrnr);
   l->setLayerType(LAYTYPE_SCORING_LINE);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_shearing_line()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SHEARING_LINE;

   G.level = "SHEARING_LINE";
   G.lyrnr = Graph_Level("SHEARING_LINE","",0);
   LayerStruct *l = doc->FindLayer(G.lyrnr);
   l->setLayerType(LAYTYPE_SHEARING_LINE);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int net_trace()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_TRACE;
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);
   
   cur_status = oldstat;

   return 1;
}

int aps_tie_leg()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   double oldWidth = G.diameter;
   apsTieLegFlag = true;

   cur_status = FILE_TRACE;
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_status = oldstat;
   G.diameter = oldWidth;
   apsTieLegFlag = false;

   return 1;
}

/****************************************************************************/
/*
*/

int vb99_netname()
{
   G.netName = strword();
   return 1;
}
/****************************************************************************/
/*
*/
int net_conductive_area()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_CONDUCTIVE_AREA;
   cur_conductive_area_teardrop = false;
   cur_conductive_area_plane = false;
   G.diameter = 0; // reset linewidth to 0

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   cur_conductive_area_teardrop = false;
   cur_conductive_area_plane = false;

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int des_net()
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
int vb99_horzjust()
{
   char     *lp;
   lp = strword();

   if (!STRCMPI(lp,"LEFT"))
      G.just |= VBTEXT_H_LEFT;
   else
   if (!STRCMPI(lp,"CENTER"))
      G.just |= VBTEXT_H_CENTER;
   else
   if (!STRCMPI(lp,"RIGHT"))
      G.just |= VBTEXT_H_RIGHT;
   
   return 1;
}

/****************************************************************************/
/*
*/
int vb99_vertjust()
{
   char     *lp;
   lp = strword();

   if (!STRCMPI(lp,"TOP"))
      G.just |= VBTEXT_V_TOP;
   else
   if (!STRCMPI(lp,"CENTER"))
      G.just |= VBTEXT_V_CENTER;
   else
   if (!STRCMPI(lp,"BOTTOM"))
      G.just |= VBTEXT_V_BOTTOM;
   
   return 1;
}

/****************************************************************************/
/*
*/
int vb99_textwidth()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.textwidth = cnv_unit(atof(lp));
   
   return 1;
}

/****************************************************************************/
/*
*/
int vb99_textoption()
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
int vb99_font()
{
   // not implemented
   return 1;
}

/****************************************************************************/
/*
*/
int des_boardorg()
{
   int   id = cur_ident;

   fprintf(ferr,"BOARD ORIGIN not implemented\n");
   go_command(board_org_lst,SIZ_BOARD_ORG_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int padform_square()
{
   int   id = cur_ident;

   G.geomtyp = T_SQUARE;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int padform_octagon()
{
   int   id = cur_ident;

   G.geomtyp = T_OCTAGON;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int padform_custom()
{
   int   id = cur_ident;

   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_radius_corner_rect()
{
   int   id = cur_ident;
   double x, y;
   double radius;
   

   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   radius = G.radius;
   x = G.xdim / 2;
   y = G.ydim / 2;

   Graph_PolyStruct(G.lyrnr, 0L, 0);
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x + radius, y, 0);
   Graph_Vertex(x - radius, y, -0.5);
   Graph_Vertex(x, y - radius, 0);
   Graph_Vertex(x, -y + radius, -0.5);
   Graph_Vertex(x - radius, -y, 0);
   Graph_Vertex(-x + radius, -y, -0.5);
   Graph_Vertex(-x, -y + radius, 0);
   Graph_Vertex(-x , y - radius, -0.5);
   Graph_Vertex(-x + radius, y, 0); 

   Graph_Block_Off();

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
   

   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.;
   y = G.ydim / 2.;

   // Case dts0100461803
   // Octagon finger pads and round finger pads always have the octagonal or rounded end across the shorter 
   // of the two dimensions. If the height is less than or equal to the width then the octagonal or rounded 
   // end is on the right edge. If the width is less than the height then the octagonal or rounded end is on 
   // the upper edge. Please look at the screen captures included in the zip with the CAD data.
   // If it is square then round/octogon is to the right.
   

   Graph_PolyStruct(G.lyrnr, 0L, 0);
   Graph_Poly(NULL,  0, 1, 0, 1);

   if (G.ydim <= G.xdim)
   {
      // Chamfer on right
      offset = G.ydim / 3.34;;
      Graph_Vertex(-x, y, 0.);
      Graph_Vertex(x - offset,  y, 0.);
      Graph_Vertex(x,  y - offset, 0.);
      Graph_Vertex(x, -y + offset, 0.);
      Graph_Vertex(x - offset, -y, 0.);
      Graph_Vertex(-x , -y, 0.);
      Graph_Vertex(-x,   y, 0.);
   }
   else
   {
      // Chamfer on top
      offset = G.xdim / 3.34;
      Graph_Vertex(-x, -y, 0.);
      Graph_Vertex(-x,  y - offset, 0.);
      Graph_Vertex(-x + offset,  y, 0.);
      Graph_Vertex( x - offset,  y, 0.);
      Graph_Vertex( x, y - offset, 0.);
      Graph_Vertex( x , -y, 0.);
      Graph_Vertex(-x,  -y, 0.);
   }

   Graph_Block_Off();

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
   

   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.;
   y = G.ydim / 2.;
   

   Graph_PolyStruct(G.lyrnr, 0L, 0);
   Graph_Poly(NULL,  0, 1, 0, 1);

   offset = min(G.xdim / 3.34, G.ydim / 3.34);
   Graph_Vertex(-x, -y + offset, 0.);
   Graph_Vertex(-x,  y - offset, 0.);
   Graph_Vertex(-x + offset,  y, 0.);
   Graph_Vertex( x - offset,  y, 0.);
   Graph_Vertex( x, y - offset, 0.);
   Graph_Vertex( x , -y + offset, 0.);
   Graph_Vertex( x - offset,  -y, 0.);
   Graph_Vertex(-x + offset, -y, 0.);
   Graph_Vertex(-x, -y + offset, 0.);

   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_square_donut()
{
   int   id = cur_ident;
   double x, y;
   

   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  
   Graph_PolyStruct(G.lyrnr, 0L, 0);

   // Outer
   x = G.xdim / 2.;
   y = x;
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x, -y, 0.);
   Graph_Vertex(-x,  y, 0.);
   Graph_Vertex( x,  y, 0.);
   Graph_Vertex( x, -y, 0.);
   Graph_Vertex(-x, -y, 0.);

   // Hole
   x = G.xdim / 3.5;
   y = x;
   Graph_Poly(NULL,  0, 1, 1, 1);
   Graph_Vertex(-x, -y, 0.);
   Graph_Vertex(-x,  y, 0.);
   Graph_Vertex( x,  y, 0.);
   Graph_Vertex( x, -y, 0.);
   Graph_Vertex(-x, -y, 0.);

   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_square_thermal()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  
   Graph_PolyStruct(G.lyrnr, 0L, 0);

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

   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x, -y, 0.);
   Graph_Vertex(-x, -y + leg, 0.);
   Graph_Vertex(-x + thk, -y + leg, 0.);
   Graph_Vertex(-x + thk, -y + thk, 0.);
   Graph_Vertex(-x + leg, -y + thk, 0.);
   Graph_Vertex(-x + leg, -y, 0.);
   Graph_Vertex(-x, -y, 0.);

   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x,  y, 0.);
   Graph_Vertex(-x + leg,  y, 0.);
   Graph_Vertex(-x + leg,  y - thk, 0.);
   Graph_Vertex(-x + thk,  y - thk, 0.);
   Graph_Vertex(-x + thk,  y - leg, 0.);
   Graph_Vertex(-x,  y - leg, 0.);
   Graph_Vertex(-x,  y, 0.);

   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( x,  y, 0.);
   Graph_Vertex( x,  y - leg, 0.);
   Graph_Vertex( x - thk,  y - leg, 0.);
   Graph_Vertex( x - thk,  y - thk, 0.);
   Graph_Vertex( x - leg,  y - thk, 0.);
   Graph_Vertex( x - leg,  y, 0.);
   Graph_Vertex( x,  y, 0.);

   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( x, -y, 0.);
   Graph_Vertex( x - leg, -y, 0.);
   Graph_Vertex( x - leg, -y + thk, 0.);
   Graph_Vertex( x - thk, -y + thk, 0.);
   Graph_Vertex( x - thk, -y + leg, 0.);
   Graph_Vertex( x, -y + leg, 0.);
   Graph_Vertex( x, -y, 0.);

   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_square_thermal_45()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

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
     
   Graph_PolyStruct(G.lyrnr, 0L, 0);

   // Top
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x + thk + a, y, 0.);
   Graph_Vertex( x - thk - a, y, 0.);
   Graph_Vertex( x - thk - a + chamfer, y - chamfer, 0.);
   Graph_Vertex( x - thk - a + chamfer, y - thk + chamfer, 0.);
   Graph_Vertex( x - thk - a, y - thk, 0.);
   Graph_Vertex(-x + thk + a, y - thk, 0.);
   Graph_Vertex(-x + thk + a - chamfer, y - thk + chamfer, 0.);
   Graph_Vertex(-x + thk + a - chamfer, y - chamfer, 0.);
   Graph_Vertex(-x + thk + a, y, 0.);

   // Bottom
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x + thk + a, -y, 0.);
   Graph_Vertex( x - thk - a, -y, 0.);
   Graph_Vertex( x - thk - a + chamfer, -y + chamfer, 0.);
   Graph_Vertex( x - thk - a + chamfer, -y + thk - chamfer, 0.);
   Graph_Vertex( x - thk - a, -y + thk, 0.);
   Graph_Vertex(-x + thk + a, -y + thk, 0.);
   Graph_Vertex(-x + thk + a - chamfer, -y + thk - chamfer, 0.);
   Graph_Vertex(-x + thk + a - chamfer, -y + chamfer, 0.);
   Graph_Vertex(-x + thk + a, -y, 0.);

   // Left
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x, -y  + thk + a, 0.);
   Graph_Vertex(-x,  y - thk - a, 0.);
   Graph_Vertex(-x + chamfer,       y - thk - a + chamfer, 0.);
   Graph_Vertex(-x + thk - chamfer, y - thk - a + chamfer, 0.);
   Graph_Vertex(-x + thk,  y - thk - a, 0.);
   Graph_Vertex(-x + thk, -y + thk + a, 0.);
   Graph_Vertex(-x + thk - chamfer, -y + thk + a - chamfer, 0.);
   Graph_Vertex(-x + chamfer, -y + thk + a - chamfer, 0.);
   Graph_Vertex(-x, -y  + thk + a, 0.);

   // Right
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-(-x), -y  + thk + a, 0.);
   Graph_Vertex(-(-x),  y - thk - a, 0.);
   Graph_Vertex(-(-x + chamfer),       y - thk - a + chamfer, 0.);
   Graph_Vertex(-(-x + thk - chamfer), y - thk - a + chamfer, 0.);
   Graph_Vertex(-(-x + thk),  y - thk - a, 0.);
   Graph_Vertex(-(-x + thk), -y + thk + a, 0.);
   Graph_Vertex(-(-x + thk - chamfer), -y + thk + a - chamfer, 0.);
   Graph_Vertex(-(-x + chamfer), -y + thk + a - chamfer, 0.);
   Graph_Vertex(-(-x), -y  + thk + a, 0.);


   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
int padform_4_web_rect_thermal()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  
   Graph_PolyStruct(G.lyrnr, 0L, 0);

   x = G.xdim / 2.; // overall width defines corner xy's.
   y = G.ydim / 2.; // overall height defines corner xy's.

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

   double xleg = x - (tie_leg_width / 2.);
   double yleg = y - (tie_leg_width / 2.);
   double thk = thermal_clearance;

   if (xleg > x) // two legs would be greater than width, not good, punt
      xleg = G.xdim / 2.35;
   if (yleg > y) // two legs would be greater than width, not good, punt
      yleg = G.ydim / 2.35;

   double minleg = min(xleg, yleg);
   if (thk > minleg) // two thicknesses would be greater than width, not good, punt
      thk = minleg / 2.35; // off the wall, makes a nice aspect ratio

   // Bottom left
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x, -y, 0.);
   Graph_Vertex(-x, -y + yleg, 0.);
   Graph_Vertex(-x + thk, -y + yleg, 0.);
   Graph_Vertex(-x + thk, -y + thk, 0.);
   Graph_Vertex(-x + xleg, -y + thk, 0.);
   Graph_Vertex(-x + xleg, -y, 0.);
   Graph_Vertex(-x, -y, 0.);

   // Top left
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x,  y, 0.);
   Graph_Vertex(-x + xleg,  y, 0.);
   Graph_Vertex(-x + xleg,  y - thk, 0.);
   Graph_Vertex(-x + thk,  y - thk, 0.);
   Graph_Vertex(-x + thk,  y - yleg, 0.);
   Graph_Vertex(-x,  y - yleg, 0.);
   Graph_Vertex(-x,  y, 0.);

   // Top right
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( x,  y, 0.);
   Graph_Vertex( x,  y - yleg, 0.);
   Graph_Vertex( x - thk,  y - yleg, 0.);
   Graph_Vertex( x - thk,  y - thk, 0.);
   Graph_Vertex( x - xleg,  y - thk, 0.);
   Graph_Vertex( x - xleg,  y, 0.);
   Graph_Vertex( x,  y, 0.);

   // Bottom right
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( x, -y, 0.);
   Graph_Vertex( x - xleg, -y, 0.);
   Graph_Vertex( x - xleg, -y + thk, 0.);
   Graph_Vertex( x - thk, -y + thk, 0.);
   Graph_Vertex( x - thk, -y + yleg, 0.);
   Graph_Vertex( x, -y + yleg, 0.);
   Graph_Vertex( x, -y, 0.);

   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_rect_thermal_45()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.; // overall width defines corner xy's.
   y = G.ydim / 2.; // overall height defines corner xy's.

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
     
   Graph_PolyStruct(G.lyrnr, 0L, 0);

#ifdef CHAMFER_RECT_THERMAL
   // The examples from Expedition are inconsistent.
   // Probably there is some other data or mode switch are unaware of and so not
   // accounting for. When we got cases for the SQUARE thermal 45 the image showed
   // chamfered corners. The info we got with the RECT thermal doesn't show
   // chamfers. This code was derviced from the square version, and so we
   // have this chamfered style sort of "for free".
   // But it is disabled in favor of a simpler graphic that matches the
   // image that is with the case. The chamfer version is saved here in
   // case we want it later.
   // Case dts0100496016 is what brought us the RECT and OBLONG thermals.


   // Top
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x + thk + a, y, 0.);
   Graph_Vertex( x - thk - a, y, 0.);
   Graph_Vertex( x - thk - a + chamfer, y - chamfer, 0.);
   Graph_Vertex( x - thk - a + chamfer, y - thk + chamfer, 0.);
   Graph_Vertex( x - thk - a, y - thk, 0.);
   Graph_Vertex(-x + thk + a, y - thk, 0.);
   Graph_Vertex(-x + thk + a - chamfer, y - thk + chamfer, 0.);
   Graph_Vertex(-x + thk + a - chamfer, y - chamfer, 0.);
   Graph_Vertex(-x + thk + a, y, 0.);

   // Bottom
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x + thk + a, -y, 0.);
   Graph_Vertex( x - thk - a, -y, 0.);
   Graph_Vertex( x - thk - a + chamfer, -y + chamfer, 0.);
   Graph_Vertex( x - thk - a + chamfer, -y + thk - chamfer, 0.);
   Graph_Vertex( x - thk - a, -y + thk, 0.);
   Graph_Vertex(-x + thk + a, -y + thk, 0.);
   Graph_Vertex(-x + thk + a - chamfer, -y + thk - chamfer, 0.);
   Graph_Vertex(-x + thk + a - chamfer, -y + chamfer, 0.);
   Graph_Vertex(-x + thk + a, -y, 0.);

   // Left
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x, -y  + thk + a, 0.);
   Graph_Vertex(-x,  y - thk - a, 0.);
   Graph_Vertex(-x + chamfer,       y - thk - a + chamfer, 0.);
   Graph_Vertex(-x + thk - chamfer, y - thk - a + chamfer, 0.);
   Graph_Vertex(-x + thk,  y - thk - a, 0.);
   Graph_Vertex(-x + thk, -y + thk + a, 0.);
   Graph_Vertex(-x + thk - chamfer, -y + thk + a - chamfer, 0.);
   Graph_Vertex(-x + chamfer, -y + thk + a - chamfer, 0.);
   Graph_Vertex(-x, -y  + thk + a, 0.);

   // Right
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-(-x), -y  + thk + a, 0.);
   Graph_Vertex(-(-x),  y - thk - a, 0.);
   Graph_Vertex(-(-x + chamfer),       y - thk - a + chamfer, 0.);
   Graph_Vertex(-(-x + thk - chamfer), y - thk - a + chamfer, 0.);
   Graph_Vertex(-(-x + thk),  y - thk - a, 0.);
   Graph_Vertex(-(-x + thk), -y + thk + a, 0.);
   Graph_Vertex(-(-x + thk - chamfer), -y + thk + a - chamfer, 0.);
   Graph_Vertex(-(-x + chamfer), -y + thk + a - chamfer, 0.);
   Graph_Vertex(-(-x), -y  + thk + a, 0.);

#else

   double inset = a + thk;

   // Top
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x + a, y, 0.);
   Graph_Vertex( x - a, y, 0.);
   Graph_Vertex( x - thk - a, y - thk, 0.);
   Graph_Vertex(-x + thk + a, y - thk, 0.);
   Graph_Vertex(-x + a, y, 0.);

   // Bottom
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x + a, -y, 0.);
   Graph_Vertex( x - a, -y, 0.);
   Graph_Vertex( x - thk - a, -y + thk, 0.);
   Graph_Vertex(-x + thk + a, -y + thk, 0.);
   Graph_Vertex(-x + a, -y, 0.);

   // Left
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-x, -y  + a, 0.);
   Graph_Vertex(-x,  y  - a, 0.);
   Graph_Vertex(-x + thk,  y - thk - a, 0.);
   Graph_Vertex(-x + thk, -y + thk + a, 0.);
   Graph_Vertex(-x, -y  + a, 0.);

   // Right
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( x, -y  + a, 0.);
   Graph_Vertex( x,  y  - a, 0.);
   Graph_Vertex( x - thk,  y - thk - a, 0.);
   Graph_Vertex( x - thk, -y + thk + a, 0.);
   Graph_Vertex( x, -y  + a, 0.);

#endif

   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
static void VB99_Graph_Vertex(CTMatrix &tmat, double x, double y, double bulge)
{
   CPoint2d pnt(x,  y);
   tmat.transform(pnt);
   Graph_Vertex(pnt.x,  pnt.y, bulge);
}

/****************************************************************************/
/*
*/
int padform_4_web_oblong_thermal()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  

   Graph_PolyStruct(G.lyrnr, 0L, 0);

   x = G.xdim / 2.; // overall width defines corner xy's.
   y = G.ydim / 2.; // overall length

   // The graphic drawing pattern assumes width (x) >= length (y).
   // That seems to be the standard aspect ratio convention, because the sample data
   // is predominatnly in this form. But there are some that are not in this form.
   // In VB99, the arcs are always on the narrow ends.
   // If we have params with length > width then to use the "standard graphic" stuff below
   // we need to rotate it. We do this by swapping X and Y and then applying a
   // rotation transform to the pts in our polyline, to get back to the original position.
   CTMatrix tmat;
   if (y > x)
   {
      double tmp = y;
      y = x;
      x = tmp;
      tmat.rotateDegrees(90.0);
   }

   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   double tie_leg_width = G.tie_leg_width;
   if (tie_leg_width <= 0.0)
      tie_leg_width = (2. * x) * .20; // default to 20% of width for gap
   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = (2. * x) * .15; // default to 15% of width for thickness

   double xleg = x - (tie_leg_width / 2.);
   double yleg = y - (tie_leg_width / 2.);
   double thk = thermal_clearance;

   if (xleg > x) // two legs would be greater than width, not good, punt
      xleg = (2. * x) / 2.35;
   if (yleg > y) // two legs would be greater than width, not good, punt
      yleg = (2. * y) / 2.35;

   double minleg = min(xleg, yleg);
   if (thk > minleg) // two thicknesses would be greater than width, not good, punt
      thk = minleg / 2.35; // off the wall, makes a nice aspect ratio

   double outerRadius = (2. * y) / 2.;
   double innerRadius = outerRadius - thk;

   double bulge = tan(DegToRad(60.) / 4.);  // bulge = tan(da/4)
   const double sin75 = sin(DegToRad(75.));
   const double cos75 = cos(DegToRad(75.));
   const double sin15 = sin(DegToRad(15.));
   const double cos15 = cos(DegToRad(15.));

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
   int res = ArcFromStartEndRadius(startOuterX, startOuterY, endOuterX, endOuterY,
      outerRadius, FALSE, TRUE, &cx, &cy, &sa, &da);

   double bulge2 = tan(da/4.);

   Graph_PolyStruct(G.lyrnr, 0L, 0);

   // Upper left
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, -x + xleg,  y, 0.);

   VB99_Graph_Vertex(tmat, -x + outerRadius,  y, bulge2);
   VB99_Graph_Vertex(tmat, -x,        y - yleg, 0.);
   VB99_Graph_Vertex(tmat, -x + thk,  y - yleg, -bulge);
   VB99_Graph_Vertex(tmat, -x + outerRadius,  y - thk, 0.);

   VB99_Graph_Vertex(tmat, -x + xleg,  y - thk, 0.);
   VB99_Graph_Vertex(tmat, -x + xleg,  y, 0.);

   // Lower left
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, -x + xleg,  -y, 0.);

   VB99_Graph_Vertex(tmat, -x + outerRadius,  -y, -bulge2);
   VB99_Graph_Vertex(tmat, -x,        -(y - yleg), 0.);
   VB99_Graph_Vertex(tmat, -x + thk,  -(y - yleg), +bulge);
   VB99_Graph_Vertex(tmat, -x + outerRadius,  -(y - thk), 0.);

   VB99_Graph_Vertex(tmat, -x + xleg,  -(y - thk), 0.);
   VB99_Graph_Vertex(tmat, -x + xleg,  -y, 0.);

   // Upper right
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat,  x - xleg,  y, 0.);

   VB99_Graph_Vertex(tmat,  x - outerRadius,  y, -bulge2);
   VB99_Graph_Vertex(tmat,  x,        y - yleg, 0.);
   VB99_Graph_Vertex(tmat,  x - thk,  y - yleg, +bulge);
   VB99_Graph_Vertex(tmat,  x - outerRadius,  y - thk, 0.);

   VB99_Graph_Vertex(tmat,  x - xleg,  y - thk, 0.);
   VB99_Graph_Vertex(tmat,  x - xleg,  y, 0.);

   // Lower right
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat,  x - xleg,  -y, 0.);

   VB99_Graph_Vertex(tmat,  x - outerRadius,  -y, bulge2);
   VB99_Graph_Vertex(tmat,  x,        -(y - yleg), 0.);
   VB99_Graph_Vertex(tmat,  x - thk,  -(y - yleg), -bulge);
   VB99_Graph_Vertex(tmat,  x - outerRadius,  -(y - thk), 0.);

   VB99_Graph_Vertex(tmat,  x - xleg,  -(y - thk), 0.);
   VB99_Graph_Vertex(tmat,  x - xleg,  -y, 0.);

   Graph_Block_Off();


   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_oblong_thermal_45()
{
   int   id = cur_ident;
   double x, y;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  

   Graph_PolyStruct(G.lyrnr, 0L, 0);

   x = G.xdim / 2.; // overall width defines corner xy's.
   y = G.ydim / 2.; // overall length

   // The graphic drawing pattern assumes width (x) >= length (y).
   // That seems to be the standard aspect ratio convention, because the sample data
   // is predominatnly in this form. But there are some that are not in this form.
   // In VB99, the arcs are always on the narrow ends.
   // If we have params with length > width then to use the "standard graphic" stuff below
   // we need to rotate it. We do this by swapping X and Y and then applying a
   // rotation transform to the pts in our polyline, to get back to the original position.
   CTMatrix tmat;
   if (y > x)
   {
      double tmp = y;
      y = x;
      x = tmp;
      tmat.rotateDegrees(90.0);
   }


   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   double tie_leg_width = G.tie_leg_width;
   if (tie_leg_width <= 0.0)
      tie_leg_width = (2. * x) * .20; // default to 20% of width for gap
   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = (2. * x) * .15; // default to 15% of width for thickness

   double xleg = x; //x - (tie_leg_width / 2.);
   double yleg = y; //y - (tie_leg_width / 2.);
   double thk = thermal_clearance;

   //if (xleg > x) // two legs would be greater than width, not good, punt
   //   xleg = (2. * x) / 2.35;
   //if (yleg > y) // two legs would be greater than width, not good, punt
   //   yleg = (2. * y) / 2.35;

   double minleg = min(xleg, yleg);
   if (thk > minleg) // two thicknesses would be greater than width, not good, punt
      thk = minleg / 2.35; // off the wall, makes a nice aspect ratio

   double outerRadius = (2. * y) / 2.;
   double innerRadius = outerRadius - thk;

   double bulge = tan(DegToRad(30.) / 4.);  // bulge = tan(da/4)
   double startsin = sin(DegToRad(0.));
   double startcos = cos(DegToRad(0.));
   double endsin = sin(DegToRad(30.));
   double endcos = cos(DegToRad(30.));

   double startInnerX = startsin * innerRadius;
   double startInnerY = startcos * innerRadius;
   double startOuterX = startsin * outerRadius;
   double startOuterY = startcos * outerRadius;

   double endInnerX = endsin * innerRadius;
   double endInnerY = endcos * innerRadius;
   double endOuterX = endsin * outerRadius;
   double endOuterY = endcos * outerRadius;

   double cx, cy, sa, da;
   int res = ArcFromStartEndRadius(startOuterX, startOuterY, endOuterX, endOuterY,
      outerRadius, FALSE, TRUE, &cx, &cy, &sa, &da);

   double bulge2 = tan(da/4.);

   Graph_PolyStruct(G.lyrnr, 0L, 0);

   // Upper left
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, -x + xleg,  y, 0.);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(startOuterX), +(startOuterY), bulge2);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(endOuterX),   +(endOuterY),    0.);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(endInnerX),   +(endInnerY),   -bulge);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(startInnerX), +(startInnerY),  0.);
   VB99_Graph_Vertex(tmat, -x + xleg,  y - thk, 0.);
   VB99_Graph_Vertex(tmat, -x + xleg,  y, 0.);


   // Lower left
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, -x + xleg,  -y, 0.);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(startOuterX), -(startOuterY), -bulge2);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(endOuterX),   -(endOuterY),    0.);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(endInnerX),   -(endInnerY),    bulge);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(startInnerX), -(startInnerY),  0.);
   VB99_Graph_Vertex(tmat, -x + xleg,  -(y - thk), 0.);
   VB99_Graph_Vertex(tmat, -x + xleg,  -y, 0.);


   // Upper right
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, x - xleg,  y, 0.);
   VB99_Graph_Vertex(tmat, x - outerRadius +(startOuterX), +(startOuterY), -bulge2);
   VB99_Graph_Vertex(tmat, x - outerRadius +(endOuterX),   +(endOuterY),    0.);
   VB99_Graph_Vertex(tmat, x - outerRadius +(endInnerX),   +(endInnerY),    bulge);
   VB99_Graph_Vertex(tmat, x - outerRadius +(startInnerX), +(startInnerY),  0.);
   VB99_Graph_Vertex(tmat, x - xleg,  y - thk, 0.);
   VB99_Graph_Vertex(tmat, x - xleg,  y, 0.);

   // Lower right
   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, x - xleg,  -y, 0.);
   VB99_Graph_Vertex(tmat, x - outerRadius +(startOuterX), -(startOuterY),  bulge2);
   VB99_Graph_Vertex(tmat, x - outerRadius +(endOuterX),   -(endOuterY),    0.);
   VB99_Graph_Vertex(tmat, x - outerRadius +(endInnerX),   -(endInnerY),   -bulge);
   VB99_Graph_Vertex(tmat, x - outerRadius +(startInnerX), -(startInnerY),  0.);
   VB99_Graph_Vertex(tmat, x - xleg,  -(y - thk), 0.);
   VB99_Graph_Vertex(tmat, x - xleg,  -y, 0.);


   // Left and Right arcs
   bulge = tan(DegToRad(60.) / 4.);  // bulge = tan(da/4)
   startsin = sin(DegToRad(60.));
   startcos = cos(DegToRad(60.));
   endsin = sin(DegToRad(120.));
   endcos = cos(DegToRad(120.));

   startInnerX = startsin * innerRadius;
   startInnerY = startcos * innerRadius;
   startOuterX = startsin * outerRadius;
   startOuterY = startcos * outerRadius;

   endInnerX = endsin * innerRadius;
   endInnerY = endcos * innerRadius;
   endOuterX = endsin * outerRadius;
   endOuterY = endcos * outerRadius;

   res = ArcFromStartEndRadius(startOuterX, startOuterY, endOuterX, endOuterY,
      outerRadius, FALSE, TRUE, &cx, &cy, &sa, &da);

   bulge2 = tan(da/4.);

   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(startOuterX), +(startOuterY),  bulge2);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(endOuterX),   +(endOuterY),    0.);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(endInnerX),   +(endInnerY),   -bulge);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(startInnerX), +(startInnerY),  0.);
   VB99_Graph_Vertex(tmat, -x + outerRadius -(startOuterX), +(startOuterY),  0.);

   Graph_Poly(NULL,  0, 1, 0, 1);
   VB99_Graph_Vertex(tmat, -(-x + outerRadius -(startOuterX)), +(startOuterY), -bulge2);
   VB99_Graph_Vertex(tmat, -(-x + outerRadius -(endOuterX)),   +(endOuterY),    0.);
   VB99_Graph_Vertex(tmat, -(-x + outerRadius -(endInnerX)),   +(endInnerY),    bulge);
   VB99_Graph_Vertex(tmat, -(-x + outerRadius -(startInnerX)), +(startInnerY),  0.);
   VB99_Graph_Vertex(tmat, -(-x + outerRadius -(startOuterX)), +(startOuterY),  0.);

   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_4_web_round_thermal()
{
   int   id = cur_ident;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

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

   double bulge = tan(DegToRad(60.) / 4.);  // bulge = tan(da/4)
   const double sin75 = sin(DegToRad(75.));
   const double cos75 = cos(DegToRad(75.));
   const double sin15 = sin(DegToRad(15.));
   const double cos15 = cos(DegToRad(15.));

   Graph_PolyStruct(G.lyrnr, 0L, 0);

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
   int res = ArcFromStartEndRadius(startOuterX, startOuterY, endOuterX, endOuterY,
      outerRadius, FALSE, TRUE, &cx, &cy, &sa, &da);

   double bulge2 = tan(da/4.);

   // top left arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( -(startOuterX), +(startOuterY),  bulge2);
   Graph_Vertex( -(endOuterX),   +(endOuterY),    0.);
   Graph_Vertex( -(endInnerX),   +(endInnerY),   -bulge);
   Graph_Vertex( -(startInnerX), +(startInnerY),  0.);
   Graph_Vertex( -(startOuterX), +(startOuterY),  0.);

   // top right arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( +(startOuterX), +(startOuterY), -bulge2);
   Graph_Vertex( +(endOuterX),   +(endOuterY),    0.);
   Graph_Vertex( +(endInnerX),   +(endInnerY),    bulge);
   Graph_Vertex( +(startInnerX), +(startInnerY),  0.);
   Graph_Vertex( +(startOuterX), +(startOuterY),  0.);

   // bottom left arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( -(startOuterX), -(startOuterY), -bulge2);
   Graph_Vertex( -(endOuterX),   -(endOuterY),    0.);
   Graph_Vertex( -(endInnerX),   -(endInnerY),    bulge);
   Graph_Vertex( -(startInnerX), -(startInnerY),  0.);
   Graph_Vertex( -(startOuterX), -(startOuterY),  0.);

   // bottom right arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( +(startOuterX), -(startOuterY),  bulge2);
   Graph_Vertex( +(endOuterX),   -(endOuterY),    0.);
   Graph_Vertex( +(endInnerX),   -(endInnerY),   -bulge);
   Graph_Vertex( +(startInnerX), -(startInnerY),  0.);
   Graph_Vertex( +(startOuterX), -(startOuterY),  0.);

   Graph_Block_Off();

   return 1;
}

/*
static void graphArcSegmentV(double sinStart, double cosStart, double sinEnd, double cosEnd, double innerRadius, double outerRadius, double bulge)
{
   // V shaped tie leg gap

   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( (cosStart * outerRadius), (sinStart * outerRadius), bulge);
   Graph_Vertex( (cosEnd   * outerRadius), (sinEnd   * outerRadius), 0.);
   Graph_Vertex( (cosEnd   * innerRadius), (sinEnd   * innerRadius), -bulge);
   Graph_Vertex( (cosStart * innerRadius), (sinStart * innerRadius), 0.);
   Graph_Vertex( (cosStart * outerRadius), (sinStart * outerRadius), 0.);
}
*/

static void graphArcSegmentP(double sinOuterStart, double cosOuterStart, double sinOuterEnd, double cosOuterEnd,
                             double sinInnerStart, double cosInnerStart, double sinInnerEnd, double cosInnerEnd,
                             double innerRadius, double outerRadius, double innerbulge, double outerbulge)
{
   // Parallel tie leg gap

   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( (cosOuterStart * outerRadius), (sinOuterStart * outerRadius), outerbulge);
   Graph_Vertex( (cosOuterEnd   * outerRadius), (sinOuterEnd   * outerRadius), 0.);
   Graph_Vertex( (cosInnerEnd   * innerRadius), (sinInnerEnd   * innerRadius), -innerbulge);
   Graph_Vertex( (cosInnerStart * innerRadius), (sinInnerStart * innerRadius), 0.);
   Graph_Vertex( (cosOuterStart * outerRadius), (sinOuterStart * outerRadius), 0.);
}

/****************************************************************************/
/*
*/
int padform_4_web_round_thermal_45()
{
   int   id = cur_ident;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
  

   // Definitions from Dean:
   // The diameter is the outer dimension of the aperture shape. (aka Width if square)
   // The Thermal Clearance is the inset from the outer diameter to calculate the inner diameter. 
   // The tie leg width is the size of the gap to be created in the annular ring.
   //
   // So Thermal Clearance gives us "line width" or "line thickness".
   // Tie Leg Width is the gap.


   double thermal_clearance = G.thermal_clearance;
   if (thermal_clearance <= 0.0)
      thermal_clearance = G.xdim * .15; // default to 15% of width for thickness

   double thk = thermal_clearance;

   double outerRadius = G.xdim / 2.;
   double innerRadius = outerRadius - thk;

   // Pad is built camcad Thermal style, thickness from cad is used.
   // Tie leg width is used to determine gap angle. 
   // Pattern from Apertur2.cpp.

   // Angles
   double innergap = 30.; // Tie leg in degrees, 30 degree gap is default, set gap, the rest follows
   double outergap = innergap;
   if (G.tie_leg_width > 0.)
   {
      // Get gap angle, it's an approximation based on the tie leg width.
      // The thermal does not have parallel ends as created here, and most of the rest of camcad for that matter.
      double radius, startAngle, deltaAngle;
      ArcCenter2(innerRadius,0., innerRadius,G.tie_leg_width, 0.,0., &radius, &startAngle, &deltaAngle, false);
      //double sa = RadToDeg(startAngle);  // for debug, should always come out 0.0
      innergap = RadToDeg(deltaAngle);

      ArcCenter2(outerRadius,0., outerRadius,G.tie_leg_width, 0.,0., &radius, &startAngle, &deltaAngle, false);
      //double sa = RadToDeg(startAngle);  // for debug, should always come out 0.0
      outergap = RadToDeg(deltaAngle);
   }

   const double innerspan = 90. - innergap; // degree span of arc segs, based on 90 because there are four
   const double innerhalfspan = innerspan / 2.;

   const double outerspan = 90. - outergap; // degree span of arc segs, based on 90 because there are four
   const double outerhalfspan = outerspan / 2.;

   const double innerbulge = tan(DegToRad(innerspan)/4.); // bulge = tan(da/4) ; da = span
   const double outerbulge = tan(DegToRad(outerspan)/4.); // bulge = tan(da/4) ; da = span

   // But most VB screen shots are more like this, with parallel edged fingers.
   // Most traditional camcad thermals have V shaped (or wedge shaped) tie legs, very unflattering.

   Graph_PolyStruct(G.lyrnr, 0L, 0);

   for (int angle = 0; angle < 360; angle += 90)
   {
      const double sinOutS = sin(DegToRad((double)angle - outerhalfspan));  // S for Start
      const double cosOutS = cos(DegToRad((double)angle - outerhalfspan));
      const double sinOutE = sin(DegToRad((double)angle + outerhalfspan));  // E for End
      const double cosOutE = cos(DegToRad((double)angle + outerhalfspan));

      const double sinInnS = sin(DegToRad((double)angle - innerhalfspan));  // S for Start
      const double cosInnS = cos(DegToRad((double)angle - innerhalfspan));
      const double sinInnE = sin(DegToRad((double)angle + innerhalfspan));  // E for End
      const double cosInnE = cos(DegToRad((double)angle + innerhalfspan));

      graphArcSegmentP(sinOutS, cosOutS, sinOutE, cosOutE, sinInnS, cosInnS, sinInnE, cosInnE, innerRadius, outerRadius, innerbulge, outerbulge);
   }


   Graph_Block_Off();

   return 1;
}



/****************************************************************************/
/*
*/
int padform_2_web_round_thermal()
{
   int   id = cur_ident;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

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

   //double bulge = tan(DegToRad(150.) / 4.);  // bulge = tan(da/4)
   //const double sin75 = sin(DegToRad(75.));
   //const double cos75 = cos(DegToRad(75.));

   // Make the segment ends parallel, rather than tapered like the standard camcad thermal

   double innerX = b;  //sin75 * innerRadius;
   double innerY = a;  //cos75 * innerRadius;
   double outerX = innerX + thk;
   double outerY = innerY;

   double cx, cy, sa, daOuter, daInner;
   int res = ArcFromStartEndRadius(innerX, innerY, -innerX, innerY,
      innerRadius, FALSE, TRUE, &cx, &cy, &sa, &daInner);
   res = ArcFromStartEndRadius(outerX, outerY, -outerX, outerY,
      outerRadius, FALSE, TRUE, &cx, &cy, &sa, &daOuter);

   double bulgeInner = tan(daInner/4.);
   double bulgeOuter = tan(daOuter/4.);

   Graph_PolyStruct(G.lyrnr, 0L, 0);

   // top arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( +(outerX), +(outerY), bulgeOuter);
   Graph_Vertex( -(outerX), +(outerY), 0.);
   Graph_Vertex( -(innerX), +(innerY), -bulgeInner);
   Graph_Vertex( +(innerX), +(innerY), 0.);
   Graph_Vertex( +(outerX), +(outerY), 0.);

   // top arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex( +(outerX), -(outerY), -bulgeOuter);
   Graph_Vertex( -(outerX), -(outerY), 0.);
   Graph_Vertex( -(innerX), -(innerY), bulgeInner);
   Graph_Vertex( +(innerX), -(innerY), 0.);
   Graph_Vertex( +(outerX), -(outerY), 0.);

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
   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_2_web_round_thermal_45()
{
   int   id = cur_ident;
   
   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

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
   const double bulge = tan(DegToRad(60. + 30. + 60.)/4.); // bulge = tan(da/4) ; da = 150°
   const double sin30 = 0.5; 
   const double cos30 = 0.866;

   Graph_PolyStruct(G.lyrnr, 0L, 0);

   // top/left arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(+(sin30 * outerRadius), +(cos30 * outerRadius), bulge);
   Graph_Vertex(-(cos30 * outerRadius), -(sin30 * outerRadius), 0.);
   Graph_Vertex(-(cos30 * innerRadius), -(sin30 * innerRadius), -bulge);
   Graph_Vertex(+(sin30 * innerRadius), +(cos30 * innerRadius), 0.);
   Graph_Vertex(+(sin30 * outerRadius), +(cos30 * outerRadius), 0.);

   // bottom/right arc
   Graph_Poly(NULL,  0, 1, 0, 1);
   Graph_Vertex(-(sin30 * outerRadius), -(cos30 * outerRadius), bulge);
   Graph_Vertex(+(cos30 * outerRadius), +(sin30 * outerRadius), 0.);
   Graph_Vertex(+(cos30 * innerRadius), +(sin30 * innerRadius), -bulge);
   Graph_Vertex(-(sin30 * innerRadius), -(cos30 * innerRadius), 0.);
   Graph_Vertex(-(sin30 * outerRadius), -(cos30 * outerRadius), 0.);


   Graph_Block_Off();

   return 1;
}


/****************************************************************************/
/*
*/
int padform_chamfered_rectangle()
{
   int   id = cur_ident;
   double x, y;
   

   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);
 
   x = G.xdim / 2.;
   y = G.ydim / 2.;
   double offset = G.chamfer;

   Graph_PolyStruct(G.lyrnr, 0L, 0);
   Graph_Poly(NULL,  0, 1, 0, 1);

   Graph_Vertex(-x, -y + offset, 0.);
   Graph_Vertex(-x,  y - offset, 0.);
   Graph_Vertex(-x + offset,  y, 0.);
   Graph_Vertex( x - offset,  y, 0.);
   Graph_Vertex( x,  y - offset, 0.);
   Graph_Vertex( x, -y + offset, 0.);
   Graph_Vertex( x - offset, -y, 0.);
   Graph_Vertex(-x + offset, -y, 0.);
   Graph_Vertex(-x, -y + offset, 0.);

   Graph_Block_Off();

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
   

   G.geomtyp = T_COMPLEX;

   CString  usr_name;
   usr_name.Format("SHAPE_%s", G.name);

   Graph_Block_On(GBO_APPEND,usr_name, -1, 0);

   G.diameter = 0;
   G.level = "0";
   G.lyrnr = Graph_Level(G.level, "", 0);

   go_command(padgeom_lst,SIZ_GRAPHIC_LST,id);

   x = G.xdim / 2.;
   y = G.ydim / 2.;
   
   Graph_PolyStruct(G.lyrnr, 0L, 0);
   Graph_Poly(NULL,  0, 1, 0, 1);

   // Case dts0100461803
   // Octagon finger pads and round finger pads always have the octagonal or rounded end across the shorter 
   // of the two dimensions. If the height is less than or equal to the width then the octagonal or rounded 
   // end is on the right edge. If the width is less than the height then the octagonal or rounded end is on 
   // the upper edge. Please look at the screen captures included in the zip with the CAD data.
   // If it is square then round/octogon is to the right.

   if (G.ydim <= G.xdim)
   {
      // Round end to right
      offset = y; // ydim is diameter, so y is radius
      Graph_Vertex(-x,  y, 0.);
      Graph_Vertex( x - offset,  y, -1.);
      Graph_Vertex( x - offset, -y,  0.);
      Graph_Vertex(-x, -y, 0.);
      Graph_Vertex(-x,  y, 0.);
   }
   else
   {
      // Round end up
      offset = x; // xdim is diameter, so x is radius
      Graph_Vertex(-x, -y, 0.);
      Graph_Vertex(-x,  y - offset, -1.);
      Graph_Vertex( x,  y - offset,  0.);
      Graph_Vertex( x, -y, 0.);
      Graph_Vertex(-x , -y, 0.);
   }

   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
int padform_round()
{
   int   id = cur_ident;

   G.geomtyp = T_ROUND;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int padform_round_donut()
{
   int   id = cur_ident;

   G.geomtyp = T_DONUT;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int padform_thermal()
{
   int   id = cur_ident;

   G.geomtyp = T_THERMAL;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int hole_option()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   return 1;
}

/****************************************************************************/
/*
*/
int drill_symbol()
{
   int   id = cur_ident;

   go_command(drillSym_lst,SIZ_DRILLSYM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int use_character_as_symbol()
{
   int   id = cur_ident;

   go_command(character_as_symbol_lst,SIZ_CHARACTER_AS_SYMBOL_LST,id);

   CString DrillName = CharacterSymbolName + "_" + CharacterSymbolSize;
   BlockStruct *Block = Graph_Block_On(GBO_APPEND,DrillName,-1,0L,blockTypeToolGraphic);      
   doc->SetUnknownAttrib(&Block->getAttributesRef(),"CHARACTER_SYMBOL", CharacterSymbolName, SA_OVERWRITE, NULL);
   doc->SetUnknownAttrib(&Block->getAttributesRef(),"SIZE", CharacterSymbolSize, SA_OVERWRITE, NULL);   
      int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   Graph_Text(laynr,CharacterSymbolName,0,0,
      cnv_unit(atof(CharacterSymbolSize)),
      cnv_unit(atof(CharacterSymbolSize))*TEXT_ASPECT_RATIO,
      0, 0L, TRUE, 0, 0, FALSE, -1, 0, horizontalPositionCenter, verticalPositionCenter);

   doc->getWidthTable().Add(Block);
   Graph_Block_Off();
   return 1;
}

/****************************************************************************/
/*
*/
int character_symbol()
{
   CharacterSymbolName = strword();
   return 1;
}

/****************************************************************************/
/*
*/
int character_size()
{
   CharacterSymbolSize = strword();

   return 1;
}
/****************************************************************************/
/*
*/

int use_symbol_from_list()
{
   int   id = cur_ident;

   go_command(symfromlist_lst,SIZ_SYMFROMLIST_LST,id);
   return 1;
}


/****************************************************************************/
/*
*/

int standard_drill_symbol()
{
   int   id = cur_ident;
   standardSymbolName = strword();
   go_command(standarddrillsymbol_lst,SIZ_STANDARDDRILLSYMBOL_LST,id);

   int   oldstat = cur_status;

   CString DrillName = standardSymbolName + "_" + standardSymbolSize;

   BlockStruct *Block = Graph_Block_On(GBO_APPEND,DrillName,-1,0L,blockTypeToolGraphic);   
   
   doc->SetUnknownAttrib(&Block->getAttributesRef(),"STANDARD_SYMBOL", standardSymbolName, SA_OVERWRITE, NULL);
   doc->SetUnknownAttrib(&Block->getAttributesRef(),"SIZE", standardSymbolSize, SA_OVERWRITE, NULL);   
   pen = 0;            // pen width
   size = cnv_unit(atof(standardSymbolSize));
   halfSize = size / 2;
   xDrlsymCenter = halfSize;   // center X/Y of Symbol
   yDrlsymCenter = halfSize;
   xDelta = -xDrlsymCenter;
   yDelta = -yDrlsymCenter;
   width = pen;
   halfWidth = width / 2;
   doubleWidth = 2 * width;

   if(standardSymbolName == "CIRCLE_PLUS")
   {
      DrawCircle(size - width);
      DrawCenterVerticalLine();
      DrawCenterHorizontalLine();
   }
   else if(standardSymbolName == "DIAMOND_PLUS")
   {
      DrawDiamond(halfWidth);
      DrawCenterVerticalLine();
      DrawCenterHorizontalLine();
   }
   else if(standardSymbolName == "SQUARE_DIAMOND")
   {
      DrawSquare();
      DrawDiamond(width);
   }
   else if(standardSymbolName == "SQUARE_CIRCLE")
   {
      DrawCircle(((double)(size - width) / 1.625));
      DrawSquare();
   }
   else if(standardSymbolName == "CIRCLE_SLASH")
   {
      DrawCircle(size - width);
      DrawCircleSlash();
   }
   else if(standardSymbolName == "X")
   {
      DrawHollowX();
   }
   else if(standardSymbolName == "TRIANGLE")
   {
      DrawTriangle();
   }
   else if(standardSymbolName == "CIRCLE")
   {
      DrawCircle(size - width);
   }
   else if(standardSymbolName == "SQUARE")
   {
      DrawSquare();
   }
   else if(standardSymbolName == "HEXAGON")
   {
      DrawHexagon();
   }
   else if(standardSymbolName == "SQUARE_TRIANGLE")
   {
      DrawSquare();
      DrawSmallTriangle();
   }
   else if(standardSymbolName == "STAR_CIRCLE")
   {
      DrawCircle(size/3);
      DrawStarCircle();
   }
   else if(standardSymbolName == "WINDOW")
   {
      DrawSquare();
      DrawCenterVerticalLine();
      DrawCenterHorizontalLine();
   }
   else if(standardSymbolName == "CIRCLE_CIRCLE")
   {
      DrawCircle(size - width);
      DrawCircle(((double)(size - width) / 1.75));
   }
   else if(standardSymbolName == "SQUARE_SLASH")
   {
      DrawSquare();
      DrawSquareSlash();
   }
   else if(standardSymbolName == "CIRCLE_TRIANGLE")
   {
      DrawCircle(size - width);
      DrawSmallTriangle();
   }
   else if(standardSymbolName == "SQUARE_STAR")
   {
      DrawSquare();
      DrawSquareStar();
   }
   else if(standardSymbolName == "2_QUARTER_CIRCLES")
   {
      DrawDoubleCone();
      DrawCenterVerticalLine();
   }
   else if(standardSymbolName == "CIRCLE_SQUARE")
   {
      DrawCircle(size - width);
      DrawSmallSquare();
   }
   else if(standardSymbolName == "CIRCLE_STAR")
   {
      DrawCircle(size - width);
      DrawCircleStar();
   }
   else if(standardSymbolName == "HEXAGON_PLUS")
   {
      DrawHexagon();
      DrawCenterVerticalLine();
      DrawShortCenterHorizontalLine();   
   }
   else if(standardSymbolName == "2_TRIANGLES")
   {
      DrawBowTie();
      DrawCenterHorizontalLine();
   }
   else if(standardSymbolName == "CIRCLE_CIRCLE_PLUS")
   {
      DrawCircle(size - width);
      DrawCircle(((double)(size - width) / 1.75));
      DrawCenterVerticalLine();
      DrawCenterHorizontalLine();
   }
   else if(standardSymbolName == "4_TRIANGLES_DIAMOND")
   {
      DrawTriangle();
      DrawInvertedTriangle();
   }
   else if(standardSymbolName == "PLUS")
   {
      DrawHollowPlus();
   }
   doc->getWidthTable().Add(Block);
   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/

int standard_drill_symbol_size()
{
   standardSymbolSize = strword();
   return 1;
}

/****************************************************************************/
/*
*/

int custom_drill_symbol()
{
   customDrillSymName = strword();
   return 1;
}

/****************************************************************************/
/*
*/

int padform_rectangle()
{
   int   id = cur_ident;

   G.geomtyp = T_RECTANGLE;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}


/****************************************************************************/
/*
*/
int hole_positive_tolerance()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   //G.plusTol = cnv_unit(atof(lp));
   G.plusTol = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int hole_negative_tolerance()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   //G.minusTol = cnv_unit(atof(lp));
   G.minusTol = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int padform_oblong()
{
   int   id = cur_ident;

   G.geomtyp = T_OBLONG;

   go_command(padgeom_lst,SIZ_PADGEOM_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
int padgeom_width()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.xdim = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int padgeom_height()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.ydim = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
int padgeom_chamfer()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.chamfer = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
int padgeom_tie_leg_width()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.tie_leg_width = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
int padgeom_thermal_clearance()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.thermal_clearance = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
int padform_offset()
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
int vb99_rot()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.rotation = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int part_cell_rot()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.comp_rotation = atof(lp);

   return 1;
}

int testPoint_rotation()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.testPointRotation = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
int part_cell_mir()
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
int part_options()
{
   Component_Options = strword();
   Component_Options.TrimLeft();
   Component_Options.TrimRight();
   return 1;
}

/****************************************************************************/
/*
*/
int vb99_mir()
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
int vb99_side()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   
   if (STRCMPI(lp,"MNT_SIDE"))   // if not top
   {
      G.sidebot = TRUE;
      if (cur_status == FILE_SILK_ARTWORK)
      {
         G.level = "SILKSCREEN_BOTTOM";
         G.lyrnr = Graph_Level("SILKSCREEN_BOTTOM", "" , 0);
      }
   }
   else
   {
      G.sidebot = FALSE;
      if (cur_status == FILE_SILK_ARTWORK)
      {
         G.level = "SILKSCREEN_TOP";
         G.lyrnr = Graph_Level("SILKSCREEN_TOP", "" , 0);
      }
   }
   return 1;
}

/****************************************************************************/
/*
   TOP BOTTOM
*/
int vb99_facement()
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
int vb99_fill()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   cur_fill  = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_shape_options()
{
   char  *lp;

   if ((lp = strtok(NULL,WORD_DELIMETER)) == NULL) return 1;

   if (!STRCMPI(lp,"FILLED"))
      cur_fill = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
int vb99_conductive_area_options()
{
   char  *lp;

   while ((lp = strtok(NULL,WORD_DELIMETER)) != NULL)
   {
      if (!STRCMPI(lp,"TEARDROP"))
         cur_conductive_area_teardrop  = true;
      else if(!STRCMPI(lp,"PLANE"))
         cur_conductive_area_plane = true;
   }

   if(cur_conductive_area_plane && use_generated_data)
   {
      CString layerName;
      layerName = G.level;
      layerName = layerName + "_" + "SOURCE_PLANE";
      G.level = layerName;      
      G.lyrnr = Graph_Level(layerName, "" , 0);
   }


   return 1;
}

/****************************************************************************/
/*
*/
int graph_actualPlaneShape()
{
   int   id = cur_ident;

   planeShapeFlag = true;
   G.diameter = 0.;
   actual_plane = 1;
   cur_actual_plane_num = 0;
      
   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   planeShapeFlag = false;
   actual_plane = 0;
   cur_actual_plane_num = 0;
   getPolyarcShapeDataList()->empty();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
int graph_polyline_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0.;

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
int generated_plane_lyr()
{
   int   id = cur_ident;
   char     *lp;
   int   oldstat = cur_status;

   // it is allowed that text has no prosa
   lp = strword();
   G.level = lp;
   G.lyrnr = Graph_Level(G.level,"",0); 

   go_command(plane_lyr_lst,SIZ_PLANE_LYR_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
int plane_lyr_plane_type()
{
   char     *lp;

   if (lp = strword())
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
   return 1;
}

/****************************************************************************/
/*
*/
int plane_lyr_plane_net()
{
   int   id = cur_ident;
   char     *lp;

   lp = strword(); // netname
   G.name = lp;
   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   return 1;
}

int thievingLayerThievingData()
{
   int   id = cur_ident;

   thievingDataFlag = true;

   // Case dts0100488078 - do not default thieving data to filled, honor data as it's found
   cur_fill = 0;

   G.diameter = 0.;

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   thievingDataFlag = false;

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
   G.diameter = 0.;

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
int graph_circle_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0;
   cur_fill = 0;
   G.diameter = 0.;

   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);
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
int graph_circle_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0;
   cur_fill = 0;
   G.diameter = 0.;

   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);
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
int graph_polyarc_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0.;

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
int graph_polyarc_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0.;

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

int graph_aps_hatch_data()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0.;

   apsHatchFlag = true;

   DataStruct* d = FindDataEntity(doc, cur_actual_plane_num); 
   if(d)
   {
      CPolyList* pl = d->getPolyList();
      if(pl)
      {
         POSITION plPOS = pl->GetHeadPosition();
         while(plPOS)
         {
            CPoly *poly = pl->GetNext(plPOS);
            if(poly)
               poly->setFilled(FALSE);
         }
      }
   }

   go_command(graphic_lst,SIZ_GRAPHIC_LST,id);

   for (CDataListIterator polyIterator(*(getPolyarcShapeDataList()),dataTypePoly);polyIterator.hasNext();)
   {
      DataStruct* data = polyIterator.getNext();

      GraphicClassTag grclass = data->getGraphicClass();

      // Case dts0100461892 - Some graphics are exempt from the hide process
      // I have no evidence, but I suspect what this really should come down to
      // is the Hide should be applied only for graphicClassNormal.
      // But with no evidence and no test case, I am not going to go that
      // far in response to the case at hand.
      // For case dts0100461892, it is really only the exemptions for
      // PanelOutline and PlaceKeepIn that are needed. I added the
      // BoardOutline and ComponentOutline on speculation. It seems if this
      // is to apply to one kind of outline then it probably would apply to all.

      for (POSITION pos = data->getPolyList()->GetHeadPosition();pos != NULL;)
      {
         CPoly* poly = data->getPolyList()->GetNext(pos);

         poly->setFilled(false);

         // Case dts100461892 - They don't want to see these at all
         //poly->setHidden(true);

         // Case dts0100461892 - But they do want to see some of them !
         if (grclass != graphicClassPanelOutline &&
            grclass  != graphicClassBoardOutline &&
            grclass  != graphicClassComponentOutline &&
            grclass  != graphicClassPlaceKeepIn)
         {
            poly->setHidden(true);
         }
      }
   }

   getPolyarcShapeDataList()->empty();

   if (polycnt)
      write_poly();

   apsHatchFlag = false;

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XYR
       LVL
       WDT
*/
int graph_rect_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0.;

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
int graph_rect_path()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0.;

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

/****************************************************************************
*
*/
int graph_obstruct()
{
   int   id = cur_ident;

   go_command(des_lst,SIZ_DES_LST,id);
   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
int poly_line()
{
   int   id = cur_ident;
   G.diameter = 0.;

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
   if(lp = strword())
   {
      // Then, we got the group ID
      if(!strcmp(lp, "MGC_UNIGROUP"))
      {
         G.unigroup = strword();
      }
   }

   prosa = lp;

   polycnt = 0;
   G.mirror = 0;
   G.rotation = 0;
   G.text_type = "";
   G.textwidth = 0;  // according to VB ASCII desc. this is ignored.
   G.just = VBTEXT_N_A;

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
   G.diameter = 0.;

   go_command(grapharc_lst,SIZ_GRAPHARC_LST,id);

   int      p1 = polycnt;
   double   x1,y1,x2,y2,bulge;
   double   center_x = poly_l[polycnt-1].x;
   double   center_y = poly_l[polycnt-1].y;

   double   startangle = DegToRad(G.startangle);
   double   deltaangle = DegToRad(G.deltaangle);
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
   G.diameter = 0.;

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
int write_arc()
{
   int   err, widthindex;

   if (G.diameter == 0)
      widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   else
      widthindex = Graph_Aperture("", T_ROUND, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   double   startangle = DegToRad(G.startangle);
   double   deltaangle = DegToRad(G.deltaangle);

   if (fabs(G.deltaangle - 360) < 1)
      Graph_Circle(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 
         0L, widthindex , FALSE, TRUE); 
   else
      Graph_Arc(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 
         startangle, deltaangle,       
         0L, widthindex , FALSE); 

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
int write_circle()
{
   int   err, widthindex;

   // If the call comes from des_part, we would create a temp goemetry to store to data
   if (!G.unigroup.IsEmpty())
      Graph_Block_On(GBO_APPEND,"Temp_Poly", -1, 0);

   if (G.diameter == 0)
      widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   else
      widthindex = Graph_Aperture("", T_ROUND, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *d = Graph_Circle(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 
         0L, widthindex , FALSE, cur_fill); 

   if (cur_status == FILE_ROUTE_OUTL)
   {
      d->setGraphicClass(GR_CLASS_ROUTKEEPIN);
   }
   else if (cur_status == FILE_BOARD_OUTL)
   {
      d->setGraphicClass(GR_CLASS_BOARDOUTLINE);
   }
   else if (cur_status == FILE_PANEL_OUTL) // panel
   {
      d->setGraphicClass(GR_CLASS_PANELOUTLINE);
   }
   else if (cur_status == FILE_PANEL_BORDER) // panel
   {
      d->setGraphicClass(GR_CLASS_PANELBORDER);
   }
   else if (cur_status == FILE_ASSM_OUTL)
   {
      d->setGraphicClass(GR_CLASS_COMPOUTLINE);
   }
   else if (cur_status == FILE_ROUTE_OBS)
   {
      d->setGraphicClass(GR_CLASS_ROUTKEEPOUT);
   }
   else if (cur_status == FILE_PLACE_OBS)
   {
      d->setGraphicClass(GR_CLASS_PLACEKEEPOUT);
   }
   else if (cur_status == FILE_VIA_OBS)
   {
      d->setGraphicClass(GR_CLASS_VIAKEEPOUT);
   }
   else if (cur_status == FILE_CONTOUR)
   {
      d->setGraphicClass(GR_CLASS_CONTOUR);
      doc->SetUnknownAttrib(&d->getAttributesRef(),"COMP", contourComp.GetBuffer(), SA_OVERWRITE, NULL);
      doc->SetUnknownAttrib(&d->getAttributesRef(),"DIRECTION", contourDirection.GetBuffer(), SA_OVERWRITE, NULL);
      doc->SetUnknownAttrib(&d->getAttributesRef(),"HOLE", contourHole.GetBuffer(), SA_OVERWRITE, NULL);

      CString temp;
      temp.Format(".HOLE_%s", contourHole);
      
      int holeGeomNum = doc->Get_Block_Num(temp.GetBuffer(), -1, 0);
      temp.Format("%d", holeGeomNum);
      doc->SetUnknownAttrib(&d->getAttributesRef(),"HOLE_GEOM", temp.GetBuffer(), SA_OVERWRITE, NULL);

   }

   if (!G.unigroup.IsEmpty())
      Graph_Block_Off();

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
int write_text(const char *tmp)
{

   double   x, y;
   int      prop = TRUE;   // veribest text is proportional
                           // also a text H125 W125 seems to be H125 W100

   if(Component_Options == "NOT_PLACED")
      return 1;

   x = poly_l[0].x;
   y = poly_l[0].y;

   HorizontalPositionTag horzAlign = horizontalPositionCenter;
   VerticalPositionTag   vertAlign = verticalPositionBaseline;

   if (G.just & VBTEXT_H_LEFT)
      horzAlign = horizontalPositionLeft;
   else if (G.just & VBTEXT_H_CENTER)
      horzAlign = horizontalPositionCenter;
   else if (G.just & VBTEXT_H_RIGHT)
      horzAlign = horizontalPositionRight;

   if (G.just & VBTEXT_V_TOP)
      vertAlign = verticalPositionTop;
   else if (G.just & VBTEXT_V_CENTER)
      vertAlign = verticalPositionCenter;
   else if (G.just & VBTEXT_V_BOTTOM)
      vertAlign = verticalPositionBottom;

   if (!G.text_type.CompareNoCase("PARTNO"))
   {
      G.name_1 = tmp;
      G.name_1.MakeUpper();
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
      int laynr = Graph_Level(G.level,"",0); 

      if (G.AttribMap)
      {
         if (polycnt)
         {
            /*doc->SetUnknownVisAttrib(G.AttribMap, ATT_TYPELISTLINK, G.name_1, x, y, DegToRad(G.rotation), G.height, 
            G.height*TEXT_ASPECT_RATIO, prop, G.mirror, TRUE, SA_OVERWRITE, 0,laynr, 0, horzAlign, vertAlign);*/
            // Fix for dts0100511673 Assembly/Silkscreen Part numbers are not imported into VisEDOC
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

            CString keyword = Get_Next_PartNo(*G.AttribMap);
            doc->SetUnknownVisAttrib(G.AttribMap, keyword, G.name_1, xoff, yoff, DegToRad(G.rotation), G.height, 
               G.height*TEXT_ASPECT_RATIO, prop, G.mirror, TRUE, SA_OVERWRITE, 0,laynr, 0, horzAlign, vertAlign);
         }
         else
         {
            doc->SetUnknownAttrib(G.AttribMap, ATT_TYPELISTLINK, G.name_1,
               SA_OVERWRITE, NULL); // x, y, rot, height
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

      int laynr = Graph_Level(G.level,"",0); 

      if (G.AttribMap)
      {
         CString keyword = Get_Next_Refname(*G.AttribMap);
         
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

            doc->SetUnknownVisAttrib(G.AttribMap, keyword, G.ref_des, xoff, yoff, DegToRad(G.rotation), G.height, 
               G.height*TEXT_ASPECT_RATIO, prop, G.mirror, TRUE, SA_OVERWRITE, 0,laynr, 0, horzAlign, vertAlign);
         }
         else
         {
            doc->SetUnknownAttrib(G.AttribMap, keyword, G.ref_des, SA_RETURN, NULL); // SA_RETURN for refname, so that it does not overwrite the placed one.
         }
      }
   }
   else if (!G.text_type.CompareNoCase("USER_TYPE"))
   {
      if (polycnt && strlen(tmp))
      {
         int laynr = Graph_Level(G.level,"",0); 
         Graph_Text(laynr,tmp, x, y,G.height,G.height*TEXT_ASPECT_RATIO, DegToRad(G.rotation), 0L, prop, G.mirror, 0, FALSE, -1, 0, horzAlign, vertAlign); 
      }
   }
   else if (!G.text_type.CompareNoCase("DRILL_DRAWING"))
   {
      if (polycnt && strlen(tmp))
      {
         int laynr = Graph_Level(G.level,"",0); 
         Graph_Text(laynr,tmp, x, y,G.height,G.height*TEXT_ASPECT_RATIO, DegToRad(G.rotation), 0L, prop, G.mirror, 0, FALSE, -1, 0, horzAlign, vertAlign); 
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
* Get_Next_PartNo
* Fix for dts0100511673 Assembly/Silkscreen Part numbers are not imported into VisEDOC
*/
CString Get_Next_PartNo(CAttributes* map)
{
   CString partNo;
   int count = 0;

   if (map != NULL)
   {      
      for (POSITION pos = map->GetStartPosition();pos != NULL;)
      {
         WORD keyword;
         Attrib* attrib;

         map->GetNextAssoc(pos, keyword, attrib);

         CString tmpKeyword = doc->getKeyWordArray()[keyword]->cc;

         if (tmpKeyword.Find("PARTNO") > -1)
            count++;
      }
   }

   if (count)
      partNo.Format("%s_%d", "PARTNO", count);
   else
      partNo = "PARTNO";

   return partNo;
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
         WORD keyword;
         Attrib* attrib;

         map->GetNextAssoc(pos, keyword, attrib);

         CString tmpKeyword = doc->getKeyWordArray()[keyword]->cc;

         if (tmpKeyword.Find("REFNAME") > -1)
            count++;
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
      fprintf(ferr,"Error, Line at %ld in [%s] not completely read.\n",
         getInputFileLineNumber(),cur_filename);  
      fprintf(ferr,"The max number of characters per line is %ld.\n", strlen(b));  
      display_error++;
   }
   else
      b[strlen(b)-1] = '\0';
   return;
}

/****************************************************************************/
/*
*/
int overwrite_pad()
{
   int   ptr;
   char  padname[80];

   return 1; // I do not think that this is needed anymore for VB99 and 2000.
             // you can not overwrite a padstack anymore according to the VB Ascii spec.

   if (curpad_cnt == 0)  return 1;

   ptr = update_localpad(padname);

   if (lastpininsert)
   {
      lastpininsert->getInsert()->setBlockNumber(doc->Get_Block_Num(padname, -1, 0));
   }

   curpad_cnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
int update_localpad(char *pn)
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

      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,pn,-1,0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      int   xpadflg = 0;
      int   smd = TRUE;
      // also watch out for rotation
      for (i=0;i<curpad_cnt;i++)
      {
         if (strlen(curpad[i].padgeomname))
         {
            if (!curpad[i].smd)  smd = FALSE;

            Graph_Block_Reference(curpad[i].padgeomname, NULL, 0, 0.0, 0.0, 0.0, 
               0 , 1.0, curpad[i].layerptr, TRUE);

            LayerStruct *l = doc->FindLayer(curpad[i].layerptr);
            // here do SMT detect
            if (!STRNICMP(l->getName(),"PadLayer",strlen("PadLayer")))
            {
               int lnr = atoi(l->getName().Right(strlen(l->getName()) - strlen("PadLayer")));
               if (lnr == 1)
                  xpadflg |= 1;
               else
               if (lnr == NumberOfLayers)
                  xpadflg |= 2;
               else
                  xpadflg |= 4;
            }
         }
      }

      // here check for SMD
      if (xpadflg == 1 || smd)      // Top SMD
      {
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE, NULL, SA_OVERWRITE, NULL); //  
      }
      else
      if (xpadflg == 2)    // Bottom only
      {
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_BLINDSHAPE, TRUE),
            VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height
      }

      Graph_Block_Off();

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
int read_netfile(FileStruct *fl)
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
            n = add_net(fl,netname);
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
            add_comppin(fl, n, compname, pinname);
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
// CVBPin
/******************************************************************************/
CVBPin::CVBPin(CString name, CString pinOption, int padstackBlkNum)
{
   m_name = name;
   m_pinOption = GetSideOption(pinOption);
   m_ccPadstackBlkNum = padstackBlkNum;
}

CVBPin::CVBPin(const CVBPin& other)
{
   if (&other != this)
   {
      m_name = other.m_name;
      m_pinOption = other.m_pinOption;
      m_ccPadstackBlkNum = other.m_ccPadstackBlkNum;
   }
}

CVBPin::~CVBPin()
{
}


/******************************************************************************/
// CVBGeom
/******************************************************************************/
CVBGeom::CVBGeom(CString name, int geomNum)
{
   m_name = name;
   m_originalName = name;
   m_geomNum = geomNum;
   m_pinList.empty();
   m_deriveGeomList.empty();
   m_silkscreenSide = sideOptionNone;
   m_soldermaskSide = sideOptionNone;
}

CVBGeom::~CVBGeom()
{
   m_pinList.empty();
   m_deriveGeomList.empty();
}

void CVBGeom::SetSilkscreenSide(const CString side)
{ 
   m_silkscreenSide = GetSideOption(side);
}

void CVBGeom::SetSoldermaskSide(const CString side)   
{ 
   m_soldermaskSide = GetSideOption(side);   
}

CVBPin* CVBGeom::AddPin(CString pinName, CString pinOption, int padstackBlkNum)
{
   CVBPin* pin = FindPin(pinName);
   if (pin == NULL)
   {
      pin = new CVBPin(pinName, pinOption, padstackBlkNum);
      m_pinList.AddTail(pin);
   }

   return pin;
}

CVBPin* CVBGeom::FindPin(CString pinName)
{
   POSITION pos = m_pinList.GetHeadPosition();
   while (pos)
   {
      CVBPin* pin = m_pinList.GetNext(pos);
      if (pin == NULL)
         continue;

      if (pin->GetName().CompareNoCase(pinName) == 0)
         return pin;
   }

   return NULL;
}

bool CVBGeom::IsDifference(CVBGeom* otherVBGeom)
{
   if (m_originalName.CompareNoCase(otherVBGeom->m_originalName) != 0)
      return true;

   POSITION pos = otherVBGeom->m_pinList.GetHeadPosition();
   while (pos)
   {
      CVBPin* otherPin = otherVBGeom->m_pinList.GetNext(pos);
      if (otherPin == NULL)
         continue;

      CVBPin* geomPin = FindPin(otherPin->GetName());
      if (geomPin == NULL)
         return true;

      if (otherPin->GetPinOption() != geomPin->GetPinOption())
         return true;

      if (otherPin->GetPadstackBlkNum() != geomPin->GetPadstackBlkNum())
         return true;
   }

   if (m_silkscreenSide != otherVBGeom->m_silkscreenSide)
      return true;
   if (m_soldermaskSide != otherVBGeom->m_soldermaskSide)
      return true;

   return false;
}

void CVBGeom::AddDerivedGeom(CVBGeom* otherVBGeom)
{
   CVBGeom* vbGeom = new CVBGeom(otherVBGeom->m_name, otherVBGeom->m_geomNum);

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
      CVBPin* otherPin = otherVBGeom->m_pinList.GetNext(pos);
      if (otherPin == NULL)
         continue;

      CVBPin* newPin = new CVBPin(*otherPin);
      vbGeom->m_pinList.AddTail(newPin);
   }

   m_deriveGeomList.AddTail(vbGeom);
}

CVBGeom* CVBGeom::CheckForDerivedGeom(CVBGeom* otherVBGeom)
{
   POSITION pos = m_deriveGeomList.GetHeadPosition();
   while (pos != NULL)
   {
      CVBGeom* vbGeom = m_deriveGeomList.GetNext(pos);
      if (vbGeom == NULL)
         continue;

      if (!vbGeom->IsDifference(otherVBGeom))
         return vbGeom;
   }

   return NULL;
}

CString CVBGeom::GetDerivedGeomName()
{
   CString derivedName;
   derivedName.Format("%s_%d", this->m_name, this->m_deriveGeomList.GetCount()+1);
   return derivedName;
}


/******************************************************************************/
// CVBGeomMap
/******************************************************************************/
CVBGeomMap::CVBGeomMap()
{
   lastAddedGeom = NULL;
   m_geomMap.empty();
}

CVBGeomMap::~CVBGeomMap()
{
   lastAddedGeom = NULL;
   m_geomMap.empty();
}

CVBGeom* CVBGeomMap::AddGeom(CString name, int geomNum)
{
   CVBGeom* vbGeom = FindGeom(name);
   if (vbGeom == NULL)
   {
      vbGeom = new CVBGeom(name, geomNum);
      m_geomMap.SetAt(vbGeom->GetName(), vbGeom);
   }
   lastAddedGeom = vbGeom;

   return vbGeom;
}

CVBGeom* CVBGeomMap::FindGeom(CString name)
{
   CVBGeom* vbGeom = NULL;
   m_geomMap.Lookup(name, vbGeom);

   return vbGeom;
}

CVBGeom* CVBGeomMap::GetLastGeom()
{
   return lastAddedGeom;
}

void CVBGeomMap::Empty()
{
   m_geomMap.empty();
}

//_________________________________________________________________________________________________
CVbPcbInstance::CVbPcbInstance(const CString& refDes)
: m_refDes(refDes)
, m_rotationRadians(0.)
, m_topFacement(true)
, m_pcbInstanceData(NULL)
{
}

CString CVbPcbInstance::getRefDes() const
{
   return m_refDes;
}

CString CVbPcbInstance::getLocalPath() const
{
   return m_localPath;
}

void CVbPcbInstance::setLocalPath(const CString& path)
{
   m_localPath = path;
}

CString CVbPcbInstance::getSourcePath() const
{
   return m_sourcePath;
}

void CVbPcbInstance::setSourcePath(const CString& path)
{
   m_sourcePath = path;
}

CString CVbPcbInstance::getHkpPath() const
{
   return m_hkpPath;
}

void CVbPcbInstance::setHkpPath(const CString& path)
{
   m_hkpPath = path;
}

DataStruct* CVbPcbInstance::getPcbInstanceData() const
{
   return m_pcbInstanceData;
}

void CVbPcbInstance::setPcbInstanceData(DataStruct* data)
{
   m_pcbInstanceData = data;
}

CPoint2d CVbPcbInstance::getOrigin() const
{
   return m_origin;
}

void CVbPcbInstance::setOrigin(const CPoint2d& origin)
{
   m_origin = origin;
}

double CVbPcbInstance::getRotationRadians() const
{
   return m_rotationRadians;
}

void CVbPcbInstance::setRotationRadians(double radians)
{
   m_rotationRadians = radians;
}

bool CVbPcbInstance::getTopFacement() const
{
   return m_topFacement;
}

void CVbPcbInstance::setTopFacement(bool flag)
{
   m_topFacement = flag;
}

//_________________________________________________________________________________________________
CVbPcbInstances* CVbPcbInstances::m_pcbInstances = NULL;

CVbPcbInstances& CVbPcbInstances::getPcbInstances()
{
   if (m_pcbInstances == NULL)
   {
      m_pcbInstances = new CVbPcbInstances();
   }

   return *m_pcbInstances;
}

void CVbPcbInstances::release()
{
   delete m_pcbInstances;

   m_pcbInstances = NULL;
}

CVbPcbInstances::CVbPcbInstances()
{
}

void CVbPcbInstances::empty()
{
   m_pcbInstanceList.empty();
}

CVbPcbInstance* CVbPcbInstances::addPanelInstance(const CString& refDes)
{
   CVbPcbInstance* pcbInstance = new CVbPcbInstance(refDes);

   m_pcbInstanceList.AddTail(pcbInstance);

   return pcbInstance;
}

int CVbPcbInstances::getCount() const
{
   return m_pcbInstanceList.GetCount();
}

POSITION CVbPcbInstances::getHeadPosition() const
{
   return m_pcbInstanceList.GetHeadPosition();
}

CVbPcbInstance* CVbPcbInstances::getNext(POSITION& pos) const
{
   return m_pcbInstanceList.GetNext(pos);
}

CVbPcbInstance& CVbPcbInstances::getLast()
{
   if (m_pcbInstanceList.GetCount() <= 0)
   {
      addPanelInstance("");
   }

   CVbPcbInstance* pcbInstance = m_pcbInstanceList.GetTail();

   return *pcbInstance;
}

void CVbPcbInstances::insertPcbInstances(FileStruct& panelSubFile,FileStruct& pcbSubFile,const CString& pcbDesignPath)
{
   Graph_Block_On(panelSubFile.getBlock());

   for (POSITION pos = m_pcbInstanceList.GetHeadPosition();pos != NULL;)
   {
      CVbPcbInstance* pcbInstance = m_pcbInstanceList.GetNext(pos);

      if (pcbInstance->getLocalPath().CompareNoCase(pcbDesignPath) == 0 &&
          pcbInstance->getPcbInstanceData() == NULL)
      {
         CString pcbGeometryName = pcbSubFile.getBlock()->getName();
         int pcbSubFileNumber = pcbSubFile.getFileNumber();
         CPoint2d origin = pcbInstance->getOrigin();
         CString refDes = pcbInstance->getRefDes();
         // Case dts0100496117 - note that CAMCAD mirrors about Y, Expedition mirrors about X, so conversino of angle
         // for bottom side is not the usual reversal of angle (360 - angle) as is done in normal CAMCAD mirroring.
         // For mirror about X, the angle is essentially off by 180, so we do (angle - 180), but its done in radians, w/ Pi.
         double rotationRadians = normalizeRadians( pcbInstance->getTopFacement() ? pcbInstance->getRotationRadians() : pcbInstance->getRotationRadians() - Pi );
         int mirror = (pcbInstance->getTopFacement() ? 0 : MIRROR_ALL);
         double scale = 1.;
         int layerNumber = -1;
         int global = 0;
         BlockTypeTag blockType = blockTypePcb;

         DataStruct* data = Graph_Block_Reference(pcbGeometryName,refDes,pcbSubFileNumber,origin.x,origin.y,rotationRadians,mirror,scale,layerNumber,global,blockType);
         data->getInsert()->setInsertType(insertTypePcb);
         pcbInstance->setPcbInstanceData(data);
      }
   }

   Graph_Block_Off();
}

/******************************************************************************/
// CDcaVBNetContainerMap
/******************************************************************************/
void CVBNetContainerMap::Release()
{
   POSITION pos = GetStartPosition();
   while(pos)
   {
      CString netName;
      VB99NetContainer *data = NULL;
      GetNextAssoc(pos,netName,data);
      if(data)
         delete data;
   }
   RemoveAll();
}

VB99NetContainer* CVBNetContainerMap::SetDefineNetAt(CString netName)
{
   VB99NetContainer *data = NULL;
   if(this->Lookup(netName,data) == NULL || data == NULL)
   {
      data = new VB99NetContainer;
      SetAt(netName,data);
      data->hasTrace = false;
      data->hasVia = false;
   }
   return data;
}

void CVBNetContainerMap::SetHasViaAt(CString netName, bool hasVia)
{
   VB99NetContainer *data = SetDefineNetAt(netName);  
   if(data)
      data->hasVia= hasVia;
}

void CVBNetContainerMap::SetHasTraceAt(CString netName, bool hasTrace)
{
   VB99NetContainer *data = SetDefineNetAt(netName);  
   if(data)
      data->hasTrace= hasTrace;
}

void CVBNetContainerMap::SetNetContainerAt(CString netName, VB99NetContainer *netdata)
{
   VB99NetContainer *data = SetDefineNetAt(netName);  
   if(data)
   {
      data->hasTrace= netdata->hasTrace;
      data->hasVia= netdata->hasVia;
   }
}

bool CVBNetContainerMap::IsEmptyRoute(CString netName)
{
   VB99NetContainer *data = SetDefineNetAt(netName);  
   if(data)
      return (!data->hasTrace && !data->hasVia);

   return true;
}
//_________________________________________________________________________________________________

void DrawCircle(double diameter)
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   int widthindex,err;
   widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   Graph_Circle(laynr,x,y, diameter/2, 
        0L, widthindex , FALSE, FALSE);       
}

void DrawCenterVerticalLine()
{   
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(halfSize + xDelta + x,width + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,size - width + yDelta + y,0.);   
}

void DrawCenterHorizontalLine()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(width + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(size - width + xDelta + x,halfSize + yDelta + y,0.);   
}

void DrawDiamond(double lwidth)
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(halfSize + xDelta + x,lwidth + yDelta + y,0.);
   Graph_Vertex(size - lwidth + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,size - lwidth + yDelta + y,0.);
   Graph_Vertex(lwidth + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,lwidth + yDelta + y,0.);   
}

void DrawSquare()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(halfWidth + xDelta + x, halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x, size - halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x, halfWidth + yDelta + y,0.);   
}

void DrawCircleSlash()
{
   double rx = width - xDrlsymCenter;
    double ry = halfSize - yDrlsymCenter;
    
    double x1 = ( rx * kCos45 - ry * kSin45 ) + xDrlsymCenter;
    double y1 = ( rx * kSin45 + ry * kCos45 ) + yDrlsymCenter;
    
    rx = size - width - xDrlsymCenter;
    ry = halfSize - yDrlsymCenter;
    
    double x2 = ( rx * kCos45 - ry * kSin45 ) + xDrlsymCenter;
    double y2 = ( rx * kSin45 + ry * kCos45 ) + yDrlsymCenter;

   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(x1 + xDelta + x, y1 + yDelta + y,0.);
   Graph_Vertex(x2 + xDelta + x, y2 + yDelta + y,0.); 
}

void DrawHollowX()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(doubleWidth + xDelta + x, halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,halfSize - width - halfWidth + yDelta + y,0.);
   Graph_Vertex(size - doubleWidth + xDelta + x, halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,doubleWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + width + halfWidth + xDelta + x, halfSize + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x, size - doubleWidth + yDelta + y,0.);
   Graph_Vertex(size - doubleWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x, halfSize + width + halfWidth + yDelta + y,0.);
   Graph_Vertex(doubleWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x, size - doubleWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - width - halfWidth + xDelta + x, halfSize + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,doubleWidth + yDelta + y,0.);
   Graph_Vertex(doubleWidth + xDelta + x, halfWidth + yDelta + y,0.);
}

void DrawTriangle()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(halfWidth + xDelta + x, halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x, size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,halfWidth + yDelta + y,0.);   
}

void DrawHexagon()
{
   double symFactor = ((double) size / 3.5);
   double f=.85;
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(x, halfWidth + yDelta + y,0.);
   Graph_Vertex((((halfSize - halfWidth) * f)) + x,symFactor + yDelta + y,0.);
   Graph_Vertex((((halfSize - halfWidth) * f)) + x,halfSize - symFactor + y,0.);
   Graph_Vertex(x,halfSize - halfWidth + y,0.);
   Graph_Vertex((((halfWidth + xDelta) * f)) + x,halfSize - symFactor + y,0.);
   Graph_Vertex((((halfWidth + xDelta) * f)) + x,symFactor + yDelta + y,0.);
   Graph_Vertex(x,halfWidth + yDelta + y,0.);
}

void DrawSmallTriangle()
{
   double xFactor = (kCos30 * (double)(halfSize - 2 * width));
   double yFactor = (kSin30 * (double)(halfSize - 2 * width));
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex((halfSize - xFactor + halfWidth) + xDelta + x, (halfSize - yFactor) + yDelta + y,0.);
   Graph_Vertex((halfSize + xFactor - halfWidth) + xDelta + x,(halfSize - yFactor) + yDelta + y,0.);
   Graph_Vertex((halfSize) + xDelta + x,(size - doubleWidth) + yDelta + y,0.);
   Graph_Vertex((halfSize - xFactor + halfWidth) + xDelta + x,(halfSize - yFactor) + yDelta + y,0.);
}

void DrawStarCircle()
{
   double symFactor = (kCos45 * (double)(size / 6));
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(halfSize + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor + xDelta + x,halfSize - symFactor + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor + xDelta + x,halfSize + symFactor + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor + xDelta + x,halfSize + symFactor + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor + xDelta + x,halfSize - symFactor + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,halfWidth + yDelta + y,0.);
}

void DrawSquareSlash()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);   
}

void DrawSquareStar()
{
   // Get the factor needed to determine where the lines will be drawn to
   double symFactor = ((double) size / 6);
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed

   // Draw the bottom right lines
   Graph_Vertex(halfSize + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,width + halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor / 2 + xDelta + x,halfSize - symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(size - width - halfWidth + xDelta + x,halfSize + yDelta + y,0.);

   // Draw the top right lines
   Graph_Vertex(size - width + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(size - width - halfWidth + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor / 2 + xDelta + x,halfSize + symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,size - width - halfWidth + yDelta + y,0.);

   // Draw the top left lines
   Graph_Vertex(halfSize + xDelta + x,size - width + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,size - width - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor / 2 + xDelta + x,halfSize + symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(width + halfWidth + xDelta + x,halfSize + yDelta + y,0.);

   // Draw the bottom left lines
   Graph_Vertex(halfWidth + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(width + halfWidth + xDelta + x,halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor / 2 + xDelta + x,halfSize - symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,width + halfWidth + yDelta + y,0.);

}

void DrawDoubleCone()
{
   //PENDING
   double symFactor = (kCos45 * (double)(halfSize - halfWidth));
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   int widthindex,err;
   widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   double   startangle = DegToRad(45);
   double   deltaangle = DegToRad(90);
   Graph_Arc(laynr,0,0, size/2, startangle, deltaangle, 
        0L, widthindex , FALSE);
   startangle = DegToRad(-135);
   deltaangle = DegToRad(90);
   Graph_Arc(laynr,0,0, size/2, startangle, deltaangle, 
        0L, widthindex , FALSE);


   // Draw the connecting lines
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(-symFactor + x,-symFactor + y,0.);
   Graph_Vertex(symFactor + x,symFactor + y,0.);

   // Draw the connecting lines
   data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(symFactor + x,-symFactor + y,0.);
   Graph_Vertex(-symFactor + x,symFactor + y,0.);
}

void DrawSmallSquare()
{
   double symFactor = size * 3 / 7;
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed
   Graph_Vertex(halfSize - symFactor / 2 + halfWidth + xDelta + x, halfSize - symFactor / 2 + halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor / 2 + halfWidth + xDelta + x,halfSize + symFactor / 2 - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor / 2 - halfWidth + xDelta + x, halfSize + symFactor / 2 - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor / 2 - halfWidth + xDelta + x,halfSize - symFactor / 2 + halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor / 2 + halfWidth + xDelta + x, halfSize - symFactor / 2 + halfWidth + yDelta + y,0.);
}

void DrawCircleStar()
{
   // Get the factor needed to determine where the lines will be drawn to
   double symFactor = ((double) size / 6);
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed

   // Draw the bottom right lines
   Graph_Vertex(halfSize + xDelta + x, 3 * width + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor / 2 + xDelta + x,halfSize - symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(size - 3 * width + xDelta + x, halfSize + yDelta + y,0.);

   // Draw the top right lines
   Graph_Vertex(size - 3 * width + xDelta + x, halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize + symFactor / 2 + xDelta + x,halfSize + symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,size - 3 * width + yDelta + y,0.);

   // Draw the top left lines
   Graph_Vertex(halfSize + xDelta + x, size - 3 * width + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor / 2 + xDelta + x,halfSize + symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(3 * width + xDelta + x,halfSize + yDelta + y,0.);

   // Draw the bottom left lines
   Graph_Vertex(3 * width + xDelta + x, halfSize + yDelta + y,0.);
   Graph_Vertex(halfSize - symFactor / 2 + xDelta + x,halfSize - symFactor / 2 + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,3 * width + yDelta + y,0.);
}

void DrawShortCenterHorizontalLine()
{
   double f =.85;

   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed

   Graph_Vertex((xDelta * f) + width + x, halfSize + yDelta + y,0.);
   Graph_Vertex(((size + xDelta) * f) - width + x,halfSize + yDelta + y,0.);   
}

void DrawBowTie()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed

   Graph_Vertex(halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
}

void DrawInvertedTriangle()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed

   Graph_Vertex(halfSize + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + xDelta + x,halfWidth + yDelta + y,0.);
}

void DrawHollowPlus()
{
   int laynr = Graph_Level("DRILLSYMBOL", "", 0);
   DataStruct *data = Graph_PolyStruct(laynr, 0, FALSE);
   Graph_Poly(NULL, 0, FALSE, 0, TRUE); // filled, void, closed

   Graph_Vertex(halfSize - width - halfWidth + xDelta + x, halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + width + halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + width + halfWidth + xDelta + x,halfSize - width - halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,halfSize - width - halfWidth + yDelta + y,0.);
   Graph_Vertex(size - halfWidth + xDelta + x,halfSize + width + halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + width + halfWidth + xDelta + x,halfSize + width + halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize + width + halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - width - halfWidth + xDelta + x,size - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - width - halfWidth + xDelta + x,halfSize + width + halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,halfSize + width + halfWidth + yDelta + y,0.);
   Graph_Vertex(halfWidth + xDelta + x,halfSize - width - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - width - halfWidth + xDelta + x,halfSize - width - halfWidth + yDelta + y,0.);
   Graph_Vertex(halfSize - width - halfWidth + xDelta + x,halfWidth + yDelta + y,0.);
}

