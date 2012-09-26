// $Header: /CAMCAD/5.0/read_wrt/Trdn73_o.cpp 31    6/17/07 9:00p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "trdn73_o.h"
#include "ck.h"
#include "pcbutil.h"
#include "graph.h"
#include "xform.h"
#include "centroid.h"
#include <float.h>
#include <math.h>
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int WriteHeterogeneos7300Output(CString pathName, TEPanelBoard *boardZero, int decimals);
static int WriteHomogeneos7300Output(CString pathName, TEPanelBoard *boardZero, int decimals, BOOL HeterogeneousPanel);
static void GetPanelBoardData(FileStruct *panelFile);
static void GetFurthestFiducials(TEPanelFiducial **fiducial1, TEPanelFiducial **fiducial2);
static void UpdateGeomList();
static void ClearCompList();
static void UpdateCompList(TEPanelBoard *boardZero);
static int GetCompIndex(CString compName);
static int GetCentroid(CString geomName, double *x, double *y);
static int cnv_rot(double radians);
int GetPadPerSide(BlockStruct *block, BOOL Mirrored);

static CCEtoODBDoc *doc;
static int display_error;
static FILE *fLog;
static double unitsFactor;

static TECompArray   compArray;
static int           compArrayCount;

static TEGeomArray   geomArray;
static int           geomArrayCount;

static TEPanelFiducialArray   panelFiducialArray;
static int                    panelFiducialArrayCount;

static TEPanelBoardArray   panelBoardArray;
static int                 panelBoardArrayCount;

static double panelMinX, panelMaxX, panelMinY, panelMaxY;

/******************************************************************************
* TERADYNE_7300_WriteFiles
*  - the path is in pathname, the prefix is in format->filename
*/ 
void TERADYNE_7300_WriteFiles(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *format, int page_units)
{
   doc = Doc;

   display_error = FALSE;

   CString logFile = GetLogfilePath("teradyne.log");

   fLog = fopen(logFile, "wt");
   if (!fLog)
   {
      MessageBox(NULL, "Can not open Logfile!", logFile, MB_ICONEXCLAMATION | MB_OK);
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

   CString checkFile( getApp().getSystemSettingsFilePath("teradyneaoi.chk") );
   check_init(checkFile);

   generate_CENTROIDS(doc);
   generate_PADSTACKACCESSFLAG(doc, 0);

   // TERADYNE units are in inches
   unitsFactor = Units_Factor(page_units, UNIT_MILS);
   int decimals = GetDecimals(UNIT_MILS); 

   panelMinX = panelMinY = FLT_MAX;
   panelMaxX = panelMaxY = -FLT_MAX;

   // get the one panel file to export
   FileStruct *panelFile = NULL;
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      
      if (!file->isShown())
         continue;

      if (file->getBlockType() == BLOCKTYPE_PANEL)
      {
         if (panelFile)
         {
            ErrorMessage("Only one panel file can be on!");
            panelFile = NULL;
            break;
         }

         panelFile = file;
      }        
   }

   if (panelFile)
      GetPanelBoardData(panelFile);

   if (!panelFile)
      ErrorMessage("No Visible Panel!");
   else if (panelMinX > panelMaxX)
      ErrorMessage("No Panel Outline found!");
   else if (!panelBoardArrayCount)
      ErrorMessage("No Panel Boards found!");
   else
   {
      UpdateGeomList();

      TEPanelBoardArray uniqueBoardArray;
      int uniqueBoardArrayCount = 0;

      // find unique boards
		int i=0;
      for (i=0; i<panelBoardArrayCount; i++)
      {
         TEPanelBoard *board = panelBoardArray[i];

         BOOL Found = FALSE;
         for (int j=0; j<uniqueBoardArrayCount; j++)
         {
            TEPanelBoard *uniqueBoard = uniqueBoardArray[j];

            if (uniqueBoard->geomNum == board->geomNum && uniqueBoard->mirror == board->mirror)
            {
               Found = TRUE;
               break;
            }
         }

         if (!Found)
            uniqueBoardArray.SetAtGrow(uniqueBoardArrayCount++, board);
      }

      // homogeneous panels
      for (i=0; i<uniqueBoardArrayCount; i++)
      {
         TEPanelBoard *boardZero = uniqueBoardArray[i];

         ClearCompList();
         UpdateCompList(boardZero);

         WriteHomogeneos7300Output(pathname, boardZero, decimals, uniqueBoardArrayCount-1);
      }

      // heterogeneous panel
      if (uniqueBoardArrayCount-1)
         WriteHeterogeneos7300Output(pathname, panelBoardArray[0], decimals);
   }

	int i=0;
   for (i=0;i<compArrayCount;i++)
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

   if (check_report(fLog))
      display_error++;
   check_deinit();

   fclose(fLog);

   if (display_error)
      Logreader(logFile);
}

