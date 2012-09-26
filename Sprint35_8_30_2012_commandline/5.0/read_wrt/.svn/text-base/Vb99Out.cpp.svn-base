// $Header: /CAMCAD/4.6/read_wrt/Vb99Out.cpp 83    4/23/07 5:26p Lynn Phung $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1999. All Rights Reserved.


   filename is used for technology file and netlist file.
   this should be changed to the true PCB file name, so it would
   allow mulitple PCB's to be translated.


*/

#include "stdafx.h"
#include "ccdoc.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include <math.h>
#include "outline.h"
#include "ck.h"
#include "pcblayer.h"
#include "polylib.h"
#include "vb99out.h"
#include "CamCadDatabase.h"
#include "TMState.h"
#include "CCEtoODB.h"
#include "CcRepair.h"
#include <set>
#include "EncryptStreamOutput.h"
#include "RwUiLib.h"
#include "DcaDftCommon.h"

// So many names for the same thing. This is just to make it easier to find again, at least, for me.
// VB99 Expedition HKP Writer

enum CellTypeTag
{
   packageCell,
   mechanicalCell,
   drawingCell,
   undefinedCellType
};

enum writtenHKP
{
   noFile = 0,
   cellFile = 1,
   jobFile = 2,
   layoutFile = 3,
   netclassFile = 4,
   netpropsFile = 5,
   padstackFile = 6,
   pdbFile = 7,
   kyinnetlistFile = 8
};

void convertSmdViasToTestPoints(CCamCadDatabase& camCadDatabase);

// dump for test the trace function.
//#define   DUMP  
#undef   DUMP

/* External Variables ***************************************************************************/

extern   CProgressDlg      *progress;
extern   char              *shapes[];

/* Static Variables ***************************************************************************/

// here are the classes with underscore, thats easer to scan from the .out file
static char *lclasses[] = 
{
   "UNKNOWN",
   "BOARD_GEOMETRY",
   "PACKAGE_GEOMETRY",
   "ETCH",
   "DRAWING_FORMAT",
   "MANUFACTURING",
   "COMPONENT_VALUE",
   "DEVICE_TYPE",
   "REF_DES",
   "TOLERANCE",
   "USER_PART_NUMBER",
   "PACKAGE_KEEPIN",
   "PACKAGE_KEEPOUT",
   "ROUTE_KEEPIN",
   "ROUTE_KEEPOUT",
   "VIA_KEEPOUT",
   "ANTI_ETCH",
   "PIN",
   "VIA_CLASS",
};

// original are like the lclasses, but no underscore
static char *origclasses[] = 
{
   "UNKNOWN",
   "BOARD GEOMETRY",
   "PACKAGE GEOMETRY",
   "ETCH",
   "DRAWING FORMAT",
   "MANUFACTURING",
   "COMPONENT VALUE",
   "DEVICE TYPE",
   "REF DES",
   "TOLERANCE",
   "USER PART NUMBER",
   "PACKAGE KEEPIN",
   "PACKAGE KEEPOUT",
   "ROUTE KEEPIN",
   "ROUTE KEEPOUT",
   "VIA KEEPOUT",
   "ANTI ETCH",
   "PIN",
   "VIA CLASS",
};

enum VB99RouteType
{
   vb99RouteTypeUnknown,
   vb99RouteTypeObstruct,
   vb99RouteTypeConductiveArea,
   vb99RouteTypeTrace
};


static   CCEtoODBDoc        *doc;
static   FileStruct        *file;
static   FILE					*flog;

static   double            mils_2_units;
static   double            unitsFactor;
static   int               display_error;
static   int               page_units;
static   int               max_signalcnt;    // this is the nunber of XRF layers

static   char					*output_units_string = "TH";
static   int               output_units = UNIT_MILS;
static   int               output_units_accuracy = 3;
static   double            vb99_smallnumber = 0.001;
static   double            one_mil = 1;
static   int               identcnt = 0;
static   double            antipadenlargement = 0;
static   BOOL					pinNetInCell = FALSE;
static   BOOL					encryptOutputFlag = FALSE;

static   CString           ident;
static   CString           default_fontsize;
static   CString           default_hole;
static   CString           topsignallayer, bottomsignallayer;

static   double            drillsymbolarray[255];
static   int               drillsymbolcnt;

static   CompPinInstArray  comppininstarray; 
static   int               comppininstcnt;

typedef  CArray<Point2, Point2&> CPolyArray;
static   CPolyArray        polyarray;
static   int               polycnt;

static   CVB99LayerArray   vbArr;
static   int               maxArr = 0;

static   VB99PadstackArray padstackarray; 
static   int               padstackcnt;

static   VB99RoutesArray   routesarray;   
static   int               routescnt;

static   PadRotArray       padrotarray;
static   int               padrotcnt;

static   double            globalClear;
static   double            holePosTol = 0.0;
static   double            holeNegTol = 0.0;

static   writtenHKP        currentFile;
static   std::set<int>     testPointGeomBlockSet;

static CMgcEncryptStreamOutput exportFileStream;

/* Function Prototypes ***************************************************************************/

static int load_VB99settings(const char *fname);
static void do_layerlist();
static int edit_layerlist();
static VB99Padstack* AddPadstackToArrary(BlockStruct *block, bool smd);
static int CreatePadstackArrayFromBlockArray();
static int CreatePadstackArrayFromBlock(BlockStruct *block);
static int CheckPadshapeOfThruPadstack(BlockStruct *block);
static int update_vianames();
static int do_padstack(FileStruct *file, const char *fname);
static int do_jobpreference(FileStruct *file, const char *fname);
static int check_pcbcomponents(FileStruct *file);
static int do_geometries(FileStruct *file, const char *fname);
static int do_netclass(FileStruct *file, const char *fname);
static int do_netprops(FileStruct *file, const char *fname);
static int do_layout(FileStruct *file, const char *fname, double accuracy); 
static int do_keyin_netlist(FileStruct *file, const char *fname);
static int do_devices(FileStruct *file, const char *fname);
static int check_devicefiles();
static void free_layerlist();
static int get_classtype(const char *n);
static CString LayerNameFromStacknum(int stackNum);

static int VB99_CountPinData(CDataList *DataList);
static int VB99_WritePackagePinData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, const char *cellname);
static int VB99_WriteCellData(FILE *wfp,CellTypeTag cellType,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, int layerclass);
static int VB99_WriteLayoutData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, int layerclass);
static int VB99_IsComplexPadData(CDataList *DataList);
static int VB99_ComplexPadData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale);
static int VB99_WriteGeomOutlineData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, const char *layername, int grclass, double compheight, bool isCell);
static void WriteCellBlock(BlockStruct *block, FILE *fp);
static void init_padform(VB99Padforms *padform);
static int VB99_WritePADSTACKData(FILE *wfp, const char *padstkname, CDataList *DataList, double rotation, int mirror, double scale, 
		int embeddedLevel, int insertLayer, int smdpad, VB99Padforms *padform);
static int write_padform(FILE *fp, VB99Padforms padform, int smd,  CString padstackType);
static int write_padstack_info(FILE *fp, double scale, bool smd, CString padstackName, CString padstackType, VB99Padstack *padstack);

static void NameAllInsertedApertures(BlockStruct *block);
static void NameAllInsertedApertures(FileStruct *pcbfile);

static bool IsShapeGraphic(bool circle, bool closed, bool filled);
static bool hasNetNameAttrib(CCEtoODBDoc *doc, DataStruct *data);
static bool isDataOnElectricalLayer(CCEtoODBDoc *doc, DataStruct *data);
static void fillTestPointGeomBlockSet( CNetList* NetList );

/******************************************************************************
* dts0100520422 - We need to write encrypted HKP if .ENCRYPT_OUTPUT Y/y
*/
int fprintfHKP(FILE *fp, LPCTSTR lpszFormat, ...)
{
   CString cStr;

   va_list argList;
   va_start(argList, lpszFormat);
   cStr.FormatV(lpszFormat, argList);

   if (encryptOutputFlag)
   {
      if (exportFileStream.IsOpen()) exportFileStream << cStr.GetBuffer();
      return cStr.GetLength();
   }
   else
   {
      return fprintf(fp, cStr.GetBuffer());
   }
}

/******************************************************************************
* VB99_WriteFiles
*/
void VB99_WriteFiles(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *format, int pageUnits, double xmin, double ymin,
                    double xmax, double ymax, const char *checkFile, const char *settingsFile)
{
   display_error = FALSE;
   drillsymbolcnt = 0;
   ident = ""; 
   identcnt = 0;
   max_signalcnt = 0;
   doc = Doc;
   page_units = pageUnits;

	CString logFile = GetLogfilePath("vbascii.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   routesarray.SetSize(100, 100);
   routescnt = 0;

   complistarray.SetSize(100, 100);
   complistcnt = 0;

   celllistarray.SetSize(100, 100);
   celllistcnt = 0;

   padrotarray.SetSize(10, 10);
   padrotcnt = 0;

   //switch (format->PortFileUnits)
   switch (page_units)
   {
   case UNIT_MM:  // mm
      // for mm it should be 5
      output_units_accuracy = 5;
      output_units_string = "MM";   
      unitsFactor = Units_Factor(page_units, UNIT_MM);
      one_mil = 1.0 * Units_Factor(UNIT_MILS, UNIT_MM);
      break;
   case UNIT_INCHES: // inch
      // for inch it should be 7
      output_units_accuracy = 7;
      output_units_string = "IN";   // for part file
      unitsFactor = Units_Factor(page_units, UNIT_INCHES);
      one_mil = 1.0 * Units_Factor(UNIT_MILS, UNIT_INCHES);
      break;
   case UNIT_MILS:   // mils
      output_units_accuracy = 4;
      output_units_string = "TH";   // for part file
      unitsFactor = Units_Factor(page_units, UNIT_MILS);
      one_mil = 1.0 * Units_Factor(UNIT_MILS, UNIT_MILS);
      break;
   default:
      CString  tmp;
      tmp.Format("VB ASCII UNITS must be IN, MM, TH");
      MessageBox(NULL, tmp, "Error Units", MB_OK | MB_ICONHAND);
      return;
      break;
   }

   load_VB99settings(settingsFile);
   doc->purgeUnusedWidthsAndBlocks( false /* no report */ );
   doc->OnGenerateSmdComponentVisibleFilesOnly();

   vb99_smallnumber = 1;
	int i=0;
   for (i=0; i<output_units_accuracy; i++)
      vb99_smallnumber /= 10;

   check_init(checkFile);

   mils_2_units = Units_Factor(UNIT_MILS, output_units);

   double accuracy = get_accuracy(doc);

   CString CELL = pathname;
   CELL += "cell.hkp";

   CString JOBPREF = pathname;
   JOBPREF += "jobprefs.hkp";

   CString LAYOUT = pathname;
   LAYOUT += "layout.hkp";

   CString NETCLASS = pathname;
   NETCLASS += "netclass.hkp";

   CString NETPROPS = pathname;
   NETPROPS += "netprops.hkp";

   CString PADSTACK = pathname;
   PADSTACK += "padstack.hkp";

   CString KEYIN_NETLIST = pathname;
   KEYIN_NETLIST += "netlist.kyn";

   CString PDB = pathname;
   PDB += "pdb.hkp";

   currentFile = noFile;

   polyarray.SetSize(100, 100);
   polycnt = 0;

   vbArr.SetSize(100, 100);
   maxArr = 0;

   padstackarray.SetSize(100, 100);
   padstackcnt = 0;

   do_layerlist();

   int pcb_found = 0;

   CCamCadDatabase camCadDatabase(*Doc);
   convertSmdViasToTestPoints(camCadDatabase);
	doc->GenerateBlockDesignSurfaceInfo(true);

   // here loop for only one PCB file
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())
         continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
         pcb_found++;
   }

   if (pcb_found > 1)
      MessageBox(NULL, "More than one PCB file found !", "VB ASCII Output", MB_OK | MB_ICONHAND);

   if (edit_layerlist())
   {
      CWaitCursor w;

      if (format->Output)  // 0 = PCB, 1 = GRAPHIC
      {
         // not implemented
      }
      else
      {
         // PCB translation
			CreatePadstackArrayFromBlockArray();
         pcb_found = 0;
         POSITION filePos = doc->getFileList().GetHeadPosition();
         while (filePos)
         {
            file = doc->getFileList().GetNext(filePos);
            if (!file->isShown())
               continue;

            if (file->getBlockType() == BLOCKTYPE_PCB)  
            {
               pcb_found++;

               // only do I pcb file
               if (pcb_found != 1)
                  break;

               NameAllInsertedApertures(file);

               CCamCadDocRepairTool changeSameNames( *doc );
               changeSameNames.verifyGeometryNames();
               fillTestPointGeomBlockSet( &file->getNetList() );

					update_vianames();

               currentFile = padstackFile;
               // padstacks need to be done before job preferences, because it collects all
               // padstacks, from which the vias are needed in job preferences.
               do_padstack(file, PADSTACK);

               currentFile = jobFile;
               do_jobpreference(file, JOBPREF);
               
               check_pcbcomponents(file);

               // cell
               currentFile = cellFile;
               do_geometries(file, CELL);

               // netclass
               currentFile = netclassFile;
               do_netclass(file, NETCLASS);

               // netprop
               currentFile = netpropsFile;
               do_netprops(file, NETPROPS);

               // layout
               currentFile = layoutFile;
               do_layout(file, LAYOUT, accuracy);

               // keyin netlist
               // We do not need to encrypt the KEYIN_NETLIST file
               BOOL oldEncryptFlag = encryptOutputFlag;
               encryptOutputFlag = FALSE;
               currentFile = kyinnetlistFile;
               do_keyin_netlist(file, KEYIN_NETLIST);
               encryptOutputFlag = oldEncryptFlag;

               // pdb - this needs to be after do_layout.
               currentFile = pdbFile;
               do_devices(file, PDB );

               check_devicefiles();

               changeSameNames.undoVerifyGeometryNames();
               testPointGeomBlockSet.clear();
            }
         }
   
         if (!pcb_found)
            MessageBox(NULL, "No PCB file found !","VB ASCII Output", MB_OK | MB_ICONHAND);
      }
   }

   free_layerlist();

   if (check_report(flog))
      display_error++;
   check_deinit();

   fclose(flog);

   polyarray.RemoveAll();

   for (i=0; i<padstackcnt; i++)
      delete padstackarray[i];
   padstackarray.RemoveAll();
   padstackcnt = 0;

   for (i=0; i<padrotcnt; i++)
      delete padrotarray[i];
   padrotarray.RemoveAll();
   padrotcnt = 0;

   for (i=0; i<routescnt; i++)
      delete routesarray[i];
   routesarray.RemoveAll();
   routescnt = 0;

   for (i=0; i<complistcnt; i++)
      delete complistarray[i];
   complistarray.RemoveAll();

   for (i=0; i<celllistcnt; i++)
      delete celllistarray[i];
   celllistarray.RemoveAll();

   if (display_error)
      Logreader(logFile);

   return;
}

/******************************************************************************
*/

static void NameAllInsertedApertures(BlockStruct *block)
{
   if (block != NULL)
   {
      if (block->isAperture() && block->getName().IsEmpty())
      {
         CString name;
         name.Format("AP%06d", block->getBlockNumber());
         block->setName(name);
      }

      POSITION pos = block->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *d = block->getNextDataInsert(pos);
         if (d != NULL && d->getDataType() == dataTypeInsert)
         {
            BlockStruct *insblk = doc->getBlockAt( d->getInsert()->getBlockNumber() );
            NameAllInsertedApertures(insblk);
         }
      }
   }
}

static void NameAllInsertedApertures(FileStruct *pcbfile)
{
   if (pcbfile != NULL)
      NameAllInsertedApertures(pcbfile->getBlock());
}

/******************************************************************************
* cnv_rot
   make sure a rotation is between 0..359.9999
*/
static double cnv_rot(double rot)
{
   rot = round(rot * 10) / 10; // round to 1/10
   while (rot < 0)
      rot += 360;
   while (rot >= 360)
      rot -= 360;

   return rot;
}

/******************************************************************************
* vb99_plusident
*/
static int vb99_plusident()
{
   identcnt++;
   ident = "";
	int i=0;

   for (i=1; i<identcnt; i++)
      ident += " ";

   for (i=0;i<identcnt;i++)
      ident += ".";

   return 1;
}

/******************************************************************************
* vb99_minusident
*/
static int vb99_minusident()
{
   if (identcnt > 0)
   {
      identcnt--;
      ident = "";
      if (identcnt > 0)
      {
			int i=0;
         for (i=1; i<identcnt; i++)
            ident += " ";

         for (i=0; i<identcnt; i++)
            ident += ".";
      }
   }
   else
   {
      ErrorMessage("Error in minus ident\n");
      return -1;
   }
   return 1;
}

/******************************************************************************
* vb99_xy
*/
static int vb99_xy(FILE *fp, double x, double y)
{
   fprintfHKP(fp, "%sXY %1.*lf, %1.*lf\n", ident, output_units_accuracy, x, output_units_accuracy, y);
   return 1;
}

/******************************************************************************
* is_layouttext_layer
*/
static int is_layouttext_layer(const char *l)
{
   if (!STRNICMP(l, "LYR_", 4))
      return TRUE;
   if (!STRCMPI(l, "ASSEMBLY_TOP"))
      return TRUE;
   if (!STRCMPI(l, "ASSEMBLY_BOTTOM"))
      return TRUE;
   if (!STRCMPI(l, "SILKSCREEN_TOP"))
      return TRUE;
   if (!STRCMPI(l, "SILKSCREEN_BOTTOM"))
      return TRUE;
   if (!STRCMPI(l, "SILKSCREEN_MNT_SIDE"))
      return TRUE;

   return FALSE;
}

/******************************************************************************
* is_celltext_layer
*/
static int is_celltext_layer(const char *l)
{
   // MNT_LYR, OPP_LYR, ALL_LYRS, REL_LYR_n for stackup
   if (!STRCMPI(l, "ALL_LYRS"))
      return TRUE;
   if (!STRCMPI(l, "MNT_LYR"))
      return TRUE;
   if (!STRCMPI(l, "OPP_LYR"))
      return TRUE;
   if (!STRNICMP(l, "REL_LYR", 4))
      return TRUE;
   //if (!STRNICMP(l, "LYR_", 4))
   //   return TRUE;
   if (!STRCMPI(l, "ASSEMBLY"))
      return TRUE;
   if (!STRCMPI(l, "SILKSCREEN_MNT_SIDE"))
      return TRUE;
   if (!STRCMPI(l, "SILKSCREEN_OPP_SIDE"))
      return TRUE;
   if( currentFile == layoutFile )
   {
      if (!STRCMPI(l, "SILKSCREEN_TOP"))
         return TRUE;
      if (!STRCMPI(l, "SILKSCREEN_BOTTOM"))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* boardLayerNameToCellLayerName
*/
CString boardLayerNameToCellLayerName(const CString& boardLayerName)
{
   CString cellLayerName(boardLayerName);

   if (!boardLayerName.CompareNoCase("SILKSCREEN_TOP") || !boardLayerName.CompareNoCase("SILKSCREEN"))
   {
      if( currentFile == layoutFile )
      {
         cellLayerName = "SILKSCREEN_TOP";
      }
      else
      {
         cellLayerName = "SILKSCREEN_MNT_SIDE";
      }
   }
   else if (boardLayerName.CompareNoCase("SILKSCREEN_BOTTOM") == 0)
   {
      if( currentFile == layoutFile )
      {
         cellLayerName = "SILKSCREEN_BOTTOM";
      }
      else
      {
         cellLayerName = "SILKSCREEN_OPP_SIDE";
      }
   }
   else if (!boardLayerName.CompareNoCase("ASSEMBLY_TOP") || !boardLayerName.CompareNoCase("ASSEMBLY_BOTTOM"))
   {
      cellLayerName = "ASSEMBLY";
   }
   else
   {
      for (int ind=0; ind<vbArr.GetSize(); ind++)
      {
         VB99LayerStruct* vbLayer = vbArr.GetAt(ind);

         if (vbLayer != NULL && !vbLayer->newName.CompareNoCase(boardLayerName))
         {
            switch (vbLayer->stackNum)
            {
            case LAY_ALL:
					cellLayerName = "ALL_LYRS";
					break;
            case LAY_TOP:
					cellLayerName = "MNT_SIDE";
					break;
            case LAY_BOT:
					cellLayerName = "OPP_SIDE";
					break;
            case LAY_INNER:  
               if (vbLayer->layer != NULL)
                  cellLayerName.Format("REL_LYR_%d", vbLayer->layer->getElectricalStackNumber() - 1);
               break;
            }

            break;
         }
      }
   }

   return cellLayerName;
}

/******************************************************************************
* update_cellwritten
*/
static int update_cellwritten(const char *c)
{
   for (int i=0; i<celllistcnt; i++)
   {
      if (celllistarray[i]->cellname.Compare(c) == 0)
         return i;
   }
   
   VB99Cell *cell = new VB99Cell;
   celllistarray.SetAtGrow(celllistcnt++, cell);  
   cell->cellname = c;
   
   return celllistcnt -1;
}

/******************************************************************************
* is_cellwritten
*/
static int is_cellwritten(const char *c)
{
   for (int i=0; i<celllistcnt; i++)
   {
      if (celllistarray[i]->cellname.Compare(c) == 0)
         return i;
   }
   
   return -1;
}

/******************************************************************************
* CString   clean_prosa
   take care of <">
*/
static CString clean_prosa(const char *t)
{
   CString w;
   for (int i=0; i<(int)strlen(t); i++)
   {
      if (t[i] == '"')  
         w += '\\';
      else if (t[i] == '\\')  
         w += '\\';

      w += t[i];
   }

   return w;
}

/******************************************************************************
* vb99_texttype
   this is for LAYOUT
*/
static int vb99_singleLineTextType(FILE *fp, int display, const char *texttype, const CString textPropertyName, const char *textValue, double x, double y,
                         double rotation, int mirror, double height, double width, const char *layer, double penwidth)
{
   if (textPropertyName.IsEmpty())
   {
      fprintfHKP(fp, "%sTEXT \"%s\"\n", ident, clean_prosa(textValue));
   }
   else
   {
      if (strlen(textValue) == 0)
         fprintfHKP(fp, "%sTEXT \"%s\" \"TRUE\"\n", ident, textPropertyName);
      else
         fprintfHKP(fp, "%sTEXT \"%s\" \"%s\"\n", ident, textPropertyName, clean_prosa(textValue));
   }

   vb99_plusident();
   fprintfHKP(fp, "%sTEXT_TYPE %s\n", ident, texttype);

   if (display)
   {
      fprintfHKP(fp, "%sDISPLAY_ATTR\n", ident);
      vb99_plusident();

      fprintfHKP(fp, "%sFONT \"%s\"\n", ident, default_fontsize);
      fprintfHKP(fp, "%sHEIGHT %1.*lf\n", ident, output_units_accuracy, height);
      fprintfHKP(fp, "%sHORZ_JUST LEFT\n", ident);
      fprintfHKP(fp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(rotation)));
      fprintfHKP(fp, "%sSTROKE_WIDTH %1.*lf\n", ident, output_units_accuracy, penwidth); 

      if (is_layouttext_layer(layer))
         fprintfHKP(fp, "%sTEXT_LYR %s\n", ident, layer);
      else
         fprintfHKP(fp, "%sUSER_LYR \"%s\"\n", ident, layer);

      CString options = "";
      if (mirror)
         options = " MIRRORED";
      else
         options = "NONE";
      fprintfHKP(fp, "%sTEXT_OPTIONS %s\n", ident, options);

      fprintfHKP(fp, "%sVERT_JUST BOTTOM\n", ident);
      fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, strlen(textValue)*width);
      vb99_xy(fp, x, y);
      vb99_minusident();
   } 
   vb99_minusident();

   return 1;
}

