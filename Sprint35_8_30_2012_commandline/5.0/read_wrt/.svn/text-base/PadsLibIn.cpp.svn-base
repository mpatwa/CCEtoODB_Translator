// $Header: /CAMCAD/5.0/read_wrt/PadsLibIn.cpp 33    2/27/07 1:58p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// begin paste from PADSIN.CPP

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
#include "logwrite.h"
#include "lyrmanip.h"
#include "menlib.h"  // just for date test
#include "CCEtoODB.h"
#include "DeviceType.h"
#include "RwUiLib.h"

// begin paste of PADSIN.H
#pragma once

#define  MAX_LINE                2000  /* Max line length.           */
#define  TEXT_HEIGHTFACTOR       (1.0/1.4)   // if a text is .12 is PADS ASCII, it is really only
                                       // 0.1 in graphic
#define  TEXT_WIDTHFACTOR        (0.55)   

#define  FID_ATT                 1
#define  SMD_ATT                 2
#define  DEVICE_ATT              3
#define  HEIGHT_ATT              4

#define  ARC_COUNTERCLOCK        1
#define  ARC_CLOCK               2

#define  MAX_ALTDECAL            32    // in PowerPCB 2.0 is upded to 16 from 4 -> allow more if a type@shape is used
#define  MAX_LAYERS              255   /* Max number of layers.   */
#define  MAX_PADSTACK_SIZE       253   // size of padstack arrays

typedef struct
{
   double   height,linewidth;
}PADSRefnamesize;

typedef struct
{
   CString  layerName;        // pads layer name
   char  layerType;           // layer attributes as defined in dbutil.h
   int   stacknumber;   // electrical stack number
} PADSAdef;

typedef struct 
{
   CString  ptypename;
   CString  originaldecalname[MAX_ALTDECAL];    // this is as defined in DECAL section
   CString  decalname[MAX_ALTDECAL];            // this is the name TYPE.DECAL
   unsigned int altused;                        // 0..15 alts are allowed, this flags which ones are used in the component list
   int      decalcnt;
   CString  value;
   CString  tolerance;
   CString  typ;
   CString  pinname;       // pinname , pinname 
   int      smdflag:1;     // flags derived from attributes.
   int      fiducial:1;
   int      alphapins:1;   // check if a part contains alpha pins 
   double   compheight;
} PADSPart;
typedef CTypedPtrArray<CPtrArray, PADSPart*> CPartArray;

typedef struct
{
   CString  name;
   int   atttype;
   DeviceTypeTag   devicetype;
}PADSAttr;
typedef CTypedPtrArray<CPtrArray, PADSAttr*> CAttrArray;

typedef struct
{
   int      stacknumber;      
   int      component;     // this must be set to make the associated layers active
   CString  layer_name;
   CString  layer_type;
   CString  associated_silk_screen;
   CString  associated_paste_mask;
   CString  associated_solder_mask;
   CString  associated_assembly;
}PADSMiscLayer;
typedef CTypedPtrArray<CPtrArray, PADSMiscLayer*> CMiscLayerArray;

typedef struct
{
   CString  name;
}PADSViadef;
typedef CTypedPtrArray<CPtrArray, PADSViadef*> CViadefArray;

typedef struct
{
   int      pinnr;
   CString  pinname;
   CString  decalname;
}PADSPadsIgnored;
typedef CTypedPtrArray<CPtrArray, PADSPadsIgnored*> CPadsignoredArray;

typedef struct
{
   CString  compname;
   CString  geomname;
   int      pinnr;
   CString  pinname;
   int      already_reported;
}PADSCompPinIgnored;
typedef CTypedPtrArray<CPtrArray, PADSCompPinIgnored*> CCompPinignoredArray;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
}PADSAttribmap;
typedef CTypedPtrArray<CPtrArray, PADSAttribmap*> CAttribmapArray;

typedef struct 
{
   CString  name;
   DataStruct  *data_adress;
} PADSPour;
typedef CTypedPtrArray<CPtrArray, PADSPour*> CPourArray;

typedef struct 
{
   int         pinnr;
   int         layer;
   BlockStruct *pinblock;
} PADSPincopper;
typedef CTypedPtrArray<CPtrArray, PADSPincopper*> CPincopperArray;

typedef struct
{
   double   x,y;
   int      padstackindex;
} PADSTerminal;
typedef CArray<PADSTerminal, PADSTerminal&> CTerminalArray;
#ifdef DEADCODE
typedef struct
{
   char     form;
   double   sizeA;
   double   sizeB;
   double   offset;
   double   rotation;
}PADS_Padform;
typedef CArray<PADS_Padform, PADS_Padform&> CPadformArray;
#endif
typedef struct
{
   double drillsize;
   int   padstack[MAX_PADSTACK_SIZE];  // -2 .. 30
   int   typ;           // 1 top, 3 inner, 2 bottom 4 drill
   CString padstackName;
}PADS_Paddef;
typedef CArray<PADS_Paddef, PADS_Paddef&> CPaddefArray;

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

static int   pads_LibHDR_Lines();
static int   pads_LibHDR_Sch();
static int   pads_LibHDR_Decal();
static int   pads_LibHDR_Type();
static int   pads_Lib_end();

static char  lib_type_flag;

static List  start_lst[] =
{
    "*PADS-LIBRARY-LINE-ITEMS-V4*",  pads_LibHDR_Lines,
    "*PADS-LIBRARY-SCH-DECALS-V4*",  pads_LibHDR_Sch,
    "*PADS-LIBRARY-PCB-DECALS-V4*",  pads_LibHDR_Decal,
    "*PADS-LIBRARY-PART-TYPES-V4*",  pads_LibHDR_Type,
    "*END*",                         pads_Lib_end,
};

#define  SIZ_START_LST             (sizeof(start_lst) / sizeof(List))

static int   pads_Lib_lines();
static int   pads_Lib_decal();
static int   pads_Lib_type();
static int   pads_Lib_part();

// end paste of padsin.h

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg  *progress;
extern char *testaccesslayers[];

static void plInitMem();
static void plFreeMem();
static int Load_PadsLibSettings();
static int pl_do_assign_layers();
static int set_boundary_to_hidden(BlockStruct *Block);
static int check_header(List *c_list, int siz_of_command);
static char *get_nextline(char *string,int n,FILE *fp);
static int pads_Lib_skip();
static double cnv_unit(double x);
static DataStruct *pl_get_pieces(double x, double y, const char *name,const char *linetype, const char *shapename);
static int pl_get_text(double x, double y, const char *name, double *height);
static int pl_get_label(CAttributes** map, double x, double y, const char *name);
static int pl_do_padstacklayers();


static int PageUnits;
static FileStruct *file = NULL;
static FILE *ferr;
static FILE *ifp;                            /* File pointers.    */
static int display_error = 0;
static int display_log;

static long ifp_linecnt;
static CCEtoODBDoc *doc;
static char ifp_line[MAX_LINE];

static int PushTok = FALSE;
static int eof_found;
static char unit_flag = ' ';           /* I = inch M = metric A = mil B = basic */
static char heightunit_flag = ' ';     /* I = inch M = metric A = mil B = basic */
static int title_found = FALSE;                                       
static int file_version = 2;
static int layer_mode = 30;
static int layer_assign = FALSE;    // since PADS Version 3 there is the complete Layerassign defined.
static PADSAdef layer_attr[MAX_LAYERS];    /* Array of layers from pdif.in   */
static int layer_attr_cnt;

static CPartArray partarray;     // this is the device - mapping
static int partcnt;

static CPourArray pourarray;  
static int pourcnt;

static CPincopperArray pincopperarray; 
static int pincoppercnt;

static CAttrArray attrarray;  
static int attrcnt;

static CMiscLayerArray misclayerarray; 
static int misclayercnt;

static CViadefArray viadefarray; 
static int viadefcnt;

static CPadsignoredArray padsignoredarray;   
static int padsignoredcnt;

static CCompPinignoredArray comppinignoredarray;   
static int comppinignoredcnt;

static CTerminalArray terminalarray;
static int terminalcnt;

static CPaddefArray paddefarray;
static int paddefcnt;

static int unroutelayer;

static int maxlayer; // number of layers
static int drilllayernum;
static int max_copper_layer;

static CString compoutline[30];  // allow upto 30 different layers for component outline
static int compoutlinecnt;

static PADSRefnamesize refnamesize;

static char keep_unrout_layer;
static char non_electrical_pins;
static int elim_double_vias;

static CString unnamednet[30];
static int unnamednetcnt;

static int ComponentSMDrule;     // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static CAttribmapArray attribmaparray;
static int attribmapcnt = 0;
static int PCB_SECTION_FOUND;

/******************************************************************************
* ReadPADSLib
*/
void ReadPADSLib(const char *fullPath, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   doc = Doc;
   PageUnits = pageunits;

   // CAD file
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(fullPath, drive, dir, fname, ext);
   CString fileName = fname;
   fileName += ext;

   ifp = fopen(fullPath, "rt");
   if (!ifp)
   {
      ErrorMessage("Error open file", fullPath);
      return;
   }

   // log file
   CString padsLibLogFile = GetLogfilePath("PadsLibRead.log");
   ferr = fopen(padsLibLogFile, "wt");
   if (!ferr)
   {
      ErrorMessage("Error open file", padsLibLogFile);
      return;
   }
   log_set_filename(fullPath); //what is this??

   plInitMem(); //is this needed??

   // init globals
   display_error = FALSE;
   display_log = FALSE;
   ifp_linecnt = 0;
   PushTok = FALSE;
   eof_found = FALSE;
   title_found = FALSE;

   file = Graph_File_Start(fileName, fileTypePadsLibrary);
   file->setBlockType(blockTypeGeomLibrary);
   file->getBlock()->setBlockType(blockTypeGeomLibrary);

   drilllayernum = Graph_Level("DRILLHOLE", "", FALSE);

   Load_PadsLibSettings();

   int res = check_header(start_lst, SIZ_START_LST);

   if (res == 2)
      res = pads_Lib_type();
   else if (res == 3)
      res = pads_Lib_decal();
   else if (res == 4)
      res = pads_Lib_decal();
   else if (res == 5)
      res = pads_Lib_lines();
   else
      res = -1;

   if (res > -1)
   {
      progress->SetStatus("Optimizing Database");
      double accuracy = get_accuracy(doc);
      // this can happen on strange via placements.
      EliminateSinglePointPolys(doc);                
      BreakSpikePolys(file->getBlock()); 

      // here do padstack layers
      pl_do_padstacklayers();

      // here assign other layers
      pl_do_assign_layers();

      update_smdpads(doc);
      update_smdrule_geometries(doc, ComponentSMDrule);

      OptimizePadstacks(doc, pageunits, 0); // this can create unused blocks
      generate_PADSTACKACCESSFLAG(doc, 1);

   }  // aborted due to unrecoverable syntax error
   else
   {
      ErrorMessage("Unrecoverable Syntax Error in PADS ASCII file!", "PADS File Load Aborted!");
   }

   plFreeMem();

   fclose(ifp);
   fclose(ferr); 

   if (display_error && display_log)
      Logreader(padsLibLogFile);
}

/******************************************************************************
* plInitMem
*/
static void plInitMem()
{
   partarray.SetSize(100,100);
   partcnt = 0;

   pourarray.SetSize(100,100);
   pourcnt = 0;

   pincopperarray.SetSize(100,100);
   pincoppercnt = 0;

   attrarray.SetSize(100,100);
   attrcnt = 0;

   misclayerarray.SetSize(100,100);
   misclayercnt = 0;

   terminalarray.SetSize(100,100);
   terminalcnt = 0;

   viadefarray.SetSize(100,100);
   viadefcnt = 0;

   padsignoredarray.SetSize(100,100);
   padsignoredcnt = 0;

   comppinignoredarray.SetSize(100,100);
   comppinignoredcnt = 0;

   paddefarray.SetSize(100,100);
   paddefcnt = 0;
}

/******************************************************************************
* plFreeMem
*/
static void plFreeMem()
{
   pincopperarray.RemoveAll();

	int i=0;
   for (i=0;i<pourcnt;i++)
      delete pourarray[i];
   pourarray.RemoveAll();

   for (i=0;i<attrcnt;i++)
      delete attrarray[i];
   attrarray.RemoveAll();

   for (i=0;i<misclayercnt;i++)
      delete misclayerarray[i];
   misclayerarray.RemoveAll();

   for (i=0;i<attribmapcnt;i++)
      delete attribmaparray[i];
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   for (i=0;i<partcnt;i++)
      delete partarray[i];
   partarray.RemoveAll();

   for (i=0;i<viadefcnt;i++)
      delete viadefarray[i];
   viadefarray.RemoveAll();

   for (i=0;i<padsignoredcnt;i++)
      delete padsignoredarray[i];
   padsignoredarray.RemoveAll();

   for (i=0;i<comppinignoredcnt;i++)
      delete comppinignoredarray[i];
   comppinignoredarray.RemoveAll();

   terminalarray.RemoveAll();

   paddefarray.RemoveAll();   // pad with drill and padstack array
}

/******************************************************************************
* clean_rot
*/
static double clean_rot(double rot)
{
   rot = round(rot * 10) / 10;
   while (rot < 0)      
      rot += 360;
   while (rot >= 360)
      rot -= 360;

   return rot;
}