/******************************************************************************
* WriteHeterogeneos7300Output
*/
static int WriteHeterogeneos7300Output(CString pathName, TEPanelBoard *boardZero, int decimals)
{
/*
      top                        bottom
   |---------------|       |---------------| 
   |               |       |               |
   |           xx  |       |  xx           | 
   |___________xx__|       |__xx___________| 
*/

   // need to do this, because panel only knows the path.ext
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(pathName, drive, dir, fname, ext );

   char topFileName[_MAX_PATH];
   char botFileName[_MAX_PATH];

   _makepath(topFileName, drive, dir, fname, ".top");
   _makepath(botFileName, drive, dir, fname, ".bot");

   FILE *fTop = fopen(topFileName, "wt");
   if (!fTop)
   {
      CString tmp;
      tmp.Format("Can not open %s.", topFileName);
      ErrorMessage(tmp, "ERROR", MB_OK | MB_ICONHAND);
      return 0;
   }

   FILE *fBot = fopen(botFileName, "wt");
   if (!fBot)
   {
      CString tmp;
      tmp.Format("Can not open %s.", botFileName);
      ErrorMessage(tmp, "ERROR", MB_OK | MB_ICONHAND);
      return 0;
   }


   // board zero
   fprintf(fTop, "*%1.*lf %1.*lf %1.*lf %1.*lf\n", 
         decimals, -(boardZero->x - panelMaxX), 
         decimals, panelMaxY - boardZero->y, 
         decimals, -(panelMinX - boardZero->x), 
         decimals, boardZero->y - panelMinY);

   fprintf(fBot, "*%1.*lf %1.*lf %1.*lf %1.*lf\n", 
         decimals, -(panelMinX - boardZero->x), 
         decimals, panelMaxY - boardZero->y, 
         decimals, -(boardZero->x - panelMaxX), 
         decimals, boardZero->y - panelMinY);


   // panel fiducials
   if (panelFiducialArrayCount)
   {
      fprintf(fTop, "Fiducials:\n");
      fprintf(fBot, "Fiducials:\n");

      TEPanelFiducial *fiducial1 = NULL;
      TEPanelFiducial *fiducial2 = NULL;
      GetFurthestFiducials(&fiducial1, &fiducial2);

      if (fiducial1)
      {
         fprintf(fTop, "Fiducial1 %1.*lf %1.*lf 0\n", 
               decimals, -(fiducial1->x - boardZero->x),
               decimals, fiducial1->y - boardZero->y);

         fprintf(fBot, "Fiducial1 %1.*lf %1.*lf 0\n", 
               decimals, -(boardZero->x - fiducial1->x),
               decimals, fiducial1->y - boardZero->y);
      }
      
      if (fiducial2)
      {
         fprintf(fTop, "Fiducial2 %1.*lf %1.*lf 0\n", 
               decimals, -(fiducial2->x - boardZero->x),
               decimals, fiducial2->y - boardZero->y);

         fprintf(fBot, "Fiducial2 %1.*lf %1.*lf 0\n", 
               decimals, -(boardZero->x - fiducial2->x),
               decimals, fiducial2->y - boardZero->y);
      }
   }


   // boards
   for (int b=0; b<panelBoardArrayCount; b++)
   {
      TEPanelBoard *board = panelBoardArray[b];

      ClearCompList();
      UpdateCompList(board);

      BOOL BoardMirrored = board->mirror ? TRUE : FALSE;

      // components
      for (int c=0; c<compArrayCount; c++)  
      {
         TEComp *comp = compArray[c];

         CString compName;
         compName.Format("%s-%d", check_name('c', comp->refName), b+1); 

         FILE *f;
         if (BoardMirrored ^ comp->bottom)
            f = fBot;
         else
            f = fTop;

         double x = comp->x + boardZero->x - board->x;
         if (comp->bottom)
            x = -x;

         CString xloc, yloc, rot;
         xloc.Format("%1.*lf", decimals, -(x));
         yloc.Format("%1.*lf", decimals, comp->y + board->y - boardZero->y);
         //changed the format from %d to %1.1lf
         rot.Format("%1.1lf", RadToDeg(comp->rotation));

         fprintf(f, "%s,%s,%s,%s,%s\n", compName.GetBuffer(0), check_name('g', comp->geomName), xloc, yloc, rot);


         if (!comp->doPins)
            continue;

         BOOL Mirrored = (BoardMirrored ^ comp->bottom);

         BlockStruct *compGeom = doc->getBlockAt(comp->geomNum);

         DTransform xform(comp->x, comp->y, 1, comp->rotation, comp->bottom);

         POSITION pinPos = NULL;

         if (compGeom != NULL)
            pinPos = compGeom->getDataList().GetHeadPosition();

         while (pinPos)
         {
            DataStruct *pinData = compGeom->getDataList().GetNext(pinPos);

            if (pinData->getDataType() != T_INSERT || pinData->getInsert()->getInsertType() != INSERTTYPE_PIN || !pinData->getInsert()->getRefname())
               continue;

            double centroidX, centroidY;
            GetCentroid(compGeom->getName(), &centroidX, &centroidY);

            Point2 p;
            p.x = pinData->getInsert()->getOriginX() * unitsFactor - centroidX;
            p.y = pinData->getInsert()->getOriginY() * unitsFactor - centroidY;
            xform.TransformPoint(&p);

            double x = p.x + boardZero->x - board->x;
            if (BoardMirrored)
               x = -x;

            CString xloc, yloc, rot;
            yloc.Format("%1.*lf", decimals, p.y + board->y - boardZero->y);
            //changed the format from %d to %1.1lf
            rot.Format("%1.1lf", RadToDeg(comp->rotation + pinData->getInsert()->getAngle()));

            int padExists = GetPadPerSide(doc->getBlockAt(pinData->getInsert()->getBlockNumber()), Mirrored);

            if (comp->doPins & 1)
            {
               xloc.Format("%1.*lf", decimals, -(x));

               if (padExists & 1)
                  fprintf(fTop, "%s.%s-%d,%s,%s,%s,%s\n", check_name('c', comp->refName), pinData->getInsert()->getRefname(), b+1, 
                        check_name('g', doc->getBlockAt(pinData->getInsert()->getBlockNumber())->getName()), xloc, yloc, rot);
               else
                  fprintf(fLog, "Comp-Pin does not exist on Top [%s-%s]\n", check_name('c', comp->refName), pinData->getInsert()->getRefname());
            }

            if (comp->doPins & 2)
            {
               xloc.Format("%1.*lf", decimals, -(-x));

               if (padExists & 2)
                  fprintf(fBot, "%s.%s-%d,%s,%s,%s,%s\n", check_name('c', comp->refName), pinData->getInsert()->getRefname(), b+1, 
                        check_name('g', doc->getBlockAt(pinData->getInsert()->getBlockNumber())->getName()), xloc, yloc, rot);
               else
                  fprintf(fLog, "Comp-Pin does not exist on Bottom [%s-%s]\n", check_name('c', comp->refName), pinData->getInsert()->getRefname());
            }

         }
      }
   }

   fprintf(fTop, "%c\n", 0x4);   // teradyne end of file
   fprintf(fBot, "%c\n", 0x4);   // teradyne end of file

   fprintf(fTop, "NAME:  0 20\n");
   fprintf(fTop, "TYPE: 20 20\n");
   fprintf(fTop, "XLOC: 40 10\n");
   fprintf(fTop, "YLOC: 50 10\n");
   fprintf(fTop, "ROT:  60 10\n");

   fprintf(fBot, "NAME:  0 20\n");
   fprintf(fBot, "TYPE: 20 20\n");
   fprintf(fBot, "XLOC: 40 10\n");
   fprintf(fBot, "YLOC: 50 10\n");
   fprintf(fBot, "ROT:  60 10\n");


   fclose(fTop);
   fclose(fBot);


   fprintf(fLog, "Top Component file [%s] created\n", topFileName);
   fprintf(fLog, "Bottom Component file [%s] created\n", botFileName);
   display_error++;


   return 1;
}

