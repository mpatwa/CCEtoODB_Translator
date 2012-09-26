
/*****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.

   Limitations:
   only a1 technology is used !
   padstack definition for top and bottom placement not supported.
   tracks are either round or square, inbetween chamfer radius not allowed.
   no blind or buried vias supported.


   layer 1 = solder
         2 = component
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
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg			*progress;
//extern LayerTypeInfoStruct layertypes[];

/****************************************************************************/
/*

*/
/****************************************************************************/

static int   fnull();            // skipped, but reports the skip
static int   fskip();            // skipped, but no logs

/* Defines *****************************************************************/

#define  CXFERR                 "cxf.log"

#define  FNULL                   fnull
#define  FSKIP                   fskip

static   double                  TEXT_CORRECT  = 0.75;

#define  MAX_LINE                500   /* Max line length. Long attributes can extent a line  */
#define  MAX_LAYERS              255   /* Max number of layers.   */

/* Layer codes.      */

#define  LAY_ALL_LAYERS          -1
#define  LAY_NULL                -2

/* Structures **************************************************************/

typedef struct
{
   CString  origname;
   CString  rename;
} RenameLayer;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   int      cxf_layernum;   // CXF number
   int      camcad_layerindex;      // Graph_Level index
   bool     isElectrical;
   CString  rule;
} CXFLayer;
typedef CTypedPtrArray<CPtrArray, CXFLayer*> CLayerTecArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   int      drillindex;
   double   diameter;
   int      used;
} CXFDrillTec;
typedef CTypedPtrArray<CPtrArray, CXFDrillTec*> CDrillTecArray;

typedef struct          // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  rulename;
   int      viaindex, drillindex;
   double   width;
   int      appform;    // T_ROUND or T_SQUARE
} CXFViaTec;
typedef CTypedPtrArray<CPtrArray, CXFViaTec*> CViaTecArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  rulename;
   int      trackindex;
   int      appform;   // oblong or rectangle   
   double   width;
} CXFTrackTec;
typedef CTypedPtrArray<CPtrArray, CXFTrackTec*> CTrackTecArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   int      textindex;
   double   height;
} CXFTextTec;
typedef CTypedPtrArray<CPtrArray, CXFTextTec*> CTextTecArray;

typedef struct
{
   CString  stackname;
}PDIF_comppadname;   
typedef CTypedPtrArray<CPtrArray, PDIF_comppadname*> CCompPadnameArray;

typedef struct
{
   CString  compname;
}PDIF_arrowname;  
typedef CTypedPtrArray<CPtrArray, PDIF_arrowname*> CArrowArray;

typedef  struct
{
   char  *token;
   int      (*function)();
} List;

/* Bracket definitions.       */

static List  brk_lst[] =
{
   "[",                    FNULL,
   "]",                    FNULL
};

#define  SIZ_BRK_LST       (sizeof(brk_lst) / sizeof(List))
#define  BRK_SQUARE        0
#define  BRK_B_SQUARE      1

/* Start of CXF file.  */

static int      cxf_history();
static int      cxf_pcbrule();
static int      cxf_pcblayerplan();
static int      cxf_pcbloclib();
static int      cxf_pcbmaclib();
static int      cxf_pcbboard();
static int      cxf_pcbplacement();
static int      cxf_pcbnetlist();

static List  str_lst[] =
{
   ".HISTORY",            cxf_history,
   ".PCBRULE",            cxf_pcbrule,
   ".PCBLAYERPLAN",       cxf_pcblayerplan,
   ".PCBMACLIB",          cxf_pcbmaclib,
   ".PCBBOARD",           cxf_pcbboard,
   ".PCBLOCLIB",          cxf_pcbloclib,
   ".PCBPLACEMENT",       cxf_pcbplacement,
   ".PCBNETLIST",         cxf_pcbnetlist,
};
#define  SIZ_STR_LST       (sizeof(str_lst) / sizeof(List))

static int     cxf_unit();

/* Function Define *********************************************************/

static   double      cnv_unit(char *);
static   int         go_command(List *,int);
static   void        free_all_mem();
static   int         get_tok();
static   int         p_error();
static   int         tok_search(List *,int);
static   int         push_tok();
static   int         loop_command(List *,int);
static   void        clean_text(char *);
static   int         get_next(char *,int);
static   int         get_line(char *,int);
static   void        pad_error(char *,int);
static   int         load_cxfsettings(const CString fname);
static   int         assign_layers();
static   int         get_trackcode(int widthcode, int layerindex);

/* Globals ****************************************************************/

typedef  struct
{
   int      electrical_layers;            /* number of electrical layers from lyrphid */
   int      grid;                         /* Grid size.     */
   int      max_layers;                   /* Number of layers defined.  */
   int      max_dlayers;                  /* Number of default layers.  */

   /* Current values that are changed with global  */
   /* PDIF commands.                                              */
   int      cur_layer;                    /* Layer.      */
   int      cur_style;                    /* Line style. */
   double   cur_width;                    /* Line width     */
   int      cur_widthindex;               //derived linewidth index
   double   cur_textsize;                 /* Line width     */

   /* Current Component info used for Instance.    */
   double   cur_pos_x;                    /* position.   */
   double   cur_pos_y;
   double   cur_scale_x;                  /* Scale.      */
   double   cur_scale_y;
   int      cur_rotation;                 /* Rotation.   */
   int      cur_mirror;                   /* Current mirror.   */

   /* Current pad stack info.    */
   int      cur_pad;                      /* Current pad.         */
   int      cur_pstack;                   /* Current padstack number. */
   int      cur_smd;
   double   cur_drillsize;                // from derivedrill
   int      cur_ty;
   /* Pad stacks */
   int      max_dpads;                    /* number of pad stacks. */
   int      cur_textmirror;
   int      cur_textjust;
   int      cur_textrot;
   int      cur_dimtextrot;
   int      cur_pdifversion;
   int      cur_polywidthindex;
   double   cur_polywidth;
} Global;

static   Global      G;                            /* global const. */

static   CDrillTecArray drilltecarray;
static   int         drillteccnt;

static   CLayerTecArray layerarray;
static   int         layercnt;

static   CViaTecArray   viatecarray;
static   int         viateccnt;

static   CTrackTecArray tracktecarray;
static   int         trackteccnt;

static   CTextTecArray  texttecarray;
static   int         textteccnt;

static   FILE        *ifp;                         /* File pointers.    */
static   long        ifp_line = 0;                 /* Current line number. */

static   char        token[MAX_LINE];              /* Current token.       */

static   RenameLayer layer_rename[MAX_LAYERS];       /* Array of layers from pdif.in   */
static   int         layer_rename_cnt;

static   int         Push_tok = FALSE;
static   char        cur_line[MAX_LINE];
static   int         cur_new = TRUE;

typedef CArray<Point2, Point2&> CPolyArray;

static   int         polycnt = 0;
static   CPolyArray  poly_l;

static   FILE        *ferr;
static   int         cur_filenum = 0;
static   FileStruct  *file = NULL;
static   CCEtoODBDoc  *doc;
static   int         pageunits;
static   double      scalefactor = 1;

static   int         cur_pin_in_poly;
static   DataStruct  *cur_compdata;    // global pointer to the current component.
static   int         convert_padcircle_2_app, convert_padrect_2_app, convert_padline_2_app;
static   int         display_error = 0;

static   int         token_name = FALSE;
static   int         complexcnt;

static   int         USE_DERIVEDSYM = TRUE;
static   double      PADSHAPE_RADIUS = 50;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static int viaInsertCount = 0;
static int unkInsertCount = 0;

/*****************************************************************************/
/*
*/
static const char  *rename_layer(const char *l)
{
   int   i;

   for (i=0;i<layer_rename_cnt;i++)
   {
      if (!STRCMPI(l,layer_rename[i].origname))
      {
         return layer_rename[i].rename;
      }
   }
   return l;
}

/****************************************************************************/
/*
*/
static int report_padstackaccess(const char *settingsFile)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         if (block->getFlags() & BL_ACCESS_TOP)    continue;
         if (block->getFlags() & BL_ACCESS_BOTTOM) continue;
         fprintf(ferr,"Padstack [%s] has no Access -> see .BUILD_PADSTACK in %s\n", 
               block->getName(), settingsFile);
         display_error++;
      }
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int PurgeUnusedDrills(CCEtoODBDoc *doc)
{
   int   i;

   for (i=0;i<drillteccnt;i++)
   {
      if (drilltecarray[i]->used)   continue;
      CString  drillname;
      drillname.Format("DRILL_%d",drilltecarray[i]->drillindex);
      BlockStruct *b = Graph_Block_Exists(doc, drillname, -1);
      if (b)   doc->RemoveBlock(b);
   }

   return 1;
}

/******************************************************************************
* ReadCXF
*/
void ReadCXF(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{
   file = NULL;
   doc = Doc;
   pageunits = PageUnits;
   display_error = 0;

   Push_tok = FALSE;
   cur_new = TRUE;
   cur_line[0] = '\0';
   ifp_line = 0;
   complexcnt = 0;
   cur_compdata = NULL;
   cur_pin_in_poly = 0;

   viaInsertCount = 0;
   unkInsertCount = 0;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(path_buffer,"rt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file",path_buffer);
      ErrorMessage(t, "Error");
      return;
   }

   CString cxfLogFile;
   ferr = getApp().OpenOperationLogFile(CXFERR, cxfLogFile);
   if (ferr == NULL) // error message already issued, just return.
      return;

   poly_l.SetSize(100,100);
   polycnt = 0;

   drilltecarray.SetSize(100,100);
   drillteccnt = 0;

   layerarray.SetSize(100,100);
   layercnt = 0;

   viatecarray.SetSize(100,100);
   viateccnt = 0;

   tracktecarray.SetSize(100,100);
   trackteccnt = 0;

   texttecarray.SetSize(100,100);
   textteccnt = 0;

   /* initialize scanning parameters */
   memset(&G,0,sizeof(Global));

   CString settingsFile = getApp().getImportSettingsFilePath("cxf.in");
   {
      CString msg;
      msg.Format("\nCALAY PRISMA: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_cxfsettings(settingsFile);

   rewind(ifp);
   if (!get_line(cur_line,MAX_LINE))
   {
      ErrorMessage("CXF Read Error");
   }
   else
   {
      CString  projectname;
      projectname = cur_line; // has ' here
      projectname.Remove('\'');
      file = Graph_File_Start(projectname, fileTypeCalayPrismaLayout);
      file->setBlockType(blockTypePcb);

      cur_filenum = file->getFileNumber();

      cur_new = TRUE;
      cur_line[0] = '\0';
      int err;
      Graph_Aperture("", T_ROUND, 0.0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      Graph_Level("0","",1);

      if (loop_command(str_lst,SIZ_STR_LST) < 0)
      {
         // printf big prob.
         ErrorMessage("CXF read error", "Error");
      }
      else
      {
#ifndef _DEBUG
         doc->purgeUnusedBlocks();
#endif
         PurgeUnusedDrills(doc);
         assign_layers();

         update_smdpads(doc); // this makes pad access
         update_smdrule_geometries(doc, ComponentSMDrule);
         update_smdrule_components(doc, file, ComponentSMDrule);

         RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.

         OptimizePadstacks(doc, pageunits, FALSE); // this can create unused blocks
         generate_PADSTACKACCESSFLAG(doc, 1);
         generate_PINLOC(doc, file, 1);   // this function generates the PINLOC argument for all pins.

         doc->RemoveUnusedLayers(false);

         if (file)   
         {
            EliminateDuplicateVias(doc, file->getBlock());
            // this can happen on strange via placements.
            EliminateSinglePointPolys(doc);                
            BreakSpikePolys(file->getBlock());
            //progress->SetStatus("Crack");
            Crack(doc,file->getBlock(), TRUE);          
   
            //progress->SetStatus("Elim traces");
            double accuracy = get_accuracy(doc);
            EliminateOverlappingTraces(doc,file->getBlock(), TRUE, accuracy);                
         }
      }
   }

   fclose(ifp);

   poly_l.RemoveAll();
   polycnt = 0;

   int   i;

   for (i=0;i<drillteccnt;i++)
   {
      delete drilltecarray[i];  
   }
   drilltecarray.RemoveAll();
   drillteccnt = 0;

   for (i=0;i<layercnt;i++)
   {
      delete layerarray[i];  
   }
   layerarray.RemoveAll();
   layercnt = 0;

   drilltecarray.RemoveAll();

   for (i=0;i<viateccnt;i++)
   {
      delete viatecarray[i];  
   }
   viatecarray.RemoveAll();
   viateccnt = 0;

   viatecarray.RemoveAll();

   for (i=0;i<trackteccnt;i++)
   {
      delete tracktecarray[i];  
   }
   tracktecarray.RemoveAll();
   trackteccnt = 0;

   tracktecarray.RemoveAll();

   for (i=0;i<textteccnt;i++)
   {
      delete texttecarray[i];  
   }
   texttecarray.RemoveAll();
   textteccnt = 0;

   texttecarray.RemoveAll();

   free_all_mem();

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!cxfLogFile.IsEmpty())
      fclose(ferr);

   if (display_error &&!cxfLogFile.IsEmpty())
      Logreader(cxfLogFile);

   return;
}

/****************************************************************************/
/*
*/
//static int get_layertype(const char *l)
//{
//   int   i;
//
//   for (i=0;i<MAX_LAYTYPE;i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return   LAYTYPE_UNKNOWN; 
//}

/****************************************************************************/
/*
*/
static int load_cxfsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   layer_rename_cnt = 0;
   USE_DERIVEDSYM = TRUE;  // use 1206 instead of 1206~3
   PADSHAPE_RADIUS = 50;
   ComponentSMDrule = 0;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "CXF Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      CString  w = line;
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            ComponentSMDrule = atoi(lp);
         }
         else
         if (!STRICMP(lp,".USE_DERIVEDSYM"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            if (lp[0] == 'N' || lp[0] == 'n')
               USE_DERIVEDSYM = FALSE;
         }
         else
         if (!STRICMP(lp,".PADSHAPE_RADIUS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            PADSHAPE_RADIUS = atof(lp);
         }
         else
         if (!STRICMP(lp,".LAYERRENAME"))
         {
            CString  olayer;
            CString  rlayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            olayer = _strupr(lp);
            olayer.TrimLeft();
            olayer.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            rlayer = lp;
            rlayer.TrimLeft();
            rlayer.TrimRight();

            if (layer_rename_cnt < MAX_LAYERS)
            {
               layer_rename[layer_rename_cnt].origname = olayer;
               layer_rename[layer_rename_cnt].rename = rlayer;
               layer_rename_cnt++;
            }
            else
            {
               fprintf(ferr,"Too many layer renames\n");
               display_error++;
            }
         }

      }
   }

   fclose(fp);

   return 1;
}



