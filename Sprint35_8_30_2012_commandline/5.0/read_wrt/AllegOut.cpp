// $Header: /CAMCAD/4.6/read_wrt/AllegOut.cpp 97    4/12/07 3:35p Lynn Phung $

/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994 - 2002. All Rights Reserved.

   filename is used for technology file and netlist file.
   this should be changed to the true PCB file name, so it would
   allow mulitple PCB's to be translated.
*/
                              
#include "stdafx.h"
#include "ccdoc.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "graph.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include <math.h>
#include "outline.h"
#include "ck.h"
#include "pcblayer.h"
#include "polylib.h"
#include "allegout.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


#define SubClassDisplay    0x1
#define SubClassSilkscreen 0x2
#define SubClassAssembly   0x4

extern   CProgressDlg      *progress;

static   Textblock   textblock[MAX_TEXTBLOCKS];
static   int         textblockcnt;

static   Thermalname thermalname[MAX_THERMALNAME];
static   int         thermalnamecnt;

static   Deflay      deflay[MAX_LAYERS];
static   int         deflaycnt;  

static   CComplistArray complistarray;    // this is the device - mapping
static   int         complistcnt;

static   Maplay      maplay[MAX_MAPLAYERS];
static   int         maplaycnt;  

static   Maperror    maperror[MAX_MAPLAYERS];
static   int         maperrorcnt;   

static   Newlay      newlay[MAX_NEWLAYERS];
static   int         newlaycnt;  

static   int         TRUE_DEVICE;
static   int         HIERACHIEMARKER;

static   CCEtoODBDoc        *doc;

static   double            mils_2_units;
static   int               display_error;
static   int               page_units;
static   FILE              *flog;
static   CString           filePathName;

static   int               max_signalcnt;    // this is the nunber of XRF layers

static   double            antipadenlargement;
static   double            tiewidth, airwidth;
static   BOOL              write_thermals;
static   BOOL              writeBoundary;
static   bool              Ref_On_Display;
static   bool              Ref_On_Silkscreen;
static   bool              Ref_On_Assembly;
static   bool              Dev_On_Display;
static   bool              Dev_On_Silkscreen;
static   bool              Dev_On_Assembly;
//static   int               Ref_Cnt;
static   int               RefSubClass;
//static   int               Dev_Cnt;
static   int               DevSubClass;
static   CString           deviceTypeAttributeName;

static   int               dialogplaceboundtop;

typedef  CArray<Point2, Point2&> CPolyArray;
static   CPolyArray        polyarray;
static   int               polycnt;

static   CAllegroLayerArray mlArr;
static   int               maxArr = 0;

static   double            drillsymbolarray[255];
static   int               drillsymbolcnt;

static   double            SMALL_DELTA = 0.0001;

static double              BOARD_ENLARGE = 1000;
static double              DRAWING_ENLARGE = 1000; // how much a drawing is bigger than the extens
static double              PAD_ENLARGE = 100;      // how much a drawing is bigger than the extens
static double              REFDES_HEIGHT  = 70.0;     // mils.

static CString             packageheaderscript, packagefooterscript;
static CString             mechanicalheaderscript, mechanicalfooterscript;
static CString             boardheaderscript, boardfooterscript;

static FileStruct          *file;
static int                 pshapecnt;

static char                *output_units_string = "Mils";
static int                 output_units = UNIT_MILS;
static int                 output_units_accuracy = 3;
static int                 Allegro_Version = 120;  // 120, 130, 136, 140, 141, 142, 150, 151, 152

static BOOL                DRC_ON = FALSE;
static BOOL                BATCH_NOGRAPHIC = FALSE;
static BOOL                FilledVoidMessage = FALSE;
static BOOL                FilledWidthMessage = FALSE;
static BOOL                SKILL = FALSE; // the low cost Allegro Studio does not support SKILL!

static double              ARC_ANGLE_DEGREE;
static CString             default_via;
static BOOL                isWritingPackageSymbol;
static CMapStringToString  notUniquePinRefnameMap; // holds the name of block that does not have unique refname for all of its pins
static CMapStringToString  shapeAlreadyInAutoShape;

static int ALLEGRO_OutlineSHAPEData(CDataList *DataList, double insert_x, double insert_y, double rotation,
      int mirror, double scale, int embeddedLevel, int insertLayer, int primary, int gr_class);
static int ALLEGRO_ApertureSHAPEData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel);
static int ALLEGRO_WritePADSTACKData(FILE *fPadstack, FILE *fAutoShape, CDataList *DataList, int insertlevel,
      double scale, int mirror, double rotation, double *drillsize, BOOL SMD);
static int ALLEGRO_WritePADSTACKData_136(FILE *fPadstack, FILE *fAutoshape, CDataList *DataList, int insertlevel,
      double scale, int mirror, double rotation, double *drillsize, BOOL SMD, CString padstackName);
static int ALLEGRO_WriteVias_GenericData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale);
static int ALLEGRO_WritePackagePinData(FILE *fGeom, BlockStruct *block, double insert_x, double insert_y,
      double rotation, int mirror, double scale, BOOL WritePins);
static int ALLEGRO_WriteMechanicalDrillData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int write_pin );
static int ALLEGRO_WriteData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, int layerclass, int hierarchiemarker);
static int ALLEGRO_WriteBrdGraphicData(FILE *wfp, FILE *ffp, FILE *pfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, int layerclass);
static int ALLEGRO_WriteDeviceData(FILE *wfp, BlockStruct *block, const char *devname);
static int load_ALLEGROsettings(const char *fname);
static int get_classtype(const char *n);
static int get_allegro_class_from_camcad(int cc_class);
static void do_layerlist();
static int edit_layerlist();
static int do_drawings(FileStruct *file, FILE *wfp);
static int do_technologyfile(const char *fname);
static int do_padstack_136(FileStruct *file, CString autoPadFileName, CString autoShapeFileName);
static int do_padstack(FileStruct *file, CString autoPadFileName, CString autoShapeFileName);
static int do_netlistfile(FileStruct *file, const char *NETFILE, const char *fname);
static int do_brdfile(FileStruct *file, const char *brdfile, const char *fillfile, const char *pourfile, const char *fname, const char *netfile,
      double xmin, double ymin, double xmax, double ymax);
static int do_devices(FileStruct *file);
static int do_geometries(FileStruct *file, CString autoSymFileName);
static int check_devicefiles();
static void free_layerlist();
static void CheckForNotUniquePinRefname();
static void UpdateCompPinName(FileStruct *file, CompPinStruct *compPin, CString netName);

static CAllegroNameManager *AllegroNameMgr = NULL;

/******************************************************************************
* ALLEGRO_WriteFiles
*/
void ALLEGRO_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int pageUnits,
      double xmin, double ymin, double xmax, double ymax, const char *checkFile, const char *settingsFile)
{
   isWritingPackageSymbol = FALSE;

   display_error = FALSE;
   drillsymbolcnt = 0;
   thermalnamecnt = 0;

   CString logFile = GetLogfilePath("ALLEGRO.log");

   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   if (AllegroNameMgr != NULL)
      delete AllegroNameMgr; // out with the old
   AllegroNameMgr = new CAllegroNameManager;

   fprintf(flog,"\nProcedure to load created data into ALLEGRO\n");

   display_error++;

   doc = Doc;
   page_units = pageUnits;

   load_ALLEGROsettings(settingsFile);

   SMALL_DELTA = 1;
	int i=0;

   for (i=0;i<output_units_accuracy;i++)
      SMALL_DELTA *= 0.1;

   check_init(checkFile, true /*ignore case aka UpCase*/, true /*ignore mapping collisions*/);

   mils_2_units = Units_Factor(UNIT_MILS, output_units);

   CFilePath baseOutfilePath(filename);
   filePathName = baseOutfilePath.getDirectoryPath();
   filePathName += "\\"; // so other places can append file name to this path

   CFilePath autopadBatFile(baseOutfilePath);
   autopadBatFile.setBaseFileName("autopad");
   autopadBatFile.setExtension("bat");

   CFilePath netFile(baseOutfilePath);
   netFile.setExtension("net");

   CFilePath autobrdSrcFile(baseOutfilePath);
   autobrdSrcFile.setBaseFileName("autobrd");
   autobrdSrcFile.setExtension("src");

   CFilePath autofillScrFile(baseOutfilePath);
   autofillScrFile.setBaseFileName("autoFill");
   autofillScrFile.setExtension("scr");

   CFilePath autopourScrFile(baseOutfilePath);
   autopourScrFile.setBaseFileName("autoPour");
   autopourScrFile.setExtension("scr");

   CFilePath autosymBatFile(baseOutfilePath);
   autosymBatFile.setBaseFileName("autosym");
   autosymBatFile.setExtension("bat");

   CFilePath autoshapeBatFile(baseOutfilePath);
   autoshapeBatFile.setBaseFileName("autoshp");
   autoshapeBatFile.setExtension("bat");

   CFilePath autodwgBatFile(baseOutfilePath);
   autodwgBatFile.setBaseFileName("autodwg");
   autodwgBatFile.setExtension("bat");

   FILE  *autodrawingfp;

   if ((autodrawingfp = fopen(autodwgBatFile.getPath(),"wt")) == NULL)
   {
      CString  tmp;
      tmp.Format("Can not open file [%s]", autodwgBatFile.getPath());
      ErrorMessage(tmp);
      return;
   }

   complistarray.SetSize(100,100);
   complistcnt = 0;

   polyarray.SetSize(100,100);
   polycnt = 0;

   mlArr.SetSize(100,100);
   maxArr = 0;
   do_layerlist();

   int   pcb_found = 0;

   // here loop for only one PCB file
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);

      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         pcb_found++;
         //net_layerlist(file); what is this needed in Allegro ????
      }
   }

   if (pcb_found > 1)
   {
      MessageBox(NULL, "More than one PCB file found !","ALLEGRO Output", MB_OK | MB_ICONHAND);
   }

   doc->OnGeneratePinNumbers();

   notUniquePinRefnameMap.RemoveAll();
   CheckForNotUniquePinRefname();

   shapeAlreadyInAutoShape.RemoveAll();

   if (edit_layerlist())
   {
      CWaitCursor wait;

      if (format->Output)  // 0 = PCB, 1 = GRAPHIC
      {
         // not implemented
         do_drawings(file, autodrawingfp);
      }
      else
      {
         // PCB translation
         pcb_found = 0;
         pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            CString  f;
   
            file = doc->getFileList().GetNext(pos);

            if (!file->isShown())  continue;

            if (file->getBlockType() == BLOCKTYPE_PCB)  
            {
               pcb_found++;

               // only do I pcb file
               if (pcb_found != 1)  break;

               // allegro makes "mistakes" on duplicated traces.
               EliminateDuplicateVias(doc, file->getBlock());
               BreakSpikePolys(file->getBlock());

               // this can happen on strange via placements.
               EliminateSinglePointPolys(doc);                
               //progress->SetStatus("Crack");
               Crack(doc,file->getBlock(), TRUE);          

               double accuracy = get_accuracy(doc);
               //progress->SetStatus("Elim traces");
               EliminateOverlappingTraces(doc,file->getBlock(), TRUE, accuracy);                

               do_technologyfile(baseOutfilePath.getBaseFileName());
               
               if (Allegro_Version > 130)
                  do_padstack_136(file, autopadBatFile.getPath(), autoshapeBatFile.getPath());
               else
                  do_padstack(file, autopadBatFile.getPath(), autoshapeBatFile.getPath());

               do_netlistfile(file, netFile.getPath(), baseOutfilePath.getBaseFileName());

               do_brdfile(file, autobrdSrcFile.getPath(), autofillScrFile.getPath(), 
                  writeBoundary ? autopourScrFile.getPath() : "", baseOutfilePath.getBaseFileName(), 
                  netFile.getPath(), xmin, ymin, xmax, ymax);

               if (TRUE_DEVICE)  // this means create a 7400 device
                  do_devices(file);

               do_geometries(file, autosymBatFile.getPath());

               check_devicefiles();
            }
            else
            {
               do_drawings(file, autodrawingfp);
            }
         }
   
         if (!pcb_found)
         {
            MessageBox(NULL, "No PCB file found !","ALLEGRO Output", MB_OK | MB_ICONHAND);
         }
      }
   } // edit layer

   fclose(autodrawingfp);

   free_layerlist();

   if (thermalnamecnt)
   {
      fprintf(flog, "Created Thermal aperture names:\n");

      for (int i=0; i<thermalnamecnt; i++)
         fprintf(flog,"Aperture name : %s\n",thermalname[i].name);

      fprintf(flog,"\n");
   }

   if (check_report(flog)) display_error++;

   check_deinit();

   fclose(flog);

   for (i=0; i<complistcnt; i++)
      delete complistarray[i];

   complistarray.RemoveAll();

   polyarray.RemoveAll();
   notUniquePinRefnameMap.RemoveAll();
   shapeAlreadyInAutoShape.RemoveAll();

   if (AllegroNameMgr != NULL)
   {
      delete AllegroNameMgr;
      AllegroNameMgr = NULL;
   }

   if (display_error)
      Logreader(logFile);
}

/******************************************************************************
* CheckForNotUniquePinRefname
   This function will put all the blocks that do not have unique pin refname for all
   of its pins into notUniquePinRefnameMap.
*/
static void CheckForNotUniquePinRefname()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      CMapStringToString pinRefnameMap;

      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *data = block->getDataList().GetNext(pos);

         if (!data)
            continue;

         if (data->getDataType() != T_INSERT || data->getInsert()->getInsertType() != insertTypePin)
            continue;

         CString refname = "";

         if (pinRefnameMap.Lookup(data->getInsert()->getRefname(), refname))
         {
            notUniquePinRefnameMap.SetAt(AllegroNameMgr->GetName(block), AllegroNameMgr->GetName(block));

            break;
         }
         else
         {
            pinRefnameMap.SetAt(data->getInsert()->getRefname(), data->getInsert()->getRefname());
         }
      }
   }
}

/*****************************************************************************/
/*
*/
static const char *do_thermalreliefname(int shape, double a, double b)
{
   char shapechar;

   switch (shape)
   {
   case T_SQUARE:
   case T_RECTANGLE:
      shapechar = 'R';
      break;
   case T_COMPLEX:
      shapechar = 'X';
      break;
   default:
      shapechar = 'C';
      break;
   }

   static CString tmp;

   if (shapechar == 'C')
      tmp.Format("th%c_%lg_%lg_%lg", shapechar, a, airwidth, tiewidth);
   else
      tmp.Format("th%c_%lg_%lg_%lg_%lg", shapechar, a, b, airwidth, tiewidth);

   // here change all . to - (according to Randy)
   char ws[1000];
   strcpy(ws, tmp);
	
	int i=0;
   for (i=0; i<(int)strlen(ws); i++)
   {
      if (ws[i] == '.')
         ws[i] = '-';
   }

   tmp = check_name('f', ws);

   BOOL Found = FALSE;

   for (i=0; i<thermalnamecnt && !Found; i++)
   {
      if (thermalname[i].name.Compare(tmp) == 0)
         Found = TRUE;
   }

   if (!Found)
   {
      if (thermalnamecnt < MAX_THERMALNAME)
      {
         thermalname[thermalnamecnt].name = tmp;
         thermalnamecnt++;
      }
   }

   return (const char *)tmp;
}

/*****************************************************************************/
/*
*/
static   int   created_by(FILE *fp)
{
   fprintf(fp,"# Allegro Script\n");
   fprintf(fp,"# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(fp,"\n");

   if (Allegro_Version == 136)
      fprintf(fp,"version 13.6\n");
   else if (Allegro_Version == 142)
      fprintf(fp,"version 14.2\n");
   else
      fprintf(fp,"version %d\n", round(Allegro_Version/10));

   fprintf(fp,"\n");

   return 1;
}

/****************************************************************************/
/*
   return NULL if not visible
*/
static const char *get_drillsymbol(double s)
{
   static   char  ds[10];
   int      i, m1,m2;

   strcpy(ds,"A");

   for (i=0;i<255;i++)
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
      fprintf(flog,"Too many drill symbols\n");
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

/****************************************************************************/
/*
*/
static int  add_newlayers(FILE *fp)
{
   int   i;

   if (newlaycnt == 0)  return 0;

   fprintf(fp,"define subclass\n");

   for (i=0;i<newlaycnt;i++)
   {
      fprintf(fp,"setwindow form.subclass\n");
      fprintf(fp,"FORM subclass %s pick\n",lclasses[newlay[i].classtype]);

      if (newlay[i].nonetch)
      {
         fprintf(fp,"setwindow form.subclass_nonetch\n");
         fprintf(fp,"FORM subclass_nonetch name %s\n",newlay[i].subclass);
      }
      else
      {
         fprintf(fp,"setwindow form.subclass_etch\n");
         fprintf(fp,"FORM subclass_etch name %s\n",newlay[i].subclass);
      }
   }

   fprintf(fp,"setwindow form.subclass\n");
   fprintf(fp,"FORM subclass done\n");

   return 1;
}

/****************************************************************************/
/*
   get the layer index poitner
*/
static const int get_mlarrptr(int l)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (mlArr[i]->layerindex == l)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
   return NULL if not visible
*/
static const char *Layer_ALLEGRO(int l)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (mlArr[i]->on == 0)  continue;   // not visible

      if (mlArr[i]->layerindex == l)
         return mlArr[i]->newName;
   }

   fprintf(flog,"Layer [%s] not translated.\n", doc->getLayerArray()[l]->getName());

   return NULL;
}