/******************************************************************************
* vb99_texttype
*/
static int vb99_texttype(FILE *fp, int display, const char *textType, const CString textPropertyName, const char *textValue, double x, double y,
                         double rotation,int mirror, double height, double width, const char *layer, double penWidth)
{
   int retval = 0;
   CSupString multiLineString(textValue);

   if (multiLineString.Find("\n") < 0)
   {
      retval = vb99_singleLineTextType(fp,display,textType,textPropertyName,textValue,x,y,rotation,mirror,height,width,layer,penWidth);
   }
   else
   {
      CStringArray lines;
      int numLines = multiLineString.Parse(lines,"\n", false);

      CTMatrix matrix;
      matrix.translate(-x, -y);
      matrix.rotateRadians(rotation);
      matrix.scale(mirror ? -1. : 1.,1.);
      matrix.translate(x, y);

      double textLeading = height * 1.25;

      y += textLeading * numLines;

      for (int lineIndex=0; lineIndex<numLines; lineIndex++)
      {
         y -= textLeading;
         CPoint2d origin(x,y);
         matrix.transform(origin);

         retval = vb99_singleLineTextType(fp,display,textType,textPropertyName,lines.GetAt(lineIndex),origin.x,origin.y,rotation,mirror,height,width,layer,penWidth);
      }      
   }

   return retval;
}

/******************************************************************************
* vb99_celltexttype
*/
static int vb99_singleLineCellTextType(FILE *fp, int display, const char *texttype, const CString textPropertyName, const char *textValue, double x, double y,
                             double rotation, double height, double width, const char *layer, double penwidth)
{
   if (textPropertyName.IsEmpty())
   {
      fprintfHKP(fp, "%sTEXT \"%s\"\n", ident, clean_prosa(textValue));
   }   
   else
   {
      if (strlen(textValue) == 0)
         fprintfHKP(fp, "%sTEXT \"%s\" \"TRUE\"\n", ident, textPropertyName);
      else
         fprintfHKP(fp, "%sTEXT \"%s\" \"%s\"\n", ident, textPropertyName, clean_prosa(textValue));
   }

   vb99_plusident();
   fprintfHKP(fp, "%sTEXT_TYPE %s\n", ident, texttype);

   if (display)
   {
      fprintfHKP(fp, "%sDISPLAY_ATTR\n", ident);
      vb99_plusident();

      fprintfHKP(fp, "%sFONT \"%s\"\n", ident, default_fontsize);
      fprintfHKP(fp, "%sHEIGHT %1.*lf\n", ident, output_units_accuracy, height);
      fprintfHKP(fp, "%sHORZ_JUST LEFT\n", ident);
      fprintfHKP(fp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(rotation)));
      fprintfHKP(fp, "%sSTROKE_WIDTH %1.*lf\n", ident, output_units_accuracy, penwidth); 

      CString layerName = boardLayerNameToCellLayerName(layer);

      if (is_celltext_layer(layerName))
         fprintfHKP(fp, "%sTEXT_LYR %s\n", ident, (const char*)layerName);
      else
         fprintfHKP(fp, "%sUSER_LYR \"%s\"\n", ident, (const char*)layerName);

      fprintfHKP(fp, "%sTEXT_OPTIONS NONE\n", ident);
      fprintfHKP(fp, "%sVERT_JUST BOTTOM\n", ident);
      fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, strlen(textValue)*width);
      vb99_xy(fp, x, y);
      vb99_minusident();
   }
   vb99_minusident();

   return 1;
}

/******************************************************************************
* vb99_celltexttype
*/
static int vb99_celltexttype(FILE *fp, int display, const char *textType, const CString textPropertyName, const char *textValue, double x, double y,
                             double rotation, double height, double width, const char *layer, double penWidth)
{
   int retval = 0;
   CSupString multiLineString(textValue);

   if (multiLineString.Find("\n") < 0)
   {
      retval = vb99_singleLineCellTextType(fp,display,textType,textPropertyName,textValue,x,y,rotation,height,width,layer,penWidth);
   }
   else
   {
      CStringArray lines;
      int numLines = multiLineString.Parse(lines, "\n", false);

      CTMatrix matrix;
      matrix.translate(-x,-y);
      matrix.rotateRadians(rotation);
      matrix.translate( x, y);

      double textLeading = height * 1.25;
      y += textLeading * numLines;

      for (int lineIndex=0; lineIndex<numLines; lineIndex++)
      {
         y -= textLeading;
         CPoint2d origin(x,y);
         matrix.transform(origin);

         retval = vb99_singleLineCellTextType(fp, display, textType, textPropertyName, lines.GetAt(lineIndex),
            origin.x, origin.y, rotation, height, width, layer, penWidth);
      }      
   }

   return retval;
}

/******************************************************************************
* int get_padstack_index
*/
static int  get_padstack_index(const char *p)
{
   for (int i=0; i<padstackcnt; i++)
   {
      if (padstackarray[i]->padname.Compare(p) == 0)
         return i;
   }

   return -1;
}

/******************************************************************************
* created_Date
*/
static void created_Date( FILE *file)
{
   char *months[] = { "January", "February", "March", "April", "May", "June", "July", 
                      "August", "September", "October", "November", "December" };
   char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

   CTime time = CTime::GetCurrentTime();
   int hour = time.GetHour();

   fprintfHKP(file, ".DATE \"%s, %s %d, %d %d:%02d %s\"\n",
         days[time.GetDayOfWeek()-1],
         months[time.GetMonth()-1], 
         time.GetDay(),
         time.GetYear(),
         (hour>12 ? hour-12 : hour),
         time.GetMinute(),
         (hour>11 ? "PM" : "AM"));

   return;
}

/******************************************************************************
* created_by
*/
static int created_by(FILE *fp, const char *ftype)
{
   fprintfHKP(fp, ".FILETYPE %s\n", ftype);
   fprintfHKP(fp, ".VERSION \"VB99.0\"\n");
   fprintfHKP(fp, ".CREATOR \"%s %s\"\n", getApp().getCompanyNameString(), getApp().getCamCadTitle());

   created_Date(fp);
   fprintfHKP(fp,"\n");

   fprintfHKP(fp, ".UNITS %s\n", output_units_string);
   fprintfHKP(fp,"\n");
   identcnt = 0;
   vb99_plusident();

   return 1;
}

/******************************************************************************
* static int   pdb_created_by
*/
static int pdb_created_by(FILE *fp, const char *ftype)
{  

   fprintfHKP(fp, ".FILETYPE %s\n", ftype);
   fprintfHKP(fp, ".VERSION \"1\"\n");
   fprintfHKP(fp, ".UNITS %s\n", output_units_string);
   fprintfHKP(fp, ".Notation si\n");
   fprintfHKP(fp, "\n");

   fprintfHKP(fp, "\n");

   identcnt = 0;
   vb99_plusident();

   return 1;
}


/****************************************************************************/
/*
   return NULL if not visible
*/
static const char *get_drillsymbol(double s)
{
   static char ds[10];
   int m1,m2;

   strcpy(ds, "A");

   for (int i=0; i<255; i++)
   {
      if (drillsymbolarray[i] == s)
      {
         m1 = i % ('Z'-'A');        
         m2 = i / ('Z'-'A');

         ds[0] = 'A' + m2;
         ds[1] = '\0';
         if (m2 > 0)
         {
            ds[1] = 'A' + m2;
            ds[2] = '\0';
         }
         return ds;
      }
   }

   if (drillsymbolcnt < 255)
   {
      drillsymbolarray[drillsymbolcnt] = s;
      drillsymbolcnt++;
   }
   else
   {
      fprintf(flog, "Too many drill symbols\n");
      display_error++;     
   }

   m1 = drillsymbolcnt % ('Z'-'A');       
   m2 = drillsymbolcnt / ('Z'-'A');

   ds[0] = 'A' + m2;
   ds[1] = '\0';
   if (m2 > 0)
   {
      ds[1] = 'A' + m2;
      ds[2] = '\0';
   }

   return ds;
}

/******************************************************************************
* char *Layer_VB99
   return NULL if not visible
*/
static const char *Layer_VB99(int l)
{
   for (int i=0; i<maxArr; i++)
   {
      if (vbArr[i]->on == 0)
         continue;   // not visible
      if (vbArr[i]->layerindex == l)
         return vbArr[i]->newName.GetBuffer(0);
   }

/*#ifdef _DEBUG
   if (l > -1)
      fprintf(flog, "DEBUG: Layer [%s] not translated.\n", doc->LayerArray[l]->getName());
#endif*/

   return NULL;
}

//******************************************************************************

VB99RouteType DeterminePolyRouteType(CCEtoODBDoc *doc, DataStruct *data, bool& closed, bool& filled, bool voided )
{
   // Determine route type and swizzle filled to match

   bool hasNetName( hasNetNameAttrib(doc, data) );
   bool isNotEtchGraphic( data->getGraphicClass() != graphicClassEtch );
   bool isOnElectricalLayer( isDataOnElectricalLayer(doc, data) );

   if( (isOnElectricalLayer && isNotEtchGraphic && !hasNetName)   //data on electrical layers not of Etch type and without net name
         || ( ( hasNetName || !isNotEtchGraphic ) && voided )     //void data that have netname or are etch type
         || ( isOnElectricalLayer && voided ) )                   //void data on electrical layers
   {
      if (closed)
         filled = true;

      return vb99RouteTypeObstruct;
   }
   else if (closed)
   {
      filled = true;

      return vb99RouteTypeConductiveArea;
   }
   else
   {
      // Leave filled as-is

      return vb99RouteTypeTrace;
   }

   return vb99RouteTypeUnknown;
}

//******************************************************************************

static bool IsShapeGraphic(bool circle, bool closed, bool filled)
{
   // Return true is set of params indicates output should be a SHAPE,
   // i.e. CIRCLE_SHAPE, POLYLINE_SHAPE or POLYARC_SHAPE.
   // Return false for xxxx_PATH graphic.

   return ( (circle || closed) && filled );
}

/******************************************************************************
* int add_polyline

   Add polyline must be changed to include voids

   if (circle)
      p->x == cx
      p->y == cy
      p->bulge == radius

 ..PLACEMENT_OUTLINE
  ...HEIGHT 280
  ...UNDERSIDE_SPACE 0
  ...CIRCLE_SHAPE
   ....XY (0, 0)
   ....RADIUS 51.4781102362
   ....SHAPE_OPTIONS NOT_FILLED

*/
static int  add_polyline(FILE *fp, Point2 *points, int cnt, bool polyFilled, bool closed,
                         bool voidout, double lineWidth, bool circle)
{
   if (circle)
   {
      if (IsShapeGraphic(circle, closed, polyFilled))
      {
         fprintfHKP(fp, "%sCIRCLE_SHAPE\n", ident);
         vb99_plusident();

         fprintfHKP(fp, "%sXY (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, points[0].x, output_units_accuracy, points[0].y);
         fprintfHKP(fp, "%sRADIUS %1.*lf\n", ident, output_units_accuracy, points[0].bulge + lineWidth);
   
         if (!polyFilled)
            fprintfHKP(fp, "%sSHAPE_OPTIONS NOT_FILLED\n", ident);
         else
            fprintfHKP(fp, "%sSHAPE_OPTIONS FILLED\n", ident);

         vb99_minusident();
      }
      else
      {
         fprintfHKP(fp, "%sCIRCLE_PATH\n", ident);
         vb99_plusident();

         fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, lineWidth);
         fprintfHKP(fp, "%sXY (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, points[0].x, output_units_accuracy, points[0].y);
         fprintfHKP(fp, "%sRADIUS %1.*lf\n", ident, output_units_accuracy, points[0].bulge+lineWidth);
         vb99_minusident();
      }
      return 1; 
   }

   if (cnt < 2)
      return 1;

   int arccnt = 0;
   for (int i=0;i<cnt;i++)
   {
      if (fabs(points[i].bulge) > BULGE_THRESHOLD)
         arccnt++;

      // this should prevent the -0.000 output, which seems
      // to cause funny problems in veribest
      if (fabs(points[i].x) < vb99_smallnumber)
         points[i].x = 0;
      if (fabs(points[i].y) < vb99_smallnumber)
         points[i].y = 0;
   }

   // this fill function does not seem to work correctly, so if non_filled make a path !
   if (IsShapeGraphic(circle, closed, polyFilled))
   {
      if (arccnt)
         fprintfHKP(fp, "%sPOLYARC_SHAPE\n", ident);
      else
         fprintfHKP(fp, "%sPOLYLINE_SHAPE\n", ident);

      vb99_plusident();

      if (polyFilled)
         fprintfHKP(fp, "%sSHAPE_OPTIONS %s\n", ident, "FILLED");
      else
         fprintfHKP(fp, "%sSHAPE_OPTIONS %s\n", ident, "NOT_FILLED");
   }
   else
   {
      if (arccnt)
         fprintfHKP(fp, "%sPOLYARC_PATH\n", ident);
      else
         fprintfHKP(fp, "%sPOLYLINE_PATH\n", ident);

      vb99_plusident();
      fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, lineWidth);
   }

   double radius = 0;
   if (arccnt)
   {
      for (int i=0; i<cnt; i++)
      {
         if (i==0)
            fprintfHKP(fp, "%sXYR (%1.*lf, %1.*lf, %1.*lf)\n", ident, output_units_accuracy, points[i].x,
                  output_units_accuracy, points[i].y, output_units_accuracy,  radius);
         else
            fprintfHKP(fp, "%*s   (%1.*lf, %1.*lf, %1.*lf)\n", identcnt*2," ",  output_units_accuracy, points[i].x,
                  output_units_accuracy, points[i].y, output_units_accuracy, radius);

         double da = atan(points[i].bulge) * 4;

         if (fabs(da) > BULGE_THRESHOLD)
         {
            double cx, cy, r, sa;
            ArcPoint2Angle(points[i].x,points[i].y,points[i+1].x, points[i+1].y ,da, &cx, &cy, &r, &sa);

            if (da < 0) // counter clockwise / ccw
               radius = r;
            else
               radius = -r;

            if (fabs(cx) < vb99_smallnumber)
               cx = 0;
            if (fabs(cy) < vb99_smallnumber)
               cy = 0;

/*          fprintfHKP(fp, "%*s   (%1.*lf, %1.*lf, %1.*lf)\n", identcnt*2, " ", output_units_accuracy, cx,
                  output_units_accuracy, cy, output_units_accuracy, radius);*/
            CString Buf = "";
            Buf.Format("%1.*lf", output_units_accuracy, radius);
            fprintfHKP(fp, "%*s   (%1.*lf, %1.*lf, %s)\n", identcnt*2, " ", output_units_accuracy, cx,
                  output_units_accuracy, cy, Buf);

         }

         radius = 0;
      }
   } // arccnt
   else
   {
      for (int i=0; i<cnt; i++)  
      {
         if (i==0)
            fprintfHKP(fp, "%sXY (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, points[i].x,
                  output_units_accuracy, points[i].y);
         else
         {
            if (fabs(points[i].x - points[i-1].x) > vb99_smallnumber || 
               fabs(points[i].y - points[i-1].y) > vb99_smallnumber) 
            {
               fprintfHKP(fp, "%*s  (%1.*lf, %1.*lf)\n", identcnt*2," ", output_units_accuracy, points[i].x,
                     output_units_accuracy, points[i].y);
            }
         }
      }
   }

   vb99_minusident();
   return 1; 
}

/******************************************************************************
* get_classtype
*/
static int get_classtype(const char *n)
{
   for (int i=0; i<CLASS_MAX_CLASSES; i++)
   {
      if (!STRCMPI(n, lclasses[i]))
         return i;
   }

   fprintf(flog, "Unknown Class [%s]\n",n);
   display_error++;

   return CLASS_UNKNOWN;
}

/******************************************************************************
* load_VB99settings
*/
static int load_VB99settings(const char *fname)
{
   FILE *fp;
   char line[255];
   char *lp;

   //output_units_string   = "TH";
   //output_units = UNIT_MILS;
   //output_units_accuracy = 3;

   default_fontsize = "VeriBest Gerber 0";
   pinNetInCell = FALSE;
   encryptOutputFlag = FALSE;

   holePosTol = holeNegTol = 0.0;

   globalClear = 0.010;
   if (page_units != UNIT_INCHES)
      globalClear = globalClear * Units_Factor(UNIT_INCHES, page_units);

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      MessageBox(NULL, tmp, " VB99 Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".PIN_NET_IN_CELL"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               pinNetInCell = TRUE;
         }
         else if (!STRCMPI(lp,".ENCRYPT_OUTPUT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               encryptOutputFlag = TRUE;
         }
         else if (!STRCMPI(lp,".HOLE_TOLERANCE"))
         {
            // Units
            double unitCnvFactor;
            if ((lp = strtok(NULL," \t\n")) != NULL)
            {
               if (!STRCMPI(lp,"I"))
                  unitCnvFactor = Units_Factor(UNIT_INCHES, page_units);
               else if (!STRCMPI(lp,"T"))
                  unitCnvFactor = Units_Factor(UNIT_MILS, page_units);
               else if (!STRCMPI(lp,"M"))
                  unitCnvFactor = Units_Factor(UNIT_MM, page_units);
            }
            else
               continue;

            // First value is required
            if ((lp = strtok(NULL," \t\n")) != NULL)
               holePosTol = atof(lp);
            else
               continue;

            // Second value is optional, re-use first value if no second value
            if ((lp = strtok(NULL," \t\n")) != NULL)
               holeNegTol = atof(lp);
            else
               holeNegTol = holePosTol;
            
            holePosTol = holePosTol * unitCnvFactor;
            holeNegTol = holeNegTol * unitCnvFactor;
         }         
         else if (!STRCMPI(lp,".GLOBAL_CLEAR"))
         {
            double unitCnvFactor;
            if ((lp = strtok(NULL," \t\n")) != NULL)
            {
               if (!STRCMPI(lp,"I"))
                  unitCnvFactor = Units_Factor(UNIT_INCHES, page_units);
               else if (!STRCMPI(lp,"T"))
                  unitCnvFactor = Units_Factor(UNIT_MILS, page_units);
               else if (!STRCMPI(lp,"M"))
                  unitCnvFactor = Units_Factor(UNIT_MM, page_units);
            }

            if ((lp = strtok(NULL," \t\n")) != NULL)
               globalClear = atof(lp);
            
            globalClear = globalClear * unitCnvFactor;
         }
      }
   }

   fclose(fp);
   return 1;
}

/******************************************************************************
* get_layernr_from_index
*/
static int get_layernr_from_index(int index)
{
   for (int i=0; i<maxArr; i++)
   {
      if (vbArr[i]->layerindex == index)
         return vbArr[i]->stackNum;
   }

   return -99;
}

/******************************************************************************
* LayerNameFromStacknum
*/
static CString LayerNameFromStacknum(int stackNum)
{
   for (int i=0; i<maxArr; i++)
   {
      VB99LayerStruct *vbLayer = vbArr[i];
      if (vbLayer->stackNum == stackNum && (vbLayer->type == 'S' || vbLayer->type == 'P'))
         return vbLayer->newName;
   }

   return "";
}

/******************************************************************************
* get_layerptr_from_index
*/
static int get_layerptr_from_index(int index)
{
   for (int i=0; i<maxArr; i++)
   {
      if (vbArr[i]->layerindex == index)
         return i;
   }

   return -1;
}

/******************************************************************************
* planelayer_has_netname
*/
static int planelayer_has_netname(LayerStruct *layer)
{
   Attrib *a =  is_attvalue(doc, layer->getAttributesRef(), LAYATT_NETNAME, 2);
   if (a)
   {
      CString l = get_attvalue_string(doc, a);  // can have multiple args
      if (strlen(l))
         return TRUE;
   }
   return FALSE;
}

/******************************************************************************
* do_layerlist
*/
static void do_layerlist()
{
   topsignallayer = "LYR_1";
   bottomsignallayer = "LYR_2";
   maxArr = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not the next index.
   {
      LayerStruct *layer = doc->getLayerArray()[j];

      if (layer == NULL)
         continue; // could have been deleted.

      int signr = 0;
      char typ = 'D';
      CString vblay = "";

      if (layer->getElectricalStackNumber() != 0)
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();

         if (layer->getLayerType() == LAYTYPE_PAD_TOP)
         {
            signr = LAY_TOP;
            vblay = "TOP_PAD";
         }
         else if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
         {
            signr = LAY_BOT;
            vblay = "BOTTOM_PAD";
         }
         else if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
         {
            topsignallayer = vblay = "LYR_1";
         }
         else if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
         {
            vblay.Format("LYR_%d" ,signr);
            bottomsignallayer = vblay;
         }
         else if (layer->getLayerType() == LAYTYPE_POWERNEG)
         {
            if (planelayer_has_netname(layer))
               typ = 'P';
            else
               typ = 'S';
            vblay.Format("LYR_%d", signr);
         }
         else if (layer->getLayerType() == LAYTYPE_POWERPOS)
         {
            if (planelayer_has_netname(layer))
               typ = 'P';
            else
               typ = 'S';
            vblay.Format("LYR_%d", signr);
         }
         else if (layer->getLayerType() == LAYTYPE_SPLITPLANE)
         {
            if (planelayer_has_netname(layer))
               typ = 'P';
            else
               typ = 'S';
            vblay.Format("LYR_%d", signr);
         }
         else if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER || layer->getLayerType() == LAYTYPE_PAD_INNER)
         {
            vblay.Format("LYR_%d", signr);
         }
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
         vblay = "TOP/BOTTOM/INTERNAL_PAD";
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
         vblay = "INTERNAL_PAD";
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         signr = LAY_INNER;
         vblay = "INNER";
      }
      else if (layer->getLayerType() == LAYTYPE_SILK_TOP)
      {
         vblay = "SilkScreen_Top";
      }
      else if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM)
      {
         vblay = "SilkScreen_Bottom";
      }
      else if (layer->getLayerType() == LAYTYPE_PASTE_TOP)
      {
         vblay = "SOLDERPASTE_TOP";
      }
      else if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
      {
         vblay = "SOLDERPASTE_BOTTOM";
      }
      else if (layer->getLayerType() == LAYTYPE_MASK_TOP)
      {
         vblay = "SOLDERMASK_TOP";
      }
      else if (layer->getLayerType() == LAYTYPE_MASK_BOTTOM)
      {
         vblay = "SOLDERMASK_BOTTOM";
      }      
      else if (signr == 0)
      {
         // do not use get_visible, because a mirror_only layer would not get into the list
         if (layer->isVisible() == FALSE)
            continue;
         vblay.Format("USER_LVL_%d", maxArr + 1);
      }

      //vbArr.SetSizes
      VB99LayerStruct *vb = new VB99LayerStruct;
      vb->stackNum = signr;
      vb->layerindex = j;
      vb->layer = layer;
      vb->on = TRUE;
      vb->type = typ;
      vb->oldName = layer->getName();
      vb->newName = vblay;
      vbArr.SetAtGrow(maxArr++, vb);

      if (vb->stackNum > max_signalcnt)
      {
         max_signalcnt = vb->stackNum;
      }
   }

   return;
}