/******************************************************************************
* char *get_attribmap
*/
static const char *get_attribmap(const char *c)
{
   for (int i=0; i<attribmapcnt; i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   return c;
}

/******************************************************************************
* is_attrib
*/
static int is_attrib(const char *s,int atttype)
{
   for (int i=0; i<attrcnt; i++)
   {
      if (attrarray[i]->atttype == atttype && !STRICMP(attrarray[i]->name,s))
         return 1;
   }

   return 0;
}

/******************************************************************************
* get_mappeddeviceindex
*/
static DeviceTypeTag get_mappeddeviceindex(const char *s)
{
   for (int i=0; i<attrcnt; i++)
   {
      if (attrarray[i]->atttype == DEVICE_ATT && !STRICMP(attrarray[i]->name,s))
         return attrarray[i]->devicetype;
   }

   return deviceTypeUnknown;
}

/******************************************************************************
* update_attr
*/
static int update_attr(char *lp,int typ)
{
   PADSAttr *attr = new PADSAttr;
   attrarray.SetAtGrow(attrcnt, attr);  
   attrcnt++;  
   attr->name = lp;
   attr->atttype = typ;
   attr->devicetype = deviceTypeUndefined;
   return attrcnt-1;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0; i<MAX_LAYTYPE; i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//   return   LAYTYPE_UNKNOWN; 
//}

/******************************************************************************
* Load_PadsLibSettings
*/
static int Load_PadsLibSettings()
{
   FILE *fp;
   char line[255];
   char *lp;

   unnamednetcnt = 0;
   compoutlinecnt = 0;
   refnamesize.height = 0.07 * Units_Factor(UNIT_INCHES, PageUnits);
   refnamesize.linewidth = 0;
   non_electrical_pins = 'n';
   keep_unrout_layer = FALSE;
   elim_double_vias = FALSE;
   ComponentSMDrule = 0;
   layer_attr_cnt = 0;

   CString settingsFile( getApp().getImportSettingsFilePath("PadsLib.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nPADS Lib Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);

   if ((fp = fopen(settingsFile, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",settingsFile);
      ErrorMessage(tmp, "Pads Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         // here do it.
         if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            ComponentSMDrule = atoi(lp);
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString attr1 = lp;
            attr1.MakeUpper();
            attr1.TrimLeft();
            attr1.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString attr2 = lp;
            attr2.MakeUpper();
            attr2.TrimLeft();
            attr2.TrimRight();

            PADSAttribmap  *attribMap = new PADSAttribmap;
            attribmaparray.SetAtGrow(attribmapcnt++, attribMap);
            attribMap->attrib = attr1;
            attribMap->mapattrib = attr2;
         }
         else if (!STRICMP(lp, ".COMPOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString compOutline = lp;
            compOutline.TrimLeft();
            compOutline.TrimRight();

            if (compoutlinecnt < 30)
            {
               compoutline[compoutlinecnt] = compOutline;
               compoutlinecnt++;
            }
            else
            {
               fprintf(ferr, "Too many compoutlines defined.\n");
               display_error++;
            }

         }
         else if (!STRCMPI(lp, ".DISPLAYLOG"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "y", 1))
               display_log = TRUE;
         }
         else if (!STRICMP(lp, ".HEIGHTATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp,HEIGHT_ATT);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if( islower( lp[0] ) )
               heightunit_flag =  _toupper( lp[0] );
            else
               heightunit_flag = lp[0];
         }
         else if (!STRICMP(lp, ".SMDATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, SMD_ATT);
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
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
               layer_attr[layer_attr_cnt].layerName = olayer;
               layer_attr[layer_attr_cnt].layerType = laytype;
               layer_attr_cnt++;
            }
            else
            {
               fprintf(ferr, "Too many layer attributes\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".MIRRORLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString lay1 = lp;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString lay2 = lp;

            Graph_Level(lay1, "", 0);
            Graph_Level(lay2, "", 0);
            Graph_Level_Mirror(lay1, lay2, "");
         }
      }
   }

   fclose(fp);
   return 1;
}

/******************************************************************************
* get_padslayertype
*/
static int get_padslayertype(int ptr)
{
   PADSMiscLayer *miscLayer = misclayerarray[ptr];       
   if (!miscLayer->layer_type.CompareNoCase("ROUTING"))
   {
      if (miscLayer->stacknumber == 1)
         return LAYTYPE_SIGNAL_TOP;
      else if (miscLayer->stacknumber == maxlayer)
         return LAYTYPE_SIGNAL_BOT;
      else
         return LAYTYPE_UNKNOWN; // this could already been marked as a plane
   }
   else if (!miscLayer->layer_type.CompareNoCase("SOLDER_MASK"))
   {
      // here need to find if it is top or bottom
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_MASK_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_MASK_BOTTOM;
      else
         return LAYTYPE_MASK_ALL;
   }
   else if (!miscLayer->layer_type.CompareNoCase("PASTE_MASK"))
   {
      // here need to find if it is top or bottom;
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_PASTE_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_PASTE_BOTTOM;
      else
         return LAYTYPE_PASTE_ALL;
   }
   else if (!miscLayer->layer_type.CompareNoCase("SILK_SCREEN"))
   {
      // here need to find if it is top or bottom
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_SILK_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_SILK_BOTTOM;
      else
         return LAYTYPE_UNKNOWN;
   }
   else if (!miscLayer->layer_type.CompareNoCase("DRILL"))
   {
      return LAYTYPE_DRILL;
   }
   else if (!miscLayer->layer_type.CompareNoCase("ASSEMBLY"))
   {
      if (miscLayer->layer_name.Find("Top") > 0)
         return LAYTYPE_TOP;
      else if (miscLayer->layer_name.Find("Bottom") > 0)
         return LAYTYPE_BOTTOM;
      else
         return LAYTYPE_UNKNOWN;
   }
   else
   {
      return LAYTYPE_UNKNOWN;
   }
}

/******************************************************************************
* pl_do_assign_layers
*  make layers more understandable for PADS users.
*/
static int pl_do_assign_layers()
{
   LayerStruct *layer;

   if (layer = doc->FindLayer(drilllayernum))
   {
      layer->setComment("Drill");
      layer->setLayerType(LAYTYPE_DRILL);
   }

   if (layer = doc->FindLayer_by_Name("BOARD_OUTLINE"))
   {
      layer->setComment("Board_Outline");
      layer->setLayerType(LAYTYPE_BOARD_OUTLINE);
   }

   if (layer = doc->FindLayer_by_Name("ALL"))
   {
      layer->setComment("All");
      layer->setLayerType(LAYTYPE_ALL);
   }


   int layerNum = Graph_Level("PADLAYER_TOP", "", 0);
   layer = doc->FindLayer(layerNum);
   layer->setComment("Pad Top");
   layer->setLayerType(LAYTYPE_PAD_TOP);

   layerNum = Graph_Level("PADLAYER_INNER", "", 0);
   layer = doc->FindLayer(layerNum);
   layer->setComment("Pad Inner");
   layer->setLayerType(LAYTYPE_PAD_INNER);

   layerNum = Graph_Level("PADLAYER_BOT", "" ,0);
   layer = doc->FindLayer(layerNum);
   layer->setComment("Pad Bottom");
   layer->setLayerType(LAYTYPE_PAD_BOTTOM);

   // switch padlayer off
	int i=0;
   for (i=2; i<maxlayer; i++)
   {
      CString layerName;
      layerName.Format("PADLAYER_%d", i);

      if (layer = doc->FindLayer_by_Name(layerName))
      {
         layer->setLayerType(LAYTYPE_PAD_INNER);
         layer->setElectricalStackNumber(i);
      }
   }

   // here attributes from pads.in
   for (i=0; i<doc->getMaxLayerIndex(); i++)
   {
      if (!(layer = doc->getLayerArray()[i]))
         continue;

      for (int j=0; j<layer_attr_cnt; j++)
      {
         if (layer->getName().CompareNoCase(layer_attr[j].layerName) == 0)
         {
            layer->setLayerType(layer_attr[j].layerType);
            break;
         }
      }
   }

   // tech section was missing, make "1" as the SIGNAL_TOP and "max" as SIGNAL_BOTTOM
   if (maxlayer == 0 && max_copper_layer > 0)
   {
      for (i=1; i<=max_copper_layer; i++)
      {
         CString layerName;
         layerName.Format("%d", i);
         if (layer != doc->FindLayer_by_Name(layerName))
            continue;
         if (layer->getLayerType() != 0)
            continue;

         if (i > 16)
         {
            fprintf(ferr, "Missing PCB header -> Suspicious Copper Layer [%d] detected\n", i);
            display_error++;
         }
         layer->setLayerType(LAYTYPE_SIGNAL);
         layer->setElectricalStackNumber(i);
      }
   }

   if (!layer_assign)
   {
      CString layer1, layer2;
      layer1.Format("DECAL_%d", 1);
      layer2.Format("DECAL_%d", maxlayer);

      int layerNum = Graph_Level(layer1, "", 0);
      layer = doc->FindLayer(layerNum);
      layer->setComment("Component_Outline_Top");
      layer->setLayerType(LAYTYPE_SILK_TOP);
   
      layerNum = Graph_Level(layer2, "", 0);
      layer = doc->FindLayer(layerNum);
      layer->setComment("Component_Outline_Bottom");
      layer->setLayerType(LAYTYPE_SILK_BOTTOM);
   
      // map decal layers
      Graph_Level_Mirror(layer1, layer2, "");

      // needed to map copper on decals
      layer1.Format("%d", 1);
      layer2.Format("%d", maxlayer);
      Graph_Level_Mirror(layer1, layer2, "");
   }
   else
   {

      CString layerName = ""; 
      int misclayertop = -1;
      int misclayerbot = -1;
      for (i=0; i<misclayercnt; i++)
      {
         PADSMiscLayer *miscLayer = misclayerarray[i];

         layerName.Format("%d", miscLayer->stacknumber);
         if (layer = doc->FindLayer_by_Name(layerName))
         {
            layer->setName( miscLayer->layer_name);
            layer->setComment(layerName);

            int layerType = get_padslayertype(i);
            if (layerType)
               layer->setLayerType(layerType);
         }
      
         if (miscLayer->stacknumber == 1)
            misclayertop = i;
         else if (miscLayer->stacknumber == maxlayer)
            misclayerbot = i;
         else
         {
            layerName.Format("DECAL_%d", miscLayer->stacknumber);
            LayerStruct *origLayer = doc->FindLayer_by_Name(layerName);
            if (origLayer && layer)
               MoveLayer(doc, origLayer, layer, TRUE);
         }
      }

      if (misclayertop > -1 && misclayerbot > -1)
      {
         PADSMiscLayer *layerTop = misclayerarray[misclayertop];
         PADSMiscLayer *layerBot = misclayerarray[misclayerbot];

         if (strlen(layerTop->associated_assembly) && strlen(layerBot->associated_assembly))
            Graph_Level_Mirror(layerTop->associated_assembly, layerBot->associated_assembly, "");

         if (strlen(layerTop->associated_silk_screen) && strlen(layerBot->associated_silk_screen))
            Graph_Level_Mirror(layerTop->associated_silk_screen, layerBot->associated_silk_screen, "");

         if (strlen(layerTop->associated_paste_mask) && strlen(layerBot->associated_paste_mask))
            Graph_Level_Mirror(layerTop->associated_paste_mask, layerBot->associated_paste_mask, "");

         if (strlen(layerTop->associated_solder_mask) && strlen(layerBot->associated_solder_mask))
            Graph_Level_Mirror(layerTop->associated_solder_mask, layerBot->associated_solder_mask, "");

         if (strlen(layerTop->layer_name) && strlen(layerBot->layer_name))
            Graph_Level_Mirror(layerTop->layer_name, layerBot->layer_name, "");

         // here now copy and merge the DECAL_%d layers to the correct Silk etc...
         // move DECAL_1 to associated_silk_screen on top
         // move DECAL_%d to associated_silk_screen on bottom
         LayerStruct *origLayer;
         LayerStruct *newLayer;
         if (origLayer = doc->FindLayer_by_Name("DECAL_1"))
         {
            if (newLayer = doc->FindLayer_by_Name(layerTop->associated_silk_screen))
               MoveLayer(doc, origLayer, newLayer, TRUE);
         }

         CString layerName;
         layerName.Format("DECAL_%d", maxlayer);
         if (origLayer = doc->FindLayer_by_Name(layerName))
         {
            if (newLayer = doc->FindLayer_by_Name(layerBot->associated_silk_screen))
               MoveLayer(doc, origLayer, newLayer, TRUE);
         }
      }
   }
   return 1;
}

/******************************************************************************
* set_boundary_to_hidden
*/
static int set_boundary_to_hidden(BlockStruct *Block)
{
   POSITION dataPos = Block->getDataList().GetHeadPosition();

   while (dataPos) 
   {
      DataStruct *data = Block->getDataList().GetNext(dataPos);
      if (data->getDataType() != T_POLY)
         continue;
      
      CPoly *boundarypoly = NULL;
      int filled = FALSE;

      POSITION polyPos = data->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = data->getPolyList()->GetNext(polyPos);
         if (poly->isFloodBoundary())
            boundarypoly = poly;
         else if (poly->isFilled())
            filled = TRUE;
      }

      // hide a boundary if a filled has been detected inside.
      // that means a pour was flooded.
      if (boundarypoly && filled)
         boundarypoly->setHidden(true);
   }

   return 1;
}

/******************************************************************************
* pl_do_padstacklayers
*/
static int pl_do_padstacklayers()
{
   CString lname;
   int i;
   LayerStruct *l;

   // -2 is top
   lname.Format("PADLAYER_TOP");
   i = Graph_Level(lname, "", 0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_TOP);

   // -1 is inner
   lname.Format("PADLAYER_INNER");
   i = Graph_Level(lname, "", 0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_INNER);

   // 0 is bottom
   lname.Format("PADLAYER_BOT");
   i = Graph_Level(lname, "", 0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   return 1;
}

/******************************************************************************
* write_one_padstack
*/
static int write_one_padstack(int i)
{
   CString  name;
   int smd = 0;
   int index = i;
   name.Format("PADSTACK_%d", index);
   while (Graph_Block_Exists(doc, name, file->getFileNumber()))
   {
      name.Format("PADSTACK_%d", ++index);
   }
   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, name, file->getFileNumber(),0);
   curblock->setBlockType(BLOCKTYPE_PADSTACK);

   PADS_Paddef paddef = paddefarray.ElementAt(i);
   paddef.padstackName = name;
   DbFlag padstackflag = 0;      

   for (int ii=0; ii<MAX_PADSTACK_SIZE; ii++)
   {
      CString lname;
      if (paddef.padstack[ii] > -1)
      {
         if (ii == 0)
         {
            lname = "PADLAYER_TOP";
            padstackflag |= BL_ACCESS_TOP;
         }
         else if (ii == 1)
         {
            lname = "PADLAYER_INNER";
         }
         else if (ii == 2)
         {
            lname = "PADLAYER_BOT";
            padstackflag |= BL_ACCESS_BOTTOM;
         }
         else if (ii == 3)
         {
            lname = "PADLAYER_TOP";
            padstackflag |= BL_ACCESS_TOP;
         }
         else if (ii == maxlayer+2)
         {
            lname = "PADLAYER_BOT";
            padstackflag |= BL_ACCESS_BOTTOM;
         }
         else
            lname.Format("PADLAYER_%d", ii-2);
            
         int layernum = Graph_Level(lname, "", 0);
         if (ii > 2)
            LayerStruct *l = doc->FindLayer(layernum);

         BlockStruct *block = doc->Find_Block_by_Num(paddef.padstack[ii]);
         DataStruct *data = Graph_Block_Reference(block->getName(), NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);

         if (ii == 0)      // -2 top 
            smd |= 1;
         else if (ii == 1) // -1 inner
            smd |= 2;
         else if (ii == 2) // 0  bottom
            smd |= 4;
      }
   }     

   if (paddef.drillsize > 0)
   {
      int drillayer = Graph_Level("DRILLHOLE", "", 0);
      BlockStruct *block = Graph_FindTool(paddef.drillsize, 0, TRUE);

      // must be filenum 0, because apertures are global.
      DataStruct* toolData = Graph_Block_Reference(block->getName(), NULL, file->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, drillayer, TRUE);
      toolData->getInsert()->setInsertType(insertTypeDrillHole);
   }

   Graph_Block_Off();
   curblock->setFlagBits(padstackflag);

   if (smd == 1)
      doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);
   
   paddefarray.SetAt(i, paddef);
   return 1;
}

/******************************************************************************
* is_command
*/
static int is_command(char *l, List *c_list, int siz_of_command)
{
   for (int i=0; i<siz_of_command; i++)
   {
      if (!_strnicmp(c_list[i].token, l, strlen(c_list[i].token)))
         return i;
   }
   return -1;
}

/******************************************************************************
* check_header
*/
static int check_header(List *c_list, int siz_of_command)
{
   // here do a while loop
   int res;
   while (get_nextline(ifp_line, MAX_LINE, ifp) && !eof_found)
   {
      CString tmp = ifp_line;

      char *lp = strtok(ifp_line, " \t\n");
      if (lp == NULL)
         continue;

      int code = is_command(lp, c_list, siz_of_command);
      if (code < 0)
      {
         fprintf(ferr, "Unknown PADS Section [%s] at %ld\n", lp, ifp_linecnt);
         display_error++;
         pads_Lib_skip();
      }
      else
      {
         res = (*c_list[code].function)();
         if ( res < 0)
            return -1;
         return res;
      }
   }

   return 1;
}

/******************************************************************************
* *get_nextline
*/
static char *get_nextline(char *string, int n, FILE *fp)
{
   static CString last_line;
   static char *last_res;
   long last_linecnt;
   char *res;
                                   
   if (PushTok)
   {
      strcpy(string, last_line);
      res = last_res;
      PushTok = FALSE;
   }
   else
   {
      int remark = TRUE;
      while (remark)
      {
         ifp_linecnt++;
         res = fgets(string, MAX_LINE, fp);
         if (res == NULL)
         {
            eof_found = TRUE;
            return NULL;
         }
         last_line = string;
         last_line.TrimLeft();
         last_line.TrimRight();
         last_linecnt = ifp_linecnt;

         CString l = last_line.Left(4);
         l.TrimRight(); 
         if (l.GetLength() == 0)
            continue;
         if (l != "*REM")
            remark = FALSE;
         last_res = res;
      }
   }
   
   // clean right 
   CString tmp = last_line;
   tmp.TrimRight();
   strcpy(string, tmp);

   return res;
}

/******************************************************************************
* pads_Lib_skip
*/
static int pads_Lib_skip()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = strtok(ifp_line," \t\n");
      if (lp == NULL)
         continue;
   }

   return 1;
}
    
/******************************************************************************
* int do_misclayer_layer
*  From 
*  {
*     {
*     }
*  }  to blacket closed
*
*  short    layertype;              // see dbutil.h dbutil.cpp
*  short    artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
*  short    electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
*  short    physicalstacknumber;    // phusycal manufacturing stacking of layers, 
*                                   // signaltop / dialectric / signal_1 / dia / power_1 .... / solder
*  
*  unsigned long attr;     // layer attributes
*/
static int  do_misclayer_layer(int layernr)
{
   int bracket_cnt = 0;
   LayerStruct *ll;
   
   layer_assign = TRUE;

   PADSMiscLayer *miscLayer = new PADSMiscLayer;
   misclayerarray.SetAtGrow(misclayercnt++, miscLayer);
   miscLayer->stacknumber = layernr;
   miscLayer->component = FALSE;    // this must be set to make the associated layers active
   miscLayer->layer_name.Format("Layer_%d", layernr);
   miscLayer->associated_silk_screen = "";
   miscLayer->associated_paste_mask = "";
   miscLayer->associated_solder_mask = "";
   miscLayer->associated_assembly = "";
   miscLayer->layer_type = "";

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = get_string(ifp_line," \t\n");
      if (lp == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "COMPONENT")) 
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         if (!STRCMPI(lp, "Y"))
            miscLayer->component = TRUE;
      }
      else if (!STRCMPI(lp,"ASSOCIATED_SILK_SCREEN")) 
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_silk_screen = lp;
         miscLayer->associated_silk_screen.TrimLeft();
         miscLayer->associated_silk_screen.TrimRight();
      }
      else if (!STRCMPI(lp, "ASSOCIATED_ASSEMBLY"))   
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_assembly = lp;
         miscLayer->associated_assembly.TrimLeft();
         miscLayer->associated_assembly.TrimRight();
      }
      else if (!STRCMPI(lp, "ASSOCIATED_PASTE_MASK")) 
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_paste_mask = lp;
         miscLayer->associated_paste_mask.TrimLeft();
         miscLayer->associated_paste_mask.TrimRight();
      }
      else if (!STRCMPI(lp, "ASSOCIATED_SOLDER_MASK"))   
      {
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         miscLayer->associated_solder_mask = lp;
         miscLayer->associated_solder_mask.TrimLeft();
         miscLayer->associated_solder_mask.TrimRight();
      }
      else if (!STRCMPI(lp, "LAYER_NAME"))   // layername are comments.
      {
         CString layer_name;
         CString lname;
         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         layer_name = lp;
         layer_name.TrimLeft();
         layer_name.TrimRight();

         lname.Format("%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            ll->setComment(layer_name);

         // here now check if a decal layer was 
         lname.Format("DECAL_%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            ll->setComment(layer_name);
         miscLayer->layer_name = layer_name;
      }
      else if (!STRCMPI(lp, "LAYER_THICKNESS"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         double h = cnv_unit(atof(lp));
         if (h > 0)
         {
            CString lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               doc->SetAttrib(&ll->getAttributesRef(), doc->IsKeyWord(LAYATT_THICKNESS, TRUE), VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL); 
         }
      }
      else if (!STRCMPI(lp, "LAYER_TYPE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         miscLayer->layer_type = lp;
      }
      else if (!STRCMPI(lp, "PLANE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;

         if (!STRCMPI(lp, "CAM"))
         {
            CString lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               ll->setLayerType(LAYTYPE_POWERNEG);
         }
         else if (!STRCMPI(lp, "MIXED"))
         {
            CString lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               ll->setLayerType(LAYTYPE_SPLITPLANE);
         }
      }
      else if (!STRCMPI(lp, "NET"))
      {
         // Need to pass false for the optional value in get_string function because sometimes net name starts with "\" - Joanne
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL)
            continue;

         CString lname;
         lname.Format("%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            doc->SetUnknownAttrib(&ll->getAttributesRef(), LAYATT_NETNAME, lp, SA_APPEND, NULL); // x, y, rot, height

         NetStruct *net = add_net(file,lp);
         if (net)
            doc->SetUnknownAttrib(&net->getAttributesRef(), ATT_POWERNET, "", SA_OVERWRITE, NULL); //  
      }
      else if (!STRCMPI(lp, "COPPER_THICKNESS"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         double h = cnv_unit(atof(lp));
         if (h > 0)
         {
            CString  lname;
            lname.Format("%d", layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               doc->SetAttrib(&ll->getAttributesRef(),doc->IsKeyWord(LAYATT_COPPER_THICKNESS, TRUE), VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL); 
         }
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString val = lp;

         CString  lname;
         lname.Format("%d", layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            doc->SetUnknownAttrib(&ll->getAttributesRef(), get_attribmap(key), val, SA_OVERWRITE, NULL); // x, y, rot, height
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattributepart_values
*/
static int  do_miscattributepart_values(const char *refname)
{
   int   bracket_cnt = 0;

   // if this is a via, find it and assign cur_netname
   DataStruct *data = datastruct_from_refdes(doc, file->getBlock(), refname);

   if (data == NULL)
   {
      fprintf(ferr, "PART [%s] not found at %ld\n", refname, ifp_linecnt);
      display_error++;
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = get_string(ifp_line," \t\n");

      if (lp == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (data)
         {
            if (is_attrib(key, HEIGHT_ATT))
            {
               char save_unit = unit_flag;
               unit_flag = heightunit_flag;

               double h = cnv_unit(atof(val));
               doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE,&h,SA_OVERWRITE, NULL);
               unit_flag = save_unit;
            }
            else
            {
               doc->SetUnknownAttrib(&data->getAttributesRef(), get_attribmap(key), val, SA_OVERWRITE, NULL);
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattributeparttype_values
*/
static int  do_miscattributeparttype_values(const char *typname)
{
   int bracket_cnt = 0;

   TypeStruct *type = AddType(file, typname);

   if (type == NULL)
   {
      fprintf(ferr, "PARTTYPE [%s] not found at %ld\n", typname, ifp_linecnt);
      display_error++;
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = get_string(ifp_line," \t\n");

      if (lp == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
                       
      if (!STRCMPI(lp,"{"))   //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (type)
         {
            if (is_attrib(key, HEIGHT_ATT))
            {
               char save_unit = unit_flag;
               unit_flag = heightunit_flag;

               double h = cnv_unit(atof(val));
               doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE,&h,SA_OVERWRITE, NULL);
               unit_flag = save_unit;
            }
            else
            {
               doc->SetUnknownAttrib(&type->getAttributesRef(), get_attribmap(key), val, SA_OVERWRITE, NULL);
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattributedecal_values
*/
static int do_miscattributedecal_values(const char *decalname)
{
   int bracket_cnt = 0;
   BlockStruct *block;

   block = Graph_Block_Exists(doc, decalname, -1);

   if (block == NULL)
   {
      fprintf(ferr, "DECAL [%s] not found at %ld\n", decalname, ifp_linecnt);
      display_error++;
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = get_string(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();

         if ((lp = get_string(NULL, "\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (block)
         {
            if (is_attrib(key, HEIGHT_ATT))
            {
               char save_unit = unit_flag;
               unit_flag = heightunit_flag;
   
               double h = cnv_unit(atof(val));
               doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL); 
               unit_flag = save_unit;
            }
            else
            {
               doc->SetUnknownAttrib(&block->getAttributesRef(), get_attribmap(key), val, SA_OVERWRITE, NULL); //  
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattributenet_values
*/
static int do_miscattributenet_values(const char *netname)
{
   int bracket_cnt = 0;

   NetStruct *net = add_net(file, netname);

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = get_string(ifp_line, "\"\t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  //
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "}"))   // 
      {
         // do nothing
      }
      else
      {
         
         if (!STRCMPI(lp, "DFT."))
            if ((lp = get_string(NULL, "\"\n")) == NULL) //keywords are quoted, spaces acceptable
                    continue;
         
         CString key = lp;
         key.TrimLeft();
         key.TrimRight();
         key.MakeUpper();


         if ((lp = get_string(NULL, " \"\n")) == NULL)
            continue;
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();
         doc->SetUnknownAttrib(&net->getAttributesRef(), get_attribmap(key), val, SA_OVERWRITE, NULL);
         
      }
   }

   return 1;
}

/******************************************************************************
* int do_miscattribute_values
*/
static int  do_miscattribute_values()
{
   int bracket_cnt = 0;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = get_string(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "{"))  
      {
         // do nothing
      }
      else
      if (!STRCMPI(lp, "}"))  
      {
         // do nothing
      }
      else if (!STRCMPI(lp, "PARTTYPE"))  
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         do_miscattributeparttype_values(lp);
      }
      else if (!STRCMPI(lp, "PART"))   
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         do_miscattributepart_values(lp);
      }
      else if (!STRCMPI(lp, "NET")) 
      {
         // Need to pass false for the optional value in get_string function because sometimes net name starts with "\" - Joanne
         if ((lp = get_string(NULL, " \t\n", FALSE)) == NULL)
            continue;
         do_miscattributenet_values(lp);
      }
      else if (!STRCMPI(lp, "DECAL"))  
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         do_miscattributedecal_values(lp);
      }
      else
      {
/*
         fprintf(ferr,"Attribute association [%s] not implemented at %ld\n",
            lp, ifp_linecnt);
         display_error++;
*/
      }
   }

   return 1;
}

/******************************************************************************
* int do_misclayer
*  From 
*  {
*     {
*     }
*  }  to blacket closed
*/
static int do_misclayer()
{
   int bracket_cnt = 0;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
      
      if (!STRCMPI(lp, "LAYER"))
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         int laynr = atoi(lp);
         do_misclayer_layer(laynr);
      }
   }

   return 1;
}

/******************************************************************************
* int misc_skip
   From 
   {
      {
      }
   }  to blacket closed
*/
static int misc_skip(const char *l)
{
   int bracket_cnt = 0;

   if (!strcmp(l, "{"))
      bracket_cnt++;
   if (!strcmp(l, "}"))
      bracket_cnt--;

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (!strcmp(lp, "{"))
         bracket_cnt++;
      if (!strcmp(lp, "}"))
         bracket_cnt--;
      if (bracket_cnt == 0)
         break;
   }

   return 1;
}

/******************************************************************************
* pads_Lib_misc
*/
static int pads_Lib_misc()
{
   // is done here because this writes all padstack layers.
   // misc can update comments to layers.

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp = strtok(ifp_line, " \t\n");
      if (lp == NULL)
         continue;
      
      if (!STRCMPI(lp, "LAYER"))
         do_misclayer();
      else if (!STRCMPI(lp, "ATTRIBUTES"))
         misc_skip(lp);
      else if (!STRCMPI(lp, "ATTRIBUTE"))
      {
         if ((lp = strtok(NULL, "\n")) == NULL)
            continue;
         CString tmp = lp;
         tmp.TrimLeft();
         tmp.TrimRight();
         if (!tmp.Compare("VALUES"))
            do_miscattribute_values();
         else
            misc_skip(lp);
      }
      else
         misc_skip(lp);
   }

   return 1;
}
    
/******************************************************************************
* pads_LibHDR_Type
*/
static int pads_LibHDR_Type()
{
   title_found = TRUE;
   file_version = 4;
   lib_type_flag = 'T'; 
   unit_flag = 'A';  // units are in mil
   return 2;
}
/******************************************************************************
* pads_LibHDR_Decal
*/
static int pads_LibHDR_Decal()
{
   title_found = TRUE;
   file_version = 4;
   lib_type_flag = 'D'; 
   unit_flag = 'A';  // units are in mil
   return 3;
}
/******************************************************************************
* pads_LibHDR_Sch
*/
static int pads_LibHDR_Sch()
{
   title_found = TRUE;
   file_version = 4;
   lib_type_flag = 'S'; 
   unit_flag = 'A';  // units are in mil
   return 4;
}
/******************************************************************************
* pads_LibHDR_Lines
*/
static int pads_LibHDR_Lines()
{
   title_found = TRUE;
   file_version = 4;
   lib_type_flag = 'L'; 
   unit_flag = 'A';  // units are in mil
   return 5;
}

/******************************************************************************
* pads_Lib_matrix
*/
static int pads_Lib_matrix()
{
   pads_Lib_skip();  
   return 1;
}
/******************************************************************************
* pads_Lib_text
*/
static int pads_Lib_text()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;   
      double rot = atof(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      if (!STRCMPI(lp, "0"))
         lp = "ALL";
      int level = Graph_Level(lp, "", 0);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;   
      double height = cnv_unit(atof(lp)) * TEXT_HEIGHTFACTOR;
      double width = height * TEXT_WIDTHFACTOR;

      if ((lp = strtok(NULL," \t\n")) == NULL)
         continue;   
      double penwidth = cnv_unit(atof(lp));

      int mirror = FALSE;
      if ((lp = strtok(NULL," \t\n")) != NULL) // mirror
         if (lp[0] == 'M') mirror = TRUE;

      ifp_linecnt++;    // here get prosa
      fgets(ifp_line, MAX_LINE, ifp);
      CString tmp = ifp_line;
      tmp.TrimRight();
      strcpy(ifp_line, tmp);
      if ((lp = strtok(ifp_line, "\n")) == NULL)
         continue;

      if (strlen(lp))
         Graph_Text(level, lp, x, y, height, width, DegToRad(rot), 0 , TRUE, mirror, 0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
   }

   return 1;
}

/******************************************************************************
* pads_Lib_lines
*/
static int pads_Lib_lines()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char *lp;
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;

      CString name = lp;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;

      CString linetype = lp;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;

      double x = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;

      double y = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;

      int pieces = atoi(lp);
      if (pieces > 9999)   // build a check in so that we do not trace bad stuff
      {
         fprintf(ferr, "PADS Syntax error at %ld\n", ifp_linecnt);
         display_error++;
         break;
      }
      int textcnt= 0;
   
      // if COPPER the text is not there, but the signal name maybe 
      if (linetype.CompareNoCase("COPPER")) // check if not equal
      {
         if ((lp = strtok(NULL, " \t\n")) != NULL) 
            textcnt = atoi(lp);
      }

      char *sigstr = "";
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         sigstr = lp;

		int i=0;
      for (i=0; i<pieces; i++)
      {
         DataStruct *data = pl_get_pieces(x, y, name, linetype, ""); // yes, we want a name
         if (strlen(sigstr))
         {
            data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING, (void *)sigstr, SA_APPEND, NULL);
         }
      }

      double h;
      for (i=0; i<textcnt; i++)
         pl_get_text(x, y, name, &h);
   }

   return 1;
}

/******************************************************************************
* is_layer_compoutline
*/
static int is_layer_compoutline(const char *l)
{
   for (int i=0; i<compoutlinecnt; i++)
   {
      if (compoutline[i].CompareNoCase(l) == 0)
         return 1;
   }
   return 0;
}

/******************************************************************************
* *pl_get_pieces
*/
DataStruct *pl_get_pieces(double x, double y, const char *name, const char *linetype, const char *shapename)
{
   CCamCadData& camCadData = doc->getCamCadData();

   get_nextline(ifp_line, MAX_LINE, ifp);

   char *lp;
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return NULL;

   if (!STRCMPI(lp, ".REUSE."))  // skip the reuse line and 
   {
      get_nextline(ifp_line, MAX_LINE, ifp);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         return NULL;
   }
   CString type = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return NULL;
   int numcoo = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return NULL;
   double width = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return NULL;   
   int layer = atoi(lp);

   CString restrictions = ""; // only for keepout
   int pinnr = 0;
   if ((lp = strtok(NULL, " \t\n")) != NULL) // keepout restrictions
   {
      if (!STRCMPI(type, "KPTCLS") || !STRCMPI(type, "KPTCIR"))
      {
         restrictions = lp;
         if (!STRCMPI(restrictions, "H"))
            width = 0; // for this the width is the height restriction
      }
      else
         pinnr = atoi(lp)+1;
   }

   if (numcoo == 0)  
   {
      return NULL;
   }
   else if (numcoo > 32000)   
   {
      fprintf(ferr, "Bad number of Coordinates %d at %ld\n", numcoo, ifp_linecnt);
      display_error++;
      return NULL;
   }

   CString layername = "";
   if (!STRICMP(linetype, "BOARD"))
   {
      layername = "BOARD_OUTLINE";
   }
   else if (!STRCMPI(linetype, "DECAL"))
   {
      // if layer is copper, use 1...n layer
      if (!STRNICMP(type, "COP", 3))
      {
         layername.Format("%d", layer);
         if (layer > max_copper_layer)
            max_copper_layer = layer;
      }
      else
      {
         if (layer == 0 || layer == 1) // layer 0 and 1 in decal is the same
            layername = "DECAL_1";
         else
            layername.Format("DECAL_%d", layer);
      }
   }
   else
   {
      if (layer == 0)      // layer 0 is all
         layername = "ALL";
      else
         layername.Format("%d", layer);
   }

   int level = Graph_Level(layername, "", 0);
   int err;
   int widthindex = 0; 
   if (width > 0)
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   int closed = FALSE;
   int filled = FALSE;
   int voidpoly = FALSE;
   if (!type.CompareNoCase("CLOSED") || !type.CompareNoCase("CIRCLE") || !type.CompareNoCase("CIRCUT"))
   {
      closed = TRUE;
   }
   else if (!type.CompareNoCase("COPCIR") || !type.CompareNoCase("COPCLS"))
   {
      closed = TRUE;
      filled = TRUE;
      widthindex = 0;
   }
   else if (!type.CompareNoCase("KPTCLS") || !type.CompareNoCase("KPTCIR"))
   {
      closed = TRUE;
      widthindex = 0;
   }
   else if (!type.CompareNoCase("BRDCLS") || !type.CompareNoCase("BRDCIR"))   // board outline cutout
   {
      closed = TRUE;
      widthindex = 0;
      voidpoly = TRUE;
   }

   if (file_version == 40)  // - Pads reader version 4.0 added new piece types, but we are not supporting these features, yet - Joanne
   {
      if (!type.CompareNoCase("ARWLN1") || !type.CompareNoCase("ARWLN2") || !type.CompareNoCase("ARWHD1") ||
         !type.CompareNoCase("ARWHD2") || !type.CompareNoCase("EXTLN1") || !type.CompareNoCase("EXTLN2") ||
         !type.CompareNoCase("BASPNT"))
      {

         fprintf(ferr, "Doesn't support [%s] Piece type.\n", type);
         display_error++;
      }

   }
    
   if (pinnr)  // copper can be attached to a pin.
   {
      if (!STRCMPI(linetype, "DECAL"))
      {
         PADSPincopper *pinCopper = new PADSPincopper;
         pincopperarray.SetAtGrow(pincoppercnt++, pinCopper);
         pinCopper->pinnr = pinnr;
         pinCopper->layer = layer;

         CString pingraphic;
         pingraphic.Format("%s %d", shapename, pinnr);
         pinCopper->pinblock = Graph_Block_On(GBO_APPEND, pingraphic, file->getFileNumber(), 0);
      }
   }

   DataStruct *data = Graph_PolyStruct(level, 0, 0);
   if (STRCMPI(linetype, "DECAL") == 0 && is_layer_compoutline(layername))
      data->setGraphicClass(GR_CLASS_COMPOUTLINE);
   if (strlen(name))
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NAME, 1), VT_STRING, (void *)name, SA_APPEND, NULL);
   if (!STRICMP(linetype, "BOARD"))
      data->setGraphicClass(GR_CLASS_BOARDOUTLINE);

   // need a function to shrink a polygon by width.
   CPoly *lastpoly = Graph_Poly(NULL, widthindex, filled , voidpoly, closed); // fillflag , negative, closed
   Point2 pointFirst;
   Point2 point;

   for (int i=0; i<numcoo; i++)
   {
      //ifp_linecnt++;     // here get prosa
      get_nextline(ifp_line, MAX_LINE, ifp);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;

      point.x = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;

      point.y = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
      {
         double ab = atof(lp) / 10;
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;

         double aa = atof(lp) / 10;
         if(aa >= 360)
         {
            fprintf(ferr, "PADS Syntax error at %ld\n", ifp_linecnt);
            display_error++;
            aa = 0;           
         }
         point.bulge = tan(DegToRad(aa) / 4);
      }
      else
      {
         point.bulge = 0;
      }

      if (!type.CompareNoCase("CIRCLE") || !type.CompareNoCase("CIRCUT") || !type.CompareNoCase("COPCIR") ||
         !type.CompareNoCase("KPTCIR") || !type.CompareNoCase("BRDCIR"))
         point.bulge = 1;

      // there can be an error in PADS to have the arc start as the last element.
      if (i == numcoo-1 && !closed)
         point.bulge = 0;
      
      Graph_Vertex(point.x + x, point.y + y, point.bulge);
      if (i==0)
         pointFirst = point;
   }

   if (closed)
   {
      if (!type.CompareNoCase("COPCIR") || !type.CompareNoCase("CIRCUT") || !type.CompareNoCase("CIRCLE") ||
         !type.CompareNoCase("KPTCIR") || !type.CompareNoCase("BRDCIR"))
         Graph_Vertex(pointFirst.x + x, pointFirst.y + y, 0.0);
      else if (point.x != pointFirst.x || point.y != pointFirst.y)
         Graph_Vertex(pointFirst.x + x, pointFirst.y + y, 0.0);
   }
   else
   {
      // check if it was OPEN but should be closed
      if (point.x == pointFirst.x && point.y == pointFirst.y)
         lastpoly->setClosed(true);
   }

   // here multiply keepout 
   if (!STRICMP(linetype, "KEEPOUT"))
   {
      if (strlen(restrictions) == 0) // generic
      {
         data->setGraphicClass(GR_CLASS_ALLKEEPOUT);
      }
      else
      {
         int first = TRUE;
         for (int r=0; r<(int)strlen(restrictions); r++)
         {
            int gclass = 0;
            switch (restrictions[r])
            {
            case 'P':   // = placement       
               gclass = GR_CLASS_PLACEKEEPOUT;
               break;
            case 'H':   // = componment height  
               gclass = GR_CLASS_PLACEKEEPOUT;
               break;
            case 'R':   // = trace              
               gclass = GR_CLASS_ROUTKEEPOUT;
               break;
            case 'C':   // = copper and plane  
               gclass = GR_CLASS_ROUTKEEPOUT;
               break;
            case 'V':   // = via             
               gclass = GR_CLASS_VIAKEEPOUT;
               break;
            case 'T':   // = test point         
               gclass = graphicClassTestPointKeepOut;
               break;
            default: 
               fprintf(ferr, "Unknown restriction [%c] in KEEPOUT at %ld\n", restrictions[r], ifp_linecnt);
               display_error++;
               continue; // unknown
               break;
            }        

            if (first)
            {
               data->setGraphicClass(gclass);
            }
            else
            {
               DataStruct *newData = camCadData.getNewDataStruct(*data);
               newData->setGraphicClass(gclass);
               AddEntity(newData);
            }
            first = FALSE;
         }
      }
   }

   if (pinnr)
   {
      if (!STRCMPI(linetype, "DECAL"))
         Graph_Block_Off();
   }

   return data;
}

/******************************************************************************
* get_terminal
*/
static int get_terminal(const char *name)
{
   get_nextline(ifp_line, MAX_LINE, ifp);

   char *lp;
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   if (lp[0] != 'T')                            
      return 0;
   lp[0] = ' ';

   PADSTerminal terminal;
   terminal.x = cnv_unit(atof(lp));
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   terminal.y = cnv_unit(atof(lp));
   terminalarray.SetAtGrow(terminalcnt++, terminal);  
      
   return 1;
}

/******************************************************************************
* get_padstackindex
*/
static int get_padstackindex(double drillsize ,int *pad_stackarray)
{
   PADS_Paddef paddef;
	int i=0; 
   for (i=0; i<paddefcnt; i++)
   {
      int found = TRUE;
      paddef = paddefarray.ElementAt(i);
		// Case 1660, This file not actually involved in case 1660, but this code
		// was copied from there once, and it was wrong there, so presumably it
		// was wrong here too.
		// Logic in comparision was backwards. Should be ">", not "<".
		// otherwise it will find pads with drills that are NOT close, instead of
		// those that ARE close.
      if (fabs(paddef.drillsize - drillsize) < 0.001)
         found = FALSE;

      for (int k=0; k<MAX_PADSTACK_SIZE; k++)
      {
         if (paddef.padstack[k] != pad_stackarray[k])
            found = FALSE;
      }
      
      if (found)
         return i;
   }

   paddef.drillsize = drillsize;
   paddef.typ = 0;
   for (i=0; i<MAX_PADSTACK_SIZE; i++)
   {
      paddef.padstack[i] = pad_stackarray[i];
      if (i == 0 && pad_stackarray[i] > -1)  // top
         paddef.typ |= 0x1;
      if (i == 1 && pad_stackarray[i] > -1)  // inner
         paddef.typ |= 0x3;
      if (i == 2 && pad_stackarray[i] > -1)  // bottom
         paddef.typ |= 0x2;
   }

   if (paddef.drillsize > 0)
      paddef.typ |= 0x4;

   paddef.padstackName = "";
   paddefarray.SetAtGrow(paddefcnt++, paddef);  
   write_one_padstack(paddefcnt - 1);

   return paddefcnt -1;
}

/******************************************************************************
* get_stack
*/
static int get_stack(const char *name)
{
   //ifp_linecnt++;     // here get prosa
   get_nextline(ifp_line, MAX_LINE, ifp);
   
   char *lp;
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return -1;  // PAD

   // if this is not the PAD keyword -- error
   if (STRCMPI(lp, "PAD"))
      return -1;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return -1;
   int pinnumber = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return -1;
   int padcnt = atoi(lp);

   BOOL plated = FALSE;
   double drill = 0, drlrotate = 0, drllen = 0, drloff = 0;


   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return -1;
   if (lp[0] == 'Y')
      plated = TRUE;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return -1;
   drill = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) != NULL)
      drlrotate = DegToRad(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) != NULL)
      drllen = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) != NULL)
      drloff = cnv_unit(atof(lp));

   int pad_stack[MAX_PADSTACK_SIZE]; // -2 .. 30
	int i=0;
   for (i=0; i<MAX_PADSTACK_SIZE; i++)
      pad_stack[i] = -1;

   for (i=0; i<padcnt; i++)
   {
      double sizeA = 0, sizeB = 0, offset = 0;
      int rotation = 0;

      //ifp_linecnt++;     // here get prosa
      get_nextline(ifp_line, MAX_LINE, ifp);
      // here make a padstack
      lp = strtok(ifp_line, " \t\n");
      int layerNum = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
      {
         fprintf(ferr, "Syntax Error: Size expected at %ld\n", ifp_linecnt);           
         display_error++;
         return -1;
      }

      sizeA = cnv_unit(atof(lp));
      if ((lp = strtok(NULL, " \t\n")) == NULL)
      {
         fprintf(ferr, "Syntax Error: Padform expected at %ld\n", ifp_linecnt);           
         display_error++;
         return -1;
      }

      CString formName = lp;
      int form;
      if (!strcmp(lp, "R"))   // round
      {
         // nothing
         form = T_ROUND;
      }
      else if (!strcmp(lp, "S")) // square
      {
         // nothing
         form = T_SQUARE;
      }
      else if (!strcmp(lp, "A")) // annular
      {
         form = T_DONUT;
         lp = strtok(NULL, " \t\n");
         sizeB = cnv_unit(atof(lp));
      }
      else if (!strcmp(lp, "OF"))   // oval finger
      {
         form = T_OBLONG;
         lp = strtok(NULL, " \t\n");
         rotation = atoi(lp);
         lp = strtok(NULL, " \t\n");
         sizeB = sizeA;
         sizeA = cnv_unit(atof(lp));
         lp = strtok(NULL, " \t\n");
         offset = cnv_unit(atof(lp));
      }
      else if (!strcmp(lp, "RF"))   // rectangle finger
      {
         form = T_RECTANGLE;
         lp = strtok(NULL, " \t\n");
         rotation = atoi(lp);
         lp = strtok(NULL, " \t\n");
         sizeB = sizeA;
         sizeA = cnv_unit(atof(lp));
         lp = strtok(NULL, " \t\n");
         offset = cnv_unit(atof(lp));
      }
      else if (!strcmp(lp, "RT"))   // round thermal
      {
         form = T_ROUND;
      }
      else if (!strcmp(lp, "RA"))   // round Antipad
      {
         form = T_ROUND;
      }
      else if (!strcmp(lp, "ST"))   // square thermal
      {
         form = T_SQUARE;
/*
         lp = strtok(NULL," \t\n");
         rotation = atoi(lp);
         lp = strtok(NULL," \t\n");
         sizeB = sizeA;
         sizeA = cnv_unit(atof(lp));
         lp = strtok(NULL," \t\n");
         offset = cnv_unit(atof(lp));
*/
      }
      else if (!strcmp(lp, "SA"))   // Square Antipad
      {
         form = T_SQUARE;
      }
      else
      {
         fprintf(ferr, "Unknown DECAL padform [%s] at %ld -> skipped\n", lp, ifp_linecnt); 
         continue;
      }

      if (sizeA == 0)
         continue;   // dummy stack

      // here get padfrom index
      BlockStruct *block = Graph_FindAperture(form, sizeA, sizeB, offset, 0, DegToRad(rotation), 0, TRUE, FALSE);
      pad_stack[layerNum + 2] = block->getBlockNumber();
   }

   // here now check Paddef index of already defined
   int psindex = get_padstackindex(drill, &pad_stack[0]);

   // here update terminal array with padstacks
   if (pinnumber == 0)
   {
      for (i=0; i<terminalcnt; i++)
      {
         PADSTerminal terminal = terminalarray.ElementAt(i);
         terminal.padstackindex = psindex;
         terminalarray.SetAtGrow(i, terminal);     
      }
   }
   else
   {
      PADSTerminal terminal = terminalarray.ElementAt(pinnumber-1);
      terminal.padstackindex = psindex;
      terminalarray.SetAtGrow(pinnumber - 1, terminal);     
   }

   return 1;
}

/******************************************************************************
* is_pad_layertype
*/
static int is_pad_layertype(const char *lname, int ltype)
{
   for (int i=0; i<layer_attr_cnt; i++)
   {
      if (!STRCMPI(layer_attr[layer_attr_cnt].layerName, lname))
      {
         return (layer_attr[layer_attr_cnt].layerType == ltype);
      }
   }
   return 0;
}

/******************************************************************************
* pl_get_text
*/
static int pl_get_text(double x, double y, const char *name, double *h)
{
   *h = 0.0;
   get_nextline(ifp_line, MAX_LINE, ifp);

   char *lp;
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   double x1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double y1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rot = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   if (!STRCMPI(lp, "0"))     // layer 0 is all
      lp = "ALL";
   int level = Graph_Level(lp, "", 0);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double height = cnv_unit(atof(lp)) * TEXT_HEIGHTFACTOR;
   double width = height * TEXT_WIDTHFACTOR;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double penwidth = cnv_unit(atof(lp));

   int mirror = FALSE;
   if ((lp = strtok(NULL, " \t\n")) != NULL)
   {
      if (lp[0] == 'M')
         mirror = TRUE;
   }

   ifp_linecnt++;    // here get prosa
   fgets(ifp_line, MAX_LINE, ifp);

   // may be an empty text line
   if ((lp = strtok(ifp_line, "\n")) == NULL)
      return 1;
   
   if (strlen(lp) == 0)
      return 1;

   if (!STRNICMP(lp, "ZHEIGHT", 7))
      *h = atof(&lp[8]);
   else
      Graph_Text(level, lp, x1 + x, y1 + y, height, width, DegToRad(rot), 0, TRUE, mirror, 0, FALSE, -1, 0);

   return 1;
}

/******************************************************************************
* pl_get_label
*     VISIBLE_LX_LY_LORI_LHEIGTH_LWIDTH_LLEVEL_MIRRORED_HJUST_VJUST_[RIGHT_READING]
*/
static int pl_get_label(CAttributes** map, double x, double y, const char *name)
{
   char *lp;
   char tmp[80];  // temp label name

   get_nextline(ifp_line, MAX_LINE, ifp);
   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   CString visible = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double x1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double y1 = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rot = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   if (!STRCMPI(lp,"0"))
      lp = "ALL";
   else
   {
      int lev = atoi(lp);
      sprintf(tmp, "DECAL_%d", lev);
      lp = tmp;
   }
   int level = Graph_Level(lp, "", 0);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double height = cnv_unit(atof(lp)) * TEXT_HEIGHTFACTOR;
   double width = height * TEXT_WIDTHFACTOR;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double penwidth = cnv_unit(atof(lp));

   int mirror = FALSE;
   if ((lp = strtok(NULL, " \t\n")) != NULL) 
   {
      if (lp[0] == 'M')
         mirror = TRUE;
   }

   ifp_linecnt++;
   fgets(ifp_line, MAX_LINE, ifp);

   // may be an empty text line
   if ((lp = strtok(ifp_line,"\n")) == NULL)
      return 1;
   if (strlen(lp) == 0)
      return 1;
   CString key = get_attribmap(lp);

   if (!STRCMPI(lp, "Part Type"))
   {
      // do not overwrite the DEVICE attribute.
   }
   else if (!key.CompareNoCase(ATT_TYPELISTLINK))
   {
      // do not overwrite the DEVICE attribute.
   }
   else if (!key.CompareNoCase(ATT_REFNAME))
      doc->SetUnknownVisAttrib(map, key, name, x1 + x, y1 + y, DegToRad(rot), height, width, 0, 0, TRUE, SA_OVERWRITE, 0L, level, 0, 0, 0);
   else
      doc->SetUnknownVisAttrib(map, key, "", x1 + x, y1 + y, DegToRad(rot), height, width, 0, 0, TRUE, SA_OVERWRITE, 0L, level, 0, 0, 0);

   return 1;
}

/******************************************************************************
* int in_pincopper
*/
static int in_pincopper(int pinnr)
{
   for (int i=0; i<pincoppercnt; i++)
   {
      if (pincopperarray[i]->pinnr == pinnr)
         return i;
   }
   return -1;
}

/******************************************************************************
* attach_pincopper
*/
static int attach_pincopper(int pinnr, DataStruct *pininstance)
{
   int pinptr = in_pincopper(pinnr);

   if (pinptr > -1)
   {
#ifdef _DEBUG
   PADSPincopper *p = pincopperarray[pinptr];
#endif

      BlockStruct *newpinshape = Graph_Block_On(GBO_APPEND, "~", file->getFileNumber(), 0);
      newpinshape->setBlockType(BLOCKTYPE_PADSTACK);

      // pininstance is the datastruct of the pin.
      // this has a padstack attached.
   
      // the pincopperarray->pinblock needs to be normalize to the pin koos
      POSITION insertPos = pincopperarray[pinptr]->pinblock->getDataList().GetHeadPosition();
      while (insertPos)
      {
         DataStruct *subData = pincopperarray[pinptr]->pinblock->getDataList().GetNext(insertPos);
         DataStruct *newData = CopyTransposeEntity(subData, -pininstance->getInsert()->getOriginX(), -pininstance->getInsert()->getOriginY(), 
               0.0, 0, 1.0, -1, TRUE);
         newpinshape->getDataList().AddTail(newData);
      }

      // now copy the existing padstack to the newpinshape
      BlockStruct *oldpin = doc->Find_Block_by_Num(pininstance->getInsert()->getBlockNumber());
      insertPos = oldpin->getDataList().GetHeadPosition();
      while (insertPos)
      {
         DataStruct *subData = oldpin->getDataList().GetNext(insertPos);
         DataStruct *newData = CopyTransposeEntity(subData, 0.0, 0.0, 0.0, 0, 1.0, -1, TRUE);
         newpinshape->getDataList().AddTail(newData);
      }

      // the current padname has to be added to the pincopper block
      Graph_Block_Off();

      CString name = pincopperarray[pinptr]->pinblock->getName();
      // remove old pincopperblock
      doc->RemoveBlock(pincopperarray[pinptr]->pinblock);

      // the pincopper block needs to be added to pininstance.
      pininstance->getInsert()->setBlockNumber(newpinshape->getBlockNumber());
      newpinshape->setName(name);
   }

   return 1;
}

/******************************************************************************
* pads_Lib_decal
*/
static int pads_Lib_decal()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      if (!STRICMP(lp, "*END*"))
         break;
      CString name = lp;
      
      pincoppercnt = 0;

      // check that name does not exist
      int exist_cnt = 0;
      while (TRUE)
      {
         if (Graph_Block_Exists(doc, name, file->getFileNumber()))
         {
            fprintf(ferr, "DECAL [%s] already defined\n", name);           
            display_error++;
            CString  tmp;
            tmp.Format("%s_%d", name, ++exist_cnt);
            name = tmp;
         }
         else
            break;
      }

      BlockStruct *block = Graph_Block_On(GBO_APPEND, name, file->getFileNumber(), 0);
      block->setBlockType(BLOCKTYPE_PCBCOMPONENT);

      // pads V5 has no decal units
      if (file_version == 4)
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         CString unit = lp;
         if (lp[0] == 'I')
            unit_flag = 'A';
           else if (lp[0] == 'M')
            unit_flag = 'M';
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      int attrscnt = 0;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      attrscnt = atoi(lp);

      int labelcnt = 0;
      if ((lp = strtok(NULL, " \t\n")) == NULL) 
         continue;
      labelcnt = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int pieces = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int textcnt = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int termcnt = atoi(lp);    
      if (!termcnt)  // has not pins
         block->setBlockType(0); // this is not a PCB component.

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int stackcnt = atoi(lp);

      if (attrscnt < 0)
      {
         fprintf(ferr, "Bad number of Drawing Pieces %d at %ld\n", pieces, ifp_linecnt);
         display_error++;
         continue;
      }
      if (pieces < 0)
      {
         fprintf(ferr, "Bad number of Drawing Pieces %d at %ld\n", pieces, ifp_linecnt);
         display_error++;
         continue;
      }
      if (textcnt < 0)
      {
         fprintf(ferr, "Bad number of Text Pieces %d at %ld\n", textcnt, ifp_linecnt);
         display_error++;
         continue;
      }
      if (labelcnt < 0)
      {
         fprintf(ferr, "Bad number of Label Pieces %d at %ld\n", labelcnt, ifp_linecnt);
         display_error++;
         continue;
      }
      if (termcnt < 0)
      {
         fprintf(ferr, "Bad number of Drawing Pieces %d at %ld\n", pieces, ifp_linecnt);
         display_error++;
         continue;
      }
      if (stackcnt < 0)
      {
         fprintf(ferr, "Bad number of Drawing Pieces %d at %ld\n", pieces, ifp_linecnt);
         display_error++;
         continue;
      }

      // loop through contents
		int i=0;
      for (i=0; i<attrscnt; i++)
         get_nextline(ifp_line, MAX_LINE, ifp); // get_attrs(name);

      for (i=0; i<labelcnt; i++)
         pl_get_label(&block->getAttributesRef(), 0, 0, "DECAL"); // no name needed

      for (i=0; i<pieces; i++)
         pl_get_pieces(0,0, "", "DECAL", name); // no piecename needed, but shapename is needed

      double height;
      for (i=0; i<textcnt; i++)
      {
         pl_get_text(0, 0, name, &height);
         if (height > 0)
         {
            double h = cnv_unit(height);
            doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL); 
         }

      }

      terminalcnt = 0;
      for (i=0; i<termcnt; i++)
         get_terminal(name);

      for (i=0; i<stackcnt; i++)
      {
         if (get_stack(name) < 0)            
         {
            CString tmp;
            tmp.Format("Big Syntax error at line %ld -> Padstack records expected!", ifp_linecnt);
            fprintf(ferr, "%s\n", tmp);
            display_error++;
            tmp += "\nNeed to generate a new PADS ASCII file.";
            ErrorMessage(tmp, "Invalid PADS ASCII file!");
            return -1;
         }  
      }

      // here write all terminals
      for (i=0; i<terminalcnt; i++)
      {
         // here place pin
         CString pinnr;
         pinnr.Format("%d", i+1);
         PADSTerminal terminal = terminalarray.ElementAt(i);

         // i = ignore
         // f = fiducial
         // n = normal
         // non_electrical_pins
         PADS_Paddef paddef = paddefarray.ElementAt(terminal.padstackindex);

         // also now need to watch out that it does not appear in the netlist.
         DataStruct *data = Graph_Block_Reference(paddef.padstackName, pinnr, file->getFileNumber(), terminal.x, terminal.y, DegToRad(0.0), 0 , 1.0, -1, TRUE);
         if (non_electrical_pins == 'f' && (paddef.typ & 3) == 0)
            data->getInsert()->setInsertType(insertTypeFiducial);
         else
            data->getInsert()->setInsertType(insertTypePin);

         attach_pincopper(atoi(pinnr), data);

         // do pinnr here
         if (strlen(pinnr))
         {
            int pnr = atoi(pinnr);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, TRUE), VT_INTEGER, &pnr, SA_OVERWRITE, NULL);
         }
      }

      for (i=0; i<pincoppercnt; i++)
         delete pincopperarray[i];

      Graph_Block_Off();
   }

   return 1;
}

