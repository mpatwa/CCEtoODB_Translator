// $Header: /CAMCAD/5.0/read_wrt/Ag_AOI_o.cpp 97    6/21/07 8:28p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

// MV Technologies SJ10, SP2, FX10, MICROVIA

#include "stdafx.h"
#include "ccdoc.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "graph.h"
#include <math.h>
#include <float.h>
#include "export.h"
#include "find.h"
#include "ck.h"
#include "ag_aoi_o.h"
#include "centroid.h"
#include "Polygon.h"
#include "StandardAperture.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "Debug.h"
#include <set>
#include <map>
using std::map;
using std::set;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CProgressDlg *progress;
extern char         *shapes[];

static CCEtoODBDoc* doc;
static CCEtoODBDoc& getCamCadDoc() { return *doc; }

static double        unitsFactor;
static double getPlxUnitsFactor() { return unitsFactor; }

static int  display_error;
static FILE *flog;

static   PLXGeomArray   geomarray;
static   int         geomcnt;

static   PLXCompArray   comparray;

static   PLXPartArray   partarray;
static   int         partcnt;

static   PLXFiducialArray  fiducialarray;
static   int         fiducialcnt;

static   PLXRejectMarkArray  rejectmarkarray;

static   PLXPadstackArray  padstackarray; 
static   int         padstackcnt;

static   TerminalArray  terminalarray; // this is for padstacks in partdecal
static   int         terminalcnt;

static   int      s_rotationIncrement;
static   bool     s_rotationIncrementBool = false;
static   bool     s_addDebugGraphics      = false;
static	CString	s_PackageNameAttrib;
static   bool     s_ExportUnloadedParts = true;
static   bool     s_CreateTPPfiles = true;
static   bool     s_UseCentroidRotation = false;
static   bool     s_UseCentroidPosition = true;
static CString s_UnloadedCompShapeSuffix = "-np";
static CString s_UnloadedCompPNSuffix = "-np";
double s_GlobalRotationAdjustAngle = 0.0;
static CGeometryFillFactorMap* geometryFillFactorMap = NULL;
static bool s_ExportLocalFiducial = false;
static   int         boardKw;
static   int         geometryKw;
static   int         compKw;
static   int         pinKw;

static int load_AOIsettings(CString settingsFile, FormatStruct* format);
static void update_geom_list();
static int do_boardarray(FileStruct *file, double insert_x, double insert_y, double rotation, double scale,FormatStruct* format);
void PLX_GetData(FileStruct* pcbFile,CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, 
      int embeddedLevel, int boardnumber, FormatStruct* format);
void PLX_GetApertures(FileStruct& fileStruct,CDataList& dataList,const CTMatrix& matrix,
      int insertLayer, int boardnumber, AoiMachineTag machine, CString compName, CString pinName, int mixed);
void PLX_GetTools(FileStruct* pcbFile,CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, 
      int insertLayer, int boardnumber, FormatStruct* format);
static int check_fiducials();
static int write_plx_file(const char *pathname, int smd_only, int sp50Mode,int sjMode);
static int do_padstacks(double scale);
static int write_tpp_file(const char *pathname, double scale);
static int write_f_records(FILE *fp, bool bottomFlag, int sp50Mode);
static int write_l_records(FILE *fp, bool bottomFlag, int sp50Mode);
static int write_b_records(FILE *fp, bool bottomFlag, int sp50Mode);
static int write_d_records(FILE *fp, bool bottomFlag, int smd_only, int sp50Mode,int sjMode);
static std::multimap<int,PLXFiducial*> boardFiducialArray;
static std::set<int> boardNumbers;
static void fillBoardFiducialArray();
static int s_FirstBoardNumber = 1;
/******************************************************************************
* Agilent_AOI_WriteFiles
*  - the path is in pathname, the prefix is in format->filename
*/
void Agilent_AOI_WriteFiles(const char *pathname, CCEtoODBDoc *Doc,FormatStruct *format, int page_units)
{
   FileStruct *file, *filefound;

   boardKw    = Doc->IsKeyWord(ATT_ECAD_BOARD_ON_PANEL, 0);
   geometryKw = Doc->IsKeyWord(ATT_ECAD_GEOMETRY_NAME, 0);
   compKw     = Doc->IsKeyWord(ATT_ECAD_REFDES, 0);
   pinKw      = Doc->IsKeyWord(ATT_ECAD_PIN, 0);

   CPlxCompPin::resetKeywordIndexes();
   CPlxCompPin::releaseResources();  
   CPlxCompPin::setExportPartGeometryInformationFlag(false);

	// case 1802 - Only SP50 has option to use/not use zero rotation.
	// SP1&2 (and the rest) ALWAYS use zero rotation, no choice.
	CPlxCompPin::setUseZeroRotationForComplexAperturesFlag(format->AOI.machine != AoiMachineSp50 || format->AOI.useZeroRotationForComplexAperturesFlag);

   doc = Doc;

   display_error = FALSE;
   int sp50Mode = FALSE;
   int sjMode = FALSE;

   CString logFile = GetLogfilePath("Agilent_AOI.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString settingsFile( getApp().getExportSettingsFilePath("Agilent_AOI.out") );
   load_AOIsettings(settingsFile,format);

   CString checkFile( getApp().getSystemSettingsFilePath("Agilent_AOI.chk") );
   check_init(checkFile);

   // all AOI units are in micrometers
   unitsFactor = Units_Factor(page_units, UNIT_MM) * 1000;

   comparray.SetSize(0,100);

   partarray.SetSize(100,100);
   partcnt = 0;

   fiducialarray.SetSize(100,100);
   fiducialcnt = 0;

   rejectmarkarray.SetSize(0,100);

   geomarray.SetSize(100,100);
   geomcnt = 0;

   padstackarray.SetSize(100,100);
   padstackcnt = 0;

   terminalarray.SetSize(10,10);
   terminalcnt = 0;

   geometryFillFactorMap = new CGeometryFillFactorMap();
   geometryFillFactorMap->InitHashTable(nextPrime2n(300));

   if (format->AOI.machine == AoiMachineSj) // SJ10 works on CAD files
   {
      // check if panel or single board,
      sjMode = TRUE;
      int panelfound = 0;
      int pcbfound   = 0;
      POSITION pos = doc->getFileList().GetHeadPosition();

      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);

         if (file->getBlockType() == BLOCKTYPE_PANEL)   
         {
            if (!file->isShown())
               continue;

            filefound = file;
            panelfound++;        
         }

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            if (!file->isShown())
               continue;

            filefound = file;
            pcbfound++;
         }
      }

      if (panelfound > 1)
      {
         ErrorMessage("Multiple Visible Panel files found. Only 1 visible Panel allowed","SJ: Too many Panels");
         return;
      }

      if (pcbfound > 1)
      {
         ErrorMessage("Multiple Visible PCB files found. Only 1 visible PCB allowed","SJ: Too many PCBs");
         return;
      }

      if (!panelfound && !pcbfound)
      {
         ErrorMessage("No Visible Panel or PCB file found.", "SJ: No Visible File");
         return;
      }

      if (panelfound && pcbfound)
      {
         ErrorMessage("Too many Visible file found.", "SJ: Too many visible Files");
         return;
      }

      generate_CENTROIDS(doc);
      update_geom_list();     //

      // filefound is the usable filestruct.
      if (panelfound == 1)
      {
         // need to do top and bottom

         //on panel ,loop through the panel and list all the boards.. 
         do_boardarray(filefound, 0.0, 0.0, filefound->getRotation(), filefound->getScale() * unitsFactor,format);
      }
      else
      {
         // no panel found, do a single board
         // top and bottom
         PLX_GetData(filefound,&(filefound->getBlock()->getDataList()),
                    filefound->getInsertX() * unitsFactor, filefound->getInsertY() * unitsFactor,
                    filefound->getRotation(), filefound->isMirrored(), filefound->getScale() * unitsFactor, -1, -1,format);
      }

   } // machinetype
   else if (format->AOI.machine == AoiMachineSp) // SP works on Gerber files with layer PASTE_TOP, PASTE_BOTTOM
   {
      Generate_Unique_DCodes(doc);

      if (!s_rotationIncrementBool)
            s_rotationIncrement = 1; 

      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         file = doc->getFileList().GetNext(filePos);

         if (!file->isShown())
            continue;

         CTMatrix matrix = file->getTMatrix();
         matrix.scaleCtm(unitsFactor);

         PLX_GetApertures(*file,file->getBlock()->getDataList(),matrix,
            -1, -1, format->AOI.machine, "", "",0);

         if (comparray.GetSize() == 0)
            ErrorMessage("No Aperture Flashes found on PASTE layer", "SP: No output created");
         else
         {
            // SP only supports 0..180 degree on 45 degree steps. this should not be a problem
            // because all apertures should be centered.
            /*for (int i=0; i<compcnt; i++)
               if (comparray[i]->rotation > 180)
                  comparray[i]->rotation -= 180;*/
         }
      }
   }
   else if (format->AOI.machine == AoiMachineVia) // FX works on Gerber
   {
      // can also have solderpaste and behaves like SP
      // all other visible layers are used as FLUX 
      Generate_Unique_DCodes(doc);

      if (!s_rotationIncrementBool)
      {
         s_rotationIncrement = 1; 
      }
      
      for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
      {
         file = doc->getFileList().GetNext(filePos);

         if (!file->isShown())
         {
            continue;
         }

         CTMatrix matrix = file->getTMatrix();
         matrix.scaleCtm(unitsFactor);

         PLX_GetApertures(*file,file->getBlock()->getDataList(),matrix,
            -1, -1, format->AOI.machine, "", "",0);

         if (comparray.GetSize() == 0)
         {
            ErrorMessage("No Aperture Flashes found on PASTE layer", "SP: No output created");
         }
         else
         {
            // SP only supports 0..180 degree on 45 degree steps. this should not be a problem
            // because all apertures should be centered.
            /*for (int i=0; i<compcnt; i++)
               if (comparray[i]->rotation > 180)
                  comparray[i]->rotation -= 180;*/
         }
      }
   }
   else if (format->AOI.machine == AoiMachineFx) // works on drill stuff
   {
      // can also have solderpaste and behaves like SP
      // all other visible layers are used as FLUX 
      Generate_Unique_TCodes(doc);

      if (!s_rotationIncrementBool)
            s_rotationIncrement = 1; 

      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);

         if (!file->isShown())
            continue;

         // loop through every file
         PLX_GetTools(file,&(file->getBlock()->getDataList()),
                    file->getInsertX(), file->getInsertY(),
                    file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, -1, -1, format);

         if (comparray.GetSize() == 0)
         {
            ErrorMessage("No Drill Tools found.", "MicroVia: No output created");
         }
      }
   }
   else if (format->AOI.machine == AoiMachineMixed) // mixed mode
   {
      // check if panel or single board,
      int panelfound = 0;
      int pcbfound   = 0;

      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);

         if (file->getBlockType() == BLOCKTYPE_PANEL)   
         {
            if (!file->isShown())
               continue;
            filefound = file;
            panelfound++;        
         }

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            if (!file->isShown())
               continue;
            filefound = file;
            pcbfound++;
         }
      }

      if (panelfound > 1)
      {
         ErrorMessage("Multiple Visible Panel files found. Only 1 visible Panel allowed","SJ: Too many Panels");
         return;
      }

      if (pcbfound > 1)
      {
         ErrorMessage("Multiple Visible PCB files found. Only 1 visible PCB allowed","SJ: Too many PCBs");
         return;
      }

      if (!panelfound && !pcbfound)
      {
         ErrorMessage("No Visible Panel or PCB file found.", "SJ: No Visible File");
         return;
      }

      if (panelfound && pcbfound)
      {
         ErrorMessage("Too many Visible file found.", "SJ: Too many visible Files");
         return;
      }

      generate_CENTROIDS(doc);
      update_geom_list();     //

      if (panelfound == 1)  // do the SJ part
      {
         // need to do top and bottom

         //on panel ,loop through the panel and list all the boards.. 
         do_boardarray(filefound, 0.0, 0.0, filefound->getRotation(), filefound->getScale() * unitsFactor,format);

      }
      else
      {
         // no panel found, do a single board
         // top and bottom
         PLX_GetData(filefound,&(filefound->getBlock()->getDataList()),
                    filefound->getInsertX() * unitsFactor, filefound->getInsertY() * unitsFactor,
                    filefound->getRotation(), filefound->isMirrored(), filefound->getScale() * unitsFactor, -1, -1,format);
      }

      int sjCnt = comparray.GetSize();
      
      // do the sp part

      Generate_Unique_DCodes(doc);

      if (!s_rotationIncrementBool)
         s_rotationIncrement = 1;

      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         file = doc->getFileList().GetNext(filePos);

         if (!file->isShown())
            continue;

         CTMatrix matrix = file->getTMatrix();
         matrix.scaleCtm(unitsFactor);

         PLX_GetApertures(*file,file->getBlock()->getDataList(),matrix,
            -1, -1, format->AOI.machine, "", "",1);

         if (comparray.GetSize() - sjCnt == 0)// if no new SP data found.
            ErrorMessage("No Unload Components found. No SP lines created", "SP: No output created");
         else
         {
            // SP only supports 0..180 degree on 45 degree steps. this should not be a problem
            // because all apertures should be centered.
            /*for (int i=0; i<compcnt; i++)
               if (comparray[i]->rotation > 180)
                  comparray[i]->rotation -= 180;*/
         }
      }
   }
   else if (format->AOI.machine == AoiMachineSp50) // SP50
   {
      sp50Mode = TRUE;

      Generate_Unique_DCodes(doc);

      if (!s_rotationIncrementBool)
         s_rotationIncrement = 1; // 

      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         file = doc->getFileList().GetNext(filePos);

         if (!file->isShown())
            continue;

         CTMatrix matrix = file->getTMatrix();
         matrix.scaleCtm(unitsFactor);

         PLX_GetApertures(*file,file->getBlock()->getDataList(),matrix,
            -1, -1, format->AOI.machine, "", "",0);

         if (comparray.GetSize() == 0)
         {
            ErrorMessage("No Aperture Flashes found on PASTE layer", "SP: No output created");
            }
         else
         {
            // SP only supports 0..180 degree on 45 degree steps. this should not be a problem
            // because all apertures should be centered.
            /*for (int i=0; i<compcnt; i++)
               if (comparray[i]->rotation > 180)
                  comparray[i]->rotation -= 180;*/
         }
      }
   }
    
   check_fiducials();
   write_plx_file(pathname, format->AOI.smd_only, sp50Mode,sjMode);
   
   if (format->AOI.machine == AoiMachineSj && s_CreateTPPfiles) // SJ10 works on CAD files
   {
      do_padstacks(unitsFactor);
      write_tpp_file(pathname, unitsFactor);
   }

   // the contents is removed in write_ttp_file
   terminalarray.RemoveAll();

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
	int i=0;   
	for (i=0;i<comparray.GetSize();i++)
   {
      delete comparray[i];
   }

   comparray.RemoveAll();

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
   for (i=0;i<partcnt;i++)
   {
      delete partarray[i];
   }

   partarray.RemoveAll();
   partcnt = 0;

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
   for (i=0;i<geomcnt;i++)
   {
      delete geomarray[i];
   }

   geomarray.RemoveAll();
   geomcnt = 0;

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
   for (i=0;i<fiducialcnt;i++)
   {
      delete fiducialarray[i];
   }

   fiducialarray.RemoveAll();
   fiducialcnt = 0;


   boardFiducialArray.clear();
   boardNumbers.clear();
   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
	for (i = 0; i < rejectmarkarray.GetSize(); i++)
   {
      delete rejectmarkarray[i];
   }

   rejectmarkarray.RemoveAll();

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
   for (i=0;i<padstackcnt;i++)
   {
      delete padstackarray[i];
   }

   padstackarray.RemoveAll();
   padstackcnt = 0;

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
   delete geometryFillFactorMap;
   geometryFillFactorMap = NULL;

   //    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -  
   if (check_report(flog)) display_error++;
   check_deinit();

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   CPlxCompPin::releaseResources();   
}