/******************************************************************************
* net_layerlist
*/
static int net_layerlist(FileStruct *file)
{
   int powercnt = 0;

   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      Attrib *a =  is_attvalue(doc, net->getAttributesRef(), ATT_POWERNET, 2);
      if (a == NULL)
         continue;

      VB99LayerStruct *vb = new VB99LayerStruct;
      vbArr.SetAtGrow(maxArr++, vb);
      vb->stackNum = 1;
      vb->layer = NULL;
      vb->on = TRUE;
      vb->type = 'P';
      vb->oldName = net->getNetName();
      vb->newName.Format("POWER_%d", ++powercnt);
   }
   return 1;
}

/******************************************************************************
* edit_layerlist
*/
static int edit_layerlist()
{
   // fill array
   VB99Layer vbdlg;
   vbdlg.arr = &vbArr;
   vbdlg.maxArr = maxArr;

   if (!getApp().SilentRunning)
   {
      if (vbdlg.DoModal() != IDOK) // SilentRunning is already checked.
         return FALSE;
   }

   max_signalcnt = 0;
   for (int i=0; i<maxArr; i++)
   {
      if (vbArr[i]->stackNum > max_signalcnt)
         max_signalcnt = vbArr[i]->stackNum;
   }
   return TRUE;
}

/******************************************************************************
* free_layerlist
*/
static void free_layerlist()
{
   for (int i=0; i<maxArr; i++)
   {
      fprintf(flog, "Layer CAMCAD [%s] Veribest [%s]\n", vbArr[i]->oldName, vbArr[i]->newName);
      display_error++;
      delete vbArr[i];
   }
   vbArr.RemoveAll();

   return;
}

/******************************************************************************
* do_jobpreference
*/
static int do_jobpreference(FileStruct *file, const char *fname)
{
   FILE *fp;
   double scale = file->getScale() * unitsFactor;

   if ((fp = fopen(fname, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fname);
      ErrorMessage(tmp);
      return -1;
   }
   else if (encryptOutputFlag)
   {
      fclose(fp);
      exportFileStream.SetOutputStream(fname);
   }

   created_by(fp, "JOB_PREFERENCES");

   fprintfHKP(fp, "%sPHYSICAL_LAYERS %d\n\n", ident, max_signalcnt);
   fprintfHKP(fp, "%sPADSTACK_TECHNOLOGY \"(Default)\"\n", ident);
   fprintfHKP(fp, "%sDESIGN_UNITS %s\n", ident, output_units_string);
   fprintfHKP(fp, "%sVP_UNITS IN_PER_NS\n", ident);
   fprintfHKP(fp, "\n");

   int viafound = 0;
   int blind_vias = FALSE;

   // write minimum 1 VIA_SPAN section
   fprintfHKP(fp, "%sVIA_SPANS_SECTION\n", ident);
   vb99_plusident();

   if (blind_vias)
      fprintfHKP(fp, "%sALLOW_BLIND_BURIED_VIAS YES\n", ident);
   else
      fprintfHKP(fp, "%sALLOW_BLIND_BURIED_VIAS NO\n", ident);

   for (int i=0; i<padstackcnt; i++)
   {
      if (!padstackarray[i]->used_as_via)
			continue;

      if (padstackarray[i]->startlayernumber == padstackarray[i]->endlayernumber)
      {
         CString tmp;
         tmp.Format("Padstack [%s] is used as a single layer Via. This is not allowed in Veribest!\n", padstackarray[i]->vianame);
         ErrorMessage(tmp, "Single Layer Vias are not allowed!");
      }
      else
      {
         viafound++;
         fprintfHKP(fp, "%sPADSTACK_NAME \"%s\"\n", ident, check_name('p', padstackarray[i]->vianame));
         vb99_plusident();
         fprintfHKP(fp, "%sFROM_LAYER %d\n", ident, padstackarray[i]->startlayernumber);
         vb99_plusident();
         fprintfHKP(fp, "%sTO_LAYER %d\n", ident, padstackarray[i]->endlayernumber);
         vb99_minusident();
         vb99_minusident();
      }
   }
   vb99_minusident();
   
   if (!viafound)
   {
      fprintf(flog, "Minimum 1 via definition needed\n");
      display_error++;
   }
   fprintfHKP(fp, "\n");
   fprintfHKP(fp, "%sLAYER_STACKUP_SECTION\n", ident);
   vb99_plusident();

   int stackcnt = 0;

   // there must be a soldermask as first and last
   // soldermask top       
   fprintfHKP(fp, "%sDIELECTRIC_LAYER %d\n", ident, ++stackcnt);
   vb99_plusident();
   fprintfHKP(fp, "%sDIELECTRIC_CONSTANT 1\n", ident);
   fprintfHKP(fp, "%sLAYER_DESCRIPTION \"Solder_Mask\"\n", ident);

   double thickness = 3*one_mil;
	fprintfHKP(fp, "%sLAYER_THICKNESS %1.*lf\n", ident, output_units_accuracy, thickness * scale);
   vb99_minusident();

   for (int l=1; l<=max_signalcnt; l++)
   {
      for (int i=0; i<maxArr; i++)
      {
         if (vbArr[i]->stackNum != l)
				continue;

         if (vbArr[i]->type == 'S')
         {
            fprintfHKP(fp, "%sSIGNAL_LAYER %d\n", ident, ++stackcnt);
            vb99_plusident();
            fprintfHKP(fp, "%sCONDUCTIVE_LAYER_NUMBER %d\n", ident, l);
            fprintfHKP(fp, "%sLAYER_DESCRIPTION \"Microstrip\"\n", ident);
            fprintfHKP(fp, "%sLAYER_ENABLED YES\n", ident);
            fprintfHKP(fp, "%sLAYER_PAIR %d\n", ident, max_signalcnt-l+1);

            double thickness = 0.7 * one_mil;
            LayerStruct *layer = doc->getLayerArray()[vbArr[i]->layerindex];
            Attrib *a =  is_attvalue(doc, layer->getAttributesRef(), LAYATT_THICKNESS, 2);
            if (a)
            {
               CString l = get_attvalue_string(doc, a);
               thickness = atof(l);
            }
            fprintfHKP(fp, "%sLAYER_THICKNESS %1.*lf\n", ident, output_units_accuracy, thickness * scale);
            fprintfHKP(fp, "%sDIRECTIONAL_BIAS VERTICAL\n", ident);  
            fprintfHKP(fp, "%sLAYER_RESISTIVITY 1.67e-008\n", ident);
            vb99_minusident();
         }
         else if (vbArr[i]->type == 'P')
         {
            fprintfHKP(fp, "%sPLANE_LAYER %d\n", ident, ++stackcnt);
            vb99_plusident();
            fprintfHKP(fp, "%sCONDUCTIVE_LAYER_NUMBER %d\n", ident, l);
            fprintfHKP(fp, "%sLAYER_DESCRIPTION \"Plane\"\n", ident);
            fprintfHKP(fp, "%sLAYER_ENABLED YES\n", ident);
            fprintfHKP(fp, "%sLAYER_PAIR %d\n", ident, max_signalcnt-l+1);
            fprintfHKP(fp, "%sDIRECTIONAL_BIAS VERTICAL\n", ident);  
            fprintfHKP(fp, "%sLAYER_RESISTIVITY 1.67e-008\n", ident);

            thickness = 1.4 * one_mil;
            LayerStruct *layer = doc->getLayerArray()[vbArr[i]->layerindex];

            if (layer->getLayerType() == LAYTYPE_POWERPOS)
               fprintfHKP(fp, "%sPLANE_TYPE POSITIVE\n", ident);
            else
               fprintfHKP(fp, "%sPLANE_TYPE NEGATIVE\n", ident);

            Attrib *a =  is_attvalue(doc, layer->getAttributesRef(), LAYATT_THICKNESS, 2);
            if (a)
            {
               CString l = get_attvalue_string(doc, a);
               thickness = atof(l);
            }
            fprintfHKP(fp, "%sLAYER_THICKNESS %1.*lf\n", ident, output_units_accuracy, thickness*scale);

            a = is_attvalue(doc, layer->getAttributesRef(), LAYATT_NETNAME, 2);
            if (a)
            {
               CString l = get_attvalue_string(doc, a); // can have multiple args
               if (strlen(l))
               {
                  char *lp = strtok(l.GetBuffer(0), "\n");
                  while (lp)
                  {
                     fprintfHKP(fp, "%sPLANE_NET \"%s\"\n", ident, lp);
                     vb99_plusident();
                     fprintfHKP(fp, "%sPLANE_NET_CLEARANCE %1.*lf\n", ident, output_units_accuracy, one_mil*scale);
                     fprintfHKP(fp, "%sUSE_ROUTE_BORDER NO\n", ident);
                     vb99_minusident();
   
                     lp = strtok(NULL, "\n");
                  }
               }
            }
            vb99_minusident();
         }

         // there must be dialectricum layers between all layers
         // to find the dialect layer, find the physical stackup layer 
         if (l < max_signalcnt)
         {
            fprintfHKP(fp, "%sDIELECTRIC_LAYER %d\n", ident, ++stackcnt);
            vb99_plusident();
            fprintfHKP(fp, "%sDIELECTRIC_CONSTANT 4.7\n", ident);
            fprintfHKP(fp, "%sLAYER_DESCRIPTION \"Core/Pre-preg\"\n", ident);

            thickness = 6 * one_mil;
            fprintfHKP(fp, "%sLAYER_THICKNESS %1.*lf\n", ident, output_units_accuracy, thickness*scale);

            vb99_minusident();
         }
         break;
      }
   }

   // soldermask bottom       
	fprintfHKP(fp, "%sDIELECTRIC_LAYER %d\n", ident, ++stackcnt);
   vb99_plusident();
   fprintfHKP(fp, "%sDIELECTRIC_CONSTANT 1\n", ident);
   fprintfHKP(fp, "%sLAYER_DESCRIPTION \"Solder_Mask\"\n", ident);

   thickness = 3 * one_mil;
   fprintfHKP(fp, "%sLAYER_THICKNESS %1.*lf\n", ident, output_units_accuracy, thickness * scale);
   vb99_minusident();
   vb99_minusident(); 
   fprintfHKP(fp, "\n");

   if (encryptOutputFlag)
      exportFileStream.Close();
   else
      fclose(fp);
   return 1;
}

/******************************************************************************
* write_aperture
*/
static int write_aperture(FILE *fp, BlockStruct *block, const char *name, double degree_rotation, double scale)
{
   // aperture is also used in PADSHAPE
   if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
   {
      double sizeA = block->getSizeA() * scale;
      double sizeB = block->getSizeB() * scale; 
      double xoffset= block->getXoffset() * scale;
      double yoffset= block->getYoffset() * scale;

      // offset is different than CAMCAD
      Rotate(-block->getXoffset() * scale, -block->getYoffset() * scale, degree_rotation, &xoffset, &yoffset);

      // a 0 size DIAMETER is not allowed.
      if (sizeA < one_mil*file->getScale() * unitsFactor)
         sizeA  = one_mil*file->getScale() * unitsFactor;
      if (sizeB < one_mil*file->getScale() * unitsFactor)
         sizeB  = one_mil*file->getScale() * unitsFactor;

		ApertureShapeTag shape = block->getShape();
		if (shape == apertureRectangle)
		{
			if (fabs(sizeA - sizeB) < SMALLNUMBER)
				shape = apertureSquare;
		}

      switch (shape)
      {
      case apertureRound:
			{
            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%sROUND\n", ident);
            vb99_plusident();
            fprintfHKP(fp, "%sDIAMETER %1.*lf\n", ident, output_units_accuracy, sizeA);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xoffset, output_units_accuracy, yoffset);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty
			}
         break;
		case apertureSquare:
			{
            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%sSQUARE\n", ident);
            vb99_plusident();
            fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, sizeA);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xoffset, output_units_accuracy, yoffset);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty
			}
         break;
      case apertureRectangle:
         {
            // the aperture rotation should not be counted, this is doe on padrot
            //int r = round(RadToDeg(block->getRotation()) + degree_rotation);
            int r = round(degree_rotation);               
            while (r < 0)
					r += 360;
            r = r % 360;

            if (r > 1)
            {
               // rotation greater 1 degree
					double tmp;
               switch (r)
               {
               case 90:
                  tmp = sizeA;
                  sizeA = sizeB;
                  sizeB = tmp;
                  break;
               case 180:
						break;
               case 270:
                  tmp = sizeA;
                  sizeA = sizeB;
                  sizeB = tmp;
                  break;
               default:
                  fprintf(flog, "Pad rotation [%d] for PadShape [%s] not supported\n", r, name);
                  display_error++;
                  break;
               }
            }

            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%sRECTANGLE\n", ident);
            vb99_plusident();
            fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, sizeA);
            fprintfHKP(fp, "%sHEIGHT %1.*lf\n", ident, output_units_accuracy, sizeB);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xoffset, output_units_accuracy, yoffset);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty
         }
         break;
      case apertureDonut:  
			{
            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%sROUND_DONUT\n", ident);
            vb99_plusident();
            fprintfHKP(fp, "%sDIAMETER %1.*lf\n", ident, output_units_accuracy, sizeA);
            double clearance = abs(sizeA - sizeB) / 2;
            fprintfHKP(fp, "%sTHERMAL_CLEARANCE %1.*lf\n", ident, output_units_accuracy, clearance);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xoffset, output_units_accuracy, yoffset);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty
			}
         break;
      case apertureThermal:
         {
            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%s4_WEB_ROUND_THERMAL_45\n", ident);
            vb99_plusident();
            fprintfHKP(fp, "%sDIAMETER %1.*lf\n", ident, output_units_accuracy, sizeB);

            double sizec =  block->getSizeC() * scale;
            if (sizec < one_mil*file->getScale() * unitsFactor)
               sizec = one_mil*file->getScale() * unitsFactor * 2;
            fprintfHKP(fp, "%sTIE_LEG_WIDTH %1.*lf\n", ident, output_units_accuracy, sizec); // <== gap width, not 0

            double clearance = abs(sizeA - sizeB) / 2;
            if (clearance > sizeB / 2)
            {
               clearance = sizeB / 2;
               fprintf(flog, "Thermal Clearance of [%s] is greater than 1/2 of Inner Diameter.  Not allow in Mentor Expedition/Veribest, changed to 1/2 of Inner Diameter.", name);
               display_error++;
            }

            fprintfHKP(fp, "%sTHERMAL_CLEARANCE %1.*lf\n", ident, output_units_accuracy, clearance);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xoffset, output_units_accuracy, yoffset);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty
         }
         break;
      case apertureOctagon:  
			{
            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%sOCTAGON\n", ident);
            vb99_plusident();
            fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, sizeA);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xoffset, output_units_accuracy, yoffset);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty
			}
         break;
      case apertureOblong: 
         {
            // the aperture rotation should not be counted, this is doe on padrot
            //int r = round(RadToDeg(block->getRotation()) + degree_rotation);
            int r = round(degree_rotation);
            while (r < 0)
					r += 360;
            r = r % 360;

            if (r > 1)
            {
               // rotation greater 1 degree
					double tmp;
               switch (r)
               {
               case 90:
                  tmp = sizeA;
                  sizeA = sizeB;
                  sizeB = tmp;
                  break;
               case 180:
                  break;
               case 270:
                  tmp = sizeA;
                  sizeA = sizeB;
                  sizeB = tmp;
                  break;
               default:
                  fprintf(flog, "Pad rotation [%d] for PadShape [%s] not supported\n", r, name);
                  display_error++;
                  break;
               }
            }

            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%sOBLONG\n", ident);
            vb99_plusident();
            fprintfHKP(fp, "%sWIDTH %1.*lf\n", ident, output_units_accuracy, sizeA);
            fprintfHKP(fp, "%sHEIGHT %1.*lf\n", ident, output_units_accuracy, sizeB);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xoffset, output_units_accuracy, yoffset);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty
         }
         break;
      case apertureComplex:
         {
            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
            
            //int res = TRUE;
            int res = VB99_IsComplexPadData(&(subblock->getDataList()));
            if (res)
            {
               vb99_plusident();
               fprintfHKP(fp, "%sCUSTOM\n", ident);
               vb99_plusident();

               /* possible forms
                  circle path
                  circle shape
                  rect path
                  rect shape
                  polyline path
                  polyline shape
                  polyarc path
                  polyarc shape
               */

               int done = VB99_ComplexPadData(fp, &(subblock->getDataList()), 0.0, 0.0, DegToRad(degree_rotation), 0, scale);
               if (!done)
               {
                  // write dummy custom
                  fprintfHKP(fp, "...POLYLINE_SHAPE\n");
                  fprintfHKP(fp, "....SHAPE_OPTIONS FILLED\n");
                  fprintfHKP(fp, "....XY (-0.05, -0.05)\n");
                  fprintfHKP(fp, "       ( 0.05, -0.05)\n");
                  fprintfHKP(fp, "       ( 0.05,  0.05)\n");
                  fprintfHKP(fp, "       (-0.05,  0.05)\n");
                  fprintfHKP(fp, "       (-0.05, -0.05)\n");
                  fprintf(flog, "Custom Pad has no contens [%s]\n", subblock->getName());
                  display_error++;
               }

               vb99_minusident();
               vb99_minusident();
            }
            else
            {
               // write a round 1 Mil
               vb99_plusident();
               fprintfHKP(fp, "%sROUND\n", ident);
               vb99_plusident();
               fprintfHKP(fp, "%sDIAMETER %1.*lf\n", ident, output_units_accuracy, one_mil * file->getScale() * unitsFactor );
               vb99_minusident();
               fprintfHKP(fp, "%sOFFSET (0, 0)\n", ident);
               vb99_minusident();
            }
            fprintfHKP(fp,"\n"); // just for beauty
         }
         break;
      default:
         {
            doc->CalcBlockExtents(block);
            sizeA = (block->getXmax() - block->getXmin()) * scale;
            fprintfHKP(fp, "%sPAD \"%s\"\n", ident, name);
            vb99_plusident();
            fprintfHKP(fp, "%sROUND\n", ident);
            vb99_plusident();
            fprintfHKP(fp," %sDIAMETER %1.*lf\n", ident, output_units_accuracy, sizeA);
            vb99_minusident();
            fprintfHKP(fp, "%sOFFSET (0, 0)\n", ident);
            vb99_minusident();
            fprintfHKP(fp, "\n"); // just for beauty

            fprintf(flog, "Unknown Aperture [%s] for Geometry [%s]\n", shapes[block->getShape()], name);
            display_error++;
         }
         break;
      }
   }

   return 1;
}

/******************************************************************************
* do_pads
*/
static int do_pads(FILE *fp, double scale)
{
	int i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
			continue;
      if (strlen(block->getName()) == 0) // only true width do not have names !
         continue;

      write_aperture(fp, block, block->getName(), 0.0, scale);
   }

   // here do the rotated padstacks
   for (i=0; i<padrotcnt; i++)
   {           

      CString padname = padrotarray[i]->newpadname;
      BlockStruct *block = doc->Find_Block_by_Num(padrotarray[i]->bnum);

      if( block->getShape() != apertureRound ){       //round doesn't need to be rotated, always will be the same
         write_aperture(fp, block, padname, padrotarray[i]->rotation, scale);
      }
   }

   return 1;
} 

/******************************************************************************
* do_holes
*/
static int do_holes(FILE *fp, double scale)
{
	CString plated = "PLATED";
   double smallesthole = 1;
	int i=0;
   for (i=0; i<output_units_accuracy; i++)
      smallesthole *=0.1;
   
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
			continue;
      if (block->getFlags() & BL_WIDTH)
         continue;

      // aperture is also used in PADSHAPE
      if (block->getFlags() & BL_TOOL)
      {
         double sizeA = block->getSizeA() * scale;
         double sizeB = block->getSizeB() * scale; 
         double xoffset= block->getXoffset() * scale;
         double yoffset= block->getYoffset() * scale;

         if (!strlen(default_hole))
            default_hole = block->getName();

         double holesize = block->getToolSize()*scale;
         if (holesize < smallesthole)
         {
            fprintf(flog, "Tool [%s] size [%lf] too small ->set to 1.*lf\n", block->getName(),  
               block->getToolSize() * scale, output_units_accuracy, smallesthole);
            display_error++;

            holesize = smallesthole;
         }

         fprintfHKP(fp, "%sHOLE \"%s\"\n", ident, block->getName());
         vb99_plusident();

         fprintfHKP(fp,"%sROUND\n", ident);
         vb99_plusident();
         fprintfHKP(fp,"%sDIAMETER %1.*lf\n", ident, output_units_accuracy, holesize);
         vb99_minusident();

         if (holePosTol != 0.0)
            fprintfHKP(fp,"%sPOSITIVE_TOLERANCE %1.*lf\n", ident, output_units_accuracy, holePosTol);
         if (holeNegTol != 0.0)
            fprintfHKP(fp,"%sNEGATIVE_TOLERANCE %1.*lf\n", ident, output_units_accuracy, holeNegTol);
      
         plated = "PLATED";
         if (!block->getToolHolePlated())
            plated = "NON_PLATED";
         fprintfHKP(fp, "%sHOLE_OPTIONS %s DRILLED USER_GENERATED_NAME\n", ident, plated);
         vb99_minusident();
         fprintfHKP(fp, "\n"); // just for beauty
      }
   }

   return 1;
}

