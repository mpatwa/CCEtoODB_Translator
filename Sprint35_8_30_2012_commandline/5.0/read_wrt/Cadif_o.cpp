// $Header: /CAMCAD/4.5/read_wrt/Cadif_o.cpp 47    8/05/06 4:53p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.

   A boardoutline is graphic marked as Primary Boardoutline or placed on a layer->type LAYTYPE_BOARDOUTLINE
    
*/

/*
* All changes done for TSRs are marked by "TSR ####" with a brief explanation
* of the changes done and their aim.
*
*  Recent Updates:
*  
*  04-03-02: TSR 3576 - Sadek Noureddine
*  04-29-02: TSR 3608 - Sadek Noureddine
*  05-08-02: Revamping - Sadek Noureddine
*
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
#include <direct.h>
#include "pcb_net.h"
#include "ck.h"
#include "pcblayer.h"
#include "cadif_o.h"
#include "apertur2.h"
#include "polylib.h"
#include "cadif_o.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#define   DUMP
#undef   DUMP              

extern CProgressDlg *progress; // from PORT.CPP
extern CViaArray *viaArray; // from PCB_NET.CPP
extern CSegmentArray *segmentArray; // from PCB_NET.CPP

static   CCEtoODBDoc        *doc;
static   FormatStruct      *format;
static   double            unitsFactor;
static   FILE              *flog;
static   int               display_error;
static   int               page_units;

static   CString           cadiflayer_file;

static   char              ident[80];
static   int               identcnt;

static   int               top, bot;         // L%d and L%d (1 and 50). This is the CADIF Index
static   int               max_signalcnt;    // this is the nunber of XRF layers

static   CCADIFLayerArray  mlArr;
static   int               maxArr = 0;

CStringArray typeNameArray;
int typeNameArrayCount;

static   ViaArray       viaarray;
static   int            viacnt, totalviacnt;

static   JunctionArray  junctionarray;
static   int            junctioncnt;

static   EndPointArray  endpointarray;
static   int            endpointcnt;

static   CompPinArray   comppinarray;
static   int            comppincnt;

static   LayerRepArray  layerreparray; // layer report file load
static   int            layerrepcnt;

static   CompArray      comparray;
static   int            compcnt;

static   PadLayerArray  padlayerarray;
static   int            padlayercnt;

static   TextArray      textarray;
static   int            textcnt;

static   NetArray       netarray;
static   int            netcnt;

static   PadStackArray  padstackarray;
static   int            padstackcnt;

static   TechnologyArray   technologyarray;
static   int            technologycnt;

static   PadShapeArray  padshapearray;
static   int            padshapecnt;

static   PadSymArray    padsymarray;
static   int            padsymcnt;

static   PadCodeRotArray      padcoderotarray;
static   int            padcoderotcnt;

static   double         one_mil; // in REDAC Units
static   int            drill_dwg;
static   int            coppercnt = 0;

static   int            COPPERTEMPLATE = TRUE;


static void CADIF_WriteData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale,
      int embeddedLevel, int insertlayer);
static void CADIF_GetTextData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale,
      int embeddedLevel, int insertlayer);
static void CADIF_GetPadRotData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale,
      int embeddedLevel, int insertlayer);
static int  CADIF_WriteSymData(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror,
      double scale, int insertlayer, int cnt);
static void CADIF_WriteAnnotate(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror,
      double scale, int insertlayer, int *cnt);
static void PCB_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
      double scale, int embeddedLevel, int insertLayer, const char *netname);
static void CADIF_WriteCopperData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
      double scale, int embeddedLevel, int insertLayer);
static void CADIF_WriteComponents(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror,
      double scale, int embeddedLevel, int insertLayer);
static int do_signalpinloc(FILE *fp, NetStruct *net, double scale);
static void Write_Traces(FILE *stream, CResultList *resultList, const char *netname);
static int GetTypeIndex(CString typeName);
static int check_layer_setup(const char *layerrepfile);
static int load_CADIFlayerfile(const char *fname);
static int update_textarray(double rot, int mirror, double height, double charwidth);
static int get_padcoderot(const char *p, int rot);
static int load_CADIFsettings(const CString fname);
static const char *select_technologyfile(const char *deffile);
static void do_layerlist();
static int edit_layerlist(CString tech_file);
static int write_cadifstart(FILE *fo,FileStruct *file, double xmin, double ymin, double xmax, double ymax);
static void free_layerlist();
static int FindPadShape(int bnum, int rot, int *angle);

/******************************************************************************
* CADIF_WriteFiles
*/
void CADIF_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits, double xmin, double ymin, double xmax, double ymax)
{
// format->CADIF_True_TextRot

   FILE *wfp;

   display_error = FALSE;

   CString logFile = GetLogfilePath("cadif.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   format = Format;
   doc = Doc;
   page_units = pageUnits;

   // 10-8 nanometer
   unitsFactor = Units_Factor(page_units, UNIT_MM) * 100000;

   // one mil in CADIF
   one_mil = Units_Factor(UNIT_MILS, UNIT_MM) * 100000;

   ident[0] = '\0';
   identcnt = 0;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString  tmp;
      tmp.Format("Can not open file [%s]",filename);
      ErrorMessage(tmp);
      return;
   }


   CString checkFile( getApp().getSystemSettingsFilePath("cadif.chk") );
   check_init(checkFile);

   viaarray.SetSize(10,10);
   viacnt = 0;

   junctionarray.SetSize(10,10);
   junctioncnt = 0;

   endpointarray.SetSize(10,10);
   endpointcnt = 0;

   comppinarray.SetSize(10,10);
   comppincnt = 0;

   layerreparray.SetSize(10,10);
   layerrepcnt = 0;

   netarray.SetSize(10,10);
   netcnt = 0;

   padstackarray.SetSize(10,10);
   padstackcnt = 0;

   technologyarray.SetSize(10,10);
   technologycnt = 0;

   padshapearray.SetSize(10,10);
   padshapecnt = 0;

   padsymarray.SetSize(10,10);
   padsymcnt = 0;

   padcoderotarray.SetSize(10,10);
   padcoderotcnt = 0;

   comparray.SetSize(10,10);
   compcnt = 0;

   typeNameArray.SetSize(10,10);
   typeNameArrayCount = 0;

   textarray.SetSize(10,10);
   textcnt = 0;

   padlayerarray.SetSize(10,10);
   padlayercnt = 0;

   mlArr.SetSize(100,100);
   maxArr = 0;

   CString settingsFile( getApp().getExportSettingsFilePath("cadif.out") );
   load_CADIFsettings(settingsFile);

   if (technologycnt)
      cadiflayer_file = select_technologyfile(cadiflayer_file);   // cadif.out can have multiple technology files
                        // and select counts signal and power layers and figures out a matching file.


   // here find out which setup file get's used.
   char  curdir[_MAX_PATH];

   /* Get the current working directory: */
   if( _getcwd( curdir, _MAX_PATH ) == NULL )   
   {
      CString  tmp;
      tmp.Format( "Problem get current directory");
      MessageBox(NULL, "Error Get Directory !", tmp, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char name[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( cadiflayer_file, drive, dir, name, ext );

   if (strlen(drive) == 0 && strlen(dir) == 0)
   {
      char f_name[_MAX_PATH];

      _makepath( f_name, drive, curdir, name, ext );
      cadiflayer_file = f_name;
   }

   // this is now the absolute path.
   if (!load_CADIFlayerfile(cadiflayer_file))
   {
      cadiflayer_file = "";
/* no message
      CString  tmp;
      tmp.Format("Can not open .Layerfile [%s]",cadiflayer_file);
      ErrorMessage(tmp);
*/
   }

   do_layerlist();

   coppercnt = 0;
   int pcbsFound = 0;
   FileStruct *pcbFile;

   // here loop for only one PCB file
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         pcbsFound++;
         pcbFile = file;
         //net_layerlist(file); what is this needed in  ????
      }
   }

   if (pcbsFound > 1)
      ErrorMessage("More than one PCB file found!","", MB_OK | MB_ICONHAND);

   if (edit_layerlist(cadiflayer_file))
   {
      if (check_layer_setup(cadiflayer_file) == 0) // no error 
      {
         // not implemented
         //do_drawings(file, autodrawingfp);
         CADIF_GetTextData(&pcbFile->getBlock()->getDataList(), pcbFile->getInsertX(), pcbFile->getInsertY(), pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale(), -1, -1);

         // also get all blockdata
			int i=0;
         for (i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block == NULL)
               continue;

            if (block->getFlags() & BL_WIDTH)
               continue;

            CADIF_GetTextData(&block->getDataList(), 0.0, 0.0, 0.0, 0, 1, -1, -1);
         }
         // also get all blockdata
         for (i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block == NULL)
               continue;

            if (block->getFlags() & BL_WIDTH)
               continue;

            if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT || block->getBlockType() == BLOCKTYPE_TESTPOINT)
               CADIF_GetPadRotData(&block->getDataList(), 0.0, 0.0, 0.0, 0, 1, -1, -1);
         }

         if (textcnt == 0)
         {
            // need to add one, just in case the file has not font.
            update_textarray(0, 0, 50*Units_Factor(UNIT_MILS, page_units), 10*Units_Factor(UNIT_MILS, page_units));
         }

         if (format->Output) // 0 = PCB, 1 = GRAPHIC
         {
            GenerateMissingTypes(doc, pcbFile);
            write_cadifstart(wfp, pcbFile, xmin, ymin, xmax, ymax);
         }
         else
         {
            // PCB translation
            pcbsFound = 0;
            POSITION filePos = doc->getFileList().GetHeadPosition();
            while (filePos)
            {
               FileStruct *file = doc->getFileList().GetNext(filePos);

               if (!file->isShown())
                  continue;

               if (file->getBlockType() == BLOCKTYPE_PCB)  
               {
                  pcbsFound++;
                  generate_PINLOC(doc, file, 0);   // this function generates the PINLOC argument for all pins.
                  GenerateMissingTypes(doc, file);
                  write_cadifstart(wfp, file, xmin, ymin, xmax, ymax);
               }
            }

            if (!pcbsFound)
               ErrorMessage("No PCB file found !","", MB_OK | MB_ICONHAND);
         } // PCB or Graphics
      } // check layer setup
   } // edit layer

   free_layerlist();

   // close write file
   fclose(wfp);

   if (check_report(flog)) display_error++;
   check_deinit();
   fclose(flog);

   int i=0;
   for (i=0;i<viacnt;i++)
   {
      delete viaarray[i];
   }
   viacnt = 0;
   viaarray.RemoveAll();

   for (i=0;i<junctioncnt;i++)
   {
      delete junctionarray[i];
   }
   junctioncnt = 0;
   junctionarray.RemoveAll();

   for (i=0;i<endpointcnt;i++)
   {
      delete endpointarray[i];
   }
   endpointcnt = 0;
   endpointarray.RemoveAll();

   for (i=0;i<comppincnt;i++)
   {
      delete comppinarray[i];
   }
   comppincnt = 0;
   comppinarray.RemoveAll();

   for (i=0;i<layerrepcnt;i++)
   {
      delete layerreparray[i];
   }
   layerrepcnt = 0;
   layerreparray.RemoveAll();

   for (i=0;i<netcnt;i++)
   {
      delete netarray[i];
   }
   netcnt = 0;
   netarray.RemoveAll();

   for (i=0;i<compcnt;i++)
   {
      delete comparray[i];
   }
   compcnt = 0;
   comparray.RemoveAll();

   typeNameArrayCount = 0;
   typeNameArray.RemoveAll();

   for (i=0;i<textcnt;i++)
   {
      delete textarray[i];
   }
   textcnt = 0;
   textarray.RemoveAll();

   for (i=0;i<padstackcnt;i++)
   {
      delete padstackarray[i];
   }
   padstackarray.RemoveAll();
   padstackcnt = 0;

   for (i=0;i<technologycnt;i++)
   {
      delete technologyarray[i];
   }
   technologyarray.RemoveAll();
   technologycnt = 0;

   for (i=0;i<padshapecnt;i++)
   {
      delete padshapearray[i];
   }
   padshapearray.RemoveAll();
   padshapecnt = 0;

   for (i=0;i<padsymcnt;i++)
   {
      delete padsymarray[i];
   }
   padsymarray.RemoveAll();
   padsymcnt = 0;

   for (i=0;i<padcoderotcnt;i++)
   {
      delete padcoderotarray[i];
   }
   padcoderotarray.RemoveAll();
   padcoderotcnt = 0;

   // the contens delete is done in cadif_padcodelib
   padlayerarray.RemoveAll();

   if (display_error)
      Logreader(logFile);
}


/******************************************************************************
* cadif_name
* convert quotes to \"
*/
static CString cadif_name(const char *origName)
{
   CString newName;

   for (int i=0; i<(int)strlen(origName); i++)
   {
      if (origName[i] == '"')
         newName += '\\';
      newName += origName[i];
   }

   return newName;
}

/******************************************************************************
* cnv_unit
*/
static long cnv_unit(double x)
{
   long l = round(x * unitsFactor);
   return l;
}

/******************************************************************************
* cnv_rot
*/
static int cnv_rot(double rot)
{
   int r = round(rot);

   while (r < 0)  
      r = r + 360;
   while (r >= 360)  
      r = r - 360;

   return r;
}

/******************************************************************************
* cnv_arc
*/
static int cnv_arc(double arc)
{
   int r = round(RadToDeg(arc));

   return r;
}

/******************************************************************************
* is_layertype_electrical
*/
static int is_layertype_electrical(const char *layertype)
{
   if (!STRCMPI(layertype,"ELECTRICAL") ||
       !STRCMPI(layertype,"SPLIT_PLANE") ||
       !STRCMPI(layertype,"POWER_PLANE"))
      return 1;

   return 0;
}

/******************************************************************************
* get_defaultlayer
*/
static const char *get_defaultlayer(const char *l, int layer_type, int *l_index)
{
   char  *ltype = "cc";

   *l_index = -1;

   if (layer_type == LAYTYPE_SILK_TOP)
      ltype =  "SILKSCREEN_TOP";
   if (layer_type == LAYTYPE_SILK_BOTTOM)
      ltype =  "SILKSCREEN_BOT";
   if (layer_type == LAYTYPE_MASK_ALL)
      ltype =  "MASK_ALL";
   if (layer_type == LAYTYPE_MASK_TOP)
      ltype =  "MASK_TOP";
   if (layer_type == LAYTYPE_MASK_BOTTOM)
      ltype =  "MASK_BOT";
   if (layer_type == LAYTYPE_PASTE_TOP)
      ltype =  "PASTE_TOP";
   if (layer_type == LAYTYPE_PASTE_BOTTOM)
      ltype =  "PASTE_BOT";
   if (layer_type == LAYTYPE_BOARD_OUTLINE)
      ltype =  "BOARDOUTLINE";

   for (int i=0;i<layerrepcnt;i++)
   {
      CADIFLayerrepStruct *lyrRep = layerreparray[i];

      if (!lyrRep->originallayertype.CompareNoCase(ltype))
      {
         *l_index = lyrRep->layerindex;
         return lyrRep->layername;
      }
   }

   return l;
}

/******************************************************************************
* get_netindex
* Attention. the S%d number is netindex + 1
*/
static int get_netindex(const char *n)
{

   for (int i=0;i<netcnt;i++)
   {
      if (netarray[i]->netname.CompareNoCase(n) == 0)
         return i;
   }
   ErrorMessage("Netname not found!");

   return -1;
}

/******************************************************************************
* get_layernr_from_index
*/
static   int   get_layernr_from_index(int index)
{
   CADIFLayerStruct *tmp;

   for (int i=0;i<maxArr;i++)
   {
      tmp = mlArr[i];
      if (tmp->layerindex == index)
         return tmp->stackNum;
   }

   return -99;
}

/******************************************************************************
* get_layercadifindex
*/
static const char *get_layercadifindex(int stacknum)
{
   CADIFLayerStruct *tmp;

   for (int i=0;i<maxArr;i++)
   {
      tmp = mlArr[i];

      if (tmp->stackNum == stacknum)
         return tmp->newName;
   }

   return "";
}

/******************************************************************************
* Layer_CADIF
* return NULL if not visible
*/
static const char *Layer_CADIF(int l)
{

   for (int i=0;i<maxArr;i++)
   {
      if (mlArr[i]->on == 0)  continue;   // not visible
      if (mlArr[i]->layerindex == l)
         return mlArr[i]->newName;
   }

   //fprintf(flog,"Layer [%s] not translated.\n", doc->LayerArray[l]->name);
   return NULL;
}

/******************************************************************************
* Get_Layerrep_Stacknum_Ptr
* find the count number of signal layer, not the exact stack number
*/
static int Get_Layerrep_Stacknum_Ptr(int count, const char *ltype)
{
   int   cnt = 0;

   for (int i=0;i<layerrepcnt;i++)
   {
      if (!layerreparray[i]->layertype.CompareNoCase(ltype))
      {
         cnt++;
         if (count == cnt)
            return i;
      }
   }

   return -1;
}

/******************************************************************************
* Get_Layerrep_Powerplane_Ptr
* find the count number of power plane, not the exact stack number
*/
static int Get_Layerrep_Powerplane_Ptr(int count, const char *ltype)
{
   int   cnt = 0;

   for (int i=0;i<layerrepcnt;i++)
   {
      if (!layerreparray[i]->layertype.CompareNoCase(ltype))
      {
         cnt++;
         if (count == cnt)
            return i;
      }
   }

   return -1;
}

/******************************************************************************
* check_cadiflayer_to_layrep
* check that a layer in the layer edit is defined in the loaded
* technology file.
*/
static int check_cadiflayer_to_layrep(const char *l)
{

   if (!STRCMPI(l,"L0"))         return 1;   // always defined
   if (!STRCMPI(l,"elecLayers")) return 1;   // always defined

   for (int i=0;i<layerrepcnt;i++)
   {
      CString  lname;
      CADIFLayerrepStruct *lyrRep = layerreparray[i];
      lname.Format("L%d", lyrRep->layerindex);
      if (lname.CompareNoCase(l) == 0)
         return 1;
   }

   return 0;
}