/******************************************************************************
* get_ptypeptr
*/
static int get_ptypeptr(const char *n)
{
   for (int i=0; i<partcnt; i++)
   {
#ifdef _DEBUG
PADSPart *cc = partarray[i];
#endif
      if (!strcmp(partarray[i]->ptypename,n))
         return i;
   }
   return -1;
}

/******************************************************************************
* update_alphapin
*/
static int update_alphapin(BlockStruct *block, int pinnr, const char *pinname)
{
   int found = 0;

   DataStruct *data = get_pindata_from_comppinnr(doc, block, pinnr);
   if (data)
   {
      data->getInsert()->setRefname(STRDUP(pinname));
   }
   else
   {
      PADSPadsIgnored *padIgnored = new PADSPadsIgnored;
      padsignoredarray.SetAtGrow(padsignoredcnt++, padIgnored);
      padIgnored->decalname = block->getName();
      padIgnored->pinnr = pinnr;
      padIgnored->pinname = pinname;
   }

   return found;
}

/******************************************************************************
* int update_comppin_ignore
   here I make a list of components and ignored pins, so that the later 
   netlist check is fast.
*/
static int update_comppin_ignore(const char *compName, const char *decal)
{
   if (padsignoredcnt == 0)
      return 0;

   // c = compname
   // s = shape
   DataStruct *data = datastruct_from_refdes(doc, file->getBlock(), compName);
   if (data == NULL)
      return 0;

   BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
   if (block == NULL)
      return 0;

   for (int i=0; i<padsignoredcnt; i++)   // this has all decals and pinne
   {
      PADSPadsIgnored *padIgnored = padsignoredarray[i];
      if (padIgnored->decalname.Compare(block->getName()) == 0)
      {
         // add to the comppinignoredcnt
         PADSCompPinIgnored *compPinIgnored = new PADSCompPinIgnored;
         comppinignoredarray.SetAtGrow(comppinignoredcnt++, compPinIgnored);  
         compPinIgnored->compname = compName;
         compPinIgnored->geomname = block->getName();
         compPinIgnored->pinnr = padIgnored->pinnr;
         compPinIgnored->pinname = padIgnored->pinname;
         compPinIgnored->already_reported = 0;
      }
   }

   return 1;
}