/******************************************************************************
* VB99_WritePADSTACKData

	return	0x1 = top
				0x2 = bottom
				0x3 = all
				0x4 = inner  
				0x8 = drill
				0x10 = clearance
				0x20 = thermal 
*/
static int VB99_WritePADSTACKData(FILE *wfp, const char *padstkname, CDataList *DataList, double rotation, int mirror,
											 double scale, int embeddedLevel, int insertLayer, int smdpad, VB99Padforms *padform)
{
   int pl = 0;
	int lastpl = 0;
   int holecnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct  *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)	continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE)    continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLSYMBOL) continue;

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
      double block_rot = rotation + np->getInsert()->getAngle();

      int block_layer = -1;
      if (insertLayer != -1)
         block_layer = insertLayer;
      else if (np->getLayerIndex() != -1)
         block_layer = np->getLayerIndex();

      if (mirror)
         block_rot = (rotation + np->getInsert()->getAngle()) -PI;   // as defined in VB99_GetPadstack

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      block_rot += block->getRotation();

      // here do hole
      if (block->getFlags() & BL_TOOL)
      {
         if (!holecnt)
         {
            pl |= 8;
            padform->HOLE_NAME = block->getName();
            padform->HOLE_X = point2.x;
            padform->HOLE_Y = point2.y;
         }
         else
         {
            fprintf(flog, "Padstack [%s] has multiple drill holes -> ignored.\n", padstkname);
            display_error++;
         }
         holecnt++;
      }

      if (fabs(point2.x) > one_mil || fabs(point2.x) > one_mil)
      {
         fprintf(flog,"Padstack [%s] has Padshape, which is offset [%1.*lf, %1.*lf].\n", 
               padstkname, output_units_accuracy, point2.x, output_units_accuracy, point2.y );
         display_error++;
      }

      if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
      {
         // block_layer is not set.
         if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            if (block->getShape() == T_COMPLEX)
               block = doc->Find_Block_by_Num((int)block->getSizeA());

				block_layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
         }
      }

      CString layername = Layer_VB99(block_layer);
      LayerStruct *l = doc->FindLayer(block_layer);

      int layerptr;
      if ((layerptr = get_layerptr_from_index(block_layer)) < 0)
			continue;
      if (!vbArr[layerptr]->on)                                   
			continue;

      if (l)
      {
         if (l->getLayerType() == LAYTYPE_DRILL)
         {
            // do not translate a drill layer or drill drawing layer
            continue;
         }
         else if (l->getLayerType() == LAYTYPE_PAD_TOP || l->getLayerType() == LAYTYPE_SIGNAL_TOP)
         {
            if (pl & 1)
					continue;   // do not duplicate
            layername = "TOP_PAD";
            pl |= 1;
         }
         else if ( l->getLayerType() == LAYTYPE_PAD_BOTTOM || l->getLayerType() == LAYTYPE_SIGNAL_BOT)
         {
            if (pl & 2)
					continue;   // do not duplicate
            layername = "BOTTOM_PAD";
            pl |= 2;
         }
         else if (l->getLayerType() == LAYTYPE_PAD_ALL || l->getLayerType() == LAYTYPE_SIGNAL_ALL)
         {
            layername = "ALL";
            pl |= 7; // outer and inner
         }
         else if (l->getLayerType() == LAYTYPE_PAD_OUTER)
         {
            layername = "OUTER";
            pl |= 3;
         }
         else if (l->getLayerType() == LAYTYPE_POWERNEG)
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD) 
            {
               layername = "CLEARANCE_PAD";
               pl |= 0x10;
            }
            else
            {
               layername = "THERMAL_PAD";
               pl |= 0x20;
            }
         }
         else if (l->getLayerType() == LAYTYPE_MASK_TOP)
         {
            layername = "TOP_SOLDERMASK_PAD";
         }
         else if (l->getLayerType() == LAYTYPE_MASK_BOTTOM)
         {
            layername = "BOTTOM_SOLDERMASK_PAD";
         }
         else if (l->getLayerType() == LAYTYPE_MASK_ALL)
         {
            layername = "SOLDERMASK";
         }
         else if (l->getLayerType() == LAYTYPE_PASTE_TOP)
         {
            layername = "TOP_SOLDERPASTE_PAD";
         }
         else if (l->getLayerType() == LAYTYPE_PASTE_BOTTOM)
         {
            layername = "BOTTOM_SOLDERPASTE_PAD";
         }
         else if (l->getLayerType() == LAYTYPE_PASTE_ALL)
         {
            layername = "SOLDERPASTE";
         }
         else if (l->getLayerType() == LAYTYPE_PAD_INNER || l->getLayerType() == LAYTYPE_SIGNAL_INNER)
         {
            
            layername = "INTERNAL_PAD";
            pl |= 4;
         }
      }
      
      CString  blockname = block->getName();
      
      if ( block_rot && ( block->getShape() != apertureRound ) )  //round apertures don't need to be rotated
      {
         int rot = round(RadToDeg(block_rot));
         while (rot < 0)
				rot += 360;
         while (rot >= 360)
				rot -= 360;

         CString degreerot;
         degreerot.Format("%d", rot);
         blockname += "_";
         blockname += degreerot;
      }

      // should have a pointx,y reference
      if (strlen(layername))
      {
         // here is because the layername could have been done in layer map.
         if (!STRCMPI(layername, "TOP_PAD"))  
         {
            padform->TOP_PAD = blockname;
            pl |= 1;
         }
         else if (!STRCMPI(layername, "BOTTOM_PAD"))  
         {
            padform->BOTTOM_PAD = blockname;
            pl |= 2;
         }
         else if (!STRCMPI(layername, "ALL"))   
         {
            padform->ALL = blockname;
            pl |= 3;
         }
         else if (!STRCMPI(layername, "OUTER"))    
         {
            padform->OUTER = blockname;
            pl |= 3;
         }
         else if (!STRNICMP(layername, "INTERNAL_PAD", 5))  
         {
            padform->INTERNAL_PAD = blockname;
            pl |= 4;
         }
         else if (!STRNICMP(layername, "CLEARANCE_PAD", 5))    
         {
            padform->CLEARANCE_PAD = blockname;
            pl |= 0x10;
         }
         else if (!STRNICMP(layername, "THERMAL_PAD", 5))   
         {
            // only thermal padstacks are allowed.
            if (block->getShape() == T_THERMAL)
            {
               padform->THERMAL_PAD = blockname;
               pl |= 0x20;
            }
         }
         else if (!STRCMPI(layername, "SOLDERMASK"))  
         {
            // no flag
            padform->SOLDERMASKALL = blockname;
            pl |= 0x40;
         }
         else if (!STRCMPI(layername, "TOP_SOLDERMASK_PAD"))   
         {
            padform->TOP_SOLDERMASK_PAD = blockname;
            pl |= 0x80;
         }
         else if (!STRCMPI(layername, "BOTTOM_SOLDERMASK_PAD"))   
         {
            padform->BOTTOM_SOLDERMASK_PAD = blockname;
            pl |= 0x100;
         }
         else if (!STRCMPI(layername, "SOLDERPASTE"))    
         {
            padform->SOLDERPASTEALL = blockname;
            pl |= 0x200;
         }
         else if (!STRCMPI(layername, "TOP_SOLDERPASTE_PAD"))  
         {
            padform->TOP_SOLDERPASTE_PAD = blockname;
            pl |= 0x400;
         }
         else if (!STRCMPI(layername, "BOTTOM_SOLDERPASTE_PAD"))  
         {
            padform->BOTTOM_SOLDERPASTE_PAD = blockname;
            pl |= 0x800;
         }
         else
         {
            fprintf(flog, "PADSTACK [%s] PAD [%s] Layer [%s] not translated\n", padstkname, blockname, layername);
            display_error++;
         }
		}
   } 

   return pl;
}

/******************************************************************************
* get_padrot
*/
static int get_padrot(const char *p, int rot)
{
   for (int i=0; i<padrotcnt; i++)
   {
      if (!strcmp(padrotarray[i]->padname, p) && padrotarray[i]->rotation == rot)
         return i;
   }

   return -1;
}

/******************************************************************************
* VB99_GetPADSTACKData
*/
static unsigned long VB99_GetPADSTACKData(const char *padstkname, CDataList *DataList, double rotation,
														int mirror, double scale, int embeddedLevel, int insertLayer)
{
   int padattcnt = 0;
   unsigned long  pl = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)   continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD)    continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE)    continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLSYMBOL) continue;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
      double block_rot = rotation + np->getInsert()->getAngle();

      int block_layer = -1;
      if (insertLayer != -1)
         block_layer = insertLayer;
      else if (np->getLayerIndex() != -1)
         block_layer = np->getLayerIndex();

      if (mirror)
         block_rot = (rotation + np->getInsert()->getAngle()) -PI;   // as defined in VB99_WritePadstack

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      block_rot += block->getRotation();

      if (block->getFlags() & BL_TOOL)
			continue;

      if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
      {
         // block_layer is not set.
         if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            if (block->getShape() == T_COMPLEX)
               block = doc->Find_Block_by_Num((int)block->getSizeA());

				block_layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
         }
      }

      // here find rotated pad_shapes
      // this pin has no padshape
      if (block->getName() && strlen(block->getName()) && block_rot)
      {
         int rot = round(RadToDeg(block_rot));
         while (rot < 0)
				rot += 360;
         while (rot >= 360)
				rot -= 360;

         CString degreerot;
         degreerot.Format("%d", rot);
         CString padname = block->getName();
         padname += "_";
         padname += degreerot;

         if (get_padrot(block->getName(), rot) < 0)
         {
            VB99PadRotStruct *p = new VB99PadRotStruct;
            p->bnum = block->getBlockNumber();
            p->padname = block->getName();     
            p->newpadname = padname;
            p->rotation = rot;
            padrotarray.SetAtGrow(padrotcnt++, p);
         }
      }

      LayerStruct *l = doc->FindLayer(block_layer);
      int stacknr = get_layernr_from_index(block_layer);
      if (stacknr == LAY_TOP) 
		{
         pl |= 1L;
		}
      else if (stacknr == LAY_BOT) 
		{
         pl |= 1L << (max_signalcnt -1);
		}
      else if (stacknr == LAY_INNER)  
      {
         for (int w=2; w<max_signalcnt; w++)
            pl |= 1L << (w -1);
      }
      else if (stacknr == LAY_ALL) 
      {
         for (int w=1; w<=max_signalcnt; w++)
            pl |= 1L << (w -1);
      }
      else if (stacknr > 0)
		{
         pl |= 1L << (stacknr -1);
		}
   }

   return pl;
}

/******************************************************************************
* get_startlayer
*/
static int get_startlayer(unsigned long l)
{
   int s = 1;

   // get the lowest map
   for (int i=1; i<=max_signalcnt; i++)
	{
      if (l & (1L << (i-1)))
			return i;
	}

   return s;
}

/******************************************************************************
* get_endlayer
*/
static int get_endlayer(unsigned long l)
{
   int s = max_signalcnt;

   // get the higest map
   for (int i=1; i<=max_signalcnt; i++)
	{
      if (l & (1L << (i-1)))  
         s = i;
	}

   return s;
}

/******************************************************************************
* AddPadstackToArrary
*/
static VB99Padstack* AddPadstackToArrary(BlockStruct *block, bool smd)
{
	VB99Padstack *p = NULL;
	int padstackIndex = get_padstack_index(block->getName());
	if (padstackIndex == -1)
	{
      bool oppside = !block->IsDesignedOnTop(doc->getCamCadData());

		double drill;
		if (block->getBlockType() == blockTypeDrillHole)
			drill = block->getToolSize();
		else
			drill = get_drill_from_block_no_scale(doc, block);

      if (smd && drill > 0)
      {
         fprintf(flog, "Padstack [%s] is marked as SMD and has a Drill [%lf] (this is not allowed and SMD mark is eliminated).\n", 
            block->getName(), drill);
         display_error++;
         smd = false;
      }

      //BlockStruct *tmpBlock( block );
      if (smd == false)
      {
         // Must check to make sure the padshape inside the padstack is either round or square
         CheckPadshapeOfThruPadstack( block );
      }
      unsigned long layermap = VB99_GetPADSTACKData(block->getName(), &block->getDataList(), 0.0, 0, 1.0, 0, -1);

		p = new VB99Padstack;
		p->blocknum = block->getBlockNumber();
		p->padname = block->getName();
		p->compPadname = "";
		p->vianame = "";
		p->fidPadname = "";
		p->mountingPadname = "";
		p->used_as_comp = false;
		p->used_as_via = false;
		p->used_as_fiducial = false;
		p->used_as_mountingHole = false;
		p->smd = smd;
		p->oppside = oppside;
		p->drill = drill;
		p->startlayernumber = get_startlayer(layermap);
		p->endlayernumber   = get_endlayer(layermap);
		padstackarray.SetAtGrow(padstackcnt++, p);		
	}
	else
	{
		p = padstackarray[padstackIndex];
	}

	return p;
}

/******************************************************************************
* CreatePadstackArrayFromBlockArray
*/
static int CreatePadstackArrayFromBlockArray()
{
	WORD technologyKey = (WORD)doc->RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);

	for (int i=0; i<doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);
		if (block == NULL || block->getFlags() & BL_WIDTH)
			continue;

		CreatePadstackArrayFromBlock(block);
	}

	return 1;
}

/******************************************************************************
* CreatePadstackArrayFromInserts
*/
static int CreatePadstackArrayFromBlock(BlockStruct *block)
{
	WORD technologyKey = (WORD)doc->RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);

   CDataList& dataList = block->getDataList();
	POSITION pos = dataList.GetHeadPosition();
	while (pos)
	{
      const DataStruct* data = dataList.GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert || data->getInsert() == NULL)
			continue;
	
      const InsertStruct* insert = data->getInsert();
		InsertTypeTag insertType = insert->getInsertType();

		if (insertType == insertTypePin				|| insertType == insertTypeVia		|| insertType == insertTypeFiducial  ||
			 insertType == insertTypeMechanicalPin || insertType == insertTypeFreePad	|| insertType == insertTypeDrillHole || insertType == insertTypeDrillTool)
		{
			BlockStruct *subBlock = doc->getBlockAt(insert->getBlockNumber());
			if (subBlock == NULL || subBlock->getFlags() & BL_WIDTH)
				continue;

			bool smd = false;
			Attrib *attrib = NULL;
			if (subBlock->getAttributes() && subBlock->getAttributes()->Lookup(technologyKey, attrib))
			{
            smd = (attrib->getStringValue().CompareNoCase("SMD") == 0);
			}


			VB99Padstack *p = AddPadstackToArrary(subBlock, smd);
			bool alreadyUsed = (p->used_as_comp || p->used_as_fiducial || p->used_as_mountingHole || p->used_as_via);
			CString errorMessage = "";
			CString newPadname = p->padname;

			if (insertType == insertTypePin)
			{
            if( !p->used_as_comp )
            {
				   if (alreadyUsed)
				   {
					   newPadname.Format("%s_PIN", p->padname);
					   errorMessage.Format("Padstack [%s] used as Pin is already used as another padstack type, renamed to [%s]\n", p->padname, newPadname);
				   }

				   p->compPadname = newPadname;
				   p->used_as_comp = true;
            }
			}
			else if (insertType == insertTypeVia)
			{
            if( !p->used_as_via )
            {
				   if (alreadyUsed)
				   {
					   newPadname.Format("%s_VIA", p->padname);
					   errorMessage.Format("Padstack [%s] used as Via is already used as another padstack type, renamed to [%s]\n", p->padname, newPadname);
				   }

				   p->used_as_via = true;
				   p->vianame = newPadname;
            }
			}
			else if (insertType == insertTypeFiducial)
			{
            if( !p->used_as_fiducial )
            {
				   if (alreadyUsed)
				   {
					   newPadname.Format("%s_FID", p->padname);
					   errorMessage.Format("Padstack [%s] used as Fiducial is already used as another padstack type, renamed to [%s]\n", p->padname, newPadname);
				   }

				   p->used_as_fiducial = true;
				   p->fidPadname = newPadname;
            }
			}
			else if (!p->used_as_mountingHole)
			{
				if (alreadyUsed)
				{
					newPadname.Format("%s_MH", p->padname);
					errorMessage.Format("Padstack [%s] used as Mounting Hole is already used as another padstack type, renamed to [%s]\n", p->padname, newPadname);
				}

				p->used_as_mountingHole = true;
				p->mountingPadname = newPadname;
			}

			if (!errorMessage.IsEmpty())
				fprintf(flog, errorMessage);
		}
		else 
		{
			BlockStruct *subBlock = doc->getBlockAt(insert->getBlockNumber());
			if (subBlock->getBlockType() == blockTypePadstack || insert->getInsertType() == insertTypePcbComponent || insert->getInsertType() == insertTypePcb)
			{
				if (block->getBlockNumber() != subBlock->getBlockNumber())
					CreatePadstackArrayFromBlock(subBlock);
			}
		}
	}

   return 1;
}

/******************************************************************************
* CheckPadshapeOfThruPadstack
*/
static int CheckPadshapeOfThruPadstack(BlockStruct *block)
{
	POSITION pos = block->getDataList().GetHeadPosition();
	while(pos)
	{
		DataStruct *data = block->getDataList().GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert || data->getInsert() == NULL)
			continue;

		InsertStruct *insert = data->getInsert();
		BlockStruct *subBlock = doc->getBlockAt(insert->getBlockNumber());

		// Make sure it is an aperture
		if (!(subBlock->getFlags() & BL_APERTURE) && !(subBlock->getFlags() & BL_BLOCK_APERTURE) )
			continue;
	
		// Skip if the aperture is round or rectangle
		ApertureShapeTag shape = subBlock->getShape();
		if (shape == apertureRound || shape == apertureSquare)
			continue;

		// Check to see if there is an existing of this aperture defined for through hole padstack
		CString newBlockName = subBlock->getName() + "_TH";
		BlockStruct *newBlock = Graph_Block_Exists(doc, newBlockName, subBlock->getFileNumber());

		if (newBlock == NULL)
		{
			// Create a new aperture and copy all the properties
			newBlock = doc->getDefinedBlockAt(doc->getMaxBlockIndex() + 1);
			newBlock->setProperties(*subBlock);
			newBlock->setName(newBlockName);

			newBlock->setSizeB(0.0);
			newBlock->setSizeC(0.0);
			newBlock->setSizeD(0.0);

			if (shape == apertureRectangle || shape == apertureOblong)
			{
				// Set sizeA to smaller size of either the rectangle or oblong
				if (subBlock->getSizeA() < subBlock->getSizeB())
					newBlock->setSizeA(subBlock->getSizeA());
				else
					newBlock->setSizeA(subBlock->getSizeB());

				if (shape == apertureRectangle)
					newBlock->setShape(apertureSquare);
				else
					newBlock->setShape(apertureRound);
			}
			else
			{
				// Set sizeA to the smaller size of the extent of the aperutre
				CExtent extent = subBlock->getExtent();
				if (extent.getSize().cx < extent.getSize().cy)
					newBlock->setSizeA((DbUnit)extent.getSize().cx);
				else
					newBlock->setSizeA((DbUnit)extent.getSize().cy);

				newBlock->setShape(apertureRound);
			}

			CString errorMessage = "";
			errorMessage.Format("Aperture [%s] in THROUGH HOLE padstack [%s] is not round or square, changed to aperture [%s]\n",
					subBlock->getName(), block->getName(), newBlock->getName());
			fprintf(flog, errorMessage);
		}

		// Set the insert to the new aperture
		insert->setBlockNumber(newBlock->getBlockNumber());
	}

	return 1;
}