/******************************************************************************
* sort_technologyfile
*/
static void sort_technologyfile()
{
   int   done = FALSE;

   while (!done)
   {
      done = TRUE;
      for (int i=1;i<technologycnt;i++)
      {
         if (technologyarray[i]->signalcnt < technologyarray[i-1]->signalcnt)
         {
            // change
            CADIFTechnologyStruct *techArray = technologyarray[i];
            technologyarray[i] = technologyarray[i-1];
            technologyarray[i-1] = techArray;
            done = FALSE;
         }
      }
   }
   return;
}

/******************************************************************************
* select_technologyfile
*/
static const char *select_technologyfile(const char *deffile)
{
   int         sigcnt = 0;
   int         powercnt = 0;
   LayerStruct *layer;
	int j=0;

   for (j=0; j< doc->getMaxLayerIndex(); j++) 
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.

      if (layer->getElectricalStackNumber())
      {
         if (layer->getElectricalStackNumber() > sigcnt)
            sigcnt = layer->getElectricalStackNumber();

         if (layer->getLayerType() == LAYTYPE_POWERNEG || layer->getLayerType() == LAYTYPE_SPLITPLANE)
         {
            powercnt++;
         }
      }
   }

   // sort technology for lowest to highest signalcnt
   sort_technologyfile();

   // now find a appropriate 
   for (j=0;j<technologycnt;j++)
   {
      CADIFTechnologyStruct *techArray = technologyarray[j];
      if (techArray->signalcnt < sigcnt)
         continue;
      if (techArray->powercnt < powercnt) 
         continue;
      return techArray->filename;
   }

   fprintf(flog, "No matching Technology file found for Signal %d Power %d Layers -> use default [%s]\n",
      sigcnt, powercnt, deffile);
   display_error++;

   return deffile;
}

/******************************************************************************
* do_layerlist
*/
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0;
   char        typ = 'D';
   CString     cadiflay;
   int         planecnt = 0;

   maxArr = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.
      signr = 0;

      int index;
      cadiflay = get_defaultlayer(layer->getName(), layer->getLayerType(), &index);
      if (index > -1)
         cadiflay.Format("L%d", index);

      typ = 'D';

      if (layer->isFloating())
      {
         cadiflay = "L0";
      }
      else
      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();

         if (layer->getLayerType() == LAYTYPE_PAD_TOP)
         {
            signr = LAY_TOP;
            cadiflay.Format("L%d", top);
         }
         else
         if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
         {
            signr = LAY_BOT;
            cadiflay.Format("L%d", bot);
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
            cadiflay.Format("L%d", top);
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
            cadiflay.Format("L%d", bot);
         else
         {
            if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER ||
                layer->getLayerType() == LAYTYPE_SIGNAL || layer->getLayerType() == LAYTYPE_POWERPOS)
            {
               int   lptr = Get_Layerrep_Stacknum_Ptr(signr, "ELECTRICAL");
               if (lptr > -1)
                  cadiflay.Format("L%d", layerreparray[lptr]->layerindex);
               else
               {
                  cadiflay.Format("No ELECTRICAL layer found for stacknr [%d]", signr);
               }
            }
            else
            if (layer->getLayerType() == LAYTYPE_POWERNEG || layer->getLayerType() == LAYTYPE_SPLITPLANE)
            {
               planecnt++;
               int   lptr = Get_Layerrep_Powerplane_Ptr(planecnt, "POWER_PLANE");
               if (lptr > -1)
                  cadiflay.Format("L%d", layerreparray[lptr]->layerindex);
               else
               {
                  cadiflay.Format("No POWER_PLANE layer found for stacknr [%d]", signr);
               }
            }
         }
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
         cadiflay = "elecLayers";
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
         cadiflay = "elecLayers";
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         signr = LAY_INNER;
         cadiflay = "SIGNAL";
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         signr = LAY_TOP;
         cadiflay.Format("L%d", top);
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         signr = LAY_BOT;
         cadiflay.Format("L%d", bot);
      }

      if (signr == 0)
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
      }

      //mlArr.SetSizes
      CADIFLayerStruct *ml = new CADIFLayerStruct;
      ml->stackNum = signr;
      ml->layerindex = j;
      ml->on = TRUE;
      ml->type = typ;
      ml->oldName = layer->getName();
      ml->newName = cadiflay;
      mlArr.SetAtGrow(maxArr++, ml);
   }

   return;
}

/******************************************************************************
* CADIF_LoadTechnology
* from PCBLayer 
*/
int CADIF_LoadTechnology(const char *f)
{

   for (int i=0;i<layerrepcnt;i++)
   {
      delete layerreparray[i];
   }
   layerrepcnt = 0;

   load_CADIFlayerfile(f);

   return 1;
}

/******************************************************************************
* edit_layerlist
*/
static int edit_layerlist(CString tech_file)
{
   // fill array
   CADIFLayer mldlg;

   while (TRUE)
   {
      mldlg.arr = &mlArr;
      mldlg.maxArr = maxArr;
      mldlg.m_technologyName = tech_file;

      if (mldlg.DoModal() != IDOK) 
         return FALSE;
      tech_file = mldlg.m_technologyName;

      max_signalcnt = 0;

      for (int i=0;i<maxArr;i++)
      {
         if (mlArr[i]->stackNum > max_signalcnt)
            max_signalcnt = mlArr[i]->stackNum;
      }

      if (check_layer_setup(tech_file) == 0) // no error 
         return TRUE;
   }  // continue until no error.

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

/******************************************************************************
* load_CADIFsettings
*/
static int load_CADIFsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   cadiflayer_file = getApp().getSystemSettingsFilePath("cadif_layer.rep");

   COPPERTEMPLATE = TRUE;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"CADIF Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         // here commands
         if (!STRCMPI(lp,".LAYERFILE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            cadiflayer_file = lp;
         }
         else
         if (!STRCMPI(lp,".TECHNOLOGYFILE"))
         {
            int   sig, power;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            sig = atoi(lp);
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            power = atoi(lp);
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            CADIFTechnologyStruct *c = new CADIFTechnologyStruct;
            technologyarray.SetAtGrow(technologycnt, c);
            technologycnt++;
            c->filename = lp;
            c->filename.TrimLeft();
            c->filename.TrimRight();
            c->powercnt = power;
            c->signalcnt = sig;
         }
         else
         if (!STRCMPI(lp,".COPPERTEMPLATE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            if (lp[0] == 'N' || lp[0] == 'n')
               COPPERTEMPLATE = FALSE;
         }
      }
   }

   fclose(fp);
   return 1;
}

/******************************************************************************
* get_layerusage
*/
static const char *get_layerusage(const char *l)
{

   if (l == NULL)       return "DOCUMENT";
   if (strlen(l) == 0)  return "DOCUMENT";

   // these names come from the CADSTAR layer report and from RSI layer rep file
   if (!STRCMPI(l, "Non-Electrical"))  return "DOCUMENT";
   if (!STRCMPI(l, "Documentation"))   return "DOCUMENT";
   if (!STRCMPI(l, "SilkScreen_Top"))  return "DOCUMENT";
   if (!STRCMPI(l, "SilkScreen_Bot"))  return "DOCUMENT";
   if (!STRCMPI(l, "Paste_Top"))       return "DOCUMENT";
   if (!STRCMPI(l, "Paste_Bot"))       return "DOCUMENT";
   if (!STRCMPI(l, "Mask_Top"))        return "DOCUMENT";
   if (!STRCMPI(l, "Mask_Bot"))        return "DOCUMENT";
   if (!STRCMPI(l, "Mask_All"))        return "DOCUMENT";

   for (int i=0;i<SIZ_LAYER_USAGE;i++)
   {
      if (!STRCMPI(l, layer_usage[i]))
         return layer_usage[i];
   }

   fprintf(flog, "Unknown LAYER USAGE Type [%s]\n", l);
   display_error++;
   return "DOCUMENT";
}

/******************************************************************************
* load_CADIFlayerfile
*/
static int load_CADIFlayerfile(const char *fname)
{
   FILE  *fp;
   char  line[255];

   drill_dwg = -1;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"CADIF Layer File", MB_OK | MB_ICONHAND);
      return 0;
   }
  
   int   layerindex;
   CString  layername;
   CString  layertype;           // this is a redac layertype as it is in the CADIF file
   CString  originallayertype;   // this is an enhanced layertype, so that CAMCAD can recognize it.
   CString  swaplayer;
   CString  material;
   double   thickness;
   CString  embedding;
   CString  routingbias;
   int      stacknum = 0;

   while (fgets(line,255,fp))
   {
      char *lp;
      if ((lp = get_string(line," \t\n")) == NULL) continue;

      if (!STRNICMP(lp,".UNITS", 3))
      {
      }
      else
      if (!STRCMPI(lp,".LAYER"))
      {
         char  *lp;
         if ((lp = get_string(NULL,",")) == NULL)  continue;
         layerindex = atoi(lp);
         layername = get_string(NULL,",");
         layername.TrimLeft();
         layername.TrimRight();

         layertype = get_string(NULL,",");
         layertype.TrimLeft();
         layertype.TrimRight();
         originallayertype = layertype;
         layertype = get_layerusage(layertype);

         swaplayer = get_string(NULL,",");
         swaplayer.TrimLeft();
         swaplayer.TrimRight();

         material = get_string(NULL,",");
         material.TrimLeft();
         material.TrimRight();

         lp = get_string(NULL,",");
         thickness = atof(lp);

         embedding = get_string(NULL,",");
         embedding.TrimLeft();
         embedding.TrimRight();

         routingbias = get_string(NULL,",");
         routingbias.TrimLeft();
         routingbias.TrimRight();

         if (routingbias.CompareNoCase("X") && routingbias.CompareNoCase("Y"))
         {
            routingbias = "";
         }

         CADIFLayerrepStruct *l = new CADIFLayerrepStruct;
         layerreparray.SetAtGrow(layerrepcnt, l);
         layerrepcnt++;
         l->layerindex = layerindex;
         l->layername = layername;
         l->originallayertype = originallayertype;
         l->layertype = layertype;
         l->swaplayer = swaplayer;
         l->material = material;
         l->thickness = thickness;
         l->embedding = embedding;
         l->routingbias = routingbias;
         l->stacknum = 0;

         if (is_layertype_electrical(layertype))
             l->stacknum = ++stacknum;

         if (layertype.CompareNoCase("DRILL_DWG") == 0)
         {
            drill_dwg = layerrepcnt-1;
         }
      }
   }

   fclose(fp);

   // find top and bottom layer
   top = -1;
   bot = -1;
   for (int i=0;i<layerrepcnt;i++)
   {
      if (layerreparray[i]->layertype.CompareNoCase("ELECTRICAL"))   // if not electrical
            continue;   
      if (top < 0)   top = layerreparray[i]->layerindex; // store the first
      bot = layerreparray[i]->layerindex;
   }

   if (drill_dwg < 0)
   {
      CString  tmp;
      tmp.Format("Error Reading [%s]\n", fname);
      ErrorMessage("No Layer with Type [DRILL_DWG] defined!", tmp);
      return 0;
   }

   return 1;
}

/******************************************************************************
* plusident
*/
static int plusident()
{
   if (identcnt < 80)
   {
      ident[identcnt] = ' ';
      ident[++identcnt] = '\0';
   }
   else
   {
      fprintf(flog, "Error in plus ident\n");
      display_error++;
      return -1;
   }
   return 1;
}

/******************************************************************************
* minusident
*/
static int minusident()
{
   if (identcnt > 0)
   {
      ident[--identcnt] = '\0';
   }
   else
   {
      fprintf(flog, "Error in minus ident\n");
      display_error++;
      return -1;
   }
   return 1;
}

/******************************************************************************
* close_b
*/
static int close_b(FILE *fo)
{
   minusident(); 
   fprintf(fo,"%s)\n",ident);
   return 1;
}

/******************************************************************************
* write_cadifformat
*/
static int write_cadifformat(FILE *fo)
{
   fprintf(fo,"%s(format CADIF 3 0)\n",ident);
   return 1;
}

/******************************************************************************
* write_cadifdataset
*/
static int write_cadifdataset(FILE *fo)
{
   fprintf(fo,"%s(dataSet ARCHIVE XFER_FILE ROUTE_RULES PLACE_RULES)\n",ident);
   return 1;
}

/******************************************************************************
* write_cadifpaper
*/
static int write_cadifpaper(FILE *fo, double xmin, double ymin, double xmax, double ymax)
{
   // needs the largest extent
   double   x = fabs(xmin) ,y = fabs(ymin);

   if (x < fabs(xmax))  x = fabs(xmax);
   if (y < fabs(ymax))  y = fabs(ymax);

   fprintf(fo,"%s(paper (name \"B\")\n", ident);
   plusident();
   fprintf(fo,"%s(box (pt %ld %ld) (pt %ld %ld))\n", ident,
      cnv_unit(-x), cnv_unit(-y), cnv_unit(x), cnv_unit(y));
   fprintf(fo,"%s(designOrigin (pt 0 0))\n", ident);
   fprintf(fo,"%s(paperScale (e 1 0))\n", ident);
   close_b(fo);
   return 1;
}

/******************************************************************************
* write_cadifsignallist
*/
static int write_cadifsignallist(FILE *fo, FileStruct *file)
{
   fprintf(fo,"%s(signallist\n", ident);
   plusident();

   NetStruct *net;
   POSITION  netPos;
   
   //wmen_Graph_Date(flog,"Start trace");
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;
      
      CADIFNetStruct *n = new CADIFNetStruct;
      n->netname = net->getNetName();    
      netarray.SetAtGrow(netcnt, n);
      netcnt++;

      fprintf(fo,"%s(signal S%d (name \"%s\"))\n", ident, netcnt, cadif_name(check_name('n', net->getNetName())));
   }

   close_b(fo);
   return 1;
}

/******************************************************************************
* write_cadiftextinfo
*/
static int write_cadiftextinfo(FILE *fo)
{
   fprintf(fo,"%s(textInfo\n", ident);
   plusident();
	
	int i=0;
   for (i=0;i<textcnt;i++)
   {
      if (i == 0) // this is how visula needs it !!!
         fprintf(fo,"%s(font FNT%d (name \"standard\")\n", ident, i);
      else
         fprintf(fo,"%s(font FNT%d (name \"std%d\")\n", ident, i, i);
      plusident();   // clsoe_b makes an automatic minusident()

/*    
      fprintf(fo,"%s(fontDesc\n", ident);
      plusident();
      fprintf(fo,"%s(refHeight %ld)\n", ident, cnv_unit(textarray[i]->height));
      fprintf(fo,"%s(refWidth %ld)\n", ident, cnv_unit(textarray[i]->charwidth));
      close_b(fo);
*/
      close_b(fo);
   }

   for (i=0;i<textcnt;i++)
   {
      fprintf(fo,"%s(textStyle TS%d\n", ident, i);
      plusident();
      fprintf(fo,"%s(fontRef FNT%d)\n", ident, i);
      fprintf(fo,"%s(height %ld)\n", ident, cnv_unit(textarray[i]->height));
      fprintf(fo,"%s(justify JUST_START)\n", ident);

      /*TSR 3608: text rotation used to be written in radians!*/
      int rot = cnv_rot(RadToDeg(textarray[i]->rotation));

      if (textarray[i]->mirror)
      {
         rot = cnv_rot(RadToDeg(PI2 - textarray[i]->rotation));
      }

      if (rot) fprintf(fo,"%s(orientation %d)\n", ident, rot);
      if (textarray[i]->mirror)  
      {
         fprintf(fo,"%s(mirrored)\n", ident);
      }
      close_b(fo);
   }

   close_b(fo);
   return 1;
}

/******************************************************************************
* write_cadifzoneinfo
*/
static int write_cadifzoneinfo(FILE *fo)
{
   fprintf(fo,"%s(zoneInfo\n", ident);
   plusident();
   fprintf(fo,"%s(box (pt -15900 -15900) (pt 15900 15900))\n", ident);
   fprintf(fo,"%s(hZone 1 ZONE_ALPHA)\n", ident);
   fprintf(fo,"%s(vZone 1 ZONE_NUM)\n", ident);
   close_b(fo);
   return 1;
}

/******************************************************************************
* write_cadifstyleinfo
*/
static int write_cadifstyleinfo(FILE *fo)
{
   fprintf(fo,"%s(styleInfo\n", ident);
   plusident();
   fprintf(fo,"%s(styleSize 50800)\n", ident);
   fprintf(fo,"%s(style STYLE_SOLID -1)\n", ident);
   fprintf(fo,"%s(style STYLE_DOTTED -1431655766)\n", ident);
   fprintf(fo,"%s(style STYLE_SHORT_DASH -286331154)\n", ident);
   fprintf(fo,"%s(style STYLE_DASH -50529028)\n", ident);
   fprintf(fo,"%s(style STYLE_LONG_DASH -458760)\n", ident);
   fprintf(fo,"%s(style STYLE_SHORT_CHAIN -327686)\n", ident);
   fprintf(fo,"%s(style STYLE_LONG_CHAIN -100)\n", ident);
   fprintf(fo,"%s(style STYLE_SHORT_PHANTOM -1376278)\n", ident);
   fprintf(fo,"%s(style STYLE_LONG_PHANTOM -3172)\n", ident);
   close_b(fo);
   return 1;
}