/******************************************************************************
* write_plx_file
*/
static int write_plx_file(const char *pathname, int smd_only, int sp50Mode,int sjMode)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   // need to do this, because panel only knows the path.ext
   _splitpath( pathname, drive, dir, fname, ext );


   char     topfile[_MAX_PATH];
   char     bottomfile[_MAX_PATH];
   CString  filename;

   filename.Format("%s_top", fname);
   _makepath(topfile,drive,dir,filename,".plx");
   filename.Format("%s_bot", fname);
   _makepath(bottomfile,drive,dir,filename,".plx");

   FILE  *tfp, *bfp;

   if ((tfp = fopen(topfile, "wt")) == NULL) // rewrite file
   {
      MessageBox(NULL, "Can not open Topfile !", topfile, MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   if ((bfp = fopen(bottomfile, "wt")) == NULL) // rewrite file
   {
      MessageBox(NULL, "Can not open Bottomfile !", bottomfile, MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   CTime t;
   t = t.GetCurrentTime();
   fprintf(tfp,"# Created : %s\n",t.Format("%A, %B %d, %Y at %H:%M:%S"));
   fprintf(tfp,"# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(tfp,"\n");
   fprintf(tfp,"# Top Layer\n\n");
   fprintf(tfp,"P 1\n\n");

   fprintf(bfp,"# Created : %s\n",t.Format("%A, %B %d, %Y at %H:%M:%S"));
   fprintf(bfp,"# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(bfp,"\n");
   fprintf(bfp,"# Bottom Layer\n\n");
   fprintf(bfp,"P 1\n\n");

   if(true == s_ExportLocalFiducial)
   {
      fillBoardFiducialArray();
   }

   write_f_records(tfp,0, sp50Mode); // top
   if(true == s_ExportLocalFiducial)
   {
      write_l_records(tfp, 0, sp50Mode);
   }
   write_b_records(tfp,0, sp50Mode); // top
   write_d_records(tfp,0, smd_only, sp50Mode,sjMode);   //top

   
   write_f_records(bfp,1, sp50Mode); // bot
   if(true == s_ExportLocalFiducial)
   {
      write_l_records(bfp, 1, sp50Mode);
   }
   write_b_records(bfp,1, sp50Mode); // bot
   write_d_records(bfp,1, smd_only, sp50Mode,sjMode);    //bot 

   fprintf(tfp,"\n");
   fprintf(tfp,"# End Top Layer\n\n");
   fprintf(bfp,"\n");
   fprintf(bfp,"# End Bottom Layer\n\n");

   fclose(tfp);
   fclose(bfp);

   return 1;
}

/*****************************************************************************/
/*
*/
static int get_centroid(const char *bname, double *x, double *y)
{   
   *x = 0;
   *y = 0;

   for (int i=0;i<geomcnt;i++)
   {
      if (geomarray[i]->name.Compare(bname) == 0)
      {
         *x = geomarray[i]->centroid_x;
         *y = geomarray[i]->centroid_y;         
         return 1;
      }
   }

   return 0;
}

/*****************************************************************************/
/*
*/
//static int rot(double r)
//{
//   int rr = round(r);
//
//   while (rr < 0)    rr += 360;
//   while (rr >= 360) rr -= 360;
//
//   return rr;
//}

/******************************************************************************
* update_geom_list
*  - update all geometries
*/
static void update_geom_list()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFlags() & BL_WIDTH)      
         continue;

      if (block->getFlags() & BL_TOOL)             
         continue;

      if (block->getFlags() & BL_APERTURE)         
         continue;

      if (block->getFlags() & BL_BLOCK_APERTURE)
         continue;
   
      if (block->getFlags() & BL_FILE)          
         continue;

      switch (block->getBlockType())
      {
      case BLOCKTYPE_PCBCOMPONENT:  
         {
            // do it
            DataStruct *d = centroid_exist_in_block(block);

            if (d)
            {
               PLXGeom *g = new PLXGeom;
               geomarray.SetAtGrow(geomcnt,g);  
               geomcnt++;  
               g->centroid_x = round(d->getInsert()->getOriginX() * unitsFactor);
               g->centroid_y = round(d->getInsert()->getOriginY() * unitsFactor);
               g->centroid_rotation = d->getInsert()->getAngle();
               g->name = block->getName();
            }
            else
            {
               fprintf(flog,"PCB Component [%s] has not Centroid\n", block->getName());
               display_error++;
            }
         }

         break;
      default:
            // do nothing
            // blocktype_unknown is type 0 and done above
         break;
      }
   } // while
}

/*****************************************************************************/
/*
*/
static int get_fiducial_layer(BlockStruct *block, int block_mirror)
{
   int   layer = 1;

   // if it is an normal insert 
   if (block_mirror & MIRROR_FLIP)
      layer = 2;

   BlockStruct *b = get_padstack_block(doc, block);

   if (b && b->getBlockType() == BLOCKTYPE_PADSTACK)
   {
      if (b->getFlags() & BL_ACCESS_NONE)
      { 
         // do nothing
      }
      else if ((b->getFlags() & BL_ACCESS_OUTER) == BL_ACCESS_OUTER)
         layer = 3;
      else if (b->getFlags() & (BL_ACCESS_TOP))
         layer = (block_mirror & MIRROR_FLIP)?2:1;
      else if (b->getFlags() & (BL_ACCESS_BOTTOM))
         layer = (block_mirror & MIRROR_FLIP)?1:2;
      else if (b->getFlags() & BL_ACCESS_BOTTOM_ONLY)
         layer = 2;
      else if (b->getFlags() & BL_ACCESS_TOP_ONLY)
         layer = 1;
   }

   return layer;
}

/******************************************************************************
* PLX_GetData
*
*/
void PLX_GetData(FileStruct* pcbFile,CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, 
      int embeddedLevel, int boardnumber,FormatStruct* format)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   CTMatrix pcbBoardMatrixCtm;
   pcbBoardMatrixCtm.translateCtm(insert_x, insert_y);
   pcbBoardMatrixCtm.rotateRadiansCtm(rotation);
   pcbBoardMatrixCtm.scaleCtm(mirror?-1.:1., 1.);

   for (CDataListIterator insertIterator(*DataList,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* np = insertIterator.getNext();
      InsertStruct* insert = np->getInsert();
      
      bool loaded = true;
      Attrib *loadedAttrib  = is_attvalue(doc, np->getAttributesRef(),ATT_LOADED, 2);

      if (loadedAttrib)
      {
         CString test = get_attvalue_string(doc, loadedAttrib); 

         if (!test.IsEmpty())
         {
            loaded = (test.CompareNoCase("TRUE") == 0);
         }
      }

      if (!loaded && !s_ExportUnloadedParts)
         continue;

      Point2 point2(insert->getOriginX() * scale, insert->getOriginY() * scale);
      pcbBoardMatrixCtm.transform(point2.x, point2.y);
      
      BlockStruct *block = doc->Find_Block_by_Num(insert->getBlockNumber());
      if (insert->getInsertType() == insertTypePcbComponent)   
      {
         CPoint2d centroidLocation;
         insert->getCentroidLocation(doc->getCamCadData(), centroidLocation);
         centroidLocation.x *= scale;
         centroidLocation.y *= scale;
         pcbBoardMatrixCtm.transform(centroidLocation);

         CString compname = insert->getRefname();

         // check for partnumber
         CString partnumber;
         Attrib* attrib =  is_attvalue(doc, np->getAttributesRef(),ATT_PARTNUMBER, 2);
         if (attrib)
         {
            partnumber = get_attvalue_string(doc, attrib);
         }

         if (!strlen(partnumber))
         {
            if (attrib =  is_attvalue(doc, np->getAttributesRef(),ATT_TYPELISTLINK, 2))
            {
               partnumber = get_attvalue_string(doc, attrib);
            }
         }

         if (!strlen(partnumber))
         {
            partnumber = "UNKNOWN_PARTNUMBER";
         }

         
        
         int smd = FALSE;
         if (attrib =  is_attvalue(doc, np->getAttributesRef(),ATT_TECHNOLOGY, 2))
         {
            CString technology = get_attvalue_string(doc, attrib);
            if (technology.CompareNoCase("SMD") == 0)
               smd = TRUE;
         }

         CString placement_machine = "M0";
         if (attrib =  is_attvalue(doc, np->getAttributesRef(), ATT_AGILENT_AOI_PLACEMENT_MACHINE, 2))
         {
            placement_machine = get_attvalue_string(doc, attrib);
         }

         CString placement_feeder = "0";
         if (attrib =  is_attvalue(doc, np->getAttributesRef(), ATT_AGILENT_AOI_PLACEMENT_FEEDER, 2))
         {
            placement_feeder = get_attvalue_string(doc, attrib);
         }

         CString machine = placement_machine + '-' + placement_feeder;

         int test = TRUE;
         if (attrib =  is_attvalue(doc, np->getAttributesRef(), ATT_AGILENT_AOI_TEST, 2))
         {
            CString l = get_attvalue_string(doc, attrib);
            if (!l.CompareNoCase("NONE"))
               test = FALSE;
         }
      
         int block_mirror = mirror ^ insert->getMirrorFlags();
         bool bottomFlag = ((block_mirror & MIRROR_FLIP) != 0);

         if (format->AOI.machine == AoiMachineSj || format->AOI.machine == AoiMachineMixed)
         {
/*
Table 4: Desired Behavior

                    Input                        |     Output 
Place Bottom    Mirror Layer    Mirror Graphics  | .top    .bot 
     n                n               n          |   y       n 
     n                n               y          |   y       n 
     n                y               n          |   n       y 
     n                y               y          |   n       y 
     y                n               n          |   y       n 
     y                n               y          |   y       n 
     y                y               n          |   n       y 
     y                y               y          |   n       y 
*/
            bottomFlag = insert->getLayerMirrored();
            if ((mirror & MIRROR_LAYERS) != 0)
            {
               bottomFlag = !bottomFlag;
            }
         }
         
         //if (block_mirror & MIRROR_FLIP)
         //   bottom = TRUE;
         double block_rot = rotation + insert->getAngle();
         CPlxCompPin *c = new CPlxCompPin(*pcbFile, format->AOI.machine);         
         comparray.Add(c);  
         c->refdes = check_name('c', compname);
         c->setComponentType("n0000");
         c->setPartNumber(check_name('b', partnumber));
         c->placement_machine = check_name('m', machine);
         c->setBasesVectorPlxUnits(centroidLocation.x, centroidLocation.y, RadToDeg(block_rot));
         c->setBottomFlag(bottomFlag);
         c->setGeometryName(block->getName());
         c->setGeometry(block);
         c->setComponent(np);
         c->boardnumber = boardnumber;
         c->test = test;
         c->smd = smd;
         c->written = 0;
         c->bnum = block->getBlockNumber();
         c->setBoardTransformMatrix(pcbBoardMatrixCtm);
         if(false == loaded)
         {
            c->loaded = false;
         }
         continue;
      }        

      if (insert->getInsertType() == insertTypeFiducial) 
      {
         CString  compname;
         compname = insert->getRefname();

         double block_rot = rotation + insert->getAngle();
         int block_mirror = mirror ^ insert->getMirrorFlags();
         // xrot is the centroid delta

         PLXFiducial *c = new PLXFiducial;
         fiducialarray.SetAtGrow(fiducialcnt,c);  
         fiducialcnt++; 

         int layer = 1;
         
         // if a fiducial is a gerber aperture, it is placed on a top or bottom layer
         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            int lnr = Get_ApertureLayer(doc, np, block, -1);

            if ( doc->get_layer_visible(lnr, mirror) )
            {
               LayerStruct *ll = doc->FindLayer(lnr);

               if (ll != NULL && is_layerbottom(ll->getLayerType()))
                  layer = 2;
            }
         }
         else
         {
            // if it is an normal insert 
            layer = get_fiducial_layer(block, block_mirror);
         }

         Attrib *a =  is_attvalue(doc, np->getAttributesRef(),ATT_FIDUCIAL_PLACE_BOTH, 2);

         if (a)
            layer = 3;

         c->x = round(point2.x);
         c->y = round(point2.y);
         c->layer = layer;
         c->boardNumber = boardnumber;
         c->geomname = block->getName();
         c->boardFiducial = true;

         continue;
      }        

      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
      }
      else // not aperture
      {
         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ insert->getMirrorFlags();
         double block_rot = rotation + insert->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + insert->getAngle());   // add 180 degree if mirrored.

         PLX_GetData(pcbFile,&(block->getDataList()), 
                           point2.x,
                           point2.y,
                           block_rot, //rotation + insert.angle,
                           block_mirror,
                           scale * insert->getScale(),
                           embeddedLevel+1, boardnumber,format);
      } // end else not aperture
   } // end for

}