//--------------------------------------------------------------
// this get as preassigned class and incoming layer.
static int get_class_subclass(const char *l, int layerclass)
{
   int   i;

   for (i=0;i<maplaycnt;i++)
   {
      if (!STRCMPI(maplay[i].setupname,l) && 
                   maplay[i].classtype == layerclass)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
// 
static int in_maperror(const char *l, const char *layerclass)
{
   int   i;

   for (i=0;i<maperrorcnt;i++)
   {
      if (!maperror[i].layername.CompareNoCase(l) && !maperror[i].subclass.CompareNoCase(layerclass))
         return i+1;
   }

   if (maperrorcnt < MAX_MAPLAYERS)
   {
      maperror[maperrorcnt].layername = l;
      maperror[maperrorcnt].subclass = layerclass;
      maperrorcnt++;
   }

   return 0;
}

/*****************************************************************************/
/*
   Classes subclasses layers ....

   in allegro.out the incomming cad layer is assigned to a class, subclass.
   The .out class is used to different a layer "draw" on a component vs. a board. This
   means more or less a hardcoded class.

   However, the user can force any class by naming a subclass class,subclass.
   All responsibility is with the user.
*/
static int write_class_subclass(FILE *fp,int layerclass, const char *ss_class)
{
   CString  c_class;
   CString  s_class;
   
   s_class = ss_class;

   if (strchr(s_class,','))
   {
      char  tmp[80], *lp;
      strcpy(tmp,s_class);

      lp = strtok(tmp,",");

      c_class = lp;
      c_class.TrimLeft();
      c_class.TrimRight();

      lp = strtok(NULL,",");

      s_class = lp;
      s_class.TrimLeft();
      s_class.TrimRight();

      layerclass = get_classtype(c_class);   // redo the class
   }

   fprintf(fp,"setwindow form.mini\n");
   fprintf(fp,"FORM mini class %s\n",origclasses[layerclass]);
   fprintf(fp,"FORM mini subclass %s\n",s_class);

   return 1;
}

/*****************************************************************************/
/*
   Classes subclasses layers ....

   in allegro.out the incomming cad layer is assigned to a class, subclass.
   The .out class is used to different a layer "draw" on a component vs. a board. This
   means more or less a hardcoded class.

   However, the user can force any class by naming a subclass class,subclass.
   All responsibility is with the user.
*/
static CString skill_class_subclass(int layerclass, const char *ss_class)
{
   CString  skill;
   CString  c_class;
   CString  s_class;
   
   s_class = ss_class;

   if (strchr(s_class,','))
   {
      char  tmp[80], *lp;
      strcpy(tmp,s_class);

      lp = strtok(tmp,",");

      c_class = lp;
      c_class.TrimLeft();
      c_class.TrimRight();

      lp = strtok(NULL,",");

      s_class = lp;
      s_class.TrimLeft();
      s_class.TrimRight();

      layerclass = get_classtype(c_class);   // redo the class
   }

   skill = origclasses[layerclass];
   skill += "/";
   skill += s_class;

   return skill;
}

/*****************************************************************************/
/*
*/
static int  header_graphic(FILE *fp, int classtype, const char *subclass, double lineWidth)
{
   write_class_subclass(fp, classtype, subclass);

   fprintf(fp,"setwindow form.mini\n");
   fprintf(fp,"FORM mini line_width %1.*lf\n", output_units_accuracy, lineWidth);
   fprintf(fp,"setwindow pcb\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_pick(FILE *fp, double x, double y, int checkdouble)
{
   static   double   lastx, lasty;

   if (checkdouble)
   {
      if ((fabs(lastx - x) < SMALL_DELTA) && (fabs(lasty - y) < SMALL_DELTA))
         return FALSE;
   }

   lastx = x;
   lasty = y;

   return fprintf(fp,"pick %1.*lf %1.*lf\n", output_units_accuracy, x, output_units_accuracy, y);
}

/*****************************************************************************/
/*
*/
static int write_skill_xy(FILE *fp, double x, double y)
{
   return fprintf(fp," %1.*lf:%1.*lf", output_units_accuracy, x, output_units_accuracy, y);
}

/*****************************************************************************/
/*
*/
static int  add_line(FILE *fp, double p1x, double p1y, double p2x, double p2y,
                     double lineWidth, int lindex)
{
   fprintf(fp,"add line\n");
   fprintf(fp,"setwindow form.mini\n");
   fprintf(fp,"FORM mini lock_mode Line\n");
   fprintf(fp,"FORM mini lock_direction Off\n");
   header_graphic(fp,maplay[lindex].classtype, maplay[lindex].subclass,lineWidth);

   write_pick(fp, p1x, p1y, 0);
   write_pick(fp, p2x, p2y, 0);

   return 1;
}

/*****************************************************************************/
/*
*/
static int  add_circle(FILE *fp, double px, double py, double radius,
                     double lineWidth, int lindex)
{

   fprintf(fp,"add circle\n");
   header_graphic(fp,maplay[lindex].classtype, maplay[lindex].subclass,lineWidth);

   write_pick(fp, px, py, 0);
   write_pick(fp, px+radius, py, 0);

   return 1;
}

/*****************************************************************************/
/*
*/
static int  add_arc(FILE *fp, double centerx, double centery, double radius, double sa, double da,
                     double lineWidth, int lindex)
{

   double sx = RadToDeg(sa), sy = RadToDeg(da); // just for debug
   double end_angle;

   // all arcs must be counterclock -- this means I move the start point.
   if (RadToDeg(da) < 0)   // clockwise
   {
      sx = centerx + cos(sa+da)*radius;
      sy = centery + sin(sa+da)*radius;
      end_angle = RadToDeg(sa);
   }
   else
   {
      sx = centerx + cos(sa)*radius;
      sy = centery + sin(sa)*radius;
      end_angle = RadToDeg(sa + da);
   }

   if (end_angle < 0)   end_angle += 360;
   if (end_angle >360)  end_angle -= 360;

   fprintf(fp,"add rarc\n");
   header_graphic(fp,maplay[lindex].classtype, maplay[lindex].subclass,lineWidth);

   write_pick(fp, centerx, centery, 0);
   write_pick(fp, sx, sy, 0);
   fprintf(fp,"angle %1.3lf\n", end_angle);   // absolute angle

   return 1;
}

/*****************************************************************************/
/*
   Add polyline must be changed to include voids

*/
static int  add_polyline(FILE *fp, Point2 *points, int cnt, 
                         int polyFilled, int closed, int voidout, double lineWidth, int lindex,
                         int ml_pointer, int write_routes, CString netName)
{
   int      i;
   int      allow_shape = TRUE; // certain classes can not contain shapes
   int      write_shapefill = FALSE;
   CString  subclass;
   int      classtype;
   AllegroLayerStruct   *tmp = NULL;
   int      electrical = FALSE;
   double   accuracy = get_accuracy(doc);

   if (cnt < 2)   return 0;

   if (ml_pointer > -1) // this in for debug only.
   {
      tmp = mlArr[ml_pointer];

      if (tmp->stackNum != 0)
         electrical = TRUE;
   }

   // a subclass xxx,yyy can overwrite classes.
   subclass = maplay[lindex].subclass;
   classtype = maplay[lindex].classtype;

   if (strchr(maplay[lindex].subclass,','))
   {
      char  tmp[80], *lp;
      CString  c_class;

      strcpy(tmp,subclass);

      lp = strtok(tmp,",");

      c_class = lp;
      c_class.TrimLeft();
      c_class.TrimRight();

      lp = strtok(NULL,",");

      subclass = lp;
      subclass.TrimLeft();
      subclass.TrimRight();

      classtype = get_classtype(c_class); // redo the class
   }
   
   if (!(netName.IsEmpty()))
   {
      if (Allegro_Version < 150)
         fprintf(fp, "add fshape\n");
      else
         fprintf(fp, "shape add\n");

      write_class_subclass(fp, classtype, subclass);

      if (Allegro_Version >= 150)
      {
         fprintf(fp, "FORM mini dyns_fill_type Dynamic copper\n");
         fprintf(fp, "FORM mini dyns_defer_fill YES\n");
         fprintf(fp, "FORM mini dyns_netname \"%s\"\n",netName);
      }

      fprintf(fp,"setwindow pcb\n");

      for (i=0;i<cnt-1;i++)   // do not write the last entity
      {
         double da = atan(points[i].bulge) * 4;

         if (fabs(da) > SMALLANGLE)
         {
            double cx,cy,r,sa;
            double x1,y1,x2,y2;   

            x1 = points[i].x;
            y1 = points[i].y;
            x2 = points[i+1].x;
            y2 = points[i+1].y;

            ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

            // make positive start angle.
            if (sa < 0) sa += PI2;

            // here arc to poly
            int ii, ppolycnt = 255; // only for an arc 
            // start center
            Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));
            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));

            for (ii=0;ii<ppolycnt;ii++)
            {
               write_pick(fp, ppoly[ii].x, ppoly[ii].y, (i > 0));
            }

            free(ppoly);
         }
         else
         {
            write_pick(fp, points[i].x, points[i].y, (i > 0));
         }
      }

      fprintf(fp,"done\n");

      if (Allegro_Version < 150)
      {
         fprintf(fp,"changenet name\n");
         fprintf(fp,"fillin \"%s\"\n",netName);
         fprintf(fp,"shape fill\n");
      }

      return 1;
   }
   
   if (!SKILL || write_routes)   // on write routes, do use the old version and not SKILL
   {
      // need to redo this as filled etch should be written to fill script.
      if (classtype == CLASS_ETCH && (voidout || (polyFilled && lineWidth > SMALLNUMBER)))
      {
         if (voidout && !FilledVoidMessage)
         {
            fprintf(flog,"Filled Void Area detected on ETCH layer will be moved to MANUFACTURING class as lines\n");
            display_error++;
            FilledVoidMessage = TRUE;
         }

         if (!voidout && !FilledWidthMessage)
         {
            fprintf(flog,"Filled Area with Width detected on ETCH layer will be moved to MANUFACTURING class as lines\n");
            display_error++;
            FilledWidthMessage = TRUE;
         }

         classtype = CLASS_MANUFACTURING;
         subclass += "_AFILL";
         polyFilled = FALSE;
         voidout = FALSE;
         write_routes = FALSE;

         // here check if it was already in 
         int i, found = FALSE;

         for (i=0;i<newlaycnt;i++)
         {
            if (newlay[i].nonetch == TRUE &&
               newlay[i].classtype == classtype &&
               !STRCMPI(newlay[i].subclass, subclass))
            {
               found = TRUE;
               break;
            }
         }

         if (!found)
         {
            // here add
            fprintf(fp,"define subclass\n");
            fprintf(fp,"setwindow form.subclass\n");
            fprintf(fp,"FORM subclass %s pick\n",lclasses[classtype]);
            fprintf(fp,"setwindow form.subclass_nonetch\n");
            fprintf(fp,"FORM subclass_nonetch name %s\n",subclass);
            
            if (newlaycnt < MAX_NEWLAYERS)
            {
               newlay[newlaycnt].nonetch = TRUE;
               newlay[newlaycnt].classtype = classtype;
               newlay[newlaycnt].subclass = subclass;
               newlaycnt++;
            }
         }
      }
   } // if not SKILL

   // here are some exections to write out classes and subclasses.
   // not all filled on all classes are handled the same way
   // this classes, subclasses must be always polyFilled !
   if (closed || polyFilled)
   {
      //closed = FALSE;
      if (polyFilled)   write_shapefill = TRUE;

      if (classtype == CLASS_ETCH && !polyFilled)  // open on etch class
      {
         closed = FALSE;   // do not use a closed shape, but lines.
      }

      // according to randy 20-Mar-99 all keepouts are filled
      if (classtype == CLASS_PACKAGE_KEEPOUT)
         polyFilled = TRUE, write_shapefill = TRUE;
      else if (classtype == CLASS_ROUTE_KEEPOUT)
         polyFilled = TRUE, write_shapefill = TRUE;
      else if (classtype == CLASS_VIA_KEEPOUT)
         polyFilled = TRUE, write_shapefill = TRUE;
      else if (subclass.CompareNoCase("ALL") == 0)               
         polyFilled = TRUE, write_shapefill = FALSE;
      else
      {
         if (subclass.CompareNoCase("NO_GLOSS_ALL") == 0)      
            polyFilled = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("NO_GLOSS_BOTTOM") == 0)   
            polyFilled = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("NO_GLOSS_TOP") == 0)      
            polyFilled = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("NO_PROBE_BOTTOM") == 0)   
            polyFilled = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("NO_PROBE_TOP") == 0)      
            polyFilled = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("PLACE_BOUND_TOP") == 0)   
            polyFilled = TRUE, write_shapefill = TRUE;

         if (subclass.CompareNoCase("PLACE_BOUND_BOTTOM") == 0)      
            polyFilled = TRUE, write_shapefill = TRUE;
   
         if (subclass.CompareNoCase("BOTH_ROOMS") == 0)  
            closed = TRUE, write_shapefill = FALSE;
   
         if (subclass.CompareNoCase("BOTTOM_ROOM") == 0) 
            closed = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("CONSTRAINT_AREA") == 0)   
            closed = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("PHOTOPLOT_OUTLINE") == 0) 
            closed = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("SWITCH_AREA_BOTTOM") == 0)   
            closed = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("SWITCH_AREA_TOP") == 0)   
            closed = TRUE, write_shapefill = FALSE;

         if (subclass.CompareNoCase("TOP_ROOM") == 0) 
            closed = TRUE, write_shapefill = FALSE;

         if (classtype == CLASS_BOARD_GEOMETRY && subclass.CompareNoCase("OUTLINE") == 0) 
            closed = TRUE, write_shapefill = FALSE;
      } // all 
   }

   // certain classes can not have a void.
   if (voidout && !write_routes)
   {
      fprintf(flog,"Void in Class [%s] SubClass [%s] not allowed -> skipped\n",
         origclasses[classtype], subclass);
      display_error++;

      return 0;
   }

   if (voidout)
   {
      fprintf(fp,"void shape\n");
      header_graphic(fp, classtype, subclass, lineWidth);

      for (i=0;i<cnt-1;i++)   // do not write the last entity
      {
         double da = atan(points[i].bulge) * 4;

         if (fabs(da) > SMALLANGLE)
         {
            double cx,cy,r,sa;
            double x1,y1,x2,y2;   

            x1 = points[i].x;
            y1 = points[i].y;
            x2 = points[i+1].x;
            y2 = points[i+1].y;

            ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

            // make positive start angle.
            if (sa < 0) sa += PI2;

            // here arc to poly
            int ii, ppolycnt = 255; // only for an arc 
            // start center
            Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));
            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));

            for (ii=0;ii<ppolycnt;ii++)
            {
               write_pick(fp,ppoly[ii].x, ppoly[ii].y, (i > 0));
            }

            free(ppoly);
         }
         else
         {
            write_pick(fp,points[i].x, points[i].y, (i > 0));
         }
      }

      fprintf(fp,"done\n");
   }
   else if (polyFilled || closed)
   {
      if (SKILL)
      {  // here we are writing a closed or closed filled non-void shape using SKILL
         fprintf(fp,"(axlDBCreateShape(axlPathStart(list(");

         for (i=0;i<cnt;i++)  // do write the last entity
         {
            double da = atan(points[i].bulge) * 4;

            if (fabs(da) > SMALLANGLE)
            {  
               double cx,cy,r,sa;
               double x1,y1,x2,y2;   

               x1 = points[i].x;
               y1 = points[i].y;
               x2 = points[i+1].x;
               y2 = points[i+1].y;

               ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

               // make positive start angle.
               if (sa < 0) sa += PI2;

               // here arc to poly
               int ii, ppolycnt = 255; // only for an arc 
               // start center
               Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));
               ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));

               for (ii=0;ii<ppolycnt;ii++)
               {
                  write_skill_xy(fp, ppoly[ii].x, ppoly[ii].y);
               }

               free(ppoly);
            }
            else
            {
               write_skill_xy(fp, points[i].x, points[i].y);
            }
         }

         if (write_shapefill)
            fprintf(fp,"),0),t,\"%s\",nil,nil))\n", skill_class_subclass(classtype, subclass));
         else
            fprintf(fp,"),0),nil,\"%s\",nil,nil))\n", skill_class_subclass(classtype, subclass));
      }
      else
      {  // here we are writing a closed or closed filled non-void shape without SKILL
         if (classtype == CLASS_PACKAGE_KEEPOUT && isWritingPackageSymbol == TRUE)
            return 1;

         if (write_shapefill)
         {
            if (Allegro_Version >= 150)
               fprintf(fp, "shape add\n");  
            else
               fprintf(fp, "add fshape\n");
         }
         else
         {
            if (Allegro_Version >= 150)
               fprintf(fp, "shape add\n");  
            else
               fprintf(fp, "add ufshape\n");
         }

         // no linewidth here - according to Randy
         //header_graphic(fp, classtype, subclass,lineWidth);
         write_class_subclass(fp, classtype, subclass);

         if (Allegro_Version >= 150)
         {
            if (write_shapefill)
               fprintf(fp, "FORM mini dyns_fill_type Static solid\n");
            else
               fprintf(fp, "FORM mini dyns_fill_type Unfilled\n");

			fprintf(fp, "FORM mini dyns_lock_mode Line\n");
         }
       
         fprintf(fp,"setwindow pcb\n");

         for (i=0;i<cnt-1;i++)   // do not write the last entity
         {
            double da = atan(points[i].bulge) * 4;

            if (fabs(da) > SMALLANGLE)
            {
               double cx,cy,r,sa;
               double x1,y1,x2,y2;   

               x1 = points[i].x;
               y1 = points[i].y;
               x2 = points[i+1].x;
               y2 = points[i+1].y;

               ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

               // make positive start angle.
               if (sa < 0) sa += PI2;

               // here arc to poly
               int ii, ppolycnt = 255; // only for an arc 

               // start center
               Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));
               ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));
   
               for (ii=0;ii<ppolycnt;ii++)
               {
                  write_pick(fp, ppoly[ii].x, ppoly[ii].y, (i > 0));
               }

               free(ppoly);
            }
            else
            {
               write_pick(fp, points[i].x, points[i].y, (i > 0));
            }
         }

         fprintf(fp,"done\n");

         if ((write_shapefill) && (Allegro_Version < 150)) fprintf(fp,"shape fill\n");
      }
   }
   else
   {
      // Case dts0100460120 - this section had line-in-progress state problems, sometimes
      // printed "done" when no line was defined, sometimes dropped "done" when a line
      // was defined.

      int overlap = 0;

      for (i=0;i<cnt-2;i++)
      {
         double da = atan(points[i].bulge) * 4;

         if (fabs(da) <= SMALLANGLE)
         {
            // if segments overlap, do make every line 
            overlap += SegmentsOverlap(points[i].x, points[i].y, points[i+1].x, points[i+1].y, 
                                       points[i+1].x, points[i+1].y, points[i+2].x, points[i+2].y, accuracy);
         }
      }

      if (overlap)
      {
         for (i=0;i < cnt-1;i++)
         {
            double da = atan(points[i].bulge) * 4;

            // If this point and next point are nearly the same without a significant bulge then skip current point
            if ((fabs(points[i].x - points[i+1].x) < SMALL_DELTA) && 
                (fabs(points[i].y - points[i+1].y) < SMALL_DELTA) && 
                 fabs(da) <= SMALLANGLE)
            {        
               // skip this point
            }
            else
            {
               if (fabs(da) > SMALLANGLE)
               {
                  double cx,cy,r,sa;
                  double x1,y1,x2,y2;   

                  x1 = points[i].x;
                  y1 = points[i].y;
                  x2 = points[i+1].x;
                  y2 = points[i+1].y;

                  ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
                  add_arc(fp, cx, cy, r, sa, da, lineWidth, lindex);
               }
               else
               {
                  add_line(fp, points[i].x, points[i].y, points[i+1].x, points[i+1].y, lineWidth, lindex);
                  fprintf(fp,"done\n");
               }
            }
         }
      }
      else
      {
         bool lineInProgress = false;
         for (i=0;i < cnt-1;i++)
         {
            double da = atan(points[i].bulge) * 4;

            // If this point and next point are nearly the same without a significant bulge then skip current point
            if ((fabs(points[i].x - points[i+1].x) < SMALL_DELTA) && 
                (fabs(points[i].y - points[i+1].y) < SMALL_DELTA) && 
                 fabs(da) <= SMALLANGLE)
            {        
               // skip this point
            }
            else
            {
               if (fabs(da) > SMALLANGLE)
               {
                  // Terminate possible current line
                  if (lineInProgress) // terminate the line, reset state
                  {
                     fprintf(fp,"done\n");
                     lineInProgress = false;
                  }

                  double cx,cy,r,sa;
                  double x1,y1,x2,y2;    

                  x1 = points[i].x;
                  y1 = points[i].y;
                  x2 = points[i+1].x;
                  y2 = points[i+1].y;

                  ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
                  add_arc(fp, cx, cy, r, sa, da, lineWidth, lindex);
                  fprintf(fp,"done\n");
               }
               else
               {
                  if (!lineInProgress)
                  {
                     // Start new line
                     add_line(fp, points[i].x, points[i].y, points[i+1].x, points[i+1].y, lineWidth, lindex);                                                 
                     lineInProgress = true;
                  }
                  else
                  {
                     // Add vertex to line in progress
                     write_pick(fp, points[i+1].x, points[i+1].y, 1);
                  }
               }
            }
         }

         if (lineInProgress) // then terminate the line
            fprintf(fp,"done\n");

      } // not overlap
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int get_textindex(double h)
{
   int   i, index = 1;

   for (i=0;i<textblockcnt;i++)
   {
      if (textblock[i].height <= h)    /// get the closest, but never bigger.
         index = textblock[i].index;
   }

   return index;     // default
}

/*****************************************************************************/
/*
*/
static int add_text( FILE *fp, const char *prosa, double x, double y,
                     double height, double width, double rot, int mirror, 
                     int ta, int la, int lindex)
{
   unsigned int   i;

   if (STRICMP(prosa, "Y30") == 0)
      int blah = 0;

   CString h_just;

   switch (ta)
   {
   case 0:
      h_just = "Left";
      break;
   case 1:
      h_just = "Center";
      break;
   case 2:
      h_just = "Right";
      break;
   default:
      h_just = "Left";
      break;
   }
   
   // Need to correct X,Y placement to account for vertical allignment of center or top (vj != 0)
   if (la != 0)
   {
      double len = 0.0;

      if (la == 1)
         len = height/2;
      else if (la == 2)
         len = height;

      x += (len * cos(rot - (Pi / 2)));
      y += (len * sin(rot - (Pi / 2)));
   }

   write_class_subclass(fp, maplay[lindex].classtype, maplay[lindex].subclass);

   fprintf(fp, "add text\n");

   write_class_subclass(fp, maplay[lindex].classtype, maplay[lindex].subclass);

   fprintf(fp, "setwindow form.mini\n");
   fprintf(fp, "FORM mini angle %1.3lf\n", RadToDeg(rot));
   fprintf(fp, "FORM mini text_block %d\n", get_textindex(height));
   fprintf(fp, "FORM mini mirror %s\n", (mirror)?"YES":"NO");
   fprintf(fp, "FORM mini text_justification %s\n", h_just); 
   fprintf(fp, "setwindow pcb\n");

   write_pick(fp, x, y, 0);

   // if text contains " than the start/end are '
   // if the text contains ' than the start/end are "
   // if both, there is a problem
   int   singlequote = 0;
   int   doublequote = 0;

   /*CString tmpString = prosa; 
   //the replacing of \ to \\, " to \" and ' to \' has been removed
   // to accomadate for an Allegro bug. If the bug is fixed in a later ISV we will enable for
   // customers with that specific ISV
   if (Allegro_Version == 142)
   {

      if (!tmpString.CompareNoCase("q\'ty"))
         int i = 0;

      tmpString.Replace("\\", "\\\\"); // This will print as "\\"
      tmpString.Replace("\'", "\\\'"); // This will print as "\'"
      tmpString.Replace("\"", "\\\""); // This will print as "\""

      fprintf(fp,"text \"%s\"\n", tmpString); 
   }
   else
   { */
      for (i=0;i<strlen(prosa);i++)
      {
         if (prosa[i] == '\'')   singlequote++;
         if (prosa[i] == '"')    doublequote++;
      }

      if (doublequote)
      {
         // convert single to double
         fprintf(fp,"text '"); 

         for (i=0;i<strlen(prosa);i++)
         {
            if (prosa[i] == '\'')   
               fprintf(fp,"\"");
            else
               fprintf(fp,"%c",prosa[i]);
         }

         if (prosa[strlen(prosa)-1] == '`')
            fprintf(fp," ");

         fprintf(fp,"'\n");
      }
      else
      {
         fprintf(fp,"text \"%s", prosa); 

         if (prosa[strlen(prosa)-1] == '`')
            fprintf(fp," ");

         fprintf(fp,"\"\n");
      }
   //}

   fprintf(fp,"done\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_geometry_properties(FILE *fp, CAttributes* map, double x, double y)
{
   if (map == NULL)
      return 0;

   WORD keyword;
   Attrib* attrib;

   // define all used properties   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      if (strlen(doc->getKeyWordArray()[keyword]->out) == 0)
         continue;

      CString  key = check_name('a', doc->getKeyWordArray()[keyword]->out);

      fprintf(fp, "define property \n");
      fprintf(fp, "setwindow form.dpr_select\n");
      fprintf(fp, "FORM dpr_select name %s\n",key);
      fprintf(fp, "setwindow form.dpr_define\n");
      fprintf(fp, "FORM dpr_define shapes_elem YES \n");
      fprintf(fp, "FORM dpr_define data_type STRING \n");
      fprintf(fp, "FORM dpr_define apply  \n");
      fprintf(fp, "setwindow form.dpr_select\n");
      fprintf(fp, "FORM dpr_select done  \n");
   }

   // set all graphic off
   fprintf(fp, "setwindow pcb\n");
   fprintf(fp, "color \n");
   fprintf(fp, "setwindow form.cvf_main\n");
   fprintf(fp, "FORM cvf_main global_vis All Invisible \n");
   fprintf(fp, "fillin yes \n");
   fprintf(fp, "FORM cvf_main class_list Geometry \n");

   if (Allegro_Version > 120)
      fprintf(fp, "FORM cvf_main package_geometry/place_bound_top/visible YES\n");
   else
      fprintf(fp, "FORM cvf_main subclass%d visible5 YES \n", dialogplaceboundtop);

   fprintf(fp, "FORM cvf_main ok  \n");

   // loop attributes
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      if (strlen(doc->getKeyWordArray()[keyword]->out) == 0)
         continue;

      CString  key = check_name('a', doc->getKeyWordArray()[keyword]->out);

      fprintf(fp, "property edit\n"); 
      fprintf(fp, "pick %1.*lf %1.*lf\n", output_units_accuracy, x, output_units_accuracy, y);
      fprintf(fp, "setwindow form.editprop1\n");
      fprintf(fp, "FORM editprop1 properties %s \n", key);
      fprintf(fp, "setwindow form.editprop2\n");


      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp, "FORM editprop2 %s ele_prop_value '%d' \n", key, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp, "FORM editprop2 %s ele_prop_value '%lg' \n", key, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            fprintf(fp,"FORM editprop2 %s ele_prop_value '", key);

            char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));
            
            CString temp2 = temp;
            temp2.Replace('!', '`');
            temp2.Replace('\'', '`');

            strcpy(temp, temp2);
            
            tok = strtok(temp, "\n");

            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
                  fprintf(fp, "%s", tok); // multiple values are delimited by \n

               tok = strtok(NULL, "\n");
            }

            free(temp);
            fprintf(fp, "' \n");
         }

         break;
      }

      fprintf(fp, "FORM editprop2 apply\n");
      fprintf(fp, "setwindow form.editprop1\n");
      fprintf(fp, "FORM editprop1 done\n");
   }

   // switch all graphic on.
   fprintf(fp, "setwindow pcb\n");
   fprintf(fp, "done \n");
   fprintf(fp, "color \n");
   fprintf(fp, "setwindow form.cvf_main\n");
   fprintf(fp, "FORM cvf_main global_vis All Visible \n");
   fprintf(fp, "fillin yes \n");
   fprintf(fp, "FORM cvf_main ok  \n");

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_easyoutline_properties(FILE *fp, CPntList *pp, int lindex, CAttributes* map)
{
   CPnt *p;
   
   if (pp == NULL)   return 0;

   // no layer index defined.
   if (lindex < 0)   return 0;

   polycnt = 0;
   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt     *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
            ErrorMessage("Poly Bulge data error !");
         else
            p2 = pp->GetAt(pos);
                     
         ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);

         // make positive start angle.
         if (sa < 0) sa += PI2;

         ppolycnt = 255;

         // start center
         ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));  

         for (i=0;i<ppolycnt;i++)
         {
            polyarray.SetAtGrow(polycnt,ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2   p2;
         p2.x = p->x;
         p2.y = p->y;
         p2.bulge = p->bulge;

         polyarray.SetAtGrow(polycnt,p2);
         polycnt++;
      }
   }

   int done = FALSE,i;

   while (!done)
   {
      done = TRUE;

      for (i=1;i<polycnt;i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);

         if (fabs(p1.x - p2.x) < SMALL_DELTA && 
             fabs(p1.y - p2.y) < SMALL_DELTA)
         {
               polyarray.RemoveAt(i);
               done = FALSE;
               polycnt--;

               break;
         }
      }
   }

   if (polycnt < 4)  return 0;   // this can not be a closed outline, because it must be minimum 4

   if (Allegro_Version < 150)
      fprintf(fp, "add fshape\n");
   else
      fprintf(fp, "shape add\n");

   write_class_subclass(fp,maplay[lindex].classtype, maplay[lindex].subclass);

   if (Allegro_Version >= 150)
   {
      fprintf(fp, "FORM mini dyns_fill_type Static solid\n");
	  fprintf(fp, "FORM mini dyns_lock_mode Line\n");
   }

   fprintf(fp,"setwindow form.mini\n");
   fprintf(fp,"setwindow pcb\n");

   //fprintf(wfp,"\t%d\n",polycnt);
   for (i=0;i<polycnt-1;i++)  // polycnt - 1 : do not do the last one.
   {
      Point2 p;
      p = polyarray.ElementAt(i);
      write_pick(fp, p.x, p.y, (i > 0));
   }

   fprintf(fp,"done\n");

   // no fill on outlines
   // fprintf(fp,"shape fill\n");
   
   if (map != NULL)
   {
      Point2   p;
      p = polyarray.ElementAt(0);
      // here write properties
      do_geometry_properties(fp, map, p.x, p.y);
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_easykeepinout(FILE *fp, CPoly *pp, const char *s)
{
   CPnt *p;
   
   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;

   POSITION pos = pp->getPntList().GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->getPntList().GetNext(pos);

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt     *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
            ErrorMessage("Poly Bulge data error !");
         else
            p2 = pp->getPntList().GetAt(pos);
                     
         ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);

         // make positive start angle.
         if (sa < 0) sa += PI2;

         ppolycnt = 255;

         // start center
         ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));  

         for (i=0;i<ppolycnt;i++)
         {
            polyarray.SetAtGrow(polycnt,ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2   p2;
         p2.x = p->x;
         p2.y = p->y;
         p2.bulge = p->bulge;

         polyarray.SetAtGrow(polycnt,p2);
         polycnt++;
      }
   }

   int done = FALSE,i;

   while (!done)
   {
      done = TRUE;

      for (i=1;i<polycnt;i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);

         if (fabs(p1.x - p2.x) < SMALL_DELTA &&
             fabs(p1.y - p2.y) < SMALL_DELTA)
         {
               polyarray.RemoveAt(i);
               done = FALSE;
               polycnt--;

               break;
         }
      }
   }

   fprintf(fp,"%s\n",s);
   //fprintf(wfp,"\t%d\n",polycnt);

   for (i=0;i<polycnt-1;i++)  // do not do the last one
   {
      Point2 p;
      p = polyarray.ElementAt(i);

      write_pick(fp,p.x, p.y, (i > 0));
   }

   fprintf(fp,"done\n");

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_easykeepinrouter(FILE *fp, CPntList *pp)
{
   CPnt *p;
   
   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;

   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt     *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
            ErrorMessage("Poly Bulge data error !");
         else
            p2 = pp->GetAt(pos);
                     
         ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);

         // make positive start angle.
         if (sa < 0) sa += PI2;

         ppolycnt = 255;

         // start center
         ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));  

         for (i=0;i<ppolycnt;i++)
         {
            polyarray.SetAtGrow(polycnt,ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2   p2;
         p2.x = p->x;
         p2.y = p->y;
         p2.bulge = p->bulge;

         polyarray.SetAtGrow(polycnt,p2);
         polycnt++;
      }
   }

   int done = FALSE,i;

   while (!done)
   {
      done = TRUE;

      for (i=1;i<polycnt;i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);

         if (fabs(p1.x - p2.x) < SMALL_DELTA &&
             fabs(p1.y - p2.y) < SMALL_DELTA)
         {
               polyarray.RemoveAt(i);
               done = FALSE;
               polycnt--;

               break;
         }
      }
   }

   if (SKILL && (Allegro_Version < 150))
   {
      fprintf(fp,"(axlDBCreateShape(axlPathStart(list(");

      for (i=0;i<polycnt;i++) // do the last one
      {
         Point2 p;
         p = polyarray.ElementAt(i);
         write_skill_xy(fp, p.x, p.y);
      }

      fprintf(fp,"),0),nil,\"%s\",nil,nil))\n", skill_class_subclass(CLASS_ROUTE_KEEPIN, "ALL"));
   }
   else
   {
      if (Allegro_Version < 150)
        fprintf(fp, "add ufshape\n");
      else
         fprintf(fp, "shape add\n");

      fprintf(fp,"setwindow form.mini\n");
      fprintf(fp,"FORM mini class ROUTE KEEPIN\n");
      fprintf(fp,"FORM mini subclass ALL\n");

      if (Allegro_Version >= 150)
	  {
        fprintf(fp, "FORM mini dyns_fill_type Unfilled\n");
		fprintf(fp, "FORM mini dyns_lock_mode Line\n");
	  }

      fprintf(fp,"setwindow form.mini\n");
      fprintf(fp,"FORM mini line_width 1.0\n");
      fprintf(fp,"setwindow pcb\n");

      //fprintf(wfp,"\t%d\n",polycnt);

      for (i=0;i<polycnt-1;i++)  // do not do the last one
      {
         Point2 p;
         p = polyarray.ElementAt(i);

         write_pick(fp,p.x, p.y, (i > 0));
      }

      fprintf(fp,"done\n");
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_easykeepinpackage(FILE *fp, CPntList *pp)
{
   CPnt *p;
   
   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;

   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt     *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
            ErrorMessage("Poly Bulge data error !");
         else
            p2 = pp->GetAt(pos);
                     
         ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);

         // make positive start angle.
         if (sa < 0) sa += PI2;

         ppolycnt = 255;

         // start center
         ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE)); 

         for (i=0;i<ppolycnt;i++)
         {
            polyarray.SetAtGrow(polycnt,ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2   p2;
         p2.x = p->x;
         p2.y = p->y;
         p2.bulge = p->bulge;

         polyarray.SetAtGrow(polycnt,p2);
         polycnt++;
      }
   }

   int done = FALSE,i;

   while (!done)
   {
      done = TRUE;

      for (i=1;i<polycnt;i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);

         if (fabs(p1.x - p2.x) < SMALL_DELTA &&
             fabs(p1.y - p2.y) < SMALL_DELTA)
         {
               polyarray.RemoveAt(i);
               done = FALSE;
               polycnt--;

               break;
         }
      }
   }

   if (SKILL)
   {
      fprintf(fp,"(axlDBCreateShape(axlPathStart(list(");

      for (i=0;i<polycnt;i++) // do the last one
      {
         Point2 p;
         p = polyarray.ElementAt(i);

         write_skill_xy(fp, p.x, p.y);
      }

      fprintf(fp,"),0),nil,\"%s\",nil,nil))\n", skill_class_subclass(CLASS_PACKAGE_KEEPIN, "ALL"));
   }
   else
   {
      if (Allegro_Version < 150)
        fprintf(fp,"add ufshape\n");
      else
        fprintf(fp, "shape add\n");

      fprintf(fp,"setwindow form.mini\n");
      fprintf(fp,"FORM mini class PACKAGE KEEPIN\n");
      fprintf(fp,"FORM mini subclass ALL\n");

      if (Allegro_Version >= 150)
	  {
        fprintf(fp, "FORM mini dyns_fill_type Unfilled\n");
		fprintf(fp, "FORM mini dyns_lock_mode Line\n");
	  }

      fprintf(fp,"setwindow form.mini\n");
      fprintf(fp,"FORM mini line_width 1.0\n");
      fprintf(fp,"setwindow pcb\n");

      //fprintf(wfp,"\t%d\n",polycnt);
      for (i=0;i<polycnt-1;i++)  // do not do the last one
      {
         Point2 p;
         p = polyarray.ElementAt(i);
         write_pick(fp,p.x, p.y, (i > 0));
      }

      fprintf(fp,"done\n");
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/******************************************************************************
* drawing_unit_size_header
*/
static void drawing_unit_size_header(FILE *fp, double xmin, double ymin, double xmax, double ymax)
{
   // Extents must always include origin
   if (xmin > 0)
   {
      xmin = 0;
      xmin -= DRAWING_ENLARGE;
   }

   if (ymin > 0)
   {
      ymin = 0;   
      ymin -= DRAWING_ENLARGE;
   }

   if (xmax < 0)
   {
      xmax = 0;
      // added double drawing enlarge because Ref and dev are added at 0,0
      // and we do not want this to extend past right edge of design.
      xmax += 2 * DRAWING_ENLARGE;
   }

   if (ymax < 0)
   {
      ymax = 0;   
      ymax += DRAWING_ENLARGE;
   }
   
   fprintf(fp, "FORM drawing units %s\n", output_units_string);
   fprintf(fp, "FORM drawing accuracy %d\n", output_units_accuracy);
   fprintf(fp, "FORM drawing size Other\n");
   fprintf(fp, "FORM drawing width %1.*lf\n", output_units_accuracy, fabs(xmax-xmin));
   fprintf(fp, "FORM drawing height %1.*lf\n", output_units_accuracy, fabs(ymax-ymin));
   fprintf(fp, "FORM drawing x %1.*lf\n", output_units_accuracy, xmin);
   fprintf(fp, "FORM drawing y %1.*lf\n", output_units_accuracy, ymin);
   fprintf(fp, "FORM drawing done\n");
}

/*****************************************************************************/
/*
*/
static int write_shape_symbol_header(FILE *fshx, const char *shxname)
{
   // here make a dummy pad
   fprintf(fshx,"new\n");

   if (Allegro_Version > 136) // also uses 136
   {
      fprintf(fshx,"newdrawfillin \"%s.dra\" \"Shape Symbol\"\n",shxname);
      fprintf(fshx,"setwindow pcb\n");
      fprintf(fshx,"drawing param\n");
   }
   else
   {
      fprintf(fshx,"newdrawfillin \"%s\" \"SHAPE\"\n",shxname);
      fprintf(fshx,"drawing param\n");
   }

   fprintf(fshx,"setwindow form.drawing\n");
   fprintf(fshx,"FORM drawing type Shape\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_easypadshapeout(FILE *fp, CPntList *pp, const char *s)
{
   CPnt *p;
   double   xmin = 0,ymin = 0,xmax = 0,ymax = 0;

   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;

   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      if (polycnt == 0)
      {
         xmin = xmax = p->x;
         ymin = ymax = p->y;
      }

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt     *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
            ErrorMessage("Poly Bulge data error !");
         else
            p2 = pp->GetAt(pos);
                     
         ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);

         // make positive start angle.
         if (sa < 0) sa += PI2;

         ppolycnt = 255;

         // start center
         ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));  

         for (i=0;i<ppolycnt;i++)
         {
            polyarray.SetAtGrow(polycnt,ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2   p2;
         p2.x = p->x;
         p2.y = p->y;
         p2.bulge = p->bulge;

         polyarray.SetAtGrow(polycnt,p2);
         polycnt++;
      }
   }

   int done = FALSE,i;

   for (i=0;i<polycnt;i++)
   {
      Point2 p2 = polyarray.ElementAt(i);

      if (p2.x < xmin)  xmin = p2.x;      
      if (p2.y < ymin)  ymin = p2.y;
      if (p2.x > xmax)  xmax = p2.x;
      if (p2.y > ymax)  ymax = p2.y;
   }

   // Make sure the extents includes the origin.
   if (xmin > 0)
      xmin = 0;

   if (ymin > 0)
      ymin = 0;

   if (xmax < 0)
      xmax = 0;

   if (ymax < 0)
      ymax = 0;

   // add something to extensions so that the drawing extens has some space.
   xmin -= PAD_ENLARGE;
   ymin -= PAD_ENLARGE;
   xmax += PAD_ENLARGE;
   ymax += PAD_ENLARGE;

   while (!done)
   {
      done = TRUE;

      for (i=1;i<polycnt;i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);

         if (fabs(p1.x - p2.x) < SMALL_DELTA && 
             fabs(p1.y - p2.y) < SMALL_DELTA)
         {
               polyarray.RemoveAt(i);
               done = FALSE;
               polycnt--;

               break;
         }
      }
   }

   write_shape_symbol_header(fp, s);   
   drawing_unit_size_header(fp, xmin, ymin, xmax, ymax);

   if (Allegro_Version < 150)
   {
      fprintf(fp,"add fshape\n");
      fprintf(fp,"setwindow form.mini\n");
      fprintf(fp,"FORM mini lock_direction Off\n");
   }
   else
   {
      fprintf(fp, "shape add\n");
      fprintf(fp, "setwindow form.mini\n");
      fprintf(fp, "FORM mini dyns_fill_type Static solid\n");
      fprintf(fp, "FORM mini dyns_lock_mode Line\n");
   }

   fprintf(fp,"setwindow pcb\n");

   for (i=0;i<polycnt-1;i++)  // do not do the last one
   {
      Point2 p;
      p = polyarray.ElementAt(i);

      write_pick(fp,p.x, p.y, (i > 0));
   }

   fprintf(fp,"done\n");

   if (Allegro_Version < 150)
      fprintf(fp,"shape fill\n");

   fprintf(fp,"save_as\n");
   fprintf(fp,"fillin \"%s.dra\"\n",s);
   fprintf(fp,"create symbol\n");
   fprintf(fp,"fillin \"%s.ssm\"\n",s);
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin \"\"\n");
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin yes\n");
   fprintf(fp,"\n");
   fprintf(fp,"# end of padshape script\n");

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

//--------------------------------------------------------------
static int Allegro_ApertureSHAPEData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;

   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_TEXT)    continue;

      if (np->getDataType() != T_INSERT)
      {
         // here add with width, so that lines and segment combinations can be found.
         Outline_Add_With_Width(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == insertTypePin)
            continue;

         if (np->getInsert()->getInsertType() == insertTypeDrillHole)
            continue;

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

         if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            found += Allegro_ApertureSHAPEData(&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(), embeddedLevel+1);
         } // end else not aperture
      } // if INSERT
   } // end Allegro_ApertureSHAPEData */

   return found;
}

//--------------------------------------------------------------
static int Allegro_OutlineSHAPEData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary,
      int gr_class)
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
         if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
            layer = np->getLayerIndex();
         else 
            layer = insertLayer;

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // never do etch
         if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

         if (primary)
         {
            if (np->getGraphicClass() != gr_class)  
               continue;
         }
         else
         {
            // do it
         }

         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == insertTypePin)
            continue;

         if (np->getInsert()->getInsertType() == insertTypeDrillHole)
            continue;

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

         if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
               block_layer = np->getLayerIndex();
            else 
               block_layer = insertLayer;

            found += Allegro_OutlineSHAPEData(&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, primary,gr_class);
         } // end else not aperture
      } // if INSERT
   } // end Allegro_OutlineSHAPEData */

   return found;
}