/******************************************************************************
* set_parttype_altused
*  If only 1 alt is used per type, I can optimize this so that it does not get renamed.
*  I can use the original type and delete the other ones.
*
*/
static int set_parttype_altused(const char *ptype, int alt)
{
   int p = get_ptypeptr(ptype);
 
   return 1;
}

/******************************************************************************
* pads_Lib_part
*/
static int pads_Lib_part()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      CString name = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString ptype = lp;  // if TOOL_126@TOOL_126A, than tool_126A is the shape and do not use the type
      CString origptype = lp;

      CString value = "";
      CString tolerance = "";

      int found = ptype.Find(",");
      if (found > -1)
      {
         CString typeLeft = ptype.Left(found);
         value = ptype.Right(strlen(ptype) - (found + 1));

         // here seperate tolerance is string end with a %
         int found1 = value.Find(",");
         if (found1 > -1)
         {
            CString valueLeft = value.Left(found1);
            tolerance = value.Right(strlen(value) - (found1 + 1));
            value = valueLeft;
         }

         found1 = value.Find("@");
         if (found1 > -1)
         {
            CString valueLeft, valueRight;
            valueLeft = value.Left(found1);
            valueRight = value.Right(strlen(value)-(found1+1));
            value = valueLeft;
            typeLeft += "@";
            typeLeft += valueRight;
         }
         ptype = typeLeft;
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double x = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double y = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double rot = atof(lp);

      int mir = 0;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      mir = (lp[0] == 'M');

      double nmx;
      double nmy;
      double nmrot;
      if (file_version < 30)
      {
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         nmx = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         nmy = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         nmrot = atof(lp);
      }
      else
      {
         nmx = 0;
         nmy = 0;
         nmrot = 0;
      }

      int alt = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         alt = atoi(lp);

      int CLSTID = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         CLSTID = atoi(lp);

      int CLSTATTR = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         CLSTATTR = atoi(lp);

      int BROTHERID = 0;   
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         BROTHERID = atoi(lp);

      int labelcnt = 0;
      if ((lp = strtok(NULL, " \t\n")) != NULL) 
         labelcnt = atoi(lp);

      if (mir)
         rot = 360 - rot;
      rot = clean_rot(rot);   

      DataStruct *data;
      int pointer;
      CString original_decal  = ""; // which original DECAL was used.
      char tmp[255];
      strcpy(tmp, origptype);
      if (lp = strchr(tmp, '@'))
      {
         CString shape;
         CString typ;
         strcpy(tmp, origptype);
         if (lp = strtok(tmp, "@"))    // seperate type
            typ = lp;
         if (lp = strtok(NULL, "\n"))  // and shape
            shape = lp;
         original_decal = shape;

         ptype = typ;
         origptype = typ;
         pointer = get_ptypeptr(origptype);
         if (pointer > -1)
         {
            CString newdecal;    
            newdecal.Format("%s_%s", typ,shape);

            BlockStruct *block = Graph_Block_Exists(doc, newdecal, file->getFileNumber());

            // only needs to create a new shape if there are alpha pins
            if (pointer > -1 && partarray[pointer]->alphapins == 0)
            {
               newdecal = shape;
            }
            else if (block == NULL) // this means a type@decal combination is not 
                                    // yet defined. a decal will get the pinnames from the type.
            {
               BlockStruct *block1 = Graph_Block_Exists(doc, shape, file->getFileNumber());
               if (block1 == NULL)
               {
                  CString tmp;
                  tmp.Format("PartDecal [%s] is used in Part [%s], but is not defined in DECAL.", shape, origptype);
                  ErrorMessage(tmp, "PADS read Error");
                  fprintf(ferr, "%s\n", tmp);
                  display_error++;
                  continue;
               }

               BlockStruct *block2 = Graph_Block_On(GBO_APPEND, newdecal, file->getFileNumber(), 0);
               block2->setBlockType(BLOCKTYPE_PCBCOMPONENT);
               Graph_Block_Copy(block1, 0, 0, 0, 0, 1, -1, TRUE);
               Graph_Block_Off();
               doc->SetUnknownAttrib(&block2->getAttributesRef(), ATT_DERIVED, shape, SA_OVERWRITE, NULL);

               // update new decal
               int pinnr = 0;
               char *tmp = STRDUP(partarray[pointer]->pinname);
               char *tmp1 = strtok(tmp,",");
               while (tmp1)
               {
                  pinnr++;
                  update_alphapin(block2, pinnr, tmp1);
                  tmp1 = strtok(NULL,",");
               }
               free(tmp);

               // update the type with the new shape
               CString typname;  
               typname.Format("%s_%s", origptype, newdecal);
               TypeStruct *type = AddType(file,typname);
               type->setBlockNumber( block2->getBlockNumber()); // b->num; pads can have multiple Decals assigned to a type.
            }
   
            data = Graph_Block_Reference(newdecal, name , file->getFileNumber(), x, y, DegToRad(rot), mir , 1.0, -1, TRUE);
            data->getInsert()->setInsertType(insertTypePcbComponent);

            // check which shape this parttype has. - if it has a different on, make a new type
            // first check if and alt fits, otherwise add a new one.
            int found = FALSE;
            for (int i=0; i<partarray[pointer]->decalcnt; i++)
            {
               if (newdecal.CompareNoCase(partarray[pointer]->decalname[i]) == 0)
               {
                  found = TRUE;
                  alt = i;
               }
            }

            if (!found)
            {
               if (partarray[pointer]->decalcnt < MAX_ALTDECAL)
               {
                  partarray[pointer]->decalname[partarray[pointer]->decalcnt] = newdecal;
                  partarray[pointer]->originaldecalname[partarray[pointer]->decalcnt] = shape;
                  alt = partarray[pointer]->decalcnt++;
               
                  CString typname;
                  typname.Format("%s_%s", origptype, newdecal);
                  TypeStruct *type = AddType(file,typname);
                  doc->SetUnknownAttrib(&type->getAttributesRef(), ATT_DERIVED, origptype, SA_OVERWRITE, NULL); //  

                  BlockStruct *block = Graph_Block_Exists(doc, newdecal, -1);
                  if (block)
                     type->setBlockNumber( block->getBlockNumber());
               }
               else
               {
                  fprintf(ferr, "Too many alternate shapes in type@shape [%s@%s] section\n", typ, newdecal);
                  origptype = "";
               }
            }
         }
         else
         {
            fprintf(ferr,"Component Decal [%s] not found at %ld\n",origptype, ifp_linecnt);
            display_error++;
         }
      }
      else
      {
         pointer = get_ptypeptr(origptype);

         if (pointer > -1)
         {
            original_decal = partarray[pointer]->originaldecalname[alt];
            data = Graph_Block_Reference(partarray[pointer]->decalname[alt], name , file->getFileNumber(), x, y,
                  DegToRad(rot), mir , 1.0, -1, TRUE);
            data->getInsert()->setInsertType(insertTypePcbComponent);
         }
         else
         {
            fprintf(ferr, "Component Decal [%s] not found at %ld\n", origptype, ifp_linecnt);
            display_error++;
         }
      }  // if ptye is type@shape

      if (pointer == -1)
         continue;

      update_comppin_ignore(name, original_decal);

      if (partarray[pointer]->fiducial)
         data->getInsert()->setInsertType(insertTypeFiducial);

      if (partarray[pointer]->smdflag)
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);

      if (partarray[pointer]->compheight > 0)
      {
         double compHeight = partarray[pointer]->compheight;
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &compHeight, SA_OVERWRITE, NULL);
      }

      if (file_version < 30)
      {
         int laynr = Graph_Level("DECAL_1", "", 0);
         doc->SetVisAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, name.GetBuffer(0), nmx ,nmy, 
               DegToRad(nmrot), refnamesize.height, refnamesize.height * .8, 1, 0, TRUE, SA_OVERWRITE, 0L, laynr, 0, 0, 0);
      }

      if (strlen(origptype))
      {
         set_parttype_altused(origptype, alt);
         if (partarray[pointer]->decalcnt == 1)
         {
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TYPELISTLINK, TRUE), VT_STRING,
                  origptype.GetBuffer(0),SA_OVERWRITE, NULL);
         }
         else
         {
            CString typname;
            typname.Format("%s_%s", origptype, partarray[pointer]->decalname[alt]);

            TypeStruct *type = FindType(file, typname);
            if (type == NULL) // a type could not exist if it was modified in the component list
            {
               type = AddType(file, typname);
               doc->SetUnknownAttrib(&type->getAttributesRef(), ATT_DERIVED, origptype, SA_OVERWRITE, NULL);  

               BlockStruct *block = Graph_Block_Exists(doc, original_decal, -1);
               if (block)
                  type->setBlockNumber( block->getBlockNumber());

               TypeStruct *origt = AddType(file,origptype);
               doc->CopyAttribs(&type->getAttributesRef(), origt->getAttributesRef());
            }
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TYPELISTLINK, TRUE),
                  VT_STRING, typname.GetBuffer(0),SA_OVERWRITE, NULL);
         }
      }

      if (strlen(value))
      {
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_VALUE,0), VT_STRING,   value.GetBuffer(0),SA_OVERWRITE, NULL);
      }
      if (strlen(tolerance))
      {
         double ptol;
         double mtol;
         double tol;
         CString  t;

         check_tolerance(tolerance, &mtol, &ptol, &tol);

         if (tol > -1)
         {
            double tt = tol;
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL);
         }
         if (mtol > -1)
         {
            double tt = -mtol;
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE), VT_DOUBLE, &tt, SA_OVERWRITE, NULL);
         }           
         if (ptol > -1)
         {
            double tt = ptol;
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE), VT_DOUBLE,&tt,SA_OVERWRITE, NULL);
         }
      }

      for (int i=0; i<labelcnt; i++)
         pl_get_label(&data->getAttributesRef(), 0, 0, name);  // no name needed

   }

   return 1;
}

