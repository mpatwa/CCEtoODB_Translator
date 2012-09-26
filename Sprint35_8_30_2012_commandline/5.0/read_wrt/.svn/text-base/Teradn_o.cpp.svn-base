// $Header: /CAMCAD/4.5/read_wrt/Teradn_o.cpp 23    6/22/06 1:44p Rick Faltersack $

/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.

   This contains 7300 and 7200
*/
                              
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
#include "outline.h"
#include "teradn_o.h"
#include "centroid.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress;

static CCEtoODBDoc   *doc;
static double        unitsFactor;

static int  display_error;
static FILE *flog;

static CPolyArray polyArray;
static int        polyArrayCount;

static TECompArray   compArray;
static int           compArrayCount;

static TEGeomArray   geomArray;
static int           geomArrayCount;

static TEPanelFiducialArray   panelFiducialArray;
static int                    panelFiducialArrayCount;

static TEPanelRejectArray  panelRejectArray;
static int                 panelRejectArrayCount;

static TEPanelBoardArray   panelBoardArray;
static int                 panelBoardArrayCount;

static double panelminx, panelmaxx, panelminy, panelmaxy;

static int write_7200output(const char *p, int output_units_accuracy);
static int write_7300output(const char *p, int firstboard, BOOL Bottom, int output_units_accuracy);
static void write_7200_header_section(FILE *wfp, const char *assyname, bool bottom);
static void write_7200_pcb_section(FILE *wfp, FileStruct *file, bool bottom, int output_units_accuracy, const char *boardname);
static void write_7200_panel_section(FILE *wfp, FileStruct *file, bool bottom, int output_units_accuracy, const char *panelname);
static void write_7200_boardfiducials(FILE *wfp, bool bottom, const char *boardname, int output_units_accuracy, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
static void write_7200_boardrejects(FILE *wfp, bool bottom, const char *boardname, int output_units_accuracy, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, 
      int insertLayer);
static void write_7200_parts(FILE *wfp, bool bottom, const char *boardname, int output_units_accuracy, FileStruct *file, 
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, 
      int insertLayer);
static void write_7200_vias(FILE *wfp, bool bottom, const char *boardname, int output_units_accuracy, FileStruct *file, 
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, 
      int insertLayer);
static int write_7200_boardoutline(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
      double scale, int embeddedLevel, int insertLayer);
static int write_easyoutline(FILE *wfp, CPntList *pp, double small_delta, int output_units_accuracy, const char *boardname);
static void Get_PANELBOARDSData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
static void update_geom_list();
static int get_centroid(const char *bname, double *x, double *y);
static int get_compindex(CString compName);
static BOOL IsUniqueBoardInPanel(int  block_num, int block_mirror);
static double cnv_xunits(double x);
static double cnv_yunits(double y);
static int cnv_rot(double r);

/******************************************************************************
* TERADYNE_7200_WriteFiles
   - the path is in pathname, the prefix is in format->filename
*/
void TERADYNE_7200_WriteFiles(const char *pathname, CCEtoODBDoc *Doc,FormatStruct *format, int page_units)
{
   doc = Doc;

   display_error = FALSE;

   CString logFile = GetLogfilePath("teradyne.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      ErrorMessage( "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   compArray.SetSize(100,100);
   compArrayCount = 0;
   geomArray.SetSize(100,100);
   geomArrayCount = 0;
   panelBoardArray.SetSize(10,10);
   panelBoardArrayCount = 0;
   panelFiducialArray.SetSize(10,10);
   panelFiducialArrayCount = 0;
   panelRejectArray.SetSize(10,10);
   panelRejectArrayCount = 0;

   generate_CENTROIDS(doc);

   // TERADYNE 7200 units are in MM
   unitsFactor = Units_Factor(page_units, UNIT_MM);
   int output_units_accuracy = GetDecimals(UNIT_MM); 

   // get all block centroids
   update_geom_list();     

   write_7200output(pathname, output_units_accuracy);

   int i;
   for (i=0; i<compArrayCount; i++)
      delete compArray[i];
   compArray.RemoveAll();
   compArrayCount = 0;

   for (i=0;i<geomArrayCount;i++)
      delete geomArray[i];
   geomArray.RemoveAll();
   geomArrayCount = 0;

   for (i=0;i<panelBoardArrayCount;i++)
      delete panelBoardArray[i];
   panelBoardArray.RemoveAll();
   panelBoardArrayCount = 0;

   for (i=0;i<panelFiducialArrayCount;i++)
      delete panelFiducialArray[i];
   panelFiducialArray.RemoveAll();
   panelFiducialArrayCount = 0;

   for (i=0;i<panelRejectArrayCount;i++)
      delete panelRejectArray[i];
   panelRejectArray.RemoveAll();
   panelRejectArrayCount = 0;

   fclose(flog);

   if (display_error)
      Logreader(logFile);
}

/******************************************************************************
* write_7200output
*/
static int write_7200output(const char *p, int output_units_accuracy)
{
   FILE *topfp, *botfp;

   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   // need to do this, because panel only knows the path.ext
   _splitpath( p, drive, dir, fname, ext );

   char     topbuildpath[_MAX_PATH];
   char     bottombuildpath[_MAX_PATH];
   CString  name;
   name = fname;
   name += "_top";
   _makepath(topbuildpath,drive,dir,name,".i2i");
   name = fname;
   name += "_bot";
   _makepath(bottombuildpath,drive,dir,name,".i2i");

   if ((topfp = fopen(topbuildpath, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %s",topbuildpath);
      ErrorMessage( tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   if ((botfp = fopen(bottombuildpath, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %s",bottombuildpath);
      ErrorMessage( tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   write_7200_header_section(topfp, fname, false /*top*/);
   write_7200_header_section(botfp, fname, true /*bottom*/);
    
   // it needs panels to work
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         if (!file->isShown())  continue;
         CString  boardname;
         boardname = file->getName();
         write_7200_pcb_section(topfp, file, false /*top*/, output_units_accuracy, boardname);
         write_7200_pcb_section(botfp, file, true /*bottom*/, output_units_accuracy, boardname);
         break;   // only 1 file allowed
      }
      else
      if (file->getBlockType() == BLOCKTYPE_PANEL)
      {
         if (!file->isShown())  continue;
         CString  panelname;
         panelname = file->getName();

         Get_PANELBOARDSData(&file->getBlock()->getDataList(), file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

         write_7200_panel_section(topfp, file, false /*top*/, output_units_accuracy, panelname);
         write_7200_panel_section(botfp, file, true /*bottom*/, output_units_accuracy, panelname);

      }
   }

   fclose(topfp);
   fclose(botfp);

   fprintf(flog,"Top    AOI file [%s] created\n", topbuildpath);
   fprintf(flog,"Bottom AOI file [%s] created\n", bottombuildpath);
   display_error++;

   return 1;
}

/******************************************************************************
* write_7200_header_section
*/
static void write_7200_header_section(FILE *wfp, const char *assyname, bool bottom)
{
   fprintf(wfp,"! Optima 7200 Preferred Import format Revision 1.5\n");

   fprintf(wfp,"! Created by %s ", getApp().getCamCadTitle());

   CTime time = time.GetCurrentTime();
   fprintf(wfp,"%s\n", time.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   fprintf(wfp,"!\n\n");

   // if there is a panel, otherwise it is the board.
   // 
   fprintf(wfp,"$Begin AssemblyData\n");
   fprintf(wfp,"$H, AssyPN, AssyRev, IsTopSide, BoardPN, Length, Width , CenterX, CenterY\n");
   fprintf(wfp,"$T, Text  , Text   , Bool     , Text   , Number, Number, Number , Number\n");
   fprintf(wfp,"$N, Opt   , Opt    , Opt      , Opt    , Opt   , Opt   , Opt    , Opt\n");
   fprintf(wfp,"$D, %s\n", assyname);
   fprintf(wfp,"$END AssemblyData\n");

   fprintf(wfp,"\n\n");
}

/******************************************************************************
* write_7200_pcb_section
*/
static void write_7200_pcb_section(FILE *wfp, FileStruct *file, bool bottom, int output_units_accuracy, const char *boardname)
{
   fprintf(wfp,"$Begin BoardCoordinateSystem\n");
   fprintf(wfp,"$H, IsTopSide, AngleOffset, IsClockwise, XDirection, YDirection, AxisUnits, BoardData\n");
   fprintf(wfp,"$T, Bool     , Number     , Bool       , Text      , Text      , Text     , Text\n");
   fprintf(wfp,"$N, Opt      , Opt        , Opt        , Opt       , Opt       , Opt      , Opt\n");

   // here I try so that top and bottom coos are the same
   if (!bottom)
      fprintf(wfp,"$D, True     , 0          ,  FALSE     , East      , North     , mm       , \"%s\"\n", boardname);
   else
      fprintf(wfp,"$D, False    , 0          ,TRUE        , West      , North     , mm       , \"%s\"\n", boardname);

   fprintf(wfp,"$END BoardCoordinateSystem\n");

   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin BoardFiducials\n");
   fprintf(wfp,"$H, Name, X     , Y     , Type , BoardData\n");
   fprintf(wfp,"$T, Text, Number, Number, Text , Text\n");
   fprintf(wfp,"$N, Gen , Mrg   , Mrg   , Mrg  , Opt\n");
   // find minumum 2 fiducials
   write_7200_boardfiducials(wfp, bottom, boardname, output_units_accuracy, file, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
   fprintf(wfp,"$END BoardFiducials\n");

   fprintf(wfp,"\n\n");
   fprintf(wfp,"$Begin BoardRejectMarks\n");
   fprintf(wfp,"$H, Name, X          , Y         , Rotation, Type, Action, BoardData\n");
   fprintf(wfp,"$T, Text, Number     , Number    , Number  , Text, Text  , Text\n");
   fprintf(wfp,"$N, Gen , Mrg        , Mrg       , Opt     , Mrg , Opt   , Opt\n");
   write_7200_boardrejects(wfp, bottom, boardname, output_units_accuracy, file, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
   fprintf(wfp,"$END BoardRejectMarks\n");
   fprintf(wfp,"\n\n");


   fprintf(wfp,"$Begin Parts\n");
   fprintf(wfp,"$H, Designator, IsPlaced, PartNumber, PackageName, Centroid X , Centroid Y , Rotation , BoardData\n");
   fprintf(wfp,"$T, Text      , Bool    , Text      , Text       , Number     , Number     , Number   , Text\n");
   fprintf(wfp,"$N, Gen       , Opt     , Mrg       , Mrg        , Mrg        , Mrg        , Mrg      , Opt\n");
   write_7200_parts(wfp, bottom, boardname, output_units_accuracy, file, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
   fprintf(wfp,"$END Parts\n");

   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin Vias\n");
   fprintf(wfp,"$H, Name, X     , Y     , Type , BoardData\n");
   fprintf(wfp,"$T, Text, Number, Number, Text , Text\n");
   fprintf(wfp,"$N, Gen , Mrg   , Mrg   , Mrg  , Opt\n");
   write_7200_vias(wfp, bottom, boardname, output_units_accuracy, file, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
   fprintf(wfp,"$END Vias\n");

   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin BoardOutline\n");
   fprintf(wfp,"$H, LINE , StartX  , StartY  , EndX    , EndY     , BoardData\n");
   fprintf(wfp,"$T, Text , Number, Number, Number, Number , Text\n");
   fprintf(wfp,"$N, Req  , Req   , Req   , Req   , Req    , Opt\n");

   Outline_Start(doc);

   int outline_found = write_7200_boardoutline(&(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
   if (outline_found)
   {
      int returnCode;
      double accuracy = 0.1;  // mm
      write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), accuracy, 
         output_units_accuracy, boardname);
   }
   else
   {
      doc->CalcBlockExtents(file->getBlock());
      Point2   pmin, pmax; 
      pmin.x = file->getBlock()->getXmin() * unitsFactor;
      pmin.y = file->getBlock()->getYmin() * unitsFactor;
      pmax.x = file->getBlock()->getXmax() * unitsFactor;
      pmax.y = file->getBlock()->getYmax() * unitsFactor;

      fprintf(wfp,"$D,RECT, %1.*lf, %1.*lf, %1.*lf, %1.*lf, \"%s\"\n",
                  output_units_accuracy, pmin.x, output_units_accuracy, pmin.y, 
                  output_units_accuracy, pmax.x, output_units_accuracy, pmax.y, boardname);
   }

   Outline_FreeResults();

   fprintf(wfp,"$END BoardOutline\n");
   fprintf(wfp,"\n\n");


   fprintf(wfp,"$Begin PanelCoordinateSystem\n");
   fprintf(wfp,"$H, IsTopSide, AngleOffset, IsClockwise, XDirection, YDirection, AxisUnits\n");
   fprintf(wfp,"$T, Bool        , Number      , Bool           , Text        , Text         , Text\n");
   fprintf(wfp,"$N, Opt         , Opt         , Opt            , Opt         , Opt          , Opt\n");
   fprintf(wfp,"$END PanelCoordinateSystem\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin PanelFiducials\n");
   fprintf(wfp,"$H, Name, X          , Y         , Type\n");
   fprintf(wfp,"$T, Text, Number     , Number    , Text\n");
   fprintf(wfp,"$N, Gen , Mrg        , Mrg       , Mrg\n");
   fprintf(wfp,"$END PanelFiducials\n");
   fprintf(wfp,"\n\n");
   
   fprintf(wfp,"$Begin Panelization\n");
   fprintf(wfp,"$H, Name, X          , Y         , Rotation, BoardData\n");
   fprintf(wfp,"$T, Text, Number     , Number    , Number  , Text      \n");
   fprintf(wfp,"$N, Gen , Req        , Req       , Req     , Req\n");
   fprintf(wfp,"$END Panelization\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin PanelOutline\n");
   fprintf(wfp,"$H, LINE, StartX  , StartY  , EndX    , EndY\n");
   fprintf(wfp,"$T, Text , Number, Number, Number, Number\n");
   fprintf(wfp,"$END PanelOutline\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin PanelRejectMarks\n");
   fprintf(wfp,"$H, Name, X          , Y         , Rotation, Type, Action, CircuitImage\n");
   fprintf(wfp,"$T, Text, Number     , Number    , Number  , Text, Text  , Text         \n");
   fprintf(wfp,"$N, Gen , Mrg        , Mrg       , Opt     , Mrg , Opt   , Req\n");
   fprintf(wfp,"$END PanelRejectMarks\n");
   fprintf(wfp,"\n\n");
}

/******************************************************************************
* write_7200_panel_section
*/
static void write_7200_panel_section(FILE *wfp, FileStruct *file, bool bottom, int output_units_accuracy, const char *panelname)
{
   int   i;

   fprintf(wfp,"!!!!!!!!!!!!!! Panelization information !!!!!!!!!!!!!!!!!!!\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin PanelCoordinateSystem\n");
   fprintf(wfp,"$H, IsTopSide, AngleOffset, IsClockwise, XDirection, YDirection, AxisUnits\n");
   fprintf(wfp,"$T, Bool    , Number, Bool      , Text      , Text      , Text\n");
   fprintf(wfp,"$N, Opt     , Opt   , Opt       , Opt       , Opt       , Opt\n");

   if (!bottom)
      fprintf(wfp,"$D, True    , 0     ,FALSE      , East      , North     , mm\n");
   else
      fprintf(wfp,"$D, False   , 0     ,TRUE       , West      , North     , mm\n");
   fprintf(wfp,"$END PanelCoordinateSystem\n");

   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin PanelFiducials\n");
   fprintf(wfp,"$H, Name, X     , Y     , Type\n");
   fprintf(wfp,"$T, Text, Number, Number, Text\n");
   fprintf(wfp,"$N, Gen , Mrg   , Mrg   , Mrg\n");


   for (i=0;i<panelFiducialArrayCount;i++)
   {
#ifdef _DEBUG
TEPanelFiducial *c = panelFiducialArray[i];
#endif
      if ((!bottom || (panelFiducialArray[i]->layerFlag & 2)) &&  
          (bottom || (panelFiducialArray[i]->layerFlag & 1)))  
      {
         fprintf(wfp,"$D,\"%s\", %1.*lf, %1.*lf, \"%s\"\n",
               panelFiducialArray[i]->fiducialRef, 
               output_units_accuracy, panelFiducialArray[i]->x,
               output_units_accuracy, panelFiducialArray[i]->y,
               panelFiducialArray[i]->geomName);
      }
   }

   fprintf(wfp,"$END PanelFiducials\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin PanelRejectMarks\n");
   fprintf(wfp,"$H, Name, X          , Y         , Rotation, Type, Action, CircuitImage\n");
   fprintf(wfp,"$T, Text, Number     , Number    , Number  , Text, Text  , Text         \n");
   fprintf(wfp,"$N, Gen , Mrg        , Mrg       , Opt     , Mrg , Opt   , Req\n");
   for (i=0;i<panelRejectArrayCount;i++)
   {
#ifdef _DEBUG
TEPanelReject *c = panelRejectArray[i];
#endif
      if ((!bottom || (panelFiducialArray[i]->layerFlag & 2)) &&  
          (bottom || (panelFiducialArray[i]->layerFlag & 1))) 
      {
         fprintf(wfp,"$D,\"%s\", %1.*lf, %1.*lf, \"%s\", RejectIfMissing\n",
               panelRejectArray[i]->rejectRef, 
               output_units_accuracy, panelRejectArray[i]->x, 
               output_units_accuracy, panelRejectArray[i]->y, panelRejectArray[i]->geomName);
      }
   }
   fprintf(wfp,"$END PanelRejectMarks\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin Panelization\n");
   fprintf(wfp,"$H, Name, X     , Y     , Rotation, BoardData\n");
   fprintf(wfp,"$T, Text, Number, Number, Number  , Text\n");
   fprintf(wfp,"$N, Gen , Req   , Req   , Req     , Req\n");
   for (i=0;i<panelBoardArrayCount;i++)
   {
#ifdef _DEBUG
      TEPanelBoard *pp = panelBoardArray[i];
#endif
      fprintf(wfp,"$D,\"%s\", %1.*lf , %1.*lf, %1.1lf, \"%s\"\n",
         panelBoardArray[i]->refname, 
         output_units_accuracy, panelBoardArray[i]->board_x, 
         output_units_accuracy, panelBoardArray[i]->board_y,
         RadToDeg(panelBoardArray[i]->board_rotation), panelBoardArray[i]->geomName);
   }
   fprintf(wfp,"$END Panelization\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin PanelOutline\n");
   fprintf(wfp,"$H, RECT , StartX, StartY, EndX  , EndY\n");
   fprintf(wfp,"$T, Text , Number, Number, Number, Number\n");
   fprintf(wfp,"$N, Req  , Req   , Req   , Req   , Req\n");
   fprintf(wfp,"$D,RECT,%1.*lf,%1.*lf,%1.*lf,%1.*lf\n",
         output_units_accuracy, panelminx, output_units_accuracy, panelminy,
         output_units_accuracy, panelmaxx, output_units_accuracy, panelmaxy);
   fprintf(wfp,"$END PanelOutline\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin BoardCoordinateSystem\n");
   fprintf(wfp,"$H, IsTopSide, AngleOffset, IsClockwise, XDirection, YDirection, AxisUnits, BoardData\n");
   fprintf(wfp,"$T, Bool     , Number     , Bool       , Text      , Text      , Text     , Text\n");
   fprintf(wfp,"$N, Opt      , Opt        , Opt        , Opt       , Opt       , Opt      , Opt\n");
   for (i=0;i<panelBoardArrayCount;i++)
   {
      if (panelBoardArray[i]->Unique)
      {

         BlockStruct *block = doc->Find_Block_by_Num(panelBoardArray[i]->geomNum);

         if ( block != NULL )
         {
            FileStruct *file = doc->Find_File_by_BlockPtr(block);

            if ( file != NULL )
            {
               // here I try so that top and bottom coos are the same
               if (!bottom)
                  fprintf(wfp,"$D, True    , 0 ,FALSE, East      , North     , mm    , \"%s\"\n", panelBoardArray[i]->geomName);
               else
                  fprintf(wfp,"$D, False   , 0 ,TRUE , West      , North     , mm    , \"%s\"\n", panelBoardArray[i]->geomName);
            }
         }
      }
   }

   fprintf(wfp,"$END BoardCoordinateSystem\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin BoardFiducials\n");
   fprintf(wfp,"$H, Name, X     , Y     , Type , BoardData\n");
   fprintf(wfp,"$T, Text, Number, Number, Text , Text\n");
   fprintf(wfp,"$N, Gen , Mrg   , Mrg   , Mrg  , Opt\n");
   for (i=0;i<panelBoardArrayCount;i++)
   {
      if (panelBoardArray[i]->Unique)
      {  
         
         BlockStruct *block = doc->Find_Block_by_Num(panelBoardArray[i]->geomNum);

         if ( block != NULL )
         {
            FileStruct *file = doc->Find_File_by_BlockPtr(block);

            // find minumum 2 fiducials
            if ( file != NULL )
            {
               write_7200_boardfiducials(wfp, bottom, panelBoardArray[i]->geomName, output_units_accuracy, file, &file->getBlock()->getDataList(),
                     0, 0, 0, file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
            }
         }
      }
   }
   fprintf(wfp,"$END BoardFiducials\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin BoardRejectMarks\n");
   fprintf(wfp,"$H, Name, X          , Y         , Rotation, Type, Action, BoardData\n");
   fprintf(wfp,"$T, Text, Number     , Number    , Number  , Text, Text  , Text\n");
   fprintf(wfp,"$N, Gen , Mrg        , Mrg       , Opt     , Mrg , Opt   , Opt\n");

   for (i=0; i<panelBoardArrayCount; i++)
   {
      if (panelBoardArray[i]->Unique)
      {  

         BlockStruct *block = doc->Find_Block_by_Num(panelBoardArray[i]->geomNum);

         if ( block != NULL )
         {
            FileStruct *file = doc->Find_File_by_BlockPtr(block);

            if ( file != NULL )
            {
               // find minumum 2 fiducials
               write_7200_boardrejects(wfp, bottom, panelBoardArray[i]->geomName, output_units_accuracy, file, &file->getBlock()->getDataList(),
                     file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 
                     0, -1);
            }
         }
      }
   }

   fprintf(wfp,"$END BoardRejectMarks\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin Parts\n");
   fprintf(wfp,"$H, Designator, IsPlaced, PartNumber, PackageName, Centroid X , Centroid Y , Rotation , BoardData\n");
   fprintf(wfp,"$T, Text      , Bool    , Text      , Text       , Number     , Number     , Number   , Text\n");
   fprintf(wfp,"$N, Gen       , Opt     , Mrg       , Mrg        , Mrg        , Mrg        , Mrg      , Opt\n");
   for (i=0;i<panelBoardArrayCount;i++)
   {
#ifdef _DEBUG
      TEPanelBoard *pp = panelBoardArray[i];
#endif
      if (panelBoardArray[i]->Unique)
      {
   
         BlockStruct *block = doc->Find_Block_by_Num(panelBoardArray[i]->geomNum);

         if ( block != NULL )
         {
            FileStruct *file = doc->Find_File_by_BlockPtr(block);

            if ( file != NULL )
            {
               write_7200_parts(wfp, bottom, panelBoardArray[i]->geomName, output_units_accuracy, file, &file->getBlock()->getDataList(), 
                    0, 0, 0, panelBoardArray[i]->board_mirror, file->getScale() * unitsFactor, 0, -1);
            }
         }
      }
   }
   fprintf(wfp,"$END Parts\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin Vias\n");
   fprintf(wfp,"$H, Name, X     , Y     , Type , BoardData\n");
   fprintf(wfp,"$T, Text, Number, Number, Text , Text\n");
   fprintf(wfp,"$N, Gen , Mrg   , Mrg   , Mrg  , Opt\n");
   for (i=0;i<panelBoardArrayCount;i++)
   {
      if (panelBoardArray[i]->Unique)
      {

         BlockStruct *block = doc->Find_Block_by_Num(panelBoardArray[i]->geomNum);

         if ( block != NULL )
         {
            FileStruct *file = doc->Find_File_by_BlockPtr(block);

            if ( file != NULL )
            {
               write_7200_vias(wfp, bottom, panelBoardArray[i]->geomName, output_units_accuracy, file, &(file->getBlock()->getDataList()), 
                     0 , 0, 0, panelBoardArray[i]->board_mirror, file->getScale() * unitsFactor, 0, -1);
            }
         }
      }
   }
   fprintf(wfp,"$END Vias\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"$Begin BoardOutline\n");
   fprintf(wfp,"$H, LINE , LeftX , TopY  , RightX, BottomY, BoardData\n");
   fprintf(wfp,"$T, Text , Number, Number, Number, Number , Text\n");
   fprintf(wfp,"$N, Req  , Req   , Req   , Req   , Req    , Opt\n");
   for (i=0;i<panelBoardArrayCount;i++)
   {
      if (panelBoardArray[i]->Unique)
      {
         BlockStruct *block = doc->Find_Block_by_Num(panelBoardArray[i]->geomNum);

         if ( block != NULL )
         {
            FileStruct *file = doc->Find_File_by_BlockPtr(block);

            if ( file != NULL )
            {
               Outline_Start(doc);

               int outline_found = write_7200_boardoutline(&(file->getBlock()->getDataList()), 
                        0.0, 0.0,0.0, panelBoardArray[i]->board_mirror, file->getScale() * unitsFactor, 0, -1);
               if (outline_found)
               {
                  int returnCode;
                  double accuracy = 0.1;  // mm
                  write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), accuracy, output_units_accuracy,
                        panelBoardArray[i]->geomName);
               }
               else
               {
                  doc->CalcBlockExtents(file->getBlock());
                  Point2   pmin, pmax; 
                  pmin.x = file->getBlock()->getXmin() * unitsFactor;
                  pmin.y = file->getBlock()->getYmin() * unitsFactor;
                  pmax.x = file->getBlock()->getXmax() * unitsFactor;
                  pmax.y = file->getBlock()->getYmax() * unitsFactor;

                  // here need to offset

                  fprintf(wfp,"$D,RECT, %1.*lf, %1.*lf, %1.*lf, %1.*lf, \"%s\"\n",
                           output_units_accuracy, pmin.x, output_units_accuracy, pmin.y, 
                           output_units_accuracy, pmax.x, output_units_accuracy, pmax.y, panelBoardArray[i]->geomName);
               }

               Outline_FreeResults();
            }
         }
      }
   }  
   fprintf(wfp,"$END BoardOutline\n");
   fprintf(wfp,"\n\n");

   fprintf(wfp,"\n\n");
}

/******************************************************************************
* write_7200_boardfiducials
*/
static void write_7200_boardfiducials(FILE *wfp, bool bottom, const char *boardname, int output_units_accuracy, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   int fiducialCount = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() == T_INSERT)
      {

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot = rotation + data->getInsert()->getAngle();

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (data->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
         {
            int layer = 1;

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int lnr = Get_ApertureLayer(doc, data, block, -1);

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
               if (block_mirror & MIRROR_FLIP)
                  layer = 2;
            }  

            if ((!bottom || (layer & 2)) &&
                (bottom || (layer & 1)))
            {
          
               CString fid = data->getInsert()->getRefname();
               if (!strlen(fid))
                  fid.Format("fid.%d", ++fiducialCount);

               fprintf(wfp,"$D,\"%s\", %1.*lf, %1.*lf, \"%s\", \"%s\"\n", fid, output_units_accuracy, point2.x, 
                  output_units_accuracy, point2.y, block->getName(), boardname);
            }
         } 
      }
   }
}

/******************************************************************************
* write_7200_boardrejects
*/
static void write_7200_boardrejects(FILE *wfp, bool bottom, const char *boardname, int output_units_accuracy, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, 
      int insertLayer)
{
   int rejectCount = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (data->getInsert()->getInsertType() == INSERTTYPE_REJECTMARK)
      {
         int layer = 1;

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            int lnr = Get_ApertureLayer(doc, data, block, -1);

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
            if (block_mirror & MIRROR_FLIP)
               layer = 2;
         }

         if (bottom &&  !(layer & 2))  break;
         if (!bottom && !(layer & 1))  break;
         
         CString rejectRef = data->getInsert()->getRefname();
         if (rejectRef.IsEmpty())
            rejectRef.Format("rej.%d", ++rejectCount);

         fprintf(wfp,"$D,\"%s\", %1.*lf, %1.*lf, \"%s\", \"%s\", RejectIfMissing\n", rejectRef, output_units_accuracy,
               point2.x, output_units_accuracy, point2.y, block->getName(), boardname);
      }
   }
}

/******************************************************************************
* write_7200_parts
*/
static void write_7200_parts(FILE *wfp, bool writingBottomOutput, const char *boardname, int output_units_accuracy, FileStruct *file, 
      CDataList *DataList, double insert_x, double insert_y, double filerotation, int filemirror, double filescale, int embeddedLevel, 
      int insertLayer)
{
   int compAttCount = 0;

   Mat2x2 m;
   RotMat2(&m, filerotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * filescale;
      if (filemirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * filescale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      bool block_mirror = (filemirror ^ data->getInsert()->getMirrorFlags()) ? true : false;
      double block_rot = filerotation + data->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      CString compname = data->getInsert()->getRefname();

      // Case 2068
      // Old code here with exclusive-or caused bottom parts never to be output.
      // Problem was when both flags are TRUE it was SUPPOSED TO OUTPUT !
      // Not to mention it was xor'ing an int TRUE/FALSE against mirror flags.
      //if (writingBottomOutput ^ block_mirror)
      // The real answer is output it if we're writing top and it is not mirrored
      // or we're writing bottom and it is mirrored. In essence, output it if both
      // conditions are true or both are false, i.e. output if they equal (boolean) each 
      // other, skip it if they don't.
      if (writingBottomOutput != block_mirror)
      {
         bool top = data->getInsert()->getPlacedTop();
         continue;
      }

      CString partNumber = "PN";

      Attrib *attrib;
      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_PARTNUMBER, 2))
         partNumber = get_attvalue_string(doc, attrib);
      else if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 0))
         partNumber = get_attvalue_string(doc, attrib);

      BOOL Loaded = TRUE;
      if (attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_TERADYNE_7200_TEST, 2))
      {
         CString test = get_attvalue_string(doc, attrib);
         
         if (!test.Left(2).CompareNoCase("NO"))
         {
            Loaded = FALSE;
            fprintf(wfp, "!");
         }
      }

      if (attrib = is_attvalue(doc, data->getAttributesRef(),ATT_LOADED, 2))
      {
         CString test = get_attvalue_string(doc, attrib);
         if (!test.Left(2).CompareNoCase("FA")) // False
            Loaded = FALSE;
      }

      double xrot, yrot;
      get_centroid(block->getName(), &xrot, &yrot);

      if (block_mirror)
         xrot = -xrot;

      Point2 point1;
      point1.x = xrot;
      point1.y = yrot;
         
      Mat2x2 mm;
      RotMat2(&mm, block_rot);

      TransPoint2(&point1, 1, &mm, 0.0, 0.0);

      fprintf(wfp,"$D,\"%s\",%s,\"%s\",\"%s\",%1.*lf, %1.*lf, %1.1lf, \"%s\"\n",
            compname, Loaded?"TRUE":"FALSE", partNumber, block->getName(), output_units_accuracy, 
            point2.x+point1.x, output_units_accuracy, point2.y+point1.y, RadToDeg(block_rot), boardname);
   }
}

/******************************************************************************
* write_7200_vias
*/
static void write_7200_vias(FILE *wfp, bool bottom, const char *boardname, int output_units_accuracy, FileStruct *file, 
      CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, 
      int insertLayer)
{
   int viaCount = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      int top = block->getFlags() & BL_ACCESS_TOP;
      int bot = block->getFlags() & BL_ACCESS_BOTTOM;

      if (block_mirror)
      {
         int tmp = top;
         top = bot;
         bot = tmp;
      }

      CString via = data->getInsert()->getRefname();
      if (!strlen(via))
         via.Format("via.%d", ++viaCount);

      if (bottom && !bot)  
         continue;   // if bottom output and does not exist on bottom

      if (!bottom && !top)
         continue;   // if bottom output and does not exist on top

      fprintf(wfp,"$D,\"%s\", %1.*lf, %1.*lf, \"%s\", \"%s\"\n", via, 
         output_units_accuracy, point2.x, output_units_accuracy, point2.y, block->getName(), boardname);
   }
}

/******************************************************************************
* write_7200_boardoutline
*/
static int write_7200_boardoutline(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
      double scale, int embeddedLevel, int insertLayer)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_TEXT)    continue;
      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if (np->getGraphicClass() == GR_CLASS_ETCH)   
            continue;   

         LayerStruct *lstr = doc->getLayerArray()[layer];
         if (lstr->getLayerType() == LAYTYPE_BOARD_OUTLINE || np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
         {
            Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
            found++;
         }
      }
      else
      {
         if (np->getInsert()->getInsertType() != 0) // do not do any inside hierachies, only 
            continue;                     // flat graphics.

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
            block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            found += write_7200_boardoutline(&(block->getDataList()), point2.x, point2.y, block_rot, block_mirror, scale * np->getInsert()->getScale(),
                  embeddedLevel+1, block_layer);
         }
      }
   }

   return found;
}

/******************************************************************************
* Get_PANELBOARDSData
*/
static void Get_PANELBOARDSData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   int fiducialCount = 0;
   int rejectCount = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      switch (data->getDataType())
      {
      case T_POLY:
         if (data->getGraphicClass() == GR_CLASS_PANELOUTLINE) // found Panel Outline
         {
            // get extents
            CPoly *poly = data->getPolyList()->GetHead();
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos)
            {
               CPnt* pnt = poly->getPntList().GetNext(pntPos);

               Point2 point2;
               point2.x = pnt->x * scale;
               if (mirror)
                  point2.x = -point2.x;
               point2.y = pnt->y * scale;
               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               if (point2.x < panelminx) panelminx = point2.x;
               if (point2.x > panelmaxx) panelmaxx = point2.x;
               if (point2.y < panelminy) panelminy = point2.y;
               if (point2.y > panelmaxy) panelmaxy = point2.y;
            }
         }
         break;

      case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            switch (data->getInsert()->getInsertType())
            {
            case INSERTTYPE_PCB:
               {
                  double rot = round(RadToDeg(block_rot));
                  double drot = round(rot / 90) * 90; 
                  if (drot != rot)
                  {
                     fprintf(flog, " Board In Panel rotation [%1.0lf] adjusted to [%1.0lf]\n", rot, drot);
                     display_error++;
                     block_rot = DegToRad(drot);
                  }

                  CString pcbGeomName = block->getName();
                  if (block_mirror)
                     pcbGeomName += "_MIRROR";
               
                  BOOL Unique = IsUniqueBoardInPanel(block->getBlockNumber(), block_mirror);

                  TEPanelBoard *panelBoard = new TEPanelBoard;
                  panelBoardArray.SetAtGrow(panelBoardArrayCount++, panelBoard);  
                  panelBoard->board_x = point2.x;
                  panelBoard->board_y = point2.y;
                  panelBoard->board_rotation = block_rot;
                  panelBoard->board_mirror = block_mirror;
                  panelBoard->geomName = pcbGeomName;
                  panelBoard->geomNum = block->getBlockNumber();
                  panelBoard->refname = data->getInsert()->getRefname();
                  panelBoard->Unique = Unique;
               }
               break;

            case INSERTTYPE_FIDUCIAL:
               {
                  int layerFlag = 1;
                  if ( block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
                  {
                     int layerNum = Get_ApertureLayer(doc, data, block, -1);

                     if (doc->get_layer_visible(layerNum, mirror) )
                     {
                        LayerStruct *layer = doc->FindLayer(layerNum);

                        if (layer != NULL && is_layerbottom(layer->getLayerType()))
                           layerFlag = 2;
                     }
                  }
                  else
                  {
                     // if it is an normal insert 
                     if (block_mirror & MIRROR_FLIP)
                        layerFlag = 2;
                  }

                  CString fiducialRef = data->getInsert()->getRefname();
                  if (fiducialRef.IsEmpty())
                     fiducialRef.Format("fid.%d", ++fiducialCount);

                  TEPanelFiducial *panelFiducial = new TEPanelFiducial;
                  panelFiducialArray.SetAtGrow(panelFiducialArrayCount++, panelFiducial);  
                  panelFiducial->x = point2.x;
                  panelFiducial->y = point2.y;
                  panelFiducial->name = data->getInsert()->getRefname();
                  panelFiducial->fiducialRef = fiducialRef;
                  panelFiducial->geomName = block->getName();
                  panelFiducial->layerFlag = layerFlag;
               }
               break;

            case INSERTTYPE_REJECTMARK:
               {
                  int layerFlag = 1;
                  if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
                  {
                     int layerNum = Get_ApertureLayer(doc, data, block, -1);

                     if (doc->get_layer_visible(layerNum, mirror))
                     {
                        LayerStruct *layer = doc->FindLayer(layerNum);

                        if (layer != NULL && is_layerbottom(layer->getLayerType()))
                           layerFlag = 2;
                     }
                  }
                  else
                  {
                     // if it is an normal insert 
                     if (block_mirror & MIRROR_FLIP)
                        layerFlag = 2;
                  }

                  CString rejectRef = data->getInsert()->getRefname();
                  if (rejectRef.IsEmpty())
                     rejectRef.Format("rej.%d", ++rejectCount);

                  TEPanelReject *p = new TEPanelReject;
                  panelRejectArray.SetAtGrow(panelRejectArrayCount,p);  
                  panelRejectArrayCount++;   
                  p->x = point2.x;
                  p->y = point2.y;
                  p->name = data->getInsert()->getRefname();
                  p->rejectRef = rejectRef;
                  p->geomName = block->getName();
                  p->layerFlag = layerFlag;
               }
            }
            break;
         }
      }
   }
}