/******************************************************************************
* write_padform
*/
static int write_padform(FILE *fp, VB99Padforms padform, int smd,  CString padstackType)
{
	BOOL isOppSiePadstack = FALSE;

   if (strlen(padform.OUTER))
   {
      if (strlen(padform.BOTTOM_PAD) == 0)
         padform.BOTTOM_PAD = padform.OUTER;
      if (strlen(padform.TOP_PAD) == 0)
         padform.TOP_PAD = padform.OUTER;
   }

   if (strlen(padform.ALL))
   {
      if (strlen(padform.BOTTOM_PAD) == 0)
         padform.BOTTOM_PAD = padform.ALL;
      if (strlen(padform.TOP_PAD) == 0)
         padform.TOP_PAD = padform.ALL;
      if (strlen(padform.INTERNAL_PAD) == 0)
         padform.INTERNAL_PAD = padform.ALL;
   }

   if (strlen(padform.SOLDERMASKALL))
   {
      if (strlen(padform.BOTTOM_SOLDERMASK_PAD) == 0)
         padform.BOTTOM_SOLDERMASK_PAD = padform.SOLDERMASKALL;
      if (strlen(padform.TOP_SOLDERMASK_PAD) == 0)
         padform.TOP_SOLDERMASK_PAD = padform.SOLDERMASKALL;
   }

   if (strlen(padform.SOLDERPASTEALL))
   {
      if (strlen(padform.BOTTOM_SOLDERPASTE_PAD) == 0)
         padform.BOTTOM_SOLDERPASTE_PAD = padform.SOLDERPASTEALL;
      if (strlen(padform.TOP_SOLDERPASTE_PAD) == 0)
         padform.TOP_SOLDERPASTE_PAD = padform.SOLDERPASTEALL;
   }

   if( smd )
   {
      if (strlen(padform.BOTTOM_SOLDERPASTE_PAD) == 0)
         padform.BOTTOM_SOLDERPASTE_PAD = padform.TOP_SOLDERPASTE_PAD;
      if (strlen(padform.TOP_SOLDERPASTE_PAD) == 0)
         padform.TOP_SOLDERPASTE_PAD = padform.BOTTOM_SOLDERPASTE_PAD;

      if (strlen(padform.BOTTOM_SOLDERMASK_PAD) == 0)
         padform.BOTTOM_SOLDERMASK_PAD = padform.TOP_SOLDERMASK_PAD;
      if (strlen(padform.TOP_SOLDERMASK_PAD) == 0)
         padform.TOP_SOLDERMASK_PAD = padform.BOTTOM_SOLDERMASK_PAD;
   }

   if (!padstackType.CompareNoCase("PIN_SMD")) // can not count on smd as pin may have been used as mechanical pin(mounting_hole).
   {
      if (padform.TOP_PAD.IsEmpty())
      {
         padform.TOP_PAD = "NULLPAD";
         if (strlen(padform.BOTTOM_PAD) == 0)  // if the bottom pad is also null, then both top and bottom is NULLPAD
			{
            padform.BOTTOM_PAD = "NULLPAD";
				isOppSiePadstack = FALSE;
			}
         else
			{
            padform.TOP_PAD = padform.BOTTOM_PAD;  // if the bottom is defined, then the top gets the same pad as bottom pad
				isOppSiePadstack = TRUE;
			}
      }
      else
      {
         if (strlen(padform.BOTTOM_PAD) == 0)
            padform.BOTTOM_PAD = padform.TOP_PAD;
      }

      fprintfHKP(fp,"%s%s \"%s\"\n", ident, "TOP_PAD", padform.TOP_PAD);
      fprintfHKP(fp,"%s%s \"%s\"\n", ident, "BOTTOM_PAD", padform.BOTTOM_PAD);

      if (strlen(padform.TOP_SOLDERMASK_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "TOP_SOLDERMASK_PAD", padform.TOP_SOLDERMASK_PAD);

      if (strlen(padform.BOTTOM_SOLDERMASK_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "BOTTOM_SOLDERMASK_PAD", padform.BOTTOM_SOLDERMASK_PAD);

      if (strlen(padform.TOP_SOLDERPASTE_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "TOP_SOLDERPASTE_PAD", padform.TOP_SOLDERPASTE_PAD);

      if (strlen(padform.BOTTOM_SOLDERPASTE_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "BOTTOM_SOLDERPASTE_PAD", padform.BOTTOM_SOLDERPASTE_PAD);
   }
   else
   {
      if (strlen(padform.HOLE_NAME) == 0)
         padform.HOLE_NAME = "NULLHOLE";

      if (strlen(padform.TOP_PAD) == 0)
         padform.TOP_PAD = "NULLPAD";
      if (strlen(padform.BOTTOM_PAD) == 0)
         padform.BOTTOM_PAD = "NULLPAD";
      if (strlen(padform.INTERNAL_PAD) == 0)
         padform.INTERNAL_PAD = "NULLPAD";

      fprintfHKP(fp,"%s%s \"%s\"\n", ident, "TOP_PAD", padform.TOP_PAD);

      if (padstackType.CompareNoCase("FIDUCIAL"))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "INTERNAL_PAD", padform.INTERNAL_PAD);
      fprintfHKP(fp,"%s%s \"%s\"\n", ident, "BOTTOM_PAD", padform.BOTTOM_PAD);

      if (strlen(padform.CLEARANCE_PAD) && padstackType.CompareNoCase("FIDUCIAL"))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "CLEARANCE_PAD", padform.CLEARANCE_PAD);

      if (strlen(padform.THERMAL_PAD) && padstackType.CompareNoCase("FIDUCIAL"))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "THERMAL_PAD", padform.THERMAL_PAD);

      if (strlen(padform.TOP_SOLDERMASK_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "TOP_SOLDERMASK_PAD", padform.TOP_SOLDERMASK_PAD);

      if (strlen(padform.BOTTOM_SOLDERMASK_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "BOTTOM_SOLDERMASK_PAD", padform.BOTTOM_SOLDERMASK_PAD);

      if (strlen(padform.TOP_SOLDERPASTE_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "TOP_SOLDERPASTE_PAD", padform.TOP_SOLDERPASTE_PAD);

      if (strlen(padform.BOTTOM_SOLDERPASTE_PAD))
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "BOTTOM_SOLDERPASTE_PAD", padform.BOTTOM_SOLDERPASTE_PAD);

      if (padstackType.CompareNoCase("FIDUCIAL"))
      {
         fprintfHKP(fp,"%s%s \"%s\"\n", ident, "HOLE_NAME", padform.HOLE_NAME);
         vb99_plusident();
         fprintfHKP(fp,"%sOFFSET (%1.*lf, %1.*lf)\n", ident, 
                     output_units_accuracy, padform.HOLE_X, output_units_accuracy, padform.HOLE_Y );
         vb99_minusident();
      }
      
   }

   return isOppSiePadstack;
}

/******************************************************************************
* init_padform
*/
static void init_padform(VB99Padforms *padform)
{
   padform->BOTTOM_PAD = "";
   padform->BOTTOM_SOLDERMASK_PAD = "";
   padform->CLEARANCE_PAD = "";
   padform->HOLE_NAME = "";
   padform->HOLE_X = padform->HOLE_Y = 0.0;
   padform->INTERNAL_PAD = "";
   padform->THERMAL_PAD = "";
   padform->TOP_PAD = "";
   padform->TOP_SOLDERMASK_PAD = "";
   padform->TOP_SOLDERPASTE_PAD = "";
   padform->BOTTOM_SOLDERPASTE_PAD = "";
   padform->ALL = "";
   padform->OUTER = "";
   padform->SOLDERMASKALL = "";
   padform->SOLDERPASTEALL = "";

   return;
}

/******************************************************************************
* write_padstacks
*/
static int write_padstacks(FILE *fp, double scale)
{
   for (int i=0; i<padstackcnt; i++)
   {
		VB99Padstack *padstack = padstackarray[i];
      if (!padstack->used_as_comp && !padstack->used_as_fiducial && !padstack->used_as_mountingHole && !padstack->used_as_via) 
			continue;

      bool smd = false;
		CString padstackName = "";
		CString padstackType = "";

		if (padstack->used_as_comp)
		{
			padstackName = check_name('p', padstack->compPadname);

			// in Veribest, a smd can never have as drill - otherwise the system crashes !!!
			if (padstackarray[i]->smd && padstackarray[i]->drill == 0)
			{
				smd = true;
				padstackType = "PIN_SMD";
			}
			else
			{
				padstackType = "PIN_THROUGH";
			}

			write_padstack_info(fp, scale, smd, padstackName, padstackType, padstack);
		}

		if (padstack->used_as_fiducial)
		{
			padstackName = check_name('p', padstack->fidPadname);
			padstackType = "FIDUCIAL";

			write_padstack_info(fp, scale, smd, padstackName, padstackType, padstack);
		}

		if (padstack->used_as_mountingHole)
		{
			padstackName = check_name('p', padstack->mountingPadname);
			padstackType = "MOUNTING_HOLE";

			write_padstack_info(fp, scale, smd, padstackName, padstackType, padstack);
		}

		if (padstack->used_as_via)
		{
			padstackName = check_name('p', padstack->vianame);
			padstackType = "VIA";

			write_padstack_info(fp, scale, smd, padstackName, padstackType, padstack);
		}
   }

   return 1;
}

/******************************************************************************
* write_padstack_info
*/
static int write_padstack_info(FILE *fp, double scale, bool smd, CString padstackName, CString padstackType, VB99Padstack *padstack)
{

   BlockStruct *block = doc->Find_Block_by_Num(padstack->blocknum);

   VB99Padforms padform;
	init_padform(&padform);

   int res = VB99_WritePADSTACKData(fp, padstackName, &block->getDataList(), 0.0, 0, scale, 0, -1, smd, &padform);
	if (!padstackType.CompareNoCase("MOUNTING_HOLE") && !(res & 8))
	{
		fprintf(flog, "Padstack [%s] of type MOUNTING_HOLE without drill hole, use default drill hole [%s]\n", padstackName, default_hole);
		display_error++;
	}
	if (!padstackType.CompareNoCase("VIA") && !(res & 8))
	{
		fprintf(flog, "Padstack [%s] of type VIA without drill hole, use default drill hole [%s]\n", padstackName, default_hole);
		display_error++;
	}
	if (!padstackType.CompareNoCase("PIN_THROUGH") && !(res & 8))
	{
		fprintf(flog, "Padstack [%s] of type PIN_THROUGH without drill hole, use default drill hole [%s]\n", padstackName, default_hole);
		display_error++;
	}

	fprintfHKP(fp,"%sPADSTACK \"%s\"\n", ident, padstackName);
   vb99_plusident();
   fprintfHKP(fp,"%sPADSTACK_TYPE %s\n", ident, padstackType);
   fprintfHKP(fp,"%sTECHNOLOGY \"(Default)\"\n", ident);
   vb99_plusident();
   fprintfHKP(fp,"%sTECHNOLOGY_OPTIONS NONE\n", ident);

   if (write_padform(fp, padform, smd, padstackType) > 0)
		padstack->oppside = TRUE;

   vb99_minusident();
   vb99_minusident();
   fprintfHKP(fp,"\n");

	return 1;
}

/******************************************************************************
* do_padstack
*/
static int do_padstack(FileStruct *file, const char *fname)
{
   FILE *fp;

   if ((fp = fopen(fname, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fname);
      ErrorMessage(tmp);
      return -1;
   }
   else if (encryptOutputFlag)
   {
      fclose(fp);
      exportFileStream.SetOutputStream(fname);
   }

   created_by(fp, "PADSTACK_LIBRARY");
   default_hole = "NULLHOLE";

   do_pads(fp, file->getScale() * unitsFactor);

   // defined a NULLPAD, because thru padstacks must have a TOP INTERNAL BOTTOM definition
   fprintfHKP(fp, "%sPAD \"%s\"\n", ident, "NULLPAD");
   vb99_plusident();
   fprintfHKP(fp, "%sROUND\n", ident);
   vb99_plusident();
   fprintfHKP(fp, "%sDIAMETER %1.*lf\n", ident, output_units_accuracy, one_mil*file->getScale() * unitsFactor );
   vb99_minusident();
   fprintfHKP(fp, "%sOFFSET (0, 0)\n", ident);
   vb99_minusident();
   fprintfHKP(fp, "\n"); // just for beauty

   // define a NULLHOLE 
   fprintfHKP(fp, "%sHOLE \"%s\"\n", ident, default_hole);
   vb99_plusident();
   fprintfHKP(fp, "%sROUND\n", ident);
   vb99_plusident();
   fprintfHKP(fp, "%sDIAMETER %1.*lf\n", ident, output_units_accuracy, one_mil*file->getScale() * unitsFactor );
   vb99_minusident();
   fprintfHKP(fp, "%sHOLE_OPTIONS PLATED DRILLED AUTO_GENERATED_NAME\n", ident);
   fprintfHKP(fp, "%sDEPTH_ASSIGNMENT_METHOD THROUGH_HOLE\n", ident);
   fprintfHKP(fp, "%sDRILL_SYMBOL\n", ident);
   vb99_plusident();
   fprintfHKP(fp, "%sASSIGN_DURING_OUTPUT\n", ident);
   vb99_minusident();
   vb99_minusident();
   fprintfHKP(fp, "\n"); // just for beauty

   do_holes(fp, file->getScale() * unitsFactor);
   write_padstacks(fp, file->getScale() * unitsFactor);

   if (encryptOutputFlag)
      exportFileStream.Close();
   else
      fclose(fp); // close 

   return 1;
}

/******************************************************************************
* complist_device_written
*/
static int complist_device_written(const char *geom, const char *device)
{
   for (int i=0; i<complistcnt; i++)
   {
      if (complistarray[i]->geomname.Compare(geom) == 0 && complistarray[i]->devicename.Compare(device) == 0)
         complistarray[i]->devicefile_written = TRUE;
   }

   return 1;
}

/******************************************************************************
* VB99_ReadPinData
*/
static int VB99_ReadPinData(CDataList *DataList)
{
   int pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() != INSERTTYPE_PIN)
			continue;

      VB99CompPinInst *c = new VB99CompPinInst;
      comppininstarray.SetAtGrow(comppininstcnt++, c);        
      c->insertnum = np->getInsert()->getBlockNumber();
      c->pinname = np->getInsert()->getRefname();
      pincnt++;
   } 

   return pincnt;
} 

/******************************************************************************
* check_devicefiles
*/
static int check_devicefiles()
{
   for (int i=0; i<complistcnt; i++)
   {
      if (complistarray[i]->devicefile_written)
			continue;

      fprintf(flog, "No device file created for Component [%s] Package [%s] Device [%s]\n",
         complistarray[i]->compname, complistarray[i]->geomname, complistarray[i]->devicename);
      display_error++;
   }

   return 1;
}

/******************************************************************************
* write_a_device
*/
static void write_a_device(FILE *fp, const char *typname, BlockStruct *block)
{
   CString geomname = check_name('s', block->getName());

   vb99_plusident();
   fprintfHKP(fp, "%sName \"%s\"\n", ident, check_name('e', typname));
   fprintfHKP(fp, "%sTopCell \"%s\"\n", ident, geomname);
   fprintfHKP(fp, "%sBottomCell \"\"\n", ident);
   fprintfHKP(fp, "%sRefPrefix \"%s\"\n", ident, "X"); // need to resolve this !!!

   comppininstarray.SetSize(100, 100);
   comppininstcnt = 0;

   int pincnt = VB99_ReadPinData( &(block->getDataList())); 

   fprintfHKP(fp, "%sSwapGroup   \"gate\"\n", ident);
   vb99_plusident();
	int i=0;
   for (i=0; i<pincnt; i++)
      fprintfHKP(fp, "%sSwapID \"P%d\"\n", ident, i);

   vb99_minusident();
   fprintfHKP(fp, "%sSlots\n", ident);
   vb99_plusident();
   fprintfHKP(fp, "%sSlot_SwapGroup \"gate\"\n", ident);
   vb99_plusident();

   for (i=0; i<pincnt; i++)
      fprintfHKP(fp, "%sPinNumber \"%s\"\n", ident, check_name('d', comppininstarray[i]->pinname));

   vb99_minusident();
   vb99_minusident();

   for (i=0; i<comppininstcnt; i++)
      delete comppininstarray[i];
   comppininstarray.RemoveAll();

   vb99_minusident();
   complist_device_written(block->getName(), typname);
   fprintfHKP(fp, "\n");

   return;
}

/******************************************************************************
* do_devices
*/
static int do_devices(FileStruct *file, const char *fname)
{
   FILE *fp;
   if ((fp = fopen(fname, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", fname);
      ErrorMessage(tmp);
      return -1;
   }
   else if (encryptOutputFlag)
   {
      fclose(fp);
      exportFileStream.SetOutputStream(fname);
   }

   pdb_created_by(fp, "ASCII_PDB");

   POSITION typPos = file->getTypeList().GetHeadPosition();
   while (typPos)
   {                                         
      TypeStruct *typ = file->getTypeList().GetNext(typPos);
      if (typ->getBlockNumber() < 0)
			continue;
      BlockStruct *block = doc->Find_Block_by_Num(typ->getBlockNumber());

      if (block->getBlockType() == BLOCKTYPE_MECHCOMPONENT  || block->getBlockType() == BLOCKTYPE_GENERICCOMPONENT )
         continue;
      fprintfHKP(fp, "%sNumber \"%s\"\n", ident, check_name('e', typ->getName()));

      write_a_device(fp, typ->getName(), block);
   }

   // here do all blocks which do not have a device.
   for (int i=0; i<complistcnt; i++)
   {
      if (complistarray[i]->devicefile_written)
			continue;
      fprintfHKP(fp, "%sNumber \"%s\"\n", ident, complistarray[i]->devicename);

      BlockStruct *block = doc->Find_Block_by_Name(complistarray[i]->geomname, -1);
      write_a_device(fp, complistarray[i]->devicename, block);
   }

   if (encryptOutputFlag)
      exportFileStream.Close();
   else
      fclose(fp);

   return 1;
}

/******************************************************************************
* check_pcbcomponents
*/
static int check_pcbcomponents(FileStruct *file)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)							continue;
      if (block->getFlags() & BL_WIDTH)            continue;
      if (block->getFlags() & BL_APERTURE)         continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
      if (block->getFlags() & BL_FILE)             continue;
      if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT && block->getBlockType() != BLOCKTYPE_TESTPOINT )
         continue;

      int pincnt = VB99_CountPinData(&(block->getDataList()));   
      if (pincnt == 0)
      {
         fprintf(flog, "PCB Component definition [%s] found without any pins -> changed to GENERIC.\n", block->getName());
         display_error++;
         block->setBlockType(BLOCKTYPE_GENERICCOMPONENT);
      }

   } 

   return 1;
}

/******************************************************************************
* do_geometries
*/
static int do_geometries(FileStruct *file, const char *fname)
{
   FILE  *fp;
   if ((fp = fopen(fname,"wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]",fname);
      ErrorMessage(tmp);
      return -1;
   }
   else if (encryptOutputFlag)
   {
      fclose(fp);
      exportFileStream.SetOutputStream(fname);
   }

   created_by(fp, "CELL_LIBRARY");

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)							continue;
      if (block->getFlags() & BL_WIDTH)				continue;
      if (block->getFlags() & BL_APERTURE)			continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
      if (block->getFlags() & BL_FILE)					continue;


      switch (block->getBlockType())
      {
         // These block types are written to cell.hkp
      case blockTypeUnknown:
      case blockTypeDrawing: 
      case blockTypeGenericComponent:
      case blockTypeMechComponent:
      case blockTypeTestPoint:   
      case blockTypePcbComponent:    
         {           
            WriteCellBlock(block, fp);
         }
         break;

		default:
         // Other block types not written to cell.hkp
			break;
      }
   }

   if (encryptOutputFlag)
      exportFileStream.Close();
   else
      fclose(fp); 

   return 1;
}

/******************************************************************************
* cellTypeTagToCellTypeName
*/
CString cellTypeTagToCellTypeName(CellTypeTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case packageCell:     retval = "PACKAGE_CELL";     break;
   case mechanicalCell:  retval = "MECHANICAL_CELL";  break;
   case drawingCell:     retval = "DRAWING_CELL";     break;
   }

   return retval;
}

/******************************************************************************
* getSMDPadstackSurface
*/

static ETestSurface getSMDPadstackSurface(BlockStruct* padstackGeometry)
{
   bool hasBotLayer = false, hasTopLayer = false;
   if(padstackGeometry && is_attvalue(doc, padstackGeometry->getAttributesRef(), ATT_SMDSHAPE, 1))
   {
      // Traverse SMD Padstacks and check if any pads on Top/Bottom Side 
       for (CDataListIterator padIterator(*padstackGeometry);padIterator.hasNext();)
       {
          DataStruct* pad = padIterator.getNext();
          LayerStruct* layer = doc->getLayerAt(pad->getLayerIndex());
          if(layer)
          {
             if(layer->isElectricalTop())
                return testSurfaceTop;                
             else if(layer->isElectricalBottom())
                return testSurfaceBottom; 
          }
       }
   }

   return testSurfaceUnset; 
}

/******************************************************************************
* isEdgeConnector
*/
static bool isEdgeConnector(BlockStruct *compGeometry)
{
   // Traverse SMD pins pf the SMD Component Geometry
   if(compGeometry && is_attvalue(doc, compGeometry->getAttributesRef(), ATT_SMDSHAPE, 1))
   {
      bool hasTopLayer = false, hasBotLayer = false;
      for (CDataListIterator pinIterator(*compGeometry,insertTypePin);pinIterator.hasNext();)
      {
         //Traverse SMD pins and check if pins have both top and bottom padstacks
         DataStruct* pin = pinIterator.getNext();
         if(is_attvalue(doc, compGeometry->getAttributesRef(), ATT_SMDSHAPE, 1))
         {
            ETestSurface surface = getSMDPadstackSurface(doc->getBlockAt(pin->getInsert()->getBlockNumber()));
            
            if(pin->getInsert()->getPlacedTop())
            {
               if(surface == testSurfaceTop)
                  hasTopLayer = true;
               else if(surface == testSurfaceBottom)
                  hasBotLayer = true;
            }
            else if(pin->getInsert()->getPlacedBottom())
            {
               if(surface == testSurfaceTop)
                  hasBotLayer = true;
               else if(surface == testSurfaceBottom)
                  hasTopLayer = true;
            }

            if(hasTopLayer && hasBotLayer)
               return true;
         }
      }   
   }

   return false;
}