/******************************************************************************
* PLX_GetApertures
*
* New parameter added: mixed. 0:not in mixed mode; 1: in mixed mode
*
*/
void PLX_GetApertures(FileStruct& pcbFile,CDataList& dataList,const CTMatrix& matrix, 
      int insertLayer, int boardnumber, AoiMachineTag machine, CString compName, CString pinName, int mixed)
{
   //Mat2x2 m;
   //RotMat2(&m, rotation);
   bool mirror = matrix.getMirror();

   for (CDataListIterator insertIterator(dataList,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct *data = insertIterator.getNext();

      CPoint2d point2 = data->getInsert()->getOrigin2d();
      matrix.transform(point2);

      //Point2 point2;
      //point2.x = data->getInsert()->getOriginX() * scale;

      //if (mirror)
      //   point2.x = -point2.x;

      //point2.y = data->getInsert()->getOriginY() * scale;
      //TransPoint2(&point2, 1, &m, insert_x, insert_y);

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (data->getInsert()->getInsertType() == insertTypeFiducial)  
      {
         CString compname = data->getInsert()->getRefname();

         //double block_rot = rotation + data->getInsert()->getAngle();
         //int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         // xrot is the centroid delta
         bool block_mirror = (mirror != data->getInsert()->getGraphicMirrored());

         PLXFiducial *c = new PLXFiducial;
         fiducialarray.SetAtGrow(fiducialcnt,c);  
         fiducialcnt++; 

         int layerNum = 1;
         
         // if a fiducial is a gerber aperture, it is placed on a top or bottom layer
         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            int apLayerNum = Get_ApertureLayer(doc, data, block, -1);

            if (doc->get_layer_visible(apLayerNum, mirror))
            {
               LayerStruct *apLayer = doc->FindLayer(apLayerNum);

               if (apLayer != NULL && is_layerbottom(apLayer->getLayerType()))
                  layerNum = 2;
            }
         }
         else
         {
            // if it is an normal insert 
            layerNum = get_fiducial_layer(block, block_mirror);
         }

         Attrib *atrib =  is_attvalue(doc, data->getAttributesRef(), ATT_FIDUCIAL_PLACE_BOTH, 2);

         if (atrib)
            layerNum = 3;

         c->x = round(point2.x);
         c->y = round(point2.y);
         c->layer = layerNum;
         c->geomname = block->getName();
         c->boardNumber = boardnumber;
         c->boardFiducial = true;
			c->compname = compname;
      
         continue;
      }        

      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         //double block_rot = rotation + data->getInsert()->getAngle() + block->getRotation(); // aperture rotation
         //int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         // xrot is the centroid delta
         bool block_mirror = (mirror != data->getInsert()->getGraphicMirrored());

         int layerNum = Get_ApertureLayer(doc, data, block, insertLayer);

         if (doc->get_layer_visible(layerNum, mirror))
         {
            //double x_cent_offset = 0;
            //double y_cent_offset = 0;
            //int xsize, ysize, fillfactor;
            //double apertureRotationDegrees = block->getRotationDegrees();

            //if (compName.CompareNoCase("U2") == 0)
            //{
            //   int iii = 3;
            //}
      
            //CString  appform;

            //switch (block->getShape())
            //{
            //case T_ROUND:
            //   fillfactor = 79;
            //   xsize = round(block->getSizeA() * unitsFactor);
            //   ysize = round(block->getSizeA() * unitsFactor);
            //   appform.Format("%04dx%04df%03d", xsize, ysize, fillfactor);
            //   break;
            //case T_SQUARE:
            //   fillfactor = 100;
            //   xsize = round(block->getSizeA() * unitsFactor);
            //   ysize = round(block->getSizeA() * unitsFactor);
            //   appform.Format("%04dx%04df%03d", xsize, ysize, fillfactor);
            //   break;
            //case T_RECTANGLE:
            //   fillfactor = 100;
            //   xsize = round(block->getSizeA() * unitsFactor);
            //   ysize = round(block->getSizeB() * unitsFactor);
            //   appform.Format("%04dx%04df%03d", xsize, ysize, fillfactor);
            //   break;
            //case T_OBLONG:
            //   fillfactor = 100;
            //   xsize = round(block->getSizeA() * unitsFactor);
            //   ysize = round(block->getSizeB() * unitsFactor);
            //   appform.Format("%04dx%04df%03d", xsize, ysize, fillfactor);
            //   break;
            //default:
            //   fillfactor = 99;
            //   apertureRotationDegrees = 0.;

            //   doc->CalcBlockExtents(block);
            //   xsize = round((block->getXmax() - block->getXmin()) * unitsFactor);
            //   ysize = round((block->getYmax() - block->getYmin()) * unitsFactor);
            //   appform.Format("%04dx%04df%03d", xsize, ysize, fillfactor);
            //   break;
            //}

            int test = TRUE;
            Attrib* attrib;

            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_AGILENT_AOI_TEST, 2))
            {
               CString l = get_attvalue_string(doc, attrib);

               if (!l.CompareNoCase("NONE"))
                  test = FALSE;
            }

            LayerStruct *layer = doc->FindLayer(layerNum);

            if (layer != NULL)
            { 
               if (layer->getLayerType() == LAYTYPE_PASTE_TOP || 
                  layer->getLayerType() == LAYTYPE_PASTE_BOTTOM || 
                  layer->getLayerType() == LAYTYPE_PASTE_ALL)
               {
                  CString machineString = "SP-1";

                  // here is an aperture to load.
                  CPlxCompPin *comp = new CPlxCompPin(pcbFile, machine);

                  Attrib* attrib;

                  if (data->getAttributesRef() != NULL /* &&
                     (data->getAttributesRef()->Lookup(boardKw, value) ||
                     (data->getAttributesRef()->Lookup(compKw, value) &&
                        data->getAttributesRef()->Lookup(pinKw, value)))*/)
                  {
                     CString boardName, geometryName, compName, pinName;

                     if (data->getAttributesRef()->Lookup(boardKw, attrib))
                     {
                        boardName = get_attvalue_string(doc, attrib);
                     }

                     if (data->getAttributesRef()->Lookup(geometryKw, attrib))
                     {
                        geometryName = get_attvalue_string(doc, attrib);
                     }

                     if (data->getAttributesRef()->Lookup(compKw, attrib))
                     {
                        compName = get_attvalue_string(doc, attrib);
                     }

                     if (data->getAttributesRef()->Lookup(pinKw, attrib))
                     {
                        pinName = get_attvalue_string(doc, attrib);
                     }

                     if (!boardName.IsEmpty())
                        comp->refdes = (CString)check_name('c', boardName) + ":";

                     if (compName.IsEmpty())
                     {
                        comp->refdes.Format("n%d-1", comparray.GetSize());
                     }
                     else
                     {
                        comp->refdes.AppendFormat("%s-%s", check_name('c', compName), check_name('c', pinName));
                     }

                     comp->setShapeGeometryName(geometryName);
                  }
                  else
                  {
                     if (compName.IsEmpty())
                     {
                        comp->refdes.Format("n%d-1", comparray.GetSize());
                     }
                     else
                     {
                        comp->refdes = compName;
                        comp->refdes += '-';

                        if (pinName.IsEmpty())
                           comp->refdes += '1';
                        else
                           comp->refdes += pinName;
                     }
                  }

                  comp->setType(plxTypePaste);
                  //comp->comptype = "pa_" + appform;
                  //comp->partnumber = "P" + appform;
                  comp->placement_machine = machineString;
                  
                  CBasesVector basesVector = data->getInsert()->getBasesVector();
                  basesVector.transform(matrix);

					   comp->setBasesVectorPlxUnits(basesVector);

                  //comp->setOrigin(point2);
                  //comp->setX(round(point2.x+x_cent_offset));
                  //comp->setY(round(point2.y+y_cent_offset));
                  //comp->rotation = rot(RadToDeg(block_rot));

                  //it is either on LAYTYPE_PASTE_TOP, LAYTYPE_PASTE_BOTTOM, or LAYTYPE_PASTE_ALL
                  bool bottomFlag = false;

                  if (block_mirror && layer->getLayerType() == LAYTYPE_PASTE_TOP)
                  {
                     //if it is on TOP and mirrored, then bottom is true.
                     bottomFlag = true;
                  }         
                  else if (block_mirror && layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
                  {
                     //if it is on BOTTOM and mirrored, then bottom is false.
                     bottomFlag = false;
                  }         
                  else if (!block_mirror)
                  {
                     //if not mirrored and top, then bottom is false.
                     if (layer->getLayerType() == LAYTYPE_PASTE_TOP)
                        bottomFlag = false;

                     //if not mirrored and bottom, then bottom is true.
                     if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
                        bottomFlag = true;
                  }
                  else
                  {
                     //else, it is on ALL, then bottom is false.
                     bottomFlag = false;
                  }

                  comp->setBottomFlag(bottomFlag); // Bottom => TRUE; Top => FALSE; ALL => FALSE here and TRUE on copy

                  CString geometryName;
                  if (block->getShape() != apertureUnknown)
                     geometryName.Format("%s_D%d", shapes[block->getShape()], block->getDcode());
                  comp->setGeometryName(geometryName);

                  comp->setGeometry(block);
                  comp->setComponent(data);
                  comp->boardnumber = -1;
                  comp->test = test;
                  comp->smd = FALSE;

                  if (comparray.GetSize() > 1)
                  {
                     // if true, then the component is through hole and unloaded, so put it on Top Plx and bottom plx
                     //if ((!comp->refdes.CompareNoCase(comparray[compcnt-2]->refdes)) && comp->x == comparray[compcnt-2]->x && comp->y == comparray[compcnt-2]->y && !pinName.IsEmpty())
                     //   comp->bottom = !comparray[compcnt-2]->bottom;         

                     CPlxCompPin *lastComp = comparray[comparray.GetSize()-1];

                     if (!lastComp->refdes.CompareNoCase(comp->refdes) && lastComp->getBottomFlag() == comp->getBottomFlag())
                     {
                        // Ignore subsequence pad on the same layers
                        delete comp;
                        continue;
                     }
                  }
            
                  comparray.Add(comp);  

                  if (layer->getLayerType() == LAYTYPE_PASTE_ALL)
                  {
                     // copy last and put it on bottom
                     CPlxCompPin *comp2 = new CPlxCompPin(pcbFile, machine);
                     comparray.Add(comp2); 

                     comp2->bnum = comp->bnum;
                     comp2->boardnumber = comp->boardnumber;
                     comp2->setBottomFlag(true);
                     comp2->setComponentType(comp->getComponentType());
                     comp2->setGeometryName(comp->getGeometryName());
                     comp2->setGeometry(comp->getGeometry());
                     comp2->setComponent(comp->getComponent());
                     comp2->setType(comp->getType());
                     comp2->setPartNumber(comp->getPartNumber());
                     comp2->placement_machine = comp->placement_machine;
                     comp2->refdes = comp->refdes;
                     //comp2->setRotationDegrees(comp->getRotationDegrees());
                     comp2->smd = comp->smd;
                     comp2->test = comp->test;
                     comp2->written = comp->written;
                     //comp2->setX(comp->getX());
                     //comp2->setY(comp->getY());
                     comp2->setBasesVector(comp->getBasesVector()); //*rcf** NOT PLX UNITS here is correct
                  }
               }
               else if (layer->getLayerType() == LAYTYPE_FLUX_TOP || 
                        layer->getLayerType() == LAYTYPE_FLUX_BOTTOM)
               {
                  if (machine != AoiMachineVia) // check if FX selection
                     continue;

                  CString machineString = "MCA-1";

                  // here is an aperture to load.
                  CPlxCompPin *comp = new CPlxCompPin(pcbFile, machine);
                  comparray.Add(comp);  

                  if (compName.IsEmpty())
                     comp->refdes.Format("n%d-1", comparray.GetSize());
                  else
                  {
                     comp->refdes = compName;
                     comp->refdes += '-';

                     if (pinName.IsEmpty())
                        comp->refdes += '1';
                     else
                        comp->refdes += pinName;
                  }

                  comp->setType(plxTypeFlux);
                  //comp->comptype = "fl_" + appform;
                  //comp->partnumber = "F" + appform;
                  comp->placement_machine = machineString;
                  
                  CBasesVector basesVector;
                  basesVector.transform(matrix);

					   comp->setBasesVectorPlxUnits(basesVector);

                  comp->setBottomFlag(layer->getLayerType() == LAYTYPE_FLUX_BOTTOM);

                  CString geometryName;
                  if (block->getShape() >=0 && block->getShape() < 10)
                     geometryName.Format("%s_D%d", shapes[block->getShape()], block->getDcode());
                  comp->setGeometryName(geometryName);

                  comp->setGeometry(block);
                  comp->boardnumber = -1;
                  comp->test = test;
                  comp->smd = FALSE;
               }
           }
         }
      }
      else // not aperture
      {
         // insert if mirror is either global mirror or block_mirror, but not if both.
         //int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         //double block_rot = rotation + data->getInsert()->getAngle();

         //if (mirror)
         //   block_rot = PI2 - (rotation + data->getInsert()->getAngle()); // add 180 degree if mirrored.

         CString newCompName = compName;
         CString newPinName = pinName;

         if (data->getInsert()->getInsertType() == insertTypePcbComponent)
            newCompName = data->getInsert()->getRefname();

         if (data->getInsert()->getInsertType() == insertTypePin)
            newPinName = data->getInsert()->getRefname();

         //if it is in mixed mode, then check the loaded attribute!
         if (mixed && data->getInsert()->getInsertType() == insertTypePcbComponent)
         {
            int loaded = TRUE;
            Attrib *loadedAttrib = is_attvalue(doc, data->getAttributesRef(),ATT_LOADED, 2);

            if (loadedAttrib)
            {
               CString test = get_attvalue_string(doc, loadedAttrib);

               if (!test.CompareNoCase("FALSE"))   // TRUE
                  loaded = FALSE;
            }

            if ((loaded && loadedAttrib) || !loadedAttrib)
               continue;
         }

         if (data->getInsert()->getInsertType() == insertTypeVia)
            continue;

         // use premultiplication
         CTMatrix matrix2 = data->getInsert()->getTMatrix() * matrix;
      
         PLX_GetApertures(pcbFile,block->getDataList(),matrix2,
               data->getLayerIndex(), boardnumber, machine, newCompName, newPinName, mixed);
      }
   }
}