/****************************************************************************/
/*
*/
static int get_classtype(const char *n)
{
   int   i;

   for (i=0;i<CLASS_MAX_CLASSES;i++)
   {
      if (!STRCMPI(n,lclasses[i]))
         return i;
   }

   fprintf(flog, "Unknown Class [%s]\n",n);
   display_error++;

   return CLASS_UNKNOWN;
}

/******************************************************************************
* load_ALLEGROsettings
*/
static int load_ALLEGROsettings(const char *fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   textblockcnt = 0;
   deflaycnt = 0;
   maplaycnt = 0;
   maperrorcnt = 0;
   newlaycnt = 0;
   TRUE_DEVICE = FALSE;
   tiewidth = 15;
   airwidth = 15;
   antipadenlargement = 20;
   dialogplaceboundtop = 6;
   Allegro_Version = 120;
   DRC_ON = FALSE;
   SKILL = FALSE;
   BATCH_NOGRAPHIC = FALSE;
   FilledVoidMessage = FALSE;
   ARC_ANGLE_DEGREE = 5;
   write_thermals = FALSE;
   writeBoundary = TRUE;
   Ref_On_Display = false;
   Ref_On_Silkscreen = false;
   Ref_On_Assembly = true;
   Dev_On_Display = false;
   Dev_On_Silkscreen = false;
   Dev_On_Assembly = true;

   //Ref_Cnt = 1;
   //Dev_Cnt = 1;
   RefSubClass = SubClassAssembly;
   DevSubClass = SubClassAssembly;

   default_via = "";
   packageheaderscript = "";  
   packagefooterscript = "";
   boardheaderscript = "";
   boardfooterscript = "";
   mechanicalheaderscript = "";
   mechanicalfooterscript = "";

   HIERACHIEMARKER = FALSE;

   output_units_string  = "Mils";
   output_units = UNIT_MILS;

   deviceTypeAttributeName = ATT_DEVICETYPE;

   pshapecnt = 0; output_units_accuracy = 3;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"  ALLEGRO Settings", MB_OK | MB_ICONHAND);

      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;

      if (lp[0] == '.')
      {
         if (!STRCMPI(lp, ".ALLEGRO_VERSION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) continue;

            int version = round(atof(lp) * 10);

            if (version == 130)  
               Allegro_Version = 130;
            else if (version == 136)   
               Allegro_Version = 136;
            else if (version == 140 || version == 141 || version == 142)   
               Allegro_Version = version;
            else if (version == 150 || version == 151 || version == 152)   
               Allegro_Version = version;
            else
            {
               fprintf(flog, "Unknown .ALLEGRO_VERSION setting [%s] in [%s] -> set to [12]\n",
                  lp, fname);

               display_error++;
            }
         }
         else if (!STRICMP(lp, ".ARCSTEPANGLE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            ARC_ANGLE_DEGREE = atof(lp);

            if (ARC_ANGLE_DEGREE < 4)     // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too small, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 4;
            }
            else if (ARC_ANGLE_DEGREE > 45)    // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too large, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 45;
            }
         }
         else if (!STRCMPI(lp, ".OUTPUT_UNIT_STRING"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (!STRCMPI(lp,"Millimeter"))
            {
               output_units_string  = "Millimeter";
               output_units = UNIT_MM;
            }
         }
         else if (!STRCMPI(lp, ".OUTPUT_UNIT_ACCURACY"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            output_units_accuracy = atoi(lp);
         }
         else if (!STRCMPI(lp, ".LAYERMAP"))
         {
            // layername classtype class subclass
            CString  setupname;
            int      classtype;  // etch etc...
            CString  subclass_layer;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            setupname = lp;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            classtype = get_classtype(lp);

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            subclass_layer = lp;

            if (maplaycnt < MAX_MAPLAYERS)
            {
               maplay[maplaycnt].setupname = setupname;
               maplay[maplaycnt].classtype = classtype;
               maplay[maplaycnt].subclass = subclass_layer;
               maplaycnt++;
            }
            else
            {
               ErrorMessage("Too many Map Layers");
            }
         }
         else if (!STRCMPI(lp, ".TEXTBLOCK"))
         {
            int   index;
            double height;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            index = atoi(lp);

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            height = atof(lp);

            if (textblockcnt < MAX_TEXTBLOCKS)
            {
               textblock[textblockcnt].index = index;
               textblock[textblockcnt].height = height;
               textblockcnt++;
            }
         }
         else if (!STRCMPI(lp, ".DRAWINGENLARGE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            DRAWING_ENLARGE = atof(lp);
         }
         else if (!STRCMPI(lp, ".PADENLARGE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            PAD_ENLARGE = atof(lp);
         }
         else if (!STRCMPI(lp, ".BOARDENLARGE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            BOARD_ENLARGE = atof(lp);
         }
         else if (!STRCMPI(lp, ".LAYERDIALOGPLACEBOUNDTOP"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            dialogplaceboundtop = atoi(lp);
         }
         else if (!STRCMPI(lp, ".PACKAGEHEADERSCRIPT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            packageheaderscript = lp;
         }
         else if (!STRCMPI(lp, ".PACKAGEFOOTERSCRIPT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            packagefooterscript = lp;
         }
         else if (!STRCMPI(lp, ".MECHANICALHEADERSCRIPT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            mechanicalheaderscript = lp;
         }
         else if (!STRCMPI(lp, ".MECHANICALFOOTERSCRIPT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            mechanicalfooterscript = lp;
         }
         else if (!STRCMPI(lp, ".BOARDFOOTERSCRIPT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            boardfooterscript = lp;
         }
         else if (!STRCMPI(lp, ".BOARDHEADERSCRIPT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            boardheaderscript = lp;
         }
         else if (!STRCMPI(lp, ".ALLEGRO_DRC"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (!STRCMPI(lp, "Off"))
               DRC_ON = FALSE;
            else if (!STRCMPI(lp, "On"))
               DRC_ON = TRUE;
         }
         else if (!STRCMPI(lp, ".BATCHMODE_NOGRAPHIC"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               BATCH_NOGRAPHIC = TRUE;
         }
         else if (!STRCMPI(lp, ".USE_SKILL"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               SKILL = TRUE;
         }
         else if (!STRCMPI(lp, ".ADDNONETCHCLASS"))
         {
            // layername classtype class subclass
            int      classtype;  // etch etc...
            CString  subclass_layer;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            classtype = get_classtype(lp);

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            subclass_layer = lp;

            if (newlaycnt < MAX_NEWLAYERS)
            {
               newlay[newlaycnt].nonetch = TRUE;
               newlay[newlaycnt].classtype = classtype;
               newlay[newlaycnt].subclass = subclass_layer;
               newlaycnt++;
            }
         }
         else if (!STRCMPI(lp, ".TRUE_DEVICE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               TRUE_DEVICE = TRUE;
         }
         else if (!STRCMPI(lp, ".HIERACHIEMARKER"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               HIERACHIEMARKER = TRUE;
         }
         else if (!STRCMPI(lp, ".TIEWIDTH"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            tiewidth = atof(lp);
         }
         else if (!STRCMPI(lp, ".AIRWIDTH"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            airwidth = atof(lp);
         }
         else if (!STRCMPI(lp, ".ANTIPADENLARGEMENT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            antipadenlargement = atof(lp);
         }
         else if (!STRCMPI(lp, ".LAYERDEFAULT"))
         {
            CString l1,l2;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            l1 = lp;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            l2 = lp;

            if (deflaycnt < MAX_LAYERS)
            {
               deflay[deflaycnt].in  = l1;
               deflay[deflaycnt].out = l2;
               deflaycnt++;
            }
         }
         else if (!STRCMPI(lp, ".WRITE_THERMAL_APR"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            write_thermals = !STRICMP(lp, "y");
         }
         else if (!STRCMPI(lp, ".WRITE_BOUNDARY"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (toupper(lp[0]) == 'N')
               writeBoundary = FALSE;
         }
         else if (!STRCMPI(lp, ".REF_ON_DISPLAY"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            Ref_On_Display = (toupper(lp[0]) == 'Y');

            if (Ref_On_Display)
            {
               RefSubClass |= SubClassDisplay;
            }
            else
            {
               RefSubClass &= ~SubClassDisplay;
            }
         }
         else if (!STRCMPI(lp, ".REF_ON_SILKSCREEN"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            Ref_On_Silkscreen = (toupper(lp[0]) == 'Y');

            if (Ref_On_Silkscreen)
            {
               RefSubClass |= SubClassSilkscreen;
            }
            else
            {
               RefSubClass &= ~SubClassSilkscreen;
            }
         }
         else if (!STRCMPI(lp, ".REF_ON_ASSEMBLY"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            Ref_On_Assembly = (toupper(lp[0]) == 'Y');

            if (Ref_On_Assembly)
            {
               RefSubClass |= SubClassAssembly;
            }
            else
            {
               RefSubClass &= ~SubClassAssembly;
            }
         }
         else if (!STRCMPI(lp, ".DEV_ON_DISPLAY"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            Dev_On_Display = (toupper(lp[0]) == 'Y');

            if (Dev_On_Display)
            {
               DevSubClass |= SubClassDisplay;
            }
            else
            {
               DevSubClass &= ~SubClassDisplay;
            }
         }
         else if (!STRCMPI(lp, ".DEV_ON_SILKSCREEN"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            Dev_On_Silkscreen = (toupper(lp[0]) == 'Y');

            if (Dev_On_Silkscreen)
            {
               DevSubClass |= SubClassSilkscreen;
            }
            else
            {
               DevSubClass &= ~SubClassSilkscreen;
            }
         }
         else if (!STRCMPI(lp, ".DEV_ON_ASSEMBLY"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            Dev_On_Assembly = (toupper(lp[0]) == 'Y');

            if (Dev_On_Assembly)
            {
               DevSubClass |= SubClassAssembly;
            }
            else
            {
               DevSubClass &= ~SubClassAssembly;
            }
         }
         else if (!STRCMPI(lp, ".DEVICETYPE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            deviceTypeAttributeName = lp;
            deviceTypeAttributeName.Replace("\"","");
         }
      } // end if (lp[0] == '.')
   } // end while

   if (!Ref_On_Display && (!Ref_On_Silkscreen && !Ref_On_Assembly))
   {
      Ref_On_Assembly = true;
      //Ref_Cnt = 1;
      RefSubClass = SubClassAssembly;
   }
   
   if (!(Dev_On_Display || (Dev_On_Silkscreen || Dev_On_Assembly)))
   {
      Dev_On_Assembly = true;
      //Dev_Cnt = 1;
      DevSubClass = SubClassAssembly;
   }

   if (output_units == UNIT_MILS && (output_units_accuracy > 2 && Allegro_Version > 140))
      output_units_accuracy = 2; //mils accuracy may = 2 in allegro 14.2 and later

   if (output_units == UNIT_MILS && output_units_accuracy > 3)
      output_units_accuracy = 3; //mils accuracy max = 3

   fclose(fp);

   return 1;
}

//--------------------------------------------------------------
static   int   get_layernr_from_index(int index)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (mlArr[i]->layerindex == index)
         return mlArr[i]->stackNum;
   }

   return -99;
}

//--------------------------------------------------------------
static const char *get_defaultlayer(const char *l)
{
   int i;

   for (i=0;i<deflaycnt;i++)
   {
      if (deflay[i].in.Compare(l) == 0)
         return deflay[i].out;
   }

   return l;
}

/******************************************************************************
* do_layerlist
*/
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0;
   char        typ = 'D';
   CString     alleglay;

   maxArr = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];

      if (layer == NULL)   continue; // could have been deleted.

      signr = 0;
      alleglay = get_defaultlayer(layer->getName());
      typ = 'D';

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();

         if (layer->getLayerType() == LAYTYPE_PAD_TOP)
         {
            signr = LAY_TOP;
            alleglay = "PADTOP";
         }
         else if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
         {
            signr = LAY_BOT;
            alleglay = "PADBOT";
         }
         else if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
            alleglay.Format("TOP");
         else if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
            alleglay.Format("BOTTOM");
         else
            alleglay.Format("INT%d",signr);
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
         alleglay = "PAD";
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
         alleglay = "PAD";
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         signr = LAY_INNER;
         alleglay = "SIGNAL";
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         signr = LAY_TOP;
         alleglay = "PADTOP";
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         signr = LAY_BOT;
         alleglay = "PADBOT";
      }

      if (signr == 0)
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
      }

      // here check if layer->getName() is not already done

      //mlArr.SetSizes
      AllegroLayerStruct *ml = new AllegroLayerStruct;
      ml->stackNum = signr;
      ml->layerindex = j;
      ml->on = TRUE;
      ml->type = typ;
      ml->oldName = layer->getName();
      ml->newName = alleglay;
      mlArr.SetAtGrow(maxArr++, ml);
   }

   return;
}

//--------------------------------------------------------------
static int net_layerlist(FileStruct *file)
{

   NetStruct *net;
   POSITION  netPos;
   Attrib   *a;
   int      powercnt = 0;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);

      // if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      if (a =  is_attvalue(doc, net->getAttributesRef(), ATT_POWERNET, 2))
      {
         //mlArr.SetSizes
         AllegroLayerStruct *ml = new AllegroLayerStruct;
         ml->stackNum = 1;
         ml->on = TRUE;
         ml->type = 'P';
         ml->oldName = net->getNetName();
         ml->newName.Format("POWER_%d", ++powercnt);
         mlArr.SetAtGrow(maxArr++, ml);
      }
   }

   return 1;
}

/******************************************************************************
* edit_layerlist
*/
static int edit_layerlist()
{
   // fill array
   AllegroLayer mldlg;

   mldlg.arr = &mlArr;
   mldlg.maxArr = maxArr;

   if (mldlg.DoModal() != IDOK) 
      return FALSE;

   int   i;
   max_signalcnt = 0;

   for (i=0;i<maxArr;i++)
   {
      if (mlArr[i]->stackNum > max_signalcnt)
         max_signalcnt = mlArr[i]->stackNum;
   }

   return TRUE;
}

/******************************************************************************
* free_layerlist
*/
static void free_layerlist()
{
   for (int i=0; i<maxArr; i++)
      delete mlArr[i];

   mlArr.RemoveAll();
}


/*****************************************************************************/
/*
*/
static   int   padstack_script_header(FILE *fp, const char *padname)
{
   created_by(fp);

   fprintf(fp,"setwindow form.padedit\n");
   fprintf(fp,"\n");

   fprintf(fp,"FORM padedit padname %s\n", padname);
   fprintf(fp,"FORM padedit units %s\n", output_units_string);
   fprintf(fp,"FORM padedit decimal_places %d\n", output_units_accuracy);
   fprintf(fp,"\n");
   
   // init to zero
   fprintf(fp,"FORM padedit begin_layer edit_pad\n");
   fprintf(fp,"setwindow form.pad.BEGIN_LAYER\n");
   fprintf(fp,"FORM pad.BEGIN_LAYER regular-pad geometry Null\n");
   fprintf(fp,"FORM pad.BEGIN_LAYER thermal-relief geometry Null\n");
   fprintf(fp,"FORM pad.BEGIN_LAYER anti-pad geometry Null\n");
   fprintf(fp,"FORM pad.BEGIN_LAYER done\n");
   fprintf(fp,"FORM padedit end_layer edit_pad\n");
   fprintf(fp,"setwindow form.pad.END_LAYER\n");
   fprintf(fp,"FORM pad.END_LAYER regular-pad geometry Null\n");
   fprintf(fp,"FORM pad.END_LAYER thermal-relief geometry Null\n");
   fprintf(fp,"FORM pad.END_LAYER anti-pad geometry Null\n");
   fprintf(fp,"FORM pad.END_LAYER done\n");
   fprintf(fp,"FORM padedit default_internal edit_pad\n");
   fprintf(fp,"setwindow form.pad.DEFAULT_INTERNAL\n");
   fprintf(fp,"FORM pad.DEFAULT_INTERNAL regular-pad geometry Null\n");
   fprintf(fp,"FORM pad.DEFAULT_INTERNAL thermal-relief geometry Null\n");
   fprintf(fp,"FORM pad.DEFAULT_INTERNAL anti-pad geometry Null\n");
   fprintf(fp,"FORM pad.DEFAULT_INTERNAL done\n");
   fprintf(fp,"FORM padedit end_layer edit_pad\n");
   fprintf(fp,"\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   padstack_136_script_header(FILE *fp, const char *padname)
{
   created_by(fp);

   fprintf(fp,"setwindow form.padedit\n");
   fprintf(fp,"\n");

   fprintf(fp,"pse_new\n");
   fprintf(fp,"fillin \"%s\"\n", padname);
   fprintf(fp,"FORM padedit units %s\n", output_units_string);
   fprintf(fp,"FORM padedit decimal_places %d\n", output_units_accuracy);
   fprintf(fp,"\n");


   fprintf(fp,"\n");

   return 1;
}

/*****************************************************************************/
/*
   does less than 136 version
*/
static   int   padstack_script_footer(FILE *fp)
{
   fprintf(fp,"FORM padedit archive\n");
   fprintf(fp,"FORM padedit done\n");
   fprintf(fp,"\n");
   fprintf(fp,"# end of padstack script\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   padstack_136_script_footer(FILE *fp, const char *padfile, int drillwarning)
{
   fprintf(fp,"pse_save\n");

   if (drillwarning)
   {
      fprintf(fp,"fillin yes\nsetwindow text\nclose\nfillin yes\nsetwindow form.padedit\n");
   }

   fprintf(fp,"pse_exit\n");
   fprintf(fp,"\n");
   fprintf(fp,"# end of padstack script\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   do_status(FILE *fp)
{
   if (Allegro_Version >= 136) // 
   {
      fprintf(fp,"status\n");
      fprintf(fp,"setwindow form.status\n");
      fprintf(fp,"FORM status line_lock\n");
      fprintf(fp,"FORM status lock_direction Off\n");
      fprintf(fp,"FORM status lock_mode Line\n");
      fprintf(fp,"FORM status symbol\n");
      fprintf(fp,"FORM status symbol_mirror NO\n");
      fprintf(fp,"FORM status done\n");
   }
   else
   {
      fprintf(fp,"setwindow pcb\n");
      fprintf(fp,"status\n");
      fprintf(fp,"setwindow form.status\n");
      fprintf(fp,"FORM status lock_direction Off\n");
      fprintf(fp,"FORM status lock_mode Line\n");
      fprintf(fp,"FORM status symbol_mirror NO\n");
      fprintf(fp,"FORM status drawing_size\n");
      fprintf(fp,"FORM status done\n");
      fprintf(fp,"setwindow pcb\n");
      fprintf(fp,"window center; pick     0.0     0.0 \n");
      fprintf(fp,"\n");
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   package_script_header(FILE *fp, const char *packagename,
                                     double xmin, double ymin, double xmax, double ymax)
{

   created_by(fp);

   fprintf(fp,"setwindow pcb\n");
   fprintf(fp,"new\n");

   if (Allegro_Version >= 136) // 
      fprintf(fp,"newdrawfillin \"%s.dra\" \"Package Symbol\"\n",packagename);
   else
      fprintf(fp,"newdrawfillin \"%s\" \"PACKAGE\"\n",packagename);

   fprintf(fp,"drawing param\n");
   fprintf(fp,"setwindow form.drawing\n");
   fprintf(fp,"FORM drawing type Package\n");

   drawing_unit_size_header(fp, xmin, ymin, xmax, ymax);

   do_status(fp);

   if (strlen(packageheaderscript))
      fprintf(fp,"replay %s\n",packageheaderscript);

   add_newlayers(fp);

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   package_script_footer(FILE *fp, const char *pack)
{
   if (strlen(packagefooterscript))
      fprintf(fp,"replay %s\n",packagefooterscript);

   fprintf(fp,"create symbol\n");
   fprintf(fp,"fillin \"%s\"\n",pack);
   fprintf(fp,"setwindow pcb\n");
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin \"\"\n");
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin yes\n");
   fprintf(fp,"\n");
   fprintf(fp,"# end of package script\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   format_script_header(FILE *fp, const char *formatname,
                                     double xmin, double ymin, double xmax, double ymax)
{
   created_by(fp);

   fprintf(fp,"setwindow pcb\n");
   fprintf(fp,"new\n");

   if (Allegro_Version >= 136) // 
      fprintf(fp,"newdrawfillin \"%s.dra\" \"Format Symbol\"\n",formatname);
   else
      fprintf(fp,"newdrawfillin \"%s\" \"FORMAT\"\n",formatname);

   fprintf(fp,"drawing param\n");
   fprintf(fp,"setwindow form.drawing\n");
   fprintf(fp,"FORM drawing type Format\n");

   drawing_unit_size_header(fp, xmin, ymin, xmax, ymax);

   do_status(fp);

   add_newlayers(fp);

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   format_script_footer(FILE *fp, const char *pack)
{
   fprintf(fp,"create symbol\n");
   fprintf(fp,"fillin \"%s\"\n",pack);
   fprintf(fp,"setwindow pcb\n");
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin \"\"\n");
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin yes\n");
   fprintf(fp,"\n");
   fprintf(fp,"# end of format script\n");

   return 1;
}

/******************************************************************************
* mechanical_script_header
*/
static int mechanical_script_header(FILE *fp, const char *mechanicalname, double xmin, double ymin, double xmax, double ymax)
{
   created_by(fp);

   fprintf(fp, "setwindow pcb\n");
   fprintf(fp, "new\n");

   if (Allegro_Version >= 140)
      fprintf(fp, "newdrawfillin \"%s.dra\" \"Mechanical Symbol\"\n", mechanicalname);
   else
      fprintf(fp, "newdrawfillin \"%s\" \"MECHANICAL\"\n", mechanicalname);

   fprintf(fp, "drawing param\n");
   fprintf(fp, "setwindow form.drawing\n");
   fprintf(fp, "FORM drawing type Mechanical\n");

   drawing_unit_size_header(fp, xmin, ymin, xmax, ymax);

   do_status(fp);

   if (strlen(mechanicalheaderscript))
      fprintf(fp,"replay %s\n",mechanicalheaderscript);

   add_newlayers(fp);

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   mechanical_script_footer(FILE *fp, const char *pack)
{
   if (strlen(mechanicalfooterscript))
      fprintf(fp,"replay %s\n",mechanicalfooterscript);

   fprintf(fp,"create symbol\n");
   fprintf(fp,"fillin \"%s\"\n",pack);
   fprintf(fp,"setwindow pcb\n");
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin \"\"\n");
   fprintf(fp,"exit\n");
   fprintf(fp,"fillin yes\n");
   fprintf(fp,"\n");
   fprintf(fp,"# end of mechanical script\n");

   return 1;
}
                                                
/*****************************************************************************/
/*
*/
static   int   brd_script_header(FILE *fp, const char *brdname)
{
   created_by(fp);

   if (strlen(boardheaderscript))
      fprintf(fp,"replay %s\n",boardheaderscript);

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   fill_script_header(FILE *fp, const char *brdname)
{
   created_by(fp);

   fprintf(fp,"open\n");
   fprintf(fp,"fillin \"%s.brd\"\n",brdname);
   fprintf(fp,"setwindow pcb\n");
   fprintf(fp,"status\n");
   fprintf(fp,"setwindow form.status\n");

   if (Allegro_Version < 150)
   {
      if (DRC_ON)
          fprintf(fp,"FORM status drc_on yes\n");
       else
          fprintf(fp,"FORM status drc_off yes\n");
   }
   else 
   {
      if (DRC_ON)
          fprintf(fp,"FORM status drc_on YES\n");
       else
          fprintf(fp,"FORM status drc_on NO\n");
   }

   if (Allegro_Version >= 136) // 
      fprintf(fp,"FORM status line_lock\n");

   fprintf(fp,"FORM status lock_direction Off\n");
   fprintf(fp,"FORM status drawing_size\n");
   fprintf(fp,"FORM status done\n");
   fprintf(fp,"setwindow pcb\n");

   return 1;
}

/*****************************************************************************/
/*
*/
static   int   brd_script_footer(FILE *fp)
{
   if (strlen(boardfooterscript))
      fprintf(fp,"replay %s\n",boardfooterscript);

   fprintf(fp,"\n");
   fprintf(fp,"# end of autobrd script\n");

   return 1;
}

/******************************************************************************
* do_technologyfile
*/
static int do_technologyfile(const char *fname)
{
   FILE  *tfp;
   CString  tname;

   tname = filePathName;
   tname +=fname;
   tname +=".tech";
   // tname.Format("%s.tech", fname);

   if ((tfp = fopen(tname,"wt")) == NULL)
   {
      ErrorMessage("Can not open TECHNOLOGY FILE\n");

      return -1;
   }

   int   l, i, n= 0 ;

   fprintf(tfp,"(allegroTechFile \"%s\"\n",tname);
   fprintf(tfp,"\t(crossSectionDescriptor\n");

   // must be first.
   fprintf(tfp,"\t\t(layer 0\n");
   fprintf(tfp,"\t\t\t(layerType SURFACE)\n");
   fprintf(tfp,"\t\t\t(layerMaterial \"AIR\")\n");
   fprintf(tfp,"\t\t)\n");

   n++;

   for (l= 1;l <= max_signalcnt;l++)
   {
      for (i=0;i<maxArr;i++)
      {
         int lindex;

         if (mlArr[i]->stackNum != l)  continue;

         if ((lindex = get_class_subclass(mlArr[i]->newName, CLASS_ETCH)) < 0)   
         {
            // here need to update maplay class if a new class was defined, not otherwise.
            if (!in_maperror(mlArr[i]->newName, lclasses[CLASS_ETCH]))
            {
               fprintf(flog, "Layer [%s] Class [%s] not found -> (update allegro.out)\n",
                  mlArr[i]->newName, lclasses[CLASS_ETCH]);
            }  

            continue;
         }

         fprintf(tfp,"\t\t(layer %d\n", l);

         CString  ltype = "CONDUCTOR";

         if (mlArr[i]->type == 'P')
            ltype = "PLANE";

         fprintf(tfp,"\t\t\t(layerType %s)\n", ltype);

         fprintf(tfp,"\t\t\t(layerName \"%s\")\n", maplay[lindex].subclass);

         fprintf(tfp,"\t\t\t(layerMaterial \"COPPER\")\n");

         if (mlArr[i]->type == 'P')
            fprintf(tfp,"\t\t\t(layerArtworkNegativeFlag TRUE)\n");

         fprintf(tfp,"\t\t)\n");
         n++;
/*
         fprintf(tfp,"%d Layer Name [%s] ALLEGRO Layer Name [%s] Layer Number [%d] Layer Type [%c]\n",i,
            mlArr[i]->oldName, mlArr[i]->newName, mlArr[i]->stackNum,
            mlArr[i]->type);
*/
         break;
      }
   }

   // must be last
   fprintf(tfp,"\t\t(layer %d\n", n+1);
   fprintf(tfp,"\t\t\t(layerType SURFACE)\n");
   fprintf(tfp,"\t\t\t(layerMaterial \"AIR\")\n");
   fprintf(tfp,"\t\t)\n");

   fprintf(tfp,"\t)\n");
   fprintf(tfp,")\n");

   fclose(tfp);

   return 1;
}

/*****************************************************************************/
/*
   always also update write padform
*/
static int write_padformthermo(FILE *fp, FILE *shapefile, const char *l, 
                        double sizea, double sizeb,
                        double Xoffset, double Yoffset,
                        int shape, double rot,int complexshape,
                        double thermosizea, double thermosizeb,
                        double thermoXoffset, double thermoYoffset,
                        int thermoshape, double thermorotation,
                        double restringA, double restringB)
{
   double   reliefA, reliefB, antiA, antiB, sizeA, sizeB;
   double   xoffset, yoffset;
   double   rotation = RadToDeg(rot);
   double   accuracy = get_accuracy(doc);
   while (rotation < 0)    rotation += 360;
   while (rotation >= 360) rotation -= 360;

   int      i, r = (int)floor(rotation / 90 + 0.5);
      
   double   unitsFactor = Units_Factor(page_units, output_units);

   CString  shxname;

   if (shape == T_COMPLEX)
   {
      BlockStruct *subblock = doc->Find_Block_by_Num(complexshape);

      if (subblock == NULL)
      {
         ErrorMessage("Can not find Complex Aperture");

         return -1;
      }

      sizea = 0;
      sizeb = 0;

      shxname = check_name('f', AllegroNameMgr->GetName(subblock));

      if (!shapeAlreadyInAutoShape.Lookup(shxname, shxname))
      {
         FILE  *fshx;
         CString fname;
         fname = filePathName;
         fname +=shxname;
         fname +=".scx";

         if ((fshx = fopen(fname,"wt")) == NULL)
         {
            CString  tmp;
            tmp.Format("Can not open PadShape file [%s]",fname);
            ErrorMessage(tmp);

            return -1;
         }

         // here get block and make a closed perfect outline
         Outline_Start(doc);         
         int found = Allegro_ApertureSHAPEData(&(subblock->getDataList()), Xoffset, Yoffset, DegToRad(rotation), 0, 
                     file->getScale() * unitsFactor, 0);

         subblock->setFlagBits(BL_BLOCK_APERTURE);

         if (found)
         {
            int returnCode;

            if (!write_easypadshapeout(fshx, Outline_GetOutline(&returnCode, accuracy), shxname))
            {
               fprintf(flog, "Invalid Padshape [%s] -> approximated size written!\n", AllegroNameMgr->GetName(subblock));
               display_error++;

               doc->validateBlockExtents(subblock);

               double xmin = subblock->getXmin()*unitsFactor, xmax = subblock->getXmax()*unitsFactor, 
                     ymin = subblock->getYmin()*unitsFactor, ymax = subblock->getYmax()*unitsFactor;

               write_shape_symbol_header(fshx, shxname);
               drawing_unit_size_header(fshx, xmin-PAD_ENLARGE, ymin-PAD_ENLARGE, xmax+PAD_ENLARGE, ymax+PAD_ENLARGE);

               if (Allegro_Version < 150)
               {
                  fprintf(fshx,"add fshape\n");
                  fprintf(fshx,"setwindow form.mini\n");
                  fprintf(fshx,"FORM mini lock_direction Off\n");
               }
               else
               {
                  fprintf(fshx, "shape add\n");
                  fprintf(fshx,"setwindow form.mini\n");
                  fprintf(fshx, "FORM mini dyns_fill_type Static solid\n");
                  fprintf(fshx, "FORM mini dyns_lock_mode Line\n");
               }

               fprintf(fshx,"setwindow pcb\n");

               write_pick(fshx,xmin, ymin, 0);
               write_pick(fshx,xmax, ymin, 1);
               write_pick(fshx,xmax, ymax, 1);
               write_pick(fshx,xmin, ymax, 1);

               fprintf(fshx,"done\n");

               if (Allegro_Version < 150)
                  fprintf(fshx,"shape fill\n");

               fprintf(fshx,"save_as\n");
               fprintf(fshx,"fillin \"%s.dra\"\n",shxname);
               fprintf(fshx,"create symbol\n");
               fprintf(fshx,"fillin \"%s.ssm\"\n",shxname);
               fprintf(fshx,"exit\n");
               fprintf(fshx,"fillin \"\"\n");
               fprintf(fshx,"exit\n");
               fprintf(fshx,"fillin yes\n");
               fprintf(fshx,"\n");
               fprintf(fshx,"# end of padshape script\n");
            }

            // tname.Format("%s.tech", fname);
            fprintf(shapefile,"allegro -s %s.scx %s\n",shxname, (BATCH_NOGRAPHIC)?"-nographic": "");
            fflush(shapefile);
         }
         else 
         {
            
            double oneMilOutputUnit = Units_Factor(UNIT_MILS, output_units);

            write_shape_symbol_header(fshx, shxname);
            drawing_unit_size_header(fshx, -PAD_ENLARGE, -PAD_ENLARGE, PAD_ENLARGE, PAD_ENLARGE);

            if (Allegro_Version < 150)
            {
               fprintf(fshx, "add fshape\n");
               fprintf(fshx, "setwindow form.mini\n");
               fprintf(fshx, "FORM mini lock_direction Off\n");
            }
            else
            {
               fprintf(fshx, "shape add\n");
               fprintf(fshx, "setwindow form.mini\n");
               fprintf(fshx, "FORM mini dyns_fill_type Static solid\n");
               fprintf(fshx, "FORM mini dyns_lock_mode Line\n");
            }

            fprintf(fshx, "setwindow pcb\n");

            write_pick(fshx, -oneMilOutputUnit, -oneMilOutputUnit, 0);
            write_pick(fshx, oneMilOutputUnit, -oneMilOutputUnit, 1);
            write_pick(fshx, oneMilOutputUnit, oneMilOutputUnit, 1);
            write_pick(fshx, -oneMilOutputUnit, oneMilOutputUnit, 1);

            fprintf(fshx, "done\n");

            if (Allegro_Version < 150)
               fprintf(fshx, "shape fill\n");

            fprintf(fshx, "save_as\n");
            fprintf(fshx, "fillin \"%s.dra\"\n",shxname);
            fprintf(fshx, "create symbol\n");
            fprintf(fshx, "fillin \"%s.ssm\"\n",shxname);
            fprintf(fshx, "exit\n");
            fprintf(fshx, "fillin \"\"\n");
            fprintf(fshx, "exit\n");
            fprintf(fshx, "fillin yes\n");
            fprintf(fshx, "\n");
            fprintf(fshx, "# end of padshape script\n");

            fprintf(shapefile, "allegro -s %s.scx %s\n", shxname, (BATCH_NOGRAPHIC)?"-nographic": "");
            fflush(shapefile);

            // no element was found
            fprintf(flog, "No Graphic found for [%s][%s]\n", subblock->getName(), AllegroNameMgr->GetName(subblock));
            display_error++;
         }

         shapeAlreadyInAutoShape.SetAt(shxname, shxname);
         fflush(fshx);
         fclose(fshx);
         Outline_FreeResults();
      }
   }
   else
   {
      if (sizea < SMALL_DELTA && sizeb < SMALL_DELTA)
         return 0;
   }


   // padstack for can not have a rotation, must adjust for it
   if ((r*90.0) != round(rotation))
   {
      fprintf(flog, "Rotation [%d] other than 90 degree in Aperture\n", round(rotation));
      display_error++;
   }

   sizeA = sizea; sizeB = sizeb;
   xoffset = Xoffset; yoffset = Yoffset;

   for (i=0;i<r;i++)
   {
      double t;
      t = sizeA;
      sizeA = sizeB;
      sizeB = t;
      t = xoffset;
      xoffset = yoffset;
      yoffset = t;
   }

   // needs to be clarified
   if (thermoshape < 0)
   {
      reliefA = sizeA + antipadenlargement;
      reliefB = sizeB + antipadenlargement;
      antiA = sizeA + antipadenlargement;
      antiB = sizeB + antipadenlargement;
   }
   else
   {
      reliefA = thermosizea;
      reliefB = thermosizeb;
      antiA = sizeA + restringA;
      antiB = sizeB + restringB;
   }

   fprintf(fp,"FORM padedit %s edit_pad\n", l);
   fprintf(fp,"setwindow form.pad.%s\n", l);

   switch (shape)
   {
   case T_OCTAGON:
   case T_TARGET:
   case T_DONUT:
   case T_ROUND:
      fprintf(fp,"FORM pad.%s regular-pad geometry Circle\n", l);
      fprintf(fp,"FORM pad.%s regular-pad width %1.*lf\n", l, output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad height %1.*lf\n", l, output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad x_offset %1.*lf\n", l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s regular-pad y_offset %1.*lf\n", l, output_units_accuracy,yoffset);

      fprintf(fp,"FORM pad.%s thermal-relief geometry Circle\n", l);
      fprintf(fp,"FORM pad.%s thermal-relief width %1.*lf\n", l, output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief height %1.*lf\n", l, output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief x_offset %1.*lf\n", l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s thermal-relief y_offset %1.*lf\n", l, output_units_accuracy,yoffset);
      fprintf(fp,"FORM pad.%s anti-pad geometry Circle\n", l);
      fprintf(fp,"FORM pad.%s anti-pad width %1.*lf\n", l, output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad height %1.*lf\n", l, output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad x_offset %1.*lf\n", l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s anti-pad y_offset %1.*lf\n", l, output_units_accuracy,yoffset);

      if (write_thermals)
         fprintf(fp,"FORM pad.%s thermal-relief flash %s\n", l, do_thermalreliefname(shape, antiA, sizeA));

      break;
   case T_OBLONG:
      fprintf(fp,"FORM pad.%s regular-pad geometry Oblong\n",l);
      fprintf(fp,"FORM pad.%s regular-pad width %1.*lf\n",l, output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad height %1.*lf\n",l, output_units_accuracy,sizeB);
      fprintf(fp,"FORM pad.%s regular-pad x_offset %1.*lf\n",l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s regular-pad y_offset %1.*lf\n",l, output_units_accuracy,yoffset);

      fprintf(fp,"FORM pad.%s thermal-relief geometry Oblong\n",l);
      fprintf(fp,"FORM pad.%s thermal-relief width %1.*lf\n",l, output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief height %1.*lf\n",l, output_units_accuracy,reliefB);
      fprintf(fp,"FORM pad.%s thermal-relief x_offset %1.*lf\n",l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s thermal-relief y_offset %1.*lf\n",l, output_units_accuracy,yoffset);
      fprintf(fp,"FORM pad.%s anti-pad geometry Oblong\n",l);
      fprintf(fp,"FORM pad.%s anti-pad width %1.*lf\n", l, output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad height %1.*lf\n", l, output_units_accuracy,antiB);
      fprintf(fp,"FORM pad.%s anti-pad x_offset %1.*lf\n", l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s anti-pad y_offset %1.*lf\n", l, output_units_accuracy,yoffset);

      if (write_thermals)
         fprintf(fp,"FORM pad.%s thermal-relief flash %s\n", l, do_thermalreliefname(shape, antiA, sizeA));

      break;
   case T_RECTANGLE:
      fprintf(fp,"FORM pad.%s regular-pad geometry Rectangle\n",l);
      fprintf(fp,"FORM pad.%s regular-pad width %1.*lf\n",l, output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad height %1.*lf\n",l, output_units_accuracy,sizeB);
      fprintf(fp,"FORM pad.%s regular-pad x_offset %1.*lf\n",l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s regular-pad y_offset %1.*lf\n",l, output_units_accuracy,yoffset);

      fprintf(fp,"FORM pad.%s thermal-relief geometry Rectangle\n",l);
      fprintf(fp,"FORM pad.%s thermal-relief width %1.*lf\n",l, output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief height %1.*lf\n",l, output_units_accuracy,reliefB);
      fprintf(fp,"FORM pad.%s thermal-relief x_offset %1.*lf\n",l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s thermal-relief y_offset %1.*lf\n",l, output_units_accuracy,yoffset);
      fprintf(fp,"FORM pad.%s anti-pad geometry Rectangle\n", l);
      fprintf(fp,"FORM pad.%s anti-pad width %1.*lf\n",l, output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad height %1.*lf\n",l, output_units_accuracy,antiB);
      fprintf(fp,"FORM pad.%s anti-pad x_offset %1.*lf\n",l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s anti-pad y_offset %1.*lf\n",l, output_units_accuracy,yoffset);

      if (write_thermals)
         fprintf(fp,"FORM pad.%s thermal-relief flash %s\n", l, do_thermalreliefname(shape, antiA, sizeA));

      break;
   case T_SQUARE:
      fprintf(fp,"FORM pad.%s regular-pad geometry Square\n",l);
      fprintf(fp,"FORM pad.%s regular-pad width %1.*lf\n",l,output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad height %1.*lf\n",l,output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad x_offset %1.*lf\n",l,output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s regular-pad y_offset %1.*lf\n",l,output_units_accuracy,yoffset);

      fprintf(fp,"FORM pad.%s thermal-relief geometry Square\n",l);
      fprintf(fp,"FORM pad.%s thermal-relief width %1.*lf\n",l,output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief height %1.*lf\n",l,output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief x_offset %1.*lf\n",l,output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s thermal-relief y_offset %1.*lf\n",l,output_units_accuracy,yoffset);
      fprintf(fp,"FORM pad.%s anti-pad geometry Square\n",l);
      fprintf(fp,"FORM pad.%s anti-pad width %1.*lf\n",l,output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad height %1.*lf\n",l,output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad x_offset %1.*lf\n",l,output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s anti-pad y_offset %1.*lf\n",l,output_units_accuracy,yoffset);

      if (write_thermals)
         fprintf(fp,"FORM pad.%s thermal-relief flash %s\n", l, do_thermalreliefname(shape, antiA, sizeA));

      break;
   case T_COMPLEX:
      if (Allegro_Version >= 136) // 
      {
         fprintf(fp,"FORM pad.%s geometry Shape\n",l);
         fprintf(fp,"FORM pad.%s shapename %s\n",l,shxname);
      }
      else
      {
         fprintf(fp,"FORM pad.%s regular-pad geometry Shape\n",l);
         fprintf(fp,"FORM pad.%s regular-pad shapename %s\n",l,shxname);
      }

      break;
   case T_UNDEFINED:             
   case T_BLANK:
      fprintf(fp,"FORM pad.%s regular-pad geometry Null\n",l);
      fprintf(fp,"FORM pad.%s regular-pad width %1.*lf\n",l,output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad height %1.*lf\n",l,output_units_accuracy,sizeA);
      fprintf(fp,"FORM pad.%s regular-pad x_offset %1.*lf\n",l,output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s regular-pad y_offset %1.*lf\n",l,output_units_accuracy,yoffset);
      fprintf(fp,"FORM pad.%s thermal-relief geometry Null\n",l);
      fprintf(fp,"FORM pad.%s thermal-relief width %1.*lf\n",l,output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief height %1.*lf\n",l,output_units_accuracy,reliefA);
      fprintf(fp,"FORM pad.%s thermal-relief x_offset %1.*lf\n",l,output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s thermal-relief y_offset %1.*lf\n",l,output_units_accuracy,yoffset);
      fprintf(fp,"FORM pad.%s anti-pad geometry Null\n",l);
      fprintf(fp,"FORM pad.%s anti-pad width %1.*lf\n",l,output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad height %1.*lf\n",l,output_units_accuracy,antiA);
      fprintf(fp,"FORM pad.%s anti-pad x_offset %1.*lf\n",l,output_units_accuracy,xoffset);
      fprintf(fp,"FORM pad.%s anti-pad y_offset %1.*lf\n",l,output_units_accuracy,yoffset);

      if (write_thermals)
         fprintf(fp,"FORM pad.%s thermal-relief flash %s\n", l, do_thermalreliefname(shape, antiA, sizeA));

      break;
   default:
      fprintf(flog, "Unknown aperture type\n");
      display_error++;

      break;
   } // switch shape

   fprintf(fp,"FORM pad.%s done\n\n",l);

   return 1;
} // write_padformthermo


/******************************************************************************
* write_padformthermo_136
   always also update write padform
*/
static int write_padformthermo_136(FILE *fPadstack, FILE *fAutoShape, int layer, double sizea, double sizeb,
                                   double xoffset, double yoffset, int shape, double rotation,int complexshape,
                                   double thermosizea, double thermosizeb, double thermoXoffset, double thermoYoffset,
                                   int thermoshape, double thermorotation, double restringA, double restringB, int thermal, BOOL SMD)
{
   double accuracy = get_accuracy(doc);
   double unitsFactor = Units_Factor(page_units, output_units);

   double X_offset;
   double Y_offset;
   CString shapeName;

   if (shape == T_COMPLEX)
   {
      BlockStruct *subblock = doc->Find_Block_by_Num(complexshape);

      if (subblock == NULL)
      {
         ErrorMessage("Can not find Complex Aperture");
         return -1;
      }
      sizea = 0;
      sizeb = 0;

      shapeName = check_name('f', AllegroNameMgr->GetName(subblock));

      if (!shapeAlreadyInAutoShape.Lookup(shapeName, shapeName))
      {
         CString fname = filePathName;
         fname += shapeName;
         fname +=".scx";

         FILE *fshx;

         if ((fshx = fopen(fname, "wt")) == NULL)
         {
            CString tmp;
            tmp.Format("Can not open PadShape file [%s]", fname);
            ErrorMessage(tmp);

            return -1;
         }

         // here get block and make a closed perfect outline
         Outline_Start(doc);         
         int found = Allegro_ApertureSHAPEData(&subblock->getDataList(), xoffset, yoffset, rotation, 0, file->getScale() * unitsFactor, 0);
         subblock->setFlagBits(BL_BLOCK_APERTURE);

         if (found)
         {
            int returnCode;

            if (!write_easypadshapeout(fshx, Outline_GetOutline(&returnCode, accuracy), shapeName))
            {
               fprintf(flog, "Invalid Padshape [%s] -> approximated size written!\n", subblock->getName());
               display_error++;

               doc->validateBlockExtents(subblock);

               double xmin = subblock->getXmin() * unitsFactor;
               double xmax = subblock->getXmax() * unitsFactor;
               double ymin = subblock->getYmin() * unitsFactor;
               double ymax = subblock->getYmax() * unitsFactor;

               write_shape_symbol_header(fshx, shapeName);
               drawing_unit_size_header(fshx, xmin - PAD_ENLARGE, ymin - PAD_ENLARGE, xmax + PAD_ENLARGE, ymax + PAD_ENLARGE);

               if (Allegro_Version < 150)
               {  
                  fprintf(fshx, "add fshape\n");
                  fprintf(fshx, "setwindow form.mini\n");
                  fprintf(fshx, "FORM mini lock_direction Off\n");
               }
               else
               {
                  fprintf(fshx, "shape add\n");
                  fprintf(fshx, "setwindow form.mini\n");
                  fprintf(fshx, "FORM mini dyns_fill_type Static solid\n");
                  fprintf(fshx, "FORM mini dyns_lock_mode Line\n");
               }
                  
               fprintf(fshx, "setwindow pcb\n");

               write_pick(fshx, xmin, ymin, 0);
               write_pick(fshx, xmax, ymin, 1);
               write_pick(fshx, xmax, ymax, 1);
               write_pick(fshx, xmin, ymax, 1);

               fprintf(fshx, "done\n");

               if (Allegro_Version < 150)
                  fprintf(fshx, "shape fill\n");

               fprintf(fshx, "save_as\n");
               fprintf(fshx, "fillin \"%s.dra\"\n", shapeName);
               fprintf(fshx, "create symbol\n");
               fprintf(fshx, "fillin \"%s.ssm\"\n", shapeName);
               fprintf(fshx, "exit\n");
               fprintf(fshx, "fillin \"\"\n");
               fprintf(fshx, "exit\n");
               fprintf(fshx, "fillin yes\n");
               fprintf(fshx, "\n");
               fprintf(fshx, "# end of padshape script\n");
            }

            // tname.Format("%s.tech", fname);
            fprintf(fAutoShape, "allegro -s %s.scx %s\n", shapeName, (BATCH_NOGRAPHIC)?"-nographic": "");
            fflush(fAutoShape);
         }
         else 
         {
            double oneMilOutputUnit = Units_Factor(UNIT_MILS, output_units);

            write_shape_symbol_header(fshx, shapeName);
            drawing_unit_size_header(fshx, -PAD_ENLARGE, -PAD_ENLARGE, PAD_ENLARGE, PAD_ENLARGE);

            if (Allegro_Version < 150)
            {
               fprintf(fshx, "add fshape\n");
               fprintf(fshx, "setwindow form.mini\n");
               fprintf(fshx, "FORM mini lock_direction Off\n");
            }
            else
            {
               fprintf(fshx, "shape add\n");
               fprintf(fshx, "setwindow form.mini\n");
               fprintf(fshx, "FORM mini dyns_fill_type Static solid\n");
               fprintf(fshx, "FORM mini dyns_lock_mode Line\n");
            }

            fprintf(fshx, "setwindow pcb\n");

            write_pick(fshx, -oneMilOutputUnit, -oneMilOutputUnit, 0);
            write_pick(fshx, oneMilOutputUnit, -oneMilOutputUnit, 1);
            write_pick(fshx, oneMilOutputUnit, oneMilOutputUnit, 1);
            write_pick(fshx, -oneMilOutputUnit, oneMilOutputUnit, 1);

            fprintf(fshx, "done\n");

            if (Allegro_Version < 150)
               fprintf(fshx, "shape fill\n");

            fprintf(fshx, "save_as\n");
            fprintf(fshx, "fillin \"%s.dra\"\n", shapeName);
            fprintf(fshx, "create symbol\n");
            fprintf(fshx, "fillin \"%s.ssm\"\n", shapeName);
            fprintf(fshx, "exit\n");
            fprintf(fshx, "fillin \"\"\n");
            fprintf(fshx, "exit\n");
            fprintf(fshx, "fillin yes\n");
            fprintf(fshx, "\n");
            fprintf(fshx, "# end of padshape script\n");

            fprintf(fAutoShape, "allegro -s %s.scx %s\n", shapeName, (BATCH_NOGRAPHIC)?"-nographic": "");

            fflush(fAutoShape);

            // no element was found
            fprintf(flog, "No Graphic found for [%s]\n", subblock->getName());
            display_error++;
         }

         shapeAlreadyInAutoShape.SetAt(shapeName, shapeName);
         fflush(fshx);
         fclose(fshx);
         Outline_FreeResults();
      }
   }

   // padstack for can not have a rotation, must adjust for it
   int r = (int)floor(RadToDeg(rotation) / 90 + 0.5);

   if ((r * 90.0) != round(RadToDeg(rotation)))
   {
      fprintf(flog, "Rotation [%d] other than 90 degree in Aperture\n", round(RadToDeg(rotation)));
      display_error++;
   }

   double size_A = sizea;
   double size_B = sizeb;
   X_offset = xoffset;
   Y_offset = yoffset;

   double relief_A = thermosizea;
   double relief_B = thermosizeb;
   double anti_A = size_A + restringA;
   double anti_B = size_B + restringB;

   switch (shape)
   {
   case T_OCTAGON:
   case T_TARGET:
   case T_DONUT:
   case T_ROUND:
   case T_SQUARE:
      size_B = size_A;
      relief_B = relief_A;
      anti_B = anti_A;
      break;
   } // switch shape

   for (int i=0; i<r; i++)
   {
      double t = size_A;
      size_A = size_B;
      size_B = t;

      t = X_offset;
      X_offset = Y_offset;
      Y_offset = t;
   }

   anti_A = size_A + restringA;
   anti_B = size_B + restringB;
   
   // needs to be clarified
   if (thermoshape < 0)
   {
      relief_A = size_A + antipadenlargement;
      relief_B = size_B + antipadenlargement;
      anti_A   = size_A + antipadenlargement;
      anti_B   = size_B + antipadenlargement;
   }

   fprintf(fPadstack, "FORM padedit layers\n");

   if (layer == 3 && SMD)
   {
      fprintf(fPadstack, "FORM padedit parameters\n");
      fprintf(fPadstack, "FORM padedit single YES\n");
      fprintf(fPadstack, "FORM padedit layers\n");

      if (Allegro_Version > 141)
      {
         fprintf(fPadstack, "FORM padedit grid row begin_layer\n");
         fprintf(fPadstack, "FORM padedit grid change begin_layer,1 BOTTOM\n"); 
      }
      else
      {
         fprintf(fPadstack, "FORM padedit grid row 1\n");
         fprintf(fPadstack, "FORM padedit grid change begin_layer_subclass_name 'BOTTOM'\n");
      }
   }
   else
      fprintf(fPadstack, "FORM padedit grid row %d\n", layer);

   if (shape == T_COMPLEX)
   {
      if (Allegro_Version >= 136) // 
      {
         fprintf(fPadstack, "FORM padedit geometry Shape\n");
         fprintf(fPadstack, "FORM padedit shapename %s\n", shapeName);
      }
      else
      {
         fprintf(fPadstack, "FORM padedit regular-pad geometry Shape\n");
         fprintf(fPadstack, "FORM padedit regular-pad shapename %s\n", shapeName);
      }
   }
   else
   {
      char *shap = "Circle";

      switch (shape)
      {
      case T_OCTAGON:
      case T_TARGET:
      case T_DONUT:
      case T_ROUND:
         shap = "Circle";
         size_B = size_A;
         relief_B = relief_A;
         anti_B = anti_A;
         break;
      case T_OBLONG:
         shap = "Oblong";
         break;
      case T_RECTANGLE:
         shap = "Rectangle";
         break;
      case T_SQUARE:
         shap = "Square";
         size_B = size_A;
         relief_B = relief_A;
         anti_B = anti_A;
         break;
      case T_UNDEFINED:             
      case T_BLANK:
         shap = "Null";
         break;
      } // switch shape

      // sizes are not allowed to be 0
      if (fabs(size_B) < ALMOSTZERO)
         size_B = size_A;

      if (fabs(relief_B) < ALMOSTZERO)
         relief_B = relief_A;

      if (fabs(anti_B) < ALMOSTZERO)
         anti_B = anti_A;

      fprintf(fPadstack, "FORM padedit geometry %s\n", shap );
      fprintf(fPadstack, "FORM padedit width %1.*lf\n", output_units_accuracy, size_A);
      fprintf(fPadstack, "FORM padedit height %1.*lf\n", output_units_accuracy, size_B);
      fprintf(fPadstack, "FORM padedit x_offset %1.*lf\n", output_units_accuracy, X_offset);
      fprintf(fPadstack, "FORM padedit y_offset %1.*lf\n", output_units_accuracy, Y_offset);

      if (thermal)
      {
         fprintf(fPadstack, "FORM padedit geometry_th %s\n", shap);
         fprintf(fPadstack, "FORM padedit width_th %1.*lf\n", output_units_accuracy, relief_A);
         fprintf(fPadstack, "FORM padedit height_th %1.*lf\n", output_units_accuracy, relief_B);
         fprintf(fPadstack, "FORM padedit x_offset_th %1.*lf\n", output_units_accuracy, X_offset);
         fprintf(fPadstack, "FORM padedit y_offset_th %1.*lf\n", output_units_accuracy, Y_offset);

         fprintf(fPadstack, "FORM padedit geometry_ant %s\n", shap);
         fprintf(fPadstack, "FORM padedit width_ant %1.*lf\n", output_units_accuracy, anti_A);
         fprintf(fPadstack, "FORM padedit height_ant %1.*lf\n", output_units_accuracy, anti_B);
         fprintf(fPadstack, "FORM padedit x_offset_ant %1.*lf\n", output_units_accuracy, X_offset);
         fprintf(fPadstack, "FORM padedit y_offset_ant %1.*lf\n", output_units_accuracy, Y_offset);
         
         if (write_thermals)
            fprintf(fPadstack, "FORM padedit flash_ant %s\n", do_thermalreliefname(shape, anti_A, size_A));
      }
   }

   // does less than 136 version
   if (Allegro_Version < 136)
      fprintf(fPadstack, "FORM padedit done\n\n");

   return 1;
}

/******************************************************************************
* write_padform
   always also update write_padformthermo
*/
static int write_padform(FILE *fPadstack, FILE *fAutoShape, CString layerName, double sizea, double sizeb, 
      double xOffset, double yOffset, int shape, double rotation, int complexshape)
{
   write_padformthermo(fPadstack, fAutoShape, layerName, sizea, sizeb, xOffset, yOffset, shape, rotation, complexshape, 0, 0, 0, 0, -1, 0, 0, 0);

   return 1;
}

/******************************************************************************
* WritePadstack
*/
static void WritePadstack(FILE *fAutoPad, FILE *fAutoShape, CString padstackName, BlockStruct *padstackGeom, int mirror)
{
   double unitsFactor = Units_Factor(page_units, output_units);

   CString padstackFileName = filePathName;
   padstackFileName += padstackName;
   padstackFileName += ".psr";

   fprintf(fAutoPad, "pad_designer -s %s.psr %s\n", padstackName, (BATCH_NOGRAPHIC) ? "-nographic" : "");

   FILE *fPadstack = fopen(padstackFileName, "wt");

   if (!fPadstack)
   {
      ErrorMessage("Can not open PADFILE", padstackFileName);
      return;
   }

   padstack_script_header(fPadstack, padstackName);

   double drillsize;
   BOOL SMD = FALSE;

   if (is_attvalue(doc, padstackGeom->getAttributesRef(), ATT_SMDSHAPE, 2))
      SMD = TRUE;

   int type = ALLEGRO_WritePADSTACKData(fPadstack, fAutoShape, &padstackGeom->getDataList(), -1, 
         file->getScale() * unitsFactor, mirror, 0, &drillsize, SMD);

   if (SMD)
      fprintf(fPadstack, "FORM padedit single YES\n");
   else
      fprintf(fPadstack, "FORM padedit through_hole YES\n");

   if (type == 3)
   {
      // multiple layers but no drill
      fprintf(flog, "Padstack [%s] has no drill hole -> added.\n", padstackName);
      display_error++;

      fprintf(fPadstack, "FORM padedit ncdrill\n");
      fprintf(fPadstack, "setwindow form.padncdrill\n");
      fprintf(fPadstack, "FORM padncdrill plating Plated\n");  // default

      fprintf(fPadstack, "FORM padncdrill size %1.*lf\n", output_units_accuracy, 1.0);
      fprintf(fPadstack, "FORM padncdrill x_offset %1.*lf\n", output_units_accuracy, 0.0);
      fprintf(fPadstack, "FORM padncdrill y_offset %1.*lf\n", output_units_accuracy,0.0);
      fprintf(fPadstack, "FORM padncdrill figure Square\n");
      fprintf(fPadstack, "FORM padncdrill character %s\n", get_drillsymbol(1.0));
      fprintf(fPadstack, "FORM padncdrill width %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);
      fprintf(fPadstack, "FORM padncdrill height %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);
      fprintf(fPadstack, "FORM padncdrill done\n");
   }
   else if (type == 0)
   {
      fprintf(flog, "NULLPAD  %s\n", padstackName);
      display_error++;
   }

   padstack_script_footer(fPadstack);
   fclose(fPadstack);
}

/******************************************************************************
* do_padstack
*  - Versions < 136
*/
static int do_padstack(FileStruct *file, CString autoPadFileName, CString autoShapeFileName)
{
   double unitsFactor = Units_Factor(page_units, output_units);

   FILE *fAutoPad = fopen(autoPadFileName, "wt");

   if (!fAutoPad)
   {
      ErrorMessage("Can not open PADFILE", autoPadFileName);
      return -1;
   }

   FILE *fAutoShape = fopen(autoShapeFileName, "wt");

   if (!fAutoShape)
   {
      ErrorMessage("Can not open SHAPEFILE", autoShapeFileName);
      return -1;
   }

   CByteArray arr;
   arr.SetSize(doc->getMaxBlockIndex());

   int i; 

   for (i=0; i<doc->getMaxBlockIndex(); i++)
      arr[i] = 0;

   // 0=off, 1=normal, 2=mirror, 3=normal&mirror
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      switch (block->getBlockType())
      {
      case BLOCKTYPE_PADSTACK:
         arr[i] = arr[i] | 1;
         break;

      case BLOCKTYPE_DRILLHOLE:
         arr[i] = arr[i] | 1;
         break;

      case blockTypePcbComponent:
      case blockTypeTestPoint:
         {
            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data->getDataType() != T_INSERT)
                  continue;

               if (data->getInsert()->getInsertType() != insertTypePin)
                  continue;

               if (data->getInsert()->getMirrorFlags())
                  arr[data->getInsert()->getBlockNumber()] = arr[data->getInsert()->getBlockNumber()] | 2;
               else
                  arr[data->getInsert()->getBlockNumber()] = arr[data->getInsert()->getBlockNumber()] | 1;
            }
         }

         break;
      }
   }

   // 0=off, 1=normal, 2=mirror, 3=normal&mirror
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      if (!arr[i])
         continue;

      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      switch (block->getBlockType())
      {
      case BLOCKTYPE_PADSTACK:
         {
            CString padstackName = check_name('p', AllegroNameMgr->GetName(block));

            if (arr[i] & 1)
               WritePadstack(fAutoPad, fAutoShape, padstackName, block, 0);

            if (arr[i] & 2)
            {
               padstackName += "_MIRROR";
               WritePadstack(fAutoPad, fAutoShape, padstackName, block, MIRROR_ALL);
            }
         }
         break;

      case BLOCKTYPE_DRILLHOLE:
         {
            CString padName = check_name('p', AllegroNameMgr->GetName(block));

            CString padFileName = filePathName;
            padFileName += padName;
            padFileName += ".psr";

            // a drill tool is a toolsize. just go inside and 
            double drillsize;

            if (block->getFlags() & BL_TOOL)          
               drillsize = block->getToolSize() * file->getScale() * unitsFactor;
            else
            {
               // loop inside hierachie to find drill inserted.
               fprintf(flog, "DrillHole [%s] is not a tool.\n", block->getName());
               display_error++;
            }

            if (drillsize > 0)
            {
               fprintf(fAutoPad, "pad_designer -s %s.psr %s\n", padName, (BATCH_NOGRAPHIC) ? "-nographic" : "");

               FILE *fPadstack = fopen(padFileName, "wt");

               if (!fPadstack)
               {
                  ErrorMessage("Can not open PADFILE", padFileName);

                  break;
               }

               padstack_script_header(fPadstack, padName);

               // write a padstack, because a drillhole could be placed by itself.
               write_padform(fPadstack, fAutoShape, "BEGIN_LAYER", drillsize, drillsize, 0.0, 0.0,T_ROUND, 0.0, -1);
               write_padform(fPadstack, fAutoShape, "DEFAULT_INTERNAL", drillsize, drillsize, 0.0, 0.0,T_ROUND, 0.0, -1);
               write_padform(fPadstack, fAutoShape, "END_LAYER", drillsize, drillsize, 0.0, 0.0,T_ROUND, 0.0, -1);

               fprintf(fPadstack, "FORM padedit through_hole YES\n");

               fprintf(fPadstack, "FORM padedit ncdrill\n");
               fprintf(fPadstack, "setwindow form.padncdrill\n");

               if ((is_attvalue(doc, block->getAttributesRef(), DRILL_NONPLATED, 1)) || !(block->getToolHolePlated()))
                  fprintf(fPadstack, "FORM padncdrill plating Non-Plated\n");
               else
                  fprintf(fPadstack, "FORM padncdrill plating Plated\n");

               fprintf(fPadstack, "FORM padncdrill size %1.*lf\n", output_units_accuracy,drillsize);
               fprintf(fPadstack, "FORM padncdrill x_offset %1.*lf\n", output_units_accuracy, block->getXoffset() * file->getScale() * unitsFactor);
               fprintf(fPadstack, "FORM padncdrill y_offset %1.*lf\n", output_units_accuracy, block->getXoffset() * file->getScale() * unitsFactor);
               fprintf(fPadstack, "FORM padncdrill figure Square\n");
               fprintf(fPadstack, "FORM padncdrill character %s\n", get_drillsymbol(drillsize));
               fprintf(fPadstack, "FORM padncdrill width %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);
               fprintf(fPadstack, "FORM padncdrill height %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);
               fprintf(fPadstack, "FORM padncdrill done\n");
               fprintf(fPadstack, "setwindow form.padedit\n");

               padstack_script_footer(fPadstack);
               fclose(fPadstack);
            }
            else
            {
               fprintf(flog, "Drill hole [%s] has no drill size\n", block->getName());
               display_error++;
            }
         }
         break;
      }
   }

   arr.RemoveAll();
   fflush(fAutoPad);
   fflush(fAutoShape);
   fclose(fAutoPad); // close AUTOPAD
   fclose(fAutoShape); // close AUTOSHAPE

   return 1;
}

/******************************************************************************
* drillhole_136
*/
static void drillhole_136(FILE *fp, double toolsize, double xoffset, double yoffset, double scale, int plated, BOOL DummyPad)
{
   fprintf(fp, "FORM padedit parameters\n");
   fprintf(fp, "FORM padedit through_hole YES\n");

   if (!plated)
      fprintf(fp, "FORM padedit plating Non-Plated\n");
   else
      fprintf(fp, "FORM padedit plating Plated\n");

   if (Allegro_Version < 152)
      fprintf(fp, "FORM padedit hole_size %1.*lf\n", output_units_accuracy, toolsize * scale);
   else
   {
      fprintf(fp, "FORM padedit hole_type Circle Drill\n");
      fprintf(fp, "FORM padedit hole_size_x %1.*lf\n", output_units_accuracy, toolsize * scale);
      fprintf(fp, "FORM padedit inner_layers_fixed YES\n");
   }
   
   fprintf(fp, "FORM padedit hole_x_offset %1.*lf\n", output_units_accuracy, xoffset * scale);
   fprintf(fp, "FORM padedit hole_y_offset %1.*lf\n", output_units_accuracy, yoffset * scale);

   if (Allegro_Version >= 136) // 
      fprintf(fp, "FORM padedit drill_fig Square\n");
   else
      fprintf(fp, "FORM padedit drill_figure Square\n");

   if (Allegro_Version < 152)
      fprintf(fp, "FORM padedit character %s\n", get_drillsymbol(toolsize * scale));
   else
      fprintf(fp, "FORM padedit characters %s\n", get_drillsymbol(toolsize * scale));
   
   fprintf(fp, "FORM padedit drill_width %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);
   fprintf(fp, "FORM padedit drill_height %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);

   if (DummyPad) // add a small dummy pad because it is illegal in Pad Designer to have a drill only
   {
      fprintf(fp, "FORM padedit layers\n");
      fprintf(fp, "FORM padedit grid row 1\n");
      fprintf(fp, "FORM padedit geometry Circle\n");
      fprintf(fp, "FORM padedit width 1.000\n");
      fprintf(fp, "FORM padedit x_offset 0.000\n");
      fprintf(fp, "FORM padedit y_offset 0.000\n");
   }

   fprintf(fp, "setwindow form.padedit\n");
}

/******************************************************************************
* WritePadstack_136
*/
static void WritePadstack_136(FILE *fAutoPad, FILE *fAutoShape, CString padstackName, BlockStruct *padstackGeom, int mirror)
{
   double unitsFactor = Units_Factor(page_units, output_units);

   CString padstackFileName = filePathName;
   padstackFileName += padstackName;
   padstackFileName += ".psr";

   fprintf(fAutoPad, "pad_designer -s %s.psr %s\n", padstackName, BATCH_NOGRAPHIC ? "-nographic" : "");

   FILE *fPadstack = fopen(padstackFileName, "wt");

   if (!fPadstack)
   {
      ErrorMessage("Can not open PADFILE\n", padstackFileName);

      return;
   }

   padstack_136_script_header(fPadstack, padstackName);

   BOOL SMD = FALSE;

   if (is_attvalue(doc, padstackGeom->getAttributesRef(), ATT_SMDSHAPE, 2))
      SMD = TRUE;

   double drillsize;
   int type = ALLEGRO_WritePADSTACKData_136(fPadstack, fAutoShape, &padstackGeom->getDataList(), -1, file->getScale() * unitsFactor,
                     mirror, 0, &drillsize, SMD, padstackName);

   fprintf(fPadstack, "FORM padedit parameters\n");

   if (SMD)
      fprintf(fPadstack, "FORM padedit single YES\n");
   else
   {
      fprintf(fPadstack, "FORM padedit through_hole YES\n");

      if (!strlen(default_via))
         default_via = padstackName;
   }

   if (type == 3)
   {
      // multiple layers but no drill
      fprintf(flog, "Padstack [%s] has no drill hole -> added.\n", padstackName);
      display_error++;

      drillhole_136(fPadstack, 1.0, 0.0, 0.0, 1.0, 0, FALSE);
   }

   padstack_136_script_footer(fPadstack, padstackFileName, FALSE);

   fclose(fPadstack);
}

/******************************************************************************
* do_padstack_136
*  - Versions 136 and higher
*/
static int do_padstack_136(FileStruct *file, CString autoPadFileName, CString autoShapeFileName)
{
   double unitsFactor = Units_Factor(page_units, output_units);

   FILE *fAutoPad = fopen(autoPadFileName, "wt");

   if (!fAutoPad)
   {
      ErrorMessage("Can not open PADFILE", autoPadFileName);

      return -1;
   }

   FILE *fAutoShape = fopen(autoShapeFileName, "wt");

   if (!fAutoShape)
   {
      ErrorMessage("Can not open SHAPEFILE", autoShapeFileName);

      return -1;
   }

   CByteArray arr;
   arr.SetSize(doc->getMaxBlockIndex());
	int i=0;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
      arr[i] = 0;

   // 0=off, 1=normal, 2=mirror, 3=normal&mirror
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      switch (block->getBlockType())
      {
      case BLOCKTYPE_PADSTACK:
         arr[i] = arr[i] | 1;
         break;

      case BLOCKTYPE_DRILLHOLE:
         arr[i] = arr[i] | 1;
         break;

      case blockTypePcbComponent:
      case blockTypeTestPoint:
         {
            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data->getDataType() != T_INSERT)
                  continue;

               if (data->getInsert()->getInsertType() != insertTypePin)
                  continue;

               if (data->getInsert()->getMirrorFlags())
                  arr[data->getInsert()->getBlockNumber()] = arr[data->getInsert()->getBlockNumber()] | 2;
               else
                  arr[data->getInsert()->getBlockNumber()] = arr[data->getInsert()->getBlockNumber()] | 1;
            }
         }

         break;
      }
   }

   // 0=off, 1=normal, 2=mirror, 3=normal&mirror
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      if (!arr[i])
         continue;

      BlockStruct *block = doc->getBlockAt(i);

      switch (block->getBlockType())
      {
      case BLOCKTYPE_PADSTACK:
         {
            CString padstackName = check_name('p', AllegroNameMgr->GetName(block));

            if (arr[i] & 1)
               WritePadstack_136(fAutoPad, fAutoShape, padstackName, block, 0);

            if (arr[i] & 2)
            {
               padstackName += "_MIRROR";
               WritePadstack_136(fAutoPad, fAutoShape, padstackName, block, MIRROR_ALL);
            }
         }

         break;
      case BLOCKTYPE_DRILLHOLE:
         {
            CString padstackName = check_name('p', AllegroNameMgr->GetName(block));

            CString padstackFileName = filePathName;
            padstackFileName += padstackName;
            padstackFileName += ".psr";

            double drillsize = 0;

            if (block->getFlags() & BL_TOOL)          
               drillsize = block->getToolSize() * file->getScale() * unitsFactor;
            else
            {
               // loop inside hierachie to find drill inserted.
               fprintf(flog, "DrillHole [%s] is not a tool.\n", block->getName());
               display_error++;
            }

            if (drillsize > 0)
            {
               fprintf(fAutoPad, "pad_designer -s %s.psr %s\n", padstackName, BATCH_NOGRAPHIC ? "-nographic" : "");
   
               FILE *fPadstack = fopen(padstackFileName, "wt");

               if (!fPadstack)
               {
                  ErrorMessage("Can not open PADFILE\n", padstackFileName);
                  break;
               }

               padstack_136_script_header(fPadstack, padstackName);
            
               fprintf(fPadstack, "FORM padedit parameters\n");
               fprintf(fPadstack, "FORM padedit through_hole YES\n");
   
               BOOL Plated = TRUE;

               if ((is_attvalue(doc, block->getAttributesRef(), DRILL_NONPLATED, 1)) || !(block->getToolHolePlated()))
                  Plated = FALSE;

               drillhole_136(fPadstack, drillsize, block->getXoffset() * file->getScale() * unitsFactor, block->getYoffset() * file->getScale() * unitsFactor, 
                     1.0, Plated, TRUE);  // drill size is already scaled

               padstack_136_script_footer(fPadstack, padstackFileName, TRUE);

               fclose(fPadstack);
            }
            else
            {
               fprintf(flog, "Drill hole [%s] has no drill size\n", block->getName());
               display_error++;
            }
         }

         break;
      }
   }

   arr.RemoveAll();

   fflush(fAutoPad);
   fflush(fAutoShape);
   fclose(fAutoPad);
   fclose(fAutoShape);

   return 1;
}

//--------------------------------------------------------------
static int complist_device_written(const char *geom, const char *device)
{
   int   i;

   for (i=0;i<complistcnt;i++)
   {
      if (complistarray[i]->geomname.Compare(geom) == 0 &&
          complistarray[i]->devicename.Compare(device) == 0)
      {
         complistarray[i]->devicefile_written = TRUE;
      }
   }

   return 1;
}

/******************************************************************************
* do_devices
*/
static int do_devices(FileStruct *file)
{
   double   unitsFactor = Units_Factor(page_units, output_units);

   TypeStruct *typ;
   POSITION typPos = file->getTypeList().GetHeadPosition();

   while (typPos != NULL)
   {                                         
      typ = file->getTypeList().GetNext(typPos);

      if (typ->getBlockNumber() < 0)  continue;

      FILE  *dfp;
      CString  devicefile;
      CString  devicename = check_name('e',typ->getName());            

      devicefile = filePathName;
      devicefile +=devicename;
      devicefile +=".txt";

      // devicefile.Format("%s.txt", devicename);
      BlockStruct *block = doc->Find_Block_by_Num(typ->getBlockNumber());

      if (block == NULL)   continue;

      CString  geomname = check_name('s', AllegroNameMgr->GetName(block));

      if ((dfp = fopen(devicefile,"wt")) == NULL)
      {
         ErrorMessage("Can not open DEVICEFILE\n");
         break;
      }

      fprintf(dfp,"(DEVICE FILE: %s)\n",devicename);
      fprintf(dfp,"\n");
      fprintf(dfp,"PACKAGE %s\n",geomname);
      fprintf(dfp,"CLASS IC\n");
      int pincnt = ALLEGRO_WritePackagePinData(NULL, block, 0, 0, 0, 0, file->getScale() * unitsFactor, FALSE);

      fprintf(dfp,"PINCOUNT %d\n",pincnt);
      ALLEGRO_WriteDeviceData(dfp, block, devicename);

      fprintf(dfp,"\nEND\n");
      fclose(dfp);
   
      complist_device_written(AllegroNameMgr->GetName(block), typ->getName());
   }

   return 1;
} // no typelink found

/******************************************************************************
* do_drawings
*/
static int do_drawings(FileStruct *file, FILE *wfp)
{
   FILE     *pfp;
   double   unitsFactor = Units_Factor(page_units, output_units);

   // this is a drawing file block
   BlockStruct *block = file->getBlock();

   CString  geomname;
   CString  geomfile;         

   geomname = check_name('s', AllegroNameMgr->GetName(block));

   geomfile = filePathName;
   geomfile +=geomname;
   geomfile +=".scr";
   // geomfile.Format("%s.scr", geomname);

   fprintf(wfp,"allegro -s %s.scr %s\n", geomname, (BATCH_NOGRAPHIC)?"-nographic": "");

   if ((pfp = fopen(geomfile,"wt")) == NULL)
   {
      ErrorMessage("Can not open GEOMFILE\n");
         return 1;
   }

   doc->validateBlockExtents(block);

   double xmin = block->getXmin();
   double xmax = block->getXmax();
   double ymin = block->getYmin();
   double ymax = block->getYmax();

   if (xmin > xmax)
   {
      xmin = xmax = ymin = ymax = 0;
   }

   format_script_header(pfp, geomname, 
               (xmin * unitsFactor) - DRAWING_ENLARGE, (ymin * unitsFactor) - DRAWING_ENLARGE, 
               (xmax * unitsFactor) + DRAWING_ENLARGE, (ymax * unitsFactor) + DRAWING_ENLARGE);

   // no mirror and rotation, because packages are defined absolute
   // this is only graphic
   ALLEGRO_WriteData(pfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0,
         file->getScale() * unitsFactor, -1, CLASS_DRAWING_FORMAT, HIERACHIEMARKER);  

   format_script_footer(pfp, geomname);
   fclose(pfp);

   return 1;
}

static int writeLabelEntry(FILE* file,Attrib& attributeInstance,const CString& subClass,bool refDesType)
{
   int instanceCount = 0;

   if (! subClass.IsEmpty())
   {
      double   unitsFactor = Units_Factor(page_units, output_units);

      double xref   = attributeInstance.getX() * unitsFactor;
      double yref   = attributeInstance.getY() * unitsFactor;
      double rotref = radiansToDegrees(attributeInstance.getRotationRadians());
      double len;
      CString textJustification;

      double refDesHeight = attributeInstance.getHeight() * unitsFactor;

      if (refDesHeight <= 0.)
      {
         refDesHeight = REFDES_HEIGHT * mils_2_units;
      }

      switch (attributeInstance.getHorizontalPosition())
      {
      default:
      case horizontalPositionLeft:    textJustification = "Left";    break;
      case horizontalPositionCenter:  textJustification = "Center";  break;
      case horizontalPositionRight:   textJustification = "Right";   break;
      }

      switch (attributeInstance.getVerticalPosition())
      {
      default:
      case verticalPositionBaseline:
      case verticalPositionBottom:    len = 0.;              break;
      case verticalPositionCenter:    len = refDesHeight/2;  break;
      case verticalPositionTop:       len = refDesHeight;    break;
      }

      xref += (len * cos(attributeInstance.getRotationRadians() - (Pi / 2)));
      yref += (len * sin(attributeInstance.getRotationRadians() - (Pi / 2)));

      fprintf(file, "label %s\n",(refDesType ? "refdes" : "device")); 
      fprintf(file, "setwindow form.mini\n");
      fprintf(file, "FORM mini text_block %d\n", get_textindex(refDesHeight));
      fprintf(file, "FORM mini angle %1.3lf\n", 0.0);
      fprintf(file, "FORM mini mirror NO\n");
      fprintf(file, "FORM mini text_justification %s\n", textJustification); 
      fprintf(file, "FORM mini class %s\n",(refDesType ? "REF DES" : "DEVICE TYPE")); 

      fprintf(file, "FORM mini subclass %s\n",subClass);

      if (rotref != 0.)
         fprintf(file, "FORM mini angle %1.3lf\n", rotref);

      fprintf(file, "setwindow pcb\n");
      write_pick(file, xref, yref, 0);

      fprintf(file, "text \"%s\"\n",(refDesType ? "REF" : "DEV")); 
      fprintf(file, "done\n"); 

      instanceCount++;
   }

   return instanceCount;
}

static int writeLabelEntry(FILE* file,Attrib& attribute,bool refDesType)
{
   int instanceCount = 0;

   double   unitsFactor = Units_Factor(page_units, output_units);

   AttribIterator attribIterator(&attribute);
#if CamCadMajorMinorVersion > 406  //  > 4.6
   Attrib attributeInstance(attribute.getCamCadData());
#else
   Attrib attributeInstance;
#endif

   while (attribIterator.getNext(attributeInstance))
   {
      if (!attributeInstance.isVisible() || attributeInstance.getValueType() != VT_STRING)
      {
         continue;
      }

      int layerIndex = attributeInstance.getLayerIndex();
      const char* allegroLayer = Layer_ALLEGRO(layerIndex);

      if (allegroLayer == NULL) 
      {
         continue;
      }

      int classType = (refDesType ? CLASS_REF_DES : CLASS_DEVICE_TYPE);
      int allegroLayerIndex = get_class_subclass(allegroLayer,classType);

      if (allegroLayerIndex >= 0)
      {
         CString subClass = maplay[allegroLayerIndex].subclass;

         instanceCount += writeLabelEntry(file,attribute,subClass,refDesType);
      }
      else
      {
         if (!in_maperror(allegroLayer,lclasses[classType]))
         {
            fprintf(flog, "Layer [%s] Class [%s] not found -> (update allegro.out)\n", 
               allegroLayer, lclasses[classType]);
         }
      }
   }

   return instanceCount;
}

static int writeRefdesLabelEntry(FILE* file,CAttributes*& attributes)
{
   int instanceCount = 0;
   Attrib* attribute = is_attvalue(doc,attributes, ATT_REFNAME, 1);

   if (attribute != NULL)
   {
      instanceCount = writeLabelEntry(file,*attribute,true);
   }

   return instanceCount;
}

static int writeDeviceLabelEntry(FILE* file,CAttributes*& attributes)
{
   int instanceCount = 0;
   Attrib* attribute = is_attvalue(doc,attributes,deviceTypeAttributeName,1);

   if (attribute != NULL)
   {
      instanceCount = writeLabelEntry(file,*attribute,false);
   }

   return instanceCount;
}

/******************************************************************************
* do_geometries
*/
static int do_geometries(FileStruct *file, CString autoSymFileName)
{
   double   unitsFactor = Units_Factor(page_units, output_units);
   double   accuracy = get_accuracy(doc);

   FILE *fAutoSym = fopen(autoSymFileName, "wt");

   if (!fAutoSym)
   {
      ErrorMessage("Can not open SYMFILE", autoSymFileName);

      return -1;
   }

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getFlags() & BL_WIDTH)         continue;
      if (block->getFlags() & BL_APERTURE)      continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
      if (block->getFlags() & BL_FILE)          continue;

      switch (block->getBlockType())
      {
      case BLOCKTYPE_PANEL:
      case BLOCKTYPE_LIBRARY:
      case BLOCKTYPE_TOOLGRAPHIC:
      case BLOCKTYPE_SYMBOL:
      case BLOCKTYPE_GATEPORT:
      case BLOCKTYPE_SHEET:
      case BLOCKTYPE_PADSTACK:
      case BLOCKTYPE_PADSHAPE:
      case BLOCKTYPE_PCB:
      case BLOCKTYPE_CENTROID:
         break;

      case BLOCKTYPE_DRILLHOLE:
         {
            CString geomName = check_name('s', AllegroNameMgr->GetName(block));

            CString geomFileName = filePathName;
            geomFileName += geomName;
            geomFileName += ".scr";

            fprintf(fAutoSym, "allegro -s %s.scr %s\n", geomName,( BATCH_NOGRAPHIC) ? "-nographic" : "");

            FILE *fGeom = fopen(geomFileName, "wt");

            if (!fGeom)
            {
               ErrorMessage("Can not open GEOMFILE", geomFileName);
               break;
            }

            doc->validateBlockExtents(block);

            double xmin = block->getXmin();
            double xmax = block->getXmax();
            double ymin = block->getYmin();
            double ymax = block->getYmax();

            if (xmin > xmax)
               xmin = xmax = ymin = ymax = 0;

            if (xmin > 0)
               xmin = 0;

            if (xmax < 0)
               xmax = 0;

            if (ymin > 0)
               ymin = 0;

            if (ymax < 0)
               ymax = 0;

            mechanical_script_header(fGeom, geomName, 
                  (xmin * unitsFactor)-DRAWING_ENLARGE, (ymin * unitsFactor)-DRAWING_ENLARGE, 
                  (xmax * unitsFactor)+DRAWING_ENLARGE, (ymax * unitsFactor)+DRAWING_ENLARGE);

            if (block->getFlags() & BL_TOOL)          
            {
               //drillsize = block->getToolSize() * file->getScale() * unitsFactor;
               fprintf(fGeom, "add pin\n");
               fprintf(fGeom, "setwindow form.mini\n");
               fprintf(fGeom, "FORM mini rotate_pin 0.0\n");

               fprintf(fGeom, "setwindow form.mini\n");

               if (Allegro_Version >= 140) // 
                  fprintf(fGeom, "FORM mini pad_name %s\n",check_name('p', AllegroNameMgr->GetName(block)));
               else
                  fprintf(fGeom, "FORM mini padname %s\n",check_name('p', AllegroNameMgr->GetName(block)));

               fprintf(fGeom, "setwindow pcb\n");
               fprintf(fGeom, "pick 0.0 0.0\n");
               fprintf(fGeom, "done\n");
            }
            else
            {
               ALLEGRO_WriteMechanicalDrillData(fGeom, &block->getDataList(), 0, 0, 0, 0, file->getScale() * unitsFactor, TRUE);
               // no mirror and rotation, because packages are defined absolute
               // this is only graphic
               ALLEGRO_WriteData(fGeom, &block->getDataList(), 0, 0, 0, 0, file->getScale() * unitsFactor, -1, CLASS_PACKAGE_GEOMETRY, 0);
            }

            mechanical_script_footer(fGeom, geomName);

            fclose(fGeom);
         }

         break;
      case BLOCKTYPE_UNKNOWN:
      case BLOCKTYPE_DRAWING:
         {
            CString geomName = check_name('s', AllegroNameMgr->GetName(block));

            CString geomFileName = filePathName;
            geomFileName += geomName;
            geomFileName += ".scr";

            fprintf(fAutoSym, "allegro -s %s.scr %s\n", geomName, (BATCH_NOGRAPHIC) ? "-nographic" : "");

            FILE *fGeom = fopen(geomFileName, "wt");

            if (!fGeom)
            {
               ErrorMessage("Can not open GEOMFILE", geomFileName);

               break;
            }

            doc->validateBlockExtents(block);

            double xmin = block->getXmin(), xmax = block->getXmax(), ymin = block->getYmin(), ymax = block->getYmax();

            if (xmin > xmax)
               xmin = xmax = ymin = ymax = 0;

            format_script_header(fGeom, geomName, 
                  (xmin * unitsFactor)-DRAWING_ENLARGE, (ymin * unitsFactor)-DRAWING_ENLARGE, 
                  (xmax * unitsFactor)+DRAWING_ENLARGE, (ymax * unitsFactor)+DRAWING_ENLARGE);

            int pincnt = ALLEGRO_WritePackagePinData(fGeom, block, 0, 0, 0, 0, file->getScale() * unitsFactor, TRUE);
   
            // no mirror and rotation, because packages are defined absolute
            // this is only graphic
            ALLEGRO_WriteData(fGeom, &block->getDataList(), 0, 0, 0, 0, file->getScale() * unitsFactor, -1, CLASS_DRAWING_FORMAT, 0);

            format_script_footer(fGeom, geomName);

            fclose(fGeom);
         }

         break;
      case BLOCKTYPE_GENERICCOMPONENT:
      case BLOCKTYPE_MECHCOMPONENT: 
         {
            CString geomName = check_name('s', AllegroNameMgr->GetName(block));

            CString geomFileName = filePathName;
            geomFileName += geomName;
            geomFileName += ".scr";

            fprintf(fAutoSym, "allegro -s %s.scr %s\n", geomName, (BATCH_NOGRAPHIC) ? "-nographic" : "");

            FILE *fGeom = fopen(geomFileName, "wt");

            if (!fGeom)
            {
               ErrorMessage("Can not open GEOMFILE", geomFileName);

               break;
            }

            doc->validateBlockExtents(block);

            double xmin = block->getXmin(), xmax = block->getXmax(), ymin = block->getYmin(), ymax = block->getYmax();

            if (xmin > xmax)
               xmin = xmax = ymin = ymax = 0;

            if (xmin > 0)
               xmin = 0;

            if (xmax < 0)
               xmax = 0;

            if (ymin > 0)
               ymin = 0;

            if (ymax < 0)
               ymax = 0;

            mechanical_script_header(fGeom, geomName, 
                  (xmin * unitsFactor)-DRAWING_ENLARGE, (ymin * unitsFactor)-DRAWING_ENLARGE, 
                  (xmax * unitsFactor)+DRAWING_ENLARGE, (ymax * unitsFactor)+DRAWING_ENLARGE);

            int pincnt = ALLEGRO_WritePackagePinData(fGeom, block, 0, 0, 0, 0, file->getScale() * unitsFactor, TRUE);

            // no mirror and rotation, because packages are defined absolute
            // this is only graphic
            ALLEGRO_WriteData(fGeom, &block->getDataList(), 0, 0, 0, 0, file->getScale() * unitsFactor, -1, CLASS_PACKAGE_GEOMETRY, 0);

            mechanical_script_footer(fGeom, geomName);

            fclose(fGeom);
         }

         break;
      // package
      case blockTypePcbComponent: 
      case blockTypeTestPoint:
         {
            CString geomName = check_name('s', AllegroNameMgr->GetName(block));

            CString geomFileName = filePathName;
            geomFileName += geomName;
            geomFileName += ".scr";

            CString deviceFileName = filePathName;
            deviceFileName += geomName;
            deviceFileName += ".txt";

            fprintf(fAutoSym, "allegro -s %s.scr %s\n", geomName, (BATCH_NOGRAPHIC)?"-nographic": "");

            FILE *fGeom = fopen(geomFileName, "wt");

            if (!fGeom)
            {
               ErrorMessage("Can not open GEOMFILE", geomFileName);

               break;
            }

            FILE *fDevice = fopen(deviceFileName, "wt");

            if (!fDevice)
            {
               ErrorMessage("Can not open DEVICEFILE", deviceFileName);

               break;
            }

            fprintf(fDevice, "(DEVICE FILE: %s)\n", geomName);
            fprintf(fDevice, "\n");
            fprintf(fDevice, "PACKAGE %s\n", geomName);
            fprintf(fDevice, "CLASS IC\n");

            complist_device_written(block->getName(), AllegroNameMgr->GetName(block));

            doc->validateBlockExtents(block);

            // To indicate that it is going to write Package Symbol
            isWritingPackageSymbol = TRUE;

            double xmin = block->getXmin(), xmax = block->getXmax(), ymin = block->getYmin(), ymax = block->getYmax();

            if (xmin > xmax)
               xmin = xmax = ymin = ymax = 0;

            package_script_header(fGeom, geomName, 
                  (xmin * unitsFactor)-DRAWING_ENLARGE, (ymin * unitsFactor)-DRAWING_ENLARGE, 
                  (xmax * unitsFactor)+DRAWING_ENLARGE, (ymax * unitsFactor)+DRAWING_ENLARGE);

            int pincnt = ALLEGRO_WritePackagePinData(fGeom, block, 0, 0, 0, 0, file->getScale() * unitsFactor, TRUE);

            fprintf(fDevice, "PINCOUNT %d\n", pincnt);

            ALLEGRO_WriteDeviceData(fDevice, block, geomName);

            // no mirror and rotation, because packages are defined absolute
            // this is only graphic
            ALLEGRO_WriteData(fGeom, &block->getDataList(), 0, 0, 0, 0, file->getScale() * unitsFactor, -1, CLASS_PACKAGE_GEOMETRY, 0);

            Outline_Start(doc);
         
            BOOL CompFound = FALSE;

            if (Allegro_OutlineSHAPEData(&block->getDataList(), 0, 0, 0, 0, file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_COMPONENT_BOUNDARY))
               CompFound = TRUE;

            if (Allegro_OutlineSHAPEData(&block->getDataList(), 0, 0, 0, 0, file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_COMPOUTLINE))
               CompFound = TRUE;

            if (!CompFound)
            {
               fprintf(flog,"Did not find a Component Outline for [%s].\n", block->getName());
               display_error++;
            }
            else
            {
               CompFound = FALSE;

               int lindex = get_class_subclass("COMPONENT_PLACEMENT_OUTLINE", CLASS_PACKAGE_GEOMETRY);

               if (lindex < 0)
               {
                  // here need to update maplay class if a new class was defined, not otherwise.
                  if (!in_maperror("COMPONENT_PLACEMENT_OUTLINE", lclasses[CLASS_PACKAGE_GEOMETRY]))
                  {
                     fprintf(flog, "Layer [%s] Class [%s] not found -> (update allegro.out)\n",
                        "COMPONENT_PLACEMENT_OUTLINE", lclasses[CLASS_PACKAGE_GEOMETRY]);
                  }
               }
               else
               {
                  int returnCode;

                  if (!write_easyoutline_properties(fGeom, Outline_GetOutline(&returnCode, accuracy), lindex, block->getAttributesRef()))
                  {
                     fprintf(flog, "Error in easy outline for [%s]\n", block->getName());
                     display_error++;
                  }
               }
            }

            Outline_FreeResults();

            // -- refDes -- refDes -- refDes -- refDes -- refDes -- refDes -- refDes -- refDes -- refDes -- 
            int refDesInstanceCount = writeRefdesLabelEntry(fGeom,block->getAttributesRef());

            if (refDesInstanceCount == 0)
            {
#if CamCadMajorMinorVersion > 406  //  > 4.6
               Attrib attribute(doc->getCamCadData());
#else
               Attrib attribute;
#endif
               // Do not set the height like this! 
			   // This converts the height to output units
			   // which is then passed as if it were page units
			   // which is then converted again to output units
			   // attribute.setHeight(REFDES_HEIGHT * mils_2_units);
			   // Instead set height to 0 and allow the called function
			   // to set the height.
			   attribute.setHeight(0.0);

               if ((RefSubClass & SubClassAssembly) != 0)
               {
                  writeLabelEntry(fGeom,attribute,"ASSEMBLY_TOP",true);
               }

               if ((RefSubClass & SubClassSilkscreen) != 0)
               {
                  writeLabelEntry(fGeom,attribute,"SILKSCREEN_TOP",true);
               }

               if ((RefSubClass & SubClassDisplay) != 0)
               {
                  writeLabelEntry(fGeom,attribute,"DISPLAY_TOP",true);
               }
            }

            // -- device -- device -- device -- device -- device -- device -- device -- device -- device -- device --
            int deviceInstanceCount = writeDeviceLabelEntry(fGeom,block->getAttributesRef());

            if (deviceInstanceCount == 0)
            {
#if CamCadMajorMinorVersion > 406  //  > 4.6
               Attrib attribute(doc->getCamCadData());
#else
               Attrib attribute;
#endif
               // Do not set the height like this! 
			   // This converts the height to output units
			   // which is then passed as if it were page units
			   // which is then converted again to output units
			   // attribute.setHeight(REFDES_HEIGHT * mils_2_units);
			   // Instead set height to 0 and allow the called function
			   // to set the height.
			   attribute.setHeight(0.0);

               if ((DevSubClass & SubClassAssembly) != 0)
               {
                  writeLabelEntry(fGeom,attribute,"ASSEMBLY_TOP",false);
               }

               if ((DevSubClass & SubClassSilkscreen) != 0)
               {
                  writeLabelEntry(fGeom,attribute,"SILKSCREEN_TOP",false);
               }

               if ((DevSubClass & SubClassDisplay) != 0)
               {
                  writeLabelEntry(fGeom,attribute,"DISPLAY_TOP",false);
               }
            }
            
            // --     --     --     --     --     --     --     --     --     --     --     --     --     --     --     --

            package_script_footer(fGeom, geomName);
            fclose(fGeom);

            fprintf(fDevice, "\nEND\n");
            fclose(fDevice);

            // To indicate that it finished writing Package Symbol
            isWritingPackageSymbol = FALSE;
         }

         break;

         default:
            // Stuff ignored, e.g. Access Markers and Test Probes come through here
            break;
      }
   }

   fflush(fAutoSym);
   fclose(fAutoSym);

   return 1;
}

/******************************************************************************
* do_brdfile
*/
static int do_brdfile(FileStruct *file, const char *brdfile, const char *fillfile, const char *pourfile, 
      const char *fname, const char *netfile,   double xmin, double ymin, double xmax, double ymax)
{
   FILE     *wfp, *ffp, *pfp;
   double   unitsFactor = Units_Factor(page_units, output_units);
   double   accuracy = get_accuracy(doc);

   if ((wfp = fopen(brdfile,"wt")) == NULL)
   {
      ErrorMessage("Can not open BOARDFILE\n");

      return -1;
   }     

   if ((ffp = fopen(fillfile,"wt")) == NULL)
   {
      ErrorMessage("Can not open FILLFILE\n");

      return -1;
   }     

   if (writeBoundary)
   {
      if ((pfp = fopen(pourfile,"wt")) == NULL)
      {
         ErrorMessage("Can not open POURFILE\n");

         return -1;
      }     
   }
   else 
      pfp = NULL;

   xmin *= unitsFactor;
   ymin *= unitsFactor;
   xmax *= unitsFactor;
   ymax *= unitsFactor;

   // Make sure the extents includes the origin.
   if (xmin > 0)
      xmin = 0;

   if (ymin > 0)
      ymin = 0;

   if (xmax < 0)
      xmax = 0;

   if (ymax < 0)
      ymax = 0;

   // add something to extensions so that the drawing extens has some space.
   xmin -= BOARD_ENLARGE;
   ymin -= BOARD_ENLARGE;
   xmax += BOARD_ENLARGE;
   ymax += BOARD_ENLARGE;

   brd_script_header(wfp, fname);
   fill_script_header(ffp, fname);

   if (writeBoundary)
      fill_script_header(pfp, fname);

   fprintf(wfp,"open\n");
   fprintf(wfp,"fillin \"unnamed.brd\"\n");
   fprintf(wfp,"save_as\n");
   fprintf(wfp,"fillin \"%s\"\n",fname);
   fprintf(wfp,"setwindow pcb\n");
   fprintf(wfp,"status\n");
   fprintf(wfp,"setwindow form.status\n");

   if (Allegro_Version < 150)
   {
      if (DRC_ON)
          fprintf(wfp,"FORM status drc_on yes\n");
       else
          fprintf(wfp,"FORM status drc_off yes\n");
   }
   else 
   {
      if (DRC_ON)
          fprintf(wfp,"FORM status drc_on YES\n");
       else
          fprintf(wfp,"FORM status drc_on NO\n");
   }

   if (Allegro_Version >= 136) // 
      fprintf(wfp,"FORM status line_lock\n");

   fprintf(wfp,"FORM status lock_direction Off\n");
   fprintf(wfp,"FORM status drawing_size\n");

   fprintf(wfp,"setwindow pcb\n");
   //should add ETCH subclasses here.
   add_newlayers(wfp);  // after unnamed board is loaded.
   fprintf(wfp,"setwindow pcb\n");

   fprintf(wfp,"setwindow form.drawing\n");
   fprintf(wfp,"FORM drawing type Drawing\n");

   drawing_unit_size_header(wfp, xmin, ymin, xmax, ymax);

   fprintf(wfp,"setwindow form.status\n");
   fprintf(wfp,"FORM status done\n");
   fprintf(wfp,"setwindow pcb\n");

   // vias and generic and mechanical components
   ALLEGRO_WriteVias_GenericData(wfp, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(),  file->isMirrored(), file->getScale() * unitsFactor);

   ALLEGRO_WriteBrdGraphicData(wfp, ffp, pfp, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor, 
         file->getRotation(),  file->isMirrored(), file->getScale() * unitsFactor, -1, CLASS_BOARD_GEOMETRY);   

   // get routing keepout - must be one and only one
   Outline_Start(doc);

   int found = Allegro_OutlineSHAPEData(&(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(),  file->isMirrored(),  
         file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_ROUTKEEPIN);

   if (!found)
   {
      fprintf(flog,"Did not find a RouteKeepin.\n");
      display_error++;
   }
   else
   {
      int returnCode;

      if (!write_easykeepinrouter(wfp, Outline_GetOutline(&returnCode, accuracy)))
      {
         fprintf(flog,"Error creating Route Keepin.\n");
         display_error++;
      }
   }

   Outline_FreeResults();

   // get  place keepin one and only one
   Outline_Start(doc);

   found = Allegro_OutlineSHAPEData(&(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(),  file->isMirrored(),  
         file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_PLACEKEEPIN);

   if (!found)
   {
      fprintf(flog,"Did not find a Package Keepin.\n");
      display_error++;
   }
   else
   {
      int returnCode;

      if (!write_easykeepinpackage(wfp, Outline_GetOutline(&returnCode, accuracy)))
      {
         fprintf(flog,"Error creating Package Keepin.\n");
         display_error++;
      }
   }

   Outline_FreeResults();

   fprintf(wfp,"setwindow pcb\n");
   fprintf(wfp,"define grid\n");
   fprintf(wfp,"setwindow form.grid\n");

   fprintf(wfp,"FORM grid all_etch all_etch_x_grids %1.*lf\n", output_units_accuracy, mils_2_units * 25.0);
   fprintf(wfp,"FORM grid all_etch all_etch_y_grids %1.*lf\n", output_units_accuracy, mils_2_units * 25.0);
   fprintf(wfp,"FORM grid done\n");
   fprintf(wfp,"setwindow pcb\n");
   fprintf(wfp,"cns\n");
   fprintf(wfp,"cns space values\n");
   fprintf(wfp,"setwindow form.cns_space_values\n");

   fprintf(flog, "Clearance Values are not implemented\n");

   display_error++;

   fprintf(wfp,"FORM cns_space_values pin_to_pin %1.*lf\n", output_units_accuracy, mils_2_units * 13.0);
   fprintf(wfp,"FORM cns_space_values line_to_pin %1.*lf\n", output_units_accuracy, mils_2_units * 13.0);
   fprintf(wfp,"FORM cns_space_values line_to_line %1.*lf\n", output_units_accuracy, mils_2_units * 13.0);
   fprintf(wfp,"FORM cns_space_values done\n");
   fprintf(wfp,"setwindow form.cns\nFORM cns done\nsetwindow pcb\n");

   if (Allegro_Version >= 142)
      fprintf(wfp,"netin\n");
   else
      fprintf(wfp,"netin param\n");

   fprintf(wfp,"setwindow form.niparams\n");

   if (Allegro_Version >= 142)
      fprintf(wfp,"FORM niparams other\n");
   else
      fprintf(wfp,"FORM niparams netin_netlist YES\n");

   // do not make it absolute, just the name
   char fn[_MAX_FNAME], dr[_MAX_DRIVE], di[_MAX_DIR], ex[_MAX_EXT];
   _splitpath( netfile, dr, di, fn, ex );

   fprintf(wfp,"FORM niparams filename %s%s\n", fn, ex);
   // fprintf(wfp,"FORM niparams save_drawing YES\n");

   // added by dean 09.10.2001
   if (Allegro_Version >= 136 && Allegro_Version < 142) // 
      fprintf(wfp,"FORM niparams options\n");

   fprintf(wfp,"FORM niparams syntax_check_only NO\n");
   fprintf(wfp,"FORM niparams supercede YES\n");

   if (Allegro_Version >= 140) // 
   {
      fprintf(wfp,"FORM niparams import\n");
   }
   else
   {
      fprintf(wfp,"FORM niparams done\n");
      fprintf(wfp,"setwindow pcb\n");
      fprintf(wfp,"netin execute\n");
   }

   fprintf(wfp,"setwindow pcb\n");
   fprintf(wfp,"plctxt in\n");
   fprintf(wfp,"setwindow form.plctxt\n");
   fprintf(wfp,"FORM plctxt execute\n");
   fprintf(wfp,"FORM plctxt cancel\n");
   fprintf(wfp,"setwindow pcb\n");

/*                     
   fprintf(wfp,"cns physical values\n");
   fprintf(wfp,"setwindow form.cns_data_values\n");
   fprintf(wfp,"FORM cns_data_values via_name PD_0\n");
   fprintf(wfp,"FORM cns_data_values add_via\n");
   fprintf(wfp,"FORM cns_data_values current_via_list VIA\n")\n");
   fprintf(wfp,"FORM cns_data_values done\n");
   fprintf(wfp,"setwindow form.cns\n");
   fprintf(wfp,"FORM cns done\n");
*/

/* do not close brd file, because it will kill the window and
   no one could see the result.

   fprintf(wfp,"exit\n");
   fprintf(wfp,"fillin \"\"\n");
   fprintf(wfp,"fillin \"yes\"\n");
*/

   brd_script_footer(wfp);

   fclose(wfp);   // close BRDFILE
   fclose(ffp);   // close FILLFILE

   if (writeBoundary)
      fclose(pfp);   // close POURFILE

   return 1;
}

/******************************************************************************
* do_netlistfile
*/
static int do_netlistfile(FileStruct *file, const char *NETFILE, const char *fname)
{
   FILE  *fp;
   FILE  *plc; // place by text file

   double   unitsFactor = Units_Factor(page_units, output_units);

   if ((fp = fopen(NETFILE,"wt")) == NULL)
   {
      ErrorMessage("Can not open NETLIST\n");

      return -1;
   }

   CString  placetxt;
   placetxt = filePathName;
   placetxt += "place_txt.txt";

   if ((plc = fopen(placetxt, "wt")) == NULL)
   {
      ErrorMessage("Can not open PLACETEXT\n");

      return -1;
   }

   CString  tmp;
   tmp = output_units_string;
   tmp.MakeUpper();
   fprintf(plc,"UUNITS = %s\n", tmp);  // must be all upper.

   fprintf(fp,"(NETLIST)\n");
   fprintf(fp,"(For drawing: %s)\n", fname);
   fprintf(fp,"$PACKAGES\n");

   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   double   rotation =  file->getRotation();
   int      mirror =  file->isMirrored();
   double   scale = file->getScale() * unitsFactor;
   double   insert_x = file->getInsertX() * unitsFactor;
   double   insert_y = file->getInsertY() * unitsFactor;

   RotMat2(&m, rotation);

   CMapStringToString compToBeRemove;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);
      
      if (np->getDataType() != T_INSERT)  
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      point2.x = np->getInsert()->getOriginX() * scale;

      if (mirror)
         point2.x = -point2.x;

      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
      double block_rot = rotation + np->getInsert()->getAngle();
      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      if (np->getInsert()->getInsertType() == insertTypePcbComponent ||
         np->getInsert()->getInsertType() == insertTypeTestPoint)
      {
         if (block->getBlockType() == BLOCKTYPE_UNKNOWN)
         {
            // Block type is unknown so added insert refname to the map to be remove from netlist later
            compToBeRemove.SetAt(np->getInsert()->getRefname(), np->getInsert()->getRefname());
            continue;
         }

         // Ensure block type is appropriate. If it is already a test point, leave it.
         // Otherwise, it is either already a pcb component so setting it again changes nothing, or
         // it is not a pcb component yet so force it to be so now.
         // Original code just set block type to pcb component with no checks at all.
         // Note that this seems somewhat dangerous, this thing might not have a pin. 
         if (block->getBlockType() != blockTypeTestPoint)
            block->setBlockType(blockTypePcbComponent);

         CString compname = check_name('c',np->getInsert()->getRefname());
         CString geometry = check_name('s', AllegroNameMgr->GetName(block));
         CString part_number = geometry;

         Attrib *a = is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1); 

         if (a)
         {
            part_number = get_attvalue_string(doc, a);
         }

         double rot = RadToDeg(block_rot);
         while (rot < 0)   rot+=360;
         while (rot >= 360)   rot -=360;

         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_UNPLACED, 1))
         {
            fprintf(flog, "Unplaced Component [%s] located at 0,0\n", compname);
            display_error++;
         }

         fprintf(plc,"%s %1.*lf %1.*lf %1.1lf %s %s\n",
               compname, output_units_accuracy, point2.x, output_units_accuracy, point2.y, 
               rot,  (block_mirror)?" m ":"   ", geometry);

         CompList *c = new CompList;
         complistarray.SetAtGrow(complistcnt++,c);  
         c->compname = compname;
         c->geomname = geometry;
         c->devicefile_written = FALSE;

         if (TRUE_DEVICE)
         {
            fprintf(fp,"'%s' ! '%s' ; '%s'\n",  geometry, check_name('e',part_number), compname);
            c->devicename = part_number;
         }
         else
         {
            fprintf(fp,"'%s' ! '%s' ; '%s'\n",  geometry, geometry, compname);
            c->devicename = geometry;
         }
      }
   } 

   fclose(plc);

   NetStruct *net;
   POSITION  netPos;
   int   unusedpincnt = 0;
   
   fprintf(fp,"$NETS\n");

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      int   cnt, pincount;
      net = file->getNetList().GetNext(netPos);

      if ((net->getFlags() & NETFLAG_UNUSEDNET)) continue;

      pincount = net->getCompPinCount();

      if ((pincount <= 0) || (pincount == NULL)) continue; //added 01/26/2004 do not write nets without pins. - DF

      cnt = fprintf(fp,"'%s' ;", check_name('n', net->getNetName()));
   
      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         // if the comp name is in compToBeRemove then don't write to netlist
         CString insertRefname;

         if (compToBeRemove.Lookup(compPin->getRefDes(), insertRefname))
            continue;

         UpdateCompPinName(file, compPin, net->getNetName());       

         if (cnt > 70)
            cnt = fprintf(fp, ",\n");

         cnt += fprintf(fp, " '%s'.", check_name('c',compPin->getRefDes()));
         cnt += fprintf(fp, "'%s'", check_name('d',compPin->getPinName()));
      }

      fprintf(fp,"\n");
   }

   fprintf(fp,"$END\n");
   fclose(fp);

   return 1;
}

/******************************************************************************
* UpdateCompPinWithUniquePinRefname
*/
static void UpdateCompPinName(FileStruct *file, CompPinStruct *compPin, CString netName)
{
   WORD pinnrKW = doc->IsKeyWord(ATT_COMPPINNR, 0);

   // Skip if it can't find the component
   DataStruct *data = datastruct_from_refdes(doc, file->getBlock(), compPin->getRefDes());

   if (!data)
      return;

   // Skip if the block of the component does not exist
   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());

   if (!block)
      return;

   // Only update the compPin pin name if the block of this compPin does not have unique pin refname
   CString blockName = "";

   if (notUniquePinRefnameMap.Lookup(AllegroNameMgr->GetName(block), blockName))
   {
      double insert_x = data->getInsert()->getOriginX();
      double insert_y = data->getInsert()->getOriginY();
      double insert_rot = data->getInsert()->getAngle();
      bool insert_mirror = ((data->getInsert()->getMirrorFlags() & MIRROR_FLIP  ) != 0);  // flips koos

      int sameNameAndLocationCount = 0;
      CString pinNoName = "";
   
      // here loop throu block (component definition) and find PINPOS
      POSITION pinDataPos = block->getDataList().GetHeadPosition();
      while (pinDataPos)
      {
         DataStruct *pinData = block->getDataList().GetNext(pinDataPos);

         if (pinData->getDataType() != T_INSERT || pinData->getInsert()->getInsertType() != insertTypePin)
            continue;

         // If pin refname is different, then continue
         CString pinRefnam = pinData->getInsert()->getRefname();

         if (pinRefnam.CompareNoCase(compPin->getPinName()))
            continue;

         // Get the insert X & Y of the pin
         Point2 point2;
         Mat2x2 m;
         RotMat2(&m, insert_rot);

         point2.x = pinData->getInsert()->getOriginX();

         if (insert_mirror)
            point2.x = -point2.x;

         point2.y = pinData->getInsert()->getOriginY();
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // Check to see if it is the same pin by location, if yes then we found the pin
         if (!compPin->getPinCoordinatesComplete() || (fabs(point2.x - compPin->getOriginX()) <= SMALLNUMBER && fabs((DbUnit)point2.y - compPin->getOriginY()) <= SMALLNUMBER))
         {
            Attrib* attrib = NULL;

            if (pinData->getAttributesRef() && pinData->getAttributesRef()->Lookup(pinnrKW, attrib))
            {
               pinNoName.Format("%d", attrib->getIntValue());
            }

            sameNameAndLocationCount++;
         }
      }

      if (sameNameAndLocationCount == 1)
      {
         compPin->setPinName(pinNoName);
      }
      else if (sameNameAndLocationCount > 1)
      {
         if (!compPin->getPinCoordinatesComplete())
         {
            fprintf(flog, "Warning: Comp Pin [%s %s] for Net [%s] - cannot create unique Comp Pin because it has no location.\n",
                  compPin->getRefDes(), compPin->getPinName(), netName);

            display_error++;
         }
         else
         {
            fprintf(flog, "Warning: Comp Pin [%s %s] for Net [%s] - cannot create unique Comp Pin because the component has duplicate Comp Pin at the same location.\n",
                  compPin->getRefDes(), compPin->getPinName(), netName);

            display_error++;
         }
      }
   }

}

/******************************************************************************
* check_devicefiles
*/
static int check_devicefiles()
{
   for (int i=0; i<complistcnt; i++)
   {
      if (complistarray[i]->devicefile_written) continue;

      fprintf(flog, "No device file created for Component [%s] Package [%s] Device [%s]\n",
         complistarray[i]->compname, complistarray[i]->geomname, complistarray[i]->devicename);

      display_error++;
   }

   return 1;
}


/*****************************************************************************/
/*
*/
static   int   write_padmaskform(FILE *fp, FILE *shapefile, const char *l, 
                        double sizea, double sizeb,
                        double Xoffset, double Yoffset,
                        int shape, double rotation,int complexshape)
{
   double   reliefA, reliefB, antiA, antiB, sizeA, sizeB;
   double   xoffset, yoffset;
   int      i, r = (int)floor(RadToDeg(rotation) / 90 + 0.5);
   double   unitsFactor = Units_Factor(page_units, output_units);
   CString  shxname;
   double   accuracy = get_accuracy(doc);


   if (shape == T_COMPLEX)
   {
      BlockStruct *subblock = doc->Find_Block_by_Num(complexshape);

      if (subblock == NULL)
      {
         ErrorMessage("Can not find Complex Aperture");

         return -1;
      }

      sizea = 0;
      sizeb = 0;

      shxname = check_name('f', AllegroNameMgr->GetName(subblock));
      FILE  *fshx;
      CString fname;
      fname = filePathName;
      fname +=shxname;
      fname +=".scx";

      if ((fshx = fopen(fname,"wt")) == NULL)
      {
         CString  tmp;
         tmp.Format("Can not open PadShape file [%s]",fname);
         ErrorMessage(tmp);

         return -1;
      }

      // here get block and make a closed perfect outline
      Outline_Start(doc);         
      int found = Allegro_ApertureSHAPEData(&(subblock->getDataList()), Xoffset, Yoffset, rotation, 0, 
                  file->getScale() * unitsFactor, 0);

      int returnCode;

      if (!write_easypadshapeout(fshx, Outline_GetOutline(&returnCode, accuracy), shxname))
      {
         fprintf(flog, "Error in outline for [%s]\n", AllegroNameMgr->GetName(subblock));
         display_error++;
      }
      else
      {
         // tname.Format("%s.tech", fname);
         fprintf(shapefile,"allegro -s %s.scx %s\n",shxname, (BATCH_NOGRAPHIC)?"-nographic": "");
         fflush(shapefile);
      }

      Outline_FreeResults();
      fflush(fshx);
      fclose(fshx);

   }
   else
   {
      if (sizea < SMALL_DELTA && sizeb < SMALL_DELTA)
         return 0;
   }

   // padstack for can not have a rotation, must adjust for it
   if ((r*90.0) != round(RadToDeg(rotation)))
   {
      fprintf(flog, "Rotation [%d] other than 90 degree in Aperture\n", round(RadToDeg(rotation)));
      display_error++;
   }

   sizeA = sizea; sizeB = sizeb;
   xoffset = Xoffset; yoffset = Yoffset;

   for (i=0;i<r;i++)
   {
      double t;
      t = sizeA;
      sizeA = sizeB;
      sizeB = t;

      t = xoffset;
      xoffset = yoffset;
      yoffset = t;
   }

   // needs to be clarified
   reliefA = sizeA + 20;
   reliefB = sizeB + 20;
   antiA = sizeA + 20;
   antiB = sizeB + 20;

   fprintf(fp,"FORM padedit %s edit_pad\n", l);
   fprintf(fp,"setwindow form.padmask.%s\n", l);

   switch (shape)
   {
   case T_OCTAGON:
   case T_TARGET:
   case T_DONUT:
   case T_ROUND:
      fprintf(fp,"FORM padmask.%s regular-pad geometry Circle\n", l);
      fprintf(fp,"FORM padmask.%s regular-pad width %1.*lf\n", l, output_units_accuracy,sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad height %1.*lf\n", l, output_units_accuracy,sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad x_offset %1.*lf\n", l, output_units_accuracy,xoffset);
      fprintf(fp,"FORM padmask.%s regular-pad y_offset %1.*lf\n", l, output_units_accuracy,yoffset);

      break;
   case T_OBLONG:
      fprintf(fp,"FORM padmask.%s regular-pad geometry Oblong\n",l);
      fprintf(fp,"FORM padmask.%s regular-pad width %1.*lf\n",l, output_units_accuracy, sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad height %1.*lf\n",l, output_units_accuracy, sizeB);
      fprintf(fp,"FORM padmask.%s regular-pad x_offset %1.*lf\n",l, output_units_accuracy, xoffset);
      fprintf(fp,"FORM padmask.%s regular-pad y_offset %1.*lf\n",l, output_units_accuracy, yoffset);

      break;
   case T_RECTANGLE:
      fprintf(fp,"FORM padmask.%s regular-pad geometry Rectangle\n",l);
      fprintf(fp,"FORM padmask.%s regular-pad width %1.*lf\n",l, output_units_accuracy, sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad height %1.*lf\n",l, output_units_accuracy,  sizeB);
      fprintf(fp,"FORM padmask.%s regular-pad x_offset %1.*lf\n",l, output_units_accuracy, xoffset);
      fprintf(fp,"FORM padmask.%s regular-pad y_offset %1.*lf\n",l, output_units_accuracy, yoffset);

      break;
   case T_SQUARE:
      fprintf(fp,"FORM padmask.%s regular-pad geometry Square\n",l);
      fprintf(fp,"FORM padmask.%s regular-pad width %1.*lf\n",l,output_units_accuracy, sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad height %1.*lf\n",l,output_units_accuracy, sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad x_offset %1.*lf\n",l,output_units_accuracy, xoffset);
      fprintf(fp,"FORM padmask.%s regular-pad y_offset %1.*lf\n",l,output_units_accuracy, yoffset);

      break;
   case T_COMPLEX:
      if (Allegro_Version >= 136) // 
      {
         fprintf(fp,"FORM padmask.%s geometry Shape\n",l);
         fprintf(fp,"FORM padmask.%s shapename %s\n",l,shxname);
      }
      else
      {
         fprintf(fp,"FORM padmask.%s regular-pad geometry Shape\n",l);
         fprintf(fp,"FORM padmask.%s regular-pad shapename %s\n",l,shxname);
      }

      break;
   case T_UNDEFINED:             
   case T_BLANK:
      fprintf(fp,"FORM padmask.%s regular-pad geometry Null\n",l);
      fprintf(fp,"FORM padmask.%s regular-pad width %1.*lf\n",l,output_units_accuracy, sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad height %1.*lf\n",l,output_units_accuracy, sizeA);
      fprintf(fp,"FORM padmask.%s regular-pad x_offset %1.*lf\n",l,output_units_accuracy, xoffset);
      fprintf(fp,"FORM padmask.%s regular-pad y_offset %1.*lf\n",l,output_units_accuracy,yoffset);

      break;
   default:
      fprintf(flog, "Unknown aperture type\n");
      display_error++;

      break;
   } // switch shape

   if (Allegro_Version >= 136) // 
   {
      // do nothing.
   }
   else
   {
      fprintf(fp,"FORM padmask.%s done\n\n",l);
   }

   return 1;
}

/******************************************************************************
* ALLEGRO_WritePADSTACKData
*/
int ALLEGRO_WritePADSTACKData(FILE *fPadstack, FILE *fAutoShape, CDataList *DataList, int insertLayer, double scale,
      int mirror, double rotation, double *drillsize, BOOL SMD)
{
   int type = 0;  

   double thermosizeA = 0;
   double thermosizeB = 0;
   double thermoxoffset = 0;
   double thermoyoffset = 0;
   int thermoshape = -1;   // no shape found
   double thermorotation = 0;

   double innersizeA = 0;
   double innersizeB = 0;
   double innerxoffset = 0;
   double inneryoffset = 0;
   int innershape = -1; // no shape found
   double innerrotation = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   int padlayerarray[MAX_LAYTYPE];

   for (int i=0; i<MAX_LAYTYPE; i++)
      padlayerarray[i] = 0;

   // here get power layer
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case insertTypePcbComponent:
      case insertTypeTestPoint:
      case INSERTTYPE_VIA:
      case INSERTTYPE_CENTROID:
         continue;
      }

      // insert if mirror is either global mirror or block_mirror, but not if both.
      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (block->getFlags() & BL_TOOL)
         continue;

      if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      {
         int layerNum = Get_ApertureLayer(doc, data, block, insertLayer);

         LayerStruct *layer = doc->FindLayer(layerNum);

         if (!layer)
            continue;

         if (mirror & MIRROR_LAYERS)
            layer = doc->FindLayer(layer->getMirroredLayerIndex());

         switch (layer->getLayerType())
         {
         case LAYTYPE_POWERNEG:
         case LAYTYPE_PAD_THERMAL:
         case LAYTYPE_POWERPOS:
            thermosizeA = block->getSizeA() * scale;
            thermosizeB = block->getSizeB() * scale;
            thermoxoffset = block->getXoffset() * scale;
            thermoyoffset = block->getYoffset() * scale;
            thermoshape = block->getShape();
            thermorotation = block->getRotation() + data->getInsert()->getAngle();

            break;
         case LAYTYPE_SIGNAL_ALL:
         case LAYTYPE_PAD_ALL:
         case LAYTYPE_SIGNAL_INNER:
         case LAYTYPE_PAD_INNER:
            innersizeA = block->getSizeA() * scale;
            innersizeB = block->getSizeB() * scale;
            innerxoffset = block->getXoffset() * scale;
            inneryoffset = block->getYoffset() * scale;
            innershape = block->getShape();
            innerrotation = block->getRotation() + data->getInsert()->getAngle();

            break;
         }
      }
   }

   BOOL TopDone = FALSE, BottomDone = FALSE;

   dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case insertTypePcbComponent:
      case insertTypeTestPoint:
      case INSERTTYPE_VIA:
      case INSERTTYPE_CENTROID:
         continue;
      }

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;

      if (mirror)
         point2.x = -point2.x;

      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, 0, 0); // padstack is normalized

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      if (mirror)
         block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

      // insert if mirror is either global mirror or block_mirror, but not if both.
      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      block_rot += block->getRotation(); // aperture rotation.

      if (block->getFlags() & BL_TOOL)
      {
         type |= 4;

         fprintf(fPadstack, "FORM padedit ncdrill\n");
         fprintf(fPadstack, "setwindow form.padncdrill\n");

         if ((is_attvalue(doc, block->getAttributesRef(), DRILL_NONPLATED, 1)) || !(block->getToolHolePlated()))
            fprintf(fPadstack, "FORM padncdrill plating Non-Plated\n");
         else
            fprintf(fPadstack, "FORM padncdrill plating Plated\n");

         fprintf(fPadstack, "FORM padncdrill size %1.*lf\n", output_units_accuracy, block->getToolSize() * scale);
         fprintf(fPadstack, "FORM padncdrill x_offset %1.*lf\n", output_units_accuracy, block->getXoffset() * scale);
         fprintf(fPadstack, "FORM padncdrill y_offset %1.*lf\n", output_units_accuracy, block->getXoffset() * scale);
         fprintf(fPadstack, "FORM padncdrill figure Square\n");
         fprintf(fPadstack, "FORM padncdrill character %s\n", get_drillsymbol(block->getToolSize() * scale));
         fprintf(fPadstack, "FORM padncdrill width %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);
         fprintf(fPadstack, "FORM padncdrill height %1.*lf\n", output_units_accuracy, mils_2_units * 75.0);
         fprintf(fPadstack, "FORM padncdrill done\n");
         fprintf(fPadstack, "setwindow form.padedit\n");

         *drillsize = block->getToolSize() * scale;
      }
      else if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      {
         int layerNum = Get_ApertureLayer(doc, data, block, insertLayer);

         LayerStruct *layer = doc->FindLayer(layerNum);

         if (!layer)
            continue;

         if (mirror & MIRROR_LAYERS)
            layer = doc->FindLayer(layer->getMirroredLayerIndex());

         switch (layer->getLayerType())
         {
         case LAYTYPE_SIGNAL_TOP:
         case LAYTYPE_PAD_TOP:
            type |= 1;
            padlayerarray[layer->getLayerType()] = TRUE; // mark as done, so that ALL does not overwrite individual TOP

            write_padformthermo(fPadstack, fAutoShape, "BEGIN_LAYER", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA(), 
                  thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                  thermosizeA - innersizeA, thermosizeB - innersizeB);

            TopDone = TRUE;

            break;
         case LAYTYPE_SIGNAL_BOT:
         case LAYTYPE_PAD_BOTTOM:
            type |= 2;
            padlayerarray[layer->getLayerType()] = TRUE; // mark as done, so that ALL does not overwrite individual TOP

            write_padformthermo(fPadstack, fAutoShape, "END_LAYER", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA(),
                  thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                  thermosizeA - innersizeA, thermosizeB - innersizeB);

            BottomDone = TRUE;

            break;
         case LAYTYPE_SIGNAL_INNER:
         case LAYTYPE_PAD_INNER:
            type |= 3;
            padlayerarray[layer->getLayerType()] = TRUE; // mark as done, so that ALL does not overwrite individual TOP

            write_padformthermo(fPadstack, fAutoShape, "DEFAULT_INTERNAL", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot,(int)block->getSizeA(), 
                  thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                  thermosizeA - innersizeA, thermosizeB - innersizeB);

            break;
         case LAYTYPE_SIGNAL_ALL:
         case LAYTYPE_PAD_ALL:
            type |= 3;
            padlayerarray[layer->getLayerType()] = TRUE; // mark as done, so that ALL does not overwrite individual TOP

            if (!TopDone)
               write_padformthermo(fPadstack, fAutoShape, "BEGIN_LAYER", block->getSizeA() *scale, block->getSizeB() *scale,
                     block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot,(int)block->getSizeA(), 
                     thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                     thermosizeA - innersizeA, thermosizeB - innersizeB);

            write_padformthermo(fPadstack, fAutoShape, "DEFAULT_INTERNAL", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot,(int)block->getSizeA(), 
                  thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                  thermosizeA - innersizeA, thermosizeB - innersizeB);

            if (!BottomDone)
               write_padformthermo(fPadstack, fAutoShape, "END_LAYER", block->getSizeA() *scale, block->getSizeB() *scale,
                     block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot,(int)block->getSizeA(),
                     thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                     thermosizeA - innersizeA, thermosizeB - innersizeB);

            break;
         case LAYTYPE_SIGNAL_OUTER:
         case LAYTYPE_PAD_OUTER:
            type |= 3;
            padlayerarray[layer->getLayerType()] = TRUE; // mark as done, so that ALL does not overwrite individual TOP

            if (!TopDone)
               write_padformthermo(fPadstack, fAutoShape, "BEGIN_LAYER", block->getSizeA() *scale, block->getSizeB() *scale,
                     block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot,(int)block->getSizeA(), 
                     thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                     thermosizeA - innersizeA, thermosizeB - innersizeB);

            if (!BottomDone)
               write_padformthermo(fPadstack, fAutoShape, "END_LAYER", block->getSizeA() *scale, block->getSizeB() *scale,
                     block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot,(int)block->getSizeA(),
                     thermosizeA, thermosizeB, thermoxoffset, thermoyoffset, thermoshape, thermorotation,
                     thermosizeA - innersizeA, thermosizeB - innersizeB);

            break;
         case LAYTYPE_MASK_TOP:
            write_padmaskform(fPadstack, fAutoShape, "SOLDERMASK_TOP", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());

            break;
         case LAYTYPE_MASK_BOTTOM:
            write_padmaskform(fPadstack, fAutoShape, "SOLDERMASK_BOTTOM", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());

            break;
         case LAYTYPE_PASTE_TOP:
            write_padmaskform(fPadstack, fAutoShape, "PASTEMASK_TOP", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());

            break;
         case LAYTYPE_PASTE_BOTTOM:
            write_padmaskform(fPadstack, fAutoShape, "PASTEMASK_BOTTOM", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());

            break;
         case LAYTYPE_MASK_ALL:
            write_padmaskform(fPadstack, fAutoShape, "SOLDERMASK_TOP", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());
            
            if (!SMD) // do not do it if it is a SMD pad.
               write_padmaskform(fPadstack, fAutoShape, "SOLDERMASK_BOTTOM", block->getSizeA() *scale, block->getSizeB() *scale,
                     block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());

            break;
         case LAYTYPE_PASTE_ALL:
            write_padmaskform(fPadstack, fAutoShape, "PASTEMASK_TOP", block->getSizeA() *scale, block->getSizeB() *scale,
                  block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());

            if (!SMD) // do not do it if it is a SMD pad.
               write_padmaskform(fPadstack, fAutoShape, "PASTEMASK_BOTTOM", block->getSizeA() *scale, block->getSizeB() *scale,
                     block->getXoffset() *scale, block->getYoffset() *scale, block->getShape(), block_rot, (int)block->getSizeA());

            break;
         }
      }
      else
      {
         fprintf(flog, "Unknown or Complex Pad graphic [%s]\n", block->getName());
         display_error++;
      }
   }

   return type;
}

/******************************************************************************
* ALLEGRO_WritePADSTACKData_136
   
   NOTE: This function has been rewrite to fixe TSR #3994 & #4005.
         To see the difference between this and last version user SourceSafe
*/
int ALLEGRO_WritePADSTACKData_136(FILE *fPadstack, FILE *fAutoShape, CDataList *DataList, int insertLayer, double scale,
      int mirror, double rotation, double *drillsize, BOOL SMD, CString padstackName)
{

   Mat2x2 m;
   RotMat2(&m, rotation);

   double innerSizeA = 0;
   double innerSizeB = 0;
   double thermoSizeA = 0;
   double thermoSizeB = 0;
   double thermoXoffset = 0;
   double thermoYoffset = 0;
   double thermoRotation = 0;
   int thermoShape = -1;   // no shape found
   int type = 0;  

   PadInsert padInsertArray[PAD_136_MAX_LAYER];

   for (int i=0; i<PAD_136_MAX_LAYER; i++)
   {
      padInsertArray[i].sizeA = 0;
      padInsertArray[i].sizeB = 0;
      padInsertArray[i].xoffset = 0;
      padInsertArray[i].yoffset = 0;
      padInsertArray[i].rotation = 0;
      padInsertArray[i].shape = -1;
      padInsertArray[i].thermal = FALSE;
      padInsertArray[i].created = FALSE;
   }

   // here get power layer
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case insertTypePcbComponent:
      case insertTypeTestPoint:
      case INSERTTYPE_VIA:
      case INSERTTYPE_CENTROID:
         continue;
      }

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (!block)
         continue;

      if (block->getFlags() & BL_TOOL) // write out the drill
      {
         type |= 4;

         int Plated = TRUE;

         if ((is_attvalue(doc, block->getAttributesRef(), DRILL_NONPLATED, 1)) || !(block->getToolHolePlated()))
            Plated = FALSE;

         drillhole_136(fPadstack, block->getToolSize(), block->getXoffset(), block->getYoffset(), scale, Plated, FALSE);
         *drillsize = block->getToolSize() * scale;
      }
      else if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      {
         int layerNum = Get_ApertureLayer(doc, data, block, insertLayer);

         LayerStruct *layer = doc->FindLayer(layerNum);

         if (!layer)
            continue;

         if (mirror & MIRROR_LAYERS)
            layer = doc->FindLayer(layer->getMirroredLayerIndex());

         int blockShape = block->getShape();
         int blockMirror = mirror ^ data->getInsert()->getMirrorFlags();
         int complexGeomNum = 0;
         double blockSizeA = block->getSizeA();
         double blockSizeB = block->getSizeB();
         double blockXoffset = block->getXoffset();
         double blockYoffset = block->getYoffset();
         double blockRotation = rotation + data->getInsert()->getAngle(); // block->getRotation() + data->getInsert()->getAngle();

         if (mirror)
            blockRotation = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

         blockRotation += block->getRotation();   // aperture rotation.

         if (block->getShape() == T_COMPLEX)
         {
            doc->validateBlockExtents(block);

            if (!block || block->getXmin() > block->getXmax())
            {
               blockSizeA = 0;
               blockSizeB = 0;
            }
            else
            {
               blockSizeA = (block->getXmax() - block->getXmin());
               blockSizeB = (block->getYmax() - block->getYmin());
               // The following 2 lines incorrectly change the block offset to
               // be the graphic center, not actually the origin.
               // blockXoffset = (block->xmin + (block->xmax - block->xmin)/2);
               // blockYoffset = (block->ymin + (block->ymax - block->ymin)/2);
            }
            complexGeomNum = (int)block->getSizeA();
         }

         // Get the type
         int layerType = layer->getLayerType();

         switch (layerType)
         {
         case LAYTYPE_SIGNAL_TOP:
         case LAYTYPE_PAD_TOP:
            type |= 1;

            break;
         case LAYTYPE_SIGNAL_BOT:
         case LAYTYPE_PAD_BOTTOM:
            type |= 2;

            break;
         case LAYTYPE_SIGNAL_ALL:
         case LAYTYPE_PAD_ALL:
         case LAYTYPE_SIGNAL_INNER:
         case LAYTYPE_PAD_INNER:
         case LAYTYPE_SIGNAL_OUTER:
         case LAYTYPE_PAD_OUTER:
            type |= 3;

            break;
         }

         if (layerType == LAYTYPE_POWERNEG || layerType == LAYTYPE_PAD_THERMAL || layerType == LAYTYPE_POWERPOS)
         {
            // Thermal layer
            if (blockShape == -1 || (blockShape != T_COMPLEX && blockShape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               thermoSizeA = blockSizeA;
               thermoSizeB = blockSizeB;
               thermoXoffset = blockXoffset;
               thermoYoffset = blockYoffset;
               thermoShape = blockShape;
               thermoRotation = block->getRotation() + data->getInsert()->getAngle();
            }
            else if (blockShape == -1 || (blockShape != T_COMPLEX && blockShape != T_COMPLEX) || (blockShape == T_COMPLEX && blockShape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > thermoSizeA)) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > thermoSizeA * thermoSizeB)) )
               {
                  thermoSizeA = blockSizeA;
                  thermoSizeB = blockSizeB;
                  thermoXoffset = blockXoffset;
                  thermoYoffset = blockYoffset;
                  thermoShape = blockShape;
                  thermoRotation = block->getRotation() + data->getInsert()->getAngle();
               }
            }
         }
         
         if (layerType == LAYTYPE_SIGNAL_ALL || layerType == LAYTYPE_SIGNAL_INNER ||
             layerType == LAYTYPE_PAD_ALL || layerType == LAYTYPE_PAD_INNER)
         {
            if (SMD) // don't add inner layer
               continue;

            // Inner Layer
            PadInsert *padInsert = &padInsertArray[PAD_136_INTLAYER];

            if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               innerSizeA = blockSizeA;
               innerSizeB = blockSizeB;

               padInsert->setSizeA(blockSizeA);
               padInsert->setSizeB(blockSizeB);
               padInsert->xoffset = blockXoffset;
               padInsert->yoffset = blockYoffset;
               padInsert->shape = blockShape;
               padInsert->rotation = blockRotation;
               padInsert->thermal = TRUE;
               padInsert->created = TRUE;
               padInsert->complexGeomNum = complexGeomNum;
            
               if (padInsert->shape != -1)
               {
                  fprintf(flog, "Warning: Padstack [%s] - multiple padshape for INNER layer, normal padshape overwrite complex padsahpe.\n", padstackName);
                  display_error++;
               }
            }
            else if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape != T_COMPLEX) || (blockShape == T_COMPLEX && padInsert->shape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > padInsert->getSizeA())) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > padInsert->getSizeA() * padInsert->getSizeB())) )
               {
                  innerSizeA = blockSizeA;
                  innerSizeB = blockSizeB;
               
                  padInsert->setSizeA(blockSizeA);
                  padInsert->setSizeB(blockSizeB);
                  padInsert->xoffset = blockXoffset;
                  padInsert->yoffset = blockYoffset;
                  padInsert->shape = blockShape;
                  padInsert->rotation = blockRotation;
                  padInsert->thermal = TRUE;
                  padInsert->created = TRUE;
                  padInsert->complexGeomNum = complexGeomNum;

                  if (padInsert->shape != -1)
                  {
                     fprintf(flog, "Warning: Padstack [%s] - multiple padshape for INNER layer, largest padshape overwrite small padsahpe.\n", padstackName);
                     display_error++;
                  }
               }
            }
         }

         if (layerType == LAYTYPE_SIGNAL_ALL || layerType == LAYTYPE_SIGNAL_OUTER || layerType == LAYTYPE_SIGNAL_TOP || 
             layerType == LAYTYPE_PAD_ALL || layerType == LAYTYPE_PAD_OUTER ||  layerType == LAYTYPE_PAD_TOP)
         {
            // Top Layer
            PadInsert *padInsert = &padInsertArray[PAD_136_TOPLAYER];

            if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               padInsert->setSizeA(blockSizeA);
               padInsert->setSizeB(blockSizeB);
               padInsert->xoffset = blockXoffset;
               padInsert->yoffset = blockYoffset;
               padInsert->shape = blockShape;
               padInsert->rotation = blockRotation;
               padInsert->thermal = TRUE;
               padInsert->created = TRUE;
               padInsert->complexGeomNum = complexGeomNum;

               if (padInsert->shape != -1)
               {
                  fprintf(flog, "Warning: Padstack [%s] - multiple padshape for TOP layer, normal padshape overwrite complex padsahpe.\n", padstackName);
                  display_error++;
               }
            }
            else if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape != T_COMPLEX) || (blockShape == T_COMPLEX && padInsert->shape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > padInsert->getSizeA())) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > padInsert->getSizeA() * padInsert->getSizeB())) )
               {
                  padInsert->setSizeA(blockSizeA);
                  padInsert->setSizeB(blockSizeB);
                  padInsert->xoffset = blockXoffset;
                  padInsert->yoffset = blockYoffset;
                  padInsert->shape = blockShape;
                  padInsert->rotation = blockRotation;
                  padInsert->thermal = TRUE;
                  padInsert->created = TRUE;
                  padInsert->complexGeomNum = complexGeomNum;

                  if (padInsert->shape != -1)
                  {
                     fprintf(flog, "Warning: Padstack [%s] - multiple padshape for TOP layer, largest padshape overwrite small padsahpe.\n", padstackName);
                     display_error++;
                  }
               }
            }
         }
         
         // Always do layer BOTTOM after layer TOP becasue layer ALL will be assing to layer TOP first
         if (layerType == LAYTYPE_SIGNAL_ALL || layerType == LAYTYPE_SIGNAL_OUTER || layerType == LAYTYPE_SIGNAL_BOT || 
             layerType == LAYTYPE_PAD_ALL || layerType == LAYTYPE_PAD_OUTER ||  layerType == LAYTYPE_PAD_BOTTOM)
         {
            if (SMD & padInsertArray[PAD_136_TOPLAYER].created) // there is already a TOP, don't add bottom
               continue;

            // Bot Layer
            PadInsert *padInsert = &padInsertArray[PAD_136_BOTLAYER];

            if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               padInsert->setSizeA(blockSizeA);
               padInsert->setSizeB(blockSizeB);
               padInsert->xoffset = blockXoffset;
               padInsert->yoffset = blockYoffset;
               padInsert->shape = blockShape;
               padInsert->rotation = blockRotation;
               padInsert->thermal = TRUE;
               padInsert->created = TRUE;
               padInsert->complexGeomNum = complexGeomNum;

               if (padInsert->shape != -1)
               {
                  fprintf(flog, "Warning: Padstack [%s] - multiple padshape for BOTTOM layer, normal padshape overwrite complex padsahpe.\n", padstackName);
                  display_error++;
               }
            }
            else if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape != T_COMPLEX) || (blockShape == T_COMPLEX && padInsert->shape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > padInsert->getSizeA())) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > padInsert->getSizeA() * padInsert->getSizeB())) )
               {
                  padInsert->setSizeA(blockSizeA);
                  padInsert->setSizeB(blockSizeB);
                  padInsert->xoffset = blockXoffset;
                  padInsert->yoffset = blockYoffset;
                  padInsert->shape = blockShape;
                  padInsert->rotation = blockRotation;
                  padInsert->thermal = TRUE;
                  padInsert->created = TRUE;
                  padInsert->complexGeomNum = complexGeomNum;

                  if (padInsert->shape != -1)
                  {
                     fprintf(flog, "Warning: Padstack [%s] - multiple padshape for BOTTOM layer, largest padshape overwrite small padsahpe.\n", padstackName);
                     display_error++;
                  }
               }
            }
         }

         if (layerType == LAYTYPE_MASK_ALL || layerType == LAYTYPE_MASK_TOP)
         {
            // Mask Top Layer
            PadInsert *padInsert = &padInsertArray[PAD_136_SMTOP];

            if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               padInsert->setSizeA(blockSizeA);
               padInsert->setSizeB(blockSizeB);
               padInsert->xoffset = blockXoffset;
               padInsert->yoffset = blockYoffset;
               padInsert->shape = blockShape;
               padInsert->rotation = blockRotation;
               padInsert->thermal = FALSE;
               padInsert->created = TRUE;
               padInsert->complexGeomNum = complexGeomNum;

               if (padInsert->shape != -1)
               {
                  fprintf(flog, "Warning: Padstack [%s] - multiple padshape for MASK_TOP layer, normal padshape overwrite complex padsahpe.\n", padstackName);
                  display_error++;
               }
            }
            else if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape != T_COMPLEX) || (blockShape == T_COMPLEX && padInsert->shape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > padInsert->getSizeA())) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > padInsert->getSizeA() * padInsert->getSizeB())) )
               {
                  padInsert->setSizeA(blockSizeA);
                  padInsert->setSizeB(blockSizeB);
                  padInsert->xoffset = blockXoffset;
                  padInsert->yoffset = blockYoffset;
                  padInsert->shape = blockShape;
                  padInsert->rotation = blockRotation;
                  padInsert->thermal = FALSE;
                  padInsert->created = TRUE;
                  padInsert->complexGeomNum = complexGeomNum;

                  if (padInsert->shape != -1)
                  {
                     fprintf(flog, "Warning: Padstack [%s] - multiple padshape for MASK_TOP layer, largest padshape overwrite small padsahpe.\n", padstackName);
                     display_error++;
                  }
               }
            }
         }

         // Always do layer BOTTOM after layer TOP becasue layer ALL will be assing to layer TOP first
         if (layerType == LAYTYPE_MASK_ALL || layerType == LAYTYPE_MASK_BOTTOM)
         {
            if (SMD & padInsertArray[PAD_136_TOPLAYER].created) // there is already a TOP, don't add bottom
               continue;

            // Mask Bot Layer
            PadInsert *padInsert = &padInsertArray[PAD_136_SMBOT];

            if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               padInsert->setSizeA(blockSizeA);
               padInsert->setSizeB(blockSizeB);
               padInsert->xoffset = blockXoffset;
               padInsert->yoffset = blockYoffset;
               padInsert->shape = blockShape;
               padInsert->rotation = blockRotation;
               padInsert->thermal = FALSE;
               padInsert->created = TRUE;
               padInsert->complexGeomNum = complexGeomNum;

               if (padInsert->shape != -1)
               {
                  fprintf(flog, "Warning: Padstack [%s] - multiple padshape for MASK_BOTTOM layer, normal padshape overwrite complex padsahpe.\n", padstackName);
                  display_error++;
               }
            }
            else if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape != T_COMPLEX) || (blockShape == T_COMPLEX && padInsert->shape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > padInsert->getSizeA())) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > padInsert->getSizeA() * padInsert->getSizeB())) )
               {
                  padInsert->setSizeA(blockSizeA);
                  padInsert->setSizeB(blockSizeB);
                  padInsert->xoffset = blockXoffset;
                  padInsert->yoffset = blockYoffset;
                  padInsert->shape = blockShape;
                  padInsert->rotation = blockRotation;
                  padInsert->thermal = FALSE;
                  padInsert->created = TRUE;
                  padInsert->complexGeomNum = complexGeomNum;

                  if (padInsert->shape != -1)
                  {
                     fprintf(flog, "Warning: Padstack [%s] - multiple padshape for MASK_BOTTOM layer, largest padshape overwrite small padsahpe.\n", padstackName);
                     display_error++;
                  }
               }
            }
         }

         if (layerType == LAYTYPE_PASTE_ALL || layerType == LAYTYPE_PASTE_TOP)
         {
            // Paste Top Layer
            PadInsert *padInsert = &padInsertArray[PAD_136_PSTTOP];

            if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               padInsert->setSizeA(blockSizeA);
               padInsert->setSizeB(blockSizeB);
               padInsert->xoffset = blockXoffset;
               padInsert->yoffset = blockYoffset;
               padInsert->shape = blockShape;
               padInsert->rotation = blockRotation;
               padInsert->thermal = FALSE;
               padInsert->created = TRUE;
               padInsert->complexGeomNum = complexGeomNum;

               if (padInsert->shape != -1)
               {
                  fprintf(flog, "Warning: Padstack [%s] - multiple padshape for PASTE_TOP layer, normal padshape overwrite complex padsahpe.\n", padstackName);
                  display_error++;
               }
            }
            else if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape != T_COMPLEX) || (blockShape == T_COMPLEX && padInsert->shape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > padInsert->getSizeA())) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > padInsert->getSizeA() * padInsert->getSizeB())) )
               {
                  padInsert->setSizeA(blockSizeA);
                  padInsert->setSizeB(blockSizeB);
                  padInsert->xoffset = blockXoffset;
                  padInsert->yoffset = blockYoffset;
                  padInsert->shape = blockShape;
                  padInsert->rotation = blockRotation;
                  padInsert->thermal = FALSE;
                  padInsert->created = TRUE;
                  padInsert->complexGeomNum = complexGeomNum;

                  if (padInsert->shape != -1)
                  {
                     fprintf(flog, "Warning: Padstack [%s] - multiple padshape for PASTE_TOP layer, largest padshape overwrite small padsahpe.\n", padstackName);
                     display_error++;
                  }
               }
            }
         }

         // Always do layer BOTTOM after layer TOP becasue layer ALL will be assing to layer TOP first
         if (layerType == LAYTYPE_PASTE_ALL || layerType == LAYTYPE_PASTE_BOTTOM)   
         {
            if (SMD & padInsertArray[PAD_136_TOPLAYER].created) // there is already a TOP, don't add bottom
               continue;

            // Paste Bot Layer
            PadInsert *padInsert = &padInsertArray[PAD_136_PSTBOT];

            if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape == T_COMPLEX)) // Overwrite if the shape in the array is complex
            {
               padInsert->setSizeA(blockSizeA);
               padInsert->setSizeB(blockSizeB);
               padInsert->xoffset = blockXoffset;
               padInsert->yoffset = blockYoffset;
               padInsert->shape = blockShape;
               padInsert->rotation = blockRotation;
               padInsert->thermal = FALSE;
               padInsert->created = TRUE;
               padInsert->complexGeomNum = complexGeomNum;

               if (padInsert->shape != -1)
               {
                  fprintf(flog, "Warning: Padstack [%s] - multiple padshape for PASTE_BOTTOM layer, normal padshape overwrite complex padsahpe.\n", padstackName);
                  display_error++;
               }
            }
            else if (padInsert->shape == -1 || (blockShape != T_COMPLEX && padInsert->shape != T_COMPLEX) || (blockShape == T_COMPLEX && padInsert->shape == T_COMPLEX))
            {
               if ( ((blockShape == T_SQUARE || blockShape == T_ROUND) && (blockSizeA > padInsert->getSizeA())) || 
                    ((blockShape != T_SQUARE && blockShape != T_ROUND) && (blockSizeA * blockSizeB > padInsert->getSizeA() * padInsert->getSizeB())) )
               {
                  padInsert->setSizeA(blockSizeA);
                  padInsert->setSizeB(blockSizeB);
                  padInsert->xoffset = blockXoffset;
                  padInsert->yoffset = blockYoffset;
                  padInsert->shape = blockShape;
                  padInsert->rotation = blockRotation;
                  padInsert->thermal = FALSE;
                  padInsert->created = TRUE;
                  padInsert->complexGeomNum = complexGeomNum;

                  if (padInsert->shape != -1)
                  {
                     fprintf(flog, "Warning: Padstack [%s] - multiple padshape for PASTE_BOTTOM layer, largest padshape overwrite small padsahpe.\n", padstackName);
                     display_error++;
                  }
               }
            }
         }
      }
   }

   double restringA = (thermoSizeA - innerSizeA) * scale;
   double restringB = (thermoSizeB - innerSizeB) * scale;

   thermoSizeA *= scale;
   thermoSizeB *= scale;

   for (int i=1; i<PAD_136_MAX_LAYER; i++)
   {
      PadInsert padInsert = padInsertArray[i];

      if (!padInsert.created)
         continue;

      write_padformthermo_136(fPadstack, fAutoShape, i, padInsert.sizeA * scale, padInsert.sizeB * scale, 
            padInsert.xoffset * scale, padInsert.yoffset * scale, padInsert.shape, padInsert.rotation, padInsert.complexGeomNum,
            thermoSizeA, thermoSizeB, thermoXoffset, thermoYoffset, thermoShape, thermoRotation, restringA, restringB,
            padInsert.thermal, SMD);
   }

   return type;
}