/******************************************************************************
* WriteHomogeneos7300Output
*/
static int WriteHomogeneos7300Output(CString pathName, TEPanelBoard *boardZero, int decimals, BOOL HeterogeneousPanel)
{
/*
      top                        bottom
   |---------------|       |---------------| 
   |               |       |               |
   |           xx  |       |  xx           | 
   |___________xx__|       |__xx___________| 
*/

   // need to do this, because panel only knows the path.ext
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(pathName, drive, dir, fname, ext );

   char topFileName[_MAX_PATH];
   char botFileName[_MAX_PATH];

   CString fileName = fname;
   if (HeterogeneousPanel)
   {
      fileName += "_";
      fileName += boardZero->geomName;
      fileName.Replace('.', '_');
   }

   _makepath(topFileName, drive, dir, fileName, ".top");
   _makepath(botFileName, drive, dir, fileName, ".bot");

   FILE *fTop = fopen(topFileName, "wt");
   if (!fTop)
   {
      CString tmp;
      tmp.Format("Can not open %s.", topFileName);
      ErrorMessage(tmp, "ERROR", MB_OK | MB_ICONHAND);
      return 0;
   }

   FILE *fBot = fopen(botFileName, "wt");
   if (!fBot)
   {
      CString tmp;
      tmp.Format("Can not open %s.", botFileName);
      ErrorMessage(tmp, "ERROR", MB_OK | MB_ICONHAND);
      return 0;
   }


   // board zero
   fprintf(fTop, "*%1.*lf %1.*lf %1.*lf %1.*lf\n", 
         decimals, -(boardZero->x - panelMaxX), 
         decimals, panelMaxY - boardZero->y, 
         decimals, -(panelMinX - boardZero->x), 
         decimals, boardZero->y - panelMinY);

   fprintf(fBot, "*%1.*lf %1.*lf %1.*lf %1.*lf\n", 
         decimals, -(panelMinX - boardZero->x), 
         decimals, panelMaxY - boardZero->y, 
         decimals, -(boardZero->x - panelMaxX), 
         decimals, boardZero->y - panelMinY);


   // boards
   int matchingBoardCount = 0;
   for (int b=0; b<panelBoardArrayCount; b++)
   {
      TEPanelBoard *board = panelBoardArray[b];

      if (board == boardZero)
         continue;

      if (boardZero->geomNum == board->geomNum && boardZero->mirror == board->mirror)
         matchingBoardCount++;
   }

   if (matchingBoardCount)
   {
      fprintf(fTop, "&%d\n", matchingBoardCount);
      fprintf(fBot, "&%d\n", matchingBoardCount);
   
      for (int i=0; i<panelBoardArrayCount; i++)
      {
         TEPanelBoard *board = panelBoardArray[i];

         if (board == boardZero)
            continue;

         if (boardZero->geomNum != board->geomNum || boardZero->mirror != board->mirror)
            continue;

         fprintf(fTop, "%1.*lf %1.*lf %1.1lf\n", 
               decimals, -(board->x - boardZero->x),
               decimals, board->y - boardZero->y,
               RadToDeg(board->rotation - boardZero->rotation));

         fprintf(fBot, "%1.*lf %1.*lf %1.1lf\n", 
               decimals, -(boardZero->x - board->x),
               decimals, board->y - boardZero->y,
               RadToDeg(boardZero->rotation - board->rotation));
      }
   }


   // panel fiducials
   if (panelFiducialArrayCount)
   {
      fprintf(fTop, "Fiducials:\n");
      fprintf(fBot, "Fiducials:\n");

      TEPanelFiducial *fiducial1 = NULL;
      TEPanelFiducial *fiducial2 = NULL;
      GetFurthestFiducials(&fiducial1, &fiducial2);

      if (fiducial1)
      {
         fprintf(fTop, "Fiducial1 %1.*lf %1.*lf 0\n", 
               decimals, -(fiducial1->x - boardZero->x),
               decimals, fiducial1->y - boardZero->y);

         fprintf(fBot, "Fiducial1 %1.*lf %1.*lf 0\n", 
               decimals, -(boardZero->x - fiducial1->x),
               decimals, fiducial1->y - boardZero->y);
      }
      
      if (fiducial2)
      {
         fprintf(fTop, "Fiducial2 %1.*lf %1.*lf 0\n", 
               decimals, -(fiducial2->x - boardZero->x),
               decimals, fiducial2->y - boardZero->y);

         fprintf(fBot, "Fiducial2 %1.*lf %1.*lf 0\n", 
               decimals, -(boardZero->x - fiducial2->x),
               decimals, fiducial2->y - boardZero->y);
      }
   }


   BOOL BoardMirrored = boardZero->mirror ? TRUE : FALSE;

   // components
   for (int c=0; c<compArrayCount; c++)
   {
      TEComp *comp = compArray[c];

      FILE *f;
      if (BoardMirrored ^ comp->bottom)
         f = fBot;
      else
         f = fTop;

      CString xloc, yloc, rot;
      if (comp->bottom)
         xloc.Format("%1.*lf", decimals, -(-comp->x));
      else
         xloc.Format("%1.*lf", decimals, -(comp->x));
      yloc.Format("%1.*lf", decimals, comp->y);
      //changed the format from %d to %1.1lf
      rot.Format("%1.1lf", (RadToDeg(comp->rotation)));

      fprintf(f, "%s,%s,%s,%s,%s\n", check_name('c', comp->refName), check_name('g', comp->geomName), xloc, yloc, rot);


      if (!comp->doPins)
         continue;

      BOOL Mirrored = (BoardMirrored ^ comp->bottom);

      BlockStruct *compGeom = doc->getBlockAt(comp->geomNum);

      DTransform xform(comp->x, comp->y, 1, comp->rotation, comp->bottom);

      POSITION pinPos = compGeom->getDataList().GetHeadPosition();
      while (pinPos)
      {
         DataStruct *pinData = compGeom->getDataList().GetNext(pinPos);

         if (pinData->getDataType() != T_INSERT || pinData->getInsert()->getInsertType() != INSERTTYPE_PIN || !pinData->getInsert()->getRefname())
            continue;

         double centroidX, centroidY;
         GetCentroid(compGeom->getName(), &centroidX, &centroidY);

         Point2 p;
         p.x = pinData->getInsert()->getOriginX() * unitsFactor - centroidX;
         p.y = pinData->getInsert()->getOriginY() * unitsFactor - centroidY;
         xform.TransformPoint(&p);

         double x = p.x;
         if (BoardMirrored)
            x = -x;

         CString xloc, yloc, rot;
         yloc.Format("%1.*lf", decimals, p.y);
         //changed the format from %d to %1.1lf
         rot.Format("%1.1lf", RadToDeg(comp->rotation + pinData->getInsert()->getAngle()));

         int padExists = GetPadPerSide(doc->getBlockAt(pinData->getInsert()->getBlockNumber()), Mirrored);

         if (comp->doPins & 1)
         {
            xloc.Format("%1.*lf", decimals, -(x));

            if (padExists & 1)
               fprintf(fTop, "%s.%s,%s,%s,%s,%s\n", check_name('c', comp->refName), pinData->getInsert()->getRefname(), 
                     check_name('g', doc->getBlockAt(pinData->getInsert()->getBlockNumber())->getName()), xloc, yloc, rot);
            else
               fprintf(fLog, "Comp-Pin does not exist on Top [%s-%s]\n", check_name('c', comp->refName), pinData->getInsert()->getRefname());
         }

         if (comp->doPins & 2)
         {
            xloc.Format("%1.*lf", decimals, -(-x));

            if (padExists & 2)
               fprintf(fBot, "%s.%s,%s,%s,%s,%s\n", check_name('c', comp->refName), pinData->getInsert()->getRefname(), 
                     check_name('g', doc->getBlockAt(pinData->getInsert()->getBlockNumber())->getName()), xloc, yloc, rot);
            else
               fprintf(fLog, "Comp-Pin does not exist on Bottom [%s-%s]\n", check_name('c', comp->refName), pinData->getInsert()->getRefname());
         }
      }
   }


   fprintf(fTop, "%c\n", 0x4);   // teradyne end of file
   fprintf(fBot, "%c\n", 0x4);   // teradyne end of file

   fprintf(fTop, "NAME:  0 20\n");
   fprintf(fTop, "TYPE: 20 20\n");
   fprintf(fTop, "XLOC: 40 10\n");
   fprintf(fTop, "YLOC: 50 10\n");
   fprintf(fTop, "ROT:  60 10\n");

   fprintf(fBot, "NAME:  0 20\n");
   fprintf(fBot, "TYPE: 20 20\n");
   fprintf(fBot, "XLOC: 40 10\n");
   fprintf(fBot, "YLOC: 50 10\n");
   fprintf(fBot, "ROT:  60 10\n");


   fclose(fTop);
   fclose(fBot);


   fprintf(fLog, "Top Component file [%s] created\n", topFileName);
   fprintf(fLog, "Bottom Component file [%s] created\n", botFileName);
   display_error++;


   return 1;
}