/******************************************************************************
* PLX_GetTools
*/
void PLX_GetTools(FileStruct* pcbFile,CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, 
      int insertLayer, int boardnumber, FormatStruct* format)
{
   Mat2x2 m;
   DataStruct *np;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
      case T_POLY:
         break;

      case T_TEXT:
         break;
         
      case T_INSERT:
         {
            Point2 point2;

            point2.x = np->getInsert()->getOriginX() * scale;

            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == insertTypeFiducial) 
            {
               CString  compname;
               compname = np->getInsert()->getRefname();

               double block_rot = rotation + np->getInsert()->getAngle();
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               // xrot is the centroid delta

               PLXFiducial *c = new PLXFiducial;
               fiducialarray.SetAtGrow(fiducialcnt,c);  
               fiducialcnt++; 

               int layer = 1;
               
               // if a fiducial is a gerber aperture, it is placed on a top or bottom layer
               if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               {
                  int lnr = Get_ApertureLayer(doc, np, block, -1);

                  if (doc->get_layer_visible(lnr, mirror) )
                  {
                     LayerStruct *ll = doc->FindLayer(lnr);

                     if (ll != NULL && is_layerbottom(ll->getLayerType()))
                        layer = 2;
                  }
               }
               else
               {
                  // if it is an normal insert 
                  layer = get_fiducial_layer(block, block_mirror);
               }

               Attrib *a =  is_attvalue(doc, np->getAttributesRef(),ATT_FIDUCIAL_PLACE_BOTH, 2);

               if (a)
                  layer = 3;

               c->x = round(point2.x);
               c->y = round(point2.y);
               c->layer = layer;
               c->geomname = block->getName();
               c->boardNumber = boardnumber;
               c->boardFiducial = true;

               break;
            }        

            if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
            {
               double block_rot = rotation + np->getInsert()->getAngle() + block->getRotation(); // aperture rotation
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               // xrot is the centroid delta

               bool bottomFlag = false;
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               int fillfactor = 79;

               long xsize = round(block->getToolSize() * unitsFactor);
               CString appform;
               appform.Format("%04dx%04df%03d", xsize, xsize, fillfactor);

               CString name;
               name.Format("x%06dy%06d", DcaRound(point2.x), DcaRound(point2.y));

               int test = TRUE;
               Attrib *a;

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_AGILENT_AOI_TEST, 2))
               {
                  CString  l;
                  l = get_attvalue_string(doc, a);

                  if (!l.CompareNoCase("NONE"))
                     test = FALSE;
               }

               CString machine; // = "VIA-1";
               machine.Format("T%02d", block->getTcode());

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_AGILENT_AOI_PLACEMENT_MACHINE, 2))
               {
                  CString  l;
                  machine = get_attvalue_string(doc, a);
               }

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_AGILENT_AOI_PLACEMENT_FEEDER, 2))
               {
                  CString  l;
                  machine += "-";
                  machine += get_attvalue_string(doc, a);
               }

               // here is an aperture to load.
               CPlxCompPin *c = new CPlxCompPin(*pcbFile, format->AOI.machine);
               comparray.Add(c);  

               c->refdes = name;
               //c->setType(plxTypeHole);
               //c->comptype = "ho_" + appform;
               //c->partnumber = "H" + appform;
               c->setComponentType("ho_" + appform);
               c->setPartNumber("H" + appform);
               c->placement_machine = machine;
               //c->setX(round(point2.x));
               //c->setY(round(point2.y));
               //c->setRotationDegrees(0);
               c->setBasesVectorPlxUnits(point2.x,point2.y);

               c->setBottomFlag(bottomFlag);
               //c->geomname.Format("Round_D%d", block->getDcode());
               c->setGeometryName("SH");
               c->setGeometry(block);
               c->boardnumber = -1;
               c->test = test;
               c->smd = FALSE;

            }
            else // not aperture
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               PLX_GetTools(pcbFile,&(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(),
                               np->getLayerIndex(), boardnumber, format);
            } // end else not aperture
         } // case INSERT

         break;
      } // end switch
   } // end for

} /* end PLX_GetAperture */


/******************************************************************************
* load_AOIsettings
*/
static int load_AOIsettings(CString fname, FormatStruct* format)
{
   FILE  *fp;
   char  line[255];
   char  *lp;
   

   s_rotationIncrement = 1;
	s_PackageNameAttrib = "";
   s_ExportUnloadedParts = true;
   s_CreateTPPfiles = true;
   s_UseCentroidRotation = false;
   s_UseCentroidPosition = true;
   s_UnloadedCompShapeSuffix = "-np";
   s_UnloadedCompPNSuffix = "-np";
   s_GlobalRotationAdjustAngle = 0.0;
   s_ExportLocalFiducial = false;
   s_FirstBoardNumber = 1;

   if (format != NULL && format->AOI.machine == AoiMachineMixed)
      s_ExportUnloadedParts = false;
      
   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      MessageBox(NULL, tmp, "Agilent AOI Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (STRCMPI(lp,".RotationIncrement") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            s_rotationIncrement = atoi(lp);
            s_rotationIncrementBool = true;
         }
         else if (STRCMPI(lp,".ExportPartGeometryInformation") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            CPlxCompPin::setExportPartGeometryInformationFlag(atoi(lp) != 0);
         }
         else if (STRCMPI(lp,".AddDebugGraphics") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            s_addDebugGraphics = (atoi(lp) != 0);
         }
         else if (STRICMP(lp, ".PACKAGE_NAME") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            s_PackageNameAttrib = lp;
         }
         else if (STRICMP(lp, ".EXPORT_UNLOADED_PARTS") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            s_ExportUnloadedParts = ((toupper(lp[0]) == 'Y') || (toupper(lp[0]) == 'T'));
         }
         else if (STRICMP(lp, ".CREATE_TPP_FILES") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            s_CreateTPPfiles = ((toupper(lp[0]) == 'Y') || (toupper(lp[0]) == 'T'));

         }
         else if (STRICMP(lp, ".COMP_POSITION") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            if(!STRCMPI(lp,"CENTROID"))
               s_UseCentroidPosition = true;
            else
               s_UseCentroidPosition = false;

         }
         else if (STRICMP(lp, ".COMP_ROTATION") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            if(!STRCMPI(lp,"CENTROID"))
               s_UseCentroidRotation = true;
            else
               s_UseCentroidRotation = false;
         }
         else if (STRICMP(lp, ".UNLOADED_COMP_SHAPE_SUFFIX") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            s_UnloadedCompShapeSuffix = lp;
            //Code for removing quotes from begining and ending of the string
            if(s_UnloadedCompShapeSuffix.GetAt(0) == '\"')
               s_UnloadedCompShapeSuffix.Delete(0);
            if(s_UnloadedCompShapeSuffix.GetAt(s_UnloadedCompShapeSuffix.GetLength()-1) == '\"')
               s_UnloadedCompShapeSuffix.Delete(s_UnloadedCompShapeSuffix.GetLength()-1);
         }
         else if (STRICMP(lp, ".UNLOADED_COMP_PN_SUFFIX") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            s_UnloadedCompPNSuffix = lp;
            //Code for removing quotes from begining and ending of the string
            if(s_UnloadedCompPNSuffix.GetAt(0) == '\"')
               s_UnloadedCompPNSuffix.Delete(0);
            if(s_UnloadedCompPNSuffix.GetAt(s_UnloadedCompPNSuffix.GetLength()-1) == '\"')
               s_UnloadedCompPNSuffix.Delete(s_UnloadedCompPNSuffix.GetLength()-1);
         }
         else if (STRICMP(lp, ".GLOBAL_ROTATION_ADJUST") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            s_GlobalRotationAdjustAngle = atof(lp);
         }
         else if (STRICMP(lp, ".LOCAL_FIDUCIALS") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            if(0 == STRCMPI(lp,"y"))
               s_ExportLocalFiducial = true;
         }
         else if (STRICMP(lp, ".FirstBoardNumber") == 0)
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            s_FirstBoardNumber =  atoi(lp);
            if(s_FirstBoardNumber < 0)
               s_FirstBoardNumber = 1;
         }
      }
   }

   fclose(fp);
   return 1;
}

/*****************************************************************************/
/*
*/
static int check_rotation_error(int rot)
{
   int r = rot % s_rotationIncrement;

   return r;
}

/*****************************************************************************/
/*
*/
static void cnv_aoi(bool bottomFlag, long x1, long y1, long *x, long *y)
{
   if (bottomFlag)
   {
      // bottom layer
      *x = -y1;
      *y = -x1;
   }
   else
   {
      // top layer
      *x = -y1;
      *y = x1;
   }

   return;
}

/*****************************************************************************/
/*
*/
static int update_partnumber(const char *partnumber, int bnum, int written, int rotation)
{   
   for (int i=0;i<partcnt;i++)
   {
      if (!partarray[i]->partnumber.CompareNoCase(partnumber) && partarray[i]->rotation == rotation)
      {
         partarray[i]->written |= written;
         return i;
      }      
   }
      
   PLXPart *c = new PLXPart;
   partarray.SetAtGrow(partcnt,c);  
   partcnt++;  

   c->partnumber = partnumber;
   c->written = written;
   c->rotation = rotation;
   c->bnum = bnum;

   return partcnt-1;
}