/******************************************************************************
* write_cadiflayerstack
*/
static int write_cadiflayerstack(FILE *fo)
{
   int   i;

   fprintf(fo,"%s(layerStack\n", ident);
   plusident();

   fprintf(fo,"%s(layer L0 (name \"\") (id 0)\n", ident);
   plusident();
   fprintf(fo,"%s(layerDesc\n", ident);
   plusident();
   fprintf(fo,"%s(layerUsage UNLAYERED)\n", ident);
   fprintf(fo,"%s(allows)\n", ident);
   fprintf(fo,"%s(visible)\n", ident);
   close_b(fo);
   close_b(fo);

   for (i=0;i<layerrepcnt;i++)
   {
      fprintf(fo,"%s(layer L%d (name \"%s\") (id %d)\n", ident, 
         layerreparray[i]->layerindex, cadif_name(layerreparray[i]->layername), layerreparray[i]->layerindex);
      plusident();
      fprintf(fo,"%s(layerDesc\n", ident);
      plusident();
      if (strlen(layerreparray[i]->layertype))
         fprintf(fo,"%s(layerUsage %s)\n", ident, layerreparray[i]->layertype);
      fprintf(fo,"%s(allows (allowRoute) (allowCopper) (allowAnnotn) (allowConstr) (allowFigure) (allowText) (allowHatch))\n", ident);
      if (strlen(layerreparray[i]->routingbias))
         fprintf(fo,"%s(routeBias BIAS_%s)\n", ident, layerreparray[i]->routingbias);
      fprintf(fo,"%s(visible)\n", ident);
      close_b(fo);
      close_b(fo);
   }

   close_b(fo);

   fprintf(fo,"%s(globalLayers\n", ident);
   plusident();

   fprintf(fo,"%s(glblayer GL0 (name \"\") (id 0)\n", ident);
   plusident();
   fprintf(fo,"%s(layerUsage UNLAYERED)\n", ident);
   fprintf(fo,"%s(allows)\n", ident);
   fprintf(fo,"%s(layerRef L0)\n", ident);
   close_b(fo);

   for (i=0;i<layerrepcnt;i++)
   {
      fprintf(fo,"%s(glbLayer GL%d (name \"%s\") (id %d)\n", ident, 
         layerreparray[i]->layerindex, cadif_name(layerreparray[i]->layername), layerreparray[i]->layerindex);
      plusident();
      if (strlen(layerreparray[i]->layertype))
         fprintf(fo,"%s(layerUsage %s)\n", ident, layerreparray[i]->layertype);
      // (allows (allowRoute) (allowCopper) (allowAnnotn) (allowConstr) (allowFigure) (allowText) (allowHatch))
      fprintf(fo,"%s(allows (allowRoute) (allowCopper) (allowAnnotn) (allowConstr) (allowFigure) (allowText) (allowHatch))\n", ident);
      fprintf(fo,"%s(layerRef L%d)\n", ident, layerreparray[i]->layerindex);
      close_b(fo);
   }

   close_b(fo);
   return 1;
}

/******************************************************************************
* get_padshape
*/
static int get_padshape(int bnum, int rot, int degreeAngle)
{
   for (int i=0; i<padshapecnt; i++)
   {
      CADIFPadShapeStruct *padshape = padshapearray[i];

      if (padshape->bnum == bnum && padshape->rot == rot && padshape->degreeAngle == degreeAngle)
         return i;
   }

   CADIFPadShapeStruct *padshape = new CADIFPadShapeStruct;
   padshapearray.SetAtGrow(padshapecnt++, padshape);

   padshape->bnum = bnum;
   padshape->rot = rot;
   padshape->degreeAngle = degreeAngle; //the insert angle
   return padshapecnt-1;
}

/******************************************************************************
* FindPadShape
*/

static int FindPadShape(int bnum, int rot, int *angle)
{
   /*TSR 3576: The *angle parameter is added to get the insert angle from the padshapearray*/
   for (int i=0; i<padshapecnt; i++)
   {
      CADIFPadShapeStruct *padshape = padshapearray[i];
      if (padshape->bnum == bnum && padshape->rot == rot) 
      {
         *angle = padshape->degreeAngle;
         return i; 
      }
   }
   return -1; //Can't find pad shape
}

/******************************************************************************
* get_padsym
*/
static int get_padsym(int bnum, int rot)
{
   for (int i=0;i<padsymcnt;i++)
   {
      CADIFPadSymStruct *padSymStruct = padsymarray[i];

      if (padSymStruct->bnum == bnum && padSymStruct->rot == rot)
         return i;
   }

   CADIFPadSymStruct *padSymStruct = new CADIFPadSymStruct;
   padsymarray.SetAtGrow(padsymcnt, padSymStruct);
   padsymcnt++;

   padSymStruct->bnum = bnum;
   padSymStruct->rot =  rot;

   return padsymcnt-1;
}

/******************************************************************************
* write_simple_aperture
*/
static void write_simple_aperture (FILE *wfp, BlockStruct *block, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel)
{
   POSITION polyPos, pntPos;
   CPolyList *polylist;
   CPoly *poly;
   CPnt *pnt;
   Mat2x2   m;

   RotMat2(&m, rotation);

   polylist = ApertureToPoly_Base(block, 0.0, 0.0, rotation, 0);  // uses block->rot

   if (!polylist)
      return;

   // loop thru polys
   polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = polylist->GetNext(polyPos);
   
      Point2   p;
                  
      fprintf(wfp,"%s(filledFigure\n", ident);
      plusident();
      fprintf(wfp,"%s(shape\n", ident);
      plusident();

      double   oldda = 0;
      pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos != NULL)
      {
         pnt = poly->getPntList().GetNext(pntPos);
         //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
         p.x = pnt->x * scale;
         if (mirror) p.x = -p.x;
         p.y = pnt->y * scale;
         p.bulge = pnt->bulge;
         TransPoint2(&p, 1, &m, insert_x, insert_y);
         // arc in cadif is on the end pt, in camcad in the start point
         double da = atan(p.bulge) * 4;
         if (fabs(oldda) > SMALLANGLE)
         {
            fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
         }
         else
         {
            fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
         }
         oldda = da;
      }
      close_b(wfp);  // path
      close_b(wfp);  // 
   }
   FreePolyList(polylist);//this function in Polylib.cpp

   return;
}

/******************************************************************************
* write_complex_aperture
*/
static void write_complex_aperture (FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel)
{
   Point2   point2;
   DataStruct *np;
   Mat2x2   m;
   int      cnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               // do not write the boundary
               if (poly->isFloodBoundary()) continue;

               double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;

               Point2   p;
                  
               fprintf(wfp,"%s(pathFigure\n", ident);
               plusident();
               if (closed)
                  fprintf(wfp,"%s(shape\n", ident);
               else
                  fprintf(wfp,"%s(openshape\n", ident);
               plusident();

               double   oldda = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
                  p.x = pnt->x * scale;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // arc in cadif is on the end pt, in camcad in the start point
                  double da = atan(p.bulge) * 4;
                     if (fabs(oldda) > SMALLANGLE)
                  {
                     fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
                  }
                  else
                  {
                     fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
                  }
                  oldda = da;
               }
               close_b(wfp);  // path
               fprintf(wfp,"%s(width %ld)\n", ident, cnv_unit(lineWidth));
               close_b(wfp);  // 
            }
         } // if color
         break;  // POLYSTRUCT

         case T_TEXT:
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
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {

               if (block->getShape() == T_COMPLEX)
               {
                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  int block_layer = -1;

                  write_complex_aperture(wfp,&(subblock->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1);
               }
               else
               {
                  // a complex aperture pointers to a normal aperture.
                  write_simple_aperture(wfp,block, 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1);
               }
            }
            else // not aperture
            {
               write_complex_aperture(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1);
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end G */
   return;
}

/******************************************************************************
* write_cadifpadsymbol
* (padShape PS2705
*     (padRound 139700
*      (filledFigure
*       (circle 139700 (pt 0 0))
*       (fillType SOLID))))
*/
static int write_cadifpadsymbol(FILE *fo)
{
   for (int i=0; i<padshapecnt; i++)
   {
      BlockStruct *block = doc->Find_Block_by_Num(padshapearray[i]->bnum);
      if (block == NULL)   
         continue;

      // aperture is also used in PADSHAPE
      if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         if (block->getShape() != T_COMPLEX)
            continue;

         BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
         CString  userdefname = subblock->getName();
         int psnr = get_padsym(subblock->getBlockNumber(), padshapearray[i]->rot);
         fprintf(fo,"%s(padSymbol PSYM%d (symName \"%s\")\n", ident, psnr, userdefname);
         plusident();
         write_complex_aperture(fo, &(subblock->getDataList()), 0.0, 0.0, DegToRad(padshapearray[i]->rot), 0, 1.0, 0 );
         close_b(fo);
      }  
   }

   return 1;
}

/******************************************************************************
* write_padshape
*/
static int write_padshape(FILE *fo, BlockStruct *block, int rotation, int degreeAngle)
{
   //TSR 3576: the parameter angle is added in order to keep track of the insert angle.

   // aperture is also used in PADSHAPE
   if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
   {
      long sizeA = cnv_unit(block->getSizeA());
      long sizeB = cnv_unit(block->getSizeB()); 
      long xoffset= cnv_unit(block->getXoffset());
      long yoffset= cnv_unit(block->getYoffset());

      int psnr = get_padshape(block->getBlockNumber(), rotation, degreeAngle);
      fprintf(fo, "%s(padShape PS%d\n", ident, psnr);
      plusident();


      switch (block->getShape())
      {
      case T_ROUND:
         fprintf(fo, "%s(padRound %ld\n", ident, sizeA);
         plusident();
         fprintf(fo, "%s(filledFigure\n", ident);
         plusident();
         fprintf(fo, "%s(circle %ld (pt %ld %ld))\n", ident, sizeA, xoffset, yoffset);
         close_b(fo);
         close_b(fo);
         break;
      case T_SQUARE:
         fprintf(fo, "%s(padSquare %ld\n", ident, sizeA);
         plusident();
         fprintf(fo, "%s(filledFigure\n", ident);
         plusident();
         fprintf(fo, "%s(rectangle (pt %ld %ld) (pt %ld %ld))\n", ident,
               -sizeA/2 - xoffset, -sizeA/2 - yoffset,
               sizeA/2 - xoffset, sizeA/2 - yoffset);
         close_b(fo);
         close_b(fo);
         break;
      case T_RECTANGLE:
         {
            long left, right, width;
            int rot;
            if (sizeA > sizeB)
            {
               rot = cnv_rot(RadToDeg(block->getRotation()) + rotation);
               left = sizeA/2 + xoffset;
               right = sizeA/2 - xoffset;
               width = sizeB;
            }
            else
            {
               rot = cnv_rot(RadToDeg(block->getRotation()) + 90 + rotation);
               left = sizeB/2 + yoffset;
               right = sizeB/2 - yoffset;
               width = sizeA;
            }
            fprintf(fo, "%s(padOblong %ld %ld %ld %d\n", ident, width, left, right,  rot);
            plusident();
            fprintf(fo, "%s(filledFigure\n", ident);
            plusident();
            fprintf(fo, "%s(rectangle (pt %ld %ld) (pt %ld %ld))\n", ident,
                  -(sizeA/2 + xoffset) , -(sizeB/2 + yoffset), sizeA/2 + xoffset, sizeB/2 + yoffset);
            close_b(fo);
            close_b(fo);
            /*TSR 3576: if there is an angle, we want to create another instance of the same padshape in order 
            to be referenced by the padstack with this angle. Example: we create a PS0_90 if PS0 has an angle 90.*/
            if(degreeAngle)
            {
               if (sizeA > sizeB)
               {
                  rot = cnv_rot(RadToDeg(block->getRotation()) + rotation + degreeAngle);
                  left = sizeA/2 + xoffset;
                  right = sizeA/2 - xoffset;
                  width = sizeB;
               }
               else
               {
                  rot = cnv_rot(RadToDeg(block->getRotation()) + 90 + rotation + degreeAngle);
                  left = sizeB/2 + yoffset;
                  right = sizeB/2 - yoffset;
                  width = sizeA;
               }
               close_b(fo);
               fprintf(fo, "%s(padShape PS%d_%d\n", ident, psnr, degreeAngle);//for example: PadShape PS0_90
               plusident();
               fprintf(fo, "%s(padOblong %ld %ld %ld %d\n", ident, width, left, right,  rot);
               plusident();
               fprintf(fo, "%s(filledFigure\n", ident);
               plusident();
               fprintf(fo, "%s(rectangle (pt %ld %ld) (pt %ld %ld))\n", ident,
                     -(sizeA/2 + xoffset) , -(sizeB/2 + yoffset), sizeA/2 + xoffset, sizeB/2 + yoffset);
               close_b(fo);
               close_b(fo);
            }
         
         }
         break;
      case T_DONUT:
         fprintf(fo, "%s(padAnnulr %ld %ld\n", ident, sizeA, sizeB);
         plusident();
         fprintf(fo, "%s(filledFigure\n", ident);
         plusident();
         fprintf(fo, "%s(circle %ld (pt %ld %ld))\n", ident, sizeA, xoffset, yoffset);
         close_b(fo);
         close_b(fo);
         break;
      case T_OCTAGON:
         fprintf(fo, "%s(padRound %ld\n", ident, sizeA);
         plusident();
         fprintf(fo, "%s(filledFigure\n", ident);
         plusident();
         fprintf(fo, "%s(circle %ld (pt %ld %ld))\n", ident, sizeA, xoffset, yoffset);
         close_b(fo);
         close_b(fo);
         break;
      case T_OBLONG:
         {
            long left, right, width;
            int rot;
            if (sizeA > sizeB)
            {
               /*TSR 3608: padFinger rotation used to be written in radians!*/
               rot = cnv_rot(RadToDeg(block->getRotation())+rotation);
               sizeA = sizeA - sizeB;  // do not calc  the width
               left = sizeA/2 + xoffset;
               right = sizeA/2 + xoffset;
               width = sizeB;
            }  
            else
            {
               rot = cnv_rot(RadToDeg(block->getRotation()) + 90 + rotation);
               sizeB = sizeB - sizeA;  // do not calc the width
               left = sizeB/2 + yoffset;
               right = sizeB/2 + yoffset;
               width = sizeA;
            }
            fprintf(fo, "%s(padFinger %ld %ld %ld %d\n", ident, width, left, right,  rot);
            plusident();
            fprintf(fo, "%s(filledFigure\n", ident);
            plusident();
            fprintf(fo, "%s(rectangle (pt %ld %ld) (pt %ld %ld))\n", ident,
                  -(sizeA/2 + xoffset) , -(sizeB/2 + yoffset), sizeA/2 + xoffset, sizeB/2 + yoffset);
            close_b(fo);
            close_b(fo);
         }
         break;
      case T_COMPLEX:
         {
            BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
            CString userdefname = subblock->getName();
            fprintf(fo, "%s(padUsrDef \"%s\" 2540 0\n", ident, userdefname); // size, angle
            plusident();
            fprintf(fo, "%s(padNormal (padback 0) (padSymRef PSYM%d)\n", ident, get_padsym(subblock->getBlockNumber(), 0));
            plusident();               
            write_complex_aperture(fo, &subblock->getDataList(), 0.0, 0.0, DegToRad(rotation), 0, 1.0, 0);
            close_b(fo);
            close_b(fo);
         }
         break;
      default:
         {
            fprintf(fo, "%s(padRound 0\n", ident);
            plusident();
            fprintf(fo, "%s(filledFigure\n", ident);
            plusident();
            fprintf(fo, "%s(circle 0 (pt 0 0))\n", ident);
            close_b(fo);
            close_b(fo);

            fprintf(flog, "Unknown Aperture [%d] for Geometry [%s]\n", block->getShape(), block->getName());
            display_error++;
         }
         break;
      }
      close_b(fo);
   }
      
   return 1;
}

/******************************************************************************
* write_cadifpadshape
*  (padShape PS2705
*     (padRound 139700
*      (filledFigure
*       (circle 139700 (pt 0 0))
*       (fillType SOLID))))
*/
static int write_cadifpadshape(FILE *fo)
{
   for (int i=0; i<padshapecnt; i++)
   {
      BlockStruct *block = doc->Find_Block_by_Num(padshapearray[i]->bnum);

      if (block == NULL)
         continue;
      //TSR 3576: parameter padshapearray[i]->degreeAngle is added. This is the insert angle.
      write_padshape(fo, block, padshapearray[i]->rot, padshapearray[i]->degreeAngle);
   }

   return 1;
}

/******************************************************************************
* legal_padlayertype
*/
static int legal_padlayertype(const char *l)
{
   if (!STRCMPI(l,"ELECTRICAL")) return TRUE;
   if (!STRCMPI(l,"BOTTOM_RESIST")) return TRUE;
   if (!STRCMPI(l,"TOP_RESIST")) return TRUE;

   fprintf(flog,"PadLayer Type [%s] not allowed\n", l);
   display_error++;

   return FALSE;
}

/******************************************************************************
* Get_LayerRepPtr
*/
static int Get_LayerRepPtr(const char *l)
{

   for (int i=0;i<layerrepcnt;i++)
   {
      CString  ll;
      ll.Format("L%d", layerreparray[i]->layerindex);
      if (ll.CompareNoCase(l) == 0)
         return i;
   }

   return -1;
}

/******************************************************************************
* GetPADShapesData
*/
static int GetPADShapesData(CDataList *DataList, int degreeRotation)
{
   int padattcnt = 0;
   int pl = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case INSERTTYPE_RELIEFPAD:
      case INSERTTYPE_CLEARPAD:
      case INSERTTYPE_OBSTACLE:
      case INSERTTYPE_DRILLSYMBOL:
         continue;
      }

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      {
         if (block->getShape() == T_COMPLEX)
            block = doc->Find_Block_by_Num((int)block->getSizeA());
         /*TSR 3576: The "data->getInsert()->getAngle()" parameter is added in order to 
         add this  angle to the padshapearray.*/
         get_padshape(block->getBlockNumber(), degreeRotation, (int)RadToDeg(data->getInsert()->getAngle())); // TODO: is this right?
      }
   }
   
   return 1;
}