/******************************************************************************
* WriteCellBlock
*/
static void WriteCellBlock(BlockStruct *block, FILE *fp )
{
   Attrib *a;
   update_cellwritten(block->getName());
   CString geomname = check_name('s', block->getName());

   fprintfHKP(fp, "\n");

   CellTypeTag cellType = undefinedCellType;
   BlockTypeTag blockType = block->getBlockType();

   switch(blockType)
   {
   case blockTypePcbComponent:      cellType = packageCell;     break;
   case blockTypeTestPoint:         cellType = packageCell;     break;
   case blockTypeMechComponent:     cellType = mechanicalCell;  break;
   case blockTypeGenericComponent:  cellType = mechanicalCell;  break;
   case blockTypeDrawing:           cellType = drawingCell;     break;
   case blockTypeUnknown:           cellType = drawingCell;     break;
   }

   if( testPointGeomBlockSet.find( block->getBlockNumber() ) != testPointGeomBlockSet.end() ){
      cellType = packageCell;
   }

   CString cellTypeName = cellTypeTagToCellTypeName(cellType);

   fprintfHKP(fp, "%s%s \"%s\"\n", ident,(const char*)cellTypeName, check_name('s', block->getName()));
   vb99_plusident();
   fprintfHKP(fp, "%sNUMBER_LAYERS %d\n\n", ident, max_signalcnt);

   if (cellType == packageCell)
   {
      if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
         fprintfHKP(fp, "%sMOUNT_TYPE SURFACE\n", ident);
      else if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_BLINDSHAPE, 1))
         fprintfHKP(fp, "%sMOUNT_TYPE MIXED\n", ident);
      else
         fprintfHKP(fp, "%sMOUNT_TYPE THROUGH\n", ident);

      if (block->getBlockType() == BLOCKTYPE_TESTPOINT)
         fprintfHKP(fp, "%sPACKAGE_GROUP TEST_POINT\n", ident);
      else if(isEdgeConnector(block))
         fprintfHKP(fp, "%sPACKAGE_GROUP EDGE_CONNECTOR\n", ident);
      else
         fprintfHKP(fp, "%sPACKAGE_GROUP GENERAL\n", ident);
   }

   if (cellType != drawingCell)
   {
      double comp_height = 0;
      a =  is_attvalue(doc, block->getAttributesRef(), ATT_COMPHEIGHT, 1);
      if (a)
      {
         CString l = get_attvalue_string(doc, a);
         comp_height = atof(l) * file->getScale() * unitsFactor;
      }

      // placement outline
      int found = VB99_WriteGeomOutlineData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0,  
         file->getScale() * unitsFactor, -1, "PLACEMENT_OUTLINE", graphicClassComponentBoundary, comp_height, true);  

      // try it to find a closed primary outline
      if (!found)
      {
         found = VB99_WriteGeomOutlineData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0,   
            file->getScale() * unitsFactor, -1, "PLACEMENT_OUTLINE", graphicClassComponentOutline, comp_height, true);   
      }
   }

   // other graphic
   VB99_WriteCellData(fp, cellType, &(block->getDataList()), 0.0, 0.0, 0.0, 0, file->getScale() * unitsFactor, -1, 0);  

   if (cellType != drawingCell)
      int pincnt = VB99_WritePackagePinData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, file->getScale() * unitsFactor, block->getName());  

   double scale = file->getScale() * unitsFactor;
   Attrib *attrib;

   if (block->getAttributesRef() != NULL)
   {
      for (POSITION pos=block->getAttributes()->GetStartPosition(); pos!=NULL;)
      {
         WORD key;
         block->getAttributes()->GetNextAssoc(pos, key, attrib);
         if (attrib != NULL)
         {
            CString keyString = doc->getKeyword(key)->getOutKeyword();
            if (keyString.CompareNoCase(ATT_REFNAME) != 0)
            {
               CString valueString = get_attvalue_string(doc, attrib);
               const char *l = Layer_VB99(attrib->getLayerIndex());
               double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
               vb99_celltexttype(fp, FALSE, "PROPERTY_PAIR", keyString, valueString, attrib->getX() * scale,
			         attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
               
               if( attrib->isVisible() )
               {
                  CString texttype = "USER_TYPE";
                  if (keyString.CompareNoCase(ATT_TYPELISTLINK) == 0 && cellType == packageCell)
                     texttype = "PARTNO";

                  // DR 679133 - Do not put PARTNO on testpoints, doing so makes Expedition think the testpoint
                  // is a schematic related item. Since it it not in schematic, this in turn causes testpoints to
                  // lose their net association. If this attrib has turned into texttype PARTNO then output
                  // only for PCB_COMPONENT, skip completely for TESTPOINT.
                  //
                  // If (it is not PARTNO) OR (type is PCB Component)) ... then write attrib
                  if (texttype.Compare("PARTNO") != 0 || blockType == blockTypePcbComponent)
                  {
                     vb99_celltexttype(fp, TRUE, texttype, "", valueString, attrib->getX() * scale,
                        attrib->getY() * scale, attrib->getRotationRadians(), 
                        attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                  }
               }
            }
         }
      }
   }

   if (attrib =  is_attvalue(doc, block->getAttributesRef(), ATT_REFNAME, 1))
   {
      // must be a layer
      const char *l = Layer_VB99(attrib->getLayerIndex());
      if (l == NULL) 
         l = "SILKSCREEN_MNT_SIDE";

      double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;

      CString texttype = "USER_TYPE";
      if (cellType == packageCell)
         texttype = "REF_DES";

      vb99_celltexttype(fp, attrib->isVisible(), texttype, "", "REF_DES", attrib->getX() * scale,
			attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
   }
   else
   {
      const char *l = "SILKSCREEN_MNT_SIDE";
      double penWidth = doc->getWidthTable()[0]->getSizeA()*scale;
      
      CString texttype = "USER_TYPE";
      if (cellType == packageCell)
         texttype = "REF_DES";

      vb99_celltexttype(fp, 1, texttype, "", "REF_DES", 0.0, 0.0, 0.0, 20.0 * one_mil, 15.0 * one_mil, l, penWidth);
   }

   vb99_minusident();
   fprintfHKP(fp,"\n");
}

/******************************************************************************
* do_placementoutline
*/
static int do_placementoutline(FILE *fp, BlockStruct* block, double insert_x, double insert_y, 
												double rotation, int mirror, double scale)
{
   double comp_height = 0;
   Attrib *attrib =  is_attvalue(doc, block->getAttributesRef(), ATT_COMPHEIGHT, 1);
   if (attrib)
   {
      CString l = get_attvalue_string(doc, attrib);
      comp_height = atof(l) * file->getScale() * unitsFactor;
   }

    // placement outline
   int found = VB99_WriteGeomOutlineData(fp, &(block->getDataList()), insert_x, insert_y, rotation,  mirror,
      scale, -1, "PLACEMENT_OUTLINE", graphicClassComponentBoundary, comp_height, false);  

   // try it to find a closed primary outline
   if (!found)
   {
      found = VB99_WriteGeomOutlineData(fp, &(block->getDataList()), insert_x, insert_y, rotation,  mirror,
         scale, -1, "PLACEMENT_OUTLINE", graphicClassComponentOutline, comp_height, false);   
   }

   return found;
}

/******************************************************************************
* VB99_WriteCOMPONENTData
*/
static void VB99_WriteCOMPONENTData(FILE *wfp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y, 
												double rotation, int mirror, double scale,int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);
      if (data->getDataType() != dataTypeInsert || data->getInsert() == NULL)
         continue;

		InsertStruct *insert = data->getInsert();
      if (insert->getInsertType() == insertTypeVia
         || insert->getInsertType() == insertTypePin
         || insert->getInsertType() == insertTypeTestPoint)
         continue;   

      BlockStruct *block = doc->getBlockAt(insert->getBlockNumber());
      if( block->getBlockType() == blockTypeTestPoint )
         continue;

      if (is_cellwritten(block->getName()) < 0)
         continue;

      CString compname = insert->getRefname();
      int facement = TRUE;


      fprintfHKP(wfp, "\n");
      if (block->getBlockType() == blockTypePcbComponent)
      {
         fprintfHKP(wfp, "%sPACKAGE_CELL \"%s\"\n", ident, check_name('s', block->getName()));
      }
      else if (block->getBlockType() == blockTypeMechComponent || block->getBlockType() ==  blockTypeGenericComponent)
      {
         fprintfHKP(wfp, "%sMECHANICAL_CELL \"%s\"\n", ident, check_name('s', block->getName()));
      }
      else
      {
         fprintfHKP(wfp, "%sDRAWING_CELL \"%s\"\n", ident, check_name('s', block->getName()));
         facement = FALSE;
      }
      vb99_plusident();


      point2.x = insert->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = insert->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);
      vb99_xy(wfp, point2.x, point2.y);


      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ insert->getMirrorFlags();
      double block_rot = rotation + insert->getAngle();
      if (block_mirror)
         block_rot = block_rot - PI;   // 

      fprintfHKP(wfp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(block_rot)));
      fprintfHKP(wfp, "%sCOMPONENT_OPTIONS NONE\n", ident);

      if (facement)
         fprintfHKP(wfp, "%sFACEMENT %s\n", ident, (block_mirror)?"BOTTOM":"TOP");

      if (block->getBlockType() == blockTypePcbComponent || block->getBlockType() == blockTypeTestPoint)
      {
         fprintfHKP(wfp, "%sPLACEMENT_LYR %s\n", ident, (block_mirror)?bottomsignallayer:topsignallayer);

         //output Pin/Net
         POSITION blockDataPos = block->getDataList().GetHeadPosition();
         while (blockDataPos)
         {
            DataStruct *blockData = block->getDataList().GetNext(blockDataPos);
            if (blockData->getDataType() != dataTypeInsert || blockData->getInsert() == NULL)
               continue;

				InsertStruct *blockInsert = blockData->getInsert();
            if (blockInsert->getInsertType() != insertTypePin)
               continue;   
            CString pinName = blockInsert->getRefname();


            // Find the net
            NetStruct *net = NULL;
            CompPinStruct *compPin = NULL;
            BOOL netFound = FALSE;
            
            POSITION netPos = file->getNetList().GetHeadPosition();
            while (netPos && !netFound)
            {
               net = file->getNetList().GetNext(netPos);
               POSITION compPinPos = net->getHeadCompPinPosition();
               while (compPinPos && !netFound)
               {
                  compPin = net->getNextCompPin(compPinPos);
                  if (!strcmp(compPin->getRefDes(), compname) && !strcmp(compPin->getPinName(), pinName)) 
                     netFound = TRUE;
               }
            }


            //output pin name
            fprintfHKP(wfp, "%sPIN \"%s\"\n", ident, pinName);
            vb99_plusident();

            //output net name
            // Dean added check_name to following NETNAME 03/21/03
            if (netFound)
               fprintfHKP(wfp, "%sNETNAME \"%s\"\n", ident, check_name('n', net->getNetName()));
            else
               fprintfHKP(wfp, "%sNETNAME \"Unconnected_Net\"\n", ident);
            netFound = FALSE;


            Mat2x2 m1;
            RotMat2(&m1, rotation + data->getInsert()->getAngle());

            Point2 point3;
            double pinRot = blockData->getInsert()->getAngle();
            point3.x = blockData->getInsert()->getOriginX() * scale;
            point3.y = blockData->getInsert()->getOriginY() * scale;

            if (block_mirror)
            {
               point3.x = -point3.x;
               pinRot = pinRot - PI;   // 
            }
            TransPoint2(&point3, 1, &m1, point2.x, point2.y);
            
            vb99_xy(wfp, point3.x, point3.y);
            fprintfHKP(wfp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(block_rot + pinRot)));


            //output component_options
            BlockStruct *pinBlock = doc->getBlockAt(blockInsert->getBlockNumber());
				CString pinPadstackName = "";
            int v = get_padstack_index(pinBlock->getName());

            if (v > -1)
            {
               if (padstackarray[v]->oppside && padstackarray[v]->smd)                 
                  fprintfHKP(wfp,"%sPIN_OPTIONS OPP_SIDE\n", ident);
               else
                  fprintfHKP(wfp,"%sPIN_OPTIONS NONE\n", ident);

					pinPadstackName = padstackarray[v]->compPadname;
            }
            else
				{
               fprintfHKP(wfp,"%sPIN_OPTIONS NONE\n", ident);
					pinPadstackName = pinBlock->getName();
				}
            
            // Dean added check_name to following PADSTACK 03/21/03				
            fprintfHKP(wfp, "%sPADSTACK \"%s\"\n", ident, check_name('p', pinPadstackName));
            vb99_minusident();
         }

         // Add placement Outline
         if (block->getBlockType() == blockTypePcbComponent)
         {
            do_placementoutline(wfp, block, point2.x, point2.y, block_rot, block_mirror, scale);
         }


         Attrib *attrib;
         if (attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_REFNAME, 1))
         {
            // must be a layer
            int al = doc->get_layer_mirror(attrib->getLayerIndex(), block_mirror);
            const char *l = Layer_VB99(al);

            if (l == NULL) 
            {
               if (block_mirror)
                  l = "SILKSCREEN_BOTTOM";
               else
                  l = "SILKSCREEN_TOP";
            }

            Mat2x2 m1;
            RotMat2(&m1, rotation + data->getInsert()->getAngle());

            Point2 point3;
            double arot = attrib->getRotationRadians();
            point3.x = attrib->getX() * scale;
            point3.y = attrib->getY() * scale;

            if (block_mirror)
            {
               point3.x = -point3.x;
               arot = arot - PI; // 
            }
            TransPoint2(&point3, 1, &m1, point2.x, point2.y);

            double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;

            vb99_texttype(wfp, attrib->isVisible(), "REF_DES", "", compname, point3.x, point3.y, block_rot+arot, 
                  block_mirror, attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
         }
         else
         {
            double penWidth = doc->getWidthTable()[0]->getSizeA()*scale;
            const char *l = "SILKSCREEN_TOP";

            if (block_mirror)
               l = "SILKSCREEN_BOTTOM";

            vb99_texttype(wfp, FALSE, "REF_DES", "", compname, point2.x, point2.y, block_rot, mirror, 
                  0.0, 0.0, l, penWidth);
         }

         CompList *complist = new CompList;
         complistarray.SetAtGrow(complistcnt++, complist);  
         complist->compname = compname;
         complist->geomname = block->getName();
         complist->devicefile_written = FALSE;

         // DR 679133 - Do not write PARTNO for TESTPOINTs.
         // PARTNO should be set only on items that would be found in the Schematic, and Testpoints
         // are PCB-Only, not in Schematic. Having PARTNO on Testpoints causes Expedition to
         // lose the association with a net. Odd, but that's the way it is in EE2007.9.

         if (block->getBlockType() != blockTypeTestPoint)
         {
            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1))
            {
               const char *l = Layer_VB99(attrib->getLayerIndex());
               const char *p = get_attvalue_string(doc, attrib);

               if (l == NULL) // there must be a refdes for components
               {
                  double penWidth = doc->getWidthTable()[0]->getSizeA()*scale;
                  vb99_texttype(wfp, FALSE, "PARTNO", "", check_name('e', p), 0.0, 0.0, 0.0, 0, 0.0, 0.0, "", penWidth);
               }
               else
               {
                  double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
                  vb99_texttype(wfp, attrib->isVisible(), "PARTNO", "", check_name('e', p) , attrib->getX() * scale, attrib->getY()*scale, 
                     attrib->getRotationRadians(), mirror, attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
               }
               complist->devicename = p;
            }
            else
            {
               double penWidth = doc->getWidthTable()[0]->getSizeA()*scale;
               vb99_texttype(wfp, FALSE, "PARTNO", "", block->getName(), 0.0, 0.0, 0.0, 0, 0.0, 0.0, "", penWidth);
               complist->devicename = block->getName();
            }
         }

         if (data->getAttributesRef() != NULL)
         {
            for (POSITION pos=data->getAttributes()->GetStartPosition(); pos!=NULL;)
            {
               WORD key;
               data->getAttributes()->GetNextAssoc(pos, key, attrib);
               if (attrib != NULL)
               {
                  CString keyString = doc->getKeyword(key)->getOutKeyword();
                  // These two already specifically handled above, output the rest
                  if (keyString.CompareNoCase(ATT_REFNAME) != 0 && keyString.CompareNoCase(ATT_TYPELISTLINK) != 0)
                  {
                     CString valueString = get_attvalue_string(doc, attrib);
                     double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
                     const char *l = Layer_VB99(attrib->getLayerIndex());

                     Mat2x2 m1;
                     double compRot = rotation + data->getInsert()->getAngle();
                     RotMat2(&m1, compRot);

                     Point2 point3;
                     double attrRot = attrib->getRotationRadians();
                     point3.x = attrib->getX() * scale * (block_mirror?-1:1);
                     point3.y = attrib->getY() * scale;
                     attrRot = compRot + attrRot * (block_mirror?-1:1); 

                     TransPoint2(&point3, 1, &m1, point2.x, point2.y);

                     vb99_texttype(wfp, FALSE, "PROPERTY_PAIR", keyString, valueString, point3.x,
			               point3.y, normalizeRadians(attrRot), block_mirror, attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                     if( attrib->isVisible() )
                     {                      
                        // dts0100634284 - To support mirror for visible text, 
                        // display text by vb99_texttype instead of vb99_celltexttype   
                        vb99_texttype(wfp, TRUE, "USER_TYPE", "", valueString, point3.x,
                           point3.y, normalizeRadians(attrRot), block_mirror, attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                     }
                  }
               }
            }
         }
       } 


      vb99_minusident();      
   } 

   return;
}

/******************************************************************************
* VB99_WriteKeyinPart
*/
static void VB99_WriteKeyinPart(FILE *wfp, CDataList *DataList)
{
   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      // As of dts0100679133, testpoints are left out on purpose. Output only pcb components.
      if (np->isInsertType(insertTypePcbComponent))
      {
         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
         if (block->getBlockType() == blockTypeMechComponent || block->getBlockType() == blockTypeGenericComponent )
            continue;

         CString compname = np->getInsert()->getRefname();
         Attrib *a;
         if (a = is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
         {
            const char *p = get_attvalue_string(doc, a);
            CString c = check_name('c', compname);
            fprintfHKP(wfp, "\\%s\\\t\\%s\\\n", check_name('e', p), c);
         }
         else
         {
            CString c = check_name('c', compname);
            fprintfHKP(wfp, "\\%s\\\t\\%s\\\n", check_name('e', block->getName()), c);
         }
      }
   } 

   return;
}

/******************************************************************************
* VB99_GetROUTESData
*/
static void VB99_GetROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   int layer;
   int layerptr;   // pointer into vbArr layer index.

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      POSITION tmpPos = pos; // pos is changed in getnext
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() == T_TEXT)
			continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // must be either a net or on electrical layer 
         if ((layerptr = get_layerptr_from_index(layer)) < 0)
				continue;

         if (vbArr[layerptr]->stackNum < 1)
				continue;
      }

      // here check if ATTR_NETNAME == netname
      Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

      // here store it
      VB99Routes *r = new VB99Routes;
      routesarray.SetAtGrow(routescnt++, r);  
      r->pos = tmpPos;

      if (a == NULL)
			r->netname = "(Net0)";
      else
         r->netname = get_attvalue_string(doc, a);
   } 
}

//******************************************************************************

static bool isDataOnElectricalLayer(CCEtoODBDoc *doc, DataStruct *data)
{
   if (doc != NULL && data != NULL)
   {
      LayerStruct* lp( doc->getLayer( data->getLayerIndex() ) );

      if( lp != NULL )
      {
         if( lp->getElectricalStackNumber() ){
            return true;
         }
      }
   }

   return false;
}

//******************************************************************************

static void fillTestPointGeomBlockSet( CNetList* NetList )
{
   testPointGeomBlockSet.clear();
   POSITION netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

      CCompPinList &pins( net->getCompPinList() );
      POSITION pos( pins.getHeadPosition() );
      while( pos ) {
         CompPinStruct *pinStruct( pins.getNext( pos ) );
         DataStruct *data( file->FindInsertData( pinStruct->getRefDes(), insertTypeTestPoint ) );

         if( data == NULL )
            continue;

         if (data->getDataType() != dataTypeInsert || data->getInsert() == NULL)
            continue;

         InsertStruct *insert = data->getInsert();
         BlockStruct *block( doc->getBlockAt( insert->getBlockNumber() ) );

         if( block ){
            testPointGeomBlockSet.insert( block->getBlockNumber() );
         }
      }
   }
}

//******************************************************************************

static bool hasNetNameAttrib(CCEtoODBDoc *doc, DataStruct *data)
{
   if (doc != NULL && data != NULL)
   {
      if( data->getAttributesRef() != NULL )
      {
         Attrib* attr( NULL );
         WORD NetNameNum( (WORD)doc->getStandardAttributeKeywordIndex(standardAttributeNetName) );
         if( data->getAttributes()->Lookup( NetNameNum, attr ) )
         {
            return true;
         }
      }
   }

   return false;
}

//******************************************************************************

static int shrinkShapePolys(CCEtoODBDoc *doc, DataStruct *data, CPolyList& polylist, double overallScale)
{
   if (doc == NULL || data == NULL)
      return 0;

   int cnt = 0; // total point count
   
   bool circleFlag(false);

   POSITION polyPos = polylist.GetHeadPosition();

   while (polyPos)
   {
      CPoly *poly = polylist.GetNext(polyPos);
      //No reason to skip hidden   if (poly->isHidden())		continue;
      //No reason to skip void     if (poly->isVoid())  continue;

      //Old 
      //bool doShrink( dataIsOnPlaneLayer );
      //if ( poly->isClosed() || poly->isFilled() ){
      //   doShrink = false;         //it's route not pour, so don't resize it
      //}
      
      // Some route types override camcad data filled flag.
      // Deterimine route type to get closed and filled overrides
      bool closed = poly->isClosed();
      bool filled = poly->isFilled();
      bool voided = poly->isVoid();
      VB99RouteType routeType = DeterminePolyRouteType(doc, data, closed, filled, voided);

      bool needsShrink( IsShapeGraphic(circleFlag, closed, filled) );

      if( needsShrink )
      {
         // Case dts0100475106 - polys that are "negative" shrink to inside edge of thick line,
         // all else shrinks (expands, really) to outside edge of thick lines.
         // Negative shrinkValue causes "expand".

         // Default to expand, that will be most common.
         double shrinkValue( ( doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*overallScale ) / -2.0 );

         // If on Power Negative layer then really shrink.
         LayerStruct *lp = doc->getLayer(data->getLayerIndex());
         if (lp != NULL && lp->getLayerType() == layerTypePowerNegative)
         {
            shrinkValue = -shrinkValue;
         }

         poly->shrink( shrinkValue, doc->getPageUnits() );
      }

      cnt += poly->getPntList().GetCount();
   }

   return cnt;
}

/******************************************************************************
* VB99_WriteROUTESData
*/
static void VB99_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   for (int i=0; i<routescnt; i++)
   {
      // different netname
      if (strcmp(routesarray[i]->netname, netname))
         continue; 

      DataStruct *data = DataList->GetNext(routesarray[i]->pos);

      int layerNum;
      const char *layerName;

      if (data->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layerNum = insertLayer;
         else
            layerNum = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layerNum, mirror))
            continue;

         if ((layerName = Layer_VB99(layerNum)) == NULL) 
            continue;
         if (strlen(layerName) == 0)
            continue;
      }

      switch (data->getDataType())
      {
      case T_POLY:
         {

            double lineWidth;

            // The decision to shrink or not depends solely on whether the poly will
            // be written as a SHAPE or a PATH. The PATH has a width parameter, so we can specify
            // the width and be done. The SHAPE does not have a WIDTH parameter, so if the
            // boundary of the poly has non-zero width, we have to expand the poly to
            // account for the line width.  (Shrink() does this with negative shirnk factor.)

            // So all the stuff about layer type does not matter.
            // And the test for closed and filled is backwards.
            // This is all conjecture at this point, so the old code is left in
            // evidence, and the new code is activated.

            // See VB99_WriteROUTESData for reference when SHAPE versus PATH is written.

            bool circleFlag(false);

            CPolyList tempPolyList( *data->getPolyList() );
            const int polylistPtCnt( shrinkShapePolys(doc, data, tempPolyList, scale) );

            Point2 *points = (Point2 *)calloc(polylistPtCnt, sizeof(Point2));
            
            POSITION polyPos = tempPolyList.GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = tempPolyList.GetNext(polyPos);
               //No reason to skip hidden   if (poly->isHidden())    continue;
               //No reason to skip void  if (poly->isVoid())  continue;  
               
               bool closed = poly->isClosed();
               bool filled = poly->isFilled();
               bool voided = poly->isVoid();
               lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;                     
               int cnt = 0;

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);

						Point2 p2;
                  p2.x = pnt->x * scale;
                  if (mirror)
							p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  if (cnt)
                  {
                     if (fabs(points[cnt-1].x - p2.x) > vb99_smallnumber || fabs(points[cnt-1].y - p2.y) > vb99_smallnumber)
                     {
                        points[cnt] = p2;
                        cnt++;
                     }
                     else
                     {
								// need to take bulge from second point
                        points[cnt-1].bulge = p2.bulge;
                     }
                  }
                  else                 
                  {
                     points[cnt] = p2;
                     cnt++;
                  }
               }

               if (cnt > 1)
               {

                  int icnt = identcnt;

                  VB99RouteType routeType = DeterminePolyRouteType(doc, data, closed, filled, voided );

                  switch (routeType)
                  {
                  case vb99RouteTypeObstruct:
                     {
                        VB99ObstructData *obstruct = new VB99ObstructData;
                        obstruct->points = (Point2 *)calloc(cnt, sizeof(Point2));
                        memcpy( obstruct->points, points, cnt*sizeof(Point2) );

                        obstruct->circle = circleFlag;
                        obstruct->closed = closed;
                        obstruct->cnt = cnt;
                        obstruct->layerName = layerName;
                        obstruct->lineWidth = lineWidth;
                        obstruct->polyFilled = filled;
                        obstruct->voidout = voided;
                        obstructsArray.Add( obstruct );

                        continue;
                     }
                     break;

                  case vb99RouteTypeConductiveArea:
                     {
                        fprintfHKP(wfp, "%sCONDUCTIVE_AREA\n", ident);
                        vb99_plusident();
                        fprintfHKP(wfp, "%sROUTE_LYR %s\n", ident, layerName);
                        fprintfHKP(wfp, "%sCONDUCTIVE_AREA_OPTIONS PLANE\n", ident);
                        fprintfHKP(wfp, "%sHATCH_PATTERN DEFAULT\n", ident);
                     }
                     break;

                  case vb99RouteTypeTrace:
                     {
                        fprintfHKP(wfp, "%sTRACE\n", ident);
                        vb99_plusident();
                        fprintfHKP(wfp, "%sROUTE_LYR %s\n", ident, layerName);
                        fprintfHKP(wfp, "%sTRACE_OPTIONS NONE\n", ident);
                     }
                     break;

                  default:
                     // no op
                     break;
                  }

                  // If section intro was written in switch above, then write body here
                  if (routeType != vb99RouteTypeUnknown)
                  {
                     add_polyline(wfp, points, cnt, filled, closed, voided, lineWidth, circleFlag);
                     fprintfHKP(wfp, "\n");   // just for better reading.

                     // get back idents 
                     for (int i=icnt; i<identcnt; i++)
                        vb99_minusident();
                  }
               }
            } 

            free(points);
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         // no text in route section
         break;
         
      case T_INSERT:
         {
            if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;
            if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
               break;

            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();
            if (block_mirror)
               block_rot = block_rot - PI;

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (data->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               int padstackIndex = get_padstack_index(block->getName());

               if (padstackIndex < -1)
                  break;

               VB99Padstack *padstack = padstackarray[padstackIndex];

               fprintfHKP(wfp, "%sVIA\n", ident);
               vb99_plusident();
               vb99_xy(wfp, point2.x, point2.y);
               fprintfHKP(wfp, "%sVIA_OPTIONS NONE\n", ident);
               fprintfHKP(wfp, "%sLAYER_PAIR %s %s\n", ident, LayerNameFromStacknum(padstack->startlayernumber), LayerNameFromStacknum(padstack->endlayernumber));
               fprintfHKP(wfp, "%sPADSTACK \"%s\"\n", ident, check_name('p', padstack->vianame));

               vb99_minusident();
               break;
            }
            
            if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT) // this is only done if create_tp_comps
            {
               fprintf(flog, "Test points in Net [%s] not supported.\n", netname);
               display_error++;
               break;
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else if (!data->getInsert()->getInsertType()) // not access point or other stuff
            {
               // hierachies in nets are not supported.
               fprintf(flog, "Hierachical Net [%s] not supported.\n", netname);
               display_error++;
            } 
         }
         break;
      }
   }
}