/******************************************************************************
* write_d_record
*/
static int write_d_records(FILE *fp, bool bottomFlag, int smd_only, int sp50Mode, int sjMode)
{
   COperationProgress progress(comparray.GetSize());
   CString status;
   status.Format("Writing %s D records",bottomFlag ? "bottom" : "top");
   progress.updateStatus(status,comparray.GetSize(),0);

   for (int i=0; i<comparray.GetSize(); i++)
   {
      progress.incrementProgress();
      CPlxCompPin *cmp = comparray[i];

      if (cmp->getBottomFlag() == bottomFlag)
      {
         long x = -cmp->getPlxY(); // x = -y
         long y =  cmp->getPlxX(); // y = x

         if (sp50Mode)
         {
            CPoint2d boundingBoxOrigin = cmp->getNormalizedExtent().getCenter();

				x = round(boundingBoxOrigin.x * getPlxUnitsFactor());
            y = round(boundingBoxOrigin.y * getPlxUnitsFactor());

            if (bottomFlag)
            {
               //y = -y;
               x = -x;
            }
         }

   
         //int rot = cmp->getRotationDegrees();

         //if (bottom) // if bottom, then add 180 degrees to the rotation.
         //   rot += 180;

         //rot = normalizeDegrees(rot);
         //cmp->setRotationDegrees(rot);

         CString refname = cmp->refdes;

         if(!sjMode)
         {
            if (cmp->boardnumber > 0)
               refname.Format("%d:%s", cmp->boardnumber-1, cmp->refdes);
         }
         else
         {
            if (cmp->boardnumber > 0)
               refname.Format("%d:%s", cmp->boardnumber-1 + s_FirstBoardNumber, cmp->refdes);
         }

         if (!sp50Mode)
         {
            if(sjMode)
            {
               if(s_UseCentroidPosition)
                  cnv_aoi(bottomFlag, cmp->getPlxX(), cmp->getPlxY(), &x, &y);
               else
               {
                  CTMatrix boardTransformMatrix = cmp->getBoardTransformMatrix();
                  CPoint2d origin;
                  origin = cmp->getComponent()->getInsert()->getOrigin2d();
                  origin.x *= unitsFactor;
                  origin.y *= unitsFactor;
                  boardTransformMatrix.transform(origin);                  
                  cnv_aoi(bottomFlag, (long)origin.x, (long)origin.y, &x, &y);                 
               }
            }
            else
            {
               cnv_aoi(bottomFlag, cmp->getPlxX(), cmp->getPlxY(), &x, &y);
            }
         }

         if (!cmp->test)
         {
            fprintf(fp, "!NoTest ");
         }
         else if (check_rotation_error(cmp->getRotationDegrees()))
         {
            fprintf(fp, "!Rotation ");
         }
         else if (smd_only)
         {
            if (!cmp->smd)    
               fprintf(fp, "!NotSMD ");
         }
         else
         {
            cmp->written |= (bottomFlag ? 2 : 1);
         }

         int rotationDegrees;

         if (sp50Mode)
         {
            rotationDegrees = normalizeDegrees(bottomFlag ? 360 - cmp->getNormalizedRotationDegrees() : cmp->getNormalizedRotationDegrees());
         }
         else
         {
            if(!sjMode)
            {
               rotationDegrees = normalizeDegrees(cmp->getNormalizedRotationDegrees() + (bottomFlag ? 180 : 0));
            }
            else
            {
               if(s_UseCentroidRotation)
               {
                  double CentroidRotation;
                  InsertStruct* componentInsert = cmp->getComponent()->getInsert();
                  CBasesVector insertBasesVector = cmp->getComponent()->getInsert()->getBasesVector();
                  componentInsert->getCentroidRotation(doc->getCamCadData(),CentroidRotation);
                  BlockStruct* block = doc->Find_Block_by_Num(cmp->getComponent()->getInsert()->getBlockNumber());
                  DataStruct *centroidData = (block != NULL) ? block->GetCentroidData() : NULL;                  
                  if(centroidData)
                  {
                     CBasesVector centroidBasesVector = centroidData->getInsert()->getBasesVector();
                     centroidBasesVector.transform(cmp->getBasesVector().getTransformationMatrix());
                     CentroidRotation = normalizeDegrees(round(centroidBasesVector.getRotationDegrees()));
                  }
                  else
                  {
                     CentroidRotation = cmp->getNormalizedRotationDegrees();
                  }
                  rotationDegrees = round(normalizeDegrees(CentroidRotation));
               }
               else
                  rotationDegrees = normalizeDegrees(cmp->getNormalizedRotationDegrees());

               if(bottomFlag)
               {
                  rotationDegrees = normalizeDegrees(360 - rotationDegrees);
               }

               rotationDegrees = round(normalizeDegrees(s_GlobalRotationAdjustAngle + rotationDegrees));
            }
         }

         if (s_addDebugGraphics)
         {
            CDebug::addDebugExtent(*doc,cmp->getNormalizedExtent(),cmp->getNormalizedRotationDegrees(),"normalizedExtent");
         }         
   
         CString partNumber = cmp->getPartNumberDescriptor();
         CString compShape = cmp->getPlxShapeCode(bottomFlag && sp50Mode);
         if(false == cmp->loaded)
         {
            partNumber += s_UnloadedCompPNSuffix;
            compShape += s_UnloadedCompShapeSuffix;
         }

         fprintf(fp, "d %8ld %8ld %13s %4s %4d %15s %15s %13s %19s\n",
            x, y, refname, cmp->getComponentTypeDescriptor(),
            rotationDegrees, partNumber, cmp->placement_machine, refname, compShape);        
         
         update_partnumber(cmp->getPartNumberDescriptor(), cmp->bnum, cmp->written, rotationDegrees);
      }
   }

   return 1;
}

/******************************************************************************
* write_f_record
*  - This optimizes the first 2 fiducal to lower left, upper rigth
*/
static int write_f_records(FILE *fp, bool bottomFlag, int sp50Mode)
{
   int   fidcnt = 0;

   if (fiducialcnt == 0)
      return 0;

   // find the furthest fiducials and write the other later.
   long  ll_x = LONG_MAX, ll_y = LONG_MAX, tr_x = -LONG_MAX, tr_y = -LONG_MAX;
	
	int i=0;
   for (i=0;i<fiducialcnt;i++)
   {
      PLXFiducial *fiducial = fiducialarray[i];
      //If l record is written then f record should be written for only first board.
      //if exportLocalFiducial is set to true and boardFiducialArray is having 
      //count more then 1 that means that we will write l record.
      if((true == s_ExportLocalFiducial)
         && (!boardFiducialArray.empty()) 
         && (1 != fiducial->boardNumber))
         continue;
      // fiducial layer 1 = top only, 2 bottom only, 3 both
      if (bottomFlag)
      {
         if (fiducial->layer == 1)
            continue;
      }
      else
      {
         if (fiducial->layer == 2)
            continue;
      }

      if (ll_x > fiducial->x) ll_x = fiducial->x;
      if (ll_y > fiducial->y) ll_y = fiducial->y;
      if (tr_x < fiducial->x) tr_x = fiducial->x;
      if (tr_y < fiducial->y) tr_y = fiducial->y;
   }
   
   int last_ll_dist = LONG_MAX;
   int last_tr_dist = LONG_MAX;
   int ll_ptr = -1, tr_ptr = -1;

   // find the lower left and upper right closed fid.
   for (i=0;i<fiducialcnt;i++)
   {
      PLXFiducial *fiducial = fiducialarray[i];

      //If l record is written then f record should be written for only first board.
      //if exportLocalFiducial is set to true and boardFiducialArray is having 
      //count more then 1 that means that we will write l record.
      if((true == s_ExportLocalFiducial)
         && (!boardFiducialArray.empty()) 
         && (1 != fiducial->boardNumber))
         continue;

      // fiducial layer 1 = top only, 2 bottom only, 3 both
      if (bottomFlag)
      {
         if (fiducial->layer == 1)
            continue;
      }
      else
      {
         if (fiducial->layer == 2)
            continue;
      }

      long ll_dist = (fiducial->x - ll_x) + (fiducial->y - ll_y);
      long tr_dist = (tr_x - fiducial->x) + (tr_y - fiducial->y);

      if (ll_dist < last_ll_dist)
      {
         ll_ptr = i;
         last_ll_dist = ll_dist;
      }

      if (tr_dist < last_tr_dist)
      {
         tr_ptr = i;
         last_tr_dist = tr_dist;
      }
   }

   // here swap ll_ptr as first and tr_ptr as second.
   if (ll_ptr > -1 && tr_ptr > -1)
   {
      if (ll_ptr != tr_ptr)
      {
         PLXFiducial *tmp = fiducialarray[0];
         fiducialarray[0] = fiducialarray[ll_ptr];
         fiducialarray[ll_ptr] = tmp;

         tmp = fiducialarray[1];
         fiducialarray[1] = fiducialarray[tr_ptr];
         fiducialarray[tr_ptr] = tmp;      
      }
   }

   for (i=0;i<fiducialcnt;i++)
   {
      PLXFiducial *fiducial = fiducialarray[i];

      //If l record is written then f record should be written for only first board.
      //if exportLocalFiducial is set to true and boardFiducialArray is having 
      //count more then 1 that means that we will write l record.
      if((true == s_ExportLocalFiducial)
         && (!boardFiducialArray.empty()) 
         && (1 != fiducial->boardNumber))
         continue;
      // fiducial layer 1 = top only, 2 bottom only, 3 both
      if (bottomFlag)
      {
         if (fiducial->layer == 1)
            continue;
      }
      else
      {
         if (fiducial->layer == 2)
            continue;
      }

      long  x = fiducial->x, y = fiducial->y;

      if (!sp50Mode)
         cnv_aoi(bottomFlag, fiducial->x, fiducial->y, &x, &y);
      else if (sp50Mode && bottomFlag)  //if it is on the bottom and sp50, flip the ...
         x = -x;	// case 1806, was y = -y, see also write_d_records(), sometime earlier someone
						// changed y = -y to x = -x there as well, now this is consistent with that.

      fidcnt++;

      if (fidcnt == 4)
         fprintf(fp,"# other fiducials\n");

      if (fidcnt > 3)
            fprintf(fp,"# ");

#ifndef DEBUG_FIDS
      fprintf(fp,"f %8ld %8ld\n", x, y);
#else
		fprintf(fp,"f %8ld %8ld Ref(%s) Geom(%s)\n", x, y, fiducial->compname, fiducial->geomname);
#endif

   }
      
   if (fidcnt < 2)
      fprintf(fp, "# Not enough fiducials found!\n");

   fprintf(fp,"\n");

   return 1;
}

/******************************************************************************
* write_l_record
*  - 
*/
static int write_l_records(FILE *fp, bool bottomFlag, int sp50Mode)
{
   int   fidcnt = 0;
   std::multimap<int,PLXFiducial *>::iterator it;
   if (fiducialcnt == 0)
      return 0;

   // find the furthest fiducials and write the other later.
   std::set<int>::iterator boardNumberIter = boardNumbers.begin();
   for( ; boardNumberIter != boardNumbers.end(); boardNumberIter++)
   {
      //No l record for first board and Panel fiducials.
      if((1 == (*boardNumberIter)) || (0 ==(*boardNumberIter)))
         continue;
      fidcnt = 0;
      long  ll_x = LONG_MAX, ll_y = LONG_MAX, tr_x = -LONG_MAX, tr_y = -LONG_MAX;

      std::pair<std::multimap<int,PLXFiducial *>::iterator,std::multimap<int,PLXFiducial *>::iterator> ret;
      ret = boardFiducialArray.equal_range(*boardNumberIter);
      for(it=ret.first; it!=ret.second; ++it)
      {
         PLXFiducial *fiducial = (*it).second;         

         // fiducial layer 1 = top only, 2 bottom only, 3 both
         if (bottomFlag)
         {
            if (fiducial->layer == 1)
               continue;
         }
         else
         {
            if (fiducial->layer == 2)
               continue;
         }

         if (ll_x > fiducial->x) ll_x = fiducial->x;
         if (ll_y > fiducial->y) ll_y = fiducial->y;
         if (tr_x < fiducial->x) tr_x = fiducial->x;
         if (tr_y < fiducial->y) tr_y = fiducial->y;
      }

      int last_ll_dist = LONG_MAX;
      int last_tr_dist = LONG_MAX;
      std::multimap<int,PLXFiducial *>::iterator ll_ptr, tr_ptr;
      ll_ptr = boardFiducialArray.end();
      tr_ptr = boardFiducialArray.end();
      // find the lower left and upper right closed fid.
      for (it=ret.first; it!=ret.second; ++it)
      {
         PLXFiducial *fiducial = (*it).second;

         // fiducial layer 1 = top only, 2 bottom only, 3 both
         if (bottomFlag)
         {
            if (fiducial->layer == 1)
               continue;
         }
         else
         {
            if (fiducial->layer == 2)
               continue;
         }

         long ll_dist = (fiducial->x - ll_x) + (fiducial->y - ll_y);
         long tr_dist = (tr_x - fiducial->x) + (tr_y - fiducial->y);

         if (ll_dist < last_ll_dist)
         {
            ll_ptr = it;
            last_ll_dist = ll_dist;
         }

         if (tr_dist < last_tr_dist)
         {
            tr_ptr = it;
            last_tr_dist = tr_dist;
         }
      }

      // here swap ll_ptr as first and tr_ptr as second.
      if (ll_ptr !=boardFiducialArray.end() && tr_ptr != boardFiducialArray.end())
      {
         if (ll_ptr != tr_ptr)
         {
            it=ret.first;
            PLXFiducial *tmp = (*it).second;
            (*it).second = (*ll_ptr).second;
            (*ll_ptr).second = tmp;
            ++it;
            tmp = (*it).second;
            (*it).second = (*tr_ptr).second;
            (*tr_ptr).second = tmp;
         }
      }
      CString fid1,fid2;
      for (it=ret.first; it!=ret.second; ++it)
      {
         PLXFiducial *fiducial = (*it).second;

         // fiducial layer 1 = top only, 2 bottom only, 3 both
         if (bottomFlag)
         {
            if (fiducial->layer == 1)
               continue;
         }
         else
         {
            if (fiducial->layer == 2)
               continue;
         }

         long  x = fiducial->x, y = fiducial->y;

         if (!sp50Mode)
            cnv_aoi(bottomFlag, fiducial->x, fiducial->y, &x, &y);
         else if (sp50Mode && bottomFlag)  //if it is on the bottom and sp50, flip the ...
            x = -x;  // case 1806, was y = -y, see also write_d_records(), sometime earlier someone
         // changed y = -y to x = -x there as well, now this is consistent with that.

         fidcnt++;
         if (fidcnt == 4)
            fprintf(fp,"# other fiducials\n");

         if (fidcnt > 3)
            fprintf(fp,"# ");

         fprintf(fp,"l %8ld %8ld %8ld\n", x, y, fiducial->boardNumber-1);
      }
      if(fidcnt > 0)
         fprintf(fp,"\n");
   }
   fprintf(fp,"\n");
   return 1;
}