/******************************************************************************
* CADIF_GetPADSTACKData
*
* return 1 = top
*        2 = bottom
*        3 = all
*        4 = inner   
*/
static int CADIF_GetPADSTACKData(const char *padstkname, CDataList *DataList, 
                             double rotation, int mirror, double scale, int embeddedLevel, 
                             int insertLayer)
{
   Mat2x2 m;
   DataStruct *np;
   int   padattcnt = 0;
   int   pl = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD) 
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE) 
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLSYMBOL) 
               break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            if (mirror)
               block_rot = PI2 - rotation;   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (block->getFlags() & BL_TOOL) break;

            CString padname;
            padname = strlen(block->getOriginalName()) ? block->getOriginalName() : block->getName();
            int     bnum = block->getBlockNumber();

            int r = round(RadToDeg(block_rot));
            while (r < 0)     r += 360;
            while (r >= 360)  r -= 360;

            if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
            {
               // block_layer is not set.
               if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               {
                  if (block->getShape() == T_COMPLEX)
                  {
                     block = doc->Find_Block_by_Num((int)block->getSizeA());
                  }
                  block_layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
               }
            }
            bnum = block->getBlockNumber();

            // this can also return elecLayers
            CString  layername = Layer_CADIF(block_layer);
            if (strlen(layername) == 0)   break;

            LayerStruct *l = doc->FindLayer(block_layer);
            //int lptr = Get_LayerRepPtr(layername);
            //if (lptr < 0)   break;

            // appearantly every padstack layer is allowed.
/*

    (padCode PC43 (name "PASSER")
     (padCodeDesc
      (drill (name "0mm3")
       (drillSize 30480)
       (drillLetter "xx")
       (drillSymbol (name "0mm3"))
       (plated))
      (padAssign PS17 (layerRef L150))
      (padStack TOP
       (pad PS17 L150))
      (padStack BOTTOM
       (pad PS17 L150))))

*/
            //if (!legal_padlayertype(layerreparray[lptr]->layertype))  break;

/*
            if (l && toppad)
            {
               // if only top padstacks are defined, skip bottom only stuff
               if ((l->getMirrorOnly()) || (l->attr & LY_BOTTOMONLY))
                  break;               
            }
*/

            // here check that the same layer is not already in list !
            int found = FALSE;
            for (int i =0;i<padlayercnt;i++)
            {
               if (padlayerarray[i]->cadiflayer.Compare(layername) == 0)
                  found = TRUE;
            }

            if (found)
            {
               fprintf(flog,"Padstack [%s] has already a Shape on Layer [%s] defined!\n", padstkname, layername);
               display_error++;
            }
            else
            {
               CADIFPadLayerStruct *p = new CADIFPadLayerStruct;
               p->name = padname;      
               p->layertype = l->getLayerType();
               p->electricalstacknumber = l->getElectricalStackNumber();
               p->cclayer = block_layer;
               p->cadiflayer = layername;
               p->rotation = block_rot;
               p->block_num = bnum; // do not use block->num, because this is different on COMPLEX Shapes.
               padlayerarray.SetAtGrow(padlayercnt, p);
               padlayercnt++;
            }

            if (l)
            {
               if (l->getLayerType() == LAYTYPE_DRILL)
               {
                  // do not translate a drill layer or drill drawing layer
                  break;
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_TOP || l->getLayerType() == LAYTYPE_SIGNAL_TOP)
               {
                  pl |= 1;
               }
               else
               if ( l->getLayerType() == LAYTYPE_PAD_BOTTOM || l->getLayerType() == LAYTYPE_SIGNAL_BOT)
               {
                  pl |= 2;
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_ALL || l->getLayerType() == LAYTYPE_SIGNAL_ALL)
               {
                  pl |= 3;
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_OUTER)
               {
                  pl |= 3;
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_INNER || l->getLayerType() == LAYTYPE_SIGNAL_INNER)
               {
                  pl |= 4;
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end CADIF_GetPADSTACKData */
   return pl;
}

/******************************************************************************
* get_drillletter
*/
static CString get_drillletter(double size)
{
   CString  letter;

   //fprintf(flog,"Get Drill letter not implemented\n");
   //display_error++;

   letter = "A";

   return letter;
}

/******************************************************************************
* get_drilllayer
*/
static int get_drilllayer()
{
   return layerreparray[drill_dwg]->layerindex;
}

/******************************************************************************
* padlayer_explode
* Explode an elec layer in to all electrical not especifically mentioned.
*/
static int padlayer_explode(FILE *fo, int psnum)
{
   for (int i=0;i<layerrepcnt;i++)
   {
      // excplode eleclayers.
      // loop through all electrical layers
      // check if one was already explicitly in the list 
      // if not - explode to it
      if (!is_layertype_electrical(layerreparray[i]->layertype))  continue;

      CADIFLayerrepStruct *lyrRep = layerreparray[i];
      CString  l1;
      l1.Format("L%d", lyrRep->layerindex);  // 
      int found = FALSE;
      for (int j=0;j<padlayercnt;j++)
      {
         CString l2 = Layer_CADIF(padlayerarray[j]->cclayer);
         if (l1.Compare(l2) == 0)   found = TRUE;     
      }
      if (!found)
         fprintf(fo,"%s(pad PS%d %s)\n", ident, psnum, l1);
   }

   return 1;
}

/******************************************************************************
* write_padcode
*/
static int write_padcode(FILE *fo, BlockStruct *block, int drilllayer, int codenum,
                         const char *padname, int rotation)
{
   fprintf(fo,"%s(padCode PC%d (name \"%s\")\n", ident, codenum, cadif_name(check_name('p', padname)));
   plusident();
   fprintf(fo,"%s(padCodeDesc\n", ident);
   plusident();

   BlockStruct *b = GetDrillInGeometry(doc, block);
   if (b)
   {
      CString  name = b->getName();
      if (strlen(name) == 0)
         name.Format("%ld", cnv_unit(b->getToolSize()));
      fprintf(fo,"%s(drill (name \"%s\")\n", ident, cadif_name(name));
      plusident();
      fprintf(fo,"%s(drillSize %ld)\n", ident, cnv_unit(b->getToolSize()));
      fprintf(fo,"%s(drillLetter \"%s\")\n", ident, get_drillletter(b->getToolSize()));

      fprintf(fo,"%s(drillSymbol (name \"X%d\")\n", ident, b->getBlockNumber());
      plusident();
      fprintf(fo,"%s(symFig SD1 (symLayer GL%d)\n", ident, drilllayer);
      plusident();
      fprintf(fo,"%s(pathFigure\n", ident);
      plusident();
      fprintf(fo,"%s(rectangle (pt -76200 -76200) (pt 76200 76200))\n",ident);
      fprintf(fo,"%s(width 30480)\n", ident);
      close_b(fo);
      close_b(fo);
      close_b(fo);

      fprintf(fo,"%s(plated)\n", ident);
      close_b(fo);
   }

   int padlayer = CADIF_GetPADSTACKData(block->getName(), &block->getDataList(), DegToRad(rotation), 0, 1.0, 0, -1);

   CADIFPadStackStruct *pp = new CADIFPadStackStruct;
   padstackarray.SetAtGrow(padstackcnt, pp);
   padstackcnt++;

   pp->padstackname = block->getName();     
   pp->pcnum = codenum;
   pp->typ = padlayer;
   if (padlayer == 1)
   {
      UINT64 baseVal = 1L;
      pp->layermap =  0x1;
      pp->mirrormap = baseVal << (max_signalcnt-1);
   }
   else
   if (padlayer == 2)
   {
      UINT64 baseVal = 1L;
      pp->layermap = baseVal << (max_signalcnt-1);
      pp->mirrormap =  0x1;
   }
   else
   {
      pp->layermap = ALL_INT64;
      pp->mirrormap = ALL_INT64;
   }

   // here write padstack
/*
      (drill (name "71120mm")
       (drillSize 71120) (drillLetter "d")
       (drillSymbol (name "71120mm"))
       (plated))
*/

   int psptr;
   int angle;
	int p=0;
   for (p=0;p<padlayercnt;p++)
   {
      CString  l;
      l = Layer_CADIF(padlayerarray[p]->cclayer);
      l.TrimLeft();
      l.TrimRight();
      if (strlen(l) == 0)
         continue;
      /*TSR 3576: We get the angle from the padshapearray*/
      psptr = FindPadShape(padlayerarray[p]->block_num, rotation, &angle);
      //If FindPadShape returns -1, then it can't find the pad shape

      if (psptr > -1)
      {
         if(!angle)//if there isn't an insert angle, reference the original padshape.
         {
            if (!l.CompareNoCase("elecLayers"))
               fprintf(fo,"%s(padAssign PS%d (%s))\n", ident, psptr, l);
            else
               fprintf(fo,"%s(padAssign PS%d (layerRef %s))\n", ident, psptr, l);
         }
         /*TSR 3576:If we have an angle, we should reference the instance of the padshape 
         which we created, not the original one.  ex: PS0_90 not PS0 */
         if(angle)
         {
            if (!l.CompareNoCase("elecLayers"))
               fprintf(fo,"%s(padAssign PS%d_%d (%s))\n", ident, psptr, angle, l);
            else
               fprintf(fo,"%s(padAssign PS%d_%d (layerRef %s))\n", ident, psptr, angle, l);        
         }
      }
      else
            fprintf(flog,"Unknown insert of [%s] was dropped from padstack\n", padlayerarray[p]->name);
   }

   fprintf(fo,"%s(padStack TOP\n", ident);
   plusident();
   for (p=0;p<padlayercnt;p++)
   {
      CString  lyr;
      lyr = Layer_CADIF(padlayerarray[p]->cclayer);
      if (strlen(lyr) == 0)   
         continue;

      psptr = FindPadShape(padlayerarray[p]->block_num, rotation, &angle);
      if (psptr > -1)
      {
         if(!angle)
         {
            if (!lyr.CompareNoCase("elecLayers"))  
               padlayer_explode(fo, psptr);
            else
               fprintf(fo,"%s(pad PS%d %s)\n", ident, psptr, lyr);
         }
         /*TSR 3576:If we have an angle, we should reference the instance of the padshape 
         which we created, not the original one.  ex: PS0_90 not PS0 */
         if(angle)
         {
            if (!lyr.CompareNoCase("elecLayers"))  
               padlayer_explode(fo, psptr);
            else
               fprintf(fo,"%s(pad PS%d_%d %s)\n", ident, psptr, angle, lyr);        
         }
      }
   }
   close_b(fo);

   fprintf(fo,"%s(padStack BOTTOM\n", ident);
   plusident();
   for (p=0;p<padlayercnt;p++)
   {
      CString  lyr;
      lyr = Layer_CADIF(padlayerarray[p]->cclayer);
      if (strlen(lyr) == 0)   continue;
      psptr = FindPadShape(padlayerarray[p]->block_num, rotation, &angle);
      if (psptr > -1)
      {
         if(!angle)
         {
            if (!lyr.CompareNoCase("elecLayers"))
               padlayer_explode(fo, psptr);
            else
               fprintf(fo,"%s(pad PS%d %s)\n", ident, psptr, lyr);
         }
         /*TSR 3576:If we have an angle, we should reference the instance of the padshape 
         which we created, not the original one.  ex: PS0_90 not PS0 */
         if(angle)
         {
            if (!lyr.CompareNoCase("elecLayers"))
               padlayer_explode(fo, psptr);
            else
               fprintf(fo,"%s(pad PS%d_%d %s)\n", ident, psptr, angle, lyr);
         }
      }
   }
   close_b(fo);

   close_b(fo);
   close_b(fo);

   for (p=0;p<padlayercnt;p++)
   {
      delete padlayerarray[p];
   }
   padlayercnt = 0;

   return 1;
}

/******************************************************************************
* write_cadifpadcode
*/
static int write_cadifpadcode(FILE *fo)
{
   int drilllayer = get_drilllayer();
   int codenum = 0;
	int i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;

      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {
         codenum++;
         write_padcode(fo, block, drilllayer, codenum, block->getName(), 0);
      }
   }

   // here do the rotated padstacks
   for (i=0;i<padcoderotcnt;i++)
   {
      CADIFPadCodeRotStruct *padCode = padcoderotarray[i];
      BlockStruct *block = doc->Find_Block_by_Num(padcoderotarray[i]->bnum);
      codenum++;
      write_padcode(fo, block, drilllayer, codenum, padcoderotarray[i]->newpadname,
         padcoderotarray[i]->rotation);
      padcoderotarray[i]->pcnum = codenum;
   }

   return 1;
}

/******************************************************************************
* write_cadifpadcodelib
*/
static int write_cadifpadcodelib(FILE *fo)
{
   // collect padstacks which are not used in component pins
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;

      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
         GetPADShapesData(&block->getDataList(), 0);
   }

   fprintf(fo, "%s(padCodeLib\n", ident);
   plusident();
   write_cadifpadsymbol(fo);  // complex apertures
   write_cadifpadshape(fo);   // simple apertures
   write_cadifpadcode(fo);

   close_b(fo);
   return 1;
}

/******************************************************************************
* get_layerrepptr
*/
static int get_layerrepptr(const char *l)
{
   for (int i=0;i<layerrepcnt;i++)
   {
      if (layerreparray[i]->layername.CompareNoCase(l) == 0)
         return i;
   }

   return -1;
}

/******************************************************************************
* write_cadiftechdesc
*/
static int write_cadiftechdesc(FILE *fo)
{
   int   i;

   fprintf(fo,"%s(techDesc\n", ident);
   plusident();

   fprintf(fo,"%s(boardTech \"%s\")\n", ident, TECHNOLOGY_NAME);
   fprintf(fo,"%s(packTech \"%s\")\n", ident, TECHNOLOGY_NAME);
   fprintf(fo,"%s(padTech \"%s\")\n", ident, TECHNOLOGY_NAME);

   fprintf(fo,"%s(techRules\n", ident);
   plusident();
   fprintf(fo,"%s(memoryRoute MEM_NORM_AXIAL)\n", ident);
   fprintf(fo,"%s(powerRoute PWR_PARALLEL)\n", ident);
   fprintf(fo,"%s(largeComp 0 0)\n", ident);
   fprintf(fo,"%s(ppPathWidth 50800)\n", ident);
   fprintf(fo,"%s(ppClearance 30480)\n", ident);
   fprintf(fo,"%s(mitering 0 0)\n", ident);
   fprintf(fo,"%s(compRotn CR_INDEPENDENT 0)\n", ident);
   fprintf(fo,"%s(compSwap CS_UNRESTRICTED)\n", ident);
   fprintf(fo,"%s(minTrackWidth 2540)\n", ident);
   fprintf(fo,"%s(acidTrap 0)\n", ident);
   fprintf(fo,"%s(drillSep 0)\n", ident);
   fprintf(fo,"%s(innerPadSupp PADSUP_NONE)\n", ident);
   fprintf(fo,"%s(padOversize BY_PAD)\n", ident);
   fprintf(fo,"%s(minNeckLength 127000)\n", ident);
   fprintf(fo,"%s(minUnneckLength 127000)\n", ident);
   fprintf(fo,"%s(wirebondLims 0 0)\n", ident);
   fprintf(fo,"%s(drillTol 0)\n", ident);
   fprintf(fo,"%s(tieWidth 0)\n", ident);
   fprintf(fo,"%s(maxTearWidth 0)\n", ident);
   fprintf(fo,"%s(drillDraw\n", ident);
   plusident();
   fprintf(fo,"%s(fontRef FNT%d)\n", ident, FONT_INDEX);
   fprintf(fo,"%s(height 127000)\n", ident);
   fprintf(fo,"%s(polarCoord 63500 0)\n", ident);
   fprintf(fo,"%s(drillPad PAD_SQUARE 63500 0)\n", ident);
   close_b(fo);   
   fprintf(fo,"%s(viaHoles)\n", ident);
   close_b(fo);

   fprintf(fo,"%s(swapLayerList\n", ident);
   plusident();
   for (i=0;i<layerrepcnt;i++)
   {
      int   l1, l2;
      CADIFLayerrepStruct *lyrRep = layerreparray[i];
      l1 = get_layerrepptr(lyrRep->layername);
      l2 = get_layerrepptr(lyrRep->swaplayer);
      if (l1 > -1 && l2 > -1)
      {
         if(l1 != l2)   // true mirror
         {
            fprintf(fo,"%s(swapLayer L%d L%d)\n", ident, 
               layerreparray[l1]->layerindex, layerreparray[l2]->layerindex);
         }
      }
   }
   close_b(fo);

   fprintf(fo,"%s(drillLayerList\n", ident);
   plusident();
   fprintf(fo,"%s(drillLayer\n", ident);
   plusident();
   fprintf(fo,"%s(layerRange L%d L%d)\n", ident, top, bot);
   close_b(fo);
   close_b(fo);

   fprintf(fo,"%s(topSurfLayer L%d)\n", ident, top);
   fprintf(fo,"%s(botSurfLayer L%d)\n", ident, bot);
   fprintf(fo,"%s(topElecLayer L%d)\n", ident, top);
   fprintf(fo,"%s(botElecLayer L%d)\n", ident, bot);

   fprintf(fo,"%s(viasUnderCmp)\n", ident);
   fprintf(fo,"%s(tracksUnderCmp)\n", ident);
   fprintf(fo,"%s(systemGrid 1)\n", ident);
   fprintf(fo,"%s(trackGrid 12700)\n", ident);
   fprintf(fo,"%s(viaGrid 12700)\n", ident);
   fprintf(fo,"%s(defAlternate \"\" \"\")\n", ident);

   fprintf(fo,"%s(wildcardNet (name \"*\") (id 32767)\n", ident);
   plusident();

   if (padstackcnt)
   {
      int   wildcardvia;

      wildcardvia = -1;
      for (i=0;i<padstackcnt;i++)
      {
         if (padstackarray[i]->used_as_via)
         {
            wildcardvia = padstackarray[i]->pcnum;
            break;
         }
      }  

      if (wildcardvia == -1)
         wildcardvia = padstackarray[0]->pcnum;

      fprintf(fo,"%s(viaPadCode PC%d)\n", ident, wildcardvia);
      fprintf(fo,"%s(wcnWidth 15240 15240\n", ident);
      plusident();
      close_b(fo);
   }
   close_b(fo); // wild card net

   fprintf(fo,"%s(plcCat (name \"default\")\n", ident);
   plusident();
   fprintf(fo,"%s(startRotn 0)\n", ident);
   fprintf(fo,"%s(endRotn 359)\n", ident);
   fprintf(fo,"%s(stepRotn 1)\n", ident);
   fprintf(fo,"%s(defRotn 0)\n", ident);
   fprintf(fo,"%s(defSide TOP)\n", ident);
   fprintf(fo,"%s(placeGrid 1 1)\n", ident);
   fprintf(fo,"%s(swappable)\n", ident);
   close_b(fo);


   fprintf(fo,"%s(treeSpacing (allLayers)\n", ident);
   plusident();
   fprintf(fo,"%s(padToPad 20320)\n", ident);
   fprintf(fo,"%s(padToVia 20320)\n", ident);
   fprintf(fo,"%s(padToTrack 20320)\n", ident);
   fprintf(fo,"%s(padToProf 20320)\n", ident);
   fprintf(fo,"%s(viaToVia 20320)\n", ident);
   fprintf(fo,"%s(viaToTrack 20320)\n", ident);
   fprintf(fo,"%s(viaToProf 20320)\n", ident);
   fprintf(fo,"%s(trackToTrack 20320)\n", ident);
   fprintf(fo,"%s(trackToProf 20320)\n", ident);
   close_b(fo);

   close_b(fo);
   return 1;
}