/******************************************************************************
* update_geom_list
*  - update all geometries
*/
static void update_geom_list()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)   continue;

      if (block->getFlags() & BL_WIDTH)            continue;
      if (block->getFlags() & BL_TOOL)             continue;
      if (block->getFlags() & BL_APERTURE)         continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
      if (block->getFlags() & BL_FILE)             continue;

      if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT) continue;

      DataStruct *centroid = centroid_exist_in_block(block);

      if (centroid)
      {
         TEGeom *geom = new TEGeom;
         geomArray.SetAtGrow(geomArrayCount++, geom);  
         geom->centroid_x = centroid->getInsert()->getOriginX() * unitsFactor;
         geom->centroid_y = centroid->getInsert()->getOriginY() * unitsFactor;
         geom->centroid_rotation = centroid->getInsert()->getAngle();
         geom->name = block->getName();
      }
      else
      {
         fprintf(flog, "PCB Component [%s] does not have a Centroid\n", block->getName());
         display_error++;
      }
   }
}

/******************************************************************************
* cnv_xunits
// converts from actual to TERADYNE units.
// all units are from the lower right corner with positive x going left
// and positive y going up
*/
static double cnv_xunits(double x)
{
   return -x;
}

/******************************************************************************
* cnv_yunits
// converts from actual to TERADYNE units.
// all units are from the lower right corner with positive x going left
// and positive y going up
*/
static double cnv_yunits(double y)
{
   return y;
}