/******************************************************************************
* get_gates
*/
static int get_gates()
{
   char  string[MAX_LINE];
   char  *lp;
   get_nextline(string, MAX_LINE, ifp);

   if ((lp = strtok(string, " \t\n")) == NULL)  // must be G or S
   {
      fprintf(ferr, "Syntax Error at %ld\n", ifp_linecnt);
      display_error++;
      return 0;
   }
      
   lp = strtok(NULL, " \t\n");
   lp = strtok(NULL, " \t\n");
   int gatecnt = atoi(lp);

   lp = strtok(NULL, "\n");   // just to make sure that this is the end of the line
   for (int i=0; i<gatecnt; i++)
   {
      if ((lp = strtok(NULL, " \t\n")) == NULL)
      {
         get_nextline(string, MAX_LINE, ifp);
         lp = strtok(string, " \t\n"); 
      }
   }
   return 1;
}

/******************************************************************************
* update_parttypepin
*/
static int update_parttypepin(const char *parttypename, int pinnr, const char *pinname)
{
   int pointer = get_ptypeptr(parttypename);
   if (pointer < 0)
      return 0;

   int found = 0;
   for (int i=0; i<MAX_ALTDECAL; i++)
   {
      if (strlen(partarray[pointer]->decalname[i]) == 0)
         continue;
      found = 0;

      CString newdecal = partarray[pointer]->decalname[i];
      BlockStruct *block = Graph_Block_Exists(doc, newdecal, -1);
      if (block)
         update_alphapin(block, pinnr, pinname);
   }

   return 1;
}