/******************************************************************************
* get_padstack_index
*/
static int  get_padstack_index(const char *p)
{

   for (int i=0;i<padstackcnt;i++)
   {
      CADIFPadStackStruct *pasStack = padstackarray[i];

      if (pasStack->padstackname.Compare(p) == 0)
         return i;
   }

   return -1;
}

/******************************************************************************
* mark_padstacks_used_as_vias
*/
static int mark_padstacks_used_as_vias(CDataList *DataList) 
{
   DataStruct  *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_INSERT)
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            int p = get_padstack_index(block->getName());
            if (p < 0)
            {
               fprintf(flog,"Via found [%s] which was not defined as a padstack\n", block->getName());
               display_error++;
            }
            else
            {
               padstackarray[p]->used_as_via = TRUE;
            }
         }
      }
   } 

   return 1;
}

/******************************************************************************
* write_cadiftechnology
*/
static int write_cadiftechnology(FILE *fo, FileStruct *file)
{
  fprintf(fo,"%s(technology (name \"%s\")\n", ident, cadif_name(TECHNOLOGY_NAME));
  plusident();
  fprintf(fo,"%s(materialLib)\n", ident);

  write_cadiflayerstack(fo);

  write_cadifpadcodelib(fo);
  mark_padstacks_used_as_vias(&(file->getBlock()->getDataList()));

  write_cadiftechdesc(fo);

   close_b(fo);
   return 1;
}

/******************************************************************************
* write_cadifattriblib
*/
static int write_cadifattriblib(FILE *fo)
{
   fprintf(fo,"%s(attrLib\n", ident);
   plusident();

   fprintf(fo,"%s(attrDef (name \"silkscrn_name\")\n", ident);
   plusident();
   fprintf(fo,"%s(binding BIND_COMP) (visible)\n", ident);
   close_b(fo);

   fprintf(fo,"%s(attrDef (name \"assembly_name\")\n", ident);
   plusident();
   fprintf(fo,"%s(binding BIND_COMP) (visible)\n", ident);
   close_b(fo);

   close_b(fo);

   return 1;
}

/******************************************************************************
* write_cadifcolorlist
*/
static int write_cadifcolorlist(FILE *fo)
{
   fprintf(fo,"%s(colorList\n", ident);
   plusident();
   close_b(fo);

   return 1;
}

/******************************************************************************
* write_cadifcolorsettings
*/
static int write_cadifcolorsettings(FILE *fo)
{
   fprintf(fo,"%s(colorSettings\n", ident);
   plusident();
   close_b(fo);

   return 1;
}

/******************************************************************************
* write_cadiflayersettings
*/
static int write_cadiflayersettings(FILE *fo)
{
   fprintf(fo,"%s(layerSettings\n", ident);
   plusident();

   fprintf(fo,"%s(currentLayer L%d)\n", ident, top);
   fprintf(fo,"%s(firstWorkLayer L%d)\n", ident, top);
   fprintf(fo,"%s(secondWorkLayer L%d)\n", ident, bot);

   close_b(fo);

   return 1;
}

/******************************************************************************
* write_cadifnet
*/
static int write_cadifnet(FILE *wfp, FileStruct *file)
{
   int   ncnt = 0;

   totalviacnt = 0;

   NetStruct *net;
   POSITION  netPos;
   
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      ++ncnt;
      fprintf(wfp,"%s(net N%d (signalRef S%d)\n", ident, ncnt, ncnt);
      plusident();

#ifdef DUMP
      fprintf(flog, "NET=%s\n", net->getNetName());
#endif   
      viacnt = 0;
      endpointcnt = 0;

      // reset status 
      CString  f;
      f.Format("Processing %s",net->getNetName());
      progress->SetStatus(f);

      fprintf(wfp,"%s(netJoins\n", ident);
      plusident();
      do_signalpinloc(wfp, net, file->getScale());
      close_b(wfp);

      // margin is 2 units
      SetMarginTrace(2);   // set rounding margin - do not change it to more or less !!!

      PCB_WriteROUTESData(wfp,  &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale(), 0, -1, net->getNetName());
      CResultList *resultList = FindShortestNetList();

#ifdef DUMP
      DumpResult(flog, resultList);
#endif
      Write_Traces(wfp, resultList,net->getNetName());

      ClearTrace();

		int i=0;
      for (i=0;i<viacnt;i++)
      {
         delete viaarray[i];
      }
      viacnt = 0;

      for (i=0;i<junctioncnt;i++)
      {
         delete junctionarray[i];
      }
      junctioncnt = 0;

      for (i=0;i<endpointcnt;i++)
      {
         delete endpointarray[i];
      }
      endpointcnt = 0;
      close_b(wfp);
   }

   return 1;

}

/******************************************************************************
* write_cadifarea
*/
static int write_cadifarea(FILE *wfp, FileStruct *file)
{
   CADIF_WriteCopperData(wfp,  &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale(), 0, -1);
   
   return 1;

}

/******************************************************************************
* write_cadifannotate
*/
static int write_cadifannotate(FILE *fo, FileStruct *file)
{
   int   cnt = 0;
   CADIF_WriteAnnotate(fo, &(file->getBlock()->getDataList()), 
         file->getInsertX() , file->getInsertY(),
         file->getRotation(), file->isMirrored(), file->getScale(), -1, &cnt);

   return 1;
}

/******************************************************************************
* write_cadifcomponent
*/
static int write_cadifcomponent(FILE *fo, FileStruct *file)
{
   CADIF_WriteComponents(fo, &(file->getBlock()->getDataList()), 
         file->getInsertX() , file->getInsertY(),
         file->getRotation(), file->isMirrored(), file->getScale(), 0, -1);

   return 1;
}

/******************************************************************************
* write_cadifsymbol
*/
static int write_cadifsymbol(FILE *fo, FileStruct *file)
{
   // always needed -according to Kevin
   fprintf(fo,"%s(symbol SYM1\n", ident);
   plusident();
   fprintf(fo,"%s(symName \"via\")\n", ident);
   fprintf(fo,"%s(alternate \"1\")\n", ident);
   fprintf(fo,"%s(symDesc\n", ident);
   plusident();
   fprintf(fo,"%s(stdSymbol)\n", ident);
   close_b(fo);
   close_b(fo);

/*
  (symbol SSYM1 (symName "cpoint") (alternate "1")
   (symDesc (stdSymbol) 
    (symFig SD1 (symLayer GL251)
     (pathFigure
      (path
       (pt -254000 -254000)
       (pt 254000 254000))))

  )
*/

   return 1;
}

/******************************************************************************
* CADIF_WritePartTerm
*/
static int CADIF_WritePartTerm(FILE *wfp, CDataList *DataList)
{
   DataStruct *np;
   int         pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               pincnt++;
/*
               fprintf(wfp,"%s(partTerm T%d (name \"%s\") (id %d))\n", ident,
                  pincnt, np->getInsert()->refname, pincnt);
*/
               fprintf(wfp,"%s(partTerm T%d (id %d))\n", ident, pincnt, pincnt);
            }
         }
         break;                                                                
      } // end switch
   } // end CADIF_WritePackTerm 
   return pincnt;
}

/******************************************************************************
* write_cadifpart
*
*  (part PRT1039 (name "22MF-5%-SB")
*   (partDesc
*    (description "")
*    (detailName "22MF-5%-SB")
*    (prefix "C")
*    (packRef PAC1014)
*    (catRef CAT2270)
*    (viaRestrict RESTRICT_NONE)
*    (trackRestrict RESTRICT_NONE)
*    (partTerm T1 (id 1))
*    (partTerm T2 (id 2))
*   )
*  )
*/
static int write_cadifpart(FILE *fo, FileStruct *file)
{
   POSITION typePos = file->getTypeList().GetHeadPosition();
   while (typePos)
   {                                         
      TypeStruct *type = file->getTypeList().GetNext(typePos);

      // if no block is assigned to the type, no need to write it.
      if (type->getBlockNumber() == -1)
         continue;
      
      BlockStruct *block = doc->Find_Block_by_Num(type->getBlockNumber());
      //complist_device_written(block->name, typ->Name);

      typeNameArray.SetAtGrow(typeNameArrayCount++, type->getName());

      fprintf(fo, "%s(part PRT%d (name \"%s\")\n", ident, typeNameArrayCount, cadif_name(check_name('t', type->getName())));
      plusident();
      fprintf(fo, "%s(partDesc\n", ident);
      plusident();

      fprintf(fo, "%s(description \"%s\")\n", ident, cadif_name(check_name('x', type->getName())));
      fprintf(fo, "%s(detailName \"%s\")\n", ident, cadif_name(check_name('x', type->getName())));
      fprintf(fo, "%s(prefix \"\")\n", ident);
      fprintf(fo, "%s(packRef PAC%d)\n", ident, type->getBlockNumber());
      fprintf(fo, "%s(catRef CAT1)\n", ident);

/*
    (catRef CAT2270)
    (viaRestrict RESTRICT_NONE)
    (trackRestrict RESTRICT_NONE)
*/
      CADIF_WritePartTerm(fo, &block->getDataList());

      close_b(fo);
      close_b(fo);
   }


   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (!is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1))
      {
         if (GetTypeIndex(block->getName()) > -1) // if we have already written, then skip
            continue;

         typeNameArray.SetAtGrow(typeNameArrayCount++, block->getName());

         fprintf(fo, "%s(part PRT%d (name \"%s\")\n", ident, typeNameArrayCount, cadif_name(check_name('t', block->getName())));
         plusident();
         fprintf(fo, "%s(partDesc\n", ident);
         plusident();

         fprintf(fo, "%s(description \"%s\")\n", ident, cadif_name(check_name('x', block->getName())));
         fprintf(fo, "%s(detailName \"%s\")\n", ident, cadif_name(check_name('x', block->getName())));
         fprintf(fo, "%s(prefix \"\")\n", ident);
         fprintf(fo, "%s(packRef PAC%d)\n", ident, block->getBlockNumber());
         fprintf(fo, "%s(catRef CAT1)\n", ident);

/*
(catRef CAT2270)
(viaRestrict RESTRICT_NONE)
(trackRestrict RESTRICT_NONE)
*/
         CADIF_WritePartTerm(fo, &(block->getDataList()));

         close_b(fo);
         close_b(fo);
      }
   }

   return 1;
}

/******************************************************************************
* CADIF_WritePackTerm
*/
static int CADIF_WritePackTerm(FILE *wfp, CDataList *DataList)
{
   DataStruct *np;
   int         pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               pincnt++;
               fprintf(wfp,"%s(packTerm T%d (name \"%s\") (id %d))\n", ident,
                  pincnt, cadif_name(np->getInsert()->getRefname()), pincnt);
            }
         }
         break;                                                                
      } // end switch
   } // end CADIF_WritePackTerm 
   return pincnt;
}

/******************************************************************************
* CADIF_WriteCompPin
*/
static int CADIF_WriteCompPin(FILE *wfp, const char *compname, int compindex, CDataList *DataList)
{
   DataStruct *np;
   int         pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               pincnt++;
               fprintf(wfp,"%s(compPin T%d (id %d))\n", ident, pincnt, pincnt);

               CADIFCompPinStruct *c = new CADIFCompPinStruct;
               c->compname = compname;
               c->compindex = compindex;
               c->pinname = np->getInsert()->getRefname();      
               c->pinindex = pincnt;
               comppinarray.SetAtGrow(comppincnt, c);
               comppincnt++;
            }
         }
         break;                                                                
      } // end switch
   } // end CADIF_WriteCompPin
   return pincnt;
}

/******************************************************************************
* CADIF_WritePackAltTerm
*
*    (packAltTerm T1 (id 1)
*     (position (pt -105000 0))
*     (labelPosn (pt -105000 0))
*     (padCodeRef PC37)
*     (exitDirn SOUTH NORTH EAST WEST))
*/
static int CADIF_WritePackAltTerm(FILE *wfp, CDataList *DataList)
{
   DataStruct *np;
   int         pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               Point2 point2;
   
               point2.x = np->getInsert()->getOriginX();
               point2.y = np->getInsert()->getOriginY();
               
               //TransPoint2(&point2, 1, &m, insert_x, insert_y);

               int block_rot = cnv_rot(RadToDeg(np->getInsert()->getAngle()));
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               int p = get_padstack_index(block->getName());
               int pccode = padstackarray[p]->pcnum;

               if (block_rot)
               {
                  CString  degreerot, padname;
                  degreerot.Format("%d", block_rot);
                  padname = block->getName();
                  padname += "_";
                  padname += degreerot;
                  int ptr = get_padcoderot(block->getName(), block_rot);
                  pccode = padcoderotarray[ptr]->pcnum;
               }

               pincnt++;
               fprintf(wfp,"%s(packAltTerm T%d (id %d)\n", ident, pincnt, pincnt);
               plusident();
               fprintf(wfp,"%s(position (pt %ld %ld))\n", ident, cnv_unit(point2.x), cnv_unit(point2.y));
               fprintf(wfp,"%s(labelPosn (pt %ld %ld))\n", ident, cnv_unit(point2.x), cnv_unit(point2.y));
               if (pccode)
               {
                  fprintf(wfp,"%s(padCodeRef PC%d)\n", ident, pccode);
                  fprintf(wfp,"%s(exitDirn)\n ", ident);
               }
               //fprintf(wfp,"%s(exitDirn SOUTH NORTH EAST WEST)\n", ident);
               close_b(wfp);
            }
         }
         break;                                                                
      } // end switch
   } // end CADIF_WritePackAltTerm 
   return pincnt;
}

/******************************************************************************
* write_cadifpackage
*
*  (package PAC982 (name "DO-35_Diode_.1")
*   (packDesc (symName "DO-35_Diode__1")
*    (leadForm THRU)
*    (packTerm T1 (name "1") (id 1))
*    (packTerm T2 (name "2") (id 2))))
*   (packDesc (symName "L8000")
*    (leadForm THRU)
*    (packTerm T1 (name "1") (id 1))
*    (packTerm T2 (name "2") (id 2))))
*/
static int write_cadifpackage(FILE *fo, FileStruct *file)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      // create necessary aperture blocks
      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT || block->getBlockType() == BLOCKTYPE_TESTPOINT)
      {  
         
         fprintf(fo,"%s(package PAC%d (name \"%s\")\n", ident, i, cadif_name(check_name('g', block->getName())));
         plusident();
         fprintf(fo,"%s(packDesc (symName \"%s\")\n", ident, cadif_name(check_name('g', block->getName())));
         plusident();

         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 2);
         if (a)
            fprintf(fo,"%s(leadForm SMD)\n", ident);
         else
            fprintf(fo,"%s(leadForm THRU)\n", ident);

         CADIF_WritePackTerm(fo, &(block->getDataList()));
         close_b(fo);
         close_b(fo);
      }
   }

   return 1;
}