/******************************************************************************
* cnv_rot
*/
static int cnv_rot(double r)
{
   int   rot = round(RadToDeg(r));
   while (rot < 0)      rot += 360;
   while (rot >= 360)   rot -= 360;

   return rot;
}

/******************************************************************************
* get_compindex
*/
static int get_compindex(CString compName)
{
   if (compName.IsEmpty()) 
      return -1;

   for (int i=0; i<compArrayCount; i++)
      if (compName == compArray[i]->name)
         return i;

   TEComp *comp = new TEComp;
   compArray.SetAtGrow(compArrayCount++, comp);  

   comp->name = compName;

   return compArrayCount-1;
}

/******************************************************************************
* get_centroid
*/
static int get_centroid(const char *bname, double *x, double *y)
{
   int   i;

   *x = 0;
   *y = 0;

   for (i=0;i<geomArrayCount;i++)
   {
      if (geomArray[i]->name.Compare(bname) == 0)
      {
         *x = geomArray[i]->centroid_x;
         *y = geomArray[i]->centroid_y;
         return 1;
      }
   }

   return 0;
}

/******************************************************************************
* IsUniqueBoardInPanel
*/
static BOOL IsUniqueBoardInPanel(int  block_num, int block_mirror)
{
   for (int i=0; i<panelBoardArrayCount; i++)
   {
      if (panelBoardArray[i]->geomNum == block_num && panelBoardArray[i]->board_mirror == block_mirror)
         return FALSE;
   }

   return TRUE;
}


