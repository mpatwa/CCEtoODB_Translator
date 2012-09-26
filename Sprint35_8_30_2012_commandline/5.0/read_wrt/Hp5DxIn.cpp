// $Header: /CAMCAD/5.0/read_wrt/Hp5DxIn.cpp 45    3/12/07 12:53p Kurt Van Ness $
 
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "hp5dxin.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <float.h>
#include <direct.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "hp5dx.h"
#include "xform.h"
#include "menlib.h"  // just for date test
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress;
extern char *HashName(char *, char *);

static void InitVariables();
static void FreeVariables();
static int LoadHP5DXSettings();
static int ReadLandPattern(const char *mainDir);
static int ReadPadGeometry(const char *mainDir);
static int ReadPackages(const char *mainDir);
static int MakeShapes();
static int MakePackages();
static int ReadProjectTestlink(const char *mainDir);
static int ReadPanel(const char *mainDir);
static void RegisterBoardNames(CString topName, CString bottomName);
static int GetBoardDef(CString topName, int topRot, CString botName, int botRot, BOOL *MirroredInstance);
static void UpdateBoardDef(HP5DXBoardDef *boardDef, CString boardName, int topRot, int botRot);
static void ReadBoard(const char *mainDir, const char *boardDir, BOOL TopSide, int boarddefptr);
static int ReadComponent(const char *mainDir, const char *boardDir, FileStruct *boardFile);
//static int ReadCamera(const char *mainDir, const char *boardDir, FileStruct *boardFile, BOOL Top);
//static int ReadNoiseReduction(const char *mainDir, const char *boardDir, FileStruct *boardFile);
//static int ReadExclusionZone(const char *mainDir, const char *boardDir, FileStruct *boardFile);
static CString GetPadstack(CString form, double xsize, double ysize, int SMD, double hole, double xoff, double yoff);
static int GetShapeIndex(const char *shapeName);
static int GetPackageIndex(const char *packageName);
static BlockStruct *FindPackageGeom(CString packageName);
static BOOL PackageNameCollision(CString packageName);
static double cnv_tok(char *buf);
static void GetUnitsFactor(char *buf);

static FILE *fLog;
static FileStruct *panelFile;
static double panelWidth;
static int pageUnits;
static int displayErrorCount;
static CCEtoODBDoc *doc;
static double unitsFactor;

static HP5DXShapePinArray shapePinArray;  
static int shapePinCnt;

static CStringArray shapeNameArray; 
static int shapeCnt;

static HP5DXPackagePinArray packagePinArray; 
static int packagePinCnt;

static CStringArray packageNameArray;  
static int packageCnt;

static HP5DXBoardDefArray boardDefArray;  
static int boardDefCnt;

static HP5DXBoardInsertArray boardInsertArray;  
static int boardInsertCnt;

static int drillLayerNum;

/******************************************************************************
* ReadHP5DX
*
*  - InitVariables()
*  - LoadHP5DXSettings()
*
*  - ReadLandPattern()
*  - ReadPadGeometry()
*  - MakeShapes()
*  - ReadPackages()
*  - MakePackages()
*
*  - ReadPanel()
*  - ReadBoard()
*  - ReadComponent()
*  - ReadCamera()
*
*  - FreeVariables()
*/
void ReadHP5DX(const char *mainDir, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits, int cur_filecnt, int tot_filecnt)
{
   doc = Doc;
   pageUnits = PageUnits;
   
   InitVariables();
   
   panelFile = Graph_File_Start("5dx panel", Type_HP5DX);
   panelFile->setBlockType(blockTypePanel); 
   panelFile->getBlock()->setBlockType(blockTypePanel);

   LoadHP5DXSettings();

   ReadLandPattern(mainDir); // landpat.ndf
   
   ReadPadGeometry(mainDir); // padgeom.ndf
   MakeShapes();

   ReadPackages(mainDir);
   MakePackages();

   // project.testlink
   ReadProjectTestlink(mainDir);

   // panel.ndf
   ReadPanel(mainDir);

   // board.ndf
	int i=0;
   for (i=0; i<boardDefCnt; i++)
   {
      HP5DXBoardDef *boardDef = boardDefArray[i];

      if (!boardDef->Found)
         continue;

      doc->PrepareAddEntity(boardDef->boardFile);        
      ReadBoard(mainDir, boardDef->topHash, TRUE, i);
      ReadBoard(mainDir, boardDef->botHash, FALSE, i);
      if (packageCnt)
      {
         ReadComponent(mainDir, boardDef->topHash, boardDef->boardFile);
         ReadComponent(mainDir, boardDef->botHash, boardDef->boardFile);
      }

      generate_PINLOC(doc, boardDef->boardFile, TRUE);

//    ReadCamera(mainDir, boardDef->topHash, boardDef->boardFile, TRUE);
//    ReadCamera(mainDir, boardDef->botHash, boardDef->boardFile, FALSE);

//    ReadNoiseReduction(mainDir, boardDef->topHash, boardDef->boardFile);
//    ReadNoiseReduction(mainDir, boardDef->botHash, boardDef->boardFile);

//    ReadExclusionZone(mainDir, boardDef->topHash, boardDef->boardFile);
//    ReadExclusionZone(mainDir, boardDef->botHash, boardDef->boardFile);
   }

 
// POTENTIAL PROBLEM AREA
   // fix flipped boards' insert points
   for (i=0; i<boardInsertCnt; i++)
   {
      HP5DXBoardInsert *boardInsert = boardInsertArray[i];

      if (!boardInsert->pcbInsert->getInsert()->getMirrorFlags())
         continue;

      HP5DXBoardDef *boardDef = boardDefArray[boardInsert->boardIndex];

      Point2 p;
      p.x = boardDef->boardWidth;
      p.y = 0;

      DTransform xform(0, 0, 1, boardInsert->pcbInsert->getInsert()->getAngle(), 0);
      xform.TransformPoint(&p);

      boardInsert->pcbInsert->getInsert()->incOriginX(p.x);
      boardInsert->pcbInsert->getInsert()->incOriginY(p.y);
   }

   // fix BottomSideOnly boards
   for (i=0; i<boardInsertCnt; i++)
   {
      HP5DXBoardInsert *boardInsert = boardInsertArray[i];

      if (!boardInsert->BottomSideOnly)
         continue;

      HP5DXBoardDef *boardDef = boardDefArray[boardInsert->boardIndex];

      int degreesRotation = (int)round(RadToDeg(boardInsert->pcbInsert->getInsert()->getAngle()));

      switch (degreesRotation)
      {
      case 90:
         boardInsert->pcbInsert->getInsert()->setOriginX(panelWidth - boardInsert->pcbInsert->getInsert()->getOriginX());
         boardInsert->pcbInsert->getInsert()->incOriginY(boardDef->boardWidth);
         break;

      case 180:
         boardInsert->pcbInsert->getInsert()->setOriginX(panelWidth - (boardInsert->pcbInsert->getInsert()->getOriginX() - boardDef->boardWidth));
         break;

      case 270:
         boardInsert->pcbInsert->getInsert()->setOriginX(panelWidth - boardInsert->pcbInsert->getInsert()->getOriginX());
         boardInsert->pcbInsert->getInsert()->incOriginY(-boardDef->boardWidth);
         break;

      case 0:
      default:
         boardInsert->pcbInsert->getInsert()->setOriginX(panelWidth - (boardInsert->pcbInsert->getInsert()->getOriginX() + boardDef->boardWidth));
         break;
      }

      boardInsert->pcbInsert->getInsert()->setAngle(
         boardInsert->pcbInsert->getInsert()->getAngle() * -1.);
   }

   fclose(fLog); 

   FreeVariables();

   if (displayErrorCount)
      Logreader(GetLogfilePath(HP5DXERR));
}