/******************************************************************************
* get_alpha
*  here look for pinnr and update pinname
*/
static int get_alpha(int cnt, const char *parttypename, int partptr)
{
   if (cnt == 0)
      return 0;

   char string[MAX_LINE];
   CString tmp;
   char *lp;

   do 
   {
      ifp_linecnt++;    // here get prosa
      fgets(string, MAX_LINE, ifp);
      tmp = string;
      tmp.TrimLeft();
      tmp.TrimRight();
      strcpy(string, tmp);
   } while (strlen(tmp) == 0);   // empty line

   lp = strtok(string, " \t\n"); 
   update_parttypepin(parttypename, 1, lp);
   partarray[partptr]->pinname = lp;

   for (int i=1; i<cnt; i++)
   {
      if ((lp = strtok(NULL, " \t\n")) == NULL)
      {
         do
         {
            ifp_linecnt++;    // here get prosa
            fgets(string, MAX_LINE, ifp);
            tmp = string;
            tmp.TrimLeft();
            tmp.TrimRight();
            strcpy(string, tmp);
         } while (strlen(tmp) == 0);   // empty line

         lp = strtok(string, " \t\n"); 
      }
      update_parttypepin(parttypename, i+1, lp);
      partarray[partptr]->pinname += ",";
      partarray[partptr]->pinname += lp;
   }
   return 1;
}