/******************************************************************************
* GetPanelBoardData
*/
static void GetPanelBoardData(FileStruct *panelFile)
{
   Mat2x2 m;
   RotMat2(&m, panelFile->getRotation());

   int fiducialCount = 0;

   POSITION dataPos = panelFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = panelFile->getBlock()->getDataList().GetNext(dataPos);
      
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
               point2.x = pnt->x * panelFile->getScale() * unitsFactor;
               if (panelFile->isMirrored())
                  point2.x = -point2.x;
               point2.y = pnt->y * panelFile->getScale() * unitsFactor;
               TransPoint2(&point2, 1, &m, panelFile->getInsertX(), panelFile->getInsertY());

               if (point2.x < panelMinX)
                  panelMinX = point2.x;
               if (point2.x > panelMaxX)
                  panelMaxX = point2.x;
               if (point2.y < panelMinY)
                  panelMinY = point2.y;
               if (point2.y > panelMaxY)
                  panelMaxY = point2.y;
            }
         }
         break;

      case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * panelFile->getScale();
            if (panelFile->isMirrored())
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * panelFile->getScale();
            TransPoint2(&point2, 1, &m, panelFile->getInsertX(), panelFile->getInsertY());

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = panelFile->getResultantMirror(data->getInsert()->getMirrorFlags());
            double block_rot = panelFile->getRotation() + data->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            switch (data->getInsert()->getInsertType())
            {
            case INSERTTYPE_PCB:
               {
                  double rot = round(RadToDeg(block_rot));
                  //took off the round(rot/90), because we need actual rotation
                  double drot = (rot / 90) * 90; 
                  if (drot != rot)
                  {
                     fprintf(fLog, " Board In Panel rotation [%1.0lf] adjusted to [%1.0lf]\n", rot, drot);
                     display_error++;
                     block_rot = DegToRad(drot);
                  }

                  CString pcbGeomName;

                  if (block != NULL)
                     pcbGeomName = block->getName();

                  if (block_mirror)
                     pcbGeomName += "_Mirror";
               
                  TEPanelBoard *panelBoard = new TEPanelBoard;
                  panelBoardArray.SetAtGrow(panelBoardArrayCount++, panelBoard);  
                  panelBoard->x = point2.x * unitsFactor;
                  panelBoard->y = point2.y * unitsFactor;
                  panelBoard->rotation = block_rot;
                  panelBoard->mirror = block_mirror;
                  panelBoard->geomName = pcbGeomName;

                  if (block != NULL)
                     panelBoard->geomNum = block->getBlockNumber();
                  else
                     panelBoard->geomNum = -1;

                  panelBoard->refName = data->getInsert()->getRefname();
               }
               break;

            case INSERTTYPE_FIDUCIAL:
               {
                  int layerFlag = 1;
                  if ( block != NULL && (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     int layerNum = Get_ApertureLayer(doc, data, block, -1);

                     if (doc->get_layer_visible(layerNum, panelFile->isMirrored()) )
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
                  panelFiducial->x = point2.x * unitsFactor;
                  panelFiducial->y = point2.y * unitsFactor;
                  panelFiducial->refName = data->getInsert()->getRefname();
                  panelFiducial->fiducialRef = fiducialRef;

                  if (block != NULL)
                     panelFiducial->geomName = block->getName();

                  panelFiducial->layerFlag = layerFlag;
               }
               break;
            }
         }
      }
   }
}