/******************************************************************************
* write_cadifpackalt
*
  (packAlt ALT1017 (name "")
   (packRef PAC1014)
   (packAltDesc
    (box (pt -215900 -342900) (pt 469900 342900))
    (insertData
     (insertHeight 0)
     (insertBox (pt -215900 -342900) (pt 469900 342900)))
    (symData
     (symFig SD1018 (symLayer GL19)padCodeLib
      (pathFigure
       (shape
        (pt -215900 -342900)
        (pt 469900 -342900)
        (pt 469900 342900)
        (pt -215900 342900)
        (pt -215900 -342900))
       (width 0)))
     (symFig SD1020 (symLayer GL20)
      (pathFigure
       (shape
        (pt 444500 0)
        (arc (e 18 1)
         (pt -190500 0))
        (arc (e 18 1)
         (pt 444500 0)))
       (width 20320)))
     (symFig SD1022 (symLayer GL18)
      (pathFigure
       (shape
        (pt 444500 0)
        (arc (e 18 1)
         (pt -190500 0))
        (arc (e 18 1)
         (pt 444500 0)))
       (width 0)))
     (symFig SD1024 (symLayer GL18)
      (pathFigure
       (openShape
        (pt -139700 -241300)
        (pt -139700 -317500))
       (width 20320)))
     (symFig SD1026 (symLayer GL18)
      (pathFigure
       (openShape
        (pt -177800 -279400)
        (pt -101600 -279400))
       (width 20320)))
     (symText SD1028 TS706
      (string "*SP=0")
      (symLayer GL18)
      (position (pt 444500 0)))
     (symText SD1029 TS706
      (string "*H=7.5")
      (symLayer GL18)
      (position (pt 444500 -63500)))
     (symText SD1030 TS706
      (string "*C=1=6")
      (symLayer GL18)
      (position (pt 444500 -127000)))
     (symLabel SD1037 TS243
      (string "silkscrn_name")
      (symLayer GL18)
      (position (pt 127000 0)))
     (symLabel SD1038 TS243
      (string "part_type")
      (symLayer GL18)
      (position (pt 127000 0)))
     (symLabel SD2525 TS243
      (string "assembly_name")
      (symLayer GL251)
      (position (pt 0 0))))
    (symPlace TOP (symDataRef))
    (symPlace BOTTOM (symDataRef))
    (symSilk TOP (symDataRef))
    (symSilk BOTTOM (symDataRef))
    (packAltTerm T1 (id 1)
     (padCodeRef PC103)
     (position (pt 0 0))
     (labelPosn (pt 0 0))
     (exitDirn NORTH SOUTH EAST WEST))
    (packAltTerm T2 (id 2)
     (padCodeRef PC103)
     (position (pt 254000 0))
     (labelPosn (pt 254000 0))
     (exitDirn NORTH SOUTH EAST WEST))))
*/
static int write_cadifpackalt(FILE *fo, FileStruct *file)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      // create necessary aperture blocks
      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT || block->getBlockType() == BLOCKTYPE_TESTPOINT)
      {  
         fprintf(fo,"%s(packAlt ALT%d (name \"%s\")\n", ident, block->getBlockNumber(), cadif_name(check_name('h', block->getName())));
         plusident();
         fprintf(fo,"%s(packRef PAC%d)\n", ident, i);
         fprintf(fo,"%s(packAltDesc\n", ident);
         plusident();

         doc->validateBlockExtents(block);

         double xmin = block->getXmin(), xmax = block->getXmax();
         double ymin = block->getYmin(), ymax = block->getYmax();

         // required.
         fprintf(fo,"%s(box (pt %ld %ld) (pt %ld %ld))\n", ident, 
            cnv_unit(xmin), cnv_unit(ymin), cnv_unit(xmax), cnv_unit(ymax));
         fprintf(fo,"%s(insertData\n",ident);
         plusident();
         fprintf(fo,"%s(insertHeight 0)\n", ident);
         fprintf(fo,"%s(insertbox (pt %ld %ld) (pt %ld %ld))\n", ident, 
            cnv_unit(xmin), cnv_unit(ymin), cnv_unit(xmax), cnv_unit(ymax));
         close_b(fo);

         // graphic and terminal placement
         fprintf(fo,"%s(symData\n", ident);
         plusident();

         CADIF_WriteSymData(fo, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, -1, 0);  

         close_b(fo);

         // packaltterm
         CADIF_WritePackAltTerm(fo, &(block->getDataList()));

         close_b(fo);
         close_b(fo);
      }
   }

   return 1;
}

/******************************************************************************
* write_cadifdesign
*/
static int write_cadifdesign(FILE *fo, FileStruct *file, double xmin, double ymin, double xmax, double ymax)
{
   fprintf(fo,"%s(design\n",ident);
   plusident();
   write_cadifdataset(fo);
   write_cadifpaper(fo, xmin, ymin, xmax, ymax);
   write_cadifsignallist(fo, file);
   write_cadiftextinfo(fo);
   write_cadifzoneinfo(fo);
   write_cadifstyleinfo(fo);
   write_cadiftechnology(fo, file);

   write_cadifattriblib(fo);
   write_cadiflayersettings(fo);

   write_cadifcolorlist(fo);
   write_cadifcolorsettings(fo);

   fprintf(fo,"%s(unit UN1 (name \"mils\")\n", ident);
   plusident();
   fprintf(fo,"%s(unitType IMPERIAL) (unitFactor (e 1 -3)) (unitPlaces 3)\n", ident);
   close_b(fo);
   fprintf(fo,"%s(ipUnits UN1)\n", ident);
   fprintf(fo,"%s(opUnits UN1)\n", ident);


   write_cadifsymbol(fo, file);

   write_cadifpackage(fo, file);

   write_cadifpackalt(fo, file);

   fprintf(fo,"%s(category CAT1 (name \"othe\")(catDesc (listed)))\n", ident);

   write_cadifpart(fo, file);

   write_cadifcomponent(fo, file);

   write_cadifnet(fo, file);

   write_cadifarea(fo, file);

   write_cadifannotate(fo, file);

/*
  (errorList (height 12700)) 
*/

   close_b(fo);
   return 1;
}

/******************************************************************************
* write_cadiffileinfo
*/
static int write_cadiffileinfo(FILE *fo)
{
   fprintf(fo,"%s(fileInfo\n",ident);
   plusident();
   fprintf(fo,"%s(jobFile \"cstr2cdf\")\n", ident);
   fprintf(fo,"%s(jobName \"Demo File for the Batch Router\")\n", ident);
   fprintf(fo,"%s(jobDesc \"\")\n", ident);
   fprintf(fo,"%s(application PCB)\n", ident);
   fprintf(fo,"%s(timeStamp 1999 10 2 8 54 35)\n", ident);
   fprintf(fo,"%s(createTime 938879675)\n", ident);
   fprintf(fo,"%s(lastEditTime 938879675)\n", ident);
   fprintf(fo,"%s(elapsedTime 0)\n", ident);
   fprintf(fo,"%s(cadifTime 938879675)\n", ident);
   close_b(fo);
 return 1;
}

/******************************************************************************
* write_cadifstart
*/
static int write_cadifstart(FILE *fo, FileStruct *file, double xmin, double ymin, double xmax, double ymax)
{
   // this is for PCB files.
   fprintf(fo,"%s(cadif\n",ident);
   plusident();

   write_cadifformat(fo);
   write_cadifdesign(fo, file, xmin, ymin, xmax, ymax);
   write_cadiffileinfo(fo);

   close_b(fo);

   return 1;
}

/******************************************************************************
* check_layer_setup
*/
static int check_layer_setup(const char *layerrepfile)
{
   int      err = 0;
   CString  errstring;

   errstring = "";

   for (int i=0;i<maxArr;i++)
   {
      CADIFLayerStruct *tmp;
      tmp = mlArr[i];
      if (tmp->on == FALSE)   
         continue;


      // here check if layer->getName() is not already done
      if (!check_cadiflayer_to_layrep(tmp->newName))
      {
         CString  tempStr;
         tempStr.Format("Assigned CADIF Layer [%s] does not exist in .LAYERFILE [%s]\n",
            tmp->newName, layerrepfile);
         //ErrorMessage(tmp,"Error in Layer Setup");
         errstring += tempStr;
         err++;
      }
   }

   if (err)
      ErrorMessage(errstring,"Error in Layer Setup");

   return err;
}

/******************************************************************************
* update_textarray
*/
static int update_textarray(double rot, int mirror, double height, double charwidth)
{

   for (int i=0;i<textcnt;i++)
   {
      if (textarray[i]->height == height &&
          textarray[i]->charwidth == charwidth &&
          textarray[i]->mirror == mirror &&
          textarray[i]->rotation == rot)
          return i;

   }

   CADIFTextStruct *t = new CADIFTextStruct;
   textarray.SetAtGrow(textcnt, t);
   textcnt++;

   t->height = height;
   t->charwidth = charwidth;
   t->mirror = mirror;
   t->rotation = rot;

   return textcnt-1;
}

/******************************************************************************
* CADIF_WriteData
*/
void CADIF_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   const char *lname;
   int   layer;

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
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         //wcadif_Graph_Level(Get_Layer_Layername(doc->get_layer_mirror(layer, mirror)));
      }

      switch(np->getDataType())
      {
         case T_POLY:
            if (doc->get_layer_visible(np->getLayerIndex(), mirror))
            {
               CPoly *poly;
               CPnt  *pnt;
               POSITION polyPos, pntPos;

               // loop thru polys
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);
                  BOOL polyFilled = poly->isFilled();
                  BOOL closed     = poly->isClosed();

                  // do not write the boundary
                  if (poly->isFloodBoundary()) continue;

                  Point2   p;
                  
                  int vertexcnt = 0;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     vertexcnt++;
                  }        
                  int tcnt = vertexcnt;
                  Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                  vertexcnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     points[vertexcnt].x = p.x;
                     points[vertexcnt].y = p.y;
                     points[vertexcnt].bulge = p.bulge;
                     vertexcnt++;
                  }
                  //wcadif_Graph_Polyline(points,vertexcnt,polyFilled);
                  free(points);
               }
            } // if color
         break;

         case T_TEXT:
         {
            double text_rot;
   
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            if (mirror)
            {
               text_rot = rotation - np->getText()->getRotation();
               point2.x = -point2.x;
            }
            else
               text_rot = rotation + np->getText()->getRotation();

            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            Mat2x2   m1;
            RotMat2(&m1, text_rot);

            int text_mirror = np->getText()->getResultantMirror(mirror);

            //wcadif_Graph_Text( np->getText()->getText(), point2.x, point2.y,
            //                np->getText()->height*scale,np->getText()->width*scale,
            //                text_rot, text_mirror);

         }
         break;
         
         case T_INSERT:
         {
            Point2 point2;

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
                  if ((lname = Get_Layer_Layername(doc->get_layer_mirror(layer, mirror))) != NULL)
                  {
                    //wcadif_Graph_Level(lname);

                    //wcadif_Graph_Aperture(point2.x,point2.y,get_appnum(block->num));
                  }
               } // end if layer visible
            }
            else // not aperture
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               CADIF_WriteData(&(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1,-1);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

} /* end CADIF_WriteData */

/******************************************************************************
* attributes_GetTextData
*/
static void attributes_GetTextData(CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      if (!attrib->isVisible())
      {
         continue;      // do not show an invisble attribute.
      }

      update_textarray(attrib->getRotationRadians(), 0, attrib->getHeight(), attrib->getWidth());

   }

   return;
}

/******************************************************************************
* write_comp_attributes
*/
static void write_comp_attributes(FILE *fo, CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      if (!attrib->isVisible())
      {
         continue;      // do not show an invisble attribute.
      }

      int tptr = update_textarray(attrib->getRotationRadians(), 0, attrib->getHeight(), attrib->getWidth());

      // if (attrib->inherited) continue;

      CString  layername = Layer_CADIF(attrib->getLayerIndex());

      if (strlen(layername) == 0)   layername = "L0";

      CString  key;
      key = doc->getKeyWordArray()[keyword]->out;

      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))     continue;

      char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));

      if ((tok = strtok(temp,"\n")) == NULL)
      {
         fprintf(fo,"%s(attribute \"%s\" \"%s\")\n", ident, key, "");
      }
      else
      {
         while (tok)
         {
            if (attrib->isVisible())
            {
               fprintf(fo,"%s(attribute \"%s\" \"%s\"\n", ident, key, tok);
               plusident();
               fprintf(fo,"%s(visible)\n", ident);
               fprintf(fo,"%s(attrDisp TS%d\n", ident, tptr);
               plusident();
               fprintf(fo,"%s(layerRef %s)\n", ident, layername);
               fprintf(fo,"%s(position (pt %ld %ld))\n", ident, cnv_unit(attrib->getX()), cnv_unit(attrib->getY()));
               close_b(fo);
               close_b(fo);
            }
            else
            {
               fprintf(fo,"%s(attribute \"%s\" \"%s\")\n", ident, doc->getKeyWordArray()[keyword]->out, tok);
            }

            tok = strtok(NULL,"\n");
         }
      }

      free(temp);
   }

   return;
}

/******************************************************************************
* get_padcoderot
*/
static int get_padcoderot(const char *p, int rot)
{
   for (int i=0;i<padcoderotcnt;i++)
   {
      if (!strcmp(padcoderotarray[i]->padname, p) && padcoderotarray[i]->rotation == rot)
         return i;
   }

   return -1;
}

/******************************************************************************
* CADIF_GetPadRotData
*/
void CADIF_GetPadRotData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale,
      int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      // here do attributes
      attributes_GetTextData(data->getAttributesRef());

      if (data->getDataType() != T_INSERT)
         continue;

      Point2 point2;

      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      // if inserting an aperture

      if (data->getInsert()->getInsertType() == INSERTTYPE_PIN && !block->getName().IsEmpty())
      {
         CString padname = block->getName();
         
         int rot = round(RadToDeg(rotation+data->getInsert()->getAngle()));
         while (rot < 0)
            rot += 360;
         while (rot >= 360)
            rot -= 360;
         if (rot)
         {
            CString degreerot;
            degreerot.Format("%d", rot);
            padname += "_";
            padname += degreerot;
            if (get_padcoderot(block->getName(), rot) < 0)
            {
               CADIFPadCodeRotStruct *padCode = new CADIFPadCodeRotStruct;
               padCode->padname = block->getName();     
               padCode->newpadname = padname;      
               padCode->bnum = block->getBlockNumber();
               padCode->rotation = rot;
               padcoderotarray.SetAtGrow(padcoderotcnt, padCode);
               padcoderotcnt++;
            }
         }
         GetPADShapesData(&block->getDataList(), rot);
      }

      if (!(block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)) // not aperture
      {
         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot = rotation + data->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

         CADIF_GetPadRotData(&block->getDataList(), point2.x, point2.y, block_rot, //rotation + data->getInsert().angle,
               block_mirror, scale * data->getInsert()->getScale(), embeddedLevel+1,-1);
      }
   }
}

/******************************************************************************
* CADIF_GetTextData
*/
void CADIF_GetTextData(CDataList *DataList, double insert_x, double insert_y, double rotation, 
                       int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // here do attributes
      attributes_GetTextData(np->getAttributesRef());

      switch(np->getDataType())
      {
         case T_POLY:
         break;

         case T_TEXT:
         {
            double text_rot;
   
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            text_rot = rotation + np->getText()->getRotation();
            if (mirror)
            {
               point2.x = -point2.x;
            }
      
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            Mat2x2   m1;
            RotMat2(&m1, text_rot);

            int text_mirror = np->getText()->getResultantMirror(mirror);

            update_textarray(text_rot, text_mirror, np->getText()->getHeight(), np->getText()->getWidth());

         }
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
            // if inserting an aperture

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               CADIF_GetTextData(&(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1,-1);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

} /* end CADIF_GetText */

/******************************************************************************
* write_cadif_string
*/
static int write_cadif_string(FILE *wfp, const char *s)
{
   fprintf(wfp, "%s(string \"", ident);
   for (int i=0; i<(int)strlen(s); i++)
   {
      if (s[i] == '"')
         fprintf(wfp, "\\");
      fprintf(wfp, "%c", s[i]);
   }
   fprintf(wfp, "\")\n");

   return 1;
}

/******************************************************************************
* CADIF_WriteSymData
*/
int CADIF_WriteSymData(FILE* wfp, CDataList *DataList, double insert_x, double insert_y, 
                       double rotation, int mirror, double scale, int insertLayer, int cnt)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   const char  *lname;
   int         layer;

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
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         int lnr = doc->get_layer_mirror(layer, mirror);
         if ((lname = Layer_CADIF(lnr)) == NULL)   continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            if (doc->get_layer_visible(np->getLayerIndex(), mirror))
            {
               CPoly *poly;
               CPnt  *pnt;
               POSITION polyPos, pntPos;

               // loop thru polys
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);
                  BOOL polyFilled = poly->isFilled();
                  BOOL closed     = poly->isClosed();

                  // do not write the boundary
                  if (poly->isFloodBoundary()) continue;

                  double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;

                  Point2   p;
                  
                  fprintf(wfp,"%s(symFig SD%d (symLayer G%s)\n", ident, ++cnt, lname);
                  plusident();
                  fprintf(wfp,"%s(pathFigure\n", ident);
                  plusident();
                  if (closed)
                     fprintf(wfp,"%s(shape\n", ident);
                  else
                     fprintf(wfp,"%s(openshape\n", ident);
                  plusident();

                  double   oldda = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);

                     // arc in cadif is on the end pt, in camcad in the start point
                     double da = atan(p.bulge) * 4;

                     if (fabs(oldda) > SMALLANGLE)
                     {
                        fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
                     }
                     else
                     {
                        fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
                     }
                     oldda = da;
                  }
                  close_b(wfp);  // path
                  fprintf(wfp,"%s(width %ld)\n", ident, cnv_unit(lineWidth));
                  close_b(wfp);  // 
                  close_b(wfp);
               }
            } // if color
         }
         break;

         case T_TEXT:
         {
            double text_rot;
   
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            text_rot = rotation + np->getText()->getRotation();
            if (mirror)
            {
               //text_rot = rotation - np->getText()->angle;
               point2.x = -point2.x;
            }

            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            Mat2x2   m1;
            RotMat2(&m1, text_rot);

            int text_mirror = np->getText()->getResultantMirror(mirror);

            int tptr = update_textarray(text_rot, text_mirror, np->getText()->getHeight(), np->getText()->getWidth());
            fprintf(wfp, "%s(symlabel SD%d TS%d\n", ident, ++cnt, tptr);
            plusident();
            write_cadif_string(wfp, np->getText()->getText());
            fprintf(wfp,"%s(symLayer G%s)\n", ident, lname);
            fprintf(wfp,"%s(position (pt %ld %ld))\n", ident, 
               cnv_unit(point2.x), cnv_unit(point2.y));
 
            close_b(wfp);
         }
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)   break;

            Point2 point2;
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               cnt = CADIF_WriteSymData(wfp, &(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(), -1, cnt);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for
   return cnt;
} /* end CADIF_WriteSymData */