//--------------------------------------------------------------
//
int ALLEGRO_WriteVias_GenericData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;

   RotMat2(&m, rotation);

   fprintf(wfp,"# Here all via, generic and mechanical component data\n");

   if (Allegro_Version >= 136) // 
   {
      fprintf(wfp,"cns\n");
      fprintf(wfp,"setwindow form.cns\n");

      if (!DRC_ON)
         fprintf(wfp,"FORM cns drc_off YES\n");
      
      fprintf(wfp,"cns simple\n");
      fprintf(wfp,"setwindow form.cns_simple\n");
   
      if ((Allegro_Version < 152) && (strlen(default_via)))
           fprintf(wfp,"FORM cns_simple default_via %s\n", default_via);

      fprintf(wfp,"FORM cns_simple done\n");
      fprintf(wfp,"setwindow form.cns\n");
      fprintf(wfp,"FORM cns done\n");
      fprintf(wfp,"setwindow pcb\n");
   }

   fprintf(wfp,"add connect\n");
   fprintf(wfp,"setwindow form.mini\n");
   fprintf(wfp,"FORM mini snap_cpt NO\n");

   // added for version 13.6
   fprintf(wfp,"setwindow form.find\n");
   fprintf(wfp,"FORM find all_off\n");
   // end of 13.6

   fprintf(wfp,"setwindow pcb\n");
   fprintf(wfp,"done\n");

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != INSERTTYPE_VIA &&
                np->getInsert()->getInsertType() != INSERTTYPE_FREEPAD &&
                np->getInsert()->getInsertType() != INSERTTYPE_DRILLHOLE &&
                np->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT &&
                np->getInsert()->getInsertType() != INSERTTYPE_GENERICCOMPONENT &&
                np->getInsert()->getInsertType() != INSERTTYPE_MECHCOMPONENT)  continue;

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;

            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            // block->getRotation() is the aperture rotation 
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.
            
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               if (SKILL)
               {
                  CString  t;
                  t.Format("%1.*lf:%1.*lf", output_units_accuracy, point2.x, output_units_accuracy, point2.y);
                  fprintf(wfp, "(axlDBCreateVia( \"%s\", %s ))\n", check_name('p', AllegroNameMgr->GetName(block)), t);
               }
               else
               {
                  fprintf(wfp,"add connect\n");

                  write_pick(wfp,point2.x, point2.y, 0);

                  fprintf(wfp,"pop drill %s\n",check_name('p', AllegroNameMgr->GetName(block)));
                  fprintf(wfp,"done\n");        
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {
               if (SKILL)
               {
                  CString  t;
                  t.Format("%1.*lf:%1.*lf", output_units_accuracy, point2.x, output_units_accuracy, point2.y);
                  fprintf(wfp, "(axlDBCreateVia( \"%s\", %s ))\n", check_name('p', AllegroNameMgr->GetName(block)), t);
               }
               else
               {
                  fprintf(wfp,"add connect\n");

                  write_pick(wfp,point2.x, point2.y, 0);

                  fprintf(wfp,"pop drill %s\n",check_name('p', AllegroNameMgr->GetName(block)));
                  fprintf(wfp,"done\n");        
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)
            {
               // 
               if (Allegro_Version >= 140) // 
               {
                  fprintf(wfp,"place manual\n");
                  fprintf(wfp,"setwindow form.plc_manual\n");
                  fprintf(wfp,"FORM plc_manual advanced_settings\n");
                  fprintf(wfp,"FORM plc_manual library YES\n");
                  fprintf(wfp,"FORM plc_manual placement_list\n");
                  fprintf(wfp,"FORM plc_manual tree  \'Mechanical symbols\'\n");
                  fprintf(wfp,"FORM plc_manual tree \'MULTISEL YES\' %s \'Mechanical symbols\'\n",check_name('s',AllegroNameMgr->GetName(block)));
                  fprintf(wfp,"FORM plc_manual tree  %s \'Mechanical symbols\'\n",check_name('s',AllegroNameMgr->GetName(block)));
                  fprintf(wfp,"setwindow pcb\n");

                  write_pick(wfp,point2.x, point2.y, 0);

                  fprintf(wfp,"setwindow form.plc_manual\n");
                  fprintf(wfp,"FORM plc_manual done\n");

               }
               else
               {
                  fprintf(wfp,"add symbol mechanical\n");
                  fprintf(wfp,"fillin \"%s\"\n",check_name('s', AllegroNameMgr->GetName(block)));

                  write_pick(wfp,point2.x, point2.y, 0);

                  fprintf(wfp,"done\n");
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
            {
               fprintf(flog, "Testpoint not implemented.\n");
               display_error++;
            }
            else if ((np->getInsert()->getInsertType() == INSERTTYPE_GENERICCOMPONENT) || (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT))
            {
               //int bt = block->getBlockType();
               if (((block->getBlockType() == BLOCKTYPE_DRILLHOLE) || 
                   (block->getBlockType() == BLOCKTYPE_GENERICCOMPONENT)) || 
                   (block->getBlockType() == BLOCKTYPE_MECHCOMPONENT))
               {
                  // Mechanical Symbol
                  if (Allegro_Version >= 140) // 
                  {
                     fprintf(wfp,"place manual\n");
                     fprintf(wfp,"setwindow form.plc_manual\n");
                     fprintf(wfp,"FORM plc_manual advanced_settings\n");
                     fprintf(wfp,"FORM plc_manual library YES\n");
                     fprintf(wfp,"FORM plc_manual placement_list\n");
                     fprintf(wfp,"FORM plc_manual tree  \'Mechanical symbols\'\n");
                     fprintf(wfp,"FORM plc_manual tree \'MULTISEL YES\' %s \'Mechanical symbols\'\n",check_name('s',AllegroNameMgr->GetName(block)));
                     fprintf(wfp,"FORM plc_manual tree  %s \'Mechanical symbols\'\n",check_name('s',AllegroNameMgr->GetName(block)));
                     fprintf(wfp,"setwindow pcb\n");
                     fprintf(wfp,"rotate\niangle %1.0lf\n",RadToDeg(block_rot));

                     write_pick(wfp,point2.x, point2.y, 0);

                     fprintf(wfp,"setwindow form.plc_manual\n");
                     fprintf(wfp,"FORM plc_manual done\n");

                  }
                  else
                  {
                     fprintf(wfp,"add symbol mechanical\n");
                     fprintf(wfp,"fillin \"%s\"\n",check_name('s',AllegroNameMgr->GetName(block)));
                     fprintf(wfp,"rotate\niangle %1.0lf\n",RadToDeg(block_rot));

                     write_pick(wfp,point2.x, point2.y, 0);

                     fprintf(wfp,"done\n");     
                  }
               }
               else if ((block->getBlockType() == BLOCKTYPE_UNKNOWN) || (block->getBlockType() == BLOCKTYPE_DRAWING))
               {
                  // Format Symbol
                  if (Allegro_Version >= 140) // 
                  {
                     fprintf(wfp,"place manual\n");
                     fprintf(wfp,"setwindow form.plc_manual\n");
                     fprintf(wfp,"FORM plc_manual advanced_settings\n");
                     fprintf(wfp,"FORM plc_manual library YES\n");
                     fprintf(wfp,"FORM plc_manual placement_list\n");
                     // fprintf(wfp,"FORM plc_manual tree  \'Components by refdes\'\n"); This line removed for the following line
                     fprintf(wfp,"FORM plc_manual tree  \'Format symbols\'\n");
                     fprintf(wfp,"FORM plc_manual tree \'MULTISEL YES\' %s \'Format symbols\'\n",check_name('s',AllegroNameMgr->GetName(block)));
                     fprintf(wfp,"FORM plc_manual tree  %s \'Format symbols\'\n",check_name('s',AllegroNameMgr->GetName(block)));
                     fprintf(wfp,"setwindow pcb\n");
                     fprintf(wfp,"rotate\niangle %1.0lf\n",RadToDeg(block_rot));

                     write_pick(wfp,point2.x, point2.y, 0);

                     fprintf(wfp,"setwindow form.plc_manual\n");
                     fprintf(wfp,"FORM plc_manual done\n");

                  }
                  else
                  {
                     fprintf(wfp,"add symbol format\n");
                     fprintf(wfp,"fillin \"%s\"\n",check_name('s',AllegroNameMgr->GetName(block)));
                     fprintf(wfp,"rotate\niangle %1.0lf\n",RadToDeg(block_rot));

                     write_pick(wfp,point2.x, point2.y, 0);

                     fprintf(wfp,"done\n");  
                  }
               }
            }
         } // case INSERT

         break;
      } // end switch
   } // end for

   return 0;
} /* end ALLEGRO_WriteVias_GenericData */

