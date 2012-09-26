
/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994 - 2011. All Rights Reserved.

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
#include "extents.h"
#include <direct.h>
#include <math.h>
#include <float.h>
#include "ck.h"
#include "outline.h"
#include "hp5dx.h"
#include "find.h"
#include "drc.h"
#include "hp5dx_o.h"
#include "hashunit.c" // hp hash routine. Supplied by HP and unmodified.
#include "centroid.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "PolyLib.h"

extern CString LogReaderProgram; // In camcad.cpp

extern CProgressDlg *progress;
extern char          *testaccesslayers[];


static   HP5DXLayerArray mlArr;
static   int         maxArr = 0;
static   int         max_signallayer = 0;

static   HP5DXF_PackageMapArray  packagemapArr;
static   int                     maxpackagemapArr;

static   HP5DXF_MaterialArray    materialarray;
static   int                     materialcnt;

static   CCEtoODBDoc    *doc;
static   double         unitsFactor;
static   double         panel_width, panel_height;

static   CompInstArray  compinstarray; 
static   int            compinstcnt;

static   CompPinInstArray  comppininstarray; 
static   int            comppininstcnt;

static   ViaDrcArray    viadrcarray;   
static   int            viadrccnt;

static   Point2         maxpanelsize;
static   Point2         maxboardsize;

static bool ExportHasRenamedPins = false;

static   int   display_error;
static   FILE  *flog;

static   HP5DXPCBList pcblist[MAX_BOARDS];
static   int         pcblistcnt;

static   Boardname   boardnamearray[MAX_BOARDS];
static   int         boardnamearraycnt;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static   double      SMALLDIST;  // distance to zero on panel and board

static   PadstackArray     padstackarray;
static   int               padstackcnt;

static   double            board_thickness;
static   int               allow_alpha_pins;

static ApertureShapeTag    defaultApertureShapeTag;
static CString             defaultStageSpeed;

static int load_hp5dxsettings(const char *fname, int pageunits);
static void check_comps(CDataList *DataList, int *topcnt, int *botcnt, double *xmin, double *ymin, double *xmax, double *ymax);
static int collect_padstacks(double scale, int punits);
static int get_padstackindex(const char *p);
static int do_layerlist();
static int do_boardarray(FileStruct *file, int punits);
static int create_directories(const char *path, const char *hproot);
static int create_directory_idfile(const char *panelname, const char *path, const char *hproot);
static int create_panel_ndf(const char *panelname, const char *path, const char *hproot, int punits, int prot, 
      int pspeed, double thickness, const char *material);
static int create_landpat_ndf(const char *panelname, const char *path, const char *hproot, int punits);
static int  create_padgeom_ndf(const char *panelname, const char *path, const char *hproot, int punits);
static int create_board_ndf(int boardnamearrayindex, const char *panelname, const char *path, const char *hproot, int punits, 
      double thickness, int bottom, int orthogonal);
static int create_component_ndf(int boardnamearrayindex, const char *panelname, const char *path, const char *hpRoot, BOOL WritingBottom);
static void free_layerlist();
static void free_packagemap();
static void free_material();
static int padstack_name_already_written(int ptr, const char *n);

static int hp5dx_created(FILE *fp, bool writeBlankLineSeparator = true);

static HackedApertureMap globalHackedApertureMap;