/******************************************************************************
* get_sig
*/
static int get_sig()
{
   char  string[MAX_LINE];

   ifp_linecnt++;    
   fgets(string, MAX_LINE, ifp);
   return 1;
}

/******************************************************************************
* pads_Lib_type
*
*  This is allowed in PADS. Therefore, we need to keep the value with the type
*
*  PCAPALT,22UF_50V PCAPALT I UND  0   0   0   2     0
*  A B 
*
*  PCAPALT,47UF_16V PCAPALT I UND  0   0   0   2     0
*  A B 
*
*  PCAPALT,4.7UF_50V PCAPALT I UND  0   0   0   2     0
*  A B 
*/
static int pads_Lib_type()
{
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      int altdecalcnt = 0;
      int gatecnt = 0;
      int sigcnt = 0;
      int attcnt = 0;
      int alphacnt = 0;
      int flg = 0;

      char tmp[MAX_LINE];
      char *lp;
      strcpy(tmp, ifp_line);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;

      int count;
      int attfound = FALSE;
      if (file_version < 30)
      {
         // if the line has more than 8 entries, there is an attribute Flag
         count = 1;  // already got one
         attfound = FALSE;
         while ((lp = strtok(NULL, " \t\n")) != NULL)
            count++;
         if (count > 8)
            attfound = TRUE;
      }

      CString value = "";
      CString tolerance = "";

      if ((lp = strtok(tmp, " \t\n")) == NULL)
         continue;
      CString nname = lp;
      CString origname = lp;

      PADSPart *part = new PADSPart;
      partarray.SetAtGrow(partcnt++, part);  
      part->altused = 0;
      part->ptypename = origname;
      part->pinname = "";
      part->smdflag = 0;
      part->fiducial = 0;
      part->alphapins = 0;
      part->compheight = 0;

      // here seperate value from name
      int found = nname.Find(",");
      if (found > -1)
      {
         CString nnameLeft = nname.Left(found);
         value = nname.Right(strlen(nname) - (found + 1));

         int found1 = value.Find(",");
         if (found1 > -1)
         {
            CString valueLeft = value.Left(found1);
            tolerance = value.Right(strlen(value) - (found1 + 1));
            value = valueLeft;
         }
         nname = nnameLeft;
      }

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString decalname = lp;

      if ((lp = strtok(NULL, "\n")) == NULL)
         continue;
      strcpy(tmp, lp);

      for (int decalCnt=0; decalCnt<MAX_ALTDECAL; decalCnt++)
         part->decalname[decalCnt] = "";

      TypeStruct *type = AddType(file, origname);
      if ((lp = strtok(decalname.GetBuffer(0), ":")))
      {
         part->decalname[0] = lp;
         part->originaldecalname[0] = lp;
         altdecalcnt++;
      }

      while ((lp = strtok(NULL, ":")))
      {
         if (altdecalcnt < MAX_ALTDECAL)
         {
            part->decalname[altdecalcnt] = lp;
            part->originaldecalname[altdecalcnt] = lp;
            altdecalcnt++;
         }
         else
         {
            CString tmp;
            tmp.Format("Type [%s]\n", origname);
            ErrorMessage("Too many Alternate Decal", tmp);
         }
      }
      part->decalcnt = altdecalcnt;

      if (file_version > 2)
      {
         if ((lp = strtok(tmp, " \t\n")) == NULL)
            continue; // unit
         lp = strtok(NULL, " \t\n"); // type
      }
      else
      {
         lp = strtok(tmp, " \t\n"); // type
      }

      if (lp) // type
      {
         part->typ = lp;

         attcnt = 0;
         if (attfound)
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            attcnt = atoi(lp);
         }

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         gatecnt = atoi(lp);
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         sigcnt = atoi(lp);
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         alphacnt = atoi(lp);
         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         flg = atoi(lp);

         if (strcmp(part->typ, "UND")) // undefined - ignored
         {        
            if (is_attrib(part->typ, SMD_ATT))  
            {
               part->smdflag = TRUE;
            }
            else
            {
               DeviceTypeTag deviceType = get_mappeddeviceindex(part->typ);
               if (deviceType == deviceTypeUnknown || deviceType == deviceTypeUndefined)
               {
                  doc->SetAttrib(&type->getAttributesRef(), doc->RegisterKeyWord("PADS TYPE", 0, VT_STRING),
                     VT_STRING, part->typ.GetBuffer(0), SA_OVERWRITE, NULL);
               }
               else
               {
                  doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_DEVICETYPE, 0),
                     VT_STRING, deviceTypeTagToValueString(deviceType).GetBuffer(0),SA_OVERWRITE, NULL);
               }
            }
         }
      }

      if (strlen(value))
         doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 0), VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);

      if (strlen(tolerance))
      {
         double tol = atof(tolerance);
         doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, 0), VT_DOUBLE, &tol, SA_OVERWRITE, NULL);
      }

      if (alphacnt)
      {
         part->alphapins = TRUE;
         for (int k=0; k<altdecalcnt; k++)
         {
            BlockStruct *block = Graph_Block_Exists(doc, part->decalname[k], -1);
            if (block == NULL)
            {
               fprintf(ferr, "Syntax Error in PADS file at %ld\n", ifp_linecnt);
               display_error++;
            }
            else
            {
               CString newdecal;    
               newdecal.Format("%s_%s", origname, part->decalname[k]);
               BlockStruct *block1 = Graph_Block_On(GBO_APPEND, newdecal, file->getFileNumber(),0);
               block1->setBlockType(BLOCKTYPE_PCBCOMPONENT);
               Graph_Block_Copy(block, 0, 0, 0, 0, 1, -1, TRUE);
               Graph_Block_Off();
               part->decalname[k] = newdecal;
            }
         } // loop through all decals.
      }
      
		int i=0;
      for (i=0; i<attcnt; i++)
      {
         get_nextline(ifp_line, MAX_LINE, ifp);
         if ((lp = strtok(ifp_line,":")))
         {
            CString keyword = lp;
            keyword.TrimLeft();
            keyword.TrimRight();
            keyword.MakeUpper();

            if (strlen(keyword))
            {
               lp = strtok(NULL, "\n");
               // here check how an attribute is used.
               // SMD needs to get to the component
               // FIDUCIAL needs to get to the component.
               if (is_attrib(keyword, SMD_ATT))
               {
                  if (lp && lp[0] == '1') // SMD:1 is the correct SMD flag
                     part->smdflag = TRUE;
               }
               else if (is_attrib(keyword, FID_ATT))
               {
                  part->fiducial = TRUE;
                  doc->SetUnknownAttrib(&type->getAttributesRef(), keyword, lp, SA_APPEND, NULL);
               }
               else if (is_attrib(keyword, HEIGHT_ATT))
               {
                  char save_unit = unit_flag;
                  unit_flag = heightunit_flag;
                  if (lp && strlen(lp))
                  {
                     double compHeight = cnv_unit(atof(lp));
                     doc->SetAttrib(&type->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, TRUE), VT_UNIT_DOUBLE, &compHeight, SA_OVERWRITE, NULL);
                     part->compheight = compHeight;
                  }
                  unit_flag = save_unit;
               }
               else
               {
                  doc->SetUnknownAttrib(&type->getAttributesRef(), get_attribmap(keyword), lp, SA_APPEND, NULL);
               }
            }
         } // lp == NULL
      }

      for (i=0;i <gatecnt; i++)
         get_gates();

      for (i=0; i<sigcnt; i++)
         get_sig();

      get_alpha(alphacnt, origname, partcnt-1);
   
      if (altdecalcnt == 1)
      {
         BlockStruct *block = Graph_Block_Exists(doc, part->decalname[0], -1);
         if (block)
            type->setBlockNumber( block->getBlockNumber());     
      }
      else if (altdecalcnt > 1)
      {
         // make this type not used, not make an alternate entry.
         // make type_decal for each and copy the contens on the end of the read.
#ifdef _DEBUG
         fprintf(ferr, "DEBUG: PARTTYPE [%s] has %d DECALS - this would not make a unique CAMCAD %s\n", origname, altdecalcnt, ATT_TYPELISTLINK); 
         display_error++;
#endif

         TypeStruct *origt = AddType(file, origname);
			int i=0;
         for (i=0;i <altdecalcnt; i++)
         {
            CString typname;
            typname.Format("%s_%s", origname, part->decalname[i]);
            TypeStruct *typeName = AddType(file,typname);
            doc->SetUnknownAttrib(&typeName->getAttributesRef(), ATT_DERIVED, origname, SA_APPEND, NULL); 

            BlockStruct *block = Graph_Block_Exists(doc, part->decalname[i], -1);
            if (block)
               typeName->setBlockNumber( block->getBlockNumber()); // b->num; pads can have multiple Decals assigned to a type.
            doc->CopyAttribs(&typeName->getAttributesRef(), origt->getAttributesRef());

         }

         for (i=0; i<altdecalcnt; i++)
            doc->SetUnknownAttrib(&origt->getAttributesRef(), ATT_ALTERNATESHAPE, part->decalname[i], SA_APPEND, NULL);
      }
   }
   return 1;
}

/******************************************************************************
* pads_Lib_route
*/
static int pads_Lib_route()
{
   pads_Lib_skip();  
   return 1;
}

/******************************************************************************
* check_ignored_pin
*/
static int check_ignored_pin(const char *comp, const char *pinname)
{
   for (int i=0; i<comppinignoredcnt; i++)
   {
      PADSCompPinIgnored *compPinIgnored = comppinignoredarray[i];
      if (!compPinIgnored->compname.Compare(comp) && !compPinIgnored->pinname.Compare(pinname))
      {
         if (!compPinIgnored->already_reported)
         {
            fprintf(ferr, "Component [%s] Pin [%s] in Netlist, but ignored due to settings in pads.in\n", comp, pinname);
            compPinIgnored->already_reported = TRUE;
            display_error++;
         }
         return 1;
      }
   }
   return 0;
}

/******************************************************************************
* already_comppin
*/
static int already_comppin(FileStruct *filePtr, const char *net_name, const char *comp, const char *pin)
{
   POSITION netPos = filePtr->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = filePtr->getNetList().GetNext(netPos);
      if (net->getNetName().Compare(net_name))
         continue;
   
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         if (compPin->getRefDes().Compare(comp) == 0 && compPin->getPinName().Compare(pin) == 0)
             return 1;
      }
   }

   // here need to check if pin is ignored
   if (check_ignored_pin(comp, pin))
      return 1;

   return 0;
}

/******************************************************************************
* pads_Lib_signal
*  No vianames T1 is layer T2 is change
*/
static int pads_Lib_signal()
{
   char *lp;   
   long sigflg = 0;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString name = lp;

   if ((lp = strtok(NULL, " \t\n")))
   {
      if ((lp = strtok(NULL, " \t\n")))   
         sigflg = atol(lp);
   }

   NetStruct *net = add_net(file, name);
   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      if ((lp = strtok(ifp_line, " .\t\n")) == NULL)
         continue;
      CString refdes = lp;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString pinname = lp;

      if (!already_comppin(file, name, refdes, pinname))
         add_comppin(file, net, refdes, pinname);

      if ((lp = strtok(NULL, " .\t\n")) == NULL)
         continue;      
      refdes = lp;
      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      pinname = lp;
   
      if (!already_comppin(file,name, refdes, pinname))
         add_comppin(file, net, refdes, pinname);
      
      int layer = -1;
      int lastlayer;
      int first = TRUE;
      while (layer != 31)
      {
         get_nextline(ifp_line, MAX_LINE, ifp);
         if ((lp = strtok(ifp_line, " \t\n")) == NULL)
            continue;
         double x = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double y = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         char viachar = lp[0];

         ++lp;
         layer = atoi(lp);
         int layernr;
         if (layer == 0)
         {
            layernr = Graph_Level("UNROUT", "", 0);
            unroutelayer = TRUE;
         }
         else if (layer < 31) // last layer in routes - not a real layer
         {
            layernr = Graph_Level(lp, "", 0);
         }

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double width = cnv_unit(atof(lp));

         int widthindex = 0;
         if (width != 0)
         {
            int err;
            widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         }

         if (first)
         {
            DataStruct *data = Graph_PolyStruct(layernr, 0, 0);
            if (layer > 0) // UNROUT is not an etch
               data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING,name.GetBuffer(0),SA_APPEND, NULL);
            Graph_Poly(NULL, widthindex, 0, 0, 0);
            lastlayer = layer;
         }
         first = FALSE;
         
         double bulge = 0;
         Graph_Vertex(x, y, bulge); 

         if (layer != lastlayer && layer != 31)
         {
            char *vianame = "STANDARDVIA";
         
            DataStruct *data = Graph_Block_Reference(vianame, NULL, file->getFileNumber(), x, y, DegToRad(0), 0 , 1.0,Graph_Level("0", "", 1), TRUE);
            data->getInsert()->setInsertType(insertTypeVia);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, name.GetBuffer(0), SA_APPEND, NULL);
            data = Graph_PolyStruct(layernr, 0, 0);

            if (layer > 0) // unrout is not an etch.
               data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING, name.GetBuffer(0), SA_APPEND, NULL);
            Graph_Poly(NULL,widthindex, 0, 0, 0);
            Graph_Vertex(x, y, bulge);
         }
         lastlayer = layer;
      }
   }
   return 1;
}

/******************************************************************************
* int is_unnamednet
*/
static int  is_unnamednet(const char *nn)
{
   for (int i=0; i<unnamednetcnt; i++)
   {
      if (!STRNICMP(nn, unnamednet[i],strlen(unnamednet[i])))
         return 1;
   }
   return 0;
}

/******************************************************************************
* is_viadef
*/
static int is_viadef(char *v)
{
   for (int i=0;i<viadefcnt;i++)
   {  
      if (viadefarray[i]->name.Compare(v) == 0)
         return 1;
   }
   return FALSE;
}