/******************************************************************************
* do_routes
*/
static int do_routes(FILE *fp, FileStruct *file,CNetList *NetList)
{
   // this gets all routes data and writes net0 which is all traces on electrical layers without
   // a netname
   VB99_GetROUTESData(fp, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   // unconnected net 
   fprintfHKP(fp, "%sNET \"%s\"\n", ident, "(Net0)");
   vb99_plusident();

   // first net is unconnected net
   VB99_WriteROUTESData(fp, &(file->getBlock()->getDataList()), 
      file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
      file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, "(Net0)");

   vb99_minusident();
   fprintfHKP(fp, "\n");

   Mat2x2 m;
   Point2 point2;
   RotMat2( &m, file->getRotation() );

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)
			continue;

      fprintfHKP(fp, "%sNET \"%s\"\n", ident, check_name('n', net->getNetName()));
      vb99_plusident();

      CCompPinList &pins( net->getCompPinList() );
      POSITION pos( pins.getHeadPosition() );
      while( pos ) {

         CompPinStruct *pinStruct( pins.getNext( pos ) );
         DataStruct *data( file->FindInsertData( pinStruct->getRefDes(), insertTypeTestPoint ) );

         if( data == NULL )
            continue;

         if (data->getDataType() != dataTypeInsert || data->getInsert() == NULL)
            continue;

         InsertStruct *insert = data->getInsert();
         BlockStruct *block( doc->getBlockAt( insert->getBlockNumber() ) );
         fprintfHKP(fp, "%sTESTPOINT \"%s\"\n", ident, check_name('s', block->getName()));

         vb99_plusident();

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror =  (int)file->isMirrored() ^ insert->getMirrorFlags();
         double block_rot = file->getRotation() + insert->getAngle();
         if (block_mirror)
            block_rot = block_rot - PI;
         double scale( file->getScale() );

         fprintfHKP(fp, "%sFACEMENT %s\n", ident, (block_mirror)?"BOTTOM":"TOP");

         POSITION pos2( block->getHeadDataInsertPosition() );
         while( pos2 ){
            DataStruct *stackData( block->getNextDataInsert( pos2 ) );
            InsertStruct *stack( stackData->getInsert() );
            if( stack->getRefname().Compare( pinStruct->getPinName() ) )
               continue;

            BlockStruct *stackBlock( doc->getBlockAt( stack->getBlockNumber() ) );
            if( stackBlock != NULL )
               fprintfHKP(fp, "%sPADSTACK \"%s\"\n", ident, stackBlock->getName() );
         }

         fprintfHKP(fp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(block_rot)));

         point2.x = insert->getOriginX() * scale;
         if( file->isMirrored() )
            point2.x = -point2.x;
         point2.y = insert->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());
         vb99_xy(fp, point2.x, point2.y);

         fprintfHKP(fp, "%sCOMPONENT_OPTIONS NONE\n", ident);

         if (data->getAttributesRef() != NULL)
         {
            for (POSITION pos=data->getAttributes()->GetStartPosition(); pos!=NULL;)
            {
               WORD key;
               Attrib *attrib(NULL);
               data->getAttributes()->GetNextAssoc(pos, key, attrib);
               if (attrib != NULL)
               {
                  CString keyString = doc->getKeyword(key)->getOutKeyword();
                  CString valueString = get_attvalue_string(doc, attrib);
                  double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
                  const char *l = Layer_VB99(attrib->getLayerIndex());
                  if( keyString.CompareNoCase(ATT_REFNAME) == 0 )
                  {
                     vb99_celltexttype(fp, TRUE, "REF_DES", "", valueString, attrib->getX() * scale,
                        attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                  } 
                  else if ( keyString.CompareNoCase(ATT_TYPELISTLINK) == 0 )
                  {
                     // DR 679133 Do not write PARTNO for testpoints, it causes Expedition (EE2007.9 tested) to
                     // lose net associativity. Just skip the TYPELISTLINK attrib completely.
                     // Left commented out here, as a warning not to put it back!
                     // NO!  vb99_celltexttype(fp, TRUE, "PARTNO", "", valueString, attrib->getX() * scale,
                     // NO!     attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                  } 
                  else 
                  {
                     vb99_texttype(fp, FALSE, "PROPERTY_PAIR", keyString, valueString, attrib->getX() * scale,
                        attrib->getY() * scale, attrib->getRotationRadians(), file->isMirrored(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                     if( attrib->isVisible() ){
                        vb99_celltexttype(fp, TRUE, "USER_TYPE", "", valueString, attrib->getX() * scale,
                           attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                     }
                  }
               }
            }
         }

         vb99_minusident();
      }

      // order by signal name
      VB99_WriteROUTESData(fp,&(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, net->getNetName());

      vb99_minusident();
      fprintfHKP(fp, "\n");
   }
   fprintfHKP(fp, "\n");

   return 1;
}

/******************************************************************************
* VB99_OutlineBOARDData
*/
static int VB99_OutlineBOARDData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int gr_class, int layer_class)
{
   int layer;
   int found = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() == T_TEXT) 
			continue;

      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         if (np->getGraphicClass() == graphicClassEtch)
				continue;

         LayerStruct *ll = doc->FindLayer(layer);
         if (gr_class && np->getGraphicClass() != gr_class)  
            continue;
         else if (layer_class && layer_class != ll->getLayerType())
            continue;

         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)  continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)        continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)        continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)       continue;

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
			Point2 point2;
         point2.x = np->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         double block_rot = rotation + np->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;
            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               block_layer = insertLayer;
            else
               block_layer = np->getLayerIndex();

            found += VB99_OutlineBOARDData(wfp,&(block->getDataList()), point2.x, point2.y, block_rot, block_mirror,
                               scale * np->getInsert()->getScale(), embeddedLevel+1, block_layer, gr_class, layer_class);
         } 
      }
   } 

   return found;
}

/******************************************************************************
* write_easyoutline
*/
static int write_easyoutline(FILE *wfp, CPntList *pp, const char *layer)
{
   if (pp == NULL)
      return 0;

   int cnt = 0;
   POSITION pos = pp->GetHeadPosition();
   while (pos)
   {
      CPnt *p = pp->GetNext(pos);
      cnt++;
   }

   Point2 *points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
   cnt = 0;

   pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      CPnt *p = pp->GetNext(pos);
      points[cnt].x = p->x;
      points[cnt].y = p->y;
      points[cnt].bulge = p->bulge;
      cnt++;
   }

   int icnt = identcnt;
   fprintfHKP(wfp, "%s%s\n", ident, layer);
   vb99_plusident();

   add_polyline(wfp, points, cnt, 0, 0, 0, 0, FALSE); // used in boardoutline and routing outline, not closed
   fprintfHKP(wfp,"\n");   // just for better reading.

   for (int i=icnt; i<identcnt; i++)
      vb99_minusident();

   return 1;
}


/******************************************************************************
* do_boardoutline
*/
static int do_boardoutline(FILE *wfp, FileStruct *file, double accuracy)
{
   // board_routing_outline
   Outline_Start(doc);

   int found = VB99_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 
					file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, file->getRotation(),  file->isMirrored(), 
					file->getScale() * unitsFactor, 0, -1, graphicClassBoardOutline, 0);

   if (!found)
   {
      // now look for board outline
      found = VB99_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 
					file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, file->getRotation(),  file->isMirrored(), 
					file->getScale() * unitsFactor, 0, -1, 0, LAYTYPE_BOARD_OUTLINE);
   }

   if (found)
   {
      int returnCode;
      if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "BOARD_OUTLINE"))
      {
         fprintf(flog, "Error in easy outline for [%s]\n", file->getBlock()->getName());
         display_error++;
      }
   }
   else
   {
      // do board extens
      fprintfHKP(wfp, "%sBOARD_OUTLINE\n", ident);
      vb99_plusident();
      fprintfHKP(wfp, "%sPOLYLINE_PATH\n", ident);
      vb99_plusident();
      fprintfHKP(wfp, "%sWIDTH 0\n", ident);

      double xmin = file->getBlock()->getXmin() * file->getScale() * unitsFactor;
      double ymin = file->getBlock()->getYmin() * file->getScale() * unitsFactor;
      double xmax = file->getBlock()->getXmax() * file->getScale() * unitsFactor;
      double ymax = file->getBlock()->getYmax() * file->getScale() * unitsFactor;

      fprintfHKP(wfp, "%sXY (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xmin, output_units_accuracy, ymin);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmax, output_units_accuracy, ymin);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmax, output_units_accuracy, ymax);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmin, output_units_accuracy, ymax);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmin, output_units_accuracy, ymin);
      vb99_minusident();
      vb99_minusident();
   }

   Outline_FreeResults();
   return 1;
}

/******************************************************************************
* do_obstructs
*/

static int do_obstructs( FILE *wfp )
{
   for( int i = 0 ; i < obstructsArray.GetCount() ; i++ )
   {
      VB99ObstructData *obstr = obstructsArray[i]; //obstr( obstructsArray[i] );

      if( obstr->polyFilled )
      {
         fprintfHKP(wfp, "%sOBSTRUCT PLANE\n", ident);
      } else {
         fprintfHKP(wfp, "%sOBSTRUCT TRACE\n", ident);
      }
      vb99_plusident();
      fprintfHKP(wfp, "%sROUTE_LYR %s\n", ident, obstr->layerName);

      add_polyline(wfp, obstr->points, obstr->cnt, obstr->polyFilled, obstr->closed,
                        obstr->voidout, obstr->lineWidth, obstr->circle);
      fprintfHKP(wfp, "\n");   // just for better reading.

      vb99_minusident();
   }

   obstructsArray.RemoveAll();

   return 1;
}

/******************************************************************************
* do_fiducials
*/
static int do_fiducialsAndTooling(FILE *wfp, FileStruct *file)
{
   CDataList* datas( &(file->getBlock()->getDataList()) );
   POSITION pos = datas->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = datas->GetNext(pos);
      if( np->getDataType() != T_INSERT ) continue;
      switch(np->getInsert()->getInsertType() )
      {
      case INSERTTYPE_FIDUCIAL:
         {
            InsertStruct* ins( np->getInsert() );

            fprintfHKP(wfp,"%sFIDUCIAL\n", ident);
            vb99_plusident();

            vb99_xy(wfp, ins->getOriginX(), ins->getOriginY() );
            fprintfHKP(wfp,"%sROTATION %1.*lf\n", ident, ins->getAngleDegrees() );
            if( ins->getGraphicMirrored() )
            {
               fprintfHKP(wfp,"%sFACEMENT BOTTOM\n", ident );
            } else {
               fprintfHKP(wfp,"%sFACEMENT TOP\n", ident );
            }
            fprintfHKP(wfp,"%sFIDUCIAL_OPTIONS NONE\n", ident );

            BlockStruct *block = doc->Find_Block_by_Num(ins->getBlockNumber());
            CString padstackName( check_name('p', block->getName() ) );
            fprintfHKP(wfp,"%sPADSTACK \"%s\"\n", ident, padstackName );

            vb99_minusident();
            fprintfHKP(wfp,"\n");
         }
         break;
      case INSERTTYPE_TOOLING:
         {
            InsertStruct* ins( np->getInsert() );

            fprintfHKP(wfp,"%sMOUNTING_HOLE\n", ident);
            vb99_plusident();

            //check if it has netName
            CString netName("Unconnected_Net");
            if( np->getAttributesRef()!= NULL )
            {
               Attrib* attr( NULL );
               WORD NetNameNum( (WORD)doc->getStandardAttributeKeywordIndex(standardAttributeNetName) );
               if( np->getAttributes()->Lookup( NetNameNum, attr ) )
               {
                  netName = attr->getStringValue();
               }
            }
            fprintfHKP(wfp,"%sNETNAME \"%s\"\n", ident, netName );
            fprintfHKP(wfp,"%sMOUNTING_HOLE_OPTIONS NONE \n", ident );

            BlockStruct *block = doc->Find_Block_by_Num(ins->getBlockNumber());
            fprintfHKP(wfp,"%sPADSTACK \"%s\"\n", ident, block->getName() );

            fprintfHKP(wfp,"%sROTATION %1.*lf\n", ident, ins->getAngleDegrees() );
            vb99_xy(wfp, ins->getOriginX(), ins->getOriginY() );

            vb99_minusident();
            fprintfHKP(wfp,"\n");
         }
         break;
      }
   }
   return 1;
}

/******************************************************************************
* do_routingoutline
*/
static int do_routingoutline(FILE *wfp, FileStruct *file, double accuracy)
{
   // board_routing_outline
   Outline_Start(doc);

   int found = VB99_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()),
      file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, file->getRotation(),  file->isMirrored(), 
      file->getScale() * unitsFactor, 0, -1, graphicClassRouteKeepIn, 0);

   if (!found)
   {
      // now look for board outline
      found = VB99_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 
					file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, file->getRotation(),  file->isMirrored(), 
					file->getScale() * unitsFactor, 0, -1, graphicClassBoardOutline, 0);

      if (!found)
      {
         found = VB99_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 
						file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, file->getRotation(),  file->isMirrored(), 
						file->getScale() * unitsFactor, 0, -1, 0, LAYTYPE_BOARD_OUTLINE);
      }
   }

   if (found)
   {
      int returnCode;
      if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "ROUTE_OUTLINE"))
      {
         fprintf(flog, "Error in easy outline for [%s]\n", file->getBlock()->getName());
         display_error++;
      }
   }
   else
   {
      // do board extens
      fprintfHKP(wfp, "%sROUTE_OUTLINE\n", ident);
      vb99_plusident();
      fprintfHKP(wfp, "%sPOLYLINE_PATH\n", ident);
      vb99_plusident();
      fprintfHKP(wfp, "%sWIDTH 0\n", ident);

      double xmin = file->getBlock()->getXmin() * file->getScale() * unitsFactor;
      double ymin = file->getBlock()->getYmin() * file->getScale() * unitsFactor;
      double xmax = file->getBlock()->getXmax() * file->getScale() * unitsFactor;
      double ymax = file->getBlock()->getYmax() * file->getScale() * unitsFactor;

      fprintfHKP(wfp, "%sXY (%1.*lf, %1.*lf)\n", ident, output_units_accuracy, xmin, output_units_accuracy, ymin);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmax, output_units_accuracy, ymin);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmax, output_units_accuracy, ymax);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmin, output_units_accuracy, ymax);
      fprintfHKP(wfp, "%*s   (%1.*lf, %1.*lf)\n", strlen(ident), " ", output_units_accuracy, xmin, output_units_accuracy, ymin);

      vb99_minusident();
      vb99_minusident();
   }

   Outline_FreeResults();
   return 1;
}

/******************************************************************************
* do_layout
*/
static int do_layout(FileStruct *file, const char *fname, double accuracy) 
{
   FILE  *fp;
   
   if ((fp = fopen(fname,"wt")) == NULL)
   {
      CString  tmp;
      tmp.Format("Can not open file [%s]",fname);
      ErrorMessage(tmp);
      return -1;
   }
   else if (encryptOutputFlag)
   {
      fclose(fp);
      exportFileStream.SetOutputStream(fname);
   }

   created_by(fp, "LAYOUT");
   fprintfHKP(fp, "%sMIGRATED_DESIGN\n", ident);
   //fprintfHKP(fp, "%sMIGRATED_DESIGN\n", ident);

   //clear obstruct array
   obstructsArray.RemoveAll();

   do_routes(fp, file, &file->getNetList());

   VB99_WriteCOMPONENTData(fp, file, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   // need to do board_outline
   do_boardoutline(fp, file, accuracy);

   // need to do fiducials and toolings
   do_obstructs( fp );

   // need to do fiducials and toolings
   do_fiducialsAndTooling(fp, file );

   // need to do route_outline
   do_routingoutline(fp, file, accuracy);

   VB99_WriteLayoutData(fp, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, 
         file->getRotation(),  file->isMirrored(), file->getScale() * unitsFactor, -1, 0); 

   if (encryptOutputFlag)
      exportFileStream.Close();
   else
      fclose(fp); // close layout

   return 1;
}

/******************************************************************************
*/
static bool isTestPointCompPin(FileStruct *file, CompPinStruct *cp)
{
   if (cp != NULL)
   {
      DataStruct *data( file->FindInsertData( cp->getRefDes(), insertTypeTestPoint ) );
      // Since Find() is requesting specific insertTypeTestPoint, if data
      // is found then cp is a testpoint comppin, if not found then it isn't.
      return (data != NULL);
   }

   return false; // Is not a comppin at all, so not a testpoint comppint
}

/******************************************************************************
*/
static int do_keyin_netlist(FileStruct *file, const char *fname)
{
   FILE *fp;
   if ((fp = fopen(fname, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]",fname);
      ErrorMessage(tmp);
      return -1;
   }

   int cnt = 0;
   fprintfHKP(fp, ";; V4.1.0\n");
   fprintfHKP(fp, "\n");
   fprintfHKP(fp, "%%%%PART\n");

   // device/typelist
   VB99_WriteKeyinPart(fp, &(file->getBlock()->getDataList()));

   fprintfHKP(fp, "\n");
   fprintfHKP(fp, "%%%%NET\n");

   int nusedpincnt = 0;
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);
      if ((net->getFlags() & NETFLAG_UNUSEDNET))
			continue;
   
      cnt = fprintfHKP(fp, "\\%s\\ ", check_name('n', net->getNetName()));

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         // DR 679133 - Do NOT write TestPoints to keyin netlist file
         if (!isTestPointCompPin(file, compPin))
         {
            cnt++;
         }
      }

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         // DR 679133 - Do NOT write TestPoints to keyin netlist file
         if (!isTestPointCompPin(file, compPin))
         {
            if (cnt > 50)
               cnt = fprintfHKP(fp, "\n*  ");

            cnt += fprintfHKP(fp, " \\%s\\", check_name('c', compPin->getRefDes()));
            cnt += fprintfHKP(fp, "-\\%s\\",check_name('d', compPin->getPinName()));
         }
      }

      if (cnt)
         fprintfHKP(fp, "\n");
   }
   fclose(fp);

   return 1;
}

/******************************************************************************
* do_netclass
*/
static int do_netclass(FileStruct *file, const char *fname)
{
   FILE  *fp;
   if ((fp = fopen(fname, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]",fname);
      ErrorMessage(tmp);
      return -1;
   }
   else if (encryptOutputFlag)
   {
      fclose(fp);
      exportFileStream.SetOutputStream(fname);
   }

   created_by(fp, "NETCLASS_FILE");

   //here we should add the GLOBAL_CKEARANCES section.
   fprintfHKP(fp, ".GLOBAL_CLEARANCES\n");																			 //! DRC clearances (previously stored in design)
   fprintfHKP(fp, " ..DIFF_PAIR_TRACE_TO_TRACE %1.*lf\n", output_units_accuracy, globalClear);      //! Differential Pair Trace to Trace clearance
   fprintfHKP(fp, " ..CON_MHOLE_TO_MHOLE %1.*lf\n", output_units_accuracy, globalClear);            //! Contour/ mounting hole to mounting hole 
   fprintfHKP(fp, " ..CON_MHOLE_TO_NONPLANE_CONDUCT %1.*lf\n", output_units_accuracy, globalClear); //! Contour/ mounting hole to nonplane conductor
   fprintfHKP(fp, " ..CON_MHOLE_TO_PLANE_CONDUCT %1.*lf\n", output_units_accuracy, globalClear);    //! Contour/ mounting hole to plane conductor
   fprintfHKP(fp, " ..PAD_TO_PAD %1.*lf\n", output_units_accuracy, globalClear);                    //! Pad to Pad Clearance
   fprintfHKP(fp, " ..TRACE_TO_RESISTOR %1.*lf\n", output_units_accuracy, globalClear);             //! Trace to Resistor Clearance
   fprintfHKP(fp, " ..PAD_TO_RESISTOR %1.*lf\n", output_units_accuracy, globalClear);               //! Pad to Resistor Clearance 
   fprintfHKP(fp, " ..COMPONENT_TO_COMPONENT %1.*lf\n", output_units_accuracy, globalClear);        //! Component to Component Clearance 
   fprintfHKP(fp, " ..COMPONENT_TO_BOARD_EDGE %1.*lf\n", output_units_accuracy, globalClear);       //! Component to Board Edge Clearance 
   fprintfHKP(fp, " ..COMPONENT_TO_KEEPOUT %1.*lf\n", output_units_accuracy, globalClear);          //! Component to Placement Keep Out Clearance
   fprintfHKP(fp, " ..TESTPOINT_TO_COMPONENT %1.*lf\n", output_units_accuracy, globalClear);        //! Test Point to Component Clearance 
   fprintfHKP(fp, " ..TESTPOINT_TO_TESTPOINT %1.*lf\n", output_units_accuracy, globalClear);        //! Test Point to Test Point Clearance

   // here do layer map
   fprintfHKP(fp, "%sLAYER_MAP\n", ident);
   vb99_plusident();

   for (int i=1; i<=max_signalcnt; i++)
   {
      for (int l=0; l<maxArr; l++)
      {
         if (vbArr[l]->stackNum != i)
				continue;

         fprintfHKP(fp, "%sMAP \"%s\"\n", ident, vbArr[l]->newName); 
         vb99_plusident();
         fprintfHKP(fp, "%sLAYER_NUM %d\n", ident, i);
         vb99_minusident();
         break;   // only do one
      }
   }

   vb99_minusident();
   if (encryptOutputFlag)
      exportFileStream.Close();
   else
      fclose(fp);
   return 1;
}

/******************************************************************************
* do_netprops
*/
static int do_netprops(FileStruct *file, const char *fname)
{
   FILE  *fp;
   if ((fp = fopen(fname, "wt")) == NULL)
   {
      CString  tmp;
      tmp.Format("Can not open file [%s]",fname);
      ErrorMessage(tmp);
      return -1;
   }
   else if (encryptOutputFlag)
   {
      fclose(fp);
      exportFileStream.SetOutputStream(fname);
   }

   created_by(fp, "NETPROPERTIES_FILE");

   int unusedpincnt = 0;
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);
      if ((net->getFlags() & NETFLAG_UNUSEDNET))		
         continue;

      //Changes for DR dts0100523583
      CString NetClassName = "(Default)";
      CString NetClassNameFromAttrib;
      int NetClassNameKeywordIndex = -1;
      NetClassNameKeywordIndex = doc->IsKeyWord("NETCLASS",0);
      if((NULL != net->getAttributes()) && ( -1 != NetClassNameKeywordIndex))
      {
         if(doc->getAttributeStringValue(NetClassNameFromAttrib,*(net->getAttributes()),NetClassNameKeywordIndex))
         {
            NetClassName = NetClassNameFromAttrib;
         }
      }

      fprintfHKP(fp, "%sNETCLASS \"%s\"\n", ident, NetClassName);
      vb99_plusident();
      fprintfHKP(fp, "%sNETNAMES \"%s\"\n", ident, check_name('n', net->getNetName()));
      vb99_minusident();
      fprintfHKP(fp, "%sNETNAME \"%s\"\n", ident, check_name('n', net->getNetName()));
      vb99_plusident();
      fprintfHKP(fp, "%sORDER_TYPE Free\n", ident);
      fprintfHKP(fp, "%sPINS\n", ident);
      vb99_plusident();
   
      int cnt = 0;
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         cnt++;
      }

      fprintfHKP(fp, "%sCOUNT %d\n", ident, cnt);

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         fprintfHKP(fp, "%sREF_PIN \"%s-", ident, check_name('c', compPin->getRefDes()));
         fprintfHKP(fp, "%s\"\n", check_name('d', compPin->getPinName()));
         fprintfHKP(fp, "%sTYPE S\n", ident);
      }
      vb99_minusident();
      vb99_minusident();
   }

   if (encryptOutputFlag)
      exportFileStream.Close();
   else
      fclose(fp);
   return 1;
}

/******************************************************************************
* update_vianames
*/
static int update_vianames()
{
   // if there is no via on the board, must make one.
   int viacnt = 0;
   for (int i=0; i<padstackcnt; i++)
   {
      if (padstackarray[i]->used_as_via)
			viacnt++;
   }

   if (!viacnt)
   {
      for (int i=0; i<padstackcnt; i++)
      {
         if (padstackarray[i]->smd)
				continue;
         padstackarray[i]->used_as_via = true;  // force a via on the first non smd
			padstackarray[i]->vianame.Format("%s_VIA", padstackarray[i]->padname);
         fprintf(flog, "Padstack [%s] marked as Via -> no other via use found\n", padstackarray[i]->padname);
         break;
      }
   }

   return 1;
}