/******************************************************************************
* InitVariables
*/
void InitVariables()
{
   panelFile = NULL;
   displayErrorCount = 0;

   // log file
   if ((fLog = fopen(GetLogfilePath(HP5DXERR), "wt")) == NULL)
   {
      fprintf(fLog, "File [%s] not found\n", HP5DXERR);
      displayErrorCount++;
      return;
   }

   shapePinArray.SetSize(100,100);
   shapePinCnt = 0;

   shapeNameArray.SetSize(100,100);
   shapeCnt = 0;

   packagePinArray.SetSize(100,100);
   packagePinCnt = 0;

   packageNameArray.SetSize(100,100);
   packageCnt = 0;

   boardDefArray.SetSize(100,100);
   boardDefCnt = 0;
}

/******************************************************************************
* LoadHP5DXSettings 
*/
int LoadHP5DXSettings()
{
   CString settingsFile( getApp().getImportSettingsFilePath("5dx.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nHP5DX Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);

   FILE *fp = fopen(settingsFile, "rt");

   if (!fp)
   {
      // no settings file found
      fprintf(fLog, "File [%s] not found\n", settingsFile);
      displayErrorCount++;
      return 0;
   }

   fclose(fp);

   return 1;
}

/******************************************************************************
* FreeVariables
*/
void FreeVariables()
{
   int i;

   for (i=0; i<shapePinCnt; i++)
      delete shapePinArray[i];
   shapePinArray.RemoveAll();
   shapePinCnt = 0;

   shapeNameArray.RemoveAll();
   shapeCnt = 0;

   for (i=0; i<packagePinCnt; i++)
      delete packagePinArray[i];
   packagePinArray.RemoveAll();
   packagePinCnt = 0;

   for (i=0; i<packageCnt; i++)
   packageCnt = 0;

   for (i=0; i<boardDefCnt; i++)
      delete boardDefArray[i];
   boardDefArray.RemoveAll();
   boardDefCnt = 0;
}

/******************************************************************************
* ReadLandPattern
*/
int ReadLandPattern(const char *mainDir)
{
   char path[_MAX_PATH];
   _makepath(path, "", mainDir, "landpat", "ndf");

   FILE *fLandPat = fopen(path, "rt");
   if (!fLandPat)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   unitsFactor = Units_Factor(UNIT_MILS, pageUnits);

   char line[255];
   while (fgets(line, 255, fLandPat))
   {
      char *lp;
      if ((lp = get_string(line, " :\t\n", FALSE)) == NULL) continue;
      
      if (!lp)
         continue;

      if (lp[0] == '#')
         continue;

      if (lp[0] == '@')
      {
         // # Land Pattern ID                         Pin #   X Loc    Y Loc  Shape   X Size   Y Size
         CString shapename, pinname, pinshape;
         double x, y, xsize, ysize;

         shapename = &lp[1];
         shapename.MakeUpper();
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         pinname = lp;
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         y = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         pinshape = lp;
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         xsize = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         ysize = cnv_tok(lp);

         CString pname = GetPadstack(pinshape, xsize, ysize, TRUE, 0.0, 0.0, 0.0);

         HP5DXShapePin *sp = new HP5DXShapePin;
         shapePinArray.SetAtGrow(shapePinCnt++, sp);  
         sp->shapeIndex = GetShapeIndex(shapename);
         sp->pinname = pinname;
         sp->x = x;
         sp->y = y;
         sp->padstack = pname;
         sp->SMD = TRUE;
      }
      else if (!STRCMPI(lp, ".UNIT"))
      {
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL) continue;
         GetUnitsFactor(lp);
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in LANDPAT.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fLandPat);

   return 1;
}

/******************************************************************************
* ReadPadGeometry
*/
int ReadPadGeometry(const char *mainDir)
{
   char path[_MAX_PATH];
   _makepath(path, "", mainDir, "padgeom", "ndf");

   FILE *fPadGeom = fopen(path, "rt");
   if (!fPadGeom)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   unitsFactor = Units_Factor(UNIT_MILS, pageUnits);

   char line[255];
   while (fgets(line, 255, fPadGeom))
   {
      char *lp;

      if ((lp = get_string(line," :\t\n", FALSE)) == NULL)  continue;

      if (lp[0] == '#') 
         continue;

      if (lp[0] == '@')
      {
         CString shapename = &lp[1];
         shapename.MakeUpper();

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString pinname = lp;

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double holex = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double holey = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double holedia = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString pinshape = lp;

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double pinx = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double piny = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double xsize = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double ysize = cnv_tok(lp);
         

         CString pname = GetPadstack(pinshape, xsize, ysize, FALSE, holedia, holex-pinx, holey-piny);

         HP5DXShapePin *sp = new HP5DXShapePin;
         shapePinArray.SetAtGrow(shapePinCnt++, sp);  
         sp->shapeIndex = GetShapeIndex(shapename);
         sp->pinname = pinname;
         sp->x = holex;
         sp->y = holey;
         sp->padstack = pname;
         sp->SMD = FALSE;
      }
      else if (!STRCMPI(lp,".UNIT"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         GetUnitsFactor(lp);
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in PADGEOM.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fPadGeom);

   return 1;
}

/******************************************************************************
* MakeShapes
*/
int MakeShapes()
{
   // thru padstack
   int layernum = Graph_Level("PAD_THRU", "",0);
   LayerStruct *layer = doc->FindLayer(layernum);
   layer->setLayerType(LAYTYPE_PAD_ALL);

   // smd padstack
   layernum = Graph_Level("PAD_SMD_BOT", "",0);
   layer = doc->FindLayer(layernum);
   layer->setLayerType(LAYTYPE_PAD_BOTTOM);
   layernum = Graph_Level("PAD_SMD_TOP", "",0);
   layer = doc->FindLayer(layernum);
   layer->setLayerType(LAYTYPE_PAD_TOP);

   Graph_Level_Mirror("PAD_SMD_TOP", "PAD_SMD_BOT", "");

   layernum = Graph_Level("PLACE_BOTTOM", "",0);
   layer = doc->FindLayer(layernum);
   layer->setLayerType(LAYTYPE_SILK_BOTTOM);
   layernum = Graph_Level("PLACE_TOP", "",0);
   layer = doc->FindLayer(layernum);
   layer->setLayerType(LAYTYPE_SILK_TOP);

   Graph_Level_Mirror("PLACE_TOP", "PLACE_BOTTOM", "");

   for (int i=0; i<shapeCnt; i++)
   {
      CString shapeName = shapeNameArray[i];

      BlockStruct *b = Graph_Block_On(GBO_APPEND, shapeName, -1, 0);
      b->setBlockType(BLOCKTYPE_PCBCOMPONENT);

      BOOL SMD = FALSE;

      // get outline by looping through pins
      double xmin = FLT_MAX, ymin = FLT_MAX, xmax = -FLT_MAX, ymax = -FLT_MAX;

      for (int p=0; p<shapePinCnt; p++)
      {
         HP5DXShapePin *shapePin = shapePinArray[p];
         if (shapePin->shapeIndex == i)
         {
            DataStruct *pindata = Graph_Block_Reference(shapePin->padstack, shapePin->pinname, 
                  0, shapePin->x, shapePin->y, 0, 0, 1.0, -1, TRUE);

            pindata->getInsert()->setInsertType(insertTypePin);

            if (shapePin->SMD)
               SMD = TRUE;

            if (xmin > shapePin->x)
               xmin = shapePin->x;
            if (ymin > shapePin->y)
               ymin = shapePin->y;
            if (xmax < shapePin->x)
               xmax = shapePin->x;
            if (ymax < shapePin->y)
               ymax = shapePin->y;
         }
      }

      if (xmin < xmax)
      {
         double offsetmil = 25 * Units_Factor(UNIT_MILS, pageUnits);
         xmin -= offsetmil;
         ymin -= offsetmil;
         xmax += offsetmil;
         ymax += offsetmil;
         DataStruct *d = Graph_PolyStruct(Graph_Level("PLACE_TOP","",0), 0 , FALSE);
         d->setGraphicClass(GR_CLASS_COMPOUTLINE);
         Graph_Poly(NULL, 0, 0, 0, 1);
         Graph_Vertex(xmin, ymin, 0.0);
         Graph_Vertex(xmax, ymin, 0.0);
         Graph_Vertex(xmax, ymax, 0.0);
         Graph_Vertex(xmin, ymax, 0.0);
         Graph_Vertex(xmin, ymin, 0.0);
      }

      Graph_Block_Off();

      if (SMD)
         doc->SetAttrib(&b->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);
   }

   return 1;
}

/******************************************************************************
* ReadPackages
*/
int ReadPackages(const char *mainDir)
{
   char path[_MAX_PATH];
   _makepath(path, "", mainDir, "package", "ndf");

   FILE *fPackage = fopen(path, "rt");
   if (!fPackage)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   unitsFactor = Units_Factor(UNIT_MILS, pageUnits);

   char line[255];
   while (fgets(line, 255, fPackage))
   {
      char *lp;

      if ((lp = get_string(line, " :\t\n", FALSE)) == NULL) continue;

      if (lp[0] == '#')
         continue;

      if (lp[0] == '@')
      {
         // packagename  pitch     PACKAGE_FORM PIN_FORM PINNUM  xloc      yloc      pin (not pad) 
         //@CONN100      31.5000   PLASTIC GULLWING      1       125.0000  -787.5000 R    12.0000    12.0000 

         CString packageName, pinname, package_form, pin_form, pin_family;
         double pinx, piny, pitch, xsize, ysize;

         packageName = &lp[1];
         packageName.MakeUpper();
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         pitch = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         package_form = lp;
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         pin_family = lp;
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         pinname = lp;
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         pinx = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         piny = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue; 
         pin_form = lp;
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         xsize = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         ysize = cnv_tok(lp);

         BOOL HasRotation = FALSE;
         double rotation = 0;
         if (lp = get_string(NULL," \t\n", FALSE)) // optional
         {
            HasRotation = TRUE;
            rotation = atof(lp);
         }
         int shape = T_ROUND;
         if (pin_form == "C")
         {
            if (xsize == ysize)
            {
               shape = T_ROUND;
               ysize = 0;
            }
            else
            {
               shape = T_OBLONG;
            }
         }
         else
         {
            if (xsize == ysize)
            {
               shape = T_SQUARE;
               ysize = 0;
            }
            else
            {
               shape = T_RECTANGLE;
            }
         }

         BlockStruct *b = Graph_FindAperture(shape, xsize, ysize, 0.0, 0.0, DegToRad(rotation), 0, 0, 0);

         HP5DXPackagePin *c = new HP5DXPackagePin;
         packagePinArray.SetAtGrow(packagePinCnt, c);  
         packagePinCnt++;  
         c->packageIndex = GetPackageIndex(packageName);
         c->pinname = pinname;
         c->x = pinx;
         c->y = piny;
         c->apertureName = b->getName();
         c->pitch = pitch;
         c->family = pin_family;
         c->HasRotation = HasRotation;
         c->rotation = rotation;
      }
      else if (!STRCMPI(lp, ".UNIT"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         GetUnitsFactor(lp);
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in PACKAGE.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fPackage);

   return 1;
}

/******************************************************************************
* MakePackages
*/
int MakePackages()
{
   int layernum = Graph_Level("PACKAGE_PINBOT","",0);
   LayerStruct *layer = doc->FindLayer(layernum);
   layer->setLayerType(LAYTYPE_BOTTOM);

   layernum = Graph_Level("PACKAGE_PINTOP","",0);
   layer = doc->FindLayer(layernum);
   layer->setLayerType(LAYTYPE_TOP);

   Graph_Level_Mirror("PACKAGE_PINTOP", "PACKAGE_PINBOT", "");

   layernum = Graph_Level("PACKAGE_BOTTOM","",0);
   layernum = Graph_Level("PACKAGE_TOP","",0);

   Graph_Level_Mirror("PACKAGE_TOP", "PACKAGE_BOTTOM", "");

   for (int i=0; i<packageCnt; i++)
   {
      CString packageName = packageNameArray[i];

      if (PackageNameCollision(packageName))
         packageName += "_5DXPKG";
      if (PackageNameCollision(packageName))
      {
         fprintf(fLog, "Package Name Collision for %s\n", packageName);
         displayErrorCount++;
         packageName += "_X";
      }

      BlockStruct *packageGeom = Graph_Block_On(GBO_APPEND, packageName, -1, 0);
      packageGeom->setBlockType(BLOCKTYPE_PCBCOMPONENT);
      packageGeom->setPackage(true); 

      // get outline by looping through pins
      double xmin = FLT_MAX, ymin = FLT_MAX, xmax = -FLT_MAX, ymax = -FLT_MAX;

      for (int p=0; p<packagePinCnt; p++)
      {
         HP5DXPackagePin *packagePin = packagePinArray[p];
         if (packagePin->packageIndex == i)
         {
            layernum = Graph_Level("PACKAGE_PINTOP", "", 0);
            DataStruct *pindata = Graph_Block_Reference(packagePin->apertureName, packagePin->pinname, 
                  0, packagePin->x, packagePin->y, DegToRad(0), 0, 1.0, layernum, TRUE);

            double pitch = packagePin->pitch;
            doc->SetAttrib(&pindata->getAttributesRef(), doc->IsKeyWord(ATT_5DX_PITCH, 0),
                  VT_UNIT_DOUBLE, &pitch, SA_OVERWRITE, NULL);
            doc->SetUnknownAttrib(&pindata->getAttributesRef(), ATT_5DX_PINFAMILY,
                  packagePin->family, SA_OVERWRITE, NULL);  
            if (packagePin->HasRotation)
               doc->SetAttrib(&pindata->getAttributesRef(), doc->RegisterKeyWord("5DX_PACKAGE_PIN_ROTATION", 0, VT_DOUBLE),
                     VT_DOUBLE, &packagePin->rotation, SA_OVERWRITE, NULL); 

            if (xmin > packagePin->x)  
               xmin = packagePin->x;
            if (ymin > packagePin->y)  
               ymin = packagePin->y;
            if (xmax < packagePin->x)  
               xmax = packagePin->x;
            if (ymax < packagePin->y)  
               ymax = packagePin->y;
         }
      }

      if (xmin < xmax)
      {
         double   offsetmil = 25 * Units_Factor(UNIT_MILS, pageUnits);
         xmin -= offsetmil;
         ymin -= offsetmil;
         xmax += offsetmil;
         ymax += offsetmil;
         DataStruct *d = Graph_PolyStruct(Graph_Level("PACKAGE_TOP", "", 0), 0, FALSE);
         Graph_Poly(NULL, 0, 0, 0, 1);
         Graph_Vertex(xmin, ymin, 0.0);
         Graph_Vertex(xmax, ymin, 0.0);
         Graph_Vertex(xmax, ymax, 0.0);
         Graph_Vertex(xmin, ymax, 0.0);
         Graph_Vertex(xmin, ymin, 0.0);
      }

      Graph_Block_Off();
   }

   return 1;
}

/******************************************************************************
* ReadProjectTestlink
*/
int ReadProjectTestlink(const char *mainDir)
{
   char path[_MAX_PATH];
   _makepath(path, "", mainDir, "project", "testlink");

   FILE *fProjectTestlink = fopen(path, "rt");
   if (!fProjectTestlink)
      return 0;

   char line[255];
   while (fgets(line, 255, fProjectTestlink))
   {
      char *lp;
      
      if ((lp = get_string(line, " :\t\n", FALSE)) == NULL)
         continue;

      if (!STRCMPI(lp, ".BOARDDEF"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString topName = lp;

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString bottomName = lp;

         RegisterBoardNames(topName, bottomName);
      }
   }

   fclose(fProjectTestlink);

   return 1;
}

/******************************************************************************
* ReadPanel
*
*  Algorithm : Swap BOTTOM data lines to be treated like TOP, so all data lines are treated like TOP
*/
int ReadPanel(const char *mainDir)
{
   char path[_MAX_PATH];
   _makepath(path, "", mainDir, "panel", "ndf");

   FILE *fPanel = fopen(path, "rt");
   if (!fPanel)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   unitsFactor = Units_Factor(UNIT_MILS, pageUnits);

   boardInsertCnt = 0;
   int boardcnt = 0;
   double panelHeight = 0, panelThickness = 0;
   panelWidth = 0;

   CAttributes** panelAttribMap = &panelFile->getBlock()->getAttributesRef();

   char line[255];
   while (fgets(line, 255, fPanel))
   {
      char *lp;
      
      if ((lp = get_string(line, " :\t\n", FALSE)) == NULL)
         continue;

      if (lp[0] == '#')
         continue;

      if (lp[0] == '@')
      {
         CString primaryName = &lp[1];

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         BOOL Swapped;
         if (lp[0] == 'T' || lp[0] == 't')
            Swapped = FALSE;
         else if (lp[0] == 'B' || lp[0] == 'b')
            Swapped = TRUE;
         else
         {
            fprintf(fLog, "Expected 'T' or 'B' for side. [%s]\n", lp);
            displayErrorCount++;
            continue;
         }

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;   // inspect

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double primaryX = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double primaryY = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         int primaryRot = atoi(lp);
         if (primaryRot % 90)
         {
            fprintf(fLog, "Non-Orthogal Board Rotation in Panel File (%d)\n", primaryRot);
            displayErrorCount++;

            primaryRot = 0;
         }

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString secondaryName = lp;

         BOOL SingleSided = FALSE;
         double secondaryX;
         double secondaryY;
         int secondaryRot;
         if (!secondaryName.CompareNoCase("NONE"))
         {
            SingleSided = TRUE;
            secondaryX = primaryX;
            secondaryY = primaryY;
            secondaryRot = primaryRot;
         }
         else
         {
            if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
            secondaryX = cnv_tok(lp);

            if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
            secondaryY = cnv_tok(lp);

            if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
            secondaryRot = atoi(lp);
            if (secondaryRot % 90)
            {
               fprintf(fLog, "Non-Orthogal Board Rotation in Panel File (%d)\n", secondaryRot);
               displayErrorCount++;

               secondaryRot = 0;
            }
         }


         CString topName, botName;
         double topX, topY, botX, botY;
         int topRot, botRot;
         if (Swapped)
         {
            topName = secondaryName;
            topX = secondaryX;
            topY = secondaryY;
            topRot = secondaryRot;

            botName = primaryName;
            botX = primaryX;
            botY = primaryY;
            botRot = primaryRot;
         }
         else
         {
            topName = primaryName;
            topX = primaryX;
            topY = primaryY;
            topRot = primaryRot;

            botName = secondaryName;
            botX = secondaryX;
            botY = secondaryY;
            botRot = secondaryRot;
         }


// POTENTIAL PROBLEM AREA
//       RegisterBoardNames(primaryName, secondaryName); // Makes the Primary Side of the first instance of a board the Top Side (assuming it wasn't already set with .BOARDDEF or PROJECT.TESTLINK)


         // Find Board Def.  Create Board Def if not found.
         BOOL MirroredInstance = FALSE;
         int boardIndex = GetBoardDef(topName, topRot, botName, botRot, &MirroredInstance);

         HP5DXBoardDef *boardDef = boardDefArray[boardIndex];
         boardDef->SingleSided = SingleSided;
      
         doc->PrepareAddEntity(panelFile);

         CString boardInstance;
         boardInstance.Format("Board_%d", ++boardcnt);

// POTENTIAL PROBLEM AREA
         // Insert Board in Panel
         DataStruct *pcbInsert;
/*       if (MirroredInstance)
            pcbInsert = Graph_Block_Reference(boardDef->boardFile->getBlock()->name, boardInstance, 0, 
                  topX, topY, DegToRad(botRot), MirroredInstance, 1.0, 0, FALSE);
         else*/
            pcbInsert = Graph_Block_Reference(boardDef->boardFile->getBlock()->getName(), boardInstance, 0, 
                  topX, topY, DegToRad(topRot), MirroredInstance, 1.0, 0, FALSE);
         pcbInsert->getInsert()->setInsertType(insertTypePcb);

         doc->SetUnknownAttrib(&pcbInsert->getAttributesRef(), "5DX_HASH_TOP", boardDef->topHash, SA_APPEND, NULL);
         doc->SetUnknownAttrib(&pcbInsert->getAttributesRef(), "5DX_HASH_BOT", boardDef->botHash, SA_APPEND, NULL);

         HP5DXBoardInsert *boardInsert = new HP5DXBoardInsert;
         boardInsertArray.SetAtGrow(boardInsertCnt++, boardInsert); 
         boardInsert->boardIndex = boardIndex;
         boardInsert->pcbInsert = pcbInsert;
         boardInsert->BottomSideOnly = SingleSided && Swapped;
      }

      else if (!STRCMPI(lp, ".BOARDDEF"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString topName = lp;

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString bottomName = lp;

         RegisterBoardNames(topName, bottomName);
      }
      else if (!STRCMPI(lp, ".UNIT"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         GetUnitsFactor(lp);
      }
      else if (!STRCMPI(lp, ".SUBPANEL_ID"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_SUBPANEL_ID", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".STAGE_SPEED"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_STAGE_SPEED", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".MATERIALS"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, BOARD_MATERIAL, lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".ROTATION"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_ROTATION", lp, SA_APPEND, NULL);

         panelFile->setRotation((DbUnit)DegToRad(atoi(lp)));
      }
      else if (!STRCMPI(lp, ".FIDUCIAL"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_FIDUCIAL", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".SURFACE_MAP_ALIGNMENT_VIEW"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_SURFACE_MAP_ALIGNMENT_VIEW", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".PAUSE_ON_AUTO_ALIGN"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_PAUSE_ON_AUTO_ALIGN", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".LASER_MAP_STAGE_SPEED"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_LASER_MAP_STAGE_SPEED", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".PANEL_LOAD_TRAILING_EDGE_DETECT"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_PANEL_LOAD_TRAILING_EDGE_DETECT", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".ACCEPT_ILLEGAL_IDENTIFIERS"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_ACCEPT_ILLEGAL_IDENTIFIERS", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".AWARETEST_ON"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_AWARETEST_ON", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".CONTAINS_DIVIDED_BOARD"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_CONTAINS_DIVIDED_BOARD", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".DUAL_PANEL_TEST_CAD"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_DUAL_PANEL_TEST_CAD", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".SOFTWARE_REVISION"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         doc->SetUnknownAttrib(panelAttribMap, "5DX_SOFTWARE_REVISION", lp, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, ".DIMENSIONS"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         panelWidth = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         panelHeight = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         panelThickness = cnv_tok(lp);

         doc->SetAttrib(panelAttribMap, doc->IsKeyWord(BOARD_THICKNESS, 0), VT_UNIT_DOUBLE, &(panelThickness), SA_OVERWRITE, NULL);

         int laynr = Graph_Level("PANELOUTLINE","",0);
         LayerStruct *l = doc->FindLayer(laynr);
         l->setLayerType(LAYTYPE_PANEL_OUTLINE);

         DataStruct *d = Graph_PolyStruct(laynr, 0 , FALSE);
         d->setGraphicClass(GR_CLASS_PANELOUTLINE); 

         Graph_Poly(NULL, 0, 0, 0, 1);
         Graph_Vertex(0, 0, 0.0);
         Graph_Vertex(panelWidth, 0, 0.0);
         Graph_Vertex(panelWidth, panelHeight, 0.0);
         Graph_Vertex(0, panelHeight, 0.0);
         Graph_Vertex(0, 0, 0.0);
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in PANEL.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fPanel);

   return 1;
}

/******************************************************************************
* RegisterBoardNames
*/
static void RegisterBoardNames(CString topName, CString bottomName)
{
   for (int i=0; i<boardDefCnt; i++)
   {
      HP5DXBoardDef *boardDef = boardDefArray[i];

      if (!boardDef->topName.Compare(topName) && !boardDef->botName.Compare(bottomName))
         return;

      if (!boardDef->topName.Compare(bottomName) && !boardDef->botName.Compare(topName))
         return;
   }

   HP5DXBoardDef *boardDef = new HP5DXBoardDef;
   boardDefArray.SetAtGrow(boardDefCnt++, boardDef);  
   boardDef->topName = topName;
   boardDef->botName = bottomName;

   boardDef->Found = FALSE;

   char fname[_MAX_PATH];
   char hname[_MAX_PATH];
   strcpy(fname, topName);
   HashName(fname, hname);
   boardDef->topHash = hname;
   strcpy(fname, bottomName);
   HashName(fname, hname);
   boardDef->botHash = hname;
}

/******************************************************************************
* GetBoardDef
*  - Loop through boardDefArray looking for a match, else add a new boardDef
*/
static int GetBoardDef(CString topName, int topRot, CString botName, int botRot, BOOL *MirroredInstance)
{
   if (MirroredInstance)
      *MirroredInstance = FALSE;

   for (int i=0; i<boardDefCnt; i++)
   {
      HP5DXBoardDef *boardDef = boardDefArray[i];

      if (!boardDef->topName.Compare(topName) && !boardDef->botName.Compare(botName))
      {
         CString boardName;
         boardName.Format("%s|%s", topName, botName);

         if (!boardDef->Found)
            UpdateBoardDef(boardDef, boardName, topRot, botRot); 

         return i;
      }

      if (!boardDef->topName.Compare(botName) && !boardDef->botName.Compare(topName))
      {
         CString boardName;
         boardName.Format("%s|%s", botName, topName);

         if (MirroredInstance)
            *MirroredInstance = TRUE;

         if (!boardDef->Found)
            UpdateBoardDef(boardDef, boardName, topRot, botRot);

         return i;
      }
   }

   CString boardName;
   boardName.Format("%s|%s", topName, botName);

   HP5DXBoardDef *boardDef = new HP5DXBoardDef;
   boardDefArray.SetAtGrow(boardDefCnt++, boardDef);  
   boardDef->topName = topName;
   boardDef->botName = botName;

   boardDef->Found = FALSE;

   char fname[_MAX_PATH];
   char hname[_MAX_PATH];
   strcpy(fname, topName);
   HashName(fname, hname);
   boardDef->topHash = hname;
   strcpy(fname, botName);
   HashName(fname, hname);
   boardDef->botHash = hname;

   UpdateBoardDef(boardDef, boardName, topRot, botRot);

   return boardDefCnt - 1;
}

/******************************************************************************
* UpdateBoardDef
*/
static void UpdateBoardDef(HP5DXBoardDef *boardDef, CString boardName, int topRot, int botRot)
{
   boardDef->Found = TRUE;

   FileStruct *boardFile = Graph_File_Start(boardName, Type_HP5DX);
   boardFile->setBlockType(blockTypePcb);
   boardFile->getBlock()->setBlockType(boardFile->getBlockType());
   boardFile->setShow(false);

   boardDef->boardFile = boardFile;

   boardDef->topRot = topRot;
   boardDef->botRot = botRot;
}

/******************************************************************************
* ReadBoard
*/
void ReadBoard(const char *mainDir, const char *boardDir, BOOL TopSide, int boardDefIndex)
{
   HP5DXBoardDef *boardDef = boardDefArray[boardDefIndex];

   char path[_MAX_PATH];
   char dir[_MAX_PATH];
   
   strcpy(dir, mainDir);
   strcat(dir, boardDir);     

   _makepath(path, "", dir, "board", "ndf");

   FILE *fBoard = fopen(path, "rt");
   if (!fBoard)
   {
      // no settings file found
      CString tmp, name, hash;

      if (TopSide)
      {
         name = boardDef->topName;
         hash = boardDef->topHash;
      }
      else
      {
         name = boardDef->botName;
         hash = boardDef->botHash;
      }

      boardDef->SingleSided = TRUE;

      fprintf(fLog, "File [%s] not found. (%s) = (%s)\n", path, name, hash);
      displayErrorCount++;
      return;
   }


   unitsFactor = Units_Factor(UNIT_MILS, pageUnits);

   CAttributes** boardAttribMap = &boardDef->boardFile->getBlock()->getAttributesRef();

   char line[255];
   while (fgets(line, 255, fBoard))
   {
      char *lp;

      if ((lp = get_string(line," :\t\n", FALSE)) == NULL)  continue;

      if (lp[0] == '#') // comment
         continue;

      if (lp[0] == '@')
      {
         CString refname = &lp[1];
         refname.MakeUpper();

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double x = cnv_tok(lp);
         
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         double y = cnv_tok(lp);
         
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         int rot = atoi(lp);
         
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString shapename = lp;
         shapename.MakeUpper();

         Point2 p;
         p.x = x;
         p.y = y;

         if (!TopSide)
         {
            p.x = -p.x;

            if (boardDef->SingleSided)
            {
               p.x += boardDef->boardWidth;
               rot = -rot;
            }
            else
            {
               int deltaRot = -boardDef->topRot - boardDef->botRot; // used to map bottom parts to their correct realtive rotation on the bottom side of the board 

               while (deltaRot < 0)
                  deltaRot += 360;

               while (deltaRot >= 360)
                  deltaRot -= 360;

               DTransform xform(0, 0, 1, DegToRad(deltaRot), 0);
               xform.TransformPoint(&p);

               // map bottom parts to their correct relative position on the bottom side of the board
               switch (deltaRot)
               {
               case 0:
                  p.x += boardDef->boardWidth;
                  break;

               case 90:
                  p.x += boardDef->boardHeight;
                  p.y += boardDef->boardWidth;
                  break;

               case 180:
                  p.y += boardDef->boardHeight;
                  break;

               case 270:
                  break;

               default:
                  ErrorMessage("Bad deltaRot");
                  break;
               }

               rot = -rot + deltaRot;
            }
         }

         DataStruct *data = Graph_Block_Reference(shapename, refname, 0, p.x, p.y, DegToRad(rot), !TopSide, 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypePcbComponent);
      }
      else if (!STRCMPI(lp, ".UNIT"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         GetUnitsFactor(lp);
      }
      else if (!STRCMPI(lp, ".DIMENSIONS"))
      {
         if (TopSide || boardDef->SingleSided) // only read dimensions once
         {
            if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
            boardDef->boardWidth = cnv_tok(lp);
            if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
            boardDef->boardHeight = cnv_tok(lp);
            if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
            double boardthick = cnv_tok(lp);            
            
            doc->SetAttrib(boardAttribMap, doc->IsKeyWord(BOARD_THICKNESS, 0), VT_UNIT_DOUBLE, &(boardthick), SA_OVERWRITE, NULL);

            int layerNum = Graph_Level("BOARDOUTLINE","",0);
            LayerStruct *layer = doc->FindLayer(layerNum);
            layer->setLayerType(LAYTYPE_BOARD_OUTLINE);

            DataStruct *data = Graph_PolyStruct(layerNum, 0 , FALSE);
            data->setGraphicClass(GR_CLASS_BOARDOUTLINE); 

            Graph_Poly(NULL, 0, 0, 0, 1);
            Graph_Vertex(0, 0, 0.0);
            Graph_Vertex(0, boardDef->boardHeight, 0.0);
            Graph_Vertex(boardDef->boardWidth, boardDef->boardHeight, 0.0);
            Graph_Vertex(boardDef->boardWidth, 0, 0.0);
            Graph_Vertex(0, 0, 0.0);
         }
      }
      else if (!STRCMPI(lp, ".ALIGNMENT_PADS"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;

         int alignmentPadsNum = atoi(lp);
         int alignmentPadsKeyword;
         switch (alignmentPadsNum)
         {
         case 1:
            alignmentPadsKeyword = doc->RegisterKeyWord("5DX_ALIGNMENT_PADS_1", 0, VT_STRING);
            break;
         case 2:
            alignmentPadsKeyword = doc->RegisterKeyWord("5DX_ALIGNMENT_PADS_2", 0, VT_STRING);
            break;
         case 3:
            alignmentPadsKeyword = doc->RegisterKeyWord("5DX_ALIGNMENT_PADS_3", 0, VT_STRING);
            break;
         default:
            {
               CString buf = get_string(NULL, "\n", FALSE);
               fprintf(fLog, "Out of Range \".ALIGNMENT_PADS\" in BOARD.NDF \"%d\" \"%s\"\n", alignmentPadsNum, buf);
               displayErrorCount++;
               continue;
            }
         }

         CString rest = get_string(NULL, "\n", FALSE);
         doc->SetAttrib(boardAttribMap, alignmentPadsKeyword, VT_STRING, rest.GetBuffer(0), SA_APPEND, NULL);
      }    
      else if (!STRCMPI(lp, ".THICKNESS_PADS"))
      {
         if ((lp = get_string(NULL, "\n", FALSE)) == NULL)  continue;
         doc->SetUnknownAttrib(boardAttribMap, "5DX_STAGE_SPEED", lp, SA_APPEND, NULL); 
      }    
      else if (!STRCMPI(lp, ".DEFAULT_PITCH"))
      {
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL)  continue;
         doc->SetUnknownAttrib(boardAttribMap, "5DX_DEFAULT_PITCH", lp, SA_APPEND, NULL); 
      }    
      else if (!STRCMPI(lp, ".DEFAULT_SURFACE_MAP_DENSITY"))
      {
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL)  continue;
         doc->SetUnknownAttrib(boardAttribMap, "5DX_DEFAULT_SURFACE_MAP_DENSITY", lp, SA_APPEND, NULL); 
      }    
      else if (!STRCMPI(lp, ".BOARD_ID"))
      {
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL)  continue;
         doc->SetUnknownAttrib(boardAttribMap, "5DX_BOARD_ID", lp, SA_APPEND, NULL); 
      }    
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in BOARD.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fBoard);

   return;
}

/******************************************************************************
* ReadComponent
*/
int ReadComponent(const char *mainDir, const char *boardDir, FileStruct *boardFile)
{
   char path[_MAX_PATH];
   char dir[_MAX_PATH];
   
   strcpy(dir, mainDir);
   strcat(dir, boardDir);     

   _makepath(path, "", dir, "componen", "ndf");

   FILE *fComponent = fopen(path, "rt");
   if (!fComponent)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   char line[255];
   while (fgets(line, 255, fComponent))
   {
      char *lp;

      if ((lp = get_string(line," :\t\n", FALSE)) == NULL)  continue;
      
      if (lp[0] == '#')
         continue;

      if (lp[0] == '@')
      {
         //# Ref Designator   Loaded Package
         CString refname = &lp[1];
         refname.MakeUpper();

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         CString packageName = lp;
         packageName.MakeUpper();

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;

         DataStruct *comp = datastruct_from_refdes(doc, boardFile->getBlock(), refname);
         if (comp)
         {
            BlockStruct *packageGeom = FindPackageGeom(packageName);

            if (!packageGeom)
               packageGeom = FindPackageGeom(packageName + "_5DXPKG");

            if (packageGeom)
            {
               int packageNum = packageGeom->getBlockNumber();
               doc->SetAttrib(&comp->getAttributesRef(), doc->RegisterKeyWord("5DX_PACKAGE_GEOM_NUM", 0, VT_INTEGER), VT_INTEGER, &packageNum, SA_OVERWRITE, NULL);
            }
            else
            {
               fprintf(fLog, "Package [%s] not found\n", packageName);
               displayErrorCount++;
            }
         }
         else
         {
            fprintf(fLog, "PANELFILE Component [%s] not found\n", refname);
            displayErrorCount++;
         }
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in COMPONENT.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fComponent);

   return 1;
}

/******************************************************************************
* ReadCamera
*
int ReadCamera(const char *mainDir, const char *boardDir, FileStruct *boardFile, BOOL Top)
{
   char path[_MAX_PATH];
   char dir[_MAX_PATH];
   
   strcpy(dir, mainDir);
   strcat(dir, boardDir);     

   _makepath(path, "", dir, "camera", "ndf");

   FILE *fCamera = fopen(path, "rt");
   if (!fCamera)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   //@Comp CameraIndex PinList
   //.DEFAULT_FG_INDEX: 2
   //@CR6 2
   //@CR2 1 1 2 3

   char line[255];
   while (fgets(line, 255, fCamera))
   {
      char *lp;

      if ((lp = get_string(line," :\t\n", FALSE)) == NULL)  continue;
      
      if (lp[0] == '#')
         continue;

      if (lp[0] == '@')
      {
         CString compRef;

         compRef = &lp[1];
         compRef.MakeUpper();
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;

         int cameraIndex = atoi(lp);

         CStringList pinList;
         while ((lp = get_string(NULL," \t\n", FALSE)))
         {
            CString pinName = lp;
            pinName.MakeUpper();
            pinList.AddTail(pinName);
         }

         POSITION netPos = boardFile->getNetList().GetHeadPosition();
         while (netPos)
         {
            NetStruct *net = boardFile->getNetList().GetNext(netPos);

            POSITION cpPos = net->getHeadCompPinPosition();
            while (cpPos)
            {
               CompPinStruct *cp = net->getNextCompPin(cpPos);
               if (cp->comp == compRef)
               {
                  if (pinList.IsEmpty() || pinList.Find(cp->pin))
                     doc->SetAttrib(&cp->getAttributesRef(), doc->RegisterKeyWord("5DX_CAMERA_INDEX", 0, VT_INTEGER), VT_INTEGER, &cameraIndex, SA_OVERWRITE, NULL);
               }
            }
         }
      }
      else if (!STRCMPI(lp, ".DEFAULT_FG_INDEX"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         int cameraIndex = atoi(lp);
         if (Top)
            doc->SetAttrib(&boardFile->getBlock()->getAttributesRef(), doc->RegisterKeyWord("5DX_DEFAULT_CAMERA_INDEX_TOP", 0, VT_INTEGER), VT_INTEGER, &cameraIndex, SA_OVERWRITE, NULL);
         else
            doc->SetAttrib(&boardFile->getBlock()->getAttributesRef(), doc->RegisterKeyWord("5DX_DEFAULT_CAMERA_INDEX_BOTTOM", 0, VT_INTEGER), VT_INTEGER, &cameraIndex, SA_OVERWRITE, NULL);
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in CAMERA.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fCamera);

   return 1;
}

/******************************************************************************
* ReadNoiseReduction
*
int ReadNoiseReduction(const char *mainDir, const char *boardDir, FileStruct *boardFile)
{
/*   char path[_MAX_PATH];
   char dir[_MAX_PATH];
   
   strcpy(dir, mainDir);
   strcat(dir, boardDir);     

   _makepath(path, "", dir, "NOISERED", "ndf");

   FILE *fNoiseReduction = fopen(path, "rt");
   if (!fNoiseReduction)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   //@Comp CameraIndex PinList
   //.DEFAULT_FG_INDEX: 2
   //@CR6 2
   //@CR2 1 1 2 3

   char line[255];
   while (fgets(line, 255, fNoiseReduction))
   {
      char *lp;

      if ((lp = get_string(line," :\t\n", FALSE)) == NULL)  continue;
      
      if (lp[0] == '#')
         continue;

      if (lp[0] == '@')
      {
         CString compRef = &lp[1];
         compRef.MakeUpper();

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;

         int cameraIndex = atoi(lp);

         CStringList pinList;
         while ((lp = get_string(NULL," \t\n", FALSE)))
         {
            CString pinName = lp;
            pinName.MakeUpper();
            pinList.AddTail(pinName);
         }

         POSITION netPos = boardFile->getNetList().GetHeadPosition();
         while (netPos)
         {
            NetStruct *net = boardFile->getNetList().GetNext(netPos);

            POSITION cpPos = net->getHeadCompPinPosition();
            while (cpPos)
            {
               CompPinStruct *cp = net->getNextCompPin(cpPos);
               if (cp->comp == compRef)
               {
                  if (pinList.IsEmpty() || pinList.Find(cp->pin))
                     doc->SetAttrib(&cp->getAttributesRef(), doc->RegisterKeyWord("5DX_CAMERA_INDEX", 0, VT_INTEGER), VT_INTEGER, &cameraIndex, SA_OVERWRITE, NULL);
               }
            }
         }
      }
      else if (!STRCMPI(lp, ".DEFAULT_FG_INDEX"))
      {
         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;
         int cameraIndex = atoi(lp);
         if (Top)
            doc->SetAttrib(&boardFile->getBlock()->getAttributesRef(), doc->RegisterKeyWord("5DX_DEFAULT_CAMERA_INDEX_TOP", 0, VT_INTEGER), VT_INTEGER, &cameraIndex, SA_OVERWRITE, NULL);
         else
            doc->SetAttrib(&boardFile->getBlock()->getAttributesRef(), doc->RegisterKeyWord("5DX_DEFAULT_CAMERA_INDEX_BOTTOM", 0, VT_INTEGER), VT_INTEGER, &cameraIndex, SA_OVERWRITE, NULL);
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in NOISERED.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fNoiseReduction);

   return 1;
}

/******************************************************************************
* ReadExclusionZone
*
int ReadExclusionZone(const char *mainDir, const char *boardDir, FileStruct *boardFile)
{
   char path[_MAX_PATH];
   char dir[_MAX_PATH];
   
   strcpy(dir, mainDir);
   strcat(dir, boardDir);     

   _makepath(path, "", dir, "exclzone", "ndf");

   FILE *fExclusionZone = fopen(path, "rt");
   if (!fExclusionZone)
   {
      fprintf(fLog, "File [%s] not found\n", path);
      displayErrorCount++;
      return 0;
   }

   //@cp1 678 1 5

   char line[255];
   while (fgets(line, 255, fExclusionZone))
   {
      char *lp;

      if ((lp = get_string(line," :\t\n", FALSE)) == NULL)  continue;
      
      if (lp[0] == '#')
         continue;

      if (lp[0] == '@')
      {
         CString compRef = &lp[1];
         compRef.MakeUpper();

         if ((lp = get_string(NULL," \t\n", FALSE)) == NULL)   continue;

         int zones = atoi(lp);

         CStringList pinList;
         while ((lp = get_string(NULL," \t\n", FALSE)))
         {
            CString pinName = lp;
            pinName.MakeUpper();
            pinList.AddTail(pinName);
         }

         POSITION netPos = boardFile->getNetList().GetHeadPosition();
         while (netPos)
         {
            NetStruct *net = boardFile->getNetList().GetNext(netPos);

            POSITION cpPos = net->getHeadCompPinPosition();
            while (cpPos)
            {
               CompPinStruct *cp = net->getNextCompPin(cpPos);
               if (!cp->comp.CompareNoCase(compRef))
               {
                  if (pinList.IsEmpty() || pinList.Find(cp->pin))
                     doc->SetAttrib(&cp->getAttributesRef(), doc->RegisterKeyWord("5DX_EXCLUSION_ZONES", 0, VT_INTEGER), VT_INTEGER, &zones, SA_OVERWRITE, NULL);
               }
            }
         }
      }
      else
      {
         CString key, rest;
         key = lp;
         rest = get_string(NULL, "\n", FALSE);
         fprintf(fLog, "Unknown section in CAMERA.NDF \"%s %s\"\n", key, rest);
         displayErrorCount++;
      }
   }

   fclose(fExclusionZone);

   return 1;
}

/******************************************************************************
* GetPadStack
*/
CString GetPadstack(CString shapeString, double xsize, double ysize, int SMD, double hole, double xoff, double yoff)
{
   int decimal = GetDecimals(doc->getSettings().getPageUnits());

   CString pName;
   if (SMD)
      pName.Format("SMD_%s_%1.*lf_%1.*lf", shapeString.GetBuffer(0), decimal, xsize, decimal, ysize);
   else
      pName.Format("THRU_%s_%1.*lf_%1.*lf_%1.*lf_%1.*lf_%1.*lf", shapeString.GetBuffer(0), decimal, xsize, decimal, ysize,
         decimal, hole, decimal, xoff, decimal, yoff);

   int layerNum;
   if (SMD)
      layerNum = Graph_Level("PAD_SMD_TOP", "", 0);
   else
      layerNum = Graph_Level("PAD_THRU", "", 0);

   if (Graph_Block_Exists(doc, pName, -1) == NULL)
   {

      BlockStruct *block = Graph_Block_On(GBO_APPEND, pName, -1, 0);
      block->setBlockType(BLOCKTYPE_PADSTACK);

      // create a padstack
      int shape = T_ROUND;
      if (shapeString == "C")
      {
         if (xsize == ysize)
         {
            shape = T_ROUND;
            ysize = 0;
         }
         else
         {
            shape = T_OBLONG;
         }
      }
      else
      {
         if (xsize == ysize)
         {
            shape = T_SQUARE;
            ysize = 0;
         }
         else
         {
            shape = T_RECTANGLE;
         }
      }

      BlockStruct *b = Graph_FindAperture(shape, xsize, ysize, xoff, yoff, 0.0, 0, 0, 0);
      Graph_Block_Reference(b->getName(), NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerNum, TRUE);

      if (hole > 0)
      {
         int drillayer = Graph_Level("DRILLHOLE", "", 0);
         LayerStruct *layer;
         layer = doc->FindLayer(drillayer);
         layer->setLayerType(LAYTYPE_DRILL);

         BlockStruct *bd = Graph_FindTool(hole, 0, TRUE);

         // must be filenum 0, because apertures are global.
         DataStruct* toolData = Graph_Block_Reference(bd->getName(), NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, drillayer, TRUE);
         toolData->getInsert()->setInsertType(insertTypeDrillHole);
      }

      Graph_Block_Off();
      if (SMD)
         doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);
   }

   return pName;
}


/******************************************************************************
* GetShapeIndex
*/
int GetShapeIndex(const char *shapeName)
{
   for (int i=0; i<shapeCnt; i++)
   {
      if (!shapeNameArray[i].Compare(shapeName))
         return i;
   }

   shapeNameArray.SetAtGrow(shapeCnt++, shapeName);

   return shapeCnt -1;
}

/******************************************************************************
* GetPackageIndex
*/
int GetPackageIndex(const char *packageName)
{
   for (int i=0; i<packageCnt; i++)
   {
      if (!packageNameArray[i].Compare(packageName))
         return i;
   }

   packageNameArray.SetAtGrow(packageCnt++, packageName);  

   return packageCnt -1;
}

/******************************************************************************
* FindPackageGeom
*/
BlockStruct *FindPackageGeom(CString packageName)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (!block->getPackage())
         continue;
      
      if (!packageName.Compare(block->getName()))
         return block;
   }

   return NULL;
}

/******************************************************************************
* PackageNameCollision
*/
BOOL PackageNameCollision(CString packageName)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getPackage())
         continue;
      
      if (!packageName.Compare(block->getName()))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* cnv_tok
*/
double cnv_tok(char *buf)
{
   return atof(buf) * unitsFactor;
}

/******************************************************************************
* GetUnitsFactor
*/
void GetUnitsFactor(char *buf)
{
   if (!STRCMPI(buf, "mils"))
      unitsFactor = Units_Factor(UNIT_MILS, pageUnits);
   else if (!STRCMPI(buf, "millimeters"))
      unitsFactor = Units_Factor(UNIT_MM, pageUnits);
   else if (!STRCMPI(buf, "inches"))
      unitsFactor = Units_Factor(UNIT_INCHES, pageUnits);
}