/******************************************************************************
* HP5DX_WriteFiles
*
   create_panel_ndf()
   create_padgeom_ndf()

   create_board_ndf() // top
   create_board_ndf() // bottom

   create_component_ndf() // top
   create_component_ndf() // bottom
*/
void HP5DX_WriteFiles(const char *panelname, CCEtoODBDoc *Doc, FormatStruct *format, int page_units,
      double UnitsFactor, const char *directoryname)
{
   globalHackedApertureMap.empty();

   FileStruct  *file;

   doc = Doc;

   display_error = 0;
   ExportHasRenamedPins = false;

   compinstarray.SetSize(100,100);
   compinstcnt = 0;

   viadrcarray.SetSize(100,100);
   viadrccnt = 0;

   padstackarray.SetSize(100,100);
   padstackcnt = 0;

   // check units
   if (page_units != UNIT_INCHES && page_units != UNIT_MM && page_units != UNIT_MILS )
   {
      CString  tmp;
      tmp.Format("Page UNITS must be INCHES, MILS, MM");
      MessageBox(NULL, tmp,"Error Units", MB_OK | MB_ICONHAND);
      return;
   }

   // 10 mil is allowed
   SMALLDIST = 10.0 * Units_Factor(UNIT_MILS, page_units);
   unitsFactor = Units_Factor(page_units, UNIT_INCHES);

   CString hp5dxLogFile = GetLogfilePath("5dx.log");
   if ((flog = fopen(hp5dxLogFile, "wt")) == NULL) // rewrite filw
   {
      MessageBox(NULL, hp5dxLogFile, "Can not open Logfile !",  MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // Write log file header.
   fprintf(flog, "# ---------------------------------------\n");
   fprintf(flog, "# Agilent 5DX Export Log File\n");
   hp5dx_created(flog, false);
	fprintf(flog, "# ---------------------------------------\n\n\n");

   boardnamearraycnt = 0;

   CString nameCheckFile( getApp().getSystemSettingsFilePath("5dx.chk") );
   check_init(nameCheckFile);

   mlArr.SetSize(100,100);
   maxArr = 0;

   packagemapArr.SetSize(100,100);
   maxpackagemapArr = 0;

   materialarray.SetSize(100,100);
   materialcnt = 0;

   CString settingsFile( getApp().getExportSettingsFilePath("5dx.out") );
   load_hp5dxsettings(settingsFile, page_units);

   board_thickness = 0.065 * Units_Factor(page_units, UNIT_INCHES);  

   char  curdir[_MAX_PATH];

   /* Get the current working directory: */
   if( _getcwd( curdir, _MAX_PATH ) == NULL )   
   {
      CString  tmp;
      tmp.Format( "Problem get current directory");
      MessageBox(NULL, "Error Get Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   int OUTPUT_ERROR = FALSE;

// here find the viewable panel file.
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PANEL)   
      {
         max_signallayer = 0;
         if (do_layerlist() == 0)
         {
            OUTPUT_ERROR = TRUE;
            break;
         }
         
         if (max_signallayer < 2)
            max_signallayer = 2;

         if (!do_boardarray(file, page_units))
         {
            OUTPUT_ERROR = TRUE;
            break;
         }
      }
   }

   if (!OUTPUT_ERROR)
   {
      HP_BoardNames bnDlg;
      bnDlg.array = boardnamearray;
      bnDlg.maxArray = boardnamearraycnt;
      if (bnDlg.DoModal() == IDOK)
      {
         // update boardnamearray
         int i;
         for (i=0;i<bnDlg.maxArray;i++)
         {
            boardnamearray[i] = bnDlg.array[i];
            char fname[_MAX_PATH];
            char hname[_MAX_PATH];
            if (strlen(boardnamearray[i].topname))
            {
               strcpy(fname, boardnamearray[i].topname);
               HashName(fname, hname);
               boardnamearray[i].tophashname = hname;
            }
            if (strlen(boardnamearray[i].bottomname))
            {
               strcpy(fname, boardnamearray[i].bottomname);
               HashName(fname, hname);
               boardnamearray[i].bottomhashname = hname;
            }
         }
      }
      else
      {
         OUTPUT_ERROR = TRUE;
      }
   }

   char fname[_MAX_PATH];
   char hname[_MAX_PATH];

   if (!OUTPUT_ERROR)
   {
      HP_DirTree dtDlg;
      dtDlg.array = boardnamearray;
      dtDlg.maxArray = boardnamearraycnt;

      strcpy(fname, panelname);
   
      HashName(fname, hname); 
      // split
      dtDlg.panelHashName.Format("%s%s (%s)", directoryname, hname, panelname);
      if (dtDlg.DoModal() != IDOK)
      {
         OUTPUT_ERROR = TRUE;
      }
   }

	allow_alpha_pins = TRUE;
   HP_DeafultSettingsDlg dsDlg(defaultStageSpeed);
   if (!OUTPUT_ERROR)
   {
      dsDlg.page_units = page_units;
      dsDlg.m_nonorthogonal = TRUE;
      dsDlg.thickness = board_thickness;  
      dsDlg.materialarray = &materialarray;
      dsDlg.materialcnt = materialcnt;
      dsDlg.m_stageSpeed = dsDlg.validatedStageSpeed(defaultStageSpeed);

      if (dsDlg.DoModal() != IDOK)
      {
         OUTPUT_ERROR = TRUE;
      }
      else
      {
         allow_alpha_pins = (dsDlg.m_enableAlphaNumericePnNumbers != 0);
      }
   }

   if (!OUTPUT_ERROR)
   {
      CString material;
      material = dsDlg.material;

      // here update thickness and other stuff
      switch (dsDlg.m_units)
      {
      case 1:
         unitsFactor = Units_Factor(page_units, UNIT_MILS);
         break;
      case 2:
         unitsFactor = Units_Factor(page_units, UNIT_MM);
         break;
      default:
         unitsFactor = Units_Factor(page_units, UNIT_INCHES);
         break;
      }

      int orthogonal = !dsDlg.m_nonorthogonal;

      CWaitCursor wait;

      if (create_directories(directoryname, hname))
      {
         // here all directories are done.
         create_directory_idfile(fname, directoryname, hname);

         create_panel_ndf(panelname, directoryname, hname, 
               dsDlg.m_units, atoi(dsDlg.m_rotation), atoi(dsDlg.m_stageSpeed), atof(dsDlg.m_boardThickness), material);

         update_smdrule(doc, ComponentSMDrule); // geoms and components

         // make a vias drc list which padstack, layer top/bottom, vianame, shape, x/ysize


         // here do landpat.ndf and padgeom.ndf file
         create_landpat_ndf(panelname, directoryname, hname, dsDlg.m_units);
         create_padgeom_ndf(panelname, directoryname, hname, dsDlg.m_units);

         // here write top board hash board.ndf
         // here write top board hash componen.mdf
         // here write bottom board hash board.ndf
         // here write bottom board hash componen.mdf
         for (int i=0; i<boardnamearraycnt; i++)
         {
            create_board_ndf(i,panelname, directoryname, hname, dsDlg.m_units, atof(dsDlg.m_boardThickness), 0, orthogonal);  // top
            create_board_ndf(i,panelname, directoryname, hname, dsDlg.m_units, atof(dsDlg.m_boardThickness), 1, orthogonal);  // bottom

            HP_PackageMap dlg;
            dlg.array = &packagemapArr;
            dlg.maxArray = maxpackagemapArr;
            if (dlg.DoModal() != IDOK)
               break;

            create_component_ndf(i, panelname, directoryname, hname, 0); // top
            create_component_ndf(i, panelname, directoryname, hname, 1); // bottom
         }
      }
   }

   // Add blank line to report to separate what came before from what
   // check_report might add.
   fprintf(flog, "\n");
   if (check_report(flog))
   {
      display_error++;
   }

   check_deinit();

   for (int i=0;i<compinstcnt;i++)
      delete compinstarray[i];
   compinstarray.RemoveAll();

   for (int i=0;i<viadrccnt;i++)
      delete viadrcarray[i];
   viadrcarray.RemoveAll();

   for (int i=0;i<padstackcnt;i++)
      delete padstackarray[i];
   padstackarray.RemoveAll();

   fclose(flog);

   free_layerlist();
   free_packagemap();
   free_material();

   globalHackedApertureMap.empty();

   // set it back to the current directory.
   _chdir( curdir );

   
   CString oldLogReaderProgram( LogReaderProgram );

   // If some pins are renamed then force logreader setting to notepad is not otherwise set.
   if (ExportHasRenamedPins)
   {
      if (LogReaderProgram.IsEmpty() || LogReaderProgram.CompareNoCase("NONE") == 0)
         LogReaderProgram = "notepad";
   }

   if (display_error || ExportHasRenamedPins)
      Logreader(hp5dxLogFile);

   LogReaderProgram = oldLogReaderProgram;
}

/******************************************************************************
* hp5dx_units
*/
static int hp5dx_units(FILE *fp, int punits)
{
   CString  u;

   // 0=Inches, 1=Mils, 2=mm
   switch (punits)
   {
      case 1:
         u = "mils";
      break;
      case 2:
         u = "millimeters";
      break;
      default: 
         u = "inches";
      break;
   }

   fprintf(fp,".UNIT: %s\n",u);
   return TRUE;

}

/******************************************************************************
* hp5dx_created
*/
static int hp5dx_created(FILE *fp, bool writeBlankLineSeparator)
{
   CTime t;
   t = t.GetCurrentTime();
   fprintf(fp,"# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(fp,"# Created : %s\n",t.Format("%A, %B %d, %Y at %H:%M:%S"));
   if (writeBlankLineSeparator)
      fprintf(fp,"\n");

   return TRUE;
}

/******************************************************************************
* get_component
*/
static int get_component(const char *cname)
{
   int   i;

   for (i=0;i<compinstcnt;i++)
   {
      if (!compinstarray[i]->compname.CompareNoCase(cname))
         return i;
   }

   HP5DXCompInst *c = new HP5DXCompInst;
   compinstarray.SetAtGrow(compinstcnt,c);  
   compinstcnt++; 

   c->compname = cname;
   c->blocknum = -1;

   return compinstcnt-1;
}

/******************************************************************************
* do_layerlist
*/
static int do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0;
   char        typ = 'D';
   int         toplayer, bottomlayer;

   toplayer = 0;
   bottomlayer = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.

      signr = 0;

      typ = 'D';

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
      }

      // now here look for layer type
      if (layer->getLayerType() == LAYTYPE_PAD_ALL || layer->getLayerType() == LAYTYPE_SIGNAL_ALL)
      {
         signr = LAY_ALL;
         toplayer = bottomlayer = 1;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_OUTER)   // outer and all is the same for 5dx
      {
         signr = LAY_ALL;
         toplayer = bottomlayer = 1;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         signr = LAY_INNER;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP || layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
      {
         signr = LAY_TOP;
         toplayer = 1;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM || layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
      {
         signr = LAY_BOT;
         max_signallayer = layer->getElectricalStackNumber();
         bottomlayer = 1;
      }

      if (signr == 0)
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
      }

      // here check if layer->getName() is not already done

      //mlArr.SetSizes
      HP5DXLayerStruct *ml = new HP5DXLayerStruct;
      ml->stackNum = signr;
      ml->layerindex = j;
      ml->on = TRUE;
      ml->type = typ;
      ml->Name = layer->getName();
      mlArr.SetAtGrow(maxArr++, ml);
   }

   if (!toplayer)
   {
      ErrorMessage("No TOP or COMPONENT PAD Layer found !", "Setup Error : Can not continue");
      return 0;
   }

   if (!bottomlayer)
   {
      ErrorMessage("No BOTTOM or SOLDERLAYER PAD Layer found !", "Setup Error : Can not continue");
      return 0;
   }


   return 1;
}

/******************************************************************************
* free_layerlist
*/
static void free_layerlist()
{
   for (int i=0; i<maxArr; i++)
      delete mlArr[i];
   mlArr.RemoveAll();
   maxArr = 0;
}

/******************************************************************************
* free_packagemap
*/
static void free_packagemap()
{
   for (int i=0; i<maxpackagemapArr; i++)
      delete packagemapArr[i];
   packagemapArr.RemoveAll();
   maxpackagemapArr = 0;
}

/******************************************************************************
* free_material
*/
static void free_material()
{
   for (int i=0; i<materialcnt; i++)
      delete materialarray[i];
   materialarray.RemoveAll();
   materialcnt = 0;
}

/******************************************************************************
* GetStackNumFromIndex
*/
static int GetStackNumFromIndex(int index)
{
   for (int i=0; i<maxArr; i++)
   {
      if (mlArr[i]->layerindex == index)
         return mlArr[i]->stackNum;
   }

   return -99;
}

/******************************************************************************
* cnv_units
// converts from actual to HP units.
*/
static long cnv_units(double x)
{
   long l;
   l = (long)floor(x * unitsFactor + 0.5);
   return l;
}

/******************************************************************************
* write_attributes
*/
static void write_attributes(FILE *fp,CMapWordToPtr *map)
{
   if (map == NULL)
      return;

   WORD keyword;
   void *voidPtr;
   Attrib   *a;

   POSITION pos = map->GetStartPosition();
   while (pos != NULL)
   {
      map->GetNextAssoc(pos, keyword, voidPtr);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))   continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))      continue;

      a = (Attrib*)voidPtr;
      switch (a->getValueType())
      {
         case VT_INTEGER:
            fprintf(fp,"! ATTRIBUTE %s %d\n", doc->getKeyWordArray()[keyword]->out, a->getIntValue());
         break;
         case VT_UNIT_DOUBLE:
         case VT_DOUBLE:
            fprintf(fp,"! ATTRIBUTE %s %lg\n", doc->getKeyWordArray()[keyword]->out, a->getDoubleValue());
         break;
         case VT_STRING:
         {
            char *tok, *temp = STRDUP(a->getStringValue());
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               if (a->getStringValueIndex() != -1)
                  fprintf(fp,"! ATTRIBUTE %s %s\n", doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
               else
                  fprintf(fp,"! ATTRIBUTE %s ?\n", doc->getKeyWordArray()[keyword]->out);
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   return;
}

/******************************************************************************
* get_package_id
*/
static int get_package_id(HP5DXF_PackageMap a)
{
   for (int i=0; i<maxpackagemapArr; i++)
   {
      if (packagemapArr[i]->geomname == a.geomname &&
          packagemapArr[i]->prefix == a.prefix)
         return i;
   }

   return -1;
}

/******************************************************************************
* load_hp5dxsettings
*/
static int load_hp5dxsettings(const char *fname, int pageunits)
{
   ComponentSMDrule = 0;
   maxpanelsize.x = FLT_MAX;
   maxpanelsize.y = FLT_MAX;
   maxboardsize.x = FLT_MAX;
   maxboardsize.y = FLT_MAX;
   defaultApertureShapeTag = apertureRound;
   defaultStageSpeed = "2";  // Backward compatible, this was the constant value elswhere before this setting was implemented.

   FILE *fSet = fopen(fname, "rt");
   if (fSet == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      MessageBox(NULL, tmp,"HP5DX Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   char line[255];
   long lcnt = 0;
   while (fgets(line, 255, fSet))
   {     
      char *tok;
      if ((tok = strtok(line, " \t\n")) == NULL)
         continue;

      if (tok[0] == '.')
      {
			if (!STRCMPI(tok, ".ComponentSMDrule"))
         {
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            ComponentSMDrule = atoi(tok);
         }
         else if (!STRCMPI(tok, ".MAXBOARDSIZE"))
         {
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            maxboardsize.x = atof(tok);
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            maxboardsize.y = atof(tok);
         }
         else if (!STRCMPI(tok, ".MAXPANELSIZE"))
         {
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            maxpanelsize.x = atof(tok);
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            maxpanelsize.y= atof(tok);
         }
         else if (!STRCMPI(tok, ".PackageId"))
         {
            HP5DXF_PackageMap packageMap;

            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            packageMap.geomname = tok;

            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            packageMap.prefix = tok;

            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            packageMap.package_id = tok;

            packageMap.used = 0;
            packageMap.changed = 0;

            if (get_package_id(packageMap) < 0)
            {
               HP5DXF_PackageMap *newPackageMap = new HP5DXF_PackageMap;
               newPackageMap->geomname = packageMap.geomname;
               newPackageMap->prefix = packageMap.prefix;
               newPackageMap->package_id = packageMap.package_id;
               newPackageMap->used = packageMap.used;
               newPackageMap->changed = packageMap.changed;
               packagemapArr.SetAtGrow(maxpackagemapArr++, newPackageMap);
            }
         }
         else if (!STRCMPI(tok, ".MATERIAL"))
         {
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            HP5DXF_Material *material = new HP5DXF_Material;
            material->name = tok;
            materialarray.SetAtGrow(materialcnt++, material);
         }
         else if (!STRCMPI(tok, ".DEFAULT_SHAPE") || !STRCMPI(tok, ".DEFAULTSHAPE"))
         {
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;
            // Circle or Rectangle, C or R is what is checked. Really, just R, all else is C.

            defaultApertureShapeTag = (toupper(tok[0]) == 'R') ? apertureRectangle : apertureRound;
         }
         else if (!STRCMPI(tok, ".STAGE_SPEED") || !STRCMPI(tok, ".STAGESPEED"))
         {
            if ((tok = strtok(NULL, " \t\n")) == NULL)
               continue;

            defaultStageSpeed = tok;
         }
         
      }
   }

   fclose(fSet);

   return 1;
}

/******************************************************************************
* in_boardarray
   Check if a board is already part of a panel
*/
static int in_boardarray(int geomnum)
{
   int   i;
   
   for (i=0;i<boardnamearraycnt;i++)
   {
      if (boardnamearray[i].geomnumber == geomnum)
         return 1;
   }

   return 0;
}

/******************************************************************************
* already_in_viadrcarray
*/
static int already_in_viadrcarray(long ent, int layer)
{
   int   i;

   for (i=0;i<viadrccnt;i++)
   {
      if (viadrcarray[i]->entitynum == ent && viadrcarray[i]->layer == layer)
         return 1;
   }
   return 0;
}

/******************************************************************************
* get_5dxlayer_from_layertype
*/
static int get_5dxlayer_from_layertype(int layertype)
{
   if (layertype == LAYTYPE_SIGNAL_TOP)   return -1;
   if (layertype == LAYTYPE_SIGNAL_BOT)   return -2;
   if (layertype == LAYTYPE_PAD_TOP)      return -1;
   if (layertype == LAYTYPE_PAD_BOTTOM)   return -2;

   return 0;
}

/******************************************************************************
* load_viaattrib_array
// this function loads vias which are marked with the ATT_VIATEST5DX attribute
*/
static void load_viaattrib_array(FileStruct *f)
{
   DataStruct *np;

   POSITION pos = f->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = f->getBlock()->getDataList().GetNext(pos);
      
      // if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_POLY:

         break;
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int   layer = -4; // initialize to all layers (if no argument)
            Attrib *a;
            if (a = is_attvalue(doc, np->getAttributesRef(), ATT_5DX_VIATEST, 2))   
            {
               CString l = get_attvalue_string(doc, a);
               if (!l.CompareNoCase("BOTH"))
                  layer = -4;
               else
               if (!l.CompareNoCase("TOP"))
                  layer = -1;
               else
               if (!l.CompareNoCase("BOTTOM"))
                  layer = -2;
               else
               if (!l.CompareNoCase("NONE"))
                  break;

               if (TRUE)
               {
                  HP5DXViaDrcList *c = new HP5DXViaDrcList;
                  viadrcarray.SetAtGrow(viadrccnt,c);  
                  viadrccnt++;

                  CString  name;
                  name.Format("TPorVia%d", viadrccnt);

                  // attribute must be append, because one via could have a problem on multiple layers
                  doc->SetUnknownAttrib(&np->getAttributesRef(),"5DX_VIANAME", name, SA_OVERWRITE, NULL); //  

                  BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

                  // -1 is top
                  // -2 is bottom

                  c->entitynum = np->getEntityNumber();
                  c->layer = layer;
                  c->xloc = np->getInsert()->getOriginX();
                  c->yloc = np->getInsert()->getOriginY();
                  c->padstackname = (block ? block->getName() : "");
                  c->vianame = name;
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end load_viaattrib_array */
   return;
}

/******************************************************************************
* load_viadrc_array
   DRC_CODE_PINtoPIN    drc_double1 = distance between 2 pins
   DRC_CODE_PINtoVIA    drc_double1 = distance between a pin and a via
                                       entity 1 is the pin,  entity 2 is the via
   DRC_CODE_VIAtoVIA    drc_double1 = distance between 2 vias and features
*/
static int load_viadrc_array(FileStruct *f)
{

   POSITION    drcPos;
   DRCStruct   *drc;

   //max_signallayer

   drcPos = f->getDRCList().GetHeadPosition();
   while (drcPos != NULL)
   {
      drc = f->getDRCList().GetNext(drcPos);

      if (drc->getAlgorithmType() == DRC_ALG_PROBABLESHORT_PINtoFEATURE)
      {

         CDataList *dataList;
         DataStruct *d    = FindDataEntity(doc, drc->getInsertEntityNumber(), &dataList, NULL);
         if (d == NULL) continue;

         DataStruct *data = FindDataEntity(doc, ((DRC_MeasureStruct*)(drc->getVoidPtr()))->entity2, &dataList, NULL);
         if (data == NULL)
            continue;

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         if (!block)
            continue;

         Attrib *a;
         int layer = 0; // -1 == top or -2 == bottom
         if (a =  is_attvalue(doc, d->getAttributesRef(), ATT_DFM_LAYERTYPE, 2)) 
         {
            CString l = get_attvalue_string(doc, a);
            if (!l.CompareNoCase("SIGNAL TOP"))
               layer = -1;
            else if (!l.CompareNoCase("PAD TOP"))
               layer = -1;
            else if (!l.CompareNoCase("SIGNAL BOTTOM"))
               layer = -2;
            else if (!l.CompareNoCase("PAD BOTTOM"))
               layer = -2;
         }
         if (!layer)
            continue;

         if (data && !already_in_viadrcarray(((DRC_MeasureStruct*)(drc->getVoidPtr()))->entity2, layer))
         {
            HP5DXViaDrcList *c = new HP5DXViaDrcList;
            viadrcarray.SetAtGrow(viadrccnt,c);  
            viadrccnt++;

            CString  name;
            name.Format("TPorVia%d", viadrccnt);

            // attribute must be append, because one via could have a problem on multiple layers
            doc->SetUnknownAttrib(&data->getAttributesRef(),"5DX_VIANAME", name, SA_OVERWRITE, NULL); //  

            c->entitynum = data->getEntityNumber();
            c->layer = layer;
            c->xloc = data->getInsert()->getOriginX();
            c->yloc = data->getInsert()->getOriginY();
            c->padstackname = block->getName();
            c->vianame = name;
         }  
      }
      else
      if (drc->getAlgorithmType() == DRC_ALG_PROBABLESHORT_FEATUREtoFEATURE)
      {
         CDataList *dataList;
         DataStruct *d = FindDataEntity(doc, drc->getInsertEntityNumber(), &dataList, NULL);
         if (d == NULL)
            continue;

         DataStruct *data = FindDataEntity(doc, ((DRC_MeasureStruct*)(drc->getVoidPtr()))->entity1, &dataList, NULL);
         if (data == NULL)
            continue;

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         if (!block)
            continue;

         Attrib *a;
         int layer = 0; // -1 == top or -2 == bottom
         if (a =  is_attvalue(doc, d->getAttributesRef(), ATT_DFM_LAYERTYPE, 2)) 
         {
            CString l = get_attvalue_string(doc, a);
            if (!l.CompareNoCase("SIGNAL TOP"))
               layer = -1;
            else
            if (!l.CompareNoCase("PAD TOP"))
               layer = -1;
            else
            if (!l.CompareNoCase("SIGNAL BOTTOM"))
               layer = -2;
            else
            if (!l.CompareNoCase("PAD BOTTOM"))
               layer = -2;
         }
         if (!layer) continue;

         if (data && !already_in_viadrcarray(((DRC_MeasureStruct*)(drc->getVoidPtr()))->entity1, layer))
         {
            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            HP5DXViaDrcList *c = new HP5DXViaDrcList;
            viadrcarray.SetAtGrow(viadrccnt,c);  
            viadrccnt++;

            CString  name;
            name.Format("TPorVia%d", viadrccnt);

            // attribute must be append, because one via could have a problem on multiple layers
            doc->SetUnknownAttrib(&data->getAttributesRef(), "5DX_VIANAME", name, SA_OVERWRITE, NULL);

            c->entitynum = data->getEntityNumber();
            c->layer = layer;
            c->xloc = data->getInsert()->getOriginX();
            c->yloc = data->getInsert()->getOriginY();
            c->padstackname = (block ? block->getName() : "");
            c->vianame = name;
         }

         data = FindDataEntity(doc, ((DRC_MeasureStruct*)(drc->getVoidPtr()))->entity2, &dataList, NULL);
         if (data == NULL)
            continue;

         block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         if (!block)
            continue;

         if (data && !already_in_viadrcarray(((DRC_MeasureStruct*)(drc->getVoidPtr()))->entity2, layer))
         {
            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            HP5DXViaDrcList *c = new HP5DXViaDrcList;
            viadrcarray.SetAtGrow(viadrccnt,c);  
            viadrccnt++;

            CString  name;
            name.Format("TPorVia%d", viadrccnt);

            // attribute must be append, because one via could have a problem on multiple layers
            doc->SetUnknownAttrib(&data->getAttributesRef(), "5DX_VIANAME", name, SA_OVERWRITE, NULL); //  

            c->entitynum = data->getEntityNumber();
            c->layer = layer;
            c->xloc = data->getInsert()->getOriginX();
            c->yloc = data->getInsert()->getOriginY();
            c->padstackname = (block ? block->getName() : "");
            c->vianame = name;
         }
      }
   }

   return 1;
}

/******************************************************************************
* do_boardarray
*/
static int do_boardarray(FileStruct *file, int punits)
{
   DataStruct *np;
   double      xmin, ymin, xmax, ymax;

   pcblistcnt = 0;

   xmin = ymin = FLT_MAX;
   xmax = ymax = -FLT_MAX;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);

      // found Panel Outline
      if (np->getGraphicClass() == GR_CLASS_PANELOUTLINE && np->getDataType() == T_POLY)
      {
         // get extents
         CPoly *poly = np->getPolyList()->GetHead();
         POSITION pntPos = poly->getPntList().GetHeadPosition();
         while (pntPos != NULL)
         {
            CPnt* pnt = poly->getPntList().GetNext(pntPos);
            if (pnt->x < xmin) xmin = pnt->x;
            if (pnt->x > xmax) xmax = pnt->x;
            if (pnt->y < ymin) ymin = pnt->y;
            if (pnt->y > ymax) ymax = pnt->y;
         }
/*
         if (xmax > xmin) 
         {
            m_xmin = xmin;
            m_xmax = xmax;
            m_ymin = ymin;
            m_ymax = ymax;
         }
         break;
*/
      }
      else
      {
         if (np->getDataType() != T_INSERT)        
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_PCB)   
         {
            if (pcblistcnt < MAX_BOARDS)
            {
               pcblist[pcblistcnt].refname = np->getInsert()->getRefname();             
               pcblist[pcblistcnt].geomnum = np->getInsert()->getBlockNumber();
               pcblist[pcblistcnt].x = np->getInsert()->getOriginX();
               pcblist[pcblistcnt].y = np->getInsert()->getOriginY();               
               pcblist[pcblistcnt].rotation = round(RadToDeg(np->getInsert()->getAngle()) / 90);             
               pcblist[pcblistcnt].mirror = np->getInsert()->getMirrorFlags();
               pcblistcnt++;
            }

            if (in_boardarray(np->getInsert()->getBlockNumber()))
               continue;

            collect_padstacks(1.0, punits);


            if (boardnamearraycnt < MAX_BOARDS)
            {
               boardnamearray[boardnamearraycnt].geomnumber = np->getInsert()->getBlockNumber();
               boardnamearray[boardnamearraycnt].boardname = np->getInsert()->getRefname();

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
               if (!block)
                  continue;
               boardnamearray[boardnamearraycnt].geomname = block->getName();   

               Attrib *a;
               if (a = is_attvalue(doc, block->getAttributesRef(), BOARD_THICKNESS, 2))   
               {
                  CString l = get_attvalue_string(doc, a);
                  board_thickness = atof(l) * unitsFactor;
               }

               FileStruct *f = doc->Find_File_by_BlockPtr(block);
               
               if (f)   
               {
                  load_viadrc_array(f);
                  load_viaattrib_array(f);
               }
               int comptop = 0, compbot = 0;

               double   xmin, ymin, xmax, ymax;
               check_comps(&(block->getDataList()), &comptop, &compbot, &xmin, &ymin, &xmax, &ymax);

               if (xmin > xmax)
               {
                  CString tmp;
                  tmp.Format("No BOARD Outline found for %s (%s)!", 
                     block->getName(), np->getInsert()->getRefname());
                  ErrorMessage(tmp);
                  return 0;
               }

               if (fabs(xmin) > SMALLDIST || fabs(ymin) > SMALLDIST)
               {
                  CString tmp;
                  tmp.Format("BOARD Outline for %s (%s) not at 0,0 -> (%lg, %lg)!", 
                     block->getName(), np->getInsert()->getRefname(), xmin, ymin);
                  ErrorMessage(tmp);
                  return 0;
               }

               CString  topname, botname;
               if (boardnamearraycnt)
               {
                  topname.Format("Primary%d",boardnamearraycnt+1);
                  botname.Format("Secondary%d",boardnamearraycnt+1);
               }
               else
               {
                  topname = "Primary";
                  botname = "Secondary";
               }
               if (comptop)
               {
                  boardnamearray[boardnamearraycnt].topname =  topname;
                  char fname[_MAX_PATH];
                  char hname[_MAX_PATH];
                  strcpy(fname, topname);
                  HashName(fname, hname);
                  boardnamearray[boardnamearraycnt].tophashname = hname;
               }
               if (compbot)
               {
                  char fname[_MAX_PATH];
                  char hname[_MAX_PATH];
                  strcpy(fname, botname);
                  HashName(fname, hname);             
                  boardnamearray[boardnamearraycnt].bottomname =  botname;
                  boardnamearray[boardnamearraycnt].bottomhashname = hname;
               }
               boardnamearray[boardnamearraycnt].xmin = xmin;
               boardnamearray[boardnamearraycnt].ymin = ymin;
               boardnamearray[boardnamearraycnt].xmax = xmax;
               boardnamearray[boardnamearraycnt].ymax = ymax;
               boardnamearraycnt++;
            }
            else
            {
               fprintf(flog,"Too many boards on a panel\n");
               display_error++;
            }
         }
      }
   }

   if (xmin > xmax)
   {
      ErrorMessage("No PANEL Outline found !");
      return 0;
   }

   if (fabs(xmin) > SMALLDIST || fabs(ymin) > SMALLDIST)
   {
      CString tmp;
      tmp.Format("PANEL Outline not at 0,0 -> (%lg, %lg)!", xmin, ymin);
      ErrorMessage(tmp);
      return 0;
   }

   panel_width = xmax - xmin; 
   panel_height= ymax - ymin;

   if (panel_width > maxpanelsize.x * Units_Factor(UNIT_INCHES,punits) || 
       panel_height > maxpanelsize.y  * Units_Factor(UNIT_INCHES,punits) )
   {
      CString tmp;
      tmp.Format("PANEL size [%2.3lf, %2.3lf] larger than MaxPanelSize [%2.3lf, %2.3lf]!", 
            panel_width, panel_height, maxpanelsize.x, maxpanelsize.y);
      MessageBox(NULL, tmp, "Warning Panel Size !", MB_ICONEXCLAMATION | MB_OK);
   }

   return 1;
}

/******************************************************************************
* create_directories
*/
static int create_directories(const char *path, const char *hproot)
{
   CString  hprootpath;
   hprootpath = path;
   hprootpath +=hproot;

   // need to start making the default directory
   // check drive
   // open file for writting
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( path, drive, dir, fname, ext );

   char *lp;
   lp = strtok(dir,"\\/");

   CString  builddir;
   builddir = "\\";
   builddir += lp;

   while (lp)
   {
      char  buildpath[_MAX_PATH];
      _makepath(buildpath,drive,builddir,"","");

      // see if it already exist
      if( _chdir( buildpath ) == 0  )
      {
         // dino does not care 
      }
      else
      {
         // we need to create directory one at a time
         if( _mkdir( buildpath ) != 0 )   
         {
            CString  tmp;
            tmp.Format( "Problem creating directory '%s'" , buildpath);
            MessageBox(NULL, "Error Creating Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
            return 0;
         }
      }

      if ((lp = strtok(NULL,"\\/")) != NULL)
      {
         builddir += "\\";
         builddir += lp;
      }
   }

   if( _chdir( hprootpath ) == 0  )
   {
      CString  tmp;
      tmp.Format( "The output directory '%s' already exists!\nUse existing path?" , hprootpath);
      if (ErrorMessage(tmp, "Directory Exists", MB_YESNO | MB_DEFBUTTON2)!=IDYES)
      {
         return 0;
      }
   }
   else
   if( _mkdir( hprootpath ) != 0 )   
   {

      CString  tmp;
      tmp.Format( "Problem creating directory '%s'" , hprootpath);
      MessageBox(NULL, "Error Creating Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   // here now build all primary and secondary directories
   int   i;

   for (i=0;i<boardnamearraycnt;i++)
   {
      CString  finalpath;

      if (strlen(boardnamearray[i].tophashname))
      {
         finalpath = hprootpath;
         finalpath += "\\";
         finalpath += boardnamearray[i].tophashname;

         // see if it already exist
         if( _chdir( finalpath ) == 0  )
         {
            // do nothing
         }
         else
         {
            // we need to create directory one at a time
            if( _mkdir( finalpath ) != 0 )   
            {  
               CString  tmp;
               tmp.Format( "Problem creating directory '%s'" , finalpath);
               MessageBox(NULL, "Error Creating Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
               return 0;
            }
         }
      }

      if (strlen(boardnamearray[i].bottomhashname))
      {
         finalpath = hprootpath;
         finalpath += "\\";
         finalpath += boardnamearray[i].bottomhashname;

         // see if it already exist
         if( _chdir( finalpath ) == 0  )
         {
            // do nothing
         }
         else
         {
            // we need to create directory one at a time
            if( _mkdir( finalpath ) != 0 )   
            {  
               CString  tmp;
               tmp.Format( "Problem creating directory '%s'" , finalpath);
               MessageBox(NULL, "Error Creating Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
               return 0;
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* create_directory_idfile
*/
static int create_directory_idfile(const char *panelname, const char *path, const char *hproot)
{
   CString  hprootpath;
   hprootpath = path;
   hprootpath +=hproot;

   FILE  *fp;
   int   i;

   CString  finalpath;

   finalpath = hprootpath;
   finalpath += "\\";
   finalpath += "dir_id.dat";

   // this is level 1 files 
   if ((fp = fopen(finalpath,"wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open File [%s]",finalpath);
      MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
   }
   else
   {
      fprintf(fp,"%s\n", panelname);
      fclose(fp);
   }

   for (i=0;i<boardnamearraycnt;i++)
   {
      if (strlen(boardnamearray[i].tophashname))
      {
         finalpath = hprootpath;
         finalpath += "\\";
         finalpath += boardnamearray[i].tophashname;
         finalpath += "\\";
         finalpath += "dir_id.dat";

         if ((fp = fopen(finalpath,"wt")) == NULL)
         {
            CString tmp;
            tmp.Format("Can not open File [%s]",finalpath);
            MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
         }
         else
         {
            fprintf(fp,"%s\n", boardnamearray[i].topname);
            fclose(fp);
         }
      }

      if (strlen(boardnamearray[i].bottomhashname))
      {
         finalpath = hprootpath;
         finalpath += "\\";
         finalpath += boardnamearray[i].bottomhashname;
         finalpath += "\\";
         finalpath += "dir_id.dat";

         if ((fp = fopen(finalpath,"wt")) == NULL)
         {
            CString tmp;
            tmp.Format("Can not open File [%s]",finalpath);
            MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
         }
         else
         {
            fprintf(fp,"%s\n", boardnamearray[i].bottomname);
            fclose(fp);
         }
      }
   }
   return 1;
}

/******************************************************************************
* create_panel_ndf
*/
static int create_panel_ndf(const char *panelname, const char *path, const char *hproot, int punits, int prot, 
      int pspeed, double thickness, const char *material)
{
   CString hprootpath = path;
   hprootpath += hproot;

   CString panelFilename = hprootpath + "\\" + "panel.ndf";

   FILE *fPanel = fopen(panelFilename, "wt");
   if (!fPanel)
   {
      CString tmp;
      tmp.Format("Can not open File [%s]", panelFilename);
      ErrorMessage(tmp, "", MB_OK | MB_ICONHAND);
      return 0;
   }

   
   fprintf(fPanel, "# PANEL.NDF\n");
   fprintf(fPanel, "# 5DX Panel Definition\n");
   fprintf(fPanel, "# Panel Name : %s\n", panelname);

   hp5dx_created(fPanel);

   fprintf(fPanel, ".SUBPANEL_ID: %s\n",panelname);
   
   hp5dx_units(fPanel, punits);
   fprintf(fPanel, ".ROTATION: %d\n", prot);
   fprintf(fPanel, ".STAGE_SPEED: %d\n", pspeed);

   if (STRCMPI(material,"None")) // if not NONE
      fprintf(fPanel, ".MATERIALS: %s\n", material);

   fprintf(fPanel, "\n");

   fprintf(fPanel, "# Dimensions X Size Y Size Thickness\n");
   fprintf(fPanel, ".DIMENSIONS: %2.3lf %2.3lf %2.3lf\n", panel_width * unitsFactor, panel_height * unitsFactor, thickness);

   fprintf(fPanel, "\n.COMPONENT_SAMPLE_PERCENT: 100\n\n");

   // here do panel board placement.
   fprintf(fPanel, "# Board 1            Side    Test?   X Loc    Y Loc   Rot  Board 2               X Loc   Y Loc   Rot\n");

   for (int i=0; i<boardnamearraycnt; i++)
   {
      double board_width = boardnamearray[i].xmax - boardnamearray[i].xmin;
      double board_height = boardnamearray[i].ymax - boardnamearray[i].ymin;

      if (board_width > maxboardsize.x  * Units_Factor(UNIT_INCHES,punits)  || 
          board_height > maxboardsize.y  * Units_Factor(UNIT_INCHES,punits) )
      {
         CString tmp;
         tmp.Format("BOARD size [%2.3lf, %2.3lf] larger than MaxBoardSize [%2.3lf, %2.3lf]!", 
               board_width, board_height, maxboardsize.x, maxboardsize.y);
         ErrorMessage(tmp, "", MB_ICONEXCLAMATION | MB_OK);
      }

      // now loop throu all pcblistcnt with the same geomnum
      for (int l=0; l<pcblistcnt; l++)
      {
         if (pcblist[l].geomnum != boardnamearray[i].geomnumber)  continue;
         fprintf(fPanel, "# %s\n",pcblist[l].refname);

         double   bd1x, bd1y, bd2x, bd2y;
         int      rot1, rot2;

         if (pcblist[l].mirror == 0)
         {
            switch (pcblist[l].rotation)
            {
            case 0:  // tested
               bd1x = pcblist[l].x * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 0;
               bd2x = (panel_width - board_width - pcblist[l].x) * unitsFactor;
               bd2y = bd1y;
               rot2 = 0;
               break;
            case 1:
               bd1x = pcblist[l].x * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 90;
               bd2x = (panel_width - pcblist[l].x) * unitsFactor;
               bd2y = (board_width + pcblist[l].y) * unitsFactor;
               rot2 = 270;
               break;
            case 2:
               bd1x = pcblist[l].x * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 180;
               bd2x = (panel_width - (pcblist[l].x - board_width)) * unitsFactor;
               bd2y = bd1y;
               rot2 = 180;
               break;
            case 3:
               bd1x = pcblist[l].x * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 270;
               bd2x = (panel_width - pcblist[l].x) * unitsFactor;
               bd2y = (pcblist[l].y - board_width) * unitsFactor;
               rot2 = 90;
               break;
            }

            if (strlen(boardnamearray[i].tophashname) && strlen(boardnamearray[i].bottomhashname))
            {
               // both sides
               fprintf(fPanel, "@%-19s %-7s %-5s %7.4lf  %7.4lf %5d  %-19s %7.4lf %7.4lf %4d\n",
                     boardnamearray[i].topname, "TOP", "TRUE", bd1x, bd1y, rot1, boardnamearray[i].bottomname, bd2x, bd2y, rot2);
            }
            else if (strlen(boardnamearray[i].tophashname) && strlen(boardnamearray[i].bottomhashname) == 0)
            {
               // top but not bottom
               fprintf(fPanel, "@%-19s %-7s %-5s %7.4lf  %7.4lf %5d  %-19s %7.4lf %7.4lf %4d\n",
                     boardnamearray[i].topname, "TOP", "TRUE", bd1x, bd1y, rot1, "none", 0.0, 0.0, 0);
            }
            else if (strlen(boardnamearray[i].tophashname) == 0 && strlen(boardnamearray[i].bottomhashname))
            {
               // no top but bottom
               fprintf(fPanel, "@%-19s %-7s %-5s %7.4lf  %7.4lf %5d  %-19s %7.4lf %7.4lf %4d\n",
                     boardnamearray[i].bottomname, "BOTTOM", "TRUE", bd1x, bd1y, rot1, "none", 0.0, 0.0, 0);
            }
            else
            {
               // none at all
            }
         }
         else
         {
            // all mirror
            switch (pcblist[l].rotation)
            {
            case 0:  // tested
               bd1x = (panel_width - pcblist[l].x) * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 0;
               bd2x = (pcblist[l].x - board_width) * unitsFactor;
               bd2y = bd1y;
               rot2 = 0;
               break;
            case 1:
               bd1x = (panel_width - pcblist[l].x) * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 270;
               bd2x = pcblist[l].x * unitsFactor;
               bd2y = (pcblist[l].y - board_width) * unitsFactor;
               rot2 = 90;
               break;
            case 2:
               bd1x = (panel_width - pcblist[l].x) * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 180;
               bd2x = (board_width + pcblist[l].x) * unitsFactor;
               bd2y = bd1y;
               rot2 = 180;
               break;
            case 3:
               bd1x = (panel_width - pcblist[l].x) * unitsFactor;
               bd1y = pcblist[l].y * unitsFactor;
               rot1 = 90;
               bd2x = pcblist[l].x * unitsFactor;
               bd2y = (pcblist[l].y + board_width) * unitsFactor;
               rot2 = 270;
               break;
            }
            if (strlen(boardnamearray[i].tophashname) && strlen(boardnamearray[i].bottomhashname))
            {
               // both sides
               fprintf(fPanel, "@%-19s %-7s %-5s %7.4lf  %7.4lf %5d  %-19s %7.4lf %7.4lf %4d\n",
                     boardnamearray[i].topname, "BOTTOM", "TRUE", bd1x, bd1y, rot1, boardnamearray[i].bottomname, bd2x, bd2y, rot2);
            }
            else if (strlen(boardnamearray[i].tophashname) && strlen(boardnamearray[i].bottomhashname) == 0)
            {
               // top but not bottom
               fprintf(fPanel, "@%-19s %-7s %-5s %7.4lf  %7.4lf %5d  %-19s %7.4lf %7.4lf %4d\n",
                     boardnamearray[i].topname, "BOTTOM", "TRUE", bd1x, bd1y, rot1, "none", 0.0, 0.0, 0);
            }
            else if (strlen(boardnamearray[i].tophashname) == 0 && strlen(boardnamearray[i].bottomhashname))
            {
               // no top but bottom
               fprintf(fPanel, "@%-19s %-7s %-5s %7.4lf  %7.4lf %5d  %-19s %7.4lf %7.4lf %4d\n",
                     boardnamearray[i].bottomname, "TOP", "TRUE", bd1x, bd1y, rot1, "none", 0.0, 0.0, 0);
            }
            else
            {
               // none at all
            }
         }
      }
   }

   fclose(fPanel);

   return 1;
}

/******************************************************************************
* get_formchar
*/
static char get_formchar(ApertureShapeTag shape)
{
   // Aperture Shape

   switch (shape)
   {
   case apertureRound:
   case apertureDonut:
   case apertureOblong:
      return 'C';  // These shapes all get circular output.

   case apertureSquare:
   case apertureRectangle:
   // case apertureComplex: Complex removed from here,so gets the out file default shape applied, per discussion with Mark 8 Sep 2011. No DR.
      return 'R';  // These shapes all get rectangle output.
   }

   // None Of The Above - Output the default shape.
   // User can select rectangle or circle for default aperture shape.
   // Handle rectangle here.
   if (defaultApertureShapeTag == apertureRectangle)
      return 'R';

   // The default-default, or possibly user selected circular as default.
   return 'C';
}

/******************************************************************************
* HP5DX_ReadPinData
*/
static int HP5DX_ReadPinData(const char *name, CDataList *DataList /*PCB Component datalist */, 
         double insert_x, double insert_y, double rotation, double scale)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   int pinCount = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      HP5DXCompPinInst *compPin = new HP5DXCompPinInst;
      comppininstarray.SetAtGrow(comppininstcnt, compPin);  
      comppininstcnt++;

      // DR 787338  Can not have dup pin refnames in NDF. Append a suffix if needed to
      // make pin refname unique.
      int counter = 0;
      CString pinRefname(data->getInsert()->getRefname());
      while (comppininstarray.HasCompPin(pinRefname) && counter < 100001)
      {
         pinRefname.Format("%s-%d", data->getInsert()->getRefname(), ++counter);
         ExportHasRenamedPins = true;
      }

      compPin->insertedPinGeometry = doc->getBlockAt( data->getInsert()->getBlockNumber() );
      compPin->pinname = pinRefname;
      compPin->originalPinname = data->getInsert()->getRefname();
      compPin->mirror = data->getInsert()->getMirrorFlags();
      compPin->rotation = rotation + data->getInsert()->getAngle();
      compPin->pinx = point2.x;
      compPin->piny = point2.y;
      pinCount++;
   }

   return pinCount;
}

bool CompPinInstArray::HasCompPin(CString refname)
{
   for (int i = 0; i < comppininstcnt; i++)
   {
      HP5DXCompPinInst *inst = this->GetAt(i);

      if (inst->pinname.Compare(refname) == 0)
         return true;
   }

   return false;
}


/******************************************************************************
* HP5DX_SortPinData
*/
static void HP5DX_SortPinData(const char *s)
{
   BOOL Done = FALSE;

   while (!Done)
   {
      Done = TRUE;

      for (int i=1; i<comppininstcnt; i++)
      {
         if (compare_name(comppininstarray[i-1]->pinname, comppininstarray[i]->pinname) > 0)
         {
            HP5DXCompPinInst *compPin = comppininstarray[i-1];
            comppininstarray[i-1] = comppininstarray[i];
            comppininstarray[i] = compPin;
            Done = FALSE;
         }
      }
   }

/*
   fprintf(flog," Pin Sort [%s]\n", s);
   for (i=0;i<comppininstcnt;i++)
   {
      fprintf(flog, "Pin Name [%s] Pin index [%d]\n", comppininstarray[i]->pinname, i+1);
   }
   display_error++;
*/
}

/******************************************************************************
*/
static int HP5DX_WriteSMDPad(FILE *wfp, const char *pcbComponentBlockName, double scale, int pinIndex, int subpadIndex, int totalPrevPinPadCnt, HP5DXCompPinInst *compPin, HP5DXPadstack *padstack)
{
   // Return number of pads (aka pins) written.

   // totalPinPadCnt is the sum of all pins+subpads that have been output so far.

   // if Round / Oblong do C
   // if Square / Rectangle do R
   char formchar = 'R';
   double dimx = 0, dimy = 0;
   double offx = 0, offy = 0;
   double aprot = 0;
   double insertx = 0, inserty = 0;


   if (!padstack->GetOutputValues(scale, formchar, dimx, dimy, offx, offy, aprot, insertx, inserty))
   {
      fprintf(flog, "Padshape [%s] without test area \n", padstack->GetName());
      display_error++;
      return 0;
   }

   // here rotate dimx, dimy in 90 degree steps
   if (dimy > 0)  // if dimy  == 0 it is round or square.
   {
      double radiansRot = aprot + compPin->rotation;
      int degreesRot = round(RadToDeg(radiansRot));
      degreesRot = normalizeDegrees(degreesRot);

      if (degreesRot != 0)
      {
         for (int i=0; i<degreesRot/90; i++)
         {
            double tmp;
            tmp = dimx;
            dimx = dimy;
            dimy = tmp;
         }

         Mat2x2 m;
         RotMat2(&m, radiansRot); 
         Point2 offsetPoint;
         offsetPoint.x = offx;
         offsetPoint.y = offy;
         TransPoint2(&offsetPoint, 1, &m, 0, 0);

         offx = offsetPoint.x;
         offy = offsetPoint.y;
      }
   }

   double padinsertx = offx + insertx;
   double padinserty = offy + inserty;
   double outputx = compPin->pinx + padinsertx;
   double outputy = compPin->piny + padinserty;

   // This is to get rid of pesky -0.000 in the output.
   if (fpeq(outputx, 0.0))
      outputx = 0.0;
   if (fpeq(outputy, 0.0))
      outputy = 0.0;

   if (allow_alpha_pins)
   {
      CString pinname = check_name('e', compPin->pinname);
      if (subpadIndex > 0)
      {
         CString subpadStr;
         subpadStr.Format("-%d", subpadIndex);
         pinname += subpadStr;
      }
      fprintf(wfp, "@%-40s %-10s %8.4lf %8.4lf %c     %8.4lf %8.4lf\n", 
         check_name('g', pcbComponentBlockName), pinname, outputx, outputy, formchar, dimx, dimy);
   }
   else
   {
      // For non-alpha pinnames support
      fprintf(wfp, "@%-40s %5d %8.4lf %8.4lf %c     %8.4lf %8.4lf\n", 
         check_name('g',pcbComponentBlockName), totalPrevPinPadCnt+1, outputx, outputy, formchar, dimx, dimy);
   }

   return 1;
}

/******************************************************************************
*/
static int HP5DX_WriteTHRUPad(FILE *wfp, const char *pcbComponentBlockName, double scale, int pinIndex, int subpadIndex, int totalPrevPinPadCnt, HP5DXCompPinInst *compPin, HP5DXPadstack *padstack)
{
   double dimx = 0.0, dimy = 0.0;
   double offx = 0.0, offy = 0.0;
   double insertx = 0.0, inserty = 0.0;
   double aprot;
   char formchar = 'R';

   if (!padstack->GetOutputValues(scale, formchar, dimx, dimy, offx, offy, aprot, insertx, inserty))
   {
      fprintf(flog, "Padshape [%s] without test area \n", padstack->GetName());
      display_error++;
      return 0;
   }

   // here rotate dimx, dimy in 90 degree steps
   if (dimy > 0)  // if dimy  == 0 it is round or square.
   {
      double radiansRot = aprot + compPin->rotation;
      int degreesRot = round(RadToDeg(radiansRot));
      degreesRot = normalizeDegrees(degreesRot);

      if (degreesRot != 0)
      {
         for (int i=0; i<degreesRot/90; i++)
         {
            double tmp;
            tmp = dimx;
            dimx = dimy;
            dimy = tmp;
         }

         // THT is ignoring offset, is that okay?
      }
   }
   else
   {
      dimy = dimx;
   }

   double padinsertx = offx + insertx;
   double padinserty = offy + inserty;

   double compPinAdjustedX = compPin->pinx + padinsertx;
   double compPinAdjustedY = compPin->piny + padinserty;

   // This is to get rid of pesky -0.000 in the output.
   // Happens to neg numbers "too close" to zero.
   if (fpeq(compPinAdjustedX, 0.0))
      compPinAdjustedX = 0.0;
   if (fpeq(compPinAdjustedY, 0.0))
      compPinAdjustedY = 0.0;

   if (allow_alpha_pins)
   {
      CString pinname = check_name('e', compPin->pinname);
      if (subpadIndex > 0)
      {
         CString subpadStr;
         subpadStr.Format("-%d", subpadIndex);
         pinname += subpadStr;
      }
      fprintf(wfp,"@%-40s %-10s %8.4lf %8.4lf %8.4lf %c     %8.4lf %8.4lf %8.4lf %8.4lf\n", 
         check_name('g', pcbComponentBlockName), pinname, 
         compPinAdjustedX, compPinAdjustedY, 
         padstack->GetDrill()*scale, formchar, 
         compPinAdjustedX, compPinAdjustedY, 
         dimx, dimy);
   }
   else
   {
      fprintf(wfp,"@%-40s %5d %8.4lf %8.4lf %8.4lf %c     %8.4lf %8.4lf %8.4lf %8.4lf\n", 
         check_name('g', pcbComponentBlockName), totalPrevPinPadCnt+1, 
         compPinAdjustedX, compPinAdjustedY, 
         padstack->GetDrill()*scale, formchar, 
         compPinAdjustedX, compPinAdjustedY, 
         dimx, dimy);
   }


   return 1;
}

/******************************************************************************
*/
static int HP5DX_WriteSMDPadstack(FILE *wfp, const char *pcbComponentBlockName, double scale, int pinIndex, int prevPadCount, HP5DXCompPinInst *compPin, HP5DXPadstack *padstack)
{
   int padcount = 0;

   if (padstack->GetShapeType() == apertureComplex)
   {
      // Output based on sub-pad collection
      PadstackArray &subpads = padstack->GetSubPadArray();
      bool useSubIndx = subpads.GetCount() > 1;
      for (int subpadIndex = 0; subpadIndex < subpads.GetCount(); subpadIndex++)
      {
         HP5DXPadstack *subps = subpads.GetAt(subpadIndex);
         int subIndx = useSubIndx ? subpadIndex + 1 : 0;
         padcount += HP5DX_WriteSMDPad(wfp, pcbComponentBlockName, scale, pinIndex, subIndx, (prevPadCount + padcount), compPin, subps);
      }
   }
   else
   {
      // Output based on this padstack
      padcount = HP5DX_WriteSMDPad(wfp, pcbComponentBlockName, scale, pinIndex, 0, (prevPadCount + padcount), compPin, padstack);
   }

   return padcount;
}

/******************************************************************************
*/
static int HP5DX_WriteTHRUPadstack(FILE *wfp, const char *pcbComponentBlockName, double scale, int pinIndex, int prevPadCount, HP5DXCompPinInst *compPin, HP5DXPadstack *padstack)
{
   int padcount = 0;

   if (padstack->GetShapeType() == apertureComplex)
   {
      // Output based on sub-pad collection.
      PadstackArray &subpads = padstack->GetSubPadArray();
      bool useSubIndx = subpads.GetCount() > 1;
      for (int subpadIndex = 0; subpadIndex < subpads.GetCount(); subpadIndex++)
      {
         HP5DXPadstack *subps = subpads.GetAt(subpadIndex);
         int subpadType = subps->GetType();
         // Only write top subpads
         if (subpadType == 1)
         {
            int subIndx = useSubIndx ? subpadIndex + 1 : 0;
            padcount += HP5DX_WriteTHRUPad(wfp, pcbComponentBlockName, scale, pinIndex, subIndx, (prevPadCount + padcount), compPin, subps);
         }
      }
   }
   else
   {
      // Output based on this padstack
      padcount = HP5DX_WriteTHRUPad(wfp, pcbComponentBlockName, scale, pinIndex, 0, (prevPadCount + padcount), compPin, padstack);
   }

   return padcount;
}

/******************************************************************************
* HP5DX_WriteSMDPinData
*/
static void HP5DX_WriteSMDPinData(FILE *wfp, const char *pcbComponentBlockName, double scale)
{
   int pinPadCount = 0;

   for (int i = 0; i < comppininstcnt; i++)
   {
      HP5DXCompPinInst *compPin = comppininstarray[i];
      if (compPin != NULL)
      {
         BlockStruct *pinGeom = compPin->insertedPinGeometry;
         if (pinGeom != NULL)
         {
            int padstackIndex = get_padstackindex(pinGeom->getName());
            if (padstackIndex >= 0)
            {
               HP5DXPadstack *padstack = padstackarray[padstackIndex];
               if (padstack != NULL)
               {
                  pinPadCount += HP5DX_WriteSMDPadstack(wfp, pcbComponentBlockName, scale, i, pinPadCount, compPin, padstack);
               }
            }
         }
      }
   }
}

/******************************************************************************
* HP5DX_WriteTHRUPinData
*/
static void HP5DX_WriteTHRUPinData(FILE *wfp, const char *pcbComponentBlockName, double scale)
{
   int pinPadCount = 0;

   for (int i = 0; i < comppininstcnt; i++)
   {
      HP5DXCompPinInst *compPin = comppininstarray[i];
      if (compPin != NULL)
      {
         BlockStruct *pinGeom = compPin->insertedPinGeometry;
         if (pinGeom != NULL)
         {
            int padstackIndex = get_padstackindex(pinGeom->getName());
            if (padstackIndex >= 0)
            {
               HP5DXPadstack *padstack = padstackarray[padstackIndex];
               if (padstack != NULL)
               {
                  pinPadCount += HP5DX_WriteTHRUPadstack(wfp, pcbComponentBlockName, scale, i, pinPadCount, compPin, padstack);
               }
            }
         }
      }
   }
}

/******************************************************************************
* write_via_definitions_smd
   run throu drc list
   only act on via/via and via/pin
*/
static int write_via_definitions_smd(FILE *fp)
{
   double scale = unitsFactor;

   fprintf(fp, "# Write SMD Short test pattern\n");

   for (int i=0; i<viadrccnt; i++)
   {
      // here get padsize
      if (padstack_name_already_written(i, viadrcarray[i]->padstackname))  
         continue;

      int pi = get_padstackindex(viadrcarray[i]->padstackname);
      if (pi < 0)
         continue;

      HP5DXPadstack *padstack = padstackarray[pi];
      if (padstack == NULL)
         continue;

      // if Round / Oblong do C
      // if Square / Rectangle do R
      char  formchar = 'R';
      double dimx = 0.0, dimy = 0.0;
      double offx = 0.0, offy = 0.0;
      double insertx = 0.0, inserty = 0.0;
      double aprotRadians = 0.0;

      if (!padstack->GetOutputValues(scale, formchar, dimx, dimy, offx, offy, aprotRadians, insertx, inserty))
      {
         fprintf(flog, "Padshape [%s] without test area \n", viadrcarray[i]->padstackname);
         display_error++;
         continue;
      }


      // here rotate dimx, dimy in 90 degree steps
      if (dimy > 0)  // if dimy  == 0 it is round or square.
      {
         double aprotDegrees = RadToDeg(aprotRadians);
         int r = normalizeDegrees(round(aprotDegrees));

         for (int i = 0; i < r/90; i++)
         {
            double tmp;
            tmp = dimx;
            dimx = dimy;
            dimy = tmp;
            tmp = offx;
            offx = offy;
            offy = tmp;
         }
      }

      fprintf(fp,"@%-40s %5d %8.4lf %8.4lf %c     %8.4lf %8.4lf\n", 
               check_name('g', viadrcarray[i]->padstackname), 1, offx, offy, formchar, dimx, dimy );
   }

   return 1;
}

/******************************************************************************
* HP5DX_WriteCOMPONENData
*/
static void HP5DX_WriteCOMPONENData(FILE *stream, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
      double scale, BOOL WritingBottom)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      CString typeString = "TH";
      if (is_attvalue(doc, data->getAttributesRef(), ATT_SMDSHAPE, 1))
         typeString = "SM";

      // DR 788418 Initially it asked to transfer LOADED attrib value to 5DX_TEST attrib. But really what
      // was wanted was to use LOADED attrib when 5DX_TEST attrib is not present. It could be that neither
      // are present, so we will start with default of TRUE.
      CString loadString = "TRUE";
      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_5DX_TEST, 2);
      // If attrib not present or not correct type or is empty then get LOADED attrib.
      if (attrib == NULL || attrib->getValueType() != VT_STRING || attrib->getStringValue().IsEmpty())
      {
         attrib = is_attvalue(doc, data->getAttributesRef(), ATT_LOADED, 2);
      }
      // If we have an attrib then test for values. We're already defaulted to TRUE
      // so only need to check values that will change that, so FALSE or PARTIAL.
      if (attrib != NULL)
      {
         CString attrVal( attrib->getStringValue() );

         if (attrVal.CompareNoCase("FALSE") == 0)
            loadString = "FALSE";
         else if (attrVal.CompareNoCase("PARTIAL") == 0)
            loadString = "PARTIAL";
      }
         
      BlockStruct *compGeom = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (!compGeom)
         continue;

      CString packageID = "";
      attrib = is_attvalue(doc, data->getAttributesRef(), ATT_5DX_PACKAGEID, 2);
      if (attrib && attrib->getValueType() == VT_STRING)
         packageID = attrib->getStringValue();

      if (packageID.IsEmpty())
      {
         CString refName = data->getInsert()->getRefname();

         HP5DXF_PackageMap packageMap;
         packageMap.prefix = refName.SpanExcluding("0123456789");
         packageMap.geomname = compGeom->getName();
         packageMap.package_id = "";

         int packageIndex = get_package_id(packageMap);
         if (packageIndex < 0)
         {
            // big problem
            packageID = "NOT_FOUND";
         }
         else
            packageID = packagemapArr[packageIndex]->package_id;

         if (packageID.IsEmpty())
         {
            packageID = compGeom->getName();
         }
      }

      if (!WritingBottom && !data->getInsert()->getMirrorFlags())
      {
         fprintf(stream,"@%-19s %-7s %-40s %s       0\n",
               check_name('c', data->getInsert()->getRefname()), loadString, check_name('i', packageID), typeString);
      }
      else if (WritingBottom && data->getInsert()->getMirrorFlags())
      {
         fprintf(stream,"@%-19s %-7s %-40s %s       0\n", 
               check_name('c', data->getInsert()->getRefname()), loadString, check_name('i', packageID), typeString);
      }
   }
}

/******************************************************************************
* HP5DX_WriteViaDrcComponent
*/
static void HP5DX_WriteViaDrcComponent(FILE *stream, BOOL WritingBottom)
{
   for (int i=0; i<viadrccnt; i++)
   {
      HP5DXViaDrcList *viaDrc = viadrcarray[i];

      if (viaDrc->layer == -4)   // only both
      {
         fprintf(stream, "@%-19s %-7s %-40s %s       0\n", 
                  check_name('c', viaDrc->vianame), "TRUE", check_name('i', viaDrc->padstackname), "SM");
      }

      if (!WritingBottom && viaDrc->layer == -1)   // only top
      {
         fprintf(stream, "@%-19s %-7s %-40s %s       0\n", 
                  check_name('c', viaDrc->vianame), "TRUE", check_name('i', viaDrc->padstackname), "SM");
      }

      if (WritingBottom && viadrcarray[i]->layer == -2)  // only bottom
      {
         fprintf(stream, "@%-19s %-7s %-40s %s       0\n", 
                  check_name('c', viaDrc->vianame), "TRUE", check_name('i', viaDrc->padstackname), "SM");
      }
   }
}

/******************************************************************************
* HP5DX_WriteViaDrcBoard
*/
static int HP5DX_WriteViaDrcBoard(FILE *wfp, int bottom, double boardwidth, double scale)
{
   int   i;

   for (i=0;i<viadrccnt;i++)
   {
      if (!bottom)
      {
         if (viadrcarray[i]->layer == -4 || viadrcarray[i]->layer == -1) // both or top
         {
            fprintf(wfp,"@%-17s %7.4lf %7.4lf %4.0lf     %s\n", 
                  check_name('c', viadrcarray[i]->vianame), 
                  viadrcarray[i]->xloc * scale, viadrcarray[i]->yloc * scale, 0.0, 
                  check_name('g', viadrcarray[i]->padstackname));
         }
      }
      else // bottom
      {
         if (viadrcarray[i]->layer == -4 || viadrcarray[i]->layer == -2) // both or bottom
         {
            fprintf(wfp,"@%-17s %7.4lf %7.4lf %4.0lf     %s\n", 
                  check_name('c', viadrcarray[i]->vianame), 
                  boardwidth - viadrcarray[i]->xloc * scale, viadrcarray[i]->yloc * scale, 0.0, 
                  check_name('g', viadrcarray[i]->padstackname));
         }
      }
   }

   return 1;
} /* end HP5DX_WriteViaDrcBoard */

/******************************************************************************
* HP5DX_WriteBOARDCOMPONENData
*/
static int HP5DX_WriteBOARDCOMPONENData(FILE *wfp, CDataList *DataList, 
      double insert_x, double insert_y, double rotation, int mirror, 
      double scale, int bottom, double boardwidth, int orthogonal)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      CString typ = "TH";

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;
/*
      if (is_attvalue(doc, data->getAttributesRef(), ATT_SMDSHAPE,1))
         typ = "SM";
*/    

      // the geom determins the SMD
      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (!block)
         continue;

      if (is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
         typ = "SM";

      CString  refname = data->getInsert()->getRefname();

      double x, y, rot, xrot, yrot;
      Point2 point2;

      if (!bottom && !data->getInsert()->getMirrorFlags())  // only top
      {
         rot = RadToDeg(rotation + data->getInsert()->getAngle());
         rot = 0.1 *  round(rot * 10); // accuracy

         if (rot >= 360)
            rot -= 360;
         if (rot < 0)
            rot += 360;

         // calculates the centroid offset from a given block.
         CalculateCentroid_PinCenters(doc, block, &x, &y);

         Rotate(x, y, rot, &xrot, &yrot);

         //TSR 3666: xrot & yrot were added to x & y causing misslocations.
         point2.x = (data->getInsert()->getOriginX() + xrot) * scale;  //TSR 3699: xrot and yrot are added back- Joanne
         point2.y = (data->getInsert()->getOriginY() + yrot) * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         x = point2.x;
         y = point2.y;

         if (orthogonal)
         {
            double drot;
            if ((drot = round(rot / 90) * 90) != rot)
            {
               fprintf(flog, " Component [%s] rotation [%1.0lf] adjusted to [%1.0lf]\n", refname, rot, drot);
               display_error++;
               rot = drot;
            }
         }

         fprintf(wfp, "@%-17s %7.4lf %7.4lf %4.0lf     %s\n", check_name('c', refname), x, y, rot, check_name('g', block->getName()));
      }
      else if (bottom && data->getInsert()->getMirrorFlags())  // only bottom
      {
         rot = RadToDeg(PI2 - (rotation + data->getInsert()->getAngle()));   // add 180 degree if mirrored.
         rot = 0.1 *  round(rot * 10); // accuracy

         if (rot >= 360)
            rot -= 360;
         if (rot < 0)
            rot += 360;

         // calculates the centroid offset from a given block.
         CalculateCentroid_PinCenters(doc, block, &x, &y);

         Rotate(x, y, rot, &xrot, &yrot);

         point2.x = boardwidth - ((data->getInsert()->getOriginX() - xrot) * scale);
         point2.y = (data->getInsert()->getOriginY()+yrot) * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         x = point2.x;
         y = point2.y;

         if (orthogonal)
         {
            double drot;
            if ((drot = round(rot / 90) * 90) != rot)
            {
               fprintf(flog, " Component [%s] rotation [%1.0lf] adjusted to [%1.0lf]\n", refname, rot, drot);
               display_error++;
               rot = drot;
            }
         }

         fprintf(wfp, "@%-17s %7.4lf %7.4lf %4.0lf     %s\n", check_name('c', refname), x, y, rot, check_name('g', block->getName()));
      }
   }

   return 1;
}

/******************************************************************************
* block_is_used
*/
static int block_is_used(int blocknum)
{
   for (int i=0;i<compinstcnt;i++)
   {
      if (compinstarray[i]->blocknum == blocknum)
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* write_geometry_definitions
*/
static int write_geometry_definitions(FILE *stream, BOOL SMD)
{
	WORD technologyKw = (WORD)doc->getStandardAttributeKeywordIndex(standardAttributeTechnology);

   bool firstPackageWithRenamedPin = true;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      
      if (block == NULL)
         continue;

      if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      if (block->getFlags() & BL_WIDTH || block->getFlags() & BL_TOOL || block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE || block->getFlags() & BL_FILE)
         continue;

      if (!block_is_used(block->getBlockNumber()))  
         continue;

      Attrib *attrib = NULL;
		if (block->getAttributes() == NULL || !block->getAttributes()->Lookup(technologyKw, attrib))
			continue;

		CString technology = attrib->getStringValue();
      if ((SMD == TRUE && technology.CompareNoCase("SMD") == 0) ||
			  (SMD == FALSE && technology.CompareNoCase("THRU") == 0))
		{
			double x, y;
			// calculates the centroid offset from a given block.
			CalculateCentroid_PinCenters(doc, block, &x, &y);

			comppininstarray.SetSize(100,100);
			comppininstcnt = 0;

			int pincnt = HP5DX_ReadPinData(block->getName(), &block->getDataList(), -x * unitsFactor, -y * unitsFactor, 0.0, unitsFactor);

			HP5DX_SortPinData(block->getName());

			if (SMD)
				HP5DX_WriteSMDPinData(stream, block->getName(), unitsFactor);
			else
				HP5DX_WriteTHRUPinData(stream, block->getName(), unitsFactor);

         // Add any renamed pins to log file.
         bool firstRenamedPin = true;
			for (int i=0; i<comppininstcnt; i++)
         {
            HP5DXCompPinInst *pinInst = comppininstarray.GetAt(i);
            if (pinInst->pinname.Compare(pinInst->originalPinname) != 0) 
            {
               if (firstPackageWithRenamedPin)
               {
                  fprintf(flog, "The following geometries had duplicated pins that were suffixed with -n to eliminate duplicates.\n");
                  firstPackageWithRenamedPin = false;
               }

               if (firstRenamedPin)
               {
                  fprintf(flog, "Package: %s\n", block->getName());
                  fprintf(flog, "Pins: ");
               }

               fprintf(flog, "%s%s", (firstRenamedPin?"":", "), pinInst->originalPinname);
               firstRenamedPin = false;
            }
         }
         if (!firstRenamedPin) // Then we output atleast one, so need to write a newline.
            fprintf(flog, "\n");

			for (int i=0; i<comppininstcnt; i++)
				delete comppininstarray[i];
			comppininstarray.RemoveAll();

			fprintf(stream, "#\n");
		}
   }

   // If we output any package/pin renames, then write a couple blank lines
   // to separate from any following log entries.
   if (!firstPackageWithRenamedPin)
      fprintf(flog, "\n\n");

   return 1;
}

/******************************************************************************
* padstack_name_already_written
   run throu drc list
   only act on via/via and via/pin
*/
static int padstack_name_already_written(int ptr, const char *n)  
{
   for (int i=0; i<ptr; i++)
   {
      if (!viadrcarray[i]->padstackname.Compare(n))
         return TRUE;
   }

   return 0;
}

/******************************************************************************
* create_landpat_ndf
*/
static int create_landpat_ndf(const char *panelname, const char *path, const char *hproot, int punits)
{
   CString  hprootpath;
   hprootpath = path;
   hprootpath +=hproot;

   FILE  *fp;

   CString  finalpath;

   finalpath = hprootpath;
   finalpath += "\\";
   finalpath += "landpat.ndf";

   // this is level 1 files 
   if ((fp = fopen(finalpath,"wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open File [%s]",finalpath);
      MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   fprintf(fp,"# LANDPAT.NDF\n");
   fprintf(fp,"# Surface Mount Land Pattern Definitions\n");
   fprintf(fp,"# Panel Name : %s\n", panelname);

   hp5dx_created(fp);

   hp5dx_units(fp, punits);
   fprintf(fp,"\n\n");

   fprintf(fp,"# Land Pattern ID                         Pin #   X Loc    Y Loc  Shape   X Size   Y Size\n");

   write_geometry_definitions(fp, TRUE);
   write_via_definitions_smd(fp);      // this is needed for the probable short analysis

   fclose(fp);

   return 1;
}

/******************************************************************************
* create_padgeom_ndf
*/
static int  create_padgeom_ndf(const char *panelname, const char *path, const char *hproot, int punits)
{
   CString  hprootpath;
   hprootpath = path;
   hprootpath +=hproot;

   FILE  *fp;

   CString  finalpath;

   finalpath = hprootpath;
   finalpath += "\\";
   finalpath += "padgeom.ndf";

   // this is level 1 files 
   if ((fp = fopen(finalpath,"wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open File [%s]",finalpath);
      MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   fprintf(fp,"# PADGEOM.NDF\n");
   fprintf(fp,"# Through Hole Land Pattern Definitions\n");
   fprintf(fp,"# Panel Name : %s\n", panelname);

   hp5dx_created(fp);

   hp5dx_units(fp, punits);
   fprintf(fp,"\n\n");

   fprintf(fp,"#                                           Pin    Hole     Hole    Hole   Pad     Pad     Pad       Pad      Pad\n");
   fprintf(fp,"# Land Pattern ID                           Num    X Loc    Y Loc   Dia    Shape   X Loc   Y Loc     X Size   Y Size\n");

   write_geometry_definitions(fp, FALSE);

   fclose(fp);

   return 1;
}

/******************************************************************************
* do_alignmentpads
*/
static int do_alignmentpads(FILE *fp, CNetList *n, int bottom)
{
   NetStruct *net;
   POSITION  netPos;
   Attrib   *a;

   // alignment 1
   int      first = TRUE;
   netPos = n->GetHeadPosition();
   while (netPos != NULL)
   {
      net = n->GetNext(netPos);
      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         int   i = get_component(compPin->getRefDes());

         if (compinstarray[i]->mirror == bottom)
         {
            if (a =  is_attvalue(doc, compPin->getAttributesRef(), ATT_5DX_ALIGNMENT_1, 2))
            {
               if (first)
                  fprintf(fp, ".ALIGNMENT_PADS: 1 ");
               first = FALSE;
               fprintf(fp, " %s", check_name('c',compPin->getRefDes()));
               fprintf(fp, " %s", compPin->getPinName());
            }
         } // not on same layer
      }
   }
   fprintf(fp,"\n");

   // alignment 2
   first = TRUE;
   netPos = n->GetHeadPosition();
   while (netPos != NULL)
   {
      net = n->GetNext(netPos);
      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         int   i = get_component(compPin->getRefDes());

         if (compinstarray[i]->mirror == bottom)
         {
            if (a =  is_attvalue(doc, compPin->getAttributesRef(), ATT_5DX_ALIGNMENT_2, 2))
            {
               if (first)
                  fprintf(fp, ".ALIGNMENT_PADS: 2 ");
               first = FALSE;
               fprintf(fp, "  %s", check_name('c',compPin->getRefDes()));
               fprintf(fp, " %s", compPin->getPinName());
            }
         }
      }
   }
   fprintf(fp,"\n");

   // alignment 3
   first = TRUE;
   netPos = n->GetHeadPosition();
   while (netPos != NULL)
   {
      net = n->GetNext(netPos);
      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         int   i = get_component(compPin->getRefDes());

         if (compinstarray[i]->mirror == bottom)
         {
            if (a =  is_attvalue(doc, compPin->getAttributesRef(), ATT_5DX_ALIGNMENT_3, 2))
            {
               if (first)
                  fprintf(fp, ".ALIGNMENT_PADS: 3 ");
               first = FALSE;
               fprintf(fp, " %s", check_name('c',compPin->getRefDes()));
               fprintf(fp, " %s", compPin->getPinName());
            }
         }
      }
   }
   fprintf(fp,"\n");

   return 1;
}

/******************************************************************************
* create_board_ndf
*/
static int create_board_ndf(int boardnamearrayindex, const char *panelname, const char *path, const char *hproot, int punits, 
      double thickness, int bottom, int orthogonal)
{
   CString  hprootpath;
   hprootpath = path;
   hprootpath +=hproot;

   FILE  *fp;

   CString  finalpath;

   if (!bottom)
   {
      if (strlen(boardnamearray[boardnamearrayindex].tophashname))
      {
         finalpath = hprootpath;
         finalpath += "\\";
         finalpath += boardnamearray[boardnamearrayindex].tophashname;
         finalpath += "\\";
         finalpath += "board.ndf";
      }
      else
      {
         // no has name 
         return 0;
      }
   }
   else
   {
      if (strlen(boardnamearray[boardnamearrayindex].bottomhashname))
      {
         finalpath = hprootpath;
         finalpath += "\\";
         finalpath += boardnamearray[boardnamearrayindex].bottomhashname;
         finalpath += "\\";
         finalpath += "board.ndf";
      }
      else
      {
         // no has name 
         return 0;
      }
   }

   // this is level 1 files 
   if ((fp = fopen(finalpath,"wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open File [%s]",finalpath);
      MessageBox(NULL, tmp,"File Create Error!", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   fprintf(fp,"# BOARD.NDF\n");
   fprintf(fp,"# Component Placement Data\n");

   CString  bname;
   BlockStruct *block = doc->Find_Block_by_Num(boardnamearray[boardnamearrayindex].geomnumber);

   if (!bottom)
      bname.Format("%s (%s)",block->getName() ,boardnamearray[boardnamearrayindex].topname);
   else
      bname.Format("%s (%s)",block->getName() ,boardnamearray[boardnamearrayindex].bottomname);

   fprintf(fp,"# Board Name : %s\n", bname);

   hp5dx_created(fp);

   if (!bottom)
      fprintf(fp,".BOARD_ID:  %s\n", boardnamearray[boardnamearrayindex].topname);
   else
      fprintf(fp,".BOARD_ID:  %s\n", boardnamearray[boardnamearrayindex].bottomname);

   hp5dx_units(fp, punits);
   fprintf(fp,"\n\n");

   fprintf(fp,"# Board Dimensions   X Size   Y Size   Thickness\n");

   // primary board outline extent - check for min on zero.
   fprintf(fp,".DIMENSIONS:   %2.3lf %2.3lf %2.3lf\n",
      boardnamearray[boardnamearrayindex].xmax * unitsFactor,
      boardnamearray[boardnamearrayindex].ymax * unitsFactor,
      thickness);

   fprintf(fp,"\n");
   fprintf(fp,"# Alignment Pads\n");

   FileStruct *f = doc->Find_File_by_BlockPtr(block);
   if (f)   do_alignmentpads(fp,&(f->getNetList()), bottom);

   fprintf(fp,"\n\n");

   fprintf(fp,"# Component Placement (Referenced to Geometry Centroid)\n");
   fprintf(fp,"# Ref Designator    X Loc   Y Loc   Rot     Land Pattern ID\n");

   HP5DX_WriteBOARDCOMPONENData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0,  unitsFactor, bottom,
      boardnamearray[boardnamearrayindex].xmax * unitsFactor, orthogonal);

   HP5DX_WriteViaDrcBoard(fp, bottom, boardnamearray[boardnamearrayindex].xmax * unitsFactor, unitsFactor);

   fclose(fp);

   return 1;
}

/******************************************************************************
* create_component_ndf
*/
static int create_component_ndf(int boardnamearrayindex, const char *panelname, const char *path, const char *hproot, BOOL WritingBottom)
{
   CString hpRootPath;
   hpRootPath = path;
   hpRootPath += hproot;

   CString finalPath;

   if (!WritingBottom)
   {
      if (!strlen(boardnamearray[boardnamearrayindex].tophashname))
         return 0;

      finalPath = hpRootPath;
      finalPath += "\\";
      finalPath += boardnamearray[boardnamearrayindex].tophashname;
      finalPath += "\\";
      finalPath += "componen.ndf";
   }
   else
   {
      if (!strlen(boardnamearray[boardnamearrayindex].bottomhashname))
         return 0;

      finalPath = hpRootPath;
      finalPath += "\\";
      finalPath += boardnamearray[boardnamearrayindex].bottomhashname;
      finalPath += "\\";
      finalPath += "componen.ndf";
   }

   // this is level 1 files 
   FILE *stream = fopen(finalPath, "wt");
   if (stream == NULL)
   {
      CString tmp;
      tmp.Format("Can not open File [%s]", finalPath);
      MessageBox(NULL, tmp, "File Create Error!", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   fprintf(stream, "# COMPONEN.NDF\n");
   fprintf(stream, "# Component Test and Package Identification\n");

   CString bname;
   BlockStruct *block = doc->Find_Block_by_Num(boardnamearray[boardnamearrayindex].geomnumber);

   if (!WritingBottom)
      bname.Format("%s (%s)",block->getName() ,boardnamearray[boardnamearrayindex].topname);
   else
      bname.Format("%s (%s)",block->getName() ,boardnamearray[boardnamearrayindex].bottomname);

   fprintf(stream, "# Board Name : %s\n", bname);

   hp5dx_created(stream);

   fprintf(stream, "# Component Test and Package Identification\n");

   fprintf(stream, "# Ref Designator     Test    Package Id                            SM/TH     Subtype\n");
   
   HP5DX_WriteCOMPONENData(stream, &block->getDataList(), 0.0, 0.0, 0.0, 0, unitsFactor, WritingBottom);

   HP5DX_WriteViaDrcComponent(stream, WritingBottom);

   fclose(stream);

   return 1;
}

/******************************************************************************
* check_comps
// this function checks if a board has components on top and bottom
*/
void check_comps(CDataList *DataList, int *topcnt, int *botcnt, double *xmin, double *ymin, double *xmax, double *ymax)
{
   *xmin = *ymin = FLT_MAX;
   *xmax = *ymax = -FLT_MAX;

   *topcnt = 0;
   *botcnt = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() == T_POLY && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
      {
         ExtentRect extents;
         Mat2x2 m;
         RotMat2(&m, 0.0);
         
         // same as autoboard origin
         PolyExtents(doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, FALSE);
         if (extents.left < *xmin)
            *xmin = extents.left;
         if (extents.right > *xmax)
            *xmax = extents.right;
         if (extents.bottom < *ymin)
            *ymin = extents.bottom;
         if (extents.top > *ymax)
            *ymax = extents.top;
      }

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)  
      {
         int block_mirror = data->getInsert()->getMirrorFlags();
         if (block_mirror)
            *botcnt = *botcnt + 1;
         else
            *topcnt = *topcnt + 1;

         CString refname = data->getInsert()->getRefname();

         int compInstIndex = get_component(refname);
         compinstarray[compInstIndex]->mirror = block_mirror;
         compinstarray[compInstIndex]->blocknum = data->getInsert()->getBlockNumber();

         HP5DXF_PackageMap a;
         a.prefix = refname.SpanExcluding("0123456789");
         a.geomname = "";
         a.package_id = "";
         a.used = TRUE;
         a.changed = 0;

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         if (block)
         {
            a.geomname = block->getName();

            // DR 805368 - Write no-pin parts to log file.
            if (block->getPinCount() < 1)
               fprintf(flog,"Component [%s] geometry [%s] has no pins.\n", refname, block->getName());
         }

         Attrib *attrib;
         if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_5DX_PACKAGEID, 2))   
         {
            a.prefix = refname;
            a.package_id = get_attvalue_string(doc, attrib);
         }

         if ((compInstIndex = get_package_id(a)) < 0)
         {
            HP5DXF_PackageMap *p = new HP5DXF_PackageMap;
            p->geomname = a.geomname;
            p->prefix = a.prefix;
            p->package_id = a.package_id;
            p->used = a.used;
            p->changed = a.changed;
            packagemapArr.SetAtGrow(maxpackagemapArr++, p);
         }              
         else
         {
            packagemapArr[compInstIndex]->used = TRUE;
         }
      }
   }
}

//---------------------------------------------------------------------------

// This constructor is used with block that is inserted as a pin
//
HP5DXPadstack::HP5DXPadstack(BlockStruct &padstackBlock, int insertLayer, double scale, int punits)
: m_block_num(padstackBlock.getBlockNumber())
, m_name(padstackBlock.getName())
, m_parentPS(NULL)
, m_drill(0.) 
, m_layerType(0)
, m_shapetypetop(apertureUndefined)
, m_shapetypebot(apertureUndefined)
, m_xsizetop(0.)
, m_ysizetop(0.)
, m_xofftop(0.)
, m_yofftop(0.)
, m_toprotation(0.)
, m_xsizebot(0.)
, m_ysizebot(0.)
, m_xoffbot(0.)
, m_yoffbot(0.)
, m_botrotation(0.)
{
   this->GetPADSTACKData( &padstackBlock, -1, scale, punits);
}

// This constructor is only for subpins
//
HP5DXPadstack::HP5DXPadstack(CString name, HP5DXPadstack *parentPS)
: m_block_num(-1)
, m_name( name )
, m_parentPS(parentPS)
, m_drill(0.) 
, m_layerType(0)
, m_shapetypetop(apertureUndefined)
, m_shapetypebot(apertureUndefined)
, m_xsizetop(0.)
, m_ysizetop(0.)
, m_xofftop(0.)
, m_yofftop(0.)
, m_toprotation(0.)
, m_xsizebot(0.)
, m_ysizebot(0.)
, m_xoffbot(0.)
, m_yoffbot(0.)
, m_botrotation(0.)
{
   /////////////////
}

//--------------------------------------------------------------------------------------

bool HP5DXPadstack::AddPolyListSubPads(int pageUnits, CPolyList *polyList, int layerType, double insertX, double insertY, double insertRotRad)
{
   bool hasAtleastOneHack = false;

   // Only want polys on copper surface layers
   if (layerType == 0)
      return false;

   POSITION polyPos = polyList->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = polyList->GetNext(polyPos);

      if (poly != NULL)
      {
         double sizeA = 0.;
         double sizeB = 0.;
         double offsetX = 0.;
         double offsetY = 0.;
         ApertureShapeTag shape = apertureUndefined;

         // In case it is a circle
         double cx = 0.;
         double cy = 0.;
         double radius = 0.;

         double llx, lly, uux, uuy;
         if (PolyIsRectangle(poly, &llx, &lly, &uux, &uuy))
         {
            // That's handy, it is already a rectangle
            sizeA = uux - llx;
            sizeB = uuy - lly;
            offsetX = llx + (uux - llx)/2;
            offsetY = lly + (uuy - lly)/2;
            shape = apertureRectangle;
         }
         else if (PolyIsCircle(poly, &cx, &cy, &radius))
         {
            // Circles are just as handy as rectangles
            sizeA = radius * 2.;  // extent is diameter
            sizeB = sizeA;
            offsetX = 0.;
            offsetY = 0.;
            shape = apertureOblong; // 5dx only does rectangles and oblongs, circle is just a symmetrical oblong
         }
         else
         {
            // Hack
            HackComplexAperture(pageUnits, poly, sizeA, sizeB, offsetX, offsetY);
            shape = defaultApertureShapeTag; // Use .DEFAULT_SHAPE setting, per discussion with Mark 8 Sep 2011, no DR.
            hasAtleastOneHack = true;
         }

         // Now add "padstack" to "padstack", i.e. add this one as sub-padstack
         // within the padstack.
         HP5DXPadstack *subpad = new HP5DXPadstack("subpad", this);
         subpad->Set(layerType, shape, sizeA, sizeB, offsetX, offsetY, insertRotRad, insertX, insertY); 
         this->m_subpadarray.Add(subpad);
      }
   }

   return hasAtleastOneHack;
}

//--------------------------------------------------------------------------------------

bool HP5DXPadstack::DissectComplexAperture(int pageUnits, BlockStruct *thisBlock, int layerType, double insertX, double insertY, double insertRotRad)
{
   bool hasAtLeastOneHack = false;

   // if aperture
   //    do aperture thing
   // else
   //    process data list

   if (thisBlock != NULL)
   {
      if (thisBlock->isTool())
      {
         this->m_drill = thisBlock->getToolSize(); // This gets set in parent, so only one drill size available.
                                                   // This could be bug in complex aperture that inserts than one
                                                   // drill and drills are different sizes.
                                                   // Choosing not to cope with that today.
      }
      else if (thisBlock->isAperture())
      {
         if (thisBlock->isComplexAperture())
         {
            BlockStruct *subBlock = doc->Find_Block_by_Num(thisBlock->getComplexApertureSubBlockNumber());
            hasAtLeastOneHack |= DissectComplexAperture(pageUnits, subBlock, layerType, insertX, insertY, insertRotRad); 
         }
         else
         {
            // Only want pads on copper surface layers
            if (layerType != 0)
            {
               // simple aperture, 5dx only supports rect and circ (oblong, really), but we just toss out the sizes as-is
               double sizeA = thisBlock->getSizeA();
               double sizeB = thisBlock->getSizeB();
               if (sizeB < SMALLNUMBER)
                  sizeB = sizeA;
               double offsetX = 0.0;
               double offsetY = 0.0;
               ApertureShapeTag shape = thisBlock->getShape();
               // Now add "padstack" to "padstack", i.e. add this one as sub-padstack
               // within the padstack.
               HP5DXPadstack *subpad = new HP5DXPadstack("subpad", this);
               subpad->Set(layerType, shape, sizeA, sizeB, offsetX, offsetY, insertRotRad, insertX, insertY);
               this->m_subpadarray.Add(subpad);
            }
         }
      }
      else
      {
         // Not aperture, process datalist

         POSITION dataPos = thisBlock->getHeadDataPosition();
         while (dataPos != NULL)
         {
            DataStruct *data = thisBlock->getNextData(dataPos);
            if (data != NULL)
            {
               if (data->getDataType() == dataTypeInsert)
               {
                  BlockStruct *insertedBlock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
                  //double newInsertX = insertX + data->getInsert()->getOriginX();
                  //double newInsertY = insertY + data->getInsert()->getOriginY();
                  double newInsertX = data->getInsert()->getOriginX() - insertX;  // Convert to relative to ultimate parent, not immediate parent
                  double newInsertY = data->getInsert()->getOriginY() - insertY;
                  double newRotRad  = insertRotRad + data->getInsert()->getAngleRadians();
                  hasAtLeastOneHack |= DissectComplexAperture(pageUnits, insertedBlock, layerType, newInsertX, newInsertY, newRotRad);   
               }
               else if (data->getDataType() == dataTypePoly)
               {
                  CPolyList *polyList = data->getPolyList();
                  hasAtLeastOneHack |= AddPolyListSubPads(pageUnits, polyList, layerType, insertX, insertY, insertRotRad);
               }
            }
         }
      }
   }

   return hasAtLeastOneHack;
}

//--------------------------------------------------------------------------------------

HP5DX_HackedAperture::HP5DX_HackedAperture(double sizeA, double sizeB, double offsetX, double offsetY)
: m_sizeA(sizeA)
, m_sizeB(sizeB)
, m_offsetX(offsetX)
, m_offsetY(offsetY)
{
   //////
}

void HP5DX_HackedAperture::GetValues(double &sizeA, double &sizeB, double &offsetX, double &offsetY)
{
   sizeA = m_sizeA;
   sizeB = m_sizeB;
   offsetX = m_offsetX;
   offsetY = m_offsetY;
}

//--------------------------------------------------------------------------------------
   
bool HP5DXPadstack::HackComplexAperture(int pageUnits, BlockStruct *cmplxAppSubBlock, double &sizeX, double &sizeY, double &offsetX, double &offsetY)
{
   HP5DX_HackedAperture *hackedAp = NULL;
   if (cmplxAppSubBlock != NULL && globalHackedApertureMap.Lookup(cmplxAppSubBlock->getBlockNumber(), hackedAp))
   {
      // Already hacked
      hackedAp->GetValues(sizeX, sizeY, offsetX, offsetY);
      return false; // means this is not a new hack
   }

   if (cmplxAppSubBlock)
   {
      doc->validateBlockExtents(cmplxAppSubBlock);
   }

   if (!cmplxAppSubBlock || cmplxAppSubBlock->getXmin() > cmplxAppSubBlock->getXmax())
   {
      sizeX = 0.01 * Units_Factor(UNIT_INCHES, pageUnits);
      sizeY = 0.01 * Units_Factor(UNIT_INCHES, pageUnits);
   }
   else
   {
      sizeX = cmplxAppSubBlock->getXmax() - cmplxAppSubBlock->getXmin();
      sizeY = cmplxAppSubBlock->getYmax() - cmplxAppSubBlock->getYmin();
      offsetX = cmplxAppSubBlock->getXmin() + (cmplxAppSubBlock->getXmax() - cmplxAppSubBlock->getXmin())/2;
      offsetY = cmplxAppSubBlock->getYmin() + (cmplxAppSubBlock->getYmax() - cmplxAppSubBlock->getYmin())/2;
   }

   hackedAp = new HP5DX_HackedAperture(sizeX, sizeY, offsetX, offsetY);
   globalHackedApertureMap.setAt(cmplxAppSubBlock->getBlockNumber(), hackedAp);

   return true; // fresh new hack
}

//--------------------------------------------------------------------------------------

void HP5DXPadstack::HackComplexAperture(int pageUnits, CPoly *poly, double &sizeX, double &sizeY, double &offsetX, double &offsetY)
{
   // These are harder to map because one block can have a lot of polys.
   // So we don't bother trying.

   CExtent polyExtent = poly->getExtent();

   offsetX = offsetY = 0.0;

   if (!polyExtent.isValid() || (polyExtent.getXmin() > polyExtent.getXmax()))
   {
      sizeX = 0.01 * Units_Factor(UNIT_INCHES, pageUnits);
      sizeY = 0.01 * Units_Factor(UNIT_INCHES, pageUnits);
   }
   else
   {
      sizeX = polyExtent.getXmax() - polyExtent.getXmin();
      sizeY = polyExtent.getYmax() - polyExtent.getYmin();
      offsetX = polyExtent.getXmin() + (polyExtent.getXmax() - polyExtent.getXmin())/2;
      offsetY = polyExtent.getYmin() + (polyExtent.getYmax() - polyExtent.getYmin())/2;
   }

   double testX = (sizeX/2.);
   double testY = (sizeY/2.);

   if (testX > 0. && testY > 0.)
   {
      CPoly testPoly;
//#define FIRST_TRY__IGNORES_OFFSET
#ifdef  FIRST_TRY__IGNORES_OFFSET
      testPoly.addVertex(-testX,-testY, 0. ); // start at lower left, go ccw
      testPoly.addVertex( testX,-testY, 0. );
      testPoly.addVertex( testX, testY, 0. );
      testPoly.addVertex(-testX, testY, 0. );
      testPoly.addVertex(-testX,-testY, 0. ); // close
#else
      // messing with offsets, this does seem to give a better result...
      testPoly.addVertex(-testX+offsetX,-testY+offsetY, 0. ); // start at lower left, go ccw
      testPoly.addVertex( testX+offsetX,-testY+offsetY, 0. );
      testPoly.addVertex( testX+offsetX, testY+offsetY, 0. );
      testPoly.addVertex(-testX+offsetX, testY+offsetY, 0. );
      testPoly.addVertex(-testX+offsetX,-testY+offsetY, 0. ); // close
#endif

      bool goodAp = poly->contains(testPoly, doc->getPageUnits());

      if (goodAp)
      {
         int jj = 0; // good, new ap is within old
      }
      else
      {
         // bad, new ap is not within old, try reducing size to make it fit.
         double reductionFactor = 0.01;  // start with 1% reduction
         double testX2 = testX;
         double testY2 = testY;

         while (!goodAp && reductionFactor < 0.65)  // will stop once crossing 65% reduction
         {
            testX2 = testX * (1.0 - reductionFactor);
            testY2 = testY * (1.0 - reductionFactor);

            CPoly testPoly2;
#ifdef FIRST_TRY__IGNORES_OFFSET
            testPoly2.addVertex(-testX2,-testY2, 0. ); // start at lower left, go ccw
            testPoly2.addVertex( testX2,-testY2, 0. );
            testPoly2.addVertex( testX2, testY2, 0. );
            testPoly2.addVertex(-testX2, testY2, 0. );
            testPoly2.addVertex(-testX2,-testY2, 0. ); // close
#else
            // experimenting with the offset...
            testPoly2.addVertex(-testX2+offsetX,-testY2+offsetY, 0. ); // start at lower left, go ccw
            testPoly2.addVertex( testX2+offsetX,-testY2+offsetY, 0. );
            testPoly2.addVertex( testX2+offsetX, testY2+offsetY, 0. );
            testPoly2.addVertex(-testX2+offsetX, testY2+offsetY, 0. );
            testPoly2.addVertex(-testX2+offsetX,-testY2+offsetY, 0. ); // close
#endif

            goodAp = poly->contains(testPoly2, doc->getPageUnits());

            if (goodAp)
            {
               int jjj = 0;
            }
            else
            {
               int jjj = 0;
               reductionFactor += 0.01;  // Shrink in 1% increments.
               // I tried doubling each time, but that shrinks too fast, makes bad fits.
            }
         }

         double originalX = sizeX; //*rcf debug
         double originalY = sizeY; //*rcf debug
         // If processing got us a good aperture per our test then use it.
         // If not then use original extent. Per discussion with Mark 8 Sep 2011, no  DR.
         if (goodAp)
         {
            // Processing made a keeper, since symmetrical about origin, X and X corner coord value is half of size.
            sizeX = testX2 * 2.;
            sizeY = testY2 * 2.;
         }
         // else sizeX and sizeY are already set to original extent, just keep them.
         int jjjj = 0;

      }
   }
}

//--------------------------------------------------------------------------------------

void HP5DXPadstack::GetPADSTACKData(BlockStruct *padstackBlock, int insertLayer, double scale, int punits)
{
   m_drill = 0;

   POSITION dataPos = padstackBlock->getHeadDataInsertPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = padstackBlock->getNextDataInsert(dataPos);

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         continue;
      if (data->getInsert()->getInsertType() == INSERTTYPE_VIA)
         continue;

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      point2.y = data->getInsert()->getOriginY() * scale;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (!block)
         continue;


      if (block->isTool())
      {
         m_drill = block->getToolSize();
      }
      else if (block->isAperture())
      {
         int layerIndex = Get_ApertureLayer(doc, data, block, insertLayer);

         // if multiple electrical layers
         int layerStack = GetStackNumFromIndex(layerIndex);
         int layerType = 0;
         if (layerStack == 1 || layerStack == LAY_TOP)
            layerType = 0x1;
         else if (layerStack > 1 || layerStack == LAY_BOT)
            layerType = 0x2;
         else if (layerStack == LAY_ALL)
            layerType = 0x3;

         double sizeA = block->getSizeA(), sizeB = block->getSizeB();
         double offsetX = block->getXoffset(), offsetY = block->getYoffset();
         ApertureShapeTag shape = block->getShape();

         if (block->getShape() == apertureComplex)
         {
            // If subblock has one item in data list
            //     if it is rectangle with rotation = 0, cool
            //         then make an R pad
            //     else
            //         do pad massage as before
            // else
            //     if can dissect pad
            //        then use dissected pad
            //     else
            //        do massage as before
            // 
            BlockStruct *subblock = doc->Find_Block_by_Num(block->getComplexApertureSubBlockNumber());

            if (subblock != NULL)
            {
               // ALWAYS USE DISSECT, it gives a better fit even for one poly in datalist. We used to
               // check locally here and handle the special case of only poly in datalist right here.
               // But it turns out the support under DissectComplexAperture() gives a better fit,
               // and no need to duplicate here what is "at the bottom" of that. So just always
               // use Dissect.

               bool hasAtleastOneHack = this->DissectComplexAperture(punits, subblock, layerType, 
                  data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle() + block->getRotation());
               sizeA = sizeB = offsetX = offsetY = 0.0; // Just clear them, they are not relevant for dissected complex.
               shape = apertureComplex;
            }
            else
            {
               // Complex aperture subblock is NULL, that is not supposed to happen.
               // Hack will make a default pad for this.
               // Returns true is new hack, false if not, but always works. Only new hacks to go log.
               if (HackComplexAperture(punits, subblock, sizeA, sizeB, offsetX, offsetY))
               {
                  // Is a new hack, report to log
                  fprintf(flog, "Visible Complex Aperture Graphic [%s] with no sub-block geometry converted to oblong, size [%lf, %lf] offset [%lf, %lf]\n",
                     block->getName(), sizeA, sizeB, offsetX, offsetY);
                  display_error++;
               }
               shape = apertureOblong;
            }

         }

         if (sizeB < SMALLNUMBER)
            sizeB = sizeA;

         this->Set(layerType, shape, sizeA, sizeB, offsetX + point2.x, offsetY + point2.y, data->getInsert()->getAngle() + block->getRotation(), 0.0, 0.0);
      }
   }
}

//-----------------------------------------------------------------------------

void HP5DXPadstack::SetTop(ApertureShapeTag shape, double sizeA, double sizeB, double offsetX, double offsetY, double rotationRadians)
{
   this->m_shapetypetop = shape;
   this->m_xsizetop = sizeA;
   this->m_ysizetop = sizeB;
   this->m_xofftop = offsetX;
   this->m_yofftop = offsetY;
   this->m_toprotation = rotationRadians;
}

//-----------------------------------------------------------------------------

void HP5DXPadstack::SetBot(ApertureShapeTag shape, double sizeA, double sizeB, double offsetX, double offsetY, double rotationRadians)
{
   this->m_shapetypebot = shape;
   this->m_xsizebot = sizeA;
   this->m_ysizebot = sizeB;
   this->m_xoffbot = offsetX;
   this->m_yoffbot = offsetY;
   this->m_botrotation = rotationRadians;
}

//-----------------------------------------------------------------------------

void HP5DXPadstack::Set(int layerType, ApertureShapeTag shape, double sizeA, double sizeB, double offsetX, double offsetY, double rotationRadians, double insertX, double insertY)
{
   // Same, regardless of layerType
   this->m_insertx = insertX;
   this->m_inserty = insertY;

   // Do not set m_layerType unless it is one of the cases 1..3.
   // We will pass through here when setting drill size, with layerType = 0.
   // The drill size is actually set outside of here, and we do not want
   // to change any of the other params when the action is just
   // adding a drill.

   // The rest vary by layerType
   if (layerType == 1)
   {
      this->m_layerType = layerType;
      this->SetTop(shape, sizeA, sizeB, offsetX, offsetY, rotationRadians);
   }
   else if (layerType == 2)
   {
      this->m_layerType = layerType;
      this->SetBot(shape, sizeA, sizeB, offsetX, offsetY, rotationRadians);
   }
   else if (layerType == 3)
   {
      this->m_layerType = layerType;
      this->SetTop(shape, sizeA, sizeB, offsetX, offsetY, rotationRadians);
      this->SetBot(shape, sizeA, sizeB, offsetX, offsetY, rotationRadians);
   }
}

//-----------------------------------------------------------------------------

bool HP5DXPadstack::GetOutputValues(double scale, char &formchar, double &dimx, double &dimy, double &offx, double &offy, double &aprot, double &insertx, double &inserty)
{
   // if Round / Oblong do C
   // if Square / Rectangle do R
   formchar = 'R';
   dimx = 0.0;
   dimy = 0.0;
   offx = 0.0;
   offy = 0.0;
   aprot = 0.0;  // is radians

   HP5DXPadstack *padstack = this;

   int padstackType = padstack->GetType(); 

   insertx = this->GetInsertX();
   inserty = this->GetInsertY();

   switch (padstackType)
   {
   case 1: 
      formchar = get_formchar(padstack->GetShapeTypeTop());
      dimx = padstack->GetXSizeTop() * scale;
      dimy = padstack->GetYSizeTop() * scale;
      aprot = padstack->GetTopRotation();
      offx = padstack->GetXOffTop() * scale;
      offy = padstack->GetYOffTop() * scale;
      break;

   case 2:
      formchar = get_formchar(padstack->GetShapeTypeBot());
      dimx = padstack->GetXSizeBot() * scale;
      dimy = padstack->GetYSizeBot() * scale;
      aprot = padstack->GetBotRotation();
      offx = padstack->GetXOffBot() * scale;
      offy = padstack->GetYOffBot() * scale;
      break;

   case 3:
      formchar = get_formchar(padstack->GetShapeTypeBot());
      dimx = padstack->GetXSizeBot() * scale;
      dimy = padstack->GetYSizeBot() * scale;
      aprot = padstack->GetBotRotation();
      offx = padstack->GetXOffBot() * scale;
      offy = padstack->GetYOffBot() * scale;
      break;

   default:
      return false; // failed
   }

   return true;
}

/******************************************************************************
* collect_padstacks
*/
static int collect_padstacks(double scale, int punits)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block != NULL && !block->isTool())
      {
         if (block->getBlockType() == BLOCKTYPE_PADSTACK)
         {
            HP5DXPadstack *c = new HP5DXPadstack(*block, -1, scale, punits);
            padstackarray.SetAtGrow(padstackcnt,c);  
            padstackcnt++;
         }
      }
   }

   return 1;
}

/******************************************************************************
* get_padstackindex
*/
static int get_padstackindex(const char *padstackName)
{
   for (int i=0; i<padstackcnt; i++)
   {
      HP5DXPadstack *padstack = padstackarray[i];
      if (!padstack->GetName().CompareNoCase(padstackName))
         return i;
   }

   fprintf(flog,"Padstack [%s] not found in padstack array\n", padstackName);
   display_error++;
   return -1;
}