/****************************************************************************/
/*
*/
int go_command(List *tok_lst,int lst_size)
{
   int      i,res = 0;

   if (!get_tok()) // this is on end of file
      return -2;

   if (token[0] != '.')
   {
      CString  tmp;
      tmp.Format("Error: Unexpected token [%s] found at %ld", token, ifp_line);
      ErrorMessage(tmp,"Syntax error in CXF file");
      return -2;
   }

   if ((i = tok_search(tok_lst,lst_size)) >= 0)
   {
      res = (*tok_lst[i].function)();
      if (res < 0)
      {
         CString ls = tok_lst[i].token;
         int w = 0;
      }
   }
   else
   {
      fnull();
      // return p_error();
   }
   return res;
}                                   


/****************************************************************************/
/*
   Loop through a section.
*/
static int loop_command(List *list,int size)
{
   int      repeat = TRUE;

   while (repeat)
   {
      int res;
      if ((res = go_command(list,size)) < 0) 
      {
         if (res == -2) // end of file
            return 0;
         return -1;
      }
   }
   return 1;
}


/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next }] endpoint.
*/
static int fnull()
{
   int      brk_count = 0;
   
#ifdef _DEBUG
   fprintf(ferr,"DEBUG: Unsupported Token [%s] at %ld -> skipped\n",token,ifp_line);
   display_error++;
#endif

   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)   continue;   // do not evaluate a name []{} token.
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_SQUARE:
               ++brk_count;
            break;
            case BRK_B_SQUARE:
               if (--brk_count == 0)
                  return 1;
            break;
            default:
            {
            }
            break;
         }
      }
      else
         return p_error();
   }
   return 1;
}

/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next }] endpoint.
*/
static int fskip()
{
   int      brk_count = 0;
   
   //fprintf(ferr,"Unsupported Token [%s] at %ld -> skipped\n",token,ifp_line);
   //display_error++;
   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)   continue;   // do not evaluate a name []{} token.
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_SQUARE:
               ++brk_count;
            break;
            case BRK_B_SQUARE:
               if (--brk_count == 0)
                  return 1;
            break;
            default:
            {
               // attrbutes can be {At Key Value" x y} without a text delimeter
            }
            break;
         }
      }
      else
         return p_error();
   }
   return 1;
}

/****************************************************************************/
/*
   Component Section.
*/

/****************************************************************************/
/*
*/
static int cxf_history()
{
   return fskip();
}

/****************************************************************************/
/*
   LayerStruct *ll = doc->FindLayer_by_Name(lay1);
         if (ll->getLayerType() == LAYTYPE_MASK_ALL)
         {
            if (!STRCMPI(lay2,"1")) ll->setLayerType(LAYTYPE_MASK_TOP);
            if (!STRCMPI(lay2,"2")) ll->setLayerType(LAYTYPE_MASK_BOTTOM);

*/
static int cxf_layerset(CString *layerset)
{
   int   mask = 0;
   int   paste = 0;

   int   brk_cnt = 0;

   *layerset = "";
   if (!get_tok())   return p_error();
   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "LayerSet Error");
      return -1;
   }
   brk_cnt++;

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
      // hierachical [] management.
      if (!STRCMPI(token,"["))   
      {
         brk_cnt++;
         continue;
      }
      if (!STRCMPI(token,"]"))   
      {
         brk_cnt--;
         if (brk_cnt == 0)
            break;
         continue;
      }
      LayerStruct *ll = doc->FindLayer_by_Name(token);
      if (ll)
      {
         if (ll->getLayerType() == LAYTYPE_MASK_TOP)    mask |= 1;
         else
         if (ll->getLayerType() == LAYTYPE_MASK_BOTTOM) mask |= 2;
         else
            mask |= 4;

         if (ll->getLayerType() == LAYTYPE_PASTE_TOP)      paste |= 1;
         else
         if (ll->getLayerType() == LAYTYPE_PASTE_BOTTOM)   paste |= 2;
         else
            paste |= 4;
      }

      if (!(*layerset).IsEmpty())
         *layerset += " ";
      *layerset += token;
   }

   if (mask == 3) 
   {
      *layerset = "SOLDERMASK";
      int l = Graph_Level(*layerset,"",0);
      doc->getLayerArray()[l]->setLayerType(LAYTYPE_MASK_ALL);
   }
   if (paste == 3)   
   {
      *layerset = "SOLDERPASTE";
      int l = Graph_Level(*layerset,"",0);
      doc->getLayerArray()[l]->setLayerType(LAYTYPE_PASTE_ALL);
   }

   // Case dts0100401221
   // Special layer *PHY means "all electrical layers", convert the name to 
   // a list of all electrical layers, so elsewhere can loop on the set doing whatever.
   // Use the cxf layer numbers, not the Layers.
   if ((*layerset).CompareNoCase("*PHY") == 0)
   {
      *layerset = "";
      for (int i = 0; i < layercnt; i++)
      {
         CXFLayer *ll = layerarray[i];
         if (ll->isElectrical)
         {
            CString layernum;
            layernum.Format("%d", ll->cxf_layernum);
            if (!(*layerset).IsEmpty())
               *layerset += " ";
            *layerset += layernum;
         }
      }
   }

   *layerset = rename_layer(*layerset);

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_keepin()
{
   fskip();
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_keepout()
{
   fskip();
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_clip()
{
   fskip();
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_fill()
{
   CPoly *firstcp = NULL, *cp = NULL;

   if (!get_tok())   return p_error();
   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "LayerSet Error");
      return -1;
   }

   // fill type
   if (!get_tok())   return p_error();

   CString  layerset;
   cxf_layerset(&layerset);
   int layerindex = Graph_Level(layerset, "", 0);
   
   DataStruct *d = Graph_PolyStruct( layerindex, 0L,FALSE);
	d->setGraphicClass(graphicClassEtch);

   int   hatchline = FALSE;

   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   //  

      if (!STRCMPI(token,".FLAGS"))
      {
         hatchline = FALSE;
         if (!get_tok())   return p_error(); // [

         while (TRUE)
         {
            if (!get_tok())   return p_error(); // ]
            if (!STRCMPI(token,"]"))   break;

            if (!STRCMPI(token,"PA"))
            {
               hatchline = TRUE;
            }
         }
      }
      else
      {
         int widthindex = atoi(token);
      
         Point2      p;
         polycnt = 0;
         // loop until closed bracket
         while (TRUE)
         {
            if (!get_tok())   return p_error();
   
            if (!STRCMPI(token,"]"))   
            {
               push_tok();
               break;
            }
				else if (!STRCMPI(token,".ARC"))
            {  
               if (!get_tok())   return p_error(); // :
               double centerx = cnv_unit(token);
               if (!get_tok())   return p_error(); // :
               if (STRCMPI(token,":"))
               {
                  ErrorMessage(": expected", "Track/Arc Error");
                  return -1;
               }
               if (!get_tok())   return p_error();
               double centery = cnv_unit(token);

               if (!get_tok())   return p_error(); // angle
               double deltaangle = atof(token);
   
               if (fabs(deltaangle) == 360)
               {
                  // it ends where it started.
                  if (!get_tok())   return p_error(); // : endx
                  p.x = cnv_unit(token);
                  if (!get_tok())   return p_error(); // :
                  if (STRCMPI(token,":"))
                  {
                     ErrorMessage(": ",   "Track Error");
                     return -1;
                  }

                  if (!get_tok())   return p_error(); // : endy
                  p.y = cnv_unit(token);

                  double r = sqrt((p.x-centerx)*(p.x-centerx) + (p.y-centery)*(p.y-centery));

                  // update the start polygon
                  p = poly_l.ElementAt(polycnt-1);
                  polycnt = polycnt-1;
                  p.x = centerx - r;
                  p.y = centery;
                  p.bulge = 1;
            
                  poly_l.SetAtGrow(polycnt,p);
                  polycnt++;

                  p.x = centerx + r;
                  p.y = centery;
                  p.bulge = 1;
            
                  poly_l.SetAtGrow(polycnt,p);
                  polycnt++;
            
                  p.x = centerx - r;
                  p.y = centery;
                  p.bulge = 0;
            
                  poly_l.SetAtGrow(polycnt,p);
                  polycnt++;
         
                  continue;   
               }
               else
               {
                  p = poly_l.ElementAt(polycnt-1);
                  polycnt = polycnt-1;
                  p.bulge = tan(DegToRad(deltaangle)/4);
                  poly_l.SetAtGrow(polycnt,p);
                  polycnt++;
               }
               if (!get_tok())   return p_error(); // next x
            }
            else if (strlen(token) && token[0] == '.')
            {
					// If the token is not ".ARC" but is "." something else then break the loop
               push_tok();
               break;
            }

            p.x = cnv_unit(token);
            if (!get_tok())   return p_error(); // :
            if (STRCMPI(token,":"))
            {
               ErrorMessage(": expected", "Track Error");
               return -1;
            }
   
            if (!get_tok())   return p_error();
            p.y = cnv_unit(token);
            p.bulge = 0;

            poly_l.SetAtGrow(polycnt,p);
            polycnt++;
         }
      
         if (polycnt)
         {
            // write out
            int wi = get_trackcode(widthindex, layerindex);
            int fill = 0, gvoid = 0, close = 0;

            Point2 p1, p2;
            p1 = poly_l.ElementAt(0);
            p2 = poly_l.ElementAt(polycnt-1);

            if (polycnt > 2)
            {
               if (fabs(p1.x - p2.x) < SMALLNUMBER && fabs(p1.y - p2.y) < SMALLNUMBER)
               {
                  fill = TRUE;
                  close = TRUE;
               }
            }

            cp = Graph_Poly(NULL, wi, fill, gvoid, close);
            if (!firstcp)
               firstcp = cp;

            if (hatchline) // if there are hatch line, make the first one a boudary and hidden.
            {
               firstcp->setFloodBoundary(true);
               firstcp->setHidden(true);
               cp->setHatchLine(true);
            }

            for (int i=0;i<polycnt;i++)
            {
               // write vertex
               p = poly_l.ElementAt(i);
               Graph_Vertex(p.x,p.y, p.bulge);
            }
            polycnt = 0;
         }
      }
   }     

   if (!get_tok())   return p_error(); // ;

   return 1;
}