/******************************************************************************
* WritePin
*/
static int WritePin(FILE *fGeom, const char *block_name, const char *pinname, double block_rot, double x, double y)
{
   fprintf(fGeom, "add pin\n");
   fprintf(fGeom, "setwindow form.mini\n");

   if (Allegro_Version >= 140) // 
      fprintf(fGeom, "FORM mini pad_name %s\n",check_name('p',block_name));
   else
      fprintf(fGeom, "FORM mini padname %s\n",check_name('p',block_name));

   fprintf(fGeom, "FORM mini next_pin_number %s\n",check_name('d',pinname));
   fprintf(fGeom, "FORM mini rotate_pin %1.3lf\n",RadToDeg(block_rot));

   if (Allegro_Version >= 140)
      fprintf(fGeom, "FORM mini offsetx 0.00\n");

   if (Allegro_Version >= 136) // includes 136
      fprintf(fGeom, "setwindow pcb\n");
   else
      fprintf(fGeom, "setwindow form.mini\n");

   fprintf(fGeom, "done\n");
   fprintf(fGeom, "add pin\n");

   write_pick(fGeom, x, y, 0);

   return 1;
}

/******************************************************************************
* ALLEGRO_WritePackagePinData
*/
int ALLEGRO_WritePackagePinData(FILE *fGeom, BlockStruct *block, double insert_x, double insert_y,
      double rotation, int mirror, double scale, BOOL WritePins)
{
   WORD pinnrKW = doc->IsKeyWord(ATT_COMPPINNR, 0);

   Mat2x2 m;
   RotMat2(&m, rotation);

   int pinCount = 0;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case INSERTTYPE_PIN:
      case INSERTTYPE_MECHANICALPIN:
      case INSERTTYPE_DRILLHOLE:
         break;         
      default:
         continue;
      }

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;

      if (mirror)
         point2.x = -point2.x;

      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      BlockStruct *subBlock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      if (mirror)
         block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

      switch (data->getInsert()->getInsertType())
      {
      case INSERTTYPE_PIN:
         {
            CString pinName = data->getInsert()->getRefname();

            CString blockName = "";

            if (notUniquePinRefnameMap.Lookup(AllegroNameMgr->GetName(block), blockName))
            {
               // pin refname of the block is not unique so use the PINNR attribute as refname

               Attrib* attrib = NULL;

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(pinnrKW, attrib))
               {
                  pinName.Format("%d", attrib->getIntValue());
               }
               else
               {
                  fprintf(flog, "Pinname is not unique and PINNR attribute is not found for Package %s\n", block->getName());

                  display_error++;
               }
            }

            if (pinName.IsEmpty())
            {
               fprintf(flog, "Pinname is not found for Package %s\n", block->getName());

               display_error++;
            }

            if (WritePins)
            {
               CString padstackName = check_name('p', AllegroNameMgr->GetName(subBlock));

               if (data->getInsert()->getMirrorFlags())
                  padstackName += "_MIRROR";

               WritePin(fGeom, padstackName, pinName, block_rot, point2.x, point2.y);
            }

            pinCount++;
         }

         break;
      case INSERTTYPE_MECHANICALPIN:
         {
            if (WritePins)
            {
               fprintf(fGeom, "add pin\n");
               fprintf(fGeom, "setwindow form.mini\n");
               fprintf(fGeom, "FORM mini pintype_mechanical YES\n");
               fprintf(fGeom, "FORM mini rotate_pin %1.3lf\n",RadToDeg(block_rot));

               //if (Allegro_Version >= 140)
               // fprintf(fGeom, "FORM mini offsetx 0.00\n");

               fprintf(fGeom, "setwindow form.mini\n");

               if (Allegro_Version >= 140) // 
                  fprintf(fGeom, "FORM mini pad_name %s\n",check_name('p', AllegroNameMgr->GetName(subBlock)));
               else
                  fprintf(fGeom, "FORM mini padname %s\n",check_name('p', AllegroNameMgr->GetName(subBlock)));

               fprintf(fGeom, "setwindow pcb\n");

               write_pick(fGeom, point2.x, point2.y, 0);
            }        
         }

         break;
      case INSERTTYPE_DRILLHOLE:
         {
            if (WritePins)
            {
               fprintf(fGeom, "add pin\n");
               fprintf(fGeom, "setwindow form.mini\n");
               fprintf(fGeom, "FORM mini rotate_pin %1.3lf\n",RadToDeg(block_rot));

               //if (Allegro_Version >= 140)
               // fprintf(fGeom, "FORM mini offsetx 0.00\n");

               fprintf(fGeom, "setwindow form.mini\n");

               if (Allegro_Version >= 140) // 
                  fprintf(fGeom, "FORM mini pad_name %s\n",check_name('p', AllegroNameMgr->GetName(subBlock)));
               else
                  fprintf(fGeom, "FORM mini padname %s\n",check_name('p', AllegroNameMgr->GetName(subBlock)));

               fprintf(fGeom, "setwindow form.mini\n");
               fprintf(fGeom, "FORM mini pintype_mechanical YES\n");
               fprintf(fGeom, "setwindow pcb\n");

               write_pick(fGeom, point2.x, point2.y, 0);
            }
         }

         break;
      }
   }

   if (WritePins && pinCount)
      fprintf(fGeom, "done\n");

   return pinCount;
}