/******************************************************************************
* VB99_WritePackagePinData
*/
int VB99_WritePackagePinData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, const char *cellname)
{
   Mat2x2 m;
   Point2 point2;
   int pincnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = DataList->GetNext(pos);
      if (data->getDataType() != dataTypeInsert || data->getInsert() == NULL)
			continue;

		// when adding more condition here, add also at CreatePadstackArrayFromBlock 
		InsertStruct *insert = data->getInsert();
      if (insert->getInsertType() != insertTypePin			&& insert->getInsertType() != insertTypeMechanicalPin &&
			 insert->getInsertType() != insertTypeDrillTool && insert->getInsertType() != insertTypeDrillHole && 
			 insert->getInsertType() != insertTypeFreePad)
				continue;


      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      point2.x = insert->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = insert->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      BlockStruct *block = doc->getBlockAt(insert->getBlockNumber());
      int v = get_padstack_index(block->getName());

      // block->getRotation() is the aperture rotation 
      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ insert->getMirrorFlags();
      double block_rot = rotation + insert->getAngle();
		if (mirror)
			block_rot = (rotation + insert->getAngle()) - PI;   // add 180 degree if mirrored.
      

      if (insert->getInsertType() == insertTypePin)
      {
         // here get pinname
         CString pinname = insert->getRefname();
         if (strlen(pinname) == 0)
         {
            fprintf(flog, "Pinname not found\n");
            display_error++;
         }

         if (mirror)
         {
            fprintf(flog, "Pinmirror in Cell [%s] not implemented\n", cellname);
            display_error++;
         
         }

		 fprintfHKP(wfp, "%sPIN \"%s\"\n", ident, check_name('d', pinname));
         vb99_plusident();
         vb99_xy(wfp, point2.x, point2.y);
         fprintfHKP(wfp, "%sNETNAME \"Net-%d\"\n", ident, pincnt+1);


         // special SMD bottom only treatment.
			CString pinPadstackName = "";
         if (v > -1)
         {
            if (padstackarray[v]->oppside && padstackarray[v]->smd)
               fprintfHKP(wfp,"%sPIN_OPTIONS OPP_SIDE\n", ident);
            else
               fprintfHKP(wfp,"%sPIN_OPTIONS NONE\n", ident);

				pinPadstackName = padstackarray[v]->compPadname;
			}
         else
			{
            fprintfHKP(wfp,"%sPIN_OPTIONS NONE\n", ident);
				pinPadstackName = block->getName();
			}


         fprintfHKP(wfp, "%sPADSTACK \"%s\"\n", ident, check_name('p', pinPadstackName));
         fprintfHKP(wfp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(block_rot)));
         vb99_minusident();
         pincnt++;
      }
      else if (insert->getInsertType() == INSERTTYPE_MECHANICALPIN || insert->getInsertType() == INSERTTYPE_TOOLING ||
					insert->getInsertType() == INSERTTYPE_DRILLHOLE || insert->getInsertType() ==  INSERTTYPE_FREEPAD)
      {
			CString padstackName = "";
         if (v > -1)
				padstackName = padstackarray[v]->mountingPadname;
         else
				padstackName = block->getName();

         fprintfHKP(wfp, "%sMOUNTING_HOLE\n", ident);
         vb99_plusident();
         vb99_xy(wfp, point2.x, point2.y);
         fprintfHKP(wfp, "%sPADSTACK \"%s\"\n", ident, check_name('p', padstackName));
         fprintfHKP(wfp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(block_rot)));
         vb99_minusident();
      }
   } 

   return pincnt;
} 

/******************************************************************************
* VB99_CountPinData
*/
int VB99_CountPinData(CDataList *DataList)
{
   int pincnt = 0;
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;

      if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         pincnt++;
   } 

   return pincnt;
} 

/******************************************************************************
* VB99_WriteCellData
*/
int VB99_WriteCellData(FILE *wfp,CellTypeTag cellType,CDataList *DataList, double insert_x, double insert_y,
							  double rotation, int mirror, double scale, int insertLayer, int layclass)
{
   Mat2x2      m;
   Point2      point2;
   int         layer;
   int         layerclass;
   const char  *l;
   int         skip; // this is used because layermapping and class mapping is done.

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = DataList->GetNext(pos);
      if (data->getDataType() != T_INSERT)
      {
         if (data->getGraphicClass() == graphicClassEtch)      continue;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))  continue;

         if ((l = Layer_VB99(layer)) == NULL)   
            continue;

         // if no graphic class, get it unless there is i.e a ROUTING_KEEPOUT in a component defintion
         if (data->getGraphicClass() != 0)
            layerclass = data->getGraphicClass();
         else
         {
            layerclass = layclass;
            LayerStruct *l = doc->FindLayer(layer);
            if (l && !l->isFloating())     layerclass = l->getLayerType();
         }
      }

      skip = FALSE;

      switch(data->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            double   lineWidth;
            Point2 *points;
            int    cnt = 0;

            // count thru polys
            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;   // according to Veribest, do not write voids -- it kills the 
                                                // processing of Veribest

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;

               bool polyFilled = poly->isFilled();
               bool closed     = poly->isClosed();
               bool voided     = poly->isVoid();

               lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                     
               double   cx, cy, radius;
               bool      circle = false;
               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  Point2 p2;
                  lineWidth = 0;
                  p2.x = cx * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = cy * scale;
                  p2.bulge = radius*scale;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  circle = true;
                  points[0] = p2;
               }
               else
               {
                  Point2 p2;
                  cnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p2.x = pnt->x * scale;
                     if (mirror) p2.x = -p2.x;
                     p2.y = pnt->y * scale;
                     p2.bulge = pnt->bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);
                     if (cnt)
                     {
                        if (fabs(points[cnt-1].x - p2.x) > vb99_smallnumber ||
                            fabs(points[cnt-1].y - p2.y) > vb99_smallnumber)
                        {
                           points[cnt] = p2;
                           cnt++;
                        }
                     }
                     else                 
                     {
                        points[cnt] = p2;
                        cnt++;
                     }                 // here deal with bulge
                  }
               }

               if (cnt < 2)   continue;

               int   icnt = identcnt;

               if ((!STRNICMP(l,"Silkscreen",4) || data->getGraphicClass() == graphicClassComponentOutline)&& cellType != drawingCell)
            {
               fprintfHKP(wfp,"%sSILKSCREEN_OUTLINE\n", ident);
               vb99_plusident();
               if (layerclass != LAYTYPE_SILK_BOTTOM)
               fprintfHKP(wfp,"%sSIDE MNT_SIDE\n", ident);
               else
               fprintfHKP(wfp,"%sSIDE OPP_SIDE\n", ident);
            }
            else if (!STRNICMP(l,"Assembly",8)&&cellType != drawingCell)
            {
               fprintfHKP(wfp,"%sASSEMBLY_OUTLINE\n", ident);
               vb99_plusident();
            }
            else if (layerclass == LAYTYPE_BOARD_OUTLINE)
            {
               skip = TRUE;   // not allowed in cell
            }
            else if(data->getGraphicClass() == graphicClassBoardOutline)
            {
               skip = TRUE;   // not allowed in cell
            }
            else
            {
               if (strlen(l) == 0)
                  skip = TRUE;
               else
               {
                  // normal graphic
                  fprintfHKP(wfp,"%sGRAPHIC\n", ident);
                  vb99_plusident();
                  fprintfHKP(wfp,"%sUSER_LYR \"%s\"\n", ident, l);
               }
            }

               if (!skip)
               {
                  if (!polyFilled)  closed = FALSE;   // on non electrical shapes (graphicClassEtch is skipped here)
                                                      // never use a SHAPE option
                  add_polyline(wfp, points, cnt, polyFilled, closed, voided, lineWidth, circle);
                  fprintfHKP(wfp,"\n");   // just for better reading.
               }

               // get back idents 
               for (int i=icnt;i<identcnt;i++)
               {
                  vb99_minusident();
               }
            }  // while

            free(points);
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         {
            point2.x = (data->getText()->getPnt().x)*scale;
            point2.y = (data->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + data->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = data->getText()->isMirrored();

            double penWidth = doc->getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA()*scale;

            if (!STRNICMP(l,"Silkscreen",4))
            {
               if (layerclass != LAYTYPE_SILK_BOTTOM)
                  l = "SILKSCREEN_MNT_SIDE";
               else
                  l = "SILKSCREEN_OPP_SIDE";
            }
            else
            if (!STRNICMP(l,"Assembly",4))
            {
               l = "ASSEMBLY";
            }
            else
            {
               // MNT_LYR, OPP_LYR, ALL_LYRS, REL_LYR_n for stackup
               CString bottom;
               bottom.Format("LYR_%d", max_signalcnt);
               if (!STRCMPI(l,"LYR_1"))   
                  l = "MNT_LYR";
               else
               if (!STRCMPI(l,bottom)) 
                  l = "OPP_LYR";
            }
            vb99_celltexttype(wfp, TRUE, "USER_TYPE", "", data->getText()->getText(), point2.x, point2.y, text_rot, 
                        data->getText()->getHeight()*scale, data->getText()->getWidth()*scale, l, penWidth);
            fprintfHKP(wfp,"\n");   // just for better reading.
         }
         break;
      case T_INSERT:
         {
            if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)            continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)      continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)  continue;

               // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = data->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            if (mirror)
               block_rot = (rotation + data->getInsert()->getAngle()) - PI;   // add 180 degree if mirrored.

            BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());

            if (data->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
            {
               fprintfHKP(wfp,"%sFIDUCIAL\n", ident);
               vb99_plusident();          
               //output x, y
               vb99_xy(wfp, point2.x, point2.y);

					CString padstackName = "";
					int v = get_padstack_index(block->getName());
					if (v > -1)
						padstackName = padstackarray[v]->fidPadname;
					else
						padstackName = block->getName();

               if (layerclass != LAYTYPE_SILK_BOTTOM)
                  fprintfHKP(wfp,"%sSIDE MNT_SIDE\n", ident);
               else
                  fprintfHKP(wfp,"%sSIDE OPP_SIDE\n", ident);
               //output rotation
               fprintfHKP(wfp, "%sROTATION %1.3lf\n", ident, cnv_rot(RadToDeg(block_rot)));
               fprintfHKP(wfp, "%sPADSTACK \"%s\"\n", ident, check_name('p', padstackName));

               vb99_minusident();
               fprintfHKP(wfp,"\n");
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
            // here we should use generic parts
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else 
               if (!doc->IsFloatingLayer(data->getLayerIndex()))
                  block_layer = data->getLayerIndex();

               VB99_WriteCellData(wfp,undefinedCellType,&(block->getDataList()), 
                              point2.x, point2.y,
                              block_rot, block_mirror,
                              scale * data->getInsert()->getScale(), block_layer, layerclass);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch

      if (data->getAttributesRef() != NULL)
      {
         for (POSITION pos=data->getAttributes()->GetStartPosition(); pos!=NULL;)
         {
            WORD key;
            Attrib* attrib = NULL;
            data->getAttributes()->GetNextAssoc(pos, key, attrib);
            if (attrib != NULL)
            {
               CString keyString = doc->getKeyword(key)->getOutKeyword();
               if (keyString.CompareNoCase(ATT_REFNAME) != 0)
               {
                  CString valueString = get_attvalue_string(doc, attrib);
                  double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
                  const char *l = Layer_VB99(attrib->getLayerIndex());
                  vb99_celltexttype(wfp, FALSE, "PROPERTY_PAIR", keyString, valueString, attrib->getX() * scale,
			            attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                  if( attrib->isVisible() ){
                     vb99_celltexttype(wfp, TRUE, "USER_TYPE", "", valueString, attrib->getX() * scale,
                        attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                  }
               }
            }
         }
      }

   } // end for

   return 0;
} /* end VB99_WriteCellData */

//--------------------------------------------------------------
int VB99_WriteGeomOutlineData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, const char *layername, int grclass, double compheight, bool isCell)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   const char  *l;
   int         found = 0;
   int         layer;
   int         placement_outline = FALSE;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         //if (!doc->get_layer_visible(layer, mirror, np->getDataType()))  continue;

         if (np->getGraphicClass() ==  grclass)
         {
            // that is found
         }
         else
         {
            if ((l = Layer_VB99(layer)) == NULL)   
               continue;

            if (!STRCMPI(l, layername))
            {
               // that is found as a layer
            }
            else
            {
               continue;
            }
         }
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            double   lineWidth;
            Point2 *points;
            int    cnt = 0;

            if (np->getGraphicClass() != grclass)
               continue;
            else if (np->getPolyList()->GetCount() == 0)
               continue;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;   // according to Veribest, do not write voids -- it kills the 
                                                // processing of Veribest


               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;

               bool polyFilled = poly->isFilled();
               bool closed     = poly->isClosed();
               bool voided     = poly->isVoid();

               lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                     
               double   cx, cy, radius;
               bool     circle = false;
               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  Point2 p2;

                  p2.x = cx * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = cy * scale;
                  p2.bulge = radius*scale;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  circle = true;
                  points[0] = p2;
               }
               else
               {
                  Point2 p2;
                  cnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p2.x = pnt->x * scale;
                     if (mirror) p2.x = -p2.x;
                     p2.y = pnt->y * scale;
                     p2.bulge = pnt->bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);
                     if (cnt)
                     {
                        if (fabs(points[cnt-1].x - p2.x) > vb99_smallnumber ||
                            fabs(points[cnt-1].y - p2.y) > vb99_smallnumber)
                        {
                           points[cnt] = p2;
                           cnt++;
                        }
                     }
                     else                 
                     {
                        points[cnt] = p2;
                        cnt++;
                     }                 // here deal with bulge
                  }
               }

               if (!closed)   continue;

               if (!circle && cnt < 2)
                  continue;

               found++;
               int   icnt = identcnt;

               fprintfHKP(wfp,"%s%s\n", ident, layername);
               vb99_plusident();

               fprintfHKP(wfp,"%sHEIGHT %1.*lf\n",ident, output_units_accuracy, compheight);
               fprintfHKP(wfp,"%sUNDERSIDE_SPACE %1.*lf\n",ident, output_units_accuracy, 0.0);
               if(!isCell)
                  fprintfHKP(wfp,"%sSIDE_NAME \"Mnt\"\n",ident);
               else
                  fprintfHKP(wfp,"%sSIDE MNT_SIDE\n",ident);

               if (!polyFilled)  closed = FALSE;   // on non electrical shapes (graphicClassEtch is skipped here)
                                                   // never use a SHAPE option
               add_polyline(wfp, points, cnt, polyFilled, closed, voided, lineWidth, circle);
               fprintfHKP(wfp,"\n");   // just for better reading.

               // get back idents 
               for (int i=icnt;i<identcnt;i++)
               {
                  vb99_minusident();
               }
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         break;
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)      continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)  continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)        continue;

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = (rotation + np->getInsert()->getAngle()) - PI;  

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
               // here we should use generic parts
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               found += VB99_WriteGeomOutlineData(wfp, &(block->getDataList()), 
                            point2.x, point2.y,
                            block_rot, block_mirror,
                            scale * np->getInsert()->getScale(), block_layer, layername, grclass, compheight, isCell);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for

   return found;
} /* end VB99_WriteGeomOutlineData */

//--------------------------------------------------------------
int VB99_ComplexPadData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   int         written = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() == graphicClassEtch)      continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            double   lineWidth;
            Point2 *points;
            int    cnt = 0;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;   // according to Veribest, do not write voids -- it kills the 
                                                // processing of Veribest


               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;

               bool polyFilled = poly->isFilled();
               bool closed     = poly->isClosed();
               bool voided     = poly->isVoid();

               lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                     
               Point2 p2;
               cnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) 
                     p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  if (cnt)
                  {
                     if (fabs(points[cnt-1].x - p2.x) > vb99_smallnumber ||
                         fabs(points[cnt-1].y - p2.y) > vb99_smallnumber)
                     {
                        points[cnt] = p2;
                        cnt++;
                     }
                  }
                  else                 
                  {
                     points[cnt] = p2;
                     cnt++;
                  }                 // here deal with bulge
               }
               
               if (cnt < 2)   continue;

               int   icnt = identcnt;

               if (!polyFilled)  closed = FALSE;   // on non electrical shapes (graphicClassEtch is skipped here)
                                                   // never use a SHAPE option
               add_polyline(wfp, points, cnt, polyFilled, closed, voided, lineWidth, FALSE);
               written = TRUE;
               fprintfHKP(wfp,"\n");   // just for better reading.

               // get back idents 
               for (int i=icnt;i<identcnt;i++)
               {
                  vb99_minusident();
               }
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {

         }
         break;
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = (rotation + np->getInsert()->getAngle()) - PI;   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // complex pad has an aperture ????
               fprintf(flog, "Complex Pad has aperture ????\n");
               display_error++;
            }
            else // not aperture
            {
               written |= VB99_ComplexPadData(wfp, &(block->getDataList()), 
                            point2.x, point2.y, block_rot, block_mirror, scale * np->getInsert()->getScale());
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for

   return written;
} /* end VB99_ComplexPadData */

//--------------------------------------------------------------
int VB99_IsComplexPadData(CDataList *DataList)
{
   DataStruct  *np;
   int         found = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() == graphicClassEtch)      continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            int    cnt = 0;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;   // accroding to Veribest, do not wrtie voids -- it kills the 
                                                // processign of Veribest


               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while
         
            if (cnt < 2)   continue;
            found += cnt;

         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {

         }
         break;
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // complex pad has an aperture ????
               fprintf(flog, "Complex Pad has aperture ????\n");
               display_error++;
            }
            else // not aperture
            {
               found += VB99_IsComplexPadData(&(block->getDataList()));
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for

   return found;
} /* end VB99_IsComplexPadData */

//--------------------------------------------------------------
int VB99_WriteLayoutData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, int layclass)
{
   Mat2x2      m;
   Point2      point2;
   int         layer;
   int         layerclass;
   int         layerptr;      // pointer into vbarr (vbArr layer array)
   const char  *l;
   int         skip; // this is used because layermapping and class mapping is done.

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = DataList->GetNext(pos);
      if (data->getDataType() != T_INSERT)
      {
         if (data->getGraphicClass() == graphicClassBoardOutline) continue;
         if (data->getGraphicClass() == graphicClassEtch)         continue;
         if (data->getGraphicClass() == graphicClassRouteKeepIn)  continue;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))  continue;

         // no electrical 
         layerptr = get_layerptr_from_index(layer);

         if ((l = Layer_VB99(layer)) == NULL)   
            continue;

         // if no graphic class, get it unless there is i.e a ROUTING_KEEPOUT in a component defintion
         if (data->getGraphicClass() != 0)
            layerclass = data->getGraphicClass();
         else
         {
            layerclass = layclass;
            LayerStruct *l = doc->FindLayer(layer);
            if (l && !l->isFloating())     layerclass = l->getLayerType();
         }

         if (layerclass == LAYTYPE_BOARD_OUTLINE)  continue;
      }

      skip = FALSE;

      switch(data->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            double   lineWidth;
            Point2 *points;
            int    cnt = 0;

            // count thru polys
            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;   // accroding to Veribest, do not wrtie voids -- it kills the 
                                                // processign of Veribest

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               if (poly->isVoid())  continue;

               bool polyFilled = poly->isFilled();
               bool closed     = poly->isClosed();
               bool voided     = poly->isVoid();

               lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                     
               double   cx, cy, radius;
               bool     circle = false;
               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  Point2 p2;
                  lineWidth = 0;
                  p2.x = cx * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = cy * scale;
                  p2.bulge = radius*scale;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  circle = true;
                  points[0] = p2;
               }
               else
               {
                  Point2 p2;
                  cnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p2.x = pnt->x * scale;
                     if (mirror) 
                        p2.x = -p2.x;
                     p2.y = pnt->y * scale;
                     p2.bulge = pnt->bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (cnt)
                     {
                        if (fabs(points[cnt-1].x - p2.x) > vb99_smallnumber ||
                            fabs(points[cnt-1].y - p2.y) > vb99_smallnumber)
                        {
                           points[cnt] = p2;
                           cnt++;
                        }
                     }
                     else                 
                     {
                        points[cnt] = p2;
                        cnt++;
                     }
                  }
               }

               if (cnt < 2)   continue;

               int   icnt = identcnt;

               if (!STRNICMP(l,"Assembly",4))
               {
                  fprintfHKP(wfp,"%sASSEMBLY_OUTLINE\n", ident);
                  vb99_plusident();
               }
               else
               if (!STRNICMP(l,"Silkscreen",4) || data->getGraphicClass() == graphicClassComponentOutline)
               {
                  fprintfHKP(wfp,"%sSILKSCREEN_OUTLINE\n", ident);
                  vb99_plusident();
                  if (layerclass != LAYTYPE_SILK_BOTTOM)
                     fprintfHKP(wfp,"%sSIDE MNT_SIDE\n", ident);
                  else
                     fprintfHKP(wfp,"%sSIDE OPP_SIDE\n", ident);
               }
               else
               {
                  if (layerptr < 0)
                  {
                     skip = TRUE;
                  }
                  else
                  {
                     // in the graphic section, there is no etch layer graphic. This is all done in Net 0
                     if (vbArr[layerptr]->stackNum > 0)
                     {
                        skip = TRUE;
                     }
                     else
                     if (strlen(l) == 0)
                        skip = TRUE;
                     else
                     {
                        // normal graphic
                        fprintfHKP(wfp,"%sGRAPHIC\n", ident);
                        vb99_plusident();
                        fprintfHKP(wfp,"%sUSER_LYR \"%s\"\n", ident, l);
                     }
                  }
               }

               if (!skip)
               {
                  add_polyline(wfp, points, cnt, polyFilled, closed, voided, lineWidth, circle);
                  fprintfHKP(wfp,"\n");   // just for better reading.
               }
               // get back idents 
               for (int i=icnt;i<identcnt;i++)
               {
                  vb99_minusident();
               }
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (data->getText()->getPnt().x)*scale;
            point2.y = (data->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + data->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = data->getText()->isMirrored();

            double penWidth = doc->getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA()*scale;

            vb99_texttype(wfp, TRUE, "USER_TYPE", "", data->getText()->getText(), point2.x, point2.y, text_rot, 
                        text_mirror, data->getText()->getHeight()*scale, data->getText()->getWidth()*scale, l, penWidth);
            fprintfHKP(wfp,"\n");   // just for better reading.
         }
         break;
         case T_INSERT:
         {
            if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)            continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)      continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)  continue;
            if (data->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)        continue;

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = data->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            if (mirror)
               block_rot = (rotation + data->getInsert()->getAngle()) - PI;   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            fprintfHKP(wfp,"! Explode %s\n", block->getName());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
               // here we should use generic parts
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (data->getLayerIndex() != -1)
                  block_layer = data->getLayerIndex();

               VB99_WriteLayoutData(wfp, &(block->getDataList()), point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(), block_layer, layerclass);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch

      if (data->getAttributesRef() != NULL)
      {
         for (POSITION pos=data->getAttributes()->GetStartPosition(); pos!=NULL;)
         {
            WORD key;
            Attrib* attrib = NULL;
            data->getAttributes()->GetNextAssoc(pos, key, attrib);
            if (attrib != NULL)
            {
               CString keyString = doc->getKeyword(key)->getOutKeyword();
               if (keyString.CompareNoCase(ATT_REFNAME) != 0)
               {
                  CString valueString = get_attvalue_string(doc, attrib);
                  double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
                  const char *l = Layer_VB99(attrib->getLayerIndex());
                  vb99_texttype(wfp, FALSE, "PROPERTY_PAIR", keyString, valueString, attrib->getX() * scale,
			            attrib->getY() * scale, attrib->getRotationRadians(), mirror, attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                  if( attrib->isVisible() ){
                     vb99_celltexttype(wfp, TRUE, "USER_TYPE", "", valueString, attrib->getX() * scale,
                        attrib->getY() * scale, attrib->getRotationRadians(), attrib->getHeight()*scale, attrib->getWidth()*scale, l, penWidth);
                  }
               }
            }
         }
      }

   } // end for

   return 0;
} /* end VB99_WriteLayoutData */

/****************************************************************************/

VB99ObstructData::VB99ObstructData()
: points(NULL)
, cnt(0)
, lineWidth(0.0)
, polyFilled(false)
, closed(false)
, voidout(false)
, circle(false)
{
}

VB99ObstructData::VB99ObstructData(VB99ObstructData &other)
{
   points = NULL;

   if (other.cnt > 0)
   {
      this->points = (Point2 *)calloc(other.cnt, sizeof(Point2));
      memcpy( this->points, other.points, other.cnt*sizeof(Point2) );
   }

   this->cnt = other.cnt;
   this->lineWidth = other.lineWidth;
   this->layerName = other.layerName;
   this->polyFilled = other.polyFilled;
   this->closed = other.closed;
   this->voidout = other.voidout;
   this->circle = other.circle;
}

VB99ObstructData::~VB99ObstructData() 
{ 
   if (points != NULL) free(points);
}

/****************************************************************************/
/*
   end VB99out.CPP
*/
/****************************************************************************/