/******************************************************************************
* GetFurthestFiducials
*/
static void GetFurthestFiducials(TEPanelFiducial **fiducial1, TEPanelFiducial **fiducial2)
{
   double max = 0;

   for (int i=0; i<panelFiducialArrayCount; i++)
   {
      TEPanelFiducial *f1 = panelFiducialArray[i];

      for (int ii=i+1; ii<panelFiducialArrayCount; ii++)
      {
         TEPanelFiducial *f2 = panelFiducialArray[ii];

         double distance = (f1->x-f2->x) * (f1->x-f2->x) + (f1->y-f2->y) * (f1->y-f2->y);

         if (distance > max)
         {
            max = distance;
            *fiducial1 = f1;
            *fiducial2 = f2;
         }
      }
   }
}

/******************************************************************************
* UpdateGeomList
*  - update all geometries
*/
static void UpdateGeomList()
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
         geom->geomName = block->getName();
      }
      else
      {
         fprintf(fLog, "PCB Component [%s] does not have a Centroid\n", block->getName());
         display_error++;
      }
   }
}

/******************************************************************************
* ClearCompList
*/
static void ClearCompList()
{
   for (int i=0;i<compArrayCount;i++)
      delete compArray[i];
   compArray.RemoveAll();
   compArrayCount = 0;
}