//--------------------------------------------------------------
int ALLEGRO_WriteMechanicalDrillData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int write_pins)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   pincnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;

            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            // no mirror, no     
            if (block->getBlockType() == BLOCKTYPE_DRILLHOLE)
            {
               fprintf(wfp,"add pin\n");
               fprintf(wfp,"setwindow form.mini\n");
               fprintf(wfp,"FORM mini rotate_pin 0.0\n");

               fprintf(wfp,"setwindow form.mini\n");

               if (Allegro_Version >= 140) // 
                  fprintf(wfp,"FORM mini pad_name %s\n",check_name('p', AllegroNameMgr->GetName(block)));
               else
                  fprintf(wfp,"FORM mini padname %s\n",check_name('p', AllegroNameMgr->GetName(block)));

               fprintf(wfp,"setwindow pcb\n");

               write_pick(wfp,point2.x, point2.y, 0);
            }
         } // case INSERT

         break;
      } // end switch
   } // end for

   return pincnt;
 } /* end ALLEGRO_WriteMechanicalDrillData */

/******************************************************************************
* ALLEGRO_WriteDeviceData
*/
int ALLEGRO_WriteDeviceData(FILE *fp, BlockStruct *block, const char *devname)
{
   WORD pinnrKW = doc->IsKeyWord(ATT_COMPPINNR, 0);
   int lcnt = fprintf(fp,"PINORDER %s ",devname);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

      CString pinname = data->getInsert()->getRefname();

      CString blockName = "";

      if (notUniquePinRefnameMap.Lookup(AllegroNameMgr->GetName(block), blockName))
      {
         // pin refname of the block is not unique so use the PINNR attribute as refname

         Attrib* attrib = NULL;

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(pinnrKW, attrib))
         {
            pinname.Format("%d", attrib->getIntValue());
         }
         else
         {
            fprintf(flog, "Pinname is not unique and PINNR attribute is not found for Package %s\n", block->getName());
            display_error++;
         }
      }

      if (strlen(pinname) == 0)
      {
         fprintf(flog, "Pinname is not found for Package %s\n", block->getName());
         display_error++;
      }

      if (lcnt > 70)
         lcnt = fprintf(fp, ",\n");

      lcnt += fprintf(fp, "'%s' ", check_name('d', pinname));
   }

   fprintf(fp, "\n");

   lcnt = fprintf(fp,"FUNCTION g1 %s ",devname);

   dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

      CString pinname = data->getInsert()->getRefname();

      CString blockName = "";

      if (notUniquePinRefnameMap.Lookup(AllegroNameMgr->GetName(block), blockName))
      {
         // pin refname of the block is not unique so use the PINNR attribute as refname

         Attrib* attrib = NULL;

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(pinnrKW, attrib))
         {
            pinname.Format("%d", attrib->getIntValue());
         }
         else
         {
            fprintf(flog, "Pinname is not unique and PINNR attribute is not found for Package %s\n", block->getName());

            display_error++;
         }
      }

      if (strlen(pinname) == 0)
      {
         fprintf(flog, "Pinname is not found for Package %s\n", block->getName());

         display_error++;
      }

      if (lcnt > 70)
         lcnt = fprintf(fp, ",\n");

      lcnt += fprintf(fp, "'%s' ", check_name('d', pinname));
   }

   fprintf(fp, "\n");

   return 0;
 }