/******************************************************************************
* pad6_signal
*/
static int pad6_signal()
{
   char *lp;   
   unsigned long sigflg = 0;
   int net_register = doc->IsKeyWord(ATT_NETNAME, 1);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString name = lp;

   if ((lp = strtok(NULL, " \t\n")))
   {
      if ((lp = strtok(NULL," \t\n"))) 
         sigflg = atol(lp);
   }

   NetStruct *net = add_net(file,name);
   if (is_unnamednet(name))
      net->setFlagBits(NETFLAG_UNNAMED);

   // only bit 1, 9, 18 are used in version 3
   if (file_version < 30)  // sigflag in Version 3 have changed and do not means power/gnd anymore.
   {
      int value  = ((sigflg >> 26) & 0xff) ;
      if (value)
      {
         int keyword = doc->IsKeyWord(ATT_POWERNET, TRUE);
         doc->SetAttrib(&net->getAttributesRef(), keyword, VT_NONE,  NULL, SA_OVERWRITE, NULL); 
         keyword = doc->IsKeyWord(NETATT_SIGNALSTACK, TRUE);
         doc->SetAttrib(&net->getAttributesRef(), keyword, VT_INTEGER, &value, SA_OVERWRITE, NULL);
      }
   }

   while (get_nextline(ifp_line, MAX_LINE, ifp))
   {
      int layernr;
      CPnt *lastvertex;

      if ((lp = strtok(ifp_line, " .\t\n")) == NULL)
         continue;
      CString refdes = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString pinname = lp;

      if (!already_comppin(file,name,refdes,pinname))
      {
         add_comppin(file, net, refdes, pinname);
      }
      if ((lp = strtok(NULL," .\t\n")) == NULL) continue;      
      refdes = lp;
      if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
      pinname = lp;
   
      if (!already_comppin(file, name, refdes, pinname))
      {
         add_comppin(file, net, refdes, pinname);
      }

      // here I can get more info like linewidth etc...
      
      double lastx;
      double lasty;
      int layer = -1;
      int lastlayer;
      int first = TRUE;
      int lastwidthindex = -1;
      while ((file_version<40 && layer != 31) || (file_version>=40 && layer != 65))
      {
         get_nextline(ifp_line, MAX_LINE, ifp);

         if ((lp = strtok(ifp_line, " \t\n")) == NULL)
            continue;

         double x = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double y = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         layer   = atoi(lp);

         if (layer == 0)
         {
            unroutelayer = TRUE;
         }
         else if ((file_version<40 && layer < 31) || (file_version>=40 && layer < 65)) // last layer in routes - not a real layer
         {
            layernr = Graph_Level(lp, "", 0);
         }
         else if ((file_version<40 && layer == 31) || (file_version>=40 && layer == 65))
         {
            // end of trace
         }
         else
         {
            fprintf(ferr, "Syntax error in PADS ASCII file at line %ld\n", ifp_linecnt);
            display_error++;
            continue;
         }

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double width = cnv_unit(atof(lp));

         int widthindex = 0;
         if (width < 0)
         {
            // lost here...
            fprintf(ferr, "Syntax error in PADS ASCII file at line %ld\n", ifp_linecnt);
            display_error++;
            continue;
         }
         else if (width > 0)
         {
            int err;
            widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         }

         if (first && layer > 0 && ((file_version<40 && layer < 31) || (file_version>=40 && layer < 65)))
         {
            DataStruct *data = Graph_PolyStruct(layernr, 0, 0);
            if (layer > 0) // UNROUT is not an etch
               data->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&data->getAttributesRef(), net_register, VT_STRING,name.GetBuffer(0),SA_APPEND, NULL); 
            Graph_Poly(NULL, widthindex, 0, 0, 0);
            lastlayer = layer;
            lastwidthindex = widthindex;
            first = FALSE;
         }
         int via = FALSE;

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         int flg = atoi(lp);

         int arcdir = 0;
         int arcfound = FALSE;
         CString vianame;
         while ((lp = strtok(NULL, " \t\n")))   
         {
            if (!STRCMPI(lp, "CW"))
            {
               // here recalc bulge
               arcdir = 1;
               lastx = x;
               lasty = y;
               arcfound = TRUE;
               continue;
            }
            else if (!STRCMPI(lp, "CCW"))
            {
               // here recalc bulge
               arcdir = 2;
               lastx = x;
               lasty = y;
               arcfound = TRUE;
               continue;
            }
            else if (!STRCMPI(lp, "TEARDROP"))  
            {
               // need to deal with this later
               if ((lp = strtok(NULL, " \t\n")) == NULL)
                  continue;
            }
            else if (!STRCMPI(lp, "THERMAL"))   
            {
               // need to deal with this later
            }
            else
            {
               if (is_viadef(lp))
               {
                  vianame = lp;
                  via = TRUE;
               }
            }
         }

         if (arcfound)
            continue;

         if (!first)
         {
            if (arcdir == 1)  // cw
            {
               double rad, sa, da;
               ArcCenter2((double)lastvertex->x, (double)lastvertex->y, x, y, lastx, lasty, &rad, &sa, &da, TRUE);
               lastvertex->bulge = (DbUnit)tan(da / 4);
            }
            else if (arcdir == 2)   // ccw
            {
               double rad, sa, da;
               ArcCenter2((double)lastvertex->x, (double)lastvertex->y, x, y, lastx, lasty, &rad, &sa, &da, FALSE);
               lastvertex->bulge = (DbUnit)tan(da / 4);
            }
            lastvertex = Graph_Vertex(x, y, 0.0); // no via, no arc
         }

         if (via)
         {
            DataStruct *data = Graph_Block_Reference(vianame, NULL, file->getFileNumber(), x, y,
                  DegToRad(0), 0 , 1.0, Graph_Level("0", "", 1), TRUE);
            data->getInsert()->setInsertType(insertTypeVia);
            doc->SetAttrib(&data->getAttributesRef(), net_register, VT_STRING, name.GetBuffer(0), SA_APPEND, NULL);

            if (layer > 0 && ((file_version<40 && layer < 31) || (file_version>=40 && layer < 65)))
            {
               DataStruct *data = Graph_PolyStruct(layernr, 0, 0);
               data->setGraphicClass(GR_CLASS_ETCH);
               doc->SetAttrib(&data->getAttributesRef(), net_register, VT_STRING, name.GetBuffer(0), SA_APPEND, NULL); 
               Graph_Poly(NULL, widthindex, 0, 0, 0); 
               lastvertex = Graph_Vertex(x, y, 0.0); 
               lastlayer = layer;
               lastwidthindex = widthindex;
               first = FALSE;
            }
         }
         else if (widthindex != lastwidthindex) // if the width changes without a via.
         {
            if (!first)
            {
               Graph_Poly(NULL,widthindex, 0, 0, 0);  // fillflag , negative, closed
               lastvertex = Graph_Vertex(x, y, 0.0); // no via, no arc
            }
            lastlayer = layer;
            lastwidthindex = widthindex;
            first = FALSE;
         }

         if (layer == 0)
            first = TRUE;
         if (widthindex != lastwidthindex)
            first = TRUE;
         if (layer != lastlayer)
            first = TRUE;

         via = FALSE;
         lastlayer = layer;
         lastwidthindex = widthindex;
         lastx = x;
         lasty = y;
         arcdir = 0;
      }
   }
   return 1;
}

/******************************************************************************
* pads_Lib_connection
*/
static int pads_Lib_connection()
{
   pads_Lib_skip();  
   return 1;
}

/******************************************************************************
* do_polypieces
*/
static int do_polypieces(const char *pourtype, double xloc, double yloc, const char *name, const char *ownername, const char *signame)
{
   char     *lp;

   get_nextline(ifp_line, MAX_LINE, ifp);

   if ((lp = strtok(ifp_line, " \t\n")) == NULL)
      return 0;
   if (!STRCMPI(lp, ".REUSE."))  // skip the reuse line and 
   {
      get_nextline(ifp_line, MAX_LINE, ifp);
      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         return 0;
   }
   CString ptype = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int corners = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int arcs = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double width = cnv_unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int level = Graph_Level(lp,"",0);

   int widthindex = 0;
   int err;
   if (width != 0)
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *data = NULL;

   if (strlen(ownername) && strcmp(ownername, name))  // not the same
   {
      // here find an existing pourout pointer
      for (int i=0; i<pourcnt && data == NULL; i++)
      {
         if (!strcmp(pourarray[i]->name, ownername))
            data = pourarray[i]->data_adress;
      }                                                                     
      if (data == NULL)
      {
         fprintf(ferr, "POUR ownername [%s] not found\n", ownername);
         display_error++;
      }
   }
   
   if (data == NULL)
   {
      data = Graph_PolyStruct(level, 0 ,0);
      data->setGraphicClass(GR_CLASS_ETCH);
      if (signame && strlen(signame))
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 1), VT_STRING, (void *)signame, SA_APPEND, NULL);
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NAME, 1), VT_STRING, (void *)name, SA_APPEND, NULL); 
   }

   PADSPour *pour = new PADSPour;
   pourarray.SetAtGrow(pourcnt++, pour);  
   pour->name = name;
   pour->data_adress = data;

   int fill = 0;
   int voidfill = 0;
   int close = 0;
   int seg = FALSE;
   int boundary = FALSE;
   if (!strcmp(pourtype, "POUROUT"))
   {
      close = TRUE;
      boundary = TRUE;
   }
   else if (!strcmp(pourtype, "HATOUT"))
   {
      fill = close = TRUE;
   }
   else if (!strcmp(pourtype, "VOIDOUT"))
   {
      fill = TRUE;
      close = TRUE;
      voidfill = TRUE;
   }
   else if (!strcmp(pourtype, "PADTHERM"))
   {
      fill = FALSE;
      close = FALSE;
      voidfill = FALSE;
      seg = TRUE;
   }
   else if (!strcmp(pourtype, "VIATHERM"))
   {
      fill = FALSE;
      close = FALSE;
      voidfill = FALSE;
      seg = TRUE;
   }
   else
   {
      fprintf(ferr, "Poly type [%s] unknown at %ld\n", pourtype, ifp_linecnt);
      display_error++;
   }

   CPoly *poly = Graph_Poly(data, widthindex, fill, voidfill, close);
   poly->setThermalLine(seg);
   poly->setFloodBoundary(boundary);

   if (!STRCMPI(ptype, "CIRCLE"))
   {
      if (corners + arcs == 2)
      {
         double   x1,y1,x2,y2;

         get_nextline(ifp_line, MAX_LINE, ifp);
         // just in case there is a error
         if ((lp = strtok(ifp_line, " \t\n")))
            x1 = cnv_unit(atof(lp));
         if ((lp = strtok(NULL, " \t\n")))      
            y1 = cnv_unit(atof(lp));

         get_nextline(ifp_line, MAX_LINE, ifp);
         // just in case there is a error
         if ((lp = strtok(ifp_line, " \t\n")))
            x2 = cnv_unit(atof(lp));
         if ((lp = strtok(NULL, " \t\n")))      
            y2 = cnv_unit(atof(lp));

         Graph_Vertex(x1 + xloc, y1 + yloc,1);
         Graph_Vertex(x2 + xloc, y2 + yloc,1);
         Graph_Vertex(x1 + xloc, y1 + yloc,0);
      }
      else
      {
         fprintf(ferr, "Circle with more than 2 corners at %ld\n", ifp_linecnt);
         display_error++;
         for (int i=0; i<corners+arcs; i++)
            get_nextline(ifp_line, MAX_LINE, ifp);
      }
   }
   else
   {
      CPnt *last_vertex = NULL;
      for (int i=0; i<corners+arcs; i++)
      {
         get_nextline(ifp_line, MAX_LINE, ifp);

         // just in case there is a error
         if ((lp = strtok(ifp_line ," \t\n")) == NULL)
            continue;
         double x = cnv_unit(atof(lp));

         if ((lp = strtok(NULL, " \t\n")) == NULL)
            continue;
         double y = cnv_unit(atof(lp));

         double bulge = 0;
         if ((lp = strtok(NULL, " \t\n")))
         {
            // if arc, x,y is the center of an arc
            double a = atof(lp) / 10;
            double sa = DegToRad(a);
            lp = strtok(NULL, " \t\n");
            a = atof(lp) / 10;

            double da = DegToRad(a);
            if (fabs(a) >= 360)
            {
               fprintf(ferr, "PADS Syntax error at %ld\n", ifp_linecnt);
               da = 0;
            }
            bulge = tan(da / 4);                                   

            if (last_vertex)
            {
               last_vertex->bulge = (DbUnit)bulge;
            }
            else
            {
               fprintf(ferr, "Arc found without last vertex at %ld\n", ifp_linecnt);
               display_error++;
            }
         }
         else
         {
            last_vertex = Graph_Vertex(x + xloc, y + yloc, bulge);
         }
      }
   }
   return 1;
}

/******************************************************************************
* pads_Lib_pour
*/
static int pads_Lib_pour()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char *lp;

      if ((lp = strtok(ifp_line, " \t\n")) == NULL)
         continue;
      CString name = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString pourtype = lp;

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double xloc = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      double yloc = cnv_unit(atof(lp));

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      int pieces = atoi(lp);

      if ((lp = strtok(NULL, " \t\n")) == NULL)
         continue;
      CString ownername = "";
      CString signame   = "";
      if ((lp = strtok(NULL, " \t\n")))   
      {
         ownername = lp;
         if ((lp = strtok(NULL," \t\n"))) 
            signame = lp;
      }

      for (int i=0; i<pieces; i++)
         do_polypieces(pourtype, xloc, yloc, name, ownername, signame);
   }
   return 1;
}

/******************************************************************************
* pads_Lib_null
*/
static int pads_Lib_null()
{
   pads_Lib_skip();  
   return 1;
}

/******************************************************************************
* cnv_unit
*   Converts from PADS any units to INCH units.
*/
static double cnv_unit(double x)
{
   double factor;

   switch (unit_flag)
   {
   case 'I': /* inch */
      factor = Units_Factor(UNIT_INCHES, PageUnits);
      break;
   case 'M': /* metric */
      factor = Units_Factor(UNIT_MM, PageUnits);
      break;
   case 'P': /* pads */

   case 'A': /* mil */
      factor = Units_Factor(UNIT_MILS, PageUnits);
      break;
   case 'B': /* basic */
      factor = Units_Factor(UNIT_INCHES, PageUnits);
      factor = factor / 38100000.0;
      break;
   }

   return x * factor;
}

/******************************************************************************
* pads_Lib_end
*/
static int pads_Lib_end()
{
   eof_found = TRUE; // logical end 
   return 1;
}

// end paste of padin.cpp