/******************************************************************************
* UpdateCompList
*  - update all components
*/
static void UpdateCompList(TEPanelBoard *boardZero)
{
   BlockStruct *boardGeom = doc->getBlockAt(boardZero->geomNum);

   double rotation = 0;
   double insert_x = 0;
   double insert_y = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = boardGeom->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = boardGeom->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      CString compName = data->getInsert()->getRefname();

      if (compName.IsEmpty())
         continue;

      int block_mirror = data->getInsert()->getMirrorFlags();
      double xDelta, yDelta;
      GetCentroid(block->getName(), &xDelta, &yDelta);
      if (block_mirror)
         xDelta = -xDelta;
      double x_cent_offset, y_cent_offset;
      Rotate(xDelta, yDelta, RadToDeg(data->getInsert()->getAngle() + boardZero->rotation), &x_cent_offset, &y_cent_offset);

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * unitsFactor;
      point2.y = data->getInsert()->getOriginY() * unitsFactor;
      TransPoint2(&point2, 1, &m, 0, 0);

      //took off the round(1.0 * degreesRotation / 90) because we need actual rotation
      double rot = data->getInsert()->getAngle();
      int degreesRotation = round(RadToDeg(rot));
      int orthogonalRot = (int)((1.0 * degreesRotation / 90) * 90);
      if (orthogonalRot != degreesRotation)
      {
         fprintf(fLog, "Component [%s] rotation [%d] adjusted to [%d]\n", compName, degreesRotation, orthogonalRot);
         display_error++;
         rot = DegToRad(orthogonalRot);
      }

      // adjust for first board rotation
      // panelBoardArray[firstboard]->rotation
      double xpos, ypos;
      Rotate(point2.x, point2.y, RadToDeg(boardZero->rotation), &xpos, &ypos);

      int compIndex = GetCompIndex(compName);
      TEComp *comp = compArray[compIndex];
      comp->x = xpos + x_cent_offset;
      comp->y = ypos + y_cent_offset;
      comp->rotation = rot + boardZero->rotation;
      comp->bottom = data->getInsert()->getPlacedBottom();
      comp->geomName = block->getName();
      comp->geomNum = data->getInsert()->getBlockNumber();
      comp->doPins = 0;

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TERADYNE_AOI_PINTEST, 1);
      if (attrib)
      {
         CString value = attrib->getStringValue();

         if (!value.CompareNoCase("TOP"))
            comp->doPins = 1;
         else if (!value.CompareNoCase("BOTTOM"))
            comp->doPins = 2;
         else if (!value.CompareNoCase("BOTH"))
            comp->doPins = 3;
      }


      // check DeviceClass vs GeomName prefix
      attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TERADYNE_AOI_DEVICECLASS, 1);
      if (attrib)
      {
         CString deviceClass = get_attvalue_string(doc, attrib);

         if (!deviceClass.Left(3).CompareNoCase("Cap"))
         {
            if (comp->geomName.Left(1).CompareNoCase("C"))
            {
               comp->geomName = "C_";
               comp->geomName += block->getName();
            }
         }
         else if (!deviceClass.Left(3).CompareNoCase("Res"))
         {
            if (comp->geomName.Left(1).CompareNoCase("R"))
            {
               comp->geomName = "R_";
               comp->geomName += block->getName();
            }
         }
      }
   }
}