/******************************************************************************
* ALLEGRO_WriteData
*/
int ALLEGRO_WriteData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, int layclass, int hierachiemarker)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   int   lindex;  // maplayer index;
   int   layerclass;
   int   mlarrptr;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
            layer = np->getLayerIndex();
         else 
            layer = insertLayer;

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         const char  *l = Layer_ALLEGRO(layer);

         if (l == NULL) 
            continue;

         mlarrptr = get_mlarrptr(layer);

         // if no graphic class, get it unless there is i.e a ROUTING_KEEPOUT in a component defintion
         if (np->getGraphicClass() != 0)
            layerclass = get_allegro_class_from_camcad(np->getGraphicClass());
         else
            layerclass = layclass;

         lindex = get_class_subclass(l, layerclass);

         if (lindex < 0)
         {
            // here need to update maplay class if a new class was defined, not otherwise.
            if (!in_maperror(l, lclasses[layerclass]))
            {
               fprintf(flog, "Layer [%s] Class [%s] CAMCAD Class [%s] not found -> (update allegro.out)\n", 
                  l, lclasses[layerclass], graphicClassTagToDisplayString(np->getGraphicClass()));
            }

            continue;
         }

         // the outline can also be the silkscreen !
         // PACKAGE_GEOMETRY PLACE_BOUND_TOP
         if (layclass == CLASS_PACKAGE_GEOMETRY)   // only do this in PCBGEOM write.
         {
            if (maplay[lindex].classtype == 2 &&!maplay[lindex].subclass.CompareNoCase("PLACE_BOUND_TOP"))
            {
               // this is written as an outline routine
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

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (poly->isHidden()) continue;

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

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL voided     = poly->isVoid();

               lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                  
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

                  // here deal with bulge
                  points[cnt] = p2;
                  cnt++;
               }

               if (closed && PolySelfIntersects(&poly->getPntList()))
               {
                  fprintf(flog, "Closed Polygon %ld has overlaps.\n", np->getEntityNumber());

                  display_error++;
                  polyFilled = FALSE;
                  closed = FALSE;
                  voided = FALSE;
               }

               add_polyline(wfp, points, cnt, polyFilled, closed, voided, lineWidth, lindex, mlarrptr, FALSE, "");
            }  // while

            free(points);
         }

         break;  // POLYSTRUCT
      case T_TEXT:
         {
            int textLines = 0;

            if ((textLines = np->getText()->getNumLines()) > 1)
            {
               // Get the upper left extext X and Y
               CPoint2d ULExtent = np->getText()->getTextBox(doc->getSettings().getTextSpaceRatio()).getUL();
               Point2 ULPoint;

               if (np->getText()->getHorizontalPosition() == horizontalPositionLeft)         
                  ULPoint.x = np->getText()->getPnt().x;                   // left
               else if (np->getText()->getHorizontalPosition() == horizontalPositionCenter)     
                  ULPoint.x = (np->getText()->getPnt().x - ULExtent.x/2);  // center
               else if (np->getText()->getHorizontalPosition() == horizontalPositionRight)      
                  ULPoint.x = (np->getText()->getPnt().x - ULExtent.x);    // right

               if (np->getText()->getVerticalPosition() == verticalPositionBaseline)         
                  ULPoint.y = (np->getText()->getPnt().y + ULExtent.y);    // bottom
               else if (np->getText()->getVerticalPosition() == verticalPositionCenter)      
                  ULPoint.y = (np->getText()->getPnt().y + ULExtent.y/2);  // center
               else if (np->getText()->getVerticalPosition() == verticalPositionTop)      
                  ULPoint.y = np->getText()->getPnt().y;                   // top

               int curPos = 0;
               int lineCount = 1;
               CString textString = np->getText()->getText();
               CString text = textString.Tokenize("\n", curPos);

               while (!text.IsEmpty())
               {
                  point2.x = ULPoint.x * scale;
                  point2.y = (ULPoint.y + (np->getText()->getHeight() * (lineCount - 1))) * scale;
                  TransPoint2(&point2, 1, &m, insert_x, insert_y);

                  double text_rot = rotation + np->getText()->getRotation();
                  int text_mirror = np->getText()->isMirrored();

                  add_text( wfp, text, point2.x, point2.y, np->getText()->getHeight() * scale, 
                           np->getText()->getWidth() * scale, text_rot, text_mirror, np->getText()->getHorizontalPosition(), 
                           np->getText()->getVerticalPosition(), lindex);

                  // Get next text
                  text = textString.Tokenize("\n", curPos).Trim();
                  lineCount++;
               }
            }
            else
            {
               point2.x = np->getText()->getPnt().x * scale;
               point2.y = np->getText()->getPnt().y * scale;
               CString text = np->getText()->getText();
               double text_rot = rotation + np->getText()->getRotation();
               int text_mirror = np->getText()->isMirrored();

               add_text(wfp, text, point2.x, point2.y, np->getText()->getHeight() * scale, 
                        np->getText()->getWidth() * scale, text_rot, text_mirror, np->getText()->getHorizontalPosition(), 
                        np->getText()->getVerticalPosition(), lindex);
            }
         }

         break;
      case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)   continue;

            if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)   continue;

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
            }
            else // not aperture
            {
               // here we should use generic parts
               int block_layer = -1;

               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               if (hierachiemarker)
               {
                  lindex = get_class_subclass("OUTLINE", CLASS_DRAWING_FORMAT);

                  if (lindex < 0)
                  {
                     if (!in_maperror("OUTLINE", lclasses[CLASS_DRAWING_FORMAT]))
                     {
                        fprintf(flog, "Layer [%s] Class [%s] not found -> (update allegro.out)\n", 
                           "OUTLINE", lclasses[CLASS_DRAWING_FORMAT]);
                     }
                  }

                  // CLASS_DRAWING_FORMAT
                  add_text( wfp, AllegroNameMgr->GetName(block), point2.x, point2.y,
                      REFDES_HEIGHT * mils_2_units, REFDES_HEIGHT * mils_2_units,
                      block_rot, block_mirror, 0, 0, lindex);
               }
               else
               {
                  ALLEGRO_WriteData(wfp, &(block->getDataList()), 
                            point2.x, point2.y,
                            block_rot, block_mirror,
                            scale * np->getInsert()->getScale(), block_layer, layclass, hierachiemarker);
               }
            } // end else not aperture
         } // case INSERT

         break;
      } // end switch
   } // end for

   return 0;
} /* end ALLEGRO_WriteData */