/****************************************************************************/
/*
*/
static int find_trackcode_rule(int widthcode, const char *rule)
{
   int   i;

   for (i=0;i<trackteccnt;i++)
   {
#ifdef _DEBUG
      CXFTrackTec *c = tracktecarray[i];
#endif
      if (tracktecarray[i]->rulename.CompareNoCase(rule) == 0 &&
          tracktecarray[i]->trackindex == widthcode)
      {
         int err;
         int w = Graph_Aperture("", tracktecarray[i]->appform, tracktecarray[i]->width ,
            0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         return w;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int find_trackcode_norule(int widthcode)
{
   int   i;

   for (i=0;i<trackteccnt;i++)
   {
#ifdef _DEBUG
      CXFTrackTec *c = tracktecarray[i];
#endif
      if (tracktecarray[i]->trackindex == widthcode)
      {
         int err;
         int w = Graph_Aperture("", tracktecarray[i]->appform, tracktecarray[i]->width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         return w;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_trackcode(int widthcode, int layerindex)
{
   int   w = 0;
   int   i;

   if (widthcode == 0)  return 0;

   // find pcbrul from layer index
   for (i=0;i<layercnt;i++)
   {
#ifdef _DEBUG
   CXFLayer *ll = layerarray[i];
#endif
      if (layerarray[i]->camcad_layerindex == layerindex)
      {
         if ((w = find_trackcode_rule(widthcode, layerarray[i]->rule)) > -1)
            return w;
      }
   }  


   // here just find any track tech code
   w = find_trackcode_norule(widthcode);
   return w ;
}

/****************************************************************************/
/*
*/
static int get_drilltype(int drilltype)
{
   int   i;

   if (drilltype == 0)  return -1;

   for (i=0;i<drillteccnt;i++)
   {
#ifdef _DEBUG
      CXFDrillTec *c = drilltecarray[i];
#endif
      if (drilltecarray[i]->drillindex == drilltype)
      {
         return i;
      }
   }

   return -1;
}
/****************************************************************************/
/*
   tracks in a padshape defintion have to be treated different than normal tracks
   if (netlist) the name is the netname, otherwise it is the definition name.
*/
static int cxf_track(int netlist, const char *name, int padshape)
{
   Point2      p;

   if (!get_tok())   return p_error();
   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "Track Error");
      return -1;
   }

   CString  layerset;
   cxf_layerset(&layerset);
   int layerindex = Graph_Level(layerset, "", 0);

   if (!get_tok())   return p_error();

   int ht = FALSE;
   if (!STRCMPI(token,".FLAGS"))
   {
      if (!get_tok())   return p_error(); // [

      while (TRUE)
      {
         if (!get_tok())   return p_error(); // ]
         if (!STRCMPI(token,"]"))   break;

         // flags
         if (!STRCMPI(token,"HT"))
            ht = TRUE;
      }
   }
   else
   {
      ErrorMessage(".FLAGS expected", "TRACK record");
   }

   if (!get_tok())   return p_error(); // tracktype
   int   widthindex = atoi(token);

   polycnt = 0;
   int fill = 0, gvoid = 0, close = 0;

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   
            break;
      
      if (!STRCMPI(token,".ARC"))
      {
         if (!get_tok())   return p_error(); // :
         double centerx = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (STRCMPI(token,":"))
         {
            ErrorMessage(": expected", "Track/Arc Error");
            return -1;
         }
         if (!get_tok())   return p_error();
         double centery = cnv_unit(token);

         if (!get_tok())   return p_error(); // angle
         double deltaangle = atof(token);

         if (fabs(deltaangle) == 360)
         {
            // it ends where it started.
            if (!get_tok())   return p_error(); // : endx
            p.x = cnv_unit(token);
            if (!get_tok())   return p_error(); // :
            if (STRCMPI(token,":"))
            {
               ErrorMessage(": ",   "Track Error");
               return -1;
            }

            if (!get_tok())   return p_error(); // : endy
            p.y = cnv_unit(token);

            double r = sqrt((p.x-centerx)*(p.x-centerx) + (p.y-centery)*(p.y-centery));

            // update the start polygon
            p = poly_l.ElementAt(polycnt-1);
            polycnt = polycnt-1;
            p.x = centerx - r; // this is start
            p.y = centery;
            p.bulge = 1;
            
            poly_l.SetAtGrow(polycnt,p);
            polycnt++;

            p.x = centerx + r;
            p.y = centery;
            p.bulge = 1;
            
            poly_l.SetAtGrow(polycnt,p);
            polycnt++;
            
            p.x = centerx - r;
            p.y = centery;
            p.bulge = 0;
            
            poly_l.SetAtGrow(polycnt,p);
            polycnt++;
         
            continue;   
         }
         else
         {
            p = poly_l.ElementAt(polycnt-1);
            polycnt = polycnt-1;
            p.bulge = tan(DegToRad(deltaangle)/4);
            poly_l.SetAtGrow(polycnt,p);
            polycnt++;
         }
         if (!get_tok())   return p_error(); // next x
      }

      p.x = cnv_unit(token);
      if (!get_tok())   return p_error(); // :
      if (STRCMPI(token,":"))
      {
         ErrorMessage(": expected", "Track Error");
         return -1;
      }

      if (!get_tok())   return p_error();
      p.y = cnv_unit(token);
      p.bulge = 0;

      poly_l.SetAtGrow(polycnt,p);
      polycnt++;
   }

   if (ht)  // do not write heat trap.
   {
      polycnt = 0;
      return 1;
   }

   int      layindex = layerindex;
   CString  pshapename;

   if (padshape)
   {
      // need to make a complex aperture
      pshapename.Format("%s_%d", name, layerindex);
      BlockStruct *b = Graph_Block_On(GBO_APPEND,pshapename,cur_filenum,0);
      b->setBlockType(BLOCKTYPE_PADSHAPE);
      layindex =  Graph_Level("0","",1); // layer is assigned when the complex aperture is placed.
   }

   // here write the poly
   DataStruct *d = Graph_PolyStruct( layindex, 0L,FALSE);

   if (netlist && strlen(name))
   {
      d->setGraphicClass(GR_CLASS_ETCH);
      doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, name,   // this makes a "real" char *
         SA_APPEND, NULL); // x, y, rot, height
   }

   int wi = get_trackcode(widthindex, layerindex);

   if (wi < 0)
   {
      // widthindex not defined
      fprintf(ferr, "WidthIndex %d not defined in .TRACKTEC found at %ld\n", widthindex, ifp_line);
      display_error++;
      wi = 0;
   }
   Graph_Poly(NULL, wi,fill, gvoid, close);

   for (int i=0;i<polycnt;i++)
   {
      // write vertex
      p = poly_l.ElementAt(i);

      Graph_Vertex(p.x,p.y, p.bulge);
   }
   polycnt = 0;

   if (padshape)
   {
      Graph_Block_Off();

      CString  pxshapename;
      pxshapename.Format("PADSHAPE_%s",pshapename);   
      Graph_Complex(cur_filenum, pxshapename, 0, pshapename, 0.0, 0.0, 0.0);
      Graph_Block_Reference(pxshapename, NULL, cur_filenum, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
   }

   return 1;
}

/****************************************************************************/
/* find_viatec_norule
*/
// Find the index of via aperture when the rule is not defined. The viaIndex is the via number in cxf file.  - Joanne

static int find_viatec_norule(int viaIndex)
{
   int   i;

   for (i=0;i<viateccnt;i++)
   {
      if (viatecarray[i]->viaindex == viaIndex)
         return i;
   }

   return -1;
}

/****************************************************************************/
/* find_viatec_rule
*/

// Find the index of via aperture with the rule defined.  It checks for the same index and the same rule and returns the 
// index.  The index number will be used to find the aperture by name. - Joanne

static int find_viatec_rule(int viaIndex, const char *rule)
{

   for (int i=0;i<viateccnt;i++)
   {
      if (viatecarray[i]->rulename.CompareNoCase(rule) == 0 &&
         viatecarray[i]->viaindex == viaIndex)
         return i;
   }

   return -1;
}

static CString find_viatec_rule_name_for_layer(int layerindex)
{
   // Need to find the viaTec rule to get the right aperture - Joanne
   for (int i = 0; i < layercnt; i++)
   {
      if (layerarray[i]->camcad_layerindex == layerindex)
      {
         return layerarray[i]->rule;
      }
   }
   return "";
}

/****************************************************************************/
/*
   ..VIA%d is the aperture
   .VIA%d is the padstack containing the aperture on PADALL
*/
static int cxf_via()
{
   double   x, y;
   int      drill;

   if (!get_tok())   return p_error();
   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "Via Error");
      return -1;
   }

   if (!get_tok())   return p_error(); // viatype
   int viatype = atoi(token);

   CString  layersetStr;
   cxf_layerset(&layersetStr);

   if (!get_tok())   return p_error();
   if (!STRCMPI(token,".FLAGS"))
   {
      if (!get_tok())   return p_error(); // [

      while (TRUE)
      {
         if (!get_tok())   return p_error(); // ]
         if (!STRCMPI(token,"]"))   break;

         // flags
      }
   }
   else
   {
      ErrorMessage(".FLAGS expected", "VIA record");
   }

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error(); // :
   if (STRCMPI(token,":"))
   {
      ErrorMessage(": expected", "Via Error");
      return -1;
   }
   if (!get_tok())   return p_error();
   y = cnv_unit(token);

   if (!get_tok())   return p_error(); // drill
   drill = atoi(token);

   if (!get_tok())   return p_error(); // ]

   // find the right index from viatecarray with rule first, viatype is actually viaindex in viatearray - Joanne

   CSupString layersetSup(layersetStr);
   CStringArray layersetAry;
   layersetSup.ParseWhite(layersetAry);

   for (int i = 0; i < layersetAry.GetCount(); i++)
   {
      CString layerStr = layersetAry.GetAt(i);
      
      int layerindex = Graph_Level(layerStr, "", 0);
      CString ViaRule = find_viatec_rule_name_for_layer(layerindex);
      int viatecptr = find_viatec_rule(viatype, ViaRule); 
      if (viatecptr < 0)
         viatecptr = find_viatec_norule(viatype);

      if (viatecptr > -1)
      {
         CString viashapename;
         viashapename.Format("..VIA%d", viatecptr);
         Graph_Block_Reference(viashapename, NULL, cur_filenum, x, y, 0.0, 0, 1.0, layerindex, TRUE);
      }
   }

   if (drill > 0)
   {
      int drilllayernum = Graph_Level("DRILLHOLE","",0);
      int ptr = get_drilltype(drill);

      if (ptr > -1)  
      {
         CString  drillname;
         drillname.Format("DRILL_%d",drill);
         if (Graph_Block_Exists(doc,drillname,-1))
         {
            drilltecarray[ptr]->used = TRUE;
            Graph_Block_Reference(drillname, NULL, cur_filenum, x, y, 0.0, 0, 1.0, drilllayernum, TRUE);

         }
      }
   } 

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_text()
{
   double   x, y, rot;

   if (!get_tok())   return p_error();
   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "Text Error");
      return -1;
   }

   if (!get_tok())   return p_error(); // texttec
   int texttec = atoi(token);

   CString  layerset;
   cxf_layerset(&layerset);
   int layerindex = Graph_Level(layerset, "", 0);

   if (!get_tok())   return p_error();
   if (!STRCMPI(token,".FLAGS"))
   {
      if (!get_tok())   return p_error(); // [

      while (TRUE)
      {
         if (!get_tok())   return p_error(); // ]
         if (!STRCMPI(token,"]"))   break;

         // flags
      }
   }
   else
   {
      ErrorMessage(".FLAGS expected", "TEXT record");
   }

   if (!get_tok())   return p_error(); // board
   if (!get_tok())   return p_error(); // ?
   if (!get_tok())   return p_error(); // ?
   if (!get_tok())   return p_error(); // ?

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error(); // :
   if (STRCMPI(token,":"))
   {
      ErrorMessage(": expected", "Track Error");
      return -1;
   }
   if (!get_tok())   return p_error();
   y = cnv_unit(token);

   if (!get_tok())   return p_error(); // roation
   rot = atof(token);

   double bheight, blength;
   if (!get_tok())   return p_error(); // box height
   bheight = cnv_unit(token);
   if (!get_tok())   return p_error(); // box length
   blength = cnv_unit(token);
   if (!get_tok())   return p_error(); // prosa (optional)
   
   CString  prosa;
   prosa = "";

   if (STRCMPI(token,"]"))
   {
      prosa = token;
      // here graphtext
      if (!get_tok())   return p_error(); // ]
   }

   if (strlen(prosa))
   {
      int   prop = TRUE;
      int   textmirror = FALSE;
      double charheight = bheight * 0.8;
      double charwidth = blength / strlen(prosa) * 0.6;
      double xoff, yoff, xx, yy;

      xoff = (blength - charwidth * strlen(prosa)) /2;
      yoff = 0;
      Rotate(xoff, yoff, rot, &xx, &yy);

      DataStruct *d = Graph_Text(layerindex, prosa, x+xx, y+yy, charheight , charwidth, DegToRad(rot),
               0L, prop, 
               textmirror,  
               0, FALSE, 0 /* pen width is small width */,0); // prop flag, mirror flag, oblique angle in deg
   }

/*
   // draw text box for debug
   CString ll;
   ll.Format("TEXTBOX %s", layerset);
   layerindex = Graph_Level(ll, "", 0);

   DataStruct *d = Graph_PolyStruct(layerindex,0L, FALSE);  // poly is always with 0
   Graph_Poly(NULL,0, 0,0, TRUE);
   Graph_Vertex(x, y, 0);
   Graph_Vertex(x+blength, y, 0);
   Graph_Vertex(x+blength, y+bheight, 0);
   Graph_Vertex(x, y+bheight, 0);
   Graph_Vertex(x, y, 0);
*/
   return 1;
}

/****************************************************************************/
/*
*/
static int is_etch(CString graname)
{
	if (!graname.IsEmpty())
	{
		BlockStruct *block = Graph_Block_Exists(doc, graname, cur_filenum);

		if (block){
			POSITION pos = block->getHeadDataPosition();
			while (pos)
			{
				DataStruct *data = block->getNextData(pos);

				if (data->getGraphicClass() == graphicClassEtch)
				{
					return true;
				}
			}
		}
	}

	return false;
}

/****************************************************************************/
/*
*/
static void etch_set_netname(BlockStruct *block, CString netname)
{
	if (block && !netname.IsEmpty())
	{
		POSITION pos = block->getHeadDataPosition();
		while (pos)
		{
			DataStruct *data = block->getNextData(pos);

			if (data->getGraphicClass() == graphicClassEtch)
			{
				doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_NETNAME, netname, SA_OVERWRITE, NULL);
			}
		}
	}
}

/****************************************************************************/
/*
*/
static int cxf_anchor(const char *netname, int via)
{
   double   x,y;
   
   if (!get_tok())   return p_error(); // layer

   if (!get_tok())   return p_error(); // x
   x = cnv_unit(token);

   if (!get_tok())   return p_error(); // :
   if (!get_tok())   return p_error(); // y
   y = cnv_unit(token);

   if (!get_tok())   return p_error();

   int   pg_flag = FALSE;
   int   ps_flag = FALSE;

   if (!STRCMPI(token,".FLAGS"))
   {
      if (!get_tok())   return p_error(); // [

      while (TRUE)
      {
         if (!get_tok())   return p_error(); // ]
         if (!STRCMPI(token,"]"))   break;
         
         // flags
         if (!STRCMPI(token,"PG"))
            pg_flag = TRUE;
         if (!STRCMPI(token,"PS"))
            ps_flag = TRUE;
      }
   }
   else
   {
      ErrorMessage(".FLAGS expected", "TRACK record");
   }

   CString  gramac;
   gramac = "";

   if (pg_flag)
   {
      if (!get_tok())   return p_error(); // ??
      if (!get_tok())   return p_error(); // ??

      // there can be 1 or 2 .GRAMAC
      //  .GRAMAC 'STDN88';    .GRAMAC 'STDN88';  [  *PHY ] 
      if (!get_tok())   return p_error(); // .GRAMAC

      if (!STRCMPI(token, ".GRAMAC"))
      {
         if (!get_tok())   return p_error(); // gramac name
         gramac = token;
         if (!get_tok())   return p_error(); // ;
      }
      else
      {
         push_tok();
      }

      if (!get_tok())   return p_error(); // get next char

      if (!STRCMPI(token, ".GRAMAC"))
      {
         if (!get_tok())   return p_error(); // gramac name
         gramac = token;
         if (!get_tok())   return p_error(); // ;
      }
      else
      {
         push_tok();
      }

      CString  layerset;
      cxf_layerset(&layerset);
   }

   if (strlen(gramac))
   {
      CString  graname;
      graname.Format("%s_%d", gramac, 4);

		int etch = is_etch(graname);

      if (etch || (!via && ps_flag)) // this is a pseudo terminal, not a real one.
      {
         BlockStruct *block = Graph_Block_Exists(doc, graname, cur_filenum);
         if (block)
         {
				if (etch && strlen(netname))
				{
					etch_set_netname(block, netname);
				}
            Graph_Block_Copy(block, x, y, 0.0, 0, 1.0, -1, FALSE);
         }
      }
      else
      {
         DataStruct *d = Graph_Block_Reference(graname, NULL, cur_filenum, x, y,
                           DegToRad(0), 0 , 1.0,Graph_Level("0","",1), TRUE);

         if (via)
         {
            d->getInsert()->setInsertType(insertTypeVia);
            viaInsertCount++;
            CString refname;
            refname.Format("Via_%d", viaInsertCount);
            d->getInsert()->setRefname(refname);
         }
#ifdef DEBUG_CXF_ANCHOR
         else
         {
            // For debug, or maybe it would be nice to have, so far no test data has gotten into here
            unkInsertCount++;
            CString refname;
            refname.Format("UNKNOWN_%d", unkInsertCount);
            d->getInsert()->setRefname(refname);
         }
#endif

         if (strlen(netname))
         {
            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, netname, SA_OVERWRITE, NULL); //  
         }
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_netlist_pcbsubtree(const char *netname)
{
   int   brk_cnt = 0;

   if (!get_tok())   return p_error(); // ?? subtree number 

   if (!get_tok())   return p_error(); // width
   if (!STRCMPI(token,".WIDTH"))
   {
      if (!get_tok())   return p_error(); // nominal
      if (!get_tok())   return p_error(); // maximum
   }
   else
   {
      ErrorMessage(".WIDTH Expected", ".PCBSUBTREE");
   }

   if (!get_tok())   return p_error(); // flags
   if (!STRCMPI(token,".FLAGS"))
   {
      if (!get_tok())   return p_error(); // [

      while (TRUE)
      {
         if (!get_tok())   return p_error(); // ]
         if (!STRCMPI(token,"]"))   break;

         // flags
      }

		if (!get_tok())   return p_error(); // component name
   }
   //else
   //{
   //   ErrorMessage(".FLAGS Expected", ".PCBSUBTREE");
   //}

   CString componentname, pinname;
   componentname = token;

   if (!get_tok())   return p_error(); // 
   if (!STRCMPI(token,"["))
   {
      if (!get_tok())   return p_error(); // pinname
      pinname = token;

      // layerset
      CString  layerset;
      cxf_layerset(&layerset);

      NetStruct *n = add_net(file, netname);
      CompPinStruct *p = add_comppin(file, n, componentname, pinname);

      if (!get_tok())   return p_error(); // ] pin record close

   }
   else
   {
      ErrorMessage("[ expected","Component Pin");
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_netlist_flags()
{
   int   brk_cnt = 0;

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBNetlist Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      // hierachical [] management.
      if (!STRCMPI(token,"["))   
      {
         brk_cnt++;
         continue;
      }
      if (!STRCMPI(token,"]"))   
      {
         brk_cnt--;
         if (brk_cnt < 0)
            break;
         continue;
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int define_vias()
{  
   // check what vianame is allowed
   CString vianame;
   int i;
   CString viaRule;
   int viaIndex;

   int layernum = Graph_Level("PADALL","",0);
   LayerStruct *ll = doc->FindLayer_by_Name("PADALL");
   ll->setLayerType(LAYTYPE_PAD_ALL);

   for (i=0; i<viateccnt; i++)
   {
      vianame.Format(".VIA%d", i);
      // need viaIndex and viaRule to get the right item from viatecarray - Joanne
      viaIndex = viatecarray[i]->viaindex;
      viaRule = viatecarray[i]->rulename;
      BlockStruct *block = Graph_Block_On(GBO_APPEND,vianame,cur_filenum,0);
      block->setBlockType(BLOCKTYPE_PADSTACK);

      // first try to find the aperture with the rule defined - Joanne
      int viatecptr = find_viatec_rule(viaIndex, viaRule);
      if (viatecptr < 0)
         viatecptr = find_viatec_norule(viaIndex);

      if (viatecptr > -1)
      {
#ifdef _DEBUG
      CXFViaTec *vv = viatecarray[viatecptr];
#endif
         if (viatecarray[viatecptr]->width > 0)
         {
            int      err;
            CString  viashapename;

            CString viarefname;
            viarefname.Format("Via_%d__%s_%d_", i, viaRule, viaIndex);

            viashapename.Format("..VIA%d", viatecptr);
            Graph_Aperture(cur_filenum, viashapename, viatecarray[viatecptr]->appform, viatecarray[viatecptr]->width , 
               0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

            layernum = Graph_Level("PADALL","",0);
            Graph_Block_Reference(viashapename, viarefname, cur_filenum, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
         }
      }
      Graph_Block_Off();
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_pcbnetlist()
{
   int      brk_count = 0;

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBNetlist Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   // end of netlist section

      if (token_name)
      {
         CString  netname;
         int      netindex;
         // netname
         netname = token;
         // index number
         if (!get_tok())   return p_error();
         netindex = atoi(token);

         if (!get_tok())   return p_error();

         // now [] flags
         if (!STRCMPI(token,"["))
         {
            // do flags
            cxf_netlist_flags();    
         }

         // should not be done for no pin nets
         //NetStruct *n = add_net(file,netname);

         while (TRUE)
         {
            if (!get_tok())   return p_error();
            // ends with a ;
            if (!STRCMPI(token,";"))   break;   // end of a single net section

            // .pcbsubtree
            if (!STRCMPI(token,".PCBSUBTREE"))
            {
               cxf_netlist_pcbsubtree(netname);
            }
            else
            if (!STRCMPI(token,".TRACK"))
            {
               cxf_track(1, netname, 0);
            }
            else
            if (!STRCMPI(token,".ANCHOR"))
            {
               cxf_anchor(netname, 1);
            }
            else
            {
               CString tmp;
               tmp.Format("Unknown token [%s] at %ld", token, ifp_line);
               ErrorMessage(tmp, "Error Netlist");
               // layout elements
            }
         }
      }
      else
      if (!STRCMPI(token,".UNIT"))
      {
         cxf_unit();    
      }
      else
      {
int r = 0;
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_pcbmaclib()
{
   int      brk_count = 0;
   CString  macname;
   int      poolnumber = 0;

   macname = "";
   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBmaclib Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   // end of netlist section

      if (token[0] == ';')
      {
         // end of a mac
      }
      else
      if (token[0] != '.')
      {
         CString mname;
         mname = token;

         if (!get_tok())   return p_error(); // poolnumber 2 = component graphic, 4 = terminal, 6 = layer plan
         poolnumber = atoi(token);

         macname.Format("%s_%d", mname, poolnumber);     // macname can be the same as a loclib name
         BlockStruct *b = Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         b->setOriginalName(mname);
         if (poolnumber == 4)
            b->setBlockType(BLOCKTYPE_PADSTACK);
         Graph_Block_Off();
      }
      else
      if (!STRCMPI(token,".UNIT"))
      {
         cxf_unit();    
      }
      else
      if (!STRCMPI(token,".TRACK"))
      {
         Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         cxf_track(0, macname, (poolnumber == 4)); 
         Graph_Block_Off();
      }
      else
      if (!STRCMPI(token,".VIA"))
      {
         Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         cxf_via();  
         Graph_Block_Off();
      }
      else
      if (!STRCMPI(token,".TEXT"))
      {
         Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         cxf_text(); 
         Graph_Block_Off();
      }
      else
      if (!STRCMPI(token,".KEEPIN"))
      {
         Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         cxf_keepin();
         Graph_Block_Off();
      }
      else
      if (!STRCMPI(token,".KEEPOUT"))
      {
         Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         cxf_keepout();
         Graph_Block_Off();
      }
      else
      if (!STRCMPI(token,".CLIP"))
      {
         Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         cxf_clip();
         Graph_Block_Off();
      }
      else
      if (!STRCMPI(token,".FILL"))
      {
         Graph_Block_On(GBO_APPEND,macname, cur_filenum, 0);
         cxf_fill();
         Graph_Block_Off();

			// If the token is "." then push back
			if (strlen(token) && token[0] == '.')
				push_tok();
      }
      else
      if (!STRCMPI(token,".FLAGS"))
      {
         if (!get_tok())   return p_error(); // [

         while (TRUE)
         {
            if (!get_tok())   return p_error(); // ]
            if (!STRCMPI(token,"]"))   break;

            // flags
         }
      }
      else
      {
         CString tmp;
         tmp.Format("Unknown token [%s] at %ld", token, ifp_line);
         ErrorMessage(tmp, "Error Maclib");
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_drilltec()
{

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBRule Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   //  
      int drilltype = atoi(token);
      if (!get_tok())   return p_error();
      double drilldiameter = cnv_unit(token);
      if (!get_tok())   return p_error();
      double annularring = cnv_unit(token);
      if (!get_tok())   return p_error();
      double depth = cnv_unit(token);

      if (!get_tok())   return p_error();

      if (!STRCMPI(token,".FLAGS"))
      {
         if (!get_tok())   return p_error(); // [

         while (TRUE)
         {
            if (!get_tok())   return p_error(); // ]
            if (!STRCMPI(token,"]"))   break;

            // flags
         }
      }
      
      if (!get_tok())   return p_error(); // ;
      
      CXFDrillTec *c = new CXFDrillTec;
      drilltecarray.SetAtGrow(drillteccnt,c);  
      drillteccnt++;
      c->drillindex = drilltype;
      c->diameter = drilldiameter;
      c->used = FALSE;

      if (drilldiameter > 0)
      {
         CString  drillname;
         drillname.Format("DRILL_%d",drilltype);
         Graph_Tool(drillname, 0, drilldiameter, 0, 0, 0, 0L);
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_viatec(const char *rule)
{

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBRule Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   //  
      int viatype = atoi(token);
      if (!get_tok())   return p_error();
      double width = cnv_unit(token);
      if (!get_tok())   return p_error(); // inner rectangle
      double rect = cnv_unit(token);

      int   appform = T_ROUND;

      if (width > 0)
      {
         if ((rect / width) < PADSHAPE_RADIUS)
            appform = T_SQUARE;     
      }

      if (!get_tok())   return p_error(); // add. clearance
      if (!get_tok())   return p_error(); // shift x
      if (!get_tok())   return p_error(); // shift y
      if (!get_tok())   return p_error(); // drilltype
      int drilltype = atoi(token);
      if (!get_tok())   return p_error();

      if (!STRCMPI(token,".FLAGS"))
      {
         if (!get_tok())   return p_error(); // [

         while (TRUE)
         {
            if (!get_tok())   return p_error(); // ]
            if (!STRCMPI(token,"]"))   break;

            // flags
         }
      }
      
      if (!get_tok())   return p_error(); // ;
      
      CXFViaTec   *c = new CXFViaTec;
      viatecarray.SetAtGrow(viateccnt,c);  
      viateccnt++;
      c->rulename = rule;
      c->viaindex = viatype;
      c->drillindex = drilltype;
      c->width = width;
      c->appform = appform;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_tracktec(const char *rule)
{

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBRule Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   //  
      int tracktype = atoi(token);
      if (!get_tok())   return p_error();
      double width = cnv_unit(token);
      if (!get_tok())   return p_error(); // inner rectangle
      double rect = cnv_unit(token);

      int   appform = T_ROUND;
      
      if (width > 0)
      {
         if ((rect / width) < PADSHAPE_RADIUS)
            appform = T_SQUARE;
      }

      if (!get_tok())   return p_error(); // add. clearance
      if (!get_tok())   return p_error(); // shift x
      if (!get_tok())   return p_error(); // shift y
      if (!get_tok())   return p_error(); // viatype

      if (!get_tok())   return p_error();

      if (!STRCMPI(token,".FLAGS"))
      {
         if (!get_tok())   return p_error(); // [

         while (TRUE)
         {
            if (!get_tok())   return p_error(); // ]
            if (!STRCMPI(token,"]"))   break;

            // flags
         }
      }
      
      if (!get_tok())   return p_error(); // ;
      
      CXFTrackTec *c = new CXFTrackTec;
      tracktecarray.SetAtGrow(trackteccnt,c);  
      trackteccnt++;
      c->rulename = rule;
      c->trackindex = tracktype;
      c->appform = appform;
      c->width = width;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_pcbtec()
{
   CString  technologyname;

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBRule Error");
      return -1;
   }

   if (!get_tok())   return p_error();
   technologyname = token;

   if (!get_tok())   return p_error(); // routing grid
   if (!get_tok())   return p_error(); // clearance
   
   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
  
      if (!STRCMPI(token,"]"))   break;   //  
      
      if (!STRCMPI(token,".VIATEC"))
      {
         cxf_viatec(technologyname);
      }
      else
      if (!STRCMPI(token,".TRACKTEC"))
      {
         cxf_tracktec(technologyname);
      }
      else
      if (!STRCMPI(token,".FLAGS"))
      {
         if (!get_tok())   return p_error(); // [

         while (TRUE)
         {
            if (!get_tok())   return p_error(); // ]
            if (!STRCMPI(token,"]"))   break;
            // flags
         }
      } // else
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_texttec( )
{

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBRule Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   //  
      int texttype = atoi(token);

      if (!get_tok())   return p_error(); // fontname
      if (!get_tok())   return p_error(); // height
      double height = cnv_unit(token);

      if (!get_tok())   return p_error(); // additional clearance
      if (!get_tok())   return p_error(); // linewidth
      if (!get_tok())   return p_error(); // xdist. between chars
      if (!get_tok())   return p_error(); // ydist. between chars
      if (!get_tok())   return p_error(); // plot aperture number
      if (!get_tok())   return p_error();

      if (!STRCMPI(token,".FLAGS"))
      {
         if (!get_tok())   return p_error(); // [

         while (TRUE)
         {
            if (!get_tok())   return p_error(); // ]
            if (!STRCMPI(token,"]"))   break;

            // flags
         }
      }
      
      if (!get_tok())   return p_error(); // ;
      
      CXFTextTec  *c = new CXFTextTec;
      texttecarray.SetAtGrow(textteccnt,c);  
      textteccnt++;
      c->textindex = texttype;
      c->height = height;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_pcbrule()
{
   int      brk_count = 0;


   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBRule Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   //  

      if (!STRCMPI(token,".DRILLTEC"))
      {
         cxf_drilltec( );
      }
      else
      if (!STRCMPI(token,".TEXTTEC"))
      {
         cxf_texttec( );
      }
      else
      if (!STRCMPI(token,".PCBTEC"))
      {
         cxf_pcbtec( );
      }
      else
      if (!STRCMPI(token,".UNIT"))
      {
         cxf_unit();    
      }
      else
      {
         CString tmp;
         tmp.Format("Unknown token [%s] at %ld", token, ifp_line);
         ErrorMessage(tmp, "Error LocLib");
      }
   }

   define_vias();
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_pcblayerplan()
{
   int      brk_count = 0;

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBLayerPlan Error");
      return -1;
   }

   int sigcnt = 0;

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   //  

      if (!STRCMPI(token,".LAYER"))
      {

         if (!get_tok())   return p_error();
         int layerindex = atoi(token);

         int l = Graph_Level(token,"",0);
         LayerStruct *ll = doc->getLayerArray()[l];

         if (!get_tok())   return p_error();
         ll->setComment(token);

         bool com = false, sol = false, sig = false, drl = false, sha = false, res = false, pas = false, pla = false;
         if (!get_tok())   return p_error(); // optional flags
         if (!STRCMPI(token,".FLAGS"))
         {
            if (!get_tok())   return p_error(); // [

            while (true)
            {
               if (!get_tok())   return p_error(); // ]
               if (!STRCMPI(token,"]"))   break;

               // flags
               if (!STRCMPI(token,"PLA")) 
                  pla = true;
               if (!STRCMPI(token,"SIG")) 
                  sig = true;
               if (!STRCMPI(token,"COM")) 
                  com = true;
               if (!STRCMPI(token,"SOL")) 
                  sol = true;
               if (!STRCMPI(token,"SHA")) 
                  sha = true;
               if (!STRCMPI(token,"DRL")) 
                  drl = true;
               if (!STRCMPI(token,"PAS")) 
                  pas = true;
               if (!STRCMPI(token,"RES")) 
                  res = true;
            }
         }

         if (sig)
         {
            ll->setElectricalStackNumber(++sigcnt);
            if (com)
               ll->setLayerType(LAYTYPE_SIGNAL_TOP);
            else
            if (sol)
               ll->setLayerType(LAYTYPE_SIGNAL_BOT);
            else
            if (pla)
               ll->setLayerType(LAYTYPE_POWERNEG);
            else
               ll->setLayerType(LAYTYPE_SIGNAL_INNER);
         }
         else
         {
            if (res)
               ll->setLayerType(LAYTYPE_MASK_ALL);
            if (drl)
               ll->setLayerType(LAYTYPE_DRILL);
            if (sha)
               ll->setLayerType(LAYTYPE_COMPONENTOUTLINE);
            if (pas)
               ll->setLayerType(LAYTYPE_PASTE_ALL);

         }

         if (!get_tok())   return p_error(); // copper height
         if (!get_tok())   return p_error(); // substract height

         CString pcbtecname;
         if (!get_tok())   return p_error(); // pcbtec name
         pcbtecname = token;

         if (!get_tok())   return p_error(); // optional planesignals

         if (STRCMPI(token,";")) // if not ;
         {
            while (TRUE)
            {
               if (!get_tok())   return p_error(); //
               if (!STRCMPI(token,";"))   break;
            }
         }

         CXFLayer *c = new CXFLayer;
         layerarray.SetAtGrow(layercnt,c);  
         layercnt++;
         c->cxf_layernum = layerindex;
         c->camcad_layerindex = l;
         c->rule = pcbtecname;
         c->isElectrical = sig;
      }
      else
      if (!STRCMPI(token,".SWAP"))
      {
         CString lay1, lay2;
         if (!get_tok())   return p_error(); // index 1
         lay1 = token;
         if (!get_tok())   return p_error(); // index 2
         lay2 = token;

         Graph_Level_Mirror(lay1, lay2, "");

         if (!get_tok())   return p_error(); // ;
      }
      else
      if (!STRCMPI(token,".ROUTPAIR"))
      {
         while (TRUE)
         {
            if (!get_tok())   return p_error();
   
            if (!STRCMPI(token,";"))   break;   //  
         }
      }
      else
      if (!STRCMPI(token,".DOCUMAP"))
      {
         CString lay1, lay2;
         if (!get_tok())   return p_error(); // index 1
         lay1 = token;
         if (!get_tok())   return p_error(); // index 2
         lay2 = token;

         LayerStruct *ll = doc->FindLayer_by_Name(lay1);
         if (ll->getLayerType() == LAYTYPE_MASK_ALL) // 1 = solder, 2 = component
         {
            if (!STRCMPI(lay2,"1")) ll->setLayerType(LAYTYPE_MASK_BOTTOM);
            if (!STRCMPI(lay2,"2")) ll->setLayerType(LAYTYPE_MASK_TOP);
         }
         if (ll->getLayerType() == LAYTYPE_PASTE_ALL)
         {
            if (!STRCMPI(lay2,"1")) ll->setLayerType(LAYTYPE_PASTE_BOTTOM);
            if (!STRCMPI(lay2,"2")) ll->setLayerType(LAYTYPE_PASTE_TOP);
         }

         if (STRCMPI(token,";")) // if not ;
         {
            while (TRUE)
            {
               if (!get_tok())   return p_error(); //
               if (!STRCMPI(token,";"))   break;
            }
         }
      }
      else
      {
         CString tmp;
         tmp.Format("Unknown token [%s] at %ld", token, ifp_line);
         ErrorMessage(tmp, "Error PCBLayerPlan");
      }
   }
   //define_vias();

   return 1;
}

/****************************************************************************/
/*
*/
static void Padstack_Copy(BlockStruct *block, int top) // 0 = top, 1 = bottom
{
   if (block == NULL)   return;

   DataStruct *data, *copy;
   CDataList *CurrentDataList = GetCurrentDataList();

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      data = block->getDataList().GetNext(pos);
      if (top == 1) // this is bottom
      {
         // do not copy *PHY layer, already done in top=0
         if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_PAD_ALL)
            continue;
         if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_MASK_ALL)
            continue;
         if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_DRILL)
            continue;
         if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_PASTE_ALL)
            continue;
         if (data->getInsert()->getInsertType() == INSERTTYPE_TOOLING) // copy only 1 drill
            continue;
      }

      copy = doc->getCamCadData().getNewDataStruct(*data,false);

      if (!doc->getLayerArray()[data->getLayerIndex()]->getName().CompareNoCase("*STL"))
      {
         if (top == 0)  // on top
         {
            int layernr = Graph_Level("SMD_TOP","",0);
            LayerStruct *ll = doc->FindLayer(layernr);
            ll->setLayerType(LAYTYPE_PAD_TOP);
            ll->setFlagBits(LY_NEVERMIRROR); 
            ll->setComment("Pad Top");
            copy->setLayerIndex(layernr);
         }
         else
         {
            int layernr = Graph_Level("SMD_BOT","",0);
            LayerStruct *ll = doc->FindLayer(layernr);
            ll->setLayerType(LAYTYPE_PAD_BOTTOM);
            ll->setFlagBits(LY_MIRRORONLY); 
            ll->setComment("Pad Bottom");
            copy->setLayerIndex(layernr);
         }
      }
      else
      if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_SIGNAL_TOP)
      {
         int layernr = Graph_Level("SMD_TOP","",0);
         LayerStruct *ll = doc->FindLayer(layernr);
         ll->setLayerType(LAYTYPE_PAD_TOP);
         ll->setFlagBits(LY_NEVERMIRROR); 
         ll->setComment("Pad Top");
         ll->setElectricalStackNumber(doc->getLayerArray()[copy->getLayerIndex()]->getElectricalStackNumber());
         copy->setLayerIndex(layernr);
      }
      else
      if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_SIGNAL_BOT)
      {
         int layernr = Graph_Level("SMD_BOT","",0);
         LayerStruct *ll = doc->FindLayer(layernr);
         ll->setLayerType(LAYTYPE_PAD_BOTTOM);
         ll->setFlagBits(LY_MIRRORONLY); 
         ll->setComment("Pad Bottom");
         ll->setElectricalStackNumber(doc->getLayerArray()[copy->getLayerIndex()]->getElectricalStackNumber());
         copy->setLayerIndex(layernr);
      }
      else
      if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_MASK_TOP)
      {
         int layernr = Graph_Level("MASK_TOP","",0);
         LayerStruct *ll = doc->FindLayer(layernr);
         ll->setLayerType(LAYTYPE_MASK_TOP);
         ll->setFlagBits(LY_NEVERMIRROR); 
         ll->setComment("Mask Top");
         copy->setLayerIndex(layernr);
      }
      else
      if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_MASK_BOTTOM)
      {
         int layernr = Graph_Level("MASK_BOT","",0);
         LayerStruct *ll = doc->FindLayer(layernr);
         ll->setLayerType(LAYTYPE_MASK_BOTTOM);
         ll->setFlagBits(LY_MIRRORONLY); 
         ll->setComment("Mask Bottom");
         copy->setLayerIndex(layernr);
      }
      else
      if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_PASTE_TOP)
      {
         int layernr = Graph_Level("PASTE_TOP","",0);
         LayerStruct *ll = doc->FindLayer(layernr);
         ll->setLayerType(LAYTYPE_PASTE_TOP);
         ll->setFlagBits(LY_NEVERMIRROR); 
         ll->setComment("Paste Top");
         copy->setLayerIndex(layernr);
      }
      else
      if (doc->getLayerArray()[data->getLayerIndex()]->getLayerType() == LAYTYPE_PASTE_BOTTOM)
      {
         int layernr = Graph_Level("PASTE_BOT","",0);
         LayerStruct *ll = doc->FindLayer(layernr);
         ll->setLayerType(LAYTYPE_PASTE_BOTTOM);
         ll->setFlagBits(LY_MIRRORONLY); 
         ll->setComment("Paste Bottom");
         copy->setLayerIndex(layernr);
      }

      CurrentDataList->AddTail(copy);
   }
   return;
}

/****************************************************************************/
/*
*/
static CString combine_padstack(const char *toppadstack, const char *botpadstack)
{
   CString  newpadstack;

   // if the padstacks are the same, use one 
   if (!STRCMPI(toppadstack, botpadstack))
   {
      newpadstack = toppadstack;
   }
   else
   {
      // copy both into one and 
      newpadstack.Format("%s_%s", toppadstack, botpadstack);
      BlockStruct *b, *block = Graph_Block_Exists(doc, newpadstack, cur_filenum);
      if (!block)
      {
         // here do the merge
         // every layer must be renamed to TOP and BOT ... 
         BlockStruct *bb = Graph_Block_On(GBO_APPEND,newpadstack, cur_filenum, 0);
         bb->setBlockType(BLOCKTYPE_PADSTACK);
         if (b = Graph_Block_Exists(doc, toppadstack, cur_filenum))
            Padstack_Copy(b, 0); // top
         if (b = Graph_Block_Exists(doc, botpadstack, cur_filenum))
            Padstack_Copy(b, 1); // bottom
         Graph_Block_Off();
      }
   }
   return newpadstack;
}

/****************************************************************************/
/*
*/
static int cxf_pcbloclib()
{
   int      brk_count = 0;
   CString  shapename;

   shapename = "";
   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBLoclib Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   // end of netlist section

      if (!STRCMPI(token,".FORMAT"))
      {
         if (!get_tok())   return p_error(); // shapename
         shapename = token;

         if (!get_tok())   return p_error(); // layernumber

         // shapesize
         double xsize, ysize, xrot, yrot;
         if (!get_tok())   return p_error(); // x
         xsize = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (!get_tok())   return p_error(); // y
         ysize = cnv_unit(token);

         // rotation point
         if (!get_tok())   return p_error(); // x
         xrot = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (!get_tok())   return p_error(); // y
         yrot = cnv_unit(token);

         Graph_Block_On(GBO_APPEND,shapename, cur_filenum, 0);

         DataStruct *d = Graph_PolyStruct(Graph_Level("COMPOUTLINE_TOP", "", 1),0L, FALSE);  // poly is always with 0
         Graph_Poly(NULL,0, 0,0, TRUE);
         Graph_Vertex(-xrot, -yrot, 0);
         Graph_Vertex(-xrot+xsize, -yrot, 0);
         Graph_Vertex(-xrot+xsize, -yrot+ysize, 0);
         Graph_Vertex(-xrot, -yrot+ysize, 0);
         Graph_Vertex(-xrot, -yrot, 0);
         Graph_Block_Off();

         // always flags
         if (!get_tok())   return p_error(); // optional flags
         if (!STRCMPI(token,".FLAGS"))
         {
            if (!get_tok())   return p_error(); // [

            while (TRUE)
            {
               if (!get_tok())   return p_error(); // ]
               if (!STRCMPI(token,"]"))   break;

               // flags
            }
         }
         else
         {
            push_tok();
         }

         // always 2 gramacs
         if (!get_tok())   return p_error(); // gramac top
         if (!STRCMPI(token,".GRAMAC"))   // like on terminals, there are 2 GRAMAC definition possible, 
                                       // the first one for top and the second for bottom placement.
         {
            CString gramac;

            if (!get_tok())   return p_error();    // name
            gramac.Format("%s_2", token); // macname can be the same as a loclib name
            if (!get_tok())   return p_error();    // ;

            // epxlode the contens of the macro stuff ! This keeps it more inline with other CAD translators.
            BlockStruct *block = Graph_Block_Exists(doc, gramac, cur_filenum);

            if (block)
            {
               Graph_Block_On(GBO_APPEND,shapename, cur_filenum, 0);
               Graph_Block_Copy(block, 0, 0, 0, 0, 1, 0, TRUE);
               Graph_Block_Off();
            }
         }
         else
         {
            push_tok();
         }

         if (!get_tok())   return p_error(); // gramac bottom
         if (!STRCMPI(token,".GRAMAC"))
         {
            CString gramac;

            if (!get_tok())   return p_error();    // name
            gramac.Format("%s_2", token); // macname can be the same as a loclib name
            if (!get_tok())   return p_error();    // ;
            // do nothing
         }
         else
         {
            push_tok();
         }

      }
      else
      if (!STRCMPI(token,".UNIT"))
      {
         cxf_unit();    
      }
      else
      if (!STRCMPI(token,".TERMINAL"))
      {
         // set this block to a pcb component if it has a terminal (pin)
         BlockStruct *b = Graph_Block_On(GBO_APPEND,shapename, cur_filenum, 0);
         Graph_Block_Off();
         b->setBlockType(BLOCKTYPE_PCBCOMPONENT);

         CString  pinname;
         if (!get_tok())   return p_error();    // name 
         pinname = token;
         if (!get_tok())   return p_error();    // layer 

         // pos
         double x, y, rot;
         if (!get_tok())   return p_error(); // x
         x = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (!get_tok())   return p_error(); // y
         y = cnv_unit(token);

         if (!get_tok())   return p_error(); // y
         rot = atof(token);

         if (!get_tok())   return p_error(); // optional flags
         if (!STRCMPI(token,".FLAGS"))
         {
            if (!get_tok())   return p_error(); // [

            while (TRUE)
            {
               if (!get_tok())   return p_error(); // ]
               if (!STRCMPI(token,"]"))   break;

               // flags
            }
         }
         else
         {
            push_tok();
         }

         CString  toppadstack, botpadstack;
         toppadstack = "";
         botpadstack = "";
         if (!get_tok())   return p_error(); // gramac primary
         if (!STRCMPI(token,".GRAMAC"))
         {
            if (!get_tok())   return p_error(); // name
            toppadstack.Format("%s_4", token);  // macname can be the same as a loclib name
            botpadstack = toppadstack;          // set bottom and top the same.
            if (!get_tok())   return p_error(); // ;
         }
         else
         {
            ErrorMessage("GRAMAC 1 Expected", "FORMAT");
         }

         if (!get_tok())   return p_error(); // gramac sec (optional)
         if (!STRCMPI(token,".GRAMAC"))
         {
            if (!get_tok())   return p_error(); // name
            botpadstack.Format("%s_4", token);  // macname can be the same as a loclib name
            if (!get_tok())   return p_error(); // ;
         }
         else
         {
            push_tok();
         }

         // here we have toppadstack and botpadstack
         CString newpadstack;
         newpadstack = combine_padstack(toppadstack, botpadstack);
         // here is a padstack
         Graph_Block_On(GBO_APPEND,shapename, cur_filenum, 0);
         DataStruct *d = Graph_Block_Reference(newpadstack, pinname, cur_filenum, 
            x,y,DegToRad(rot), 0 , 1.0, Graph_Level("0","",1), TRUE);
         d->getInsert()->setInsertType(insertTypePin);
         Graph_Block_Off();
      }
      else
      {
         CString tmp;
         tmp.Format("Unknown token [%s] at %ld", token, ifp_line);
         ErrorMessage(tmp, "Error LocLib");
      }
   }
   return 1;
}

/****************************************************************************/
/*

*/
static int cxf_pcbboard()
{
   // .UNIT
   // .FRAME
   // .BOARD
   // .ROUT
   // .LAYERGRAPHIC
   // .DRILLSYMBOL
   // .ANCHOR
   // .FLAGS

   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBBoard Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   // end of board section

      if (!STRCMPI(token,".UNIT"))
      {
         cxf_unit();    
      }
      else
      if (!STRCMPI(token,".FRAME"))
      {
         // xy frame size
         double x, y;

         if (!get_tok())   return p_error();
         x = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (STRCMPI(token,":"))
         {
            ErrorMessage(": expected", "Frame Size Error");
            return -1;
         }
         if (!get_tok())   return p_error();
         y = cnv_unit(token);

         // 
         int layerindex = Graph_Level("FRAME", "", 0);
         DataStruct *d = Graph_PolyStruct( layerindex, 0L,FALSE);
         Graph_Poly(NULL, 0, 0, 0, TRUE);
         Graph_Vertex(0.0, 0.0, 0.0);
         Graph_Vertex(x, 0.0, 0.0);
         Graph_Vertex(x, y, 0.0);
         Graph_Vertex(0, y, 0.0);
         Graph_Vertex(0.0, 0.0, 0.0);

         if (!get_tok())   return p_error(); // get ;

      }
      else
      if (!STRCMPI(token,".BOARD"))
      {
         // board lower left, upper right
         double x1, y1, x2, y2;

         if (!get_tok())   return p_error();
         x1 = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (STRCMPI(token,":"))
         {
            ErrorMessage(": expected", "Board Error");
            return -1;
         }
         if (!get_tok())   return p_error();
         y1 = cnv_unit(token);

         if (!get_tok())   return p_error(); // ,

         if (!get_tok())   return p_error();
         x2 = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (STRCMPI(token,":"))
         {
            ErrorMessage(": expected", "Board Error");
            return -1;
         }
         if (!get_tok())   return p_error();
         y2 = cnv_unit(token);

         // 
         int layerindex = Graph_Level("BOARD", "", 0);
         DataStruct *d = Graph_PolyStruct( layerindex, 0L,FALSE);
         Graph_Poly(NULL, 0, 0, 0, TRUE);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         if (!get_tok())   return p_error(); // ,

         double production_origin_x, production_origin_y;
         if (!get_tok())   return p_error();
         production_origin_x = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (STRCMPI(token,":"))
         {
            ErrorMessage(": expected", "Board Error");
            return -1;
         }
         if (!get_tok())   return p_error();
         production_origin_y = cnv_unit(token);

         if (!get_tok())   return p_error(); // board title
         if (!get_tok())   return p_error(); // board stock id

         
         if (!get_tok())   return p_error(); // gramac for board outline
         if (!STRCMPI(token, ".GRAMAC"))
         {
            if (!get_tok())   return p_error(); // gramac name
            int layerindex = Graph_Level("OUTLINE", "", 0);
            DataStruct *d = Graph_Block_Reference(token, "", cur_filenum, production_origin_x, production_origin_y, 
               DegToRad(0.0), 0 , 1.0, layerindex, TRUE);
            if (!get_tok())   return p_error(); // ;
         }

         if (!get_tok())   return p_error(); // flags
         if (!STRCMPI(token, ".FLAGS"))
         {
            if (!get_tok())   return p_error(); // [

            while (TRUE)
            {
               if (!get_tok())   return p_error(); // ]
               if (!STRCMPI(token,"]"))   break;

               // flags
            }  
            if (!get_tok())   return p_error(); // ;
         }
      }
      else
      if (!STRCMPI(token,".ROUT"))
      {
         // xy frame size
         double x1, y1, x2, y2;

         if (!get_tok())   return p_error();
         x1 = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (STRCMPI(token,":"))
         {
            ErrorMessage(": expected", "Frame Size Error");
            return -1;
         }
         if (!get_tok())   return p_error();
         y1 = cnv_unit(token);

         if (!get_tok())   return p_error(); // ,

         if (!get_tok())   return p_error();
         x2 = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (STRCMPI(token,":"))
         {
            ErrorMessage(": expected", "Frame Size Error");
            return -1;
         }
         if (!get_tok())   return p_error();
         y2 = cnv_unit(token);

         // 
         int layerindex = Graph_Level("ROUT", "", 0);
         DataStruct *d = Graph_PolyStruct( layerindex, 0L,FALSE);
         Graph_Poly(NULL, 0, 0, 0, TRUE);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         if (!get_tok())   return p_error(); // get ;

      }
      else
      if (!STRCMPI(token,".LAYERGRAFIC"))
      {
         if (!get_tok())   return p_error(); // layerindex
         int layerindex = Graph_Level(token, "", 0);

         if (!get_tok())   return p_error(); // graphic type

         if (!get_tok())   return p_error(); // graphic name
         if (!STRCMPI(token,".GRAMAC"))
         {
            CString  graphic;
            if (!get_tok())   return p_error(); // graphic name
            graphic.Format("%s_6", token);   // layerplan graphics is pool 6

            BlockStruct *block = Graph_Block_Exists(doc, graphic, cur_filenum);
            if (block)
            {
               Graph_Block_Copy(block, 0.0, 0.0, 0.0, 0, 1.0, layerindex, FALSE);
            }

            //DataStruct *d = Graph_Block_Reference(graphic, "", 0, 0.0, 0.0, 0.0, 0 , 1.0, layerindex, TRUE);
         }
         if (!get_tok())   return p_error(); // get ;
      }
      else
      if (!STRCMPI(token,".DRILLSYMBOL"))
      {
         if (!get_tok())   return p_error(); // drill type

         if (!get_tok())   return p_error(); // gramac 
         if (!STRCMPI(token, ".GRAMAC"))
         {
            if (!get_tok())   return p_error(); // 
         }
         if (!get_tok())   return p_error(); // get ;
      }
      else
      if (!STRCMPI(token,".ANCHOR"))
      {
         cxf_anchor("", 0);
      }
      else
      {
int r = 0;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_gradef()
{
   if (!get_tok())   return p_error(); //

   if (STRCMPI(token,";")) // if not ;
   {
      while (TRUE)
      {
         if (!get_tok())   return p_error(); //
         if (!STRCMPI(token,";"))   break;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_pcbplacement()
{
   int      brk_count = 0;
   CString  shapename, componenttype, stockid, alternate;

   shapename = "";
   // get open bracket
   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "PCBPlacement Error");
      return -1;
   }

   // loop until closed bracket
   while (TRUE)
   {
      if (!get_tok())   return p_error();
   
      if (!STRCMPI(token,"]"))   break;   // end of netlist section

      if (token[0] != '.')
      {
         int compnametypenr = atoi(token);

         if (!get_tok())   return p_error(); // component type - shapename 
         componenttype = token;

         if (!componenttype.CompareNoCase("NOTYPE"))
            componenttype = "";
         else
         if (!componenttype.CompareNoCase("NM"))   // seems to also not be unique
            componenttype = "";

         if (!get_tok())   return p_error(); // stockid
         stockid = token;
         if (!get_tok())   return p_error(); // alternative
         alternate = token;

         if (!get_tok())   return p_error(); // shapename      
         shapename = token;
   
         if (!get_tok())   return p_error(); // .local or shapelib      

         if (!get_tok())   return p_error(); // 
         if (!STRCMPI(token,".FLAGS"))
         {
            if (!get_tok())   return p_error(); // [

            while (TRUE)
            {
               if (!get_tok())   return p_error(); // ]
               if (!STRCMPI(token,"]"))   break;

               // flags
            }  
         }
      }
      else
      if (!STRCMPI(token,".UNIT"))
      {
         cxf_unit();    
      }
      else
      if (!STRCMPI(token,".PCBLIST"))
      {
         int   iscomponent = TRUE;

         CString  compname;
         if (!get_tok())   return p_error();    // compname
         compname = token;

         if (!get_tok())   return p_error();    // index
         
         if (!get_tok())   return p_error();

         if (STRCMPI(token,"["))
         {
            ErrorMessage("[ expected", "PCBList Error");
            return -1;
         }

         //placelayer
         if (!get_tok())   return p_error(); // 
         int placelayer = atoi(token);       // 2 is component, 1 solder

         // pos
         double x, y, rot;
         if (!get_tok())   return p_error(); // x
         x = cnv_unit(token);
         if (!get_tok())   return p_error(); // :
         if (!get_tok())   return p_error(); // y
         y = cnv_unit(token);

         if (!get_tok())   return p_error(); // y
         rot = atof(token);
         if (!get_tok())   return p_error();

         if (STRCMPI(token,"]"))
         {
            ErrorMessage("] expected", "PCBPlacement Error");
            return -1;
         }

         if (!get_tok())   return p_error(); // 
         if (!STRCMPI(token,".FLAGS"))
         {
            if (!get_tok())   return p_error(); // [

            while (TRUE)
            {
               if (!get_tok())   return p_error(); // ]
               if (!STRCMPI(token,"]"))   break;
               // flags
            }  
         }

         if (!get_tok())   return p_error(); //

         if (!STRCMPI(token,".GRADEF"))
         {
            // until ;
            cxf_gradef();
            if (!get_tok())   return p_error(); // get the ; from the PCBLIST
         }

         if (!STRCMPI(token,";"))
         {
            int cur_mirror = 0;

            if (placelayer == 1) 
            {
               rot += 180;
               cur_mirror = TRUE;
            }

            rot = round(rot);
            while (rot < 0)   rot += 360;
            while (rot >=360) rot -= 360; 

            // planes are made like components
            BlockStruct *block = Graph_Block_Exists(doc, shapename, cur_filenum);
            if (block->getBlockType() == 0 && !strlen(componenttype))
               iscomponent = FALSE;

            if (!iscomponent)
            {
               Graph_Block_Copy(block, x, y, DegToRad(rot), cur_mirror, 1.0, -1, FALSE);
            }
            else
            {
               // convert 1206~1 into 1206, only convert a pcb component, not a free graphics.
               if (USE_DERIVEDSYM)
               {
                  BlockStruct *block = Graph_Block_Exists(doc, shapename, cur_filenum);
                  if (block && block->getBlockType() != 0) // if it was a graphic based block, keep the original
                  {
                     CString  tmp;
                     tmp = shapename;
                     int f = shapename.ReverseFind('~');
                     if (f > -1)
                     {
                        shapename.Delete(f,strlen(shapename)-f);
                     }  
         
                     if (!Graph_Block_Exists(doc, shapename, cur_filenum)) // if it was a graphic based block, keep the original
                     {
                        shapename = tmp;  // if this does not exist, take the original one.
                     }
                  }
               }
            
               DataStruct *d = Graph_Block_Reference(shapename, compname, cur_filenum,
                  x, y, DegToRad(rot), cur_mirror , 1.0, -1, TRUE);
               BlockStruct *b = doc->Find_Block_by_Num(d->getInsert()->getBlockNumber());
               d->getInsert()->setInsertType(default_inserttype(b->getBlockType()));

               if (strlen(componenttype) && (d->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT))
               {
                  TypeStruct *curtype = AddType(file,componenttype);

                  // here update compdef list
                  if (curtype->getBlockNumber() > -1 && curtype->getBlockNumber() != b->getBlockNumber())
                  {
                     BlockStruct *already_assigned = doc->Find_Block_by_Num(curtype->getBlockNumber());
                     fprintf(ferr,"Try to assign Pattern [%s] to Device [%s] -> already a different Pattern [%s] assigned!\n",
                        b->getName(), curtype->getName(), already_assigned->getName());
                     display_error++;

                     CString  typname;
                     typname.Format("%s_%s", componenttype, b->getName());
                     curtype = AddType(file,typname);
         
                     curtype->setBlockNumber( b->getBlockNumber());
      
                     doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TYPELISTLINK, typname,
                         SA_OVERWRITE, NULL); // x, y, rot, height
   
                  }
                  else
                  {
                     curtype->setBlockNumber( b->getBlockNumber());
                     doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TYPELISTLINK, componenttype,
                         SA_OVERWRITE, NULL); // x, y, rot, height
                  }

                  if (strlen(stockid))
                  {
                     doc->SetUnknownAttrib(&curtype->getAttributesRef(), "STOCKID", stockid,  SA_OVERWRITE, NULL); 
                  }
               }
            }
         }
         else
         {
            ErrorMessage("; expected", "PCBPlacement Error");
            return -1;
         }
      }
      else
      if (!STRCMPI(token,".CONTAINER"))
      {
         int brk_cnt = 0;

         if (!get_tok())   return p_error();

         if (STRCMPI(token,"["))
         {
            ErrorMessage("[ expected", "ADDTRMPAR Error");
            return -1;
         }

         // loop until closed bracket
         while (TRUE)
         {  
            if (!get_tok())   return p_error();
   
            // hierachical [] management.
            if (!STRCMPI(token,"["))   
            {
               brk_cnt++;
               continue;
            }
            if (!STRCMPI(token,"]"))   
            {
               brk_cnt--;
               if (brk_cnt < 0)
                  break;
               continue;
            }
            int r = 0;
         }
      }
      else
      if (!STRCMPI(token,".ADDTRMPAR"))
      {
         int brk_cnt = 0;

         if (!get_tok())   return p_error();

         if (STRCMPI(token,"["))
         {
            ErrorMessage("[ expected", "ADDTRMPAR Error");
            return -1;
         }

         // loop until closed bracket
         while (TRUE)
         {  
            if (!get_tok())   return p_error();
   
            // hierachical [] management.
            if (!STRCMPI(token,"["))   
            {
               brk_cnt++;
               continue;
            }
            if (!STRCMPI(token,"]"))   
            {
               brk_cnt--;
               if (brk_cnt < 0)
                  break;
               continue;
            }
            int r = 0;
         }
      }
      else
      {
int r = 0;
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cxf_unit()
{
   int   cxfunit;

   if (!get_tok())   return p_error();

   if (STRCMPI(token,"["))
   {
      ErrorMessage("[ expected", "Unit Error");
      return -1;
   }

   if (!get_tok())   return p_error();
   if (!STRCMPI("INCH",token))         cxfunit = 0; 
   else
   if (!STRCMPI("MM",token))           cxfunit = 1;    
   else
   {
      CString  tmp;
      tmp.Format("Unknown Unit [%s]", token);
      ErrorMessage(tmp,"Unit Error");
   }

   double   maj, min;
   if (!get_tok())   return p_error(); // maj 
   maj = atoi(token);
   if (!get_tok())   return p_error(); // min
   min = atoi(token);

   switch (cxfunit)
   {
      case  0:
         scalefactor = Units_Factor(UNIT_INCHES, pageunits) * maj / min;
      break;
      case  1:
         scalefactor = Units_Factor(UNIT_MM, pageunits) * maj / min;
      break;
   }

   if (!get_tok())   return p_error();

   if (STRCMPI(token,"]"))
   {
      ErrorMessage("] expected", "Unit Error");
      return -1;
   }

   return 1;
}

/*****************************************************************************/
/* 
   a is left side
   b is right side

   offset is the len/s plus 1 side
*/
static double offset_app(double a, double b)
{
   return a + (b - a)/2;
}

/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(List *tok_lst,int tok_size)
{
   int      i;
   CString  t = token; // only for debug

   for (i = 0; i < tok_size; ++i)
      if (!STRCMPI(token,tok_lst[i].token))
         return(i);

   return(-1);
}

/****************************************************************************/
/*
   Get a token from the input file.
*/
int get_tok()
{

   if (!Push_tok)
   {
      while (!get_next(cur_line,cur_new))
      {
         if (get_line(cur_line,MAX_LINE) < 1)
            return(FALSE);
         else
            cur_new = TRUE;
      }
      cur_new = FALSE;
   }
   else
      Push_tok = FALSE;

   return(TRUE);
}

/****************************************************************************/
/*
   Push back last token.
*/
int push_tok()
{
   return (Push_tok = TRUE);
}

/****************************************************************************/
/*
   Get the next token from line.
*/
int get_next(char *lp,int newlp)
{
   static   char  *cp;
   int      i = 0;

   token_name = FALSE;

   if (newlp)  cp = lp;

   for (; isspace(*cp) && *cp != '\0'; ++cp) ;

   switch(*cp)
   {
      case '\0':
         return 0;
      case '[':
      case ']':
         token[i++] = *(cp++);
      break;
      case '\"':
         token[i] = '\0';
         token_name = TRUE;
         for (++cp; *cp != '\"'; ++cp, ++i)
         {
            // line ended in the middle of a text
            if (*cp == '\n' || *cp == '\0')
            {
               /* text spans over 1 line */
               if (!get_line(cur_line,MAX_LINE))
               {
                  fprintf(ferr,"Error in Line Read at %ld\n", ifp_line);
                  return -1;
               }
               cp = cur_line;

               /* the quote can be the first character in the new line */
               if (*cp == '\"' && token[i] != '\\')
                  break;
               if (*cp == '\\')
               {
                  ++cp;
                  if (*cp == 'b')
                     token[i] = ' ';
                  else
                     token[i] = *cp;
               }
               else
                  token[i] = *cp;
            }
            else
            if (*cp == '\\')
            {
               ++cp;
               if (*cp == '\n')
               {
                  /* text spans over 1 line */
                  if (!get_line(cur_line,MAX_LINE))
                  {
                     fprintf(ferr,"Error in Line Read\n");
                     return -1;
                  }
                  cp = cur_line;
               }
               if (*cp == 'b')
                  token[i] = ' ';
               else
                  token[i] = *cp;
            }
            else
            if (*cp != '\0')
               token[i] = *cp;
         }
         ++cp;
      break;
      case '\'':
         token[i] = '\0';
         token_name = TRUE;
         for (++cp; *cp != '\''; ++cp, ++i)
         {
            // line ended in the middle of a text
            if (*cp == '\n' || *cp == '\0')
            {
               /* text spans over 1 line */
               if (!get_line(cur_line,MAX_LINE))
               {
                  fprintf(ferr,"Error in Line Read at %ld\n", ifp_line);
                  return -1;
               }
               cp = cur_line;

               /* the quote can be the first character in the new line */
               if (*cp == '\'' && token[i] != '\\')
                  break;
               if (*cp == '\\')
               {
                  ++cp;
                  if (*cp == 'b')
                     token[i] = ' ';
                  else
                     token[i] = *cp;
               }
               else
                  token[i] = *cp;
            }
            else
            if (*cp == '\\')
            {
               ++cp;
               if (*cp == '\n')
               {
                  /* text spans over 1 line */
                  if (!get_line(cur_line,MAX_LINE))
                  {
                     fprintf(ferr,"Error in Line Read\n");
                     return -1;
                  }
                  cp = cur_line;
               }
               if (*cp == 'b')
                  token[i] = ' ';
               else
                  token[i] = *cp;
            }
            else
            if (*cp != '\0')
               token[i] = *cp;
         }
         ++cp;
      break;
      default:
         for (; !isspace(*cp) && *cp != '\"' && *cp != '\'' &&
                           *cp != '[' && *cp != ']' &&
                           *cp != '\0'; ++cp, ++i)
         {
            if (*cp == '\\')
            {
               if (*(cp+1) == '\0' || isspace(*(cp+1)))
               {
                  fprintf(ferr,"Parse warning: additional character expected at %ld\n", ifp_line);
                  display_error++;
               }
               else
                  ++cp;
            }
            token[i] = *cp;
         }
      break;
   }

   if (!i && !token_name)
      return(FALSE);

   token[i] = '\0';
   return(TRUE);
}

/****************************************************************************/
/*
   Get a line from the input file.
*/
int get_line(char *cp,int size)
{
   CString  t;
   long     lc;

   do
   {
      if (fgets(cp,size,ifp) == NULL)
      {
         if (!feof(ifp))
         {
            fprintf(ferr,"Read error\n");
            return -1;
         }
         else
         {
            cp[0] = '\0';
            return(-2);
         }
      }
      t = cp;
      t.TrimLeft();
      t.TrimRight();
      strcpy(cp,t);
      lc = ++ifp_line;
   }while (t.IsEmpty());

   return(TRUE);
}

/****************************************************************************/
/*
   Parsing error.
*/
int p_error()
{
   CString  tmp;
   CString  t;
   tmp.Format("Fatal Parsing Error : Token \"%s\" on line %ld\n",token,ifp_line);
   ErrorMessage(t,"Fatal CXF Read Error", MB_OK | MB_ICONHAND);
   
   return -1;
}

/****************************************************************************/
/*
*/
void free_all_mem()
{

   return;
}

/****************************************************************************/
/*
*/
static void clean_text(char *p)
{
   int   t = 0,i,l = strlen(p);
   CString  tmp = "";

   for (i=0;i<l;i++)
   {
      if (p[i] == '\\')
      {
         i++;
         if (p[i] == 'b')  p[i] = ' '; /* convert to blank */
      }
      tmp += p[i];
   }

   tmp.TrimLeft();
   tmp.TrimRight();
  
   // $$p = plusminus
   // $$a = degree
   // $$d = diameter
   int f;
   if ((f = tmp.Find("$$p")) > -1)
   {
      CString  tl,tr;
      tl = tmp.Left(f);
      tr = tmp.Right(strlen(tmp)-(f+3));
      tmp = tl;
      tmp += CHAR_PLUSMINUS;
      tmp += tr;
   }

   if ((f = tmp.Find("$$a")) > -1)
   {
      CString  tl,tr;
      tl = tmp.Left(f);
      tr = tmp.Right(strlen(tmp)-(f+3));
      tmp = tl;
      tmp += CHAR_DEGREE;
      tmp += tr;
   }

   if ((f = tmp.Find("$$d")) > -1)
   {
      CString  tl,tr;
      tl = tmp.Left(f);
      tr = tmp.Right(strlen(tmp)-(f+3));
      tmp = tl;
      tmp += CHAR_DIAMETER;
      tmp += tr;
   }
   strcpy(p,tmp);

   return;
}

/*****************************************************************************/
/*
*/
static void elim_ext(char *n)
{
   unsigned int   i;

   for (i=0;i<strlen(n);i++)
   {
      if (n[i] == '.')
      {
         n[i] = '\0';
         break;
      }        
   }  
}

/****************************************************************************/
/*
   NULL string will return 0  
*/
static double   cnv_unit(char *s)
{
   if (STRLEN(s) == 0)  return 0.0;
   return atof(s) * scalefactor;
}

/*****************************************************************************/
/*
*/
static int assign_layers()
{
   LayerStruct *ll;

   if (ll = doc->FindLayer_by_Name("DRILLHOLE"))
   {
      ll->setLayerType(LAYTYPE_DRILL);
   }

   Graph_Level("COMPOUTLINE_BOTTOM","",0);
   Graph_Level("COMPOUTLINE_TOP","",0);
   Graph_Level_Mirror("COMPOUTLINE_TOP", "COMPOUTLINE_BOTTOM", "");

   Graph_Level("REFDES_BOTTOM","",0);
   int laynr = Graph_Level("REFDES_TOP","",0);
   Graph_Level_Mirror("REFDES_TOP", "REFDES_BOTTOM", "");

   double minref = 0.05 * Units_Factor(UNIT_INCHES, pageunits);
   double maxref = 0.05 * Units_Factor(UNIT_INCHES, pageunits);
   update_manufacturingrefdes(doc, file,*(file->getBlock()), minref, maxref, laynr, FALSE);

   int layernr = Graph_Level("COMPOUTLINE_TOP","",0);
   ll = doc->FindLayer(layernr);
   ll->setComment("Component_Outline_Top");
   ll->setLayerType(LAYTYPE_SILK_TOP);
   
   layernr = Graph_Level("COMPOUTLINE_BOTTOM","",0);
   ll = doc->FindLayer(layernr);
   ll->setComment("Component_Outline_Bottom");
   ll->setLayerType(LAYTYPE_SILK_BOTTOM);

   layernr = Graph_Level("REFDES_TOP","",0);
   ll = doc->FindLayer(layernr);
   ll->setComment("Redfdes_Top");
   ll->setLayerType(LAYTYPE_SILK_TOP);
   
   layernr = Graph_Level("REFDES_BOTTOM","",0);
   ll = doc->FindLayer(layernr);
   ll->setComment("Refdes_Bottom");
   ll->setLayerType(LAYTYPE_SILK_BOTTOM);


   if (ll = doc->FindLayer_by_Name("MASK_TOP"))
   {
      ll->setLayerType(LAYTYPE_MASK_TOP);
      ll->setFlagBits(LY_NEVERMIRROR); 
      ll->setComment("Mask Top");
   }
   if (ll = doc->FindLayer_by_Name("MASK_BOT"))
   {
      ll->setLayerType(LAYTYPE_MASK_BOTTOM);
      ll->setFlagBits(LY_MIRRORONLY); 
      ll->setComment("Mask Bottom");
   }
   if (ll = doc->FindLayer_by_Name("PASTE_TOP"))
   {
      ll->setLayerType(LAYTYPE_PASTE_TOP);
      ll->setFlagBits(LY_NEVERMIRROR); 
      ll->setComment("Paste Top");
   }
   if (ll = doc->FindLayer_by_Name("PASTE_BOT"))
   {
      ll->setLayerType(LAYTYPE_PASTE_BOTTOM);
      ll->setFlagBits(LY_MIRRORONLY); 
      ll->setComment("Paste Bottom");
   }

   return 1;
}


/*Ende ***********************************************************************/