/******************************************************************************
* GetCompIndex
*/
static int GetCompIndex(CString compName)
{
   if (compName.IsEmpty()) 
      return -1;

   for (int i=0; i<compArrayCount; i++)
      if (compName == compArray[i]->refName)
         return i;

   TEComp *comp = new TEComp;
   compArray.SetAtGrow(compArrayCount++, comp);  

   comp->refName = compName;

   return compArrayCount-1;
}

/******************************************************************************
* GetCentroid
*/
static int GetCentroid(CString geomName, double *x, double *y)
{
   *x = 0;
   *y = 0;

   for (int i=0; i<geomArrayCount; i++)
   {
      if (geomArray[i]->geomName.Compare(geomName) == 0)
      {
         *x = geomArray[i]->centroid_x;
         *y = geomArray[i]->centroid_y;
         return 1;
      }
   }

   return 0;
}

/******************************************************************************
* cnv_rot
*/
static int cnv_rot(double radians)
{
   int rot = round(RadToDeg(radians));

   while (rot < 0)
      rot += 360;

   while (rot >= 360)
      rot -= 360;

   return rot;
}

/******************************************************************************
* GetPadPerSide
*/
int GetPadPerSide(BlockStruct *block, BOOL Mirrored)
{
   int result = 0;

   if ( block == NULL )
      return result;

   BOOL Top = block->getFlags() & BL_ACCESS_TOP;
   BOOL Bottom = block->getFlags() & BL_ACCESS_BOTTOM;
   BOOL TopOnly = block->getFlags() & BL_ACCESS_TOP_ONLY;
   BOOL BottomOnly = block->getFlags() & BL_ACCESS_BOTTOM_ONLY;

   if (Mirrored)
   {
      if (Top || TopOnly)
         result |= 2;

      if (Bottom || BottomOnly)
         result |= 1;
   }
   else
   {
      if (Top || TopOnly)
         result |= 1;

      if (Bottom || BottomOnly)
         result |= 2;
   }

   return result;
}