//--------------------------------------------------------------
static   int   get_allegro_class_from_camcad(int cc_class)
{
   switch(cc_class)
   {
      case  graphicClassAnalysis:   
         return   CLASS_BOARD_GEOMETRY;
      case  GR_CLASS_ANTI_ETCH:
         return   CLASS_ANTI_ETCH;
      case  GR_CLASS_BOARD_GEOM:
         return   CLASS_BOARD_GEOMETRY;
      case  GR_CLASS_DRAWING:
         return   CLASS_DRAWING_FORMAT;
      case  graphicClassDrc:
         return   CLASS_BOARD_GEOMETRY;
      case  GR_CLASS_ETCH:
         return   CLASS_ETCH;
      case  graphicClassManufacturing:
         return   CLASS_MANUFACTURING;
      case  GR_CLASS_ROUTKEEPIN:
         return   CLASS_ROUTE_KEEPIN;
      case  GR_CLASS_ROUTKEEPOUT:
         return   CLASS_ROUTE_KEEPOUT;
      case  GR_CLASS_PLACEKEEPIN:
         return   CLASS_PACKAGE_KEEPIN;   
      case  GR_CLASS_PLACEKEEPOUT:
         return   CLASS_PACKAGE_KEEPOUT;  
      case  GR_CLASS_VIAKEEPIN:
         return   CLASS_DRAWING_FORMAT;   
      case  GR_CLASS_VIAKEEPOUT:
         return   CLASS_VIA_KEEPOUT;   
      case  GR_CLASS_ALLKEEPIN:
         return   CLASS_DRAWING_FORMAT;   
      case  GR_CLASS_ALLKEEPOUT:
         return   CLASS_DRAWING_FORMAT;   
      case  GR_CLASS_BOARDOUTLINE:
         return   CLASS_BOARD_GEOMETRY;   
      case  GR_CLASS_COMPOUTLINE:
         return   CLASS_PACKAGE_GEOMETRY; 
      case  GR_CLASS_PANELOUTLINE:
         return   CLASS_BOARD_GEOMETRY;   
      case  GR_CLASS_MILLINGPATH:
         return   CLASS_BOARD_GEOMETRY;   
      case  GR_CLASS_UNROUTE:
         return   CLASS_DRAWING_FORMAT;   
      case  GR_CLASS_TRACEFANOUT:
         return   CLASS_DRAWING_FORMAT;   
   }

   return   CLASS_UNKNOWN;
}

//--------------------------------------------------------------
int ALLEGRO_WriteBrdGraphicData(FILE *wfp, FILE *ffp, FILE *pfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, int layerclass)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   int   lindex;     // maplayer index;
   int   mlarrptr;   // pointer into mlArr
   int   write_routes;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
            layer = np->getLayerIndex();
         else 
            layer = insertLayer;

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         const char  *l = Layer_ALLEGRO(layer);

         if (l == NULL) 
            continue;

         mlarrptr = get_mlarrptr(layer);

         // yes boardoutline
         // if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE) 


         // no route keepin - only one allowed and done in easy ...
         if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)   continue;

         // no place keepin
         if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)  continue;
   
         // here assign graphic classes - only one allowed and done in easy...
         if (np->getGraphicClass() == 0)
            layerclass = CLASS_BOARD_GEOMETRY;
         else
            layerclass = get_allegro_class_from_camcad(np->getGraphicClass());

         lindex = get_class_subclass(l, layerclass);

         if (lindex < 0)   
         {
            if (!in_maperror(l, lclasses[layerclass]))
            {
               fprintf(flog, "Write BOARD data: Layer [%s] Class [%s] not found -> (update allegro.out)\n", 
                  l, lclasses[layerclass]);
            }
            continue;
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
            Attrib *a = NULL;
            CString av;

            if (np->getGraphicClass() == GR_CLASS_ETCH)
               write_routes = TRUE;
            else
               write_routes = FALSE;

            if (write_routes && writeBoundary)
            {
               a = is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 0);

               if (a != NULL)
                  av = get_attvalue_string(doc, a);

               a = NULL;
            }

            // count thru polys
            int cnt = 0;
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (!poly->isHidden() || (poly->isFloodBoundary() && writeBoundary))
               {
                  cnt += poly->getPntList().GetCount();
               }
            }

            Point2 *points = NULL;
            
            // Case dts0100499849 - Was allocating for cnt=0, and check for "keeper" poly was not consistent
            // in counting section above and processing section below, caused bad mem references and crash.

            if (cnt > 0)
            {
               points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            }

            if (points != NULL)  // make sure calloc worked
            {
               // loop thru polys
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);

                  if (!poly->isHidden() || (poly->isFloodBoundary() && writeBoundary))
                  {
                     BOOL polyFilled = poly->isFilled();
                     BOOL closed     = poly->isClosed();
                     BOOL voided     = poly->isVoid();

                     if (poly->isFloodBoundary() && writeBoundary)  
                     {
                        polyFilled = TRUE;
                        closed = TRUE;
                     }

                     lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;

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

                        // here deal with bulge
                        points[cnt] = p2;
                        cnt++;
                     }

                     // if it is a boundary poly and we are writing boundaries to autoPour do not check for 
                     // self intersect. If we are writing boundaries to autoPour and the poly has a width
                     // do not check for self intersect. Otherwise check for self intersecting and report errors.
                     if (!(poly->isFloodBoundary() && writeBoundary)||!(writeBoundary && lineWidth > SMALLNUMBER))
                     {
                        if (closed && PolySelfIntersects(&poly->getPntList()))
                        {
                           fprintf(flog, "Closed Polygon %ld has overlaps.\n", np->getEntityNumber());
                           display_error++;
                           polyFilled = FALSE;
                           closed = FALSE;
                           voided = FALSE;
                        }
                     }

                     if ((writeBoundary) && (poly->isFloodBoundary()))
                        add_polyline(pfp, points, cnt, polyFilled, closed, voided, lineWidth, lindex, mlarrptr, write_routes, av);
                     else if (write_routes && polyFilled)
                        add_polyline(ffp, points, cnt, polyFilled, closed, voided, lineWidth, lindex, mlarrptr, write_routes, "");
                     else
                        add_polyline(wfp, points, cnt, polyFilled, closed, voided, lineWidth, lindex, mlarrptr, write_routes, "");
                  }

               }  // while

               free(points);
            }
         }

         break;  // POLYSTRUCT

      case T_TEXT:
         {
            int textLines = 0;

            if ((textLines = np->getText()->getNumLines()) > 1)
            {
               // Get the upper left extext X and Y
               CPoint2d ULExtent = np->getText()->getTextBox(doc->getSettings().getTextSpaceRatio()).getUL();
               Point2 ULPoint;

               if (np->getText()->getHorizontalPosition() == horizontalPositionLeft)         
                  ULPoint.x = np->getText()->getPnt().x;                   // left
               else if (np->getText()->getHorizontalPosition() == horizontalPositionCenter)     
                  ULPoint.x = (np->getText()->getPnt().x - ULExtent.x/2);  // center
               else if (np->getText()->getHorizontalPosition() == horizontalPositionRight)      
                  ULPoint.x = (np->getText()->getPnt().x - ULExtent.x);    // right

               if (np->getText()->getVerticalPosition() == verticalPositionBaseline)         
                  ULPoint.y = (np->getText()->getPnt().y + ULExtent.y);    // bottom
               else if (np->getText()->getVerticalPosition() == verticalPositionCenter)      
                  ULPoint.y = (np->getText()->getPnt().y + ULExtent.y/2);  // center
               else if (np->getText()->getVerticalPosition() == verticalPositionTop)      
                  ULPoint.y = np->getText()->getPnt().y;                   // top

               int curPos = 0;
               int lineCount = 1;
               CString textString = np->getText()->getText();
               CString text = textString.Tokenize("\n", curPos);

               while (!text.IsEmpty())
               {
                  point2.x = ULPoint.x * scale;
                  point2.y = (ULPoint.y + np->getText()->getHeight() * lineCount) * scale;
                  TransPoint2(&point2, 1, &m, insert_x, insert_y);

                  double text_rot = rotation + np->getText()->getRotation();
                  int text_mirror = np->getText()->isMirrored();

                  add_text( wfp, text, point2.x, point2.y, np->getText()->getHeight() * scale, 
                           np->getText()->getWidth() * scale, text_rot, text_mirror, np->getText()->getHorizontalPosition(), 
                           np->getText()->getVerticalPosition(), lindex);

                  // Get next text
                  text = textString.Tokenize("\n", curPos).Trim();
                  lineCount++;
               }
            }
            else
            {
               point2.x = np->getText()->getPnt().x * scale;
               point2.y = np->getText()->getPnt().y * scale;
               CString text = np->getText()->getText();
               double text_rot = rotation + np->getText()->getRotation();
               int text_mirror = np->getText()->isMirrored();

               add_text(wfp, text, point2.x, point2.y, np->getText()->getHeight() * scale, 
                        np->getText()->getWidth() * scale, text_rot, text_mirror, np->getText()->getHorizontalPosition(), 
                        np->getText()->getVerticalPosition(), lindex);
            }
         }

         break;
      case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)      continue;
            if (np->getInsert()->getInsertType() == insertTypePcbComponent)    continue;
            if (np->getInsert()->getInsertType() == insertTypeTestPoint)       continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)  continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCB)            continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)            continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       continue;

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
            }
            else // not aperture
            {
               // here we should use generic parts
               int block_layer = -1;

               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               ALLEGRO_WriteBrdGraphicData(wfp, ffp, pfp, &(block->getDataList()), 
                            point2.x, point2.y,
                            block_rot, block_mirror,
                            scale * np->getInsert()->getScale(), block_layer, layerclass);
            } // end else not aperture
         } // case INSERT

         break;
      } // end switch
   } // end for

   return 0;
} /* end ALLEGRO_WriteBrdGraphicData */

//**********************************************************************************

CString CAllegroNameManager::GetName(BlockStruct *block)
{
   // Make sure names are unique and non-blank.
   // Generate names for non-unique or blank.

   if (block != NULL)
   {
      CString blockNumStr;
      blockNumStr.Format("%d", block->getBlockNumber());
      CString blockName (block->getName() );

      // If we've seen this block before just return the approved name
      CString approvedName;
      if (m_mapBlkNumToName.Lookup(blockNumStr, approvedName))
         return approvedName;

      // If name is blank, generate a name.
      // Generated name is based on block number, so allegedly guaranteed to be unique since there is
      // supposed to be only one block with given number.
      if (blockName.IsEmpty())
         blockName.Format("GEOM%d", block->getBlockNumber());

      // If we've seen this name before, is this the same block?  (handle dup names)
      int existingBlockNum = -1;
      if (m_mapNameToBlkNum.Lookup(blockName, existingBlockNum))
      {
         if (existingBlockNum != block->getBlockNumber())
            blockName.Format("GEOM%d", block->getBlockNumber());
      }

      // We know we haven't seen this block before, it was not in block number map.
      // If the name was blank or dup we have reset to unique name based on block number.
      // Add to both maps.
      m_mapNameToBlkNum.SetAt(blockName, block->getBlockNumber());
      m_mapBlkNumToName.SetAt(blockNumStr, blockName);

      return blockName;
   }

   // No block, no name
   return "";
}



/****************************************************************************/
/*
   end ALLEGOUT.CPP
*/
/****************************************************************************/