/******************************************************************************
* write_easyoutline
*/
static int write_easyoutline(FILE *wfp, CPntList *pp, double small_delta, int output_units_accuracy, const char *boardname)
{
   CPnt     *p;
   polyArrayCount = 0;
   int      arcfound = 0;
   double   ARC_ANGLE_DEGREE = 5;

   polyArrayCount = 0;

   if (pp == NULL)
   {
      return 0;
   }

   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt  *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
         {
            ErrorMessage("Poly Bulge data error !");
         }
         else
         {
            p2 = pp->GetAt(pos);
                     
            ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);
            // make positive start angle.
            if (sa < 0) 
            {
               sa += PI2;
            }
            ppolycnt = 255;
            // start center
            ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));
            ++arcfound;
            for (i=0;i<ppolycnt;++i)
            {
               polyArray.SetAtGrow(polyArrayCount,ppoly[i]);
               ++polyArrayCount;
            }
         }
      }
      else
      {
         Point2   p1;
         p1.x = p->x;
         p1.y = p->y;
         p1.bulge = 0;
         polyArray.SetAtGrow(polyArrayCount,p1);
         polyArrayCount++;
      }
   }

   int done = FALSE,i;
   int px = polyArrayCount;
   while (!done)
   {
      done = TRUE;
      for (i=1;i<polyArrayCount;i++)
      {
         Point2 p1 = polyArray.ElementAt(i-1);
         Point2 p2 = polyArray.ElementAt(i);
         if (fabs(p1.x - p2.x) < small_delta && 
             fabs(p1.y - p2.y) < small_delta)
         {
               polyArray.RemoveAt(i);
               done = FALSE;
               polyArrayCount--;
               break;
         }
      }
   }
   px = polyArrayCount;

   for (i=1;i<polyArrayCount;i++)
   {
      Point2 p1, p2;
      p1 = polyArray.ElementAt(i-1);  
      p2 = polyArray.ElementAt(i);  
      fprintf(wfp,"$D,LINE, %1.*lf, %1.*lf, %1.*lf, %1.*lf, \"%s\"\n",
                  output_units_accuracy, p1.x, output_units_accuracy, p1.y, 
                  output_units_accuracy, p2.x, output_units_accuracy, p2.y, boardname);
   }     

   polyArray.RemoveAll();
   polyArrayCount = 0;

   return 1;
}