/******************************************************************************
* CADIF_WriteAnnotate
*
*  write annotate writes only elements on !is_electrical layers
* including board outlines.
*/ 
void CADIF_WriteAnnotate(FILE* wfp, CDataList *DataList, double insert_x, double insert_y, 
                         double rotation, int mirror, double scale, int insertLayer, int *cnt)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   const char  *lname;
   int         layer, lnr;
   int         boardoutline = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         // nothing with a netname and etch. a netname without the gr_etch is a copper
         if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)                                       continue;
   
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         lnr = doc->get_layer_mirror(layer, mirror);
         if ((lname = Layer_CADIF(lnr)) == NULL)      continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            if (doc->get_layer_visible(np->getLayerIndex(), mirror))
            {
               CPoly *poly;
               CPnt  *pnt;
               POSITION polyPos, pntPos;

               int   boardfound = FALSE;
               // A boardoutline is graphic marked as Primary Boardoutline or placed on a layer->type LAYTYPE_BOARDOUTLINE
               LayerStruct *lstr = doc->getLayerArray()[lnr];

               if (lstr->getLayerType() == LAYTYPE_BOARD_OUTLINE || np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
                  boardfound = TRUE;

               // loop thru polys
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);
                  BOOL polyFilled = poly->isFilled();
                  BOOL closed     = poly->isClosed();
                  BOOL voidpoly   = poly->isVoid();

                  // do not write the boundary
                  if (poly->isFloodBoundary()) continue;

                  // A boardoutline is graphic marked as Primary Boardoutline or placed on a layer->type LAYTYPE_BOARDOUTLINE
                  if (boardfound)
                  {
                     if (!closed)
                     {
                        fprintf(flog," Board Outline found but not closed -> not used!\n");
                        display_error++;
                        boardfound = FALSE;
                     }
                     else
                     {
                        boardfound = TRUE;
                        boardoutline++;
                     }
                     if (boardoutline > 1)
                     {
                        fprintf(flog," Multiple Board Outlinea found, only the first is used!\n");
                        display_error++;
                        boardfound = FALSE;
                     }
                  }

                  double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;

                  Point2   p;

                  if (boardfound)
                  {
                     *cnt += 1;
                     fprintf(wfp,"%s(area A%d (name \"outline%d\")\n", ident, *cnt, cnt);
                     plusident();
                     fprintf(wfp,"%s(areaType BOARD_AREA)\n", ident);
                     fprintf(wfp,"%s(areaSide BOTH_AREA)\n", ident);
                     fprintf(wfp,"%s(areaWeight 100)\n", ident);
                     fprintf(wfp,"%s(areaHeights -1 -1)\n", ident);
                     fprintf(wfp,"%s(filledFigure\n", ident);
                  }
                  else
                  {
                     *cnt += 1;
                     int   lptr = Get_LayerRepPtr(lname);
                     if (lptr < 0)  break;;  

                     if (is_layertype_electrical(layerreparray[lptr]->layertype))   continue;

                     fprintf(wfp,"%s(annoFig AF%d (layerRef %s)\n", ident, *cnt, lname);

                     plusident();

                     if (polyFilled && closed)
                        fprintf(wfp,"%s(filledFigure\n", ident);
                     else
                        fprintf(wfp,"%s(pathFigure\n", ident);
                  }
                  plusident();

                  if (closed)
                     fprintf(wfp,"%s(shape\n", ident);
                  else
                     fprintf(wfp,"%s(openshape\n", ident);
                  plusident();

                  pntPos = poly->getPntList().GetHeadPosition();
                  double oldda = 0;
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);

                     double da = atan(p.bulge) * 4;

                     if (fabs(oldda) > SMALLANGLE)
                     {
                        fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
                     }
                     else
                     {
                        fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
                     }
                     oldda = da;
                  }

                  if (boardfound)
                  {
                     close_b(wfp);
                     fprintf(wfp,"%s(fillType CLEAR)\n", ident);
                     close_b(wfp);

                     fprintf(wfp,"%s(areaComps)\n", ident);
                     fprintf(wfp,"%s(fixed)\n", ident);
                     fprintf(wfp,"%s(keepIn)\n", ident);
                     close_b(wfp);
                     fprintf(wfp,"%s(bareBoard A%d)\n", ident, *cnt);
                  }
                  else
                  {
                     close_b(wfp);

                     if (polyFilled && closed)
                     {
                        // should not have a width !
                     }
                     else
                     {
                        fprintf(wfp,"%s(width %ld)\n", ident, cnv_unit(lineWidth));
                     }
                     close_b(wfp);
                     close_b(wfp);
                  }
               }
            } // if color
         }
         break;

         case T_TEXT:
         {
            double text_rot;
   
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            text_rot = rotation + np->getText()->getRotation();
            if (mirror)
            {
               point2.x = -point2.x;
            }
            
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            Mat2x2   m1;
            RotMat2(&m1, text_rot);

            int text_mirror = np->getText()->getResultantMirror(mirror);

            int tptr = update_textarray(text_rot, text_mirror, np->getText()->getHeight(), np->getText()->getWidth());
            fprintf(wfp, "%s(text TS%d\n", ident, tptr);
            plusident();
            write_cadif_string(wfp, np->getText()->getText());
            fprintf(wfp,"%s(layerRef %s)\n", ident, lname);
            fprintf(wfp,"%s(position (pt %ld %ld))\n", ident, 
               cnv_unit(point2.x), cnv_unit(point2.y));
 
            close_b(wfp);
         }
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;

            Point2 point2;
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               CADIF_WriteAnnotate(wfp, &(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(), -1, cnt);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for
   return;
} /* end CADIF_WriteAnnotate */

/******************************************************************************
* GetTypeIndex
*/
static int GetTypeIndex(CString typeName)
{
   for (int i=0; i<typeNameArrayCount; i++)
   {
      if (!typeNameArray[i].Compare(typeName))
         return i;
   }

   return -1;
}

/******************************************************************************
* CADIF_WriteComponents
*/
void CADIF_WriteComponents(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, 
                           double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   DataStruct  *np;
   Attrib* attrib;

   RotMat2(&m, rotation);
   
   for (POSITION pos = DataList->GetHeadPosition();pos != NULL;)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {         
      case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && np->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)
                break;

            Point2 point2;

            point2.x = np->getInsert()->getOriginX() *scale;
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
            {
               point2.x = -point2.x;
            }

            if (np->getInsert()->getMirrorFlags() & MIRROR_FLIP)
               block_rot = PI2 - block_rot;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            
            CADIFCompStruct *cmp = new CADIFCompStruct;
            cmp->name = np->getInsert()->getRefname();    
            comparray.SetAtGrow(compcnt, cmp);
            compcnt++;

            fprintf(wfp,"%s(component C%d (name \"%s\")\n", ident, compcnt, 
                  cadif_name(check_name('c', np->getInsert()->getRefname())));
            plusident();

/* 
   partRef is typelist entry
   packaltref is block index.
*/
            int typeptr = 0;

            if (attrib = is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
            {
               CString  devicename;
               devicename = get_attvalue_string(doc, attrib);
               typeptr = GetTypeIndex(devicename)+1;
            }
            else
            {
               typeptr = GetTypeIndex(block->getName())+1;
            }

            if (typeptr == 0)
            {
#ifdef _DEBUG
               fprintf(flog, "Type for [%s] not found !\n", block->getName());
               display_error++;
#endif
               fprintf(wfp,"%s(compDefn (packAltRef ALT%d))\n", ident,  block->getBlockNumber());
            }
            else
               fprintf(wfp,"%s(compDefn (partRef PRT%d) (packAltRef ALT%d))\n", ident, typeptr, block->getBlockNumber());

            fprintf(wfp,"%s(location\n", ident);
            plusident();
            /*TSR 3608: component rotation used to be written in radians!*/
            fprintf(wfp,"%s(orientation %d)\n", ident, cnv_rot(RadToDeg(block_rot)));

            if (np->getInsert()->getPlacedBottom())
               fprintf(wfp,"%s(side BOTTOM)\n", ident);

            fprintf(wfp,"%s(position (pt %ld %ld))\n", 
               ident, cnv_unit(point2.x), cnv_unit(point2.y));

            if (np->getInsert()->getMirrorFlags() & MIRROR_FLIP)
               fprintf(wfp,"%s(mirrored)\n", ident);

            close_b(wfp);

            CADIF_WriteCompPin(wfp, np->getInsert()->getRefname(), compcnt, &(block->getDataList()));

            if (attrib = is_attvalue(doc, np->getAttributesRef(), ATT_REFNAME, 1))
            {
               fprintf(wfp,"%s(attribute \"assembly_name\"  \"%s\"\n", ident,  np->getInsert()->getRefname());
               plusident();

               if (attrib->isVisible())
               {
                  int tptr = update_textarray(attrib->getRotationRadians(), 0, attrib->getHeight(), attrib->getWidth());

                  CString  layername = Layer_CADIF(attrib->getLayerIndex());

                  if (strlen(layername) == 0)   layername = "L0";

                  fprintf(wfp,"%s(visible)\n", ident);
                  fprintf(wfp,"%s(attrDisp TS%d\n", ident, tptr);
                  plusident();
                  fprintf(wfp,"%s(layerRef %s)\n", ident, layername);
                  fprintf(wfp,"%s(position (pt %ld %ld))\n", ident, cnv_unit(attrib->getX()), cnv_unit(attrib->getY()));
                  close_b(wfp);
               }

               close_b(wfp);
               
               fprintf(wfp,"%s(attribute \"silkscrn_name\"  \"%s\"\n", ident,  np->getInsert()->getRefname());
               plusident();

               if (attrib->isVisible())
               {
                  int tptr = update_textarray(attrib->getRotationRadians(), 0, attrib->getHeight(), attrib->getWidth());
                  CString  layername = Layer_CADIF(attrib->getLayerIndex());
                  if (strlen(layername) == 0)   layername = "L0";

                  fprintf(wfp,"%s(visible)\n", ident);
                  fprintf(wfp,"%s(attrDisp TS%d\n", ident, tptr);
                  plusident();
                  fprintf(wfp,"%s(layerRef %s)\n", ident, layername);
                  fprintf(wfp,"%s(position (pt %ld %ld))\n", ident, cnv_unit(attrib->getX()), cnv_unit(attrib->getY()));
                  close_b(wfp);
               }
               close_b(wfp);
            }
            write_comp_attributes(wfp, np->getAttributesRef());

            close_b(wfp);
         } // case INSERT
         break;

      } // end switch
   } // end for
   return;

} /* end CADIF_WriteComponents */

/******************************************************************************
* PCB_WriteROUTESData
*/
void PCB_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, double rotation, 
                         int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   const char  *ln;

   // CString  lay;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != GR_CLASS_ETCH)      continue;

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         LayerStruct *l = doc->FindLayer(layer);
         ln = l->getName();
         if (ln == NULL)   continue;

         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
         {
            if (strcmp(get_attvalue_string(doc, a),netname))
               continue;
         }  
         else
            continue;
      }
      else
      {
         // here check if ATTR_NETNAME == netname

         // allow only vias and test points
         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
         {
            if (strcmp(get_attvalue_string(doc, a),netname))
               continue;
         }  
         else
            continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               
               // do not write the boundary
               if (poly->isFloodBoundary()) continue;

               double width = cnv_unit(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA());

               // do not do poly thermallines
               if (poly->isThermalLine()) 
               {
                  // this is done in CADIF_WriteCopper()
                  continue;
               }
               else
               // no polyvoids
               if (poly->isVoid())  
               {
                  continue;
               }
               
               if (polyFilled || closed)
               {
                  continue;
               }
               
               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               char lay = get_layernr_from_index(layer);
               if (lay < 0)   break;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {                 pnt = poly->getPntList().GetNext(pntPos);

                  p2.x = cnv_unit(pnt->x);
                  if (mirror) p2.x = -p2.x;
                  p2.y = cnv_unit(pnt->y);
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  if (first)
                  {
                     firstp = p2;
                     p1 = p2;
                     first = FALSE;
                  }
                  else
                  {
                     // char layer = (char)atoi(lay); // limit 32 layers
                     int err = LoadSegment(p1.x, p1.y, p2.x, p2.y, lay, poly->getWidthIndex());
                     if (err & 1)
                     {
#ifdef DUMP
                        fprintf(flog,"Segment length Error -> SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                           p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, width);
#endif
                     }
                     else
                     if (err & 2)
                     {
                        fprintf(flog,"Layer Error -> SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                           p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, width);
                     }
                     else
                     {
#ifdef DUMP
                        fprintf(flog,"SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                           p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, width);
#endif
                     }
                     p1 = p2;
                  }
               }
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            // no text in route section
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       break;

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = cnv_unit(np->getInsert()->getOriginX());
            if (mirror)
               point2.x = -point2.x;

            point2.y = cnv_unit(np->getInsert()->getOriginY());
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            int p = get_padstack_index(block->getName());
            int pccode = padstackarray[p]->pcnum;

            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               Attrib *a;
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_VIALAYER, 1))
                  fprintf(flog,"Via range not implemented\n");
#ifdef DUMP
               fprintf(flog,"%d VIA X=%lf Y=%lf P=%s\n",viacnt, point2.x, point2.y,block->name);
#endif
               UINT64 layermap = ALL_INT64;

               fprintf(wfp,"%s(via V%d\n", ident, ++totalviacnt);
               plusident();
               fprintf(wfp,"%s(position (pt %ld % ld))\n", ident, round(point2.x), round(point2.y));
               fprintf(wfp,"%s(padCodeRef PC%d)\n", ident, pccode);
               fprintf(wfp,"%s(layerRange L%d L%d)\n", ident, top, bot);
               close_b(wfp);

               // vias are loaded as endpopints, because the tracing has to stop there
               LoadEndpoint(-(viacnt+1), point2.x, point2.y, layermap);
               CADIFEndPointStruct *e = new CADIFEndPointStruct;
               endpointarray.SetAtGrow(endpointcnt, e);
               endpointcnt++;
               e->index = -(viacnt+1);

               CADIFViaStruct *v = new CADIFViaStruct;
               v->vianame = block->getName();  
               v->totalviacnt = totalviacnt;
               viaarray.SetAtGrow(viacnt, v);
               viacnt++;
            
               break;
            }

            if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
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

               PCB_WriteROUTESData(wfp, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, netname);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end PCB_WriteROUTESData */
   return;
}