/******************************************************************************
* write_b_records
*/
static int write_b_records(FILE *fp, bool bottomFlag, int sp50Mode)
{

   if (rejectmarkarray.GetCount() > 0)
   {
      for (int i = 0; i < rejectmarkarray.GetCount(); i++)
      {
         PLXRejectMark *mark = rejectmarkarray[i];

         long x = mark->x;
         long y = mark->y;

         if (!sp50Mode)
            cnv_aoi(bottomFlag, mark->x, mark->y, &x, &y);
         else if (sp50Mode && bottomFlag)  //if it is on the bottom and sp50, flip the ...
            x = -x;

         fprintf(fp,"B %8ld %8ld %s\n", x, y, mark->refname);
      }

      fprintf(fp,"\n");
   }

   return 1;
}


//--------------------------------------------------------------
//
//
static int PLX_GetPinInfo(CDataList *DataList, double insert_x, double insert_y,
                             double rotation, int mirror, double scale,
                             int insertLayer)
{
   DataStruct *np;
   Mat2x2 m;
   Point2 point2;
   int typ = 0;   

   RotMat2(&m, rotation);

   for (CDataListIterator insertIterator(*DataList,dataTypeInsert);insertIterator.hasNext();)
   {
      np = insertIterator.getNext();

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
      double block_rot = rotation + np->getInsert()->getAngle();
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror) point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x,insert_y);

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      if (np->getInsert()->getInsertType() == insertTypePin)
      {
         PLX_Terminal *t = new PLX_Terminal;
         terminalarray.SetAtGrow(terminalcnt, t);
         terminalcnt++;

         long  x_cent_offset = 0;
         long  y_cent_offset = 0;

         t->x = round(point2.x+x_cent_offset);
         t->y = round(point2.y+y_cent_offset);
         t->rot = round(RadToDeg(block_rot));

         t->pinname = np->getInsert()->getRefname();
         t->pinnr = 0;
         t->padstackname = block->getName();
      }

      //if (np->getInsert()->getInsertType() != 0)  break;   // do not do other contens

      // if inserting an aperture
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         if (block->getShape() == T_COMPLEX)
         {
            int block_layer;

            if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
               block_layer = np->getLayerIndex();
            else 
               block_layer = insertLayer;

            BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));

            PLX_GetPinInfo( &(subblock->getDataList()), point2.x, point2.y,  
                           block_rot, mirror, scale * np->getInsert()->getScale(), block_layer);
         }
      }
      else // not aperture
      {
         int block_layer;

         if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
            block_layer = np->getLayerIndex();
         else 
            block_layer = insertLayer;

         PLX_GetPinInfo( &(block->getDataList()), point2.x, point2.y,  
                           block_rot, mirror, scale * np->getInsert()->getScale(), block_layer);
      } // end else not aperture
   } // end for

   return 1;
} /* end PLX_GetPinInfo */

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
static int PLX_GetPADSTACKData(CDataList *DataList, int insertLayer, 
                           double *drill, double scale, PLXPadstack *padstack)
{
   DataStruct *np;
   int layer;
   int typ = 0;   

   *drill = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // no etch data.
      if (np->getGraphicClass() == GR_CLASS_ETCH)
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();
      }

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == insertTypePcbComponent)
               break;

            if (np->getInsert()->getInsertType() == insertTypeVia)
               break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_TOOL))
            {
               *drill = block->getToolSize() * scale;
            }
            else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               layer = np->getLayerIndex();

               if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
               {
                  // block_layer is not set.
                  if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     if (block->getShape() == T_COMPLEX)
                     {
                        block = doc->Find_Block_by_Num((int)block->getSizeA());
                     }

                     layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
                  }
               }

               if (layer == -1)
                  layer = insertLayer;

               if (layer < 0) break;
               LayerStruct *l = doc->getLayerArray()[layer];

               long     sizeA, sizeB , Xoffset, Yoffset;
               int      shape = block->getShape();

               doc->CalcBlockExtents(block);

               sizeA = round((block->getXmax() - block->getXmin())*scale);
               sizeB = round((block->getYmax() - block->getYmin())*scale);
                  
               Xoffset = round(block->getXmin()*scale) + sizeA/2;
               Yoffset = round(block->getYmin()*scale) + sizeB/2;

               // only evaluate this type of apertures;
               if (shape != T_ROUND && 
                   shape != T_SQUARE && 
                   shape != T_RECTANGLE && 
                   shape != T_OCTAGON && 
                   shape != T_OBLONG &&
                   shape != T_DONUT)   
               {
                  // complex aperture not evaluated in DRC - get padstack ???
                  shape   = T_RECTANGLE;
               }
         
               if (l->getLayerType() == LAYTYPE_PAD_TOP ||
                   l->getLayerType() == LAYTYPE_SIGNAL_TOP)
               {
                  if (l->getNeverMirror()) 
                     typ |= 0x8;
                  else
                     typ |= 0x1;

                  padstack->shapetypetop = shape;

                  padstack->xsizetop = sizeA;
                  padstack->ysizetop = sizeB;
                  padstack->xofftop = Xoffset;
                  padstack->yofftop = Yoffset;
               }
               else if (l->getLayerType() == LAYTYPE_PAD_BOTTOM ||
                        l->getLayerType() == LAYTYPE_SIGNAL_BOT)
               {
                  if (l->getMirrorOnly())  
                     typ |= 0x10;
                  else
                     typ |= 0x2;

                  padstack->shapetypebot = shape;

                  padstack->xsizebot = sizeA;
                  padstack->ysizebot = sizeA;
                  padstack->xoffbot = Xoffset;
                  padstack->yoffbot = Yoffset;
               }
               else if (l->getLayerType() == LAYTYPE_PAD_ALL ||
                        l->getLayerType() == LAYTYPE_SIGNAL_ALL ||
                        l->getLayerType() == LAYTYPE_PAD_OUTER)
               {
                  typ |= 0x3;
                  padstack->shapetypetop = shape;

                  padstack->xsizetop = sizeA;
                  padstack->ysizetop = sizeB;
                  padstack->xofftop = Xoffset;
                  padstack->yofftop = Yoffset;

                  padstack->shapetypebot = shape;
                  padstack->xsizebot = sizeA;
                  padstack->ysizebot = sizeB;
                  padstack->xoffbot = Xoffset;
                  padstack->yoffbot = Yoffset;

               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
 } /* end PLX_GetPadstackData */


/******************************************************************************
* do_padstacks
*/
static int do_padstacks(double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         double drillsize;
         PLXPadstack *c = new PLXPadstack;
         padstackarray.SetAtGrow(padstackcnt,c);  
         padstackcnt++;   

         c->name = block->getName();
         c->block_num = block->getBlockNumber();
         c->shapetypebot = -1;
         c->shapetypetop = -1;

         int typ = PLX_GetPADSTACKData( &(block->getDataList()), -1, 
                     &drillsize, scale, c);

         c->typ = typ;
         c->drill = drillsize;
      }
   }

   return 1;
}

/******************************************************************************
* get_padstackindex
*/
static int get_padstackindex(const char *p)
{
   
   for (int i=0; i<padstackcnt; i++)
   {
      if (!padstackarray[i]->name.CompareNoCase(p))
         return i;
   }

   fprintf(flog, "Padstack [%s] not found in index\n", p);
   display_error++;

   return -1;
}

/******************************************************************************
* get_padarea
*/
static int get_padarea(const char *padstackname, int rot, int bottom, long *px, long *py)
{
   int ptr = get_padstackindex(padstackname);

   if (ptr < 0)
      return 0;

   PLXPadstack *padStack = padstackarray[ptr];

   if (!padStack)
      return 0;

   if (bottom)
   {
      if (padStack->shapetypebot < 0)
         return 0;

      *px = padStack->xsizebot;
      *py = padStack->ysizebot;
   }
   else
   {
      if (padStack->shapetypetop < 0)
         return 0;

      *px = padStack->xsizetop;
      *py = padStack->ysizetop;
   }

   return 1;
}

/*****************************************************************************/
/*
type c161          90
!tag 1/2 DX 1/2DY  CX   CY  Corner pad_type    Direction
pad    254   254  -508     0 1 pa_0508x0508f100 w
pad    254   254   508     0 1 pa_0508x0508f100 e
!
*/
static int write_tpp_records(FILE *fp, bool bottomFlag, double scale)
{

   for (int i=0; i<partcnt; i++)
   {
      PLXPart *part = partarray[i];

      if (!part)
         continue;

      if (bottomFlag)
      {
         if (!(part->written & 2))  // was not written on bottom
            continue;
      }
      else
      {
         if (!(part->written & 1))  
            continue;
      }

      // here write
      fprintf(fp, "type %s    %d\n", part->partnumber, part->rotation);

      // now loop through all pads.
      terminalcnt = 0;

      BlockStruct *block = doc->Find_Block_by_Num(part->bnum);

      if (!block)
         continue;

      PLX_GetPinInfo(&(block->getDataList()), 0.0, 0.0, 0, 0, scale, -1);
		
		int t=0;
      for (t=0; t<terminalcnt; t++)
      {
         PLX_Terminal *terminal = terminalarray[t];

         long x, y, px, py;
         cnv_aoi(bottomFlag, terminal->x, terminal->y, &x, &y);

         // direction field not needed.
         if (get_padarea(terminal->padstackname, terminal->rot, bottomFlag, &px, &py))
         {
            CString appform;
            appform.Format("pa_%04dx%04df%03d", px, py, 100);

            if (px <= 0 || py <= 0)
            {
               fprintf(flog, "Warning: Zero pad size encountered.\n");
               display_error++;
            }

            fprintf(fp,"pad %4ld %4ld %8ld %8ld 0 %s\n", px/2, py/2, x, y, appform);
         }
         else
         {
            fprintf(fp, "! No Padsize found for %s\n", (bottomFlag ? "bottom" : "top"));
            fprintf(fp, "pad ? ? %8ld %8ld 0 %s\n", x, y, terminal->padstackname);
         }
      }
      
      for (t=0; t<terminalcnt; t++)
         delete terminalarray[t];

      terminalcnt = 0;

      fprintf(fp, "\n");
   }

   return 1;
}