/******************************************************************************
* CADIF_WriteCopperData
*
* this writes copper with netnames -- copper without netnames are written
* in writeannotate
*/
void CADIF_WriteCopperData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, double rotation, 
                           int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer, lnr;
   const char  *ln;
   CString  netname;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      netname = "";

      if (np->getDataType() != T_INSERT)
      {
         // cadif_annotate for not do copper fills.
         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
            netname = get_attvalue_string(doc, a);

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         LayerStruct *l = doc->FindLayer(layer);
         ln = l->getName();
         if (ln == NULL)   continue;
         lnr = doc->get_layer_mirror(layer, mirror);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            const char *lname;
            if ((lname = Layer_CADIF(lnr)) == NULL)   break;
            int   lptr = Get_LayerRepPtr(lname);
            if (lptr < 0)                             break;   
            if (!is_layertype_electrical(layerreparray[lptr]->layertype))  break;

            int   copperfound = FALSE;
            int   boundaryfound = FALSE;
            // only copper with netnames has arrived here.
            // here check that the copper has one filled closed copper area
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               
               // do not write the boundary
               if (poly->isFloodBoundary()) 
               {
                  boundaryfound = TRUE;
                  continue;
               }
               if (polyFilled && closed)  // if it is not filled and closed, it must be open
               {
                  copperfound = TRUE;
               }
            }

            if (!copperfound)
            {
               // do not do etches.
               if (np->getGraphicClass() == GR_CLASS_ETCH)      break;

               // template - only gets the outer polygon
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);

                  // do not write the boundary
                  if (poly->isFloodBoundary())                   continue;

                  double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale;

                  BOOL polyFilled = poly->isFilled();
                  BOOL closed     = poly->isClosed();
                  Point2   p;

                  coppercnt++;
   
                  fprintf(wfp,"%s(copper CU%d\n", ident, coppercnt);
                  plusident();
                  fprintf(wfp,"%s(layerRef %s)\n", ident, lname);

                  fprintf(wfp,"%s(pathFigure\n", ident);
                  plusident();
                  fprintf(wfp,"%s(openShape\n", ident);
                  plusident();

                  double   oldda = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);

                     // arc in cadif is on the end pt, in camcad in the start point
                     double da = atan(p.bulge) * 4;

                     if (fabs(oldda) > SMALLANGLE)
                     {
                        fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, 
                           cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
                     }
                     else
                     {
                        fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
                     }
                     oldda = da;
                  }
                  close_b(wfp);  // openshape
                  fprintf(wfp,"%s(width %ld)\n", ident, cnv_unit(lineWidth));

                  close_b(wfp);  // pathfigure
                  close_b(wfp);  // copper
               }
               break;
            }

            // this function assimes a good poly | void combination.

            // it must have been electrical and closed.
            // if copper has a closed void or closed fill arrive here.
            coppercnt++;
            if (COPPERTEMPLATE)
            {
               if (boundaryfound)
               {
                  // here now I know I need to write a copper area in 3 sections
                  // partial
                  fprintf(wfp,"%s(partialPp PPP%d (name \"partialpp%d\")\n",ident, 
                     coppercnt, coppercnt);
                  plusident();
                  fprintf(wfp,"%s(minArea 0)\n", ident);
                  fprintf(wfp,"%s(minDrawWidth 12700)\n", ident);
                  fprintf(wfp,"%s(extraIsolation 0)\n", ident);
                  fprintf(wfp,"%s(facetApprox 8)\n", ident);
                  fprintf(wfp,"%s(pinBoxing)\n", ident);
                  if (strlen(netname))
                     fprintf(wfp,"%s(signalRef S%d)\n", ident, get_netindex(netname)+1);
                  close_b(wfp);

                  // template - only gets the outer polygon
                  polyPos = np->getPolyList()->GetHeadPosition();
                  while (polyPos != NULL)
                  {
                     poly = np->getPolyList()->GetNext(polyPos);
                     BOOL polyFilled = poly->isFilled();
                     BOOL closed     = poly->isClosed();

                     // do not write the boundary
                     if (!poly->isFloodBoundary())   continue;
   
                     Point2   p;
                     fprintf(wfp,"%s(template TMP%d\n", ident, coppercnt);
                     plusident();
                     fprintf(wfp,"%s(pathFigure\n", ident);
                     plusident();
                     fprintf(wfp,"%s(shape\n", ident);
                     plusident();

                     double   oldda = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
                        p.x = pnt->x * scale;
                        if (mirror) p.x = -p.x;
                        p.y = pnt->y * scale;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);

                        // arc in cadif is on the end pt, in camcad in the start point
                        double da = atan(p.bulge) * 4;
   
                        if (fabs(oldda) > SMALLANGLE)
                        {
                           fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, 
                              cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
                        }
                        else
                        {
                           fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
                        }
                        oldda = da;
                     }
                     close_b(wfp);
                     close_b(wfp);
                     fprintf(wfp,"%s(partialPpRef PPP%d)\n", ident, coppercnt);
                     close_b(wfp);
                  }
               }
               else
               {
                  // here now I know I need to write a copper area in 3 sections
                  // partial
                  fprintf(wfp,"%s(partialPp PPP%d (name \"partialpp%d\")\n",ident, 
                     coppercnt, coppercnt);
                  plusident();
                  fprintf(wfp,"%s(minArea 0)\n", ident);
                  fprintf(wfp,"%s(minDrawWidth 12700)\n", ident);
                  fprintf(wfp,"%s(extraIsolation 0)\n", ident);
                  fprintf(wfp,"%s(facetApprox 8)\n", ident);
                  fprintf(wfp,"%s(pinBoxing)\n", ident);
                  if (strlen(netname))
                     fprintf(wfp,"%s(signalRef S%d)\n", ident, get_netindex(netname)+1);
                  close_b(wfp);

                  // template - only gets the outer polygon
                  polyPos = np->getPolyList()->GetHeadPosition();
                  while (polyPos != NULL)
                  {
                     poly = np->getPolyList()->GetNext(polyPos);
                     BOOL polyFilled = poly->isFilled();
                     BOOL closed     = poly->isClosed();

                     // do not write the boundary
                     if (poly->isFloodBoundary()) continue;
   
                     if (polyFilled && closed)
                     {
                        //fprintf(wfp,"// AREA [%s]\n", netname);
                     }
                     else
                     {
                        // this is done in PCB_Route data segment
                        break;   // not an area of other special  
                     }
   
                     Point2   p;
                     fprintf(wfp,"%s(template TMP%d\n", ident, coppercnt);
                     plusident();
                     fprintf(wfp,"%s(pathFigure\n", ident);
                     plusident();
                     fprintf(wfp,"%s(shape\n", ident);
                     plusident();

                     double   oldda = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
                        p.x = pnt->x * scale;
                        if (mirror) p.x = -p.x;
                        p.y = pnt->y * scale;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);

                        // arc in cadif is on the end pt, in camcad in the start point
                        double da = atan(p.bulge) * 4;
   
                        if (fabs(oldda) > SMALLANGLE)
                        {
                           fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, 
                              cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
                        }
                        else
                        {
                           fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
                        }
                        oldda = da;
                     }
                     close_b(wfp);
                     close_b(wfp);
                     fprintf(wfp,"%s(partialPpRef PPP%d)\n", ident, coppercnt);
                     close_b(wfp);
                     break;   // only ther first outer one
                  }
               }  // if boundaryfound
            }

            // copper and voids
/*
            fprintf(wfp,"%s(copper CU%d\n", ident, coppercnt);
            plusident();
            fprintf(wfp,"%s(layerRef %s)\n", ident, lname);
            fprintf(wfp,"%s(filledFigure\n", ident);
            plusident();
*/
            int   voidmode = FALSE;
            int   newcopper = TRUE;
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
   
               // do not write the boundary
               if (poly->isFloodBoundary()) continue;

               Point2   p;

               // only closed voids and filled
               if (!voidmode && poly->isVoid()) 
               {
                  voidmode = TRUE;
                  fprintf(wfp,"%s(cutout\n", ident);
                  plusident();
               }
               else
               if (polyFilled)
               {
                  if (!newcopper)
                  {
                     close_b(wfp);  // close filled figure
                     if (COPPERTEMPLATE)
                        fprintf(wfp,"%s(partialPpRef PPP%d)\n", ident, coppercnt);
                     if (strlen(netname))
                        fprintf(wfp,"%s(signalRef S%d)\n", ident, get_netindex(netname)+1);
                     close_b(wfp);  // close copper
                  }
                  newcopper = FALSE;
                  voidmode = FALSE;
                  // this starts a new copper
                  fprintf(wfp,"%s(copper CU%d\n", ident, ++coppercnt);
                  plusident();
                  fprintf(wfp,"%s(layerRef %s)\n", ident, lname);
                  fprintf(wfp,"%s(filledFigure\n", ident);
                  plusident();
               }
               else
               {
                  continue;
               }

               fprintf(wfp,"%s(shape\n", ident);
               plusident();

               double   oldda = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  //wcadif_Graph_Width(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale);
                  p.x = pnt->x * scale;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);

                  // arc in cadif is on the end pt, in camcad in the start point
                  double da = atan(p.bulge) * 4;

                  if (fabs(oldda) > SMALLANGLE)
                  {
                     fprintf(wfp,"%s(arc (e %d -1)(pt %ld %ld))\n", ident, 
                        cnv_arc(oldda)*10, cnv_unit(p.x), cnv_unit(p.y));
                  }
                  else
                  {
                     fprintf(wfp,"%s(pt %ld %ld)\n", ident, cnv_unit(p.x), cnv_unit(p.y));
                  }
                  oldda = da;
               }
               close_b(wfp);  // close shape
            
               if (voidmode)  // only if it write a cutout
                  close_b(wfp);
               
            }
            close_b(wfp);  // close filled figure

            if (COPPERTEMPLATE)
               fprintf(wfp,"%s(partialPpRef PPP%d)\n", ident, coppercnt);
            if (strlen(netname))
               fprintf(wfp,"%s(signalRef S%d)\n", ident, get_netindex(netname)+1);
            close_b(wfp);  // close copper
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            // no text in route section
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)            break;

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = cnv_unit(np->getInsert()->getOriginX());
            if (mirror)
               point2.x = -point2.x;

            point2.y = cnv_unit(np->getInsert()->getOriginY());
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
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

               CADIF_WriteCopperData(wfp, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end CADIF_WriteCopperData */
   return;
}

/******************************************************************************
* get_padstacklayermap
*/
static UINT64 get_padstacklayermap(const char *p, int mirror)
{
   for (int i=0;i<padstackcnt;i++)
   {
      if (!strcmp(padstackarray[i]->padstackname,p))
      {
         if (mirror)
            return padstackarray[i]->mirrormap;
      
         return padstackarray[i]->layermap;
      }
   }

   return ALL_INT64;
}

/******************************************************************************
* get_comppinindex
*/
static int get_comppinindex(const char *c, const char *p)
{

   for (int i=0;i<comppincnt;i++)
   {
      if (comppinarray[i]->compname.Compare(c) == 0 && comppinarray[i]->pinname.Compare(p) == 0)
         return i;
   }

   fprintf(flog,"CompPin [%s] [%s] error\n", c, p);
   display_error++;

   return -1;
}

/******************************************************************************
* write_cadifcomppinref
*/
static int write_cadifcomppinref(FILE *fp, const char *c, const char *p)
{

   int   cptr = get_comppinindex(c, p);
   if (cptr < 0)  return -1;
   fprintf(fp,"%s(compPinRef T%d (compRef C%d))\n", ident, 
      comppinarray[cptr]->pinindex, comppinarray[cptr]->compindex);
   
   return 1;
}

/******************************************************************************
* do_signalpinloc
*/
static int do_signalpinloc(FILE *fp, NetStruct *net, double scale)
{
   CompPinStruct *compPin;
   POSITION compPinPos;
   int      pinindex = 0;
   
   compPinPos = net->getHeadCompPinPosition();
   while (compPinPos != NULL)
   {
      compPin = net->getNextCompPin(compPinPos);

      if (compPin->getPinCoordinatesComplete())
      {
         double  pinx, piny, pinrot;
         int     padmirror;
         CString padstackname;

         pinx = cnv_unit(compPin->getOriginX());
         piny = cnv_unit(compPin->getOriginY());
         pinrot = RadToDeg(compPin->getRotationRadians());
         padmirror = compPin->getMirror();

         if (compPin->getPadstackBlockNumber() > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
            if (block == NULL)   continue;
            padstackname = block->getName();
         }
         else
         {
            fprintf(flog, "No PinLoc found for Comp [%s] Pin [%s]\n", compPin->getRefDes(), compPin->getPinName());
            display_error++;
            continue;
         }

         UINT64 layermap = get_padstacklayermap(padstackname,padmirror);
         pinindex = get_comppinindex(compPin->getRefDes(), compPin->getPinName());

#ifdef DUMP
         fprintf(flog, "index %d PIN X=%lf Y=%lf R=%s.%s P=%s layermap [%x] mirror %d\n",
               pinindex, pinx*scale, piny*scale,
               compPin->getRefDes(), compPin->getPinName(),padstackname, layermap, padmirror);
#endif

         LoadEndpoint(pinindex, pinx, piny, layermap);
         CADIFEndPointStruct *e = new CADIFEndPointStruct;
         endpointarray.SetAtGrow(endpointcnt, e);
         endpointcnt++;
         e->index = pinindex;

         write_cadifcomppinref(fp, compPin->getRefDes(), compPin->getPinName());
      }
   }
   return 1;
}                                                        

/******************************************************************************
* get_junctionindex
*/
static int get_junctionindex(FILE *stream, long x, long y, int layer)
{
   for (int i=0;i<junctioncnt;i++)
   {
      if (junctionarray[i]->x == x && junctionarray[i]->y == y && junctionarray[i]->layer == layer)
         return i+1;
   }

   CADIFJunctionStruct *jnction = new CADIFJunctionStruct;
   junctionarray.SetAtGrow(junctioncnt, jnction);
   junctioncnt++;
   jnction->x = x;
   jnction->y = y;
   jnction->layer = layer;

   fprintf(stream,"%s(juncPt JP%d (position (pt %ld %ld)))\n", ident, junctioncnt, x, y);

   return junctioncnt;
}

/******************************************************************************
* cadif_joins
*/
static void cadif_joins(FILE *stream, Net_Path *node)
{
   if (node->type == ENDPOINT_NODE)
   {
      if (node->index_id < 0)
      {
         // via
         // (viaRef V744)
         int v = viaarray[abs(node->index_id) - 1]->totalviacnt;
         fprintf(stream,"%s(viaRef V%d)\n", ident, v);
      }
      else
      {
         // pin
         // (compPinRef T6 (compRef C246))
         fprintf(stream,"%s(compPinRef T%d (compRef C%d))\n", ident,
            comppinarray[node->index_id]->pinindex,
            comppinarray[node->index_id]->compindex);
      }
   }
   else
   {
      // this can not happen !
   }
}

/******************************************************************************
* Write_Traces
*/
void Write_Traces(FILE *stream, CResultList *resultList, const char *netname)
{
   if (resultList == NULL) return;

   Net_Path *node, *lastnode;
   /* struct Net_Path
      {
         char type; // VIA_NODE, SEGMENT_NODE, ENDPOINT_NODE
         int index_id;
         BOOL reverse; // segment goes from p2 to p1
      }; */
   CPathList *path;

#ifdef DUMP
   fprintf(flog, "DumpResults: NET \'%s\'\n", netname);
   DumpResult(flog,resultList);
#endif

   int   lastindex  = endpointarray[0]->index;

   if (endpointcnt < 2)
   {
      fprintf(flog, "Net [%s] has [%d] pins -> need to be converted to copper = not implemented.\n",
         netname, endpointcnt);
      display_error++;
      return;
   }

   // this is needed to guaranty connectitity in any order between all pins.
   for (int i=1;i<endpointcnt;i++)
   {
      fprintf(stream,"%s(connection\n", ident);
      plusident();
      fprintf(stream,"%s(joins\n", ident);
      plusident();

      if (endpointarray[i]->index < 0)
      {
         // via
         // (viaRef V744)
         int v = viaarray[abs(endpointarray[i]->index) - 1]->totalviacnt;
         fprintf(stream,"%s(viaRef V%d)\n", ident, v);
      }
      else
      {
         // pin
         // (compPinRef T6 (compRef C246))
         fprintf(stream,"%s(compPinRef T%d (compRef C%d))\n", ident,
                  comppinarray[endpointarray[i]->index]->pinindex,
                  comppinarray[endpointarray[i]->index]->compindex);
      }

      if (lastindex < 0)
      {
         // via
         // (viaRef V744)
         int v = viaarray[abs(lastindex) - 1]->totalviacnt;
         fprintf(stream,"%s(viaRef V%d)\n", ident, v);
      }
      else
      {
         // pin
         // (compPinRef T6 (compRef C246))
         fprintf(stream,"%s(compPinRef T%d (compRef C%d))\n", ident,
            comppinarray[lastindex]->pinindex,
            comppinarray[lastindex]->compindex);
      }
      close_b(stream);
      close_b(stream);
      lastindex = endpointarray[i]->index;
   }
   // end of connectivity loop


   // here starts the routing.
   POSITION resPos, pathPos;
   // loop result paths
   // POSITION resPos, pathPos;
   resPos = resultList->GetHeadPosition();

   lastnode = NULL;
   // here get 1 endpoint, incase the trace has no enpoint connection
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      path = resultList->GetNext(resPos)->path;
      pathPos = path->GetHeadPosition();
      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
               lastnode = node;
            break;
            case SEGMENT_NODE:
            break;
         }
      }
   }

   // here write trace by trace. This is done to allow 
   // 1. traces without start in a pin
   // 2. necked linewidth
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      lastnode = NULL;
      path = resultList->GetNext(resPos)->path;
      // loop nodes of this path
      pathPos = path->GetHeadPosition();
      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
            {
               lastnode = node;
            }
            break;
            case SEGMENT_NODE:
            {
               // if no next node, trace ends in itself without a pin.
               Net_Path *nextnode = node;
               if (pathPos)   
                  nextnode = path->GetAt(pathPos);

               // a segment is between lastnode and next node
               int lastid, nextid;
               if (lastnode == NULL || lastnode->type != ENDPOINT_NODE)
               {
                  Net_Segment *seg = segmentArray->GetAt(node->index_id);
                  if (node->reverse)
                  {
                     lastid = get_junctionindex(stream, round(seg->x2), round(seg->y2), seg->layer);
                  }
                  else
                  {
                     lastid = get_junctionindex(stream, round(seg->x1), round(seg->y1), seg->layer);
                  }
               }
               if (nextnode->type != ENDPOINT_NODE)   // if the next node is not an endpoint, make a junction
               {  
                  Net_Segment *seg = segmentArray->GetAt(node->index_id);
                  if (node->reverse)   // where does the current node end.
                  {
                     nextid = get_junctionindex(stream, round(seg->x1), round(seg->y1), seg->layer);
                  }
                  else
                  {
                     nextid = get_junctionindex(stream, round(seg->x2), round(seg->y2), seg->layer);
                  }
               }

               fprintf(stream,"%s(route\n", ident);
               plusident();

               fprintf(stream,"%s(joins\n", ident);
               plusident();
               if (lastnode == NULL || lastnode->type != ENDPOINT_NODE)
                  fprintf(stream,"%s(juncPtRef JP%d)\n", ident, lastid);
               else
                  cadif_joins(stream, lastnode);
                  
               if (nextnode->type != ENDPOINT_NODE)   // if the next node is not an endpoint, make a junction
                  fprintf(stream,"%s(juncPtRef JP%d)\n", ident, nextid);
               else
                  cadif_joins(stream, nextnode);

               close_b(stream);  // joins

               Net_Segment *seg = segmentArray->GetAt(node->index_id);
               long lineWidth = cnv_unit(doc->getWidthTable()[seg->widthIndex]->getSizeA());
               CString  l = get_layercadifindex(seg->layer);
               fprintf(stream,"%s(layerRef %s)\n", ident, l);
               fprintf(stream,"%s(pathFigure\n", ident);
               plusident();
               fprintf(stream,"%s(path\n", ident);
               plusident();

               if (node->reverse)
               {
                  fprintf(stream,"%s(pt %ld %ld)\n", ident, round(seg->x2), round(seg->y2));
                  fprintf(stream,"%s(pt %ld %ld)\n", ident, round(seg->x1), round(seg->y1));
               }
               else
               {
                  fprintf(stream,"%s(pt %ld %ld)\n", ident, round(seg->x1), round(seg->y1));
                  fprintf(stream,"%s(pt %ld %ld)\n", ident, round(seg->x2), round(seg->y2));
               }

               close_b(stream);  // path
               fprintf(stream,"%s(width %ld)\n", ident, lineWidth);
               close_b(stream);  // pathfigure
               close_b(stream);  // route
               lastnode = node;
            }
            break;
         }
      }
   }
   return;
}

// end CADIF_O.CPP