/******************************************************************************
* write_tpp_file
!
! .tpp file is a "type pad" file ( or a footprint file )
!
! Write one to match each .plx file ( thus one ( plx and .tpp ) for the top and another pair for the bottom.
!
! You can leave the corner and direction fields blank, 0.
!
! tag type      angle ( as in plx )
!  or part number in CPlxCompPin record
!
type c161          90
!tag 1/2 DX 1/2DY  CX   CY  Corner pad_type    Direction
pad    254   254  -508     0 1 pa_0508x0508f100 w
pad    254   254   508     0 1 pa_0508x0508f100 e
!

   Need to write every partnumber for every rotation.
*/
static int write_tpp_file(const char *pathname, double scale)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   // need to do this, because panel only knows the path.ext
   _splitpath( pathname, drive, dir, fname, ext );

   char     topfile[_MAX_PATH];
   char     bottomfile[_MAX_PATH];
   CString  filename;

   filename.Format("%s_top", fname);
   _makepath(topfile,drive,dir,filename,".tpp");
   filename.Format("%s_bot", fname);
   _makepath(bottomfile,drive,dir,filename,".tpp");

   FILE  *tfp, *bfp;

   if ((tfp = fopen(topfile, "wt")) == NULL) // rewrite file
   {
      MessageBox(NULL, "Can not open Topfile !", topfile, MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   if ((bfp = fopen(bottomfile, "wt")) == NULL) // rewrite file
   {
      MessageBox(NULL, "Can not open Bottomfile !", bottomfile, MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   CTime t;
   t = t.GetCurrentTime();
   fprintf(tfp,"! Created : %s\n",t.Format("%A, %B %d, %Y at %H:%M:%S"));
   fprintf(tfp,"! Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(tfp,"\n");
   fprintf(tfp,"! Top Layer\n\n");
   fprintf(tfp,"\n\n");

   fprintf(bfp,"! Created : %s\n",t.Format("%A, %B %d, %Y at %H:%M:%S"));
   fprintf(bfp,"! Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(bfp,"\n");
   fprintf(bfp,"! Bottom Layer\n\n");
   fprintf(bfp,"\n\n");

   write_tpp_records(tfp, 0, scale);   // top
   write_tpp_records(bfp, 1, scale);   // bottom

   fprintf(tfp,"\n");
   fprintf(tfp,"! End Top Layer\n\n");
   fprintf(bfp,"\n");
   fprintf(bfp,"! End Bottom Layer\n\n");

   fclose(tfp);
   fclose(bfp);

   return 1;
}

/******************************************************************************
* do_boardarray
*
*/
static int do_boardarray(FileStruct *file, double insert_x, double insert_y, double rotation, double scale,FormatStruct* format)
{
   bool mixedFlag = (format->AOI.machine == AoiMachineMixed);

   int rejectMarkCount = 0;

   DataStruct  *np;
   int         boardnumber = 0;
   Mat2x2      m;
   RotMat2(&m, rotation);

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);

      //if in mixed mode, check the LOADED attribute, if it is false, skip!
      if (mixedFlag)
      {
         int loaded = FALSE;
         Attrib *loadedAttrib = is_attvalue(doc, np->getAttributesRef(),ATT_LOADED, 1);

         if (loadedAttrib)
         {
            CString test = get_attvalue_string(doc, loadedAttrib);

            if (!test.CompareNoCase("TRUE")) // TRUE
               loaded = TRUE;
         }

         if (!loaded && loadedAttrib)
            continue;
      }

      if (np->getDataType() != T_INSERT)  
         continue;

      if (np->getInsert()->getInsertType() == insertTypePcb)   
      {
         boardnumber++;

         Point2 point2;

         point2.x = np->getInsert()->getOriginX() * scale;
         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);
         
         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = np->getInsert()->getMirrorFlags();

         if (np->getInsert()->getMirrorFlags())
            block_mirror = MIRROR_FLIP | MIRROR_LAYERS;

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         // insert if mirror is either global mirror or block_mirror, but not if both.
         double block_rot = rotation + np->getInsert()->getAngle();

         PLX_GetData(file,&(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(), 1, boardnumber,format);
      }
      else if (np->getInsert()->getInsertType() == insertTypeFiducial)    // panel fiducial
      {
         Point2 point2;

         point2.x = np->getInsert()->getOriginX() * scale;
         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = np->getInsert()->getMirrorFlags();

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         // NONE OF THE RESUSLT OF THE FOLLOWING CODE BLOCK SEEMS TO BE USED FOR ANYTHING
         //double block_rot = rotation + np->getInsert()->getAngle();
         // xrot is the centroid delta
         //double xrot, yrot, xpos, ypos;
         //get_centroid(block->getName(), &xrot, &yrot);
         //Rotate(xrot, yrot, RadToDeg(block_rot), &xpos, &ypos);
         //double x_cent_offset = xpos;
         //double y_cent_offset = ypos;

         PLXFiducial *c = new PLXFiducial;
         fiducialarray.SetAtGrow(fiducialcnt,c);  
         fiducialcnt++; 

         int layer = 1;
               
         // if a fiducial is a gerber aperture, it is placed on a top or bottom layer
         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            int lnr = Get_ApertureLayer(doc, np, block, -1);

            if ( doc->get_layer_visible(lnr, block_mirror) )
            {
               LayerStruct *ll = doc->FindLayer(lnr);

               if (ll != NULL && is_layerbottom(ll->getLayerType()))
                  layer = 2;
            }
         }
         else
         {
            // if it is an normal insert 
            layer = get_fiducial_layer(block, block_mirror);
         }

         Attrib *a =  is_attvalue(doc, np->getAttributesRef(),ATT_FIDUCIAL_PLACE_BOTH, 2);

         if (a)
            layer = 3;

         c->x = round(point2.x);
         c->y = round(point2.y);
         c->layer = layer;
         c->compname = np->getInsert()->getRefname();
         c->geomname = block->getName();
         c->boardNumber = 0;
         c->boardFiducial = false;
      }
      else if (np->getInsert()->getInsertType() == insertTypeXout)    // reject mark (why are we being asked to use insertTypeXout instead of insertTypeRejectMark ?)
      {
         rejectMarkCount++;

         CString  refname = np->getInsert()->getRefname();

         Point2 point2;

         point2.x = np->getInsert()->getOriginX() * scale;
         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         PLXRejectMark *c = new PLXRejectMark;
         rejectmarkarray.Add(c);  

         c->x = round(point2.x);
         c->y = round(point2.y);
         c->refname = np->getInsert()->getRefname();
         if (c->refname.IsEmpty())
            c->refname.Format("Unnamed%d", rejectMarkCount);
      }
   }


   return 1;
}

/******************************************************************************
* check_fiducials
*/
static int check_fiducials() 
{
   int topfid = 0, botfid = 0;

   for (int i=0;i<fiducialcnt;i++)
   {
      PLXFiducial *fiducial = fiducialarray[i];

      if (fiducial->layer & 1)   topfid++;
      if (fiducial->layer & 2)   botfid++;
   }

   if (topfid < 2)
   {
      fprintf(flog, "Not enough fiducials found! Only %d of %d required fiducials found on TOP.\n", topfid, 2);
      display_error++;
   }

   if (botfid < 2)
   {
      fprintf(flog, "Not enough fiducials found! Only %d of %d required fiducials found on BOTTOM.\n", botfid, 2);
      display_error++;
   }

   return 1;
}

//_____________________________________________________________________________
int CPlxCompPin::m_stencilBaseThicknessKeywordIndex = -1;
int CPlxCompPin::m_stencilThicknessKeywordIndex     = -1;
CMapStringToString CPlxCompPin::m_checkedGeometryNameMap;
bool CPlxCompPin::m_exportPartGeometryInformationFlag = false;
bool CPlxCompPin::m_useZeroRotationForComplexAperturesFlag = false;

CPlxCompPin::CPlxCompPin(FileStruct& pcbFile, AoiMachineTag mchTag) :
   m_pcbFile(pcbFile)
{
   m_geometry       = NULL;
   m_component      = NULL;
   m_type           = plxTypeUndefined;
   m_complexPolygon = NULL;
   m_stencilBaseThickness = -1.;
   m_fillFactor           =  1.;
	m_mchTag				= mchTag;
   loaded = true;
   //m_rotationDegrees = 0;
}

CPlxCompPin::~CPlxCompPin()
{
   delete m_complexPolygon;
}

//void CPlxCompPin::setRotationDegrees(int degrees)
//{ 
//   m_rotationDegrees = degrees; 
//}

const CBasesVector& CPlxCompPin::getBasesVector()
{
   return m_basesVector;
}

void CPlxCompPin::setBasesVector(const CBasesVector& basesVector)
{
   m_basesVector = basesVector;

   //m_x               = round(basesVector.getOrigin().x);
   //m_y               = round(basesVector.getOrigin().y);
   //m_rotationDegrees = round(basesVector.getRotationDegrees());

   invalidate();
}

void CPlxCompPin::setBasesVectorPlxUnits(const CBasesVector& basesVector)
{
   m_basesVector = basesVector;

   CTMatrix matrix;
   matrix.scaleCtm(1./getPlxUnitsFactor());
   m_basesVector.transform(matrix);

   invalidate();
}

void CPlxCompPin::setBasesVectorPlxUnits(double xOriginPlxUnits,double yOriginPlxUnits,double degrees,bool mirror,double scale)
{
	double xOrigin = xOriginPlxUnits / getPlxUnitsFactor();
	double yOrigin = yOriginPlxUnits / getPlxUnitsFactor();
   m_basesVector.set(xOrigin,yOrigin,degrees,mirror,scale);

   invalidate();
}

CString CPlxCompPin::getPlxShapeCode(bool bottomFlag) const
{
   validate();

	CString plxShapeCode;

	// Case 1922, use specified attrib for shape code (aka package) if present
	if (!s_PackageNameAttrib.IsEmpty())
	{
		if (m_component != NULL && m_component->getAttributesRef() != NULL)
		{
			CString val;
			Attrib *a;

			if ((a = is_attvalue(doc, m_component->getAttributesRef(),s_PackageNameAttrib, 0)) != NULL)
			{
				val = get_attvalue_string(doc, a);
				plxShapeCode = val;	
			}
		}
	}

	// Default, if not set by attrib above, then set old way, based on camcad geometry name
	if (plxShapeCode.IsEmpty())
	{
		CString geometryName = getShapeGeometryName();
		plxShapeCode = getCheckedGeometryName(geometryName);
	}

	// case 1802 - Only SP50 gets center-of-mass in output
	if (m_mchTag == AoiMachineSp50)
	{
		int xCenterOfMassOffset = round(m_biasOffset.x * getPlxUnitsFactor());
		int yCenterOfMassOffset = round(m_biasOffset.y * getPlxUnitsFactor());

		if (xCenterOfMassOffset != 0 || yCenterOfMassOffset != 0)
		{
			plxShapeCode.AppendFormat("$$%dy%d",
				(bottomFlag ? -xCenterOfMassOffset : xCenterOfMassOffset),
				yCenterOfMassOffset);
		}
	}


	return plxShapeCode;
}

CPoint2d CPlxCompPin::getOrigin() const
{
   CPoint2d origin = m_basesVector.getOrigin();
   origin.x *= getPlxUnitsFactor();
   origin.y *= getPlxUnitsFactor();

   return origin;
}

CString CPlxCompPin::getShapeGeometryName() const
{ 
   CString shapeGeometryName;

   if (m_exportPartGeometryInformationFlag && ! m_shapeGeometryName.IsEmpty())
   {
      shapeGeometryName = m_shapeGeometryName;
   }
   else
   {
      shapeGeometryName = m_geometryName;
   }

   return shapeGeometryName; 
}

void CPlxCompPin::setShapeGeometryName(const CString& name) 
{ 
   //if (refdes.Left(3).CompareNoCase("C2-") == 0)
   //{
   //   int iii = 3;
   //}

   m_shapeGeometryName = name;  
   invalidate(); 
}

CString CPlxCompPin::getGeometryName() const
{ 
   return m_geometryName; 
}

void CPlxCompPin::setGeometryName(const CString& name) 
{ 
   //if (refdes.Left(3).CompareNoCase("C2-") == 0)
   //{
   //   int iii = 3;
   //}

   m_geometryName = name;  
   invalidate(); 
}

CString CPlxCompPin::getCheckedGeometryName(const CString& geometryName)
{
   CString checkedGeometryName;

   if (!m_checkedGeometryNameMap.Lookup(geometryName,checkedGeometryName))
   {
      checkedGeometryName = check_name('g',geometryName);
      //checkedGeometryName = geometryName;
      m_checkedGeometryNameMap.SetAt(geometryName,checkedGeometryName);
   }

   return checkedGeometryName;
}

CString CPlxCompPin::getCheckedGeometryName() const
{
   return getCheckedGeometryName(m_geometryName);
}

void CPlxCompPin::invalidate() const
{
   delete m_complexPolygon;
   m_complexPolygon = NULL;
}

void CPlxCompPin::determineFillFactor(double shapeArea, double extentArea) const
{
	// case 1881 - Fill factor for SP1&2 and SP part of MixedMode is a set of constants, for
	// SP50 is calculated

	if (m_mchTag == AoiMachineSp50)
	{
		m_fillFactor = shapeArea / extentArea;
	}
	else
	{
		// SP 1 & 2 and MixedMode -- actually, everything else but SP50
		ApertureShapeTag shapeTag = m_geometry->getShape();
		switch (shapeTag)
		{
		case T_ROUND:
		   m_fillFactor = 0.79;
		   break;
		case T_SQUARE:
		   m_fillFactor = 1.00;
		   break;
		case T_RECTANGLE:
		   m_fillFactor = 1.00;
		   break;
		case T_OBLONG:
		   m_fillFactor = 1.00;
		   break;
		default:
		   m_fillFactor = 0.99;
		   break;
		}
	}
}

void CPlxCompPin::validate() const
{
   //if (refdes.CompareNoCase("R6-2") == 0)
   //{
   //   int iii = 3;
   //}

   if (m_complexPolygon == NULL)
   {
      m_complexPolygon = new CPolygon(getPageUnits());

      if (m_geometry != NULL)
      {
         CExtent extent = m_geometry->getExtent();

         if (!extent.isValid())
         {
            doc->CalcBlockExtents(m_geometry);
         }

         int xSize = round(extent.getXsize() * getPlxUnitsFactor());
         int ySize = round(extent.getYsize() * getPlxUnitsFactor());

         if (xSize == 0 && ySize == 0)
         {
            doc->CalcBlockExtents(m_geometry);

            extent = m_geometry->getExtent();
         }

         m_normalizedExtent = extent;
         m_normalizedExtent.transform(m_basesVector.getTransformationMatrix());

         if (m_geometry->isAperture())
         {
            if (m_geometry->getShape() == apertureComplex)
            {
               double maxArea = 0.;
               int polyCount = 0;
               BlockStruct* apertureBlock = getCamCadDoc().getBlockAt(m_geometry->getComplexApertureSubBlockNumber());

               for (POSITION dataPos = apertureBlock->getHeadDataPosition();dataPos != NULL;)
               {
                  DataStruct* data = apertureBlock->getNextData(dataPos);

                  if (data->getDataType() == dataTypePoly)
                  {
                     CPolyList* polyList = data->getPolyList();

                     for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
                     {
                        CPoly* poly = polyList->GetNext(polyPos);
                        CPolygon polygon(*poly,getPageUnits());

                        double area = polygon.getArea();

                        if (area > maxArea)
                        {
                           maxArea = area;
                           *m_complexPolygon = polygon;
                        }

                        polyCount++;
                     }
                  }
               }
            }
         }
      }

      double* geometryFillFactor;
      m_normalizedBasesVector = m_basesVector;
      //bool useMap = (m_geometry->getShape() != apertureComplex) || m_useZeroRotationForComplexAperturesFlag ||
      bool foundMapEntry = (geometryFillFactorMap->Lookup(m_geometry,geometryFillFactor) != 0);

      if (foundMapEntry)
      {
         m_fillFactor = *geometryFillFactor;
      }

      if (m_geometry != NULL && m_geometry->isAperture())
         {
            if (m_geometry->getShape() == apertureComplex)
            {
               if (m_useZeroRotationForComplexAperturesFlag)
               {
                  m_normalizedBasesVector.initRotationMirror(0.,m_basesVector.getMirror());
               }

               if (m_complexPolygon->isValid())
               {
                  m_complexPolygon->transform(m_basesVector.getTransformationMatrix());

                  if (s_addDebugGraphics)
                  {
                     CDebug::addDebugPolygon(*doc,*m_complexPolygon,"complexPolygon");
                  }

                  CPoint2d centerOfMass;
                  if (m_useZeroRotationForComplexAperturesFlag)
                  {
                     m_normalizedBasesVector.initRotationMirror(0.,m_basesVector.getMirror());

                     m_normalizedExtent = m_complexPolygon->getExtent();

                     centerOfMass   = m_complexPolygon->getCenterOfMass();
                  }
                  else
                  {
                     double deg = m_normalizedBasesVector.getRotationDegrees();

                     CTMatrix matrix;
                     m_complexPolygon->normalizeByMinimizingExtents(matrix);

                     CTMatrix zeroRotationMatrix = matrix;
                     zeroRotationMatrix.rotateDegreesCtm(-deg);

                     m_normalizedBasesVector.initRotationMirror(0.,false);
                     m_normalizedBasesVector.transform(matrix);

                     m_normalizedExtent = m_complexPolygon->getExtent();

                     CBasesVector extentBasesVector;
                     extentBasesVector.set(m_normalizedBasesVector.getOrigin().x,m_normalizedBasesVector.getOrigin().y);
                     m_normalizedExtent.transform(extentBasesVector.getTransformationMatrix());

                     CPolygon zeroRotationPoly(*m_complexPolygon); //Need this below
                     m_complexPolygon->transform(matrix);


                     // The entities above are "normalized" to fit the drawing orientations,
                     // and end up being drawn if the "debug graphics" option (in the .out file)
                     // is turned on. But they are not normalized to zero rotation. We need zero 
                     // rotation to get the correct bias offsets for non "use zero rotation" mode.
                     
                     zeroRotationPoly.transform(zeroRotationMatrix);
                     centerOfMass   = zeroRotationPoly.getCenterOfMass();
                  }

                  if (s_addDebugGraphics)
                  {
                     CDebug::addDebugPolygon(*doc,*m_complexPolygon,"normalizedPolygon");
                  }

					   determineFillFactor(m_complexPolygon->getArea(), m_normalizedExtent.getArea());
   					
					   CPoint2d centerOfExtent = m_normalizedExtent.getCenter();
					   m_biasOffset            = centerOfMass - centerOfExtent;

                  if (!foundMapEntry)
                  {
                     geometryFillFactorMap->SetAt(m_geometry,new double(m_fillFactor));
                  }
               }
            }
            else // non complex aperture
            {
               CBasesVector apertureBasesVector = m_geometry->getApertureBasesVector();

               CStandardAperture standardAperture(m_geometry,getCamCadDoc().getPageUnits());
               standardAperture.setRotationDegrees(0.);
               apertureBasesVector.transform(m_basesVector.getTransformationMatrix());
               m_normalizedBasesVector = apertureBasesVector;
               m_normalizedExtent      = standardAperture.getExtent();
               m_normalizedExtent.translate(m_normalizedBasesVector.getOrigin());

               if (!foundMapEntry)
               {
                  double area    = standardAperture.getArea();
                  CExtent extent = standardAperture.getExtent();

					   determineFillFactor(area, extent.getArea());
                  
                  geometryFillFactorMap->SetAt(m_geometry,new double(m_fillFactor));
               }
            }
         }      
   }
}

CPolygon CPlxCompPin::getComplexPolygon() const
{
   validate();

   return *m_complexPolygon;
}

CExtent CPlxCompPin::getNormalizedExtent() const 
{
   validate();

   return m_normalizedExtent; 
}

double CPlxCompPin::getFillFactor() const
{
   validate();

   return m_fillFactor;
}

int CPlxCompPin::getNormalizedRotationDegrees() const
{
   validate();

   int normalizeRotationDegrees = round(m_normalizedBasesVector.getRotationDegrees());

   return normalizeRotationDegrees;
}

int CPlxCompPin::getStencilBaseThicknessKeywordIndex()
{
   if (m_stencilBaseThicknessKeywordIndex < 0)
   {
      CMessageBoxWriteFormat errorLog;

      m_stencilBaseThicknessKeywordIndex = 
         doc->RegisterKeyWord("StencilBaseThickness",0,valueTypeUnitDouble);
   }

   return m_stencilBaseThicknessKeywordIndex;
}

int CPlxCompPin::getStencilThicknessKeywordIndex()
{
   if (m_stencilThicknessKeywordIndex < 0)
   {
      CMessageBoxWriteFormat errorLog;

      m_stencilThicknessKeywordIndex = 
         doc->RegisterKeyWord("StencilThickness",0,valueTypeUnitDouble);
   }

   return m_stencilThicknessKeywordIndex;
}

void CPlxCompPin::resetKeywordIndexes()
{
   m_stencilBaseThicknessKeywordIndex = -1;
   m_stencilThicknessKeywordIndex     = -1;
}

void CPlxCompPin::releaseResources()
{
   m_checkedGeometryNameMap.RemoveAll();
   m_checkedGeometryNameMap.InitHashTable(nextPrime2n(500),false);
}

double CPlxCompPin::getStencilBaseThickness() const
{
   if (m_stencilBaseThickness < 0.)
   {
      BlockStruct* pcbBlock = getPcbFile().getBlock();
      Attrib* attrib;

      if (pcbBlock->getDefinedAttributes()->Lookup(getStencilBaseThicknessKeywordIndex(),attrib))
      {
         m_stencilBaseThickness = attrib->getDoubleValue();
      }
      else
      {
         m_stencilBaseThickness = 0.;

         //if (::isMetric(doc->getPageUnits()))
         //{
         //   m_stencilBaseThickness = doc->convertToPageUnits(pageUnitsMilliMeters,.2);
         //}
         //else
         //{
         //   m_stencilBaseThickness = doc->convertToPageUnits(pageUnitsMils,8.);
         //}
      }
   }

   return m_stencilBaseThickness;
}

int CPlxCompPin::getDepositHeightInMicrons() const
{
   double stencilThickness;
   Attrib* attrib;

   if (m_component != NULL &&
       m_component->getAttributesRef() != NULL &&
       m_component->getAttributesRef()->Lookup(getStencilThicknessKeywordIndex(),attrib))
   {
      stencilThickness = attrib->getDoubleValue();
   }
   else
   {
      stencilThickness = getStencilBaseThickness();
   }

   int microns = round(doc->convertPageUnitsTo(pageUnitsMicroMeters,stencilThickness));

   return microns;
}

CPoint2d CPlxCompPin::getBoundingBoxCenter() const
{
   CPoint2d center = getComplexPolygon().getExtent().getCenter();

   center.x *= getPlxUnitsFactor();
   center.y *= getPlxUnitsFactor();

   return center;
}

CString CPlxCompPin::getDescriptor() const
{
   validate();

   CString descriptor;

   //if (refdes.Left(2).CompareNoCase("U2") == 0)
   //{
   //   int iii = 3;
   //}

   if (m_geometry != NULL)
   {
  //    CExtent extent;

      ////case 1047: use the block's extents
  //    //if (m_geometry->getShape() == apertureComplex)
  //    //{
  //    //   CPolygon complexPolygon = getComplexPolygon();

  //    //   if (complexPolygon.isValid())
  //    //   {
  //    //      extent = complexPolygon.getExtent();
  //    //   }
  //    //}
  //    //else
  //    //{
  //    //   CStandardAperture standardAperture(m_geometry,getCamCadDoc().getPageUnits());
  //    //   extent = standardAperture.getExtent();
  //    //}
      //extent = m_geometry->getExtent();

  //    if (!extent.isValid())
  //    {
  //       doc->CalcBlockExtents(m_geometry);
  //    }

  //    int xSize = round(extent.getXsize() * getPlxUnitsFactor());
  //    int ySize = round(extent.getYsize() * getPlxUnitsFactor());

      //if (refdes.GetLength() > 2 && refdes[0] == 'R' && (refdes[1] == '6' || refdes[1] == '7'))
      //{
      //   int iii = 3;
      //}

      int xSize = round(m_normalizedExtent.getXsize() * getPlxUnitsFactor());
      int ySize = round(m_normalizedExtent.getYsize() * getPlxUnitsFactor());

      if (xSize == 0 && ySize == 0)
      {
         doc->CalcBlockExtents(m_geometry);

         CExtent extent = m_geometry->getExtent();
         xSize = round(extent.getXsize() * getPlxUnitsFactor());
         ySize = round(extent.getYsize() * getPlxUnitsFactor());
      }

      descriptor.Format("%04dx%04df%03d",xSize,ySize,round(getFillFactor()*100.));

      int depositHeight = getDepositHeightInMicrons();

      if (depositHeight > 0)
      {
         CString depositHeightDescriptor;
         depositHeightDescriptor.Format("x%d",depositHeight);
         descriptor += depositHeightDescriptor;
      }
   }

   return descriptor;
}

CString CPlxCompPin::getPartNumberDescriptor() const
{
   CString partNumber(m_partNumber);

   switch (m_type)
   {
   case plxTypePaste:  partNumber = "P" + getDescriptor();  break;
   case plxTypeFlux:   partNumber = "F" + getDescriptor();  break;
   case plxTypeHole:   partNumber = "H" + getDescriptor();  break;
   }

   return partNumber;
}

void CPlxCompPin::setPartNumber(const CString& partNumber)
{
   m_partNumber = partNumber;
   invalidate();
}

CString CPlxCompPin::getComponentTypeDescriptor() const
{
   CString componentType(m_componentType);

   switch (m_type)
   {
   case plxTypePaste:  componentType = "pa_" + getDescriptor();  break;
   case plxTypeFlux:   componentType = "fl_" + getDescriptor();  break;
   case plxTypeHole:   componentType = "ho_" + getDescriptor();  break;
   }

   return componentType;
}

void CPlxCompPin::setComponentType(const CString& componentType)
{
   m_componentType = componentType;
   invalidate(); 
}

void fillBoardFiducialArray()
{
   boardFiducialArray.clear();
   boardNumbers.clear();
   for (int i=0;i<fiducialcnt;i++)
   {
      PLXFiducial *fiducial = fiducialarray[i];
      //l record is only for 2,3,4... board. Need not write l record for first board.
      if((true == fiducial->boardFiducial) && (0 != fiducial->boardNumber) && (1 != fiducial->boardNumber))
      {
         boardFiducialArray.insert(std::make_pair<const int,PLXFiducial*>(fiducial->boardNumber,fiducial));
         boardNumbers.insert(fiducial->boardNumber);
      }
   }
}
