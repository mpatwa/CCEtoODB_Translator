
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
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
#include "pdifin.h"
#include "extents.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress;
//extern LayerTypeInfoStruct layertypes[];

static   int         get_padsmd(const char *,int);
static   const char  *get_padname(int pt, int *found);
static   int         get_defpad_ptr(const char *n);
static   double      cnv_unit(char *);
static   int         go_command(List *,int);
static   void        free_all_mem();
static   int         get_tok();
static   int         p_error();
static   int         tok_search(List *,int);
static   int         push_tok();
static   int         loop_command(List *,int);
static   int         tok_layer();
static   void        clean_text(char *);
static   void        clean_libname(char *);
static   int         update_pdef(int,int);
static   int         check_pdef(int);
static   void        elim_ext(char *);
static   int         get_padnumber(const char *);
static   int         get_next(char *,int);
static   int         get_line(char *,int);
static   void        pad_error(char *,int);
static   int         get_pincnt(const char *,int,int *);
static   int         do_planenet(char *);
static   int         is_layer_used(char *l);
static   int         pinnr_from_circle(const char *l,double x1,double y1,double rad);
static   int         pinnr_from_poly(const char *l, double w);
static   int         is_attr_layer(const char *l,char c);
static   int         get_appshape(char *t);
static   int         get_apptr(int apnr);
static   int         get_physnr(char *l,int lphys);
static void correct_viapadname();
static   GraphicClassTag get_layerclass(const char *layername);
static   int         load_pdifsettings(const CString fname);
static   int         assign_layers();
static int assign_drillholes();
static int write_apertures();
static int report_padstackaccess(const char *settingsFile);

static char  *line_style[] =      /* PDIF line styles.    */
{
   "solid",
   "dashed",
   "dotted"
};

static   Global      G;                            /* global const. */
static double TEXT_CORRECT  = 0.75;

static   Adef        layer_attr[MAX_LAYERS];       /* Array of layers from pdif.in   */
static   int         layer_attr_cnt;

static   Mapdef      layer_map[MAX_LAYERS];       /* Array of layers from pdif.in   */
static   int         layer_map_cnt;

static   Ignoredef   ignore_pad_layer[MAX_LAYERS];       /* Array of layers from pdif.in   */
static   int         ignore_pad_layer_cnt;


static   CAttrmapArray  attrmaparray;
static   int         attrmapcnt;

static   CUnknownapertureArray   unknownaperturearray;
static   int         unknownaperturecnt;

static   CDeriveDrillArray derivedrillarray;
static   int         derivedrillcnt;

static   Pdef        *def_pad;                     /* Array of pad stacks. */
static   PDIFLayers  layer[MAX_LAYERS];            /* Array of layer names from the actual pdif file */

static   FILE        *ifp;                         /* File pointers.    */
static   long        ifp_line = 0;                 /* Current line number. */

static   char        token[MAX_LINE];              /* Current token.       */

static   CString     boardoutline;     
static   CString     compoutline[30];
static   int         compoutlinecnt;

static   CString     cur_compname;                /* current used library name */
static   CString     cur_libname;                 /* current used library name */
static   CString     cur_pinname;                 /* current used library name */
static   CString     cur_padname;                 /* current used library name */
static   CString     cur_netname;                 /* current used library name */
static   CString     cur_afn;                     /* current arrow file name */

static   int         pad_def_flg = FALSE;
static   int         comp_def_flg = FALSE;
static   int         pincnt;
/* different pins with same name can not exist */
static   int         askdrillhole = TRUE;
static   int         pinwarning = TRUE;
static   int         pinsmdcorrect = FALSE;
static   int         pdifunit = U_MIL;
static   int         padnumber = FALSE;

static   int         Push_tok = FALSE;
static   char        cur_line[MAX_LINE];
static   int         cur_new = TRUE;

static   PDIF_ShapeName   *shapename;
static   int         shapenamecnt = 0;

static   PDIF_Planenet     *planenet;
static   int         planenetcnt=0;

static   PDIF_Pin    *pinlist;
static   int         pinlistcnt;

static   PDIF_Aperture     *aperture;
static   int         aperturecnt;

typedef CArray<Point2, Point2&> CPolyArray;

static   int         polycnt = 0;
static   CPolyArray  poly_l;

static   CUIntArray  typeassign;

static   int         generatedrill = FALSE;
static   double      generatedrillunits = 1;
static   double      generatedrillpercent;

static   FILE        *ferr;
static   int         cur_filenum = 0;
static   FileStruct  *file = NULL;
static   CCEtoODBDoc  *doc;
static   int         pageunits;

static   int         cur_pin_in_poly;
static   DataStruct  *cur_compdata;    // global pointer to the current component.
static   int         convert_padcircle_2_app, convert_padrect_2_app, convert_padline_2_app;
static   int         display_error = 0;

static   CCompPadstackArray   comppadstackarray;
static   int            comppadstackcnt;

static   CCompPadnameArray comppadnamearray;
static   int            comppadnamecnt;

static   CArrowArray arrownamearray;
static   int         arrownamecnt;

static   char        BUILD_PADSTACK = 'G';   // g = graphic, f = flash, a = all
static   char        BUILD_COMPONENT= 'G';   // g = graphic, f = flash, a = all

static   int         token_name = FALSE;
static   int         complexcnt;

static   CDrillArray drillarray;
static   int         drillcnt;

static   int         cur_layout_mode;     // pdif can read layout and schematic. 
                                          // 0 = schematic , 1 = layout

static   int         DO_IPT_CODE = FALSE;
static   int         ACCEL_PDIF = FALSE;  // this has an influence on how the Cn section is done
static   int         USE_PART_AS_DEVICE = FALSE;
static   int         USE_PIN_SEQ = FALSE;

static   PDIFRd      current_rd;

static   double      accuracy;
static   bool        nonsupportedFile;

/******************************************************************************
* ReadPDIF     aka PCAD Reader
*/
void ReadPDIF(const char *f, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits, int Layout)
{
   nonsupportedFile = false;
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
   ACCEL_PDIF = FALSE;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(f,"rt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file",f);
      ErrorMessage(t, "Error");
      return;
   }

   CString pdifLogFile = GetLogfilePath("pdif.log");
   if ((ferr = fopen(pdifLogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", pdifLogFile);
      ErrorMessage(t, "Error");
      return;
   }

   // file assign is done on COMPONENT 
   if ((def_pad = (Pdef*)calloc(MAX_PDEF, sizeof(Pdef))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   poly_l.SetSize(100,100);
   polycnt = 0;

   typeassign.SetSize(100,100);  // no cnt needed because the ty indexes into the array

   comppadstackarray.SetSize(100,100);
   comppadstackcnt = 0;

   comppadnamearray.SetSize(100,100);
   comppadnamecnt = 0;

   arrownamearray.SetSize(100,100);
   arrownamecnt = 0;

   attrmaparray.SetSize(100,100);
   attrmapcnt = 0;

   unknownaperturearray.SetSize(100,100);
   unknownaperturecnt = 0;

   derivedrillarray.SetSize(100,100);
   derivedrillcnt = 0;

   drillarray.SetSize(100,100);
   drillcnt = 0;

   if ((aperture = (PDIF_Aperture *)calloc(MAX_APERTURE, sizeof(PDIF_Aperture))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   aperturecnt = 0;

   if ((planenet = (PDIF_Planenet *)calloc(MAX_PLANENET, sizeof(PDIF_Planenet))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   planenetcnt = 0;

   if ((pinlist = (PDIF_Pin *)calloc(MAX_COM_PINS, sizeof(PDIF_Pin))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   pinlistcnt = 0;

   if ((shapename = (PDIF_ShapeName *)calloc(MAX_SHAPE,sizeof(PDIF_ShapeName))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   shapenamecnt = 0;

   /* initialize scanning parameters */
   cur_netname = "";
   memset(&G,0,sizeof(Global));
   layer_attr_cnt = 0;
   layer_map_cnt = 0;
   ignore_pad_layer_cnt = 0;

   boardoutline= "";
   cur_compname= ""; 
   cur_libname = "";                 
   cur_pinname = "";                
   cur_padname = "";
   cur_afn = "";
   cur_layout_mode = Layout;

   accuracy = get_accuracy(doc);

   CString settingsFile( getApp().getImportSettingsFilePath("pdif.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nPDIF Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_pdifsettings(settingsFile);

   rewind(ifp);

   if (go_command(str_lst,SIZ_STR_LST) < 0)
   {
      // printf big prob.
      if (nonsupportedFile)
      {
         // Removed the file since nothing is read
         //POSITION pos = doc->getFileList().Find(file);
         //doc->FreeFile(file);
         //doc->getFileList().RemoveAt(pos);  
         doc->getFileList().deleteFile(file);
         file =  NULL;
      }
      else
         ErrorMessage("PDIF read error", "Error");
   }

   if (Layout && !nonsupportedFile)
   {
      // here update all vias, only if a file was loaded.   
      if (file != NULL)
         correct_viapadname();

      assign_layers();

      update_smdpads(doc); // this makes pad access
      RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.

      assign_drillholes();
      write_apertures();

      report_padstackaccess(settingsFile);

      if (file)   // John Robb had a PDIF file which had overlapping traces.
      {
         double accuracy = get_accuracy(doc);
         EliminateDuplicateVias(doc, file->getBlock());
         // this can happen on strange via placements.
         EliminateSinglePointPolys(doc);                
         BreakSpikePolys(file->getBlock());
         //progress->SetStatus("Crack");
         Crack(doc,file->getBlock(), TRUE);          
         //progress->SetStatus("Elim traces");
         EliminateOverlappingTraces(doc,file->getBlock(), TRUE, accuracy);                
      }
   }

   fclose(ifp);

   doc->purgeUnusedWidthsAndBlocks(false /*no report*/);

   poly_l.RemoveAll();
   polycnt = 0;

   int   i;
   for (i=0;i<comppadstackcnt;i++)
   {
      delete comppadstackarray[i];
   }
   comppadstackarray.RemoveAll();
   comppadstackcnt = 0;

   for (i=0;i<comppadnamecnt;i++)
   {
      delete comppadnamearray[i];
   }
   comppadnamearray.RemoveAll();
   comppadnamecnt = 0;

   for (i=0;i<arrownamecnt;i++)
   {
      delete arrownamearray[i];
   }
   arrownamearray.RemoveAll();
   arrownamecnt = 0;

   for (i=0;i<attrmapcnt;i++)
   {
      delete attrmaparray[i];  
   }
   attrmaparray.RemoveAll();
   attrmapcnt = 0;

   for (i=0;i<unknownaperturecnt;i++)
   {
      delete unknownaperturearray[i];  
   }
   unknownaperturearray.RemoveAll();
   unknownaperturecnt = 0;

   for (i=0;i<derivedrillcnt;i++)
   {
      delete derivedrillarray[i];  
   }
   derivedrillarray.RemoveAll();
   derivedrillcnt = 0;

   drillarray.RemoveAll();

   typeassign.RemoveAll();

   free_all_mem();
   fclose(ferr);

   if (display_error && !nonsupportedFile)
      Logreader(pdifLogFile);
}


/****************************************************************************/
/*
*/
static int get_drillindex(double size, int layernum)
{
   PDIFDrill      p;
   int            i;

   if (size == 0) return -1;

   for (i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d",drillcnt);

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   p.d = size;
   drillarray.SetAtGrow(drillcnt,p);  
   drillcnt++;

   return drillcnt -1;
}

/******************************************************************************
* assign_drillholes
*/
static int assign_drillholes()
{
   int   i;

   // here is case the SMD flag was not set !
   for (i=0;i<G.max_dpads;i++)
   {
      if (def_pad[i].used == FALSE)             continue;
      if (def_pad[i].pstack_name == NULL)       continue;
      if (strlen(def_pad[i].pstack_name) == 0)  continue;

      BlockStruct *b = Graph_Block_Exists(doc, def_pad[i].pstack_name,-1);
      if (b)
      {
         Attrib *a;
         if (a = is_attvalue(doc, b->getAttributesRef(), ATT_SMDSHAPE, 1))
            def_pad[i].smd = TRUE;
      }
   }

   if (generatedrill)
   {
      // from name
      for (i=0;i<G.max_dpads;i++)
      {
         if (def_pad[i].used == FALSE) continue;
         /* this can be NULL */
         if (def_pad[i].smd == 0)
         {
            if (def_pad[i].drill == 0)
            {
               // char num char drillsize
               char  c1, c2;
               int   psize, drill;
      
               if (def_pad[i].pstack_name == NULL)    continue;
               if (!strlen(def_pad[i].pstack_name))   continue;

               CString  tmp = def_pad[i].pstack_name;
               if (sscanf(def_pad[i].pstack_name,"%c%d%c%d", &c1, &psize, &c2, &drill) == 4)
               {
                  if (drill > 0 && drill < psize)
                  {
                     tmp.Format("%d", drill);   // this is always mils
                     def_pad[i].drill   = generatedrillunits * atoi(tmp);
                  }
               }
            }
         }
      }
   }

   for (i=0;i<G.max_dpads;i++)
   {
      if (def_pad[i].used == FALSE) continue;
      if (def_pad[i].pstack_name == NULL) continue;
      if (strlen(def_pad[i].pstack_name) == 0)  continue;

      BlockStruct *block = doc->Find_Block_by_Name(def_pad[i].pstack_name, -1);
      if (block == NULL)
         continue;

      /* this can be NULL */
      if (def_pad[i].smd == 0)
      {
         if (def_pad[i].drill == 0)
         {
            double xmin, xmax, ymin, ymax;
            if (block_extents(doc, &xmin, &xmax, &ymin, &ymax, &block->getDataList(), 0.0, 0.0, 0.0, 0, 1.0, -1, FALSE))
            {
               double psize = fabs(xmax - xmin);
               if (fabs(ymax - ymin))
                  psize = fabs(ymax - ymin);
               def_pad[i].drill = psize * generatedrillpercent/100; 
            }
         }
      }
   }

   // list all with drill
   for (i=0; i<G.max_dpads; i++)
   {
      if (def_pad[i].used == FALSE)
         continue;

      /* this can be NULL */
      if (def_pad[i].smd == 0)
      {
         if (def_pad[i].drill > 0)
         {
            BlockStruct *block = Graph_Block_On(GBO_APPEND, def_pad[i].pstack_name, cur_filenum, 0);
            if (block == NULL)
               continue;

            int layernum = Graph_Level("DRILLHOLE","",0);
            int drillindex;
            CString  drillname;

            if ((drillindex = get_drillindex(def_pad[i].drill, layernum)) > -1)
            {
               drillname.Format("DRILL_%d",drillindex);
               Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
            }
            else
            {
               fprintf(ferr," Drill Index error for Padstack [%s], Drill [%lf]\n", def_pad[i].pstack_name, def_pad[i].drill);
               display_error++;     
            }

            Graph_Block_Off();

            fprintf(ferr," Padstack [%s] has drill [%lf]\n", def_pad[i].pstack_name, def_pad[i].drill);
            display_error++;     
         }
      }
   }

   // list all without drill
   for (i=0;i<G.max_dpads;i++)
   {
      if (def_pad[i].used == FALSE) continue;

      /* this can be NULL */
      if (def_pad[i].smd == 0)
      {
         if (def_pad[i].drill == 0)
         {
            int   found;
            cur_padname = get_padname(def_pad[i].pdnum, &found);
            fprintf(ferr," Padstack [%s] has no drill\n", cur_padname);
            display_error++;
         }
      }
   }

   // 

   return 1;
}

/******************************************************************************
* report_padstackaccess
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

/******************************************************************************
* write_apertures
*/
static int write_apertures()
{
   int   a, err;

   for (a=0;a<aperturecnt;a++)
   {
      if (aperture[a].used == FALSE)   continue;

      CString  appname;
      appname.Format("APP_%d",aperture[a].number);

      if (aperture[a].sidecnt)
      {
         CString  pshapename;

         pshapename.Format("COMPLEX_%d", ++complexcnt);
         Graph_Block_On(GBO_APPEND,pshapename, cur_filenum, 0);

         DataStruct *d = Graph_PolyStruct(Graph_Level("0", "", 1),0L, FALSE); // poly is always with 0
         Graph_Poly(NULL,0, 0,0, TRUE);
			
			int i=0;
         for (i=0;i<aperture[a].sidecnt;i++)
         {
            Graph_Vertex(aperture[a].sides[i].x, aperture[a].sides[i].y, 0);
         }     
      
         i = aperture[a].sidecnt-1;
         if (aperture[a].sides[i].x != aperture[a].sides[0].x ||
             aperture[a].sides[i].y != aperture[a].sides[0].y)
         {
            Graph_Vertex(aperture[a].sides[0].x, aperture[a].sides[0].y, 0);
         }

         Graph_Block_Off();
         Graph_Complex(cur_filenum, appname, 0, pshapename, 0.0, 0.0, 0.0);
      }
      else
      {  
         // a round aperture can also be used as a width.
         Graph_Aperture(cur_filenum, appname, aperture[a].shape, 
            aperture[a].ow, aperture[a].oh, 
            0.0, 0.0, DegToRad(aperture[a].rotation) , 
            aperture[a].dcode, BL_WIDTH, TRUE, &err); // this is an overwrite !!!, because the aperture was placed before
      }
   }

   return 1;
}


/****************************************************************************/
/*
*/
static const char* get_maplayer(const char *l)
{
   int   i;

   for (i=0;i<layer_map_cnt;i++)
   {
      if (!STRCMPI(layer_map[i].name, l))
         return layer_map[i].map;
   }

   return l;
}

/****************************************************************************/
/*
*/
static const char* get_attrmap(const char *a)
{
   int   i;

   for (i=0;i<attrmapcnt;i++)
   {
      if (attrmaparray[i]->pdif_name.CompareNoCase(a) == 0)
         return attrmaparray[i]->cc_name.GetBuffer(0);
   }

   return a;
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
static int load_pdifsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   compoutlinecnt = 0;
   boardoutline = "";
   convert_padcircle_2_app = TRUE;
   convert_padrect_2_app = TRUE;
   convert_padline_2_app = TRUE;
   BUILD_PADSTACK = 'G';   // g = graphic, f = flash, a = all
   BUILD_COMPONENT= 'G';   // g = graphic, f = flash, a = all
   generatedrill = FALSE;
   generatedrillpercent = 50;
   DO_IPT_CODE = FALSE;
   USE_PART_AS_DEVICE = FALSE;
   USE_PIN_SEQ = FALSE;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp, "PDIF Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      CString  w = line;
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".GENERATEDRILL"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (lp[0] == 'y' || lp[0] == 'Y')
               generatedrill = TRUE;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            int   u;
            if ((u =  GetUnitIndex(lp)) > -1)
               generatedrillunits = Units_Factor(u, pageunits);
            else
            {
               generatedrill = FALSE;
               fprintf(ferr, "Wrong .GENERATEDRILL units [%s]\n", lp);
               display_error++;
            }
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
               generatedrillpercent = atof(lp);
         }
         else
         if (!STRICMP(lp,".CONVERT_PADCIRCLE_2_APP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (lp[0] == 'n' || lp[0] == 'N')
                  convert_padcircle_2_app = FALSE;
         }
         else
         if (!STRICMP(lp,".CONVERT_PADRECT_2_APP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (lp[0] == 'n' || lp[0] == 'N')
                  convert_padrect_2_app = FALSE;
         }
         else
         if (!STRICMP(lp,".CONVERT_PADLINE_2_APP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (lp[0] == 'n' || lp[0] == 'N')
                  convert_padline_2_app = FALSE;
         }
         else
         if (!STRICMP(lp,".COMPOUTLINE"))
         {
            if (compoutlinecnt < 30)
            {
               if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
               compoutline[compoutlinecnt] = lp;
               compoutlinecnt++;
            }
         }
         else
         if (!STRICMP(lp,".BOARDOUTLINE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            boardoutline = lp;
         }
         else
         if (!STRICMP(lp,".BUILD_PADSTACK"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            BUILD_PADSTACK = toupper(lp[0]);
         }
         else
         if (!STRICMP(lp,".USE_IPT_CODE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (lp[0] == 'Y' || lp[0] == 'y')
               DO_IPT_CODE = TRUE;
         }
         else
         if (!STRICMP(lp,".USE_PART_AS_DEVICE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (lp[0] == 'Y' || lp[0] == 'y')
               USE_PART_AS_DEVICE = TRUE;
         }
         else
         if (!STRICMP(lp,".BUILD_COMPONENT"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            BUILD_COMPONENT = toupper(lp[0]);
         }
         else
         if (!STRICMP(lp,".LAYERATTR"))
         {
            CString  pdiflayer;
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            pdiflayer = lp;
            pdiflayer.TrimLeft();
            pdiflayer.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);

            if (layer_attr_cnt < MAX_LAYERS)
            {
               layer_attr[layer_attr_cnt].name = pdiflayer;
               layer_attr[layer_attr_cnt].attr = laytype;
               layer_attr_cnt++;
            }
            else
            {
               fprintf(ferr,"Too many layer attributes\n");
               display_error++;
            }
         }
         else
         if (!STRICMP(lp,".LAYERMAP"))
         {
            CString  pdiflayer;
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            pdiflayer = lp;
            pdiflayer.TrimLeft();
            pdiflayer.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            if (layer_map_cnt < MAX_LAYERS)
            {
               layer_map[layer_map_cnt].name = pdiflayer;
               layer_map[layer_map_cnt].map = cclayer;
               layer_map_cnt++;
            }
            else
            {
               fprintf(ferr,"Too many layer map\n");
               display_error++;
            }

         }
         else
         if (!STRICMP(lp,".IGNORE_PAD_LAYER"))
         {
            CString  pdiflayer;
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            pdiflayer = lp;
            pdiflayer.TrimLeft();
            pdiflayer.TrimRight();

            if (ignore_pad_layer_cnt < MAX_LAYERS)
            {
               ignore_pad_layer[ignore_pad_layer_cnt].name = pdiflayer;
               ignore_pad_layer_cnt++;
            }
            else
            {
               fprintf(ferr,"Too many ignore pad layer\n");
               display_error++;
            }
         }
         else
         if (!STRCMPI(lp,".TYPEASSIGN"))
         {
            int   typenr, geomtyp;
            // assign a type to a geometry
            // MECHCOMP
            // GENCOMP
            // PCBCOM
            // TESTPROBE
            // FIDUCIAL
            // TOOLING
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            typenr = atoi(lp);

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (!STRNICMP(lp,"MECH",3))
               geomtyp = BLOCKTYPE_MECHCOMPONENT;
            else
            if (!STRNICMP(lp,"GEN",3))
               geomtyp = BLOCKTYPE_GENERICCOMPONENT;
            else
            if (!STRNICMP(lp,"PCB",3))
               geomtyp = BLOCKTYPE_PCBCOMPONENT;
            else
            if (!STRNICMP(lp,"TEST",3))
               geomtyp = BLOCKTYPE_TESTPOINT;
            else
            if (!STRNICMP(lp,"FID",3))
               geomtyp = BLOCKTYPE_FIDUCIAL;
            else
            if (!STRNICMP(lp,"TOOL",3))
               geomtyp = BLOCKTYPE_TOOLING;
            else
            {
               // unknown type
               fprintf(ferr,"Unknown .TYPEASSIGN [%s]\n",lp);
               display_error++;
               continue;
            }
            // ty is allowed from -32000 to 32000
            typenr += 32000;
            if (typenr > 0)
            {
               typeassign.SetAtGrow(typenr,geomtyp);           
            }
         }
         else
         if (!STRICMP(lp,".ATTRIBMAP"))
         {
            CString  pdifattr;
            CString  ccattr;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            pdifattr = _strupr(lp);
            pdifattr.TrimLeft();
            pdifattr.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            ccattr = lp;
            ccattr.TrimLeft();
            ccattr.TrimRight();

            PDIFAttrmap *c = new PDIFAttrmap;
            attrmaparray.SetAtGrow(attrmapcnt,c);  
            attrmapcnt++;
            c->pdif_name = pdifattr;
            c->cc_name = ccattr;
         }
         else
         if (!STRICMP(lp,".DERIVEDRILL"))
         {
            CString  layer;
            double   percent;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            layer = _strupr(lp);
            layer.TrimLeft();
            layer.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            percent = atof(lp);

            PDIFDeriveDrill   *c = new PDIFDeriveDrill;
            derivedrillarray.SetAtGrow(derivedrillcnt,c);  
            derivedrillcnt++;
            c->layername = layer;
            c->percent = percent;
         }
         else
         if (!STRICMP(lp,".USEPINSEQ"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (lp[0] == 'Y' || lp[0] == 'y')
               USE_PIN_SEQ = TRUE;

         }
      }
   }

   fclose(fp);

   if (USE_PIN_SEQ)
   {
      fprintf(ferr, "Pin Sequence will be used as Pin Names.\n");
      display_error++;
   }

   if (!convert_padcircle_2_app)
   {
      fprintf(ferr, "Circles as part of a padstack are not converted to Apertures.\n");
      display_error++;
   }

   if (!convert_padrect_2_app)
   {
      fprintf(ferr, "Rectangles (FilledRectangles) as part of a padstack are not converted to Apertures.\n");
      display_error++;
   }

   if (!convert_padline_2_app)
   {
      fprintf(ferr, "Lines as part of a padstack are not converted to Apertures.\n");
      display_error++;
   }


   return 1;
}


/******************************************************************************
* correct_viapadname
*/
static void correct_viapadname()
{
   BOOL correctVia[256];
	
	int i=0;
   for (i=0; i<256; i++)
      correctVia[i] = FALSE;

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
      int vi = atoi(block->getName());
      i = update_pdef(atoi(block->getName()),0);

      // here via must have a name
      if (def_pad[i].pstack_name && strlen(def_pad[i].pstack_name))
      {
         int blockNum = doc->Get_Block_Num(def_pad[i].pstack_name, cur_filenum, TRUE); 
         data->getInsert()->setBlockNumber(blockNum); 

         if (vi >= 0 && vi < 256)
            correctVia[vi] = TRUE;
      }
   }
   
   // delete old vianames from database, because they have been renamed to their true name
   for (i=0; i<256; i++)
   {
      if (!correctVia[i])
         continue;

      CString viaName;
      viaName.Format("%d", i);

      BlockStruct *viaGeom = Graph_Block_On(GBO_APPEND, viaName, cur_filenum, 0); // just make sure it is there.
      Graph_Block_Off();

      if (viaGeom->getBlockType() == BLOCKTYPE_PADSTACK)
         doc->RemoveBlock(viaGeom);
   }
}

/****************************************************************************/
/*
   Call function associated with next token.
   The token must be enclosed by brackets ( {} or [] ).
   Tokens enclosed by {} are searched for on the local
   token list.  Tokens enclosed by [] are searched for
   in the global token list.
*/
int go_command(List *tok_lst,int lst_size)
{
   int      i,brk, res = 0;
   int      square_bracket = 0;

   // debug
   CString  t;
   long     l;


   if (!get_tok()) return p_error();

   t = token;
   l = ifp_line;

   switch((brk = tok_search(brk_lst,SIZ_BRK_LST)))
   {
      case BRK_SQUIGLY:
         if (get_tok() && (i = tok_search(tok_lst,lst_size)) >= 0)
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
      break;
      case BRK_SQUARE:
         square_bracket++;
         if (get_tok() && (i = tok_search(glb_lst,SIZ_GLB_LST)) >= 0)
         {
            res = (*glb_lst[i].function)();
         }
         else
         {
            fnull();
            // return p_error();
         }
      break;
      case BRK_B_SQUIGLY:
         push_tok();
         return 1;
      break;
      case BRK_B_SQUARE:       // I seperate [] from {} because there are syntax error in PDIF
         // and [] are always nodal commands (no hierachies). 
         if (square_bracket)
         {
            square_bracket--;
            push_tok();
         }
         else
         {
            // close a square bracket, even that it was not open ???
            int w = 0;
         }
         return 1;
      break;
      default:
         return p_error();
      break;
   }
   if (res == -1)
   {
      CString  t;
      t.Format("PDIF Read error : Token [%s] at %ld", token, ifp_line);
      // big error occured.
      ErrorMessage(t, "Error");
      return -1;
   }
   else if (res < 0)
   {
      return res;
   }


   if (!get_tok()) return p_error();
   if (brk != tok_search(brk_lst,SIZ_BRK_LST) - 2)
      return p_error();
   return res;
}                                   


/****************************************************************************/
/*
   Loop through a section.
*/
static int loop_command(List *list,int size)
{
   int repeat = TRUE;
   int res = 0;
   while (repeat)
   {
      if ((res = go_command(list,size)) < 0)   
         return res;
      if (!get_tok())
         return p_error();
      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_SQUIGLY)
         repeat = FALSE;
      push_tok();
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
   char     tmp[MAX_LINE];
   int      square_brk_count = 0;
   
   fprintf(ferr,"Unsupported Token [%s] at %ld -> skipped\n",token,ifp_line);
   display_error++;
   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)   continue;   // do not evaluate a name []{} token.
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_SQUIGLY:
               ++brk_count;
            break;
            case BRK_SQUARE:
               //++brk_count;
               ++square_brk_count; // [
            break;
            case BRK_B_SQUIGLY:
               if (brk_count-- == 0)
               {
                  push_tok();
                  return 1;
               }
            break;
            case BRK_B_SQUARE:
               // if in a global, also stop
               if (square_brk_count-- == 0 && brk_count == 0) // ]
               {
                  push_tok();
                  return 1;
               }
            break;
            default:
            {
               // attrbutes can be {At Key Value" x y} without a text delimeter
               if (!STRCMPI(token,"At") && cur_line[0] == '{')
               {
                  tmp[0] = '\0';
                  while (strlen(tmp) + strlen(cur_line) < MAX_LINE)
                  {
                     strcat(tmp,cur_line);
                     while (isspace(tmp[strlen(tmp)-1])) 
                        tmp[strlen(tmp)-1] = '\0';

                     if (tmp[strlen(tmp)-1] == '}')   break;
                     fprintf(ferr,"Error in At command at %ld\n",ifp_line);
                     return -1;
                  }
                  cur_new = TRUE;
                  cur_line[0] = '\0';
                  if (brk_count-- == 0)
                  {
                     push_tok();
                     return 1;
                  }
               }
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
   char     tmp[MAX_LINE];
   int      square_brk_count = 0;
   
   //fprintf(ferr,"Unsupported Token [%s] at %ld -> skipped\n",token,ifp_line);
   //display_error++;
   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)   continue;   // do not evaluate a name []{} token.
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_SQUIGLY:
               ++brk_count;
            break;
            case BRK_SQUARE:
               //++brk_count;
               ++square_brk_count; // [
            break;
            case BRK_B_SQUIGLY:
               if (brk_count-- == 0)
               {
                  push_tok();
                  return 1;
               }
            break;
            case BRK_B_SQUARE:
               // if in a global, also stop
               if (square_brk_count-- == 0 && brk_count == 0) // ]
               {
                  push_tok();
                  return 1;
               }
            break;
            default:
            {
               // attrbutes can be {At Key Value" x y} without a text delimeter
               if (!STRCMPI(token,"At") && cur_line[0] == '{')
               {
                  tmp[0] = '\0';
                  while (strlen(tmp) + strlen(cur_line) < MAX_LINE)
                  {
                     strcat(tmp,cur_line);
                     while (isspace(tmp[strlen(tmp)-1])) 
                        tmp[strlen(tmp)-1] = '\0';

                     if (tmp[strlen(tmp)-1] == '}')   break;
                     fprintf(ferr,"Error in At command at %ld\n",ifp_line);
                     return -1;
                  }
                  cur_new = TRUE;
                  cur_line[0] = '\0';
                  if (brk_count-- == 0)
                  {
                     push_tok();
                     return 1;
                  }
               }
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
   cur_layout_mode;     // pdif can read layout and schematic. 
                        // 0 = schematic , 1 = layout
*/
static int com_component()
{
   if (!get_tok())   
      return p_error();                       /* Original file name.  */

   file = Graph_File_Start(token, fileTypePdifLayout);

   if (cur_layout_mode)
      file->setBlockType(blockTypePcb);
   else
      file->setBlockType(blockTypeSheet);

   file->getBlock()->setBlockType(file->getBlockType());

   cur_filenum = file->getFileNumber();

   return loop_command(com_lst,SIZ_COM_LST);
}

/****************************************************************************/
/*
   Process aperture section.
*/
static int apt_aprnum()
{
   if (aperturecnt >= MAX_APERTURE)
   {
      fprintf(ferr,"Too many aperatures defined.\n");
      return -1;
   }

   if (!get_tok())   return p_error();
   aperture[aperturecnt].number =atoi(token);
   aperture[aperturecnt].used = FALSE;
   G.cur_rotation = 0;
   loop_command(app_lst,SIZ_APP_LST);
   aperture[aperturecnt].rotation = G.cur_rotation;
   // here update aperture
   
   aperturecnt++;
   return 1;
}

/****************************************************************************/
/*
*/
static int apt_apfile()
{
   if (!get_tok())   return p_error();

   // no name found. This seems to be legal.
   if (!STRCMPI(token,"}"))
      push_tok();

   return 1;
}

/****************************************************************************/
/*
*/
static int app_shp()
{
   if (!get_tok())   return p_error();
   aperture[aperturecnt].shape =get_appshape(token);
   return 1;
}
static int app_dcode()
{
   if (!get_tok())   return p_error();
   aperture[aperturecnt].dcode =atoi(token);
   return 1;
}

static int app_odia()
{
   if (!get_tok())   return p_error();
   aperture[aperturecnt].ow =cnv_unit(token);
   return 1;
}

static int app_sides()
{
   int      i, sides;
   double   x, y;

   if (!get_tok())   return p_error();
   sides = aperture[aperturecnt].sidecnt =atoi(token);

   if (aperture[aperturecnt].sidecnt > 10)
   {
      fprintf(ferr,"Maximum 10 sides are allowed in an POLYGON aperture at %ld\n", ifp_line);
      display_error++;
      aperture[aperturecnt].sidecnt = 10;
   }


   for (i=0;i<sides;i++)
   {
      if (!get_tok())   return p_error();
      x = cnv_unit(token);
      if (!get_tok())   return p_error();
      y = cnv_unit(token);

      if (i < 10)
      {
         aperture[aperturecnt].sides[i].x = x;
         aperture[aperturecnt].sides[i].y = y;
      }
   }

   return 1;
}

static int app_idia()
{
   if (!get_tok())   return p_error();
   aperture[aperturecnt].ow =cnv_unit(token);
   return 1;
}

static int app_owd()
{
   if (!get_tok())   return p_error();
   aperture[aperturecnt].ow =cnv_unit(token);
   return 1;
}

static int app_oht()
{
   if (!get_tok())   return p_error();
   aperture[aperturecnt].oh =cnv_unit(token);
   return 1;
}

static int app_apcomm()
{
   if (!get_tok())   return p_error();
   return 1;
}

static int app_aptype()
{
   if (!get_tok())   return p_error();
   return 1;
}

static int app_aprot()
{
   if (!get_tok())   return p_error();
   G.cur_rotation = atoi(token);
   return 1;
}

/****************************************************************************/
/*
   Process environment section.
*/
static int com_environment()
{
   return loop_command(env_lst,SIZ_ENV_LST);
}

static int apt_apver()
{
   if (!get_tok())   return p_error();
   // printf("{Apver %s} not supported\n",token);
   return 1;
}

/****************************************************************************/
/*
   Aperature Table -> new in Version 7
*/
static int com_aperture_table()
{
   return loop_command(apt_lst,SIZ_APT_LST);
}


/****************************************************************************/
/*
   Process user section.
*/
static int com_user()
{
   return loop_command(usr_lst,SIZ_USR_LST);
}


/****************************************************************************/
/*
   Process display section.
   Display contains only global settings.
*/
static int com_display()
{
   return loop_command(glb_lst,SIZ_GLB_LST);
}


/****************************************************************************/
/*
   Process symbol section.
*/
static int com_symbol()
{
   int res = loop_command(sym_lst,SIZ_SYM_LST);
   return res;
}

/****************************************************************************/
/*
   Process detail section.
*/
static int com_detail()
{
   return loop_command(det_lst,SIZ_DET_LST);
}

/****************************************************************************/
/*
   Environment Section.

*/
/****************************************************************************/


/****************************************************************************/
/*
   Get PDIF version.
*/
static int env_version()
{
   // can be 8.5
   if (!get_tok())   return p_error();
   G.cur_pdifversion = (int)floor(atof(token));
   //printf("\t\t\tPDIF Version %1.1lf\n",atof(token));

   return 1;
}

/****************************************************************************/
/*
   {Program "ACCEL P-CAD PCB Version 14.00.46"}
   {Program "PDIF-OUT Version 8.6"}
   {Program "P-CAD 2000 PCB Version 15.10.17"}

   if the PDIF file was generated by ACCEL, the Cn section is pinname dependend and
   not sequence dependend.

*/
static int env_program()
{
   if (!get_tok())   return p_error();
   
   if (STRNICMP(token, "PDIF-OUT", strlen("PDIF-OUT")))
   {
      CString errMessage = (CString)"Thank you for trying to import this file into CAMCAD.\n\n" +
                           "The CAMCAD PDIF Reader only supports files generated by PDIF-OUT from " +
                           "PCAD v8.x and Prior (DOX/UNIX).  If the PDIF file was generated from " +
                           "PCAD 200x or Accell Eda then please generate and use a PCB ASCII file " +
                           "rather than using PDIF from this system.";
      ErrorMessage(errMessage, "");
      nonsupportedFile = true;
      return -2;
   }
   else
   {
      // go for version number >= 14
      char *lp;
      lp = strtok(token," \t");

      while (lp)
      {
         if (lp && !STRCMPI(lp,"Version"))
         {
            if (lp = strtok(NULL," \t"))
            {
               int ver = atoi(lp);
               if (ver >= 14) ACCEL_PDIF = TRUE;
            }
         }
         lp = strtok(NULL," \t");
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int env_dbtype()
{
   CString  t;
   if (!get_tok())   return p_error();
   // must be PC-Board
   t = token;

   if(cur_layout_mode)
   {
      if (t.CompareNoCase("PC-Board") != 0)
      {
         t.Format("DbType [%s] expected -> found [%s]\nPlease use the PDIF Layout Reader in Import.", "PC-Board", token);
         ErrorMessage(t, "Fatal PDIF Read Error", MB_OK | MB_ICONHAND);
         return -2;
      }
      file->setBlockType(blockTypePcb);
      file->getBlock()->setBlockType(file->getBlockType());
   }
   else
   {
      if (t.CompareNoCase("Schematic") != 0)
      {
         t.Format("DbType [%s] expected -> found [%s]\nPlease use the PDIF Schematic Reader in Import.", "Schematic", token);
         ErrorMessage(t,"Fatal PDIF Read Error", MB_OK | MB_ICONHAND);
         return -2;
      }
      file->setBlockType(blockTypeSheet);
      file->getBlock()->setBlockType(file->getBlockType());
   }
   return 1;
}

/****************************************************************************/
/*
   Get the sst file name. Not needed.
*/
static int env_sstfile()
{
   if (!get_tok())   return p_error();
   return 1;
}

/****************************************************************************/
/*
   Get grid size and save in Global.
*/
static int env_grid()
{
   if (get_tok())
      G.grid = atoi(token);
   else
      return p_error();
      
   return 1;
}

/****************************************************************************/
/*
*/
static int env_dbunit()
{
   if (!get_tok())   return p_error();

   if (!STRCMPI("CMM",token))          pdifunit = U_CMM; 
   else
   if (!STRCMPI("MIL",token))          pdifunit = U_MIL;    
   else
   if (!STRCMPI("CENTIMIL",token))     pdifunit = U_MIL; 
   else
   if (!STRCMPI("DECIMICRON",token))   pdifunit = U_DECIMM; 
   else
   {
      CString  tmp;
      tmp.Format("Unknown DbUnit [%s]", token);
      ErrorMessage(tmp,"Unit Error");
   }

   return 1;
}

/****************************************************************************/
/*
   Load layer table.
*/
static int env_layer()
{
   while (get_tok() && tok_search(brk_lst,SIZ_BRK_LST) < BRK_B_SQUIGLY)
   {
      if (G.max_layers < MAX_LAYERS)
      {
         layer[G.max_layers].layer_name = token;
         // Only write out layers which are used !
         if (!get_tok()) return p_error();
         layer[G.max_layers].flg = 0;
         layer[G.max_layers].used = 0;
         layer[G.max_layers].mirror_index = -1;
         layer[G.max_layers].physnr = 0;
         layer[G.max_layers].electrical = 0;
         layer[G.max_layers++].color = atoi(token);
      }
      else
      {
         fprintf(ferr,"Too many Layers\n");
         return -1;
      }
   }
   push_tok();
   return 1;
}

/****************************************************************************/
/*
   Load layer table.
*/

static int env_lyrphid()
{
   int   n1,n2,n3;

   while (get_tok() && tok_search(brk_lst,SIZ_BRK_LST) < BRK_B_SQUIGLY)
   {
      n1 = atoi(token);
      if (!get_tok()) return p_error();
      n2 = atoi(token);
      if (!get_tok()) return p_error();
      n3 = atoi(token);
      /* if n1 == n3 layer is trace layer */

      /* check if layers exist */
      if (n1 >= G.max_layers) continue;

      if (n1 != n2)
      {
         // here is mirror
         layer[n1].mirror_index = n2;
         layer[n2].mirror_index = n1;
      }

      if (n1 == n3)
      {
         // this is electrical layers
         // how do i get a layerattribute
         int n =  Graph_Level(get_maplayer(layer[n1].layer_name),"", 0);
         layer[n1].electrical = TRUE;
         LayerStruct *l = doc->FindLayer(n);
         l->setLayerType(LAYTYPE_SIGNAL);
      }
   }
   push_tok();
   return 1;
}

/****************************************************************************/
/*
   User Section.
*/
/****************************************************************************/

/****************************************************************************/
/*

   Display Section.

   Contains definitions for global commands.

*/
/****************************************************************************/


/****************************************************************************/
/*
   Set current layer number.
*/

static int glb_layer()
{
   CString  l;
   if (!get_tok())   // an empty layer just returns.
      return -1;

   l = token;
   l.TrimLeft();
   l.TrimRight();

   if (strlen(l) == 0)  return 1;

   if ((G.cur_layer = tok_layer()) < 0)
   {
      if (G.max_layers < MAX_LAYERS)
      {
         G.cur_layer = G.max_layers;

         layer[G.max_layers].layer_name = token;
         layer[G.max_layers].flg = 0;
         layer[G.max_layers].used = 0;
         layer[G.max_layers].mirror_index = -1;
         layer[G.max_layers].physnr = 0;
         layer[G.max_layers].electrical = 0;
         layer[G.max_layers++].color = 0;
      }
      else
      {
         fprintf(ferr,"Too many Layers\n");
         display_error++;
         return -1;
      }
      return 1;
   }

   return 1;
}


/****************************************************************************/
/*
   Set current line style.
*/

static int glb_style()
{
   int      i;

   if (!get_tok())
      return p_error();

   for (i = 0; i < MAX_LINE_STYLE; ++i)
      if (!STRCMPI(line_style[i],token))
      {
         G.cur_style = i;
         break;
      }

   if (i == MAX_LINE_STYLE)
   {
      fprintf(ferr,"Wrong Linestyle\n");
      fprintf(ferr,"Style: \"%s\" on line %ld\n",token,ifp_line);
      return -1;
   }

/*
   printf("Current line style: %s\n",line_style[G.cur_style]);
*/
   return 1;
}


/****************************************************************************/
/*
   Set current line width.
*/
static int glb_width()
{
   int   err;

   if (!get_tok())
      return p_error();

   G.cur_width = round_accuracy(cnv_unit(token), accuracy);
   if (G.cur_width == 0)
      G.cur_widthindex = 0;   // make it small width.
   else
      G.cur_widthindex = Graph_Aperture("", T_ROUND, G.cur_width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   return 1;
}

/****************************************************************************/
/*
   Set current textsize.
*/
static int glb_textsize()
{
   if (!get_tok()) return p_error();

   G.cur_textsize = cnv_unit(token) * TEXT_CORRECT;
   return 1;
}

/****************************************************************************/
/*
*/
static int glb_textjust()
{
   G.cur_textjust = 0;
   if (!get_tok())      return p_error();
   /* first is Width LCR */
   if (strlen(token) > 0)
   {

      switch (toupper(token[0]))
      {
         case  'L':
            G.cur_textjust = GRTEXT_W_L;
         break;
         case  'C':
            G.cur_textjust = GRTEXT_W_C;
         break;
         case  'R':
            G.cur_textjust = GRTEXT_W_R;
         break;
      }
   }
   /* next is height */ 
   if (strlen(token) > 1)
   {
      switch (toupper(token[1]))
      {
         case  'B':
            G.cur_textjust |= GRTEXT_H_B;
         break;
         case  'C':
            G.cur_textjust |= GRTEXT_H_C;
         break;
         case  'T':
            G.cur_textjust |= GRTEXT_H_T;
         break;
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int glb_textrot()
{
   if (!get_tok())   return p_error();

   G.cur_textrot = atoi(token) * 90;
   return 1;
}

/****************************************************************************/
/*
*/
static int glb_dimtextrot()
{
   if (!get_tok())   return p_error();

   G.cur_dimtextrot = atoi(token) * 90;
   return 1;
}

/****************************************************************************/
/*
*/
static int glb_arrowfilename()
{
   if (!get_tok())   return p_error();
   cur_afn = token;
   return 1;
}

/****************************************************************************/
/*
*/
static int glb_textmirror()
{
   if (!get_tok())   return p_error();
   if (!strcmp(token,"Y"))
      G.cur_textmirror = 1;
   else
      G.cur_textmirror = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int in_smd()
{
   if (!get_tok())   return p_error();
   if (!strcmp(token,"Y"))
   {
      G.cur_smd = 1;
      if (pad_def_flg)
         get_padsmd(cur_padname,TRUE);
   }
   else
      G.cur_smd = 0;
   return 1;
}

/****************************************************************************/
/*
   can be - 32000 to + 32000
*/
static int in_ty()
{
   if (!get_tok())   return p_error();
   G.cur_ty = atoi(token);

   // ty is allowed from -32000 to 32000
   G.cur_ty += 32000;

   return 1;
}

/****************************************************************************/
/*
   
*/
static int in_org()
{
   double   x, y;

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error();
   y = cnv_unit(token);

   if (x != 0 || y != 0)
   {
      fprintf(ferr,"Origin not a 0,0 at %ld\n", ifp_line);
      display_error++;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int nain_rats()
{
   // not needed.
   if (!get_tok())   return p_error();
   return 1;
}

/****************************************************************************/
/*

   Symbol Section.

*/
/****************************************************************************/
/* Nothing in symbol section is defined.     */

/****************************************************************************/
/*
   Detail Section.
*/

/****************************************************************************/


/****************************************************************************/
/*
   Process the bourd outline.
*/
static int det_annotate()
{
   loop_command(graph_lst,SIZ_GRAPH_LST);
   return 1;
}

/*****************************************************************************/
/* 
   Text string.      
*/
static int graph_text()
{
   double   x,y;
   char     prosa[256+1];
   char     *tmp,*lp;
   char     wtmp[100];
   CString  layername;
   int      layernr;

   if ((tmp = (char *)malloc(1000+1)) == NULL)  // for HPUX char *
   {
         MemErrorMessage(__FILE__, __LINE__);
   }
   tmp[0] = '\0';
   while ((strlen(tmp) + strlen(cur_line)) < 1000)
   {
      strcat(tmp,cur_line);
      if (tmp[strlen(tmp)-1] == '\n')  tmp[strlen(tmp)-1] = '\0';
      if (tmp[strlen(tmp)-1] == '}')   break;
      if (!get_line(cur_line,MAX_LINE))
      {
         fprintf(ferr,"Error in graph_text\n");
         return -1;
      }
      strcat(tmp," ");
   }
   cur_new = TRUE;
   cur_line[0] = '\0';

   // Here take away the bracket
   while (strlen(tmp) && tmp[strlen(tmp)-1] != '}')
      tmp[strlen(tmp)-1] = '\0';
   tmp[strlen(tmp)-1] = '\0';

   /*
      here is an error in PDIF 4.51
      {T ".062"" -1800 -3463}
   */
   STRREV(tmp);
   lp = strtok(tmp," \t");
   strcpy(wtmp,lp);
   STRREV(wtmp);
   y = cnv_unit(wtmp);
   lp = strtok(NULL," \t");
   strcpy(wtmp,lp);
   STRREV(wtmp);
   x = cnv_unit(wtmp);

   lp = strtok(NULL,"\n");
   strcpy(prosa,lp);
   // Here take away the reversed {T until start of text
   while (strlen(prosa) && prosa[strlen(prosa)-1] != '"')
      prosa[strlen(prosa)-1] = '\0';
   prosa[strlen(prosa)-1] = '\0';
   STRREV(prosa);
   while (strlen(prosa) && prosa[strlen(prosa)-1] != '"')
      prosa[strlen(prosa)-1] = '\0';
   prosa[strlen(prosa)-1] = '\0';
   clean_text(prosa);

   layername = layer[G.cur_layer].layer_name;
   /* if smdpin change PIN -> PINTOP
                       PAD -> PADTOP
   */ 
   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   double w = G.cur_textsize * 6.0 / 8.0;
   layernr = Graph_Level(get_maplayer(layername), "", 0);

   // normalize
   normalize_text(&x,&y,G.cur_textjust,
                  G.cur_textrot,
                  G.cur_textmirror,
                  G.cur_textsize,
                  w * strlen(prosa));

   int   prop = TRUE;
   DataStruct *d = Graph_Text(layernr,prosa,x,y,G.cur_textsize,w,DegToRad(G.cur_textrot),
               0L, prop, 
               G.cur_textmirror,  
               0, FALSE, 0 /* pen width is small width */,0); // prop flag, mirror flag, oblique angle in deg

   // can return NULL if prosa is empty
   if (!cur_layout_mode)   // schematic has no mirror layers
   {
      if (d)   d->getText()->setMirrorDisabled(true);
   }

   free(tmp);

   // Do this to get back into the scanner
   push_tok();
   strcpy(token,"}");
   return 1;
}

/*****************************************************************************/
/* 
   Dimension Text string.      
   needs its own because it uses dimtextrot
*/
static int graph_dimtext()
{
   double   x,y;
   char     prosa[256+1];
   char     *tmp,*lp;
   char     wtmp[100];
   CString  layername;
   int      layernr;

   if ((tmp = (char *)malloc(1000+1)) == NULL)  // for HPUX char *
   {
         MemErrorMessage(__FILE__, __LINE__);
   }
   tmp[0] = '\0';
   while ((strlen(tmp) + strlen(cur_line)) < 1000)
   {
      strcat(tmp,cur_line);
      if (tmp[strlen(tmp)-1] == '\n')  tmp[strlen(tmp)-1] = '\0';
      if (tmp[strlen(tmp)-1] == '}')   break;
      if (!get_line(cur_line,MAX_LINE))
      {
         fprintf(ferr,"Error in graph_dimtext\n");
         return -1;
      }
      strcat(tmp," ");
   }
   cur_new = TRUE;
   cur_line[0] = '\0';

   // Here take away the bracket
   while (strlen(tmp) && tmp[strlen(tmp)-1] != '}')
      tmp[strlen(tmp)-1] = '\0';
   tmp[strlen(tmp)-1] = '\0';

   /*
      here is an error in PDIF 4.51
      {T ".062"" -1800 -3463}
   */
   STRREV(tmp);
   lp = strtok(tmp," \t");
   strcpy(wtmp,lp);
   STRREV(wtmp);
   y = cnv_unit(wtmp);
   lp = strtok(NULL," \t");
   strcpy(wtmp,lp);
   STRREV(wtmp);
   x = cnv_unit(wtmp);

   lp = strtok(NULL,"\n");
   strcpy(prosa,lp);
   // Here take away the reversed {T until start of text
   while (strlen(prosa) && prosa[strlen(prosa)-1] != '"')
      prosa[strlen(prosa)-1] = '\0';
   prosa[strlen(prosa)-1] = '\0';
   STRREV(prosa);
   while (strlen(prosa) && prosa[strlen(prosa)-1] != '"')
      prosa[strlen(prosa)-1] = '\0';
   prosa[strlen(prosa)-1] = '\0';

   clean_text(prosa);

   layername = layer[G.cur_layer].layer_name;
   /* if smdpin change PIN -> PINTOP
                       PAD -> PADTOP
   */ 
   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   double w = G.cur_textsize;
   layernr = Graph_Level(get_maplayer(layername), "", 0);

   // normalize
   normalize_text(&x,&y,G.cur_textjust,
                  G.cur_dimtextrot,
                  G.cur_mirror,
                  G.cur_textsize,
                  w * strlen(prosa));

   int   prop = TRUE;

   Graph_Text(layernr,prosa,x,y,G.cur_textsize,w,DegToRad(G.cur_dimtextrot),
               0L, prop, 
               0,    // no text mirror in dimension
               0, FALSE, 0,0); // prop flag, mirror flag, oblique angle in deg
   free(tmp);

   // Do this to get back into the scanner
   push_tok();
   strcpy(token,"}");
   return 1;
}

/*****************************************************************************/
/* 

*/                         
static int get_derivedrill(const char *layname)
{
   int   i;

   for (i=0;i<derivedrillcnt;i++)
   {
      if (derivedrillarray[i]->layername.CompareNoCase(layname) == 0)
         return i;
   }

   return -1;
}

/*****************************************************************************/
/* 
*/
static int graph_arrow()
{
   if (!get_tok())   return p_error();

   PDIF_arrowname *c = new PDIF_arrowname;
   arrownamearray.SetAtGrow(arrownamecnt, c);
   arrownamecnt++;   
   c->compname = token;

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

/*****************************************************************************/
/* 
*/
static int ignorepadlayer(const char *lname)
{
   int   i;

   for (i=0;i<ignore_pad_layer_cnt;i++)
   {
      if (!STRCMPI(ignore_pad_layer[i].name, lname))
         return 1;
   }

   return 0;
}

/*****************************************************************************/
/* 
   Series of lines.     
*/
static int graph_line()
{
   int      repeat = TRUE,i;
   int      layernr;
   CString  layername;
   Point2   p1, p2;


   if (pad_def_flg && BUILD_PADSTACK == 'F') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }
   layernr = Graph_Level(get_maplayer(layername), "", 0);

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   poly_l.RemoveAll();
   polycnt = 0;

   while (repeat)
   {
      if (!get_tok())   return p_error();
      p1.x = cnv_unit(token);
      if (!get_tok())   return p_error();
      p1.y  = cnv_unit(token);

      poly_l.SetAtGrow(polycnt,p1);
      polycnt++;

      if (!get_tok())   return p_error();
      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_SQUIGLY)
         repeat = FALSE;
      push_tok();
   }

   if (pad_def_flg && convert_padline_2_app)
   {
      if (G.cur_width == 0)
      {
         // do nothing if linewidth == 0;
      }
      else
      if (polycnt == 2)
      {
         // simple aperture
         p1 = poly_l.ElementAt(0);
         p2 = poly_l.ElementAt(1);

         double   x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
         double len, x, y, rot;
         // here make an aperture
         // Find an aperture with same shape and size without care about name
         // If found return pointer or create one.
         len = round_accuracy(sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))) , accuracy);
         rot = ArcTan2( (y2 - y1), (x2 - x1) );
         x = offset_app(x1, x2);
         y = offset_app(y1, y2);


         // must be named and no width
         BlockStruct *b = Graph_FindAperture(cur_filenum, T_OBLONG, len, G.cur_width, 0.0, 0.0, rot, true, false);
         Graph_Block_Reference(b->getName(), NULL, 0, x, y, 0.0, 0, 1.0, layernr, TRUE);
      }
      else
      {
         // complex apertue
         DataStruct *d = Graph_PolyStruct(layernr,0L, FALSE);  // poly is always with 0
         d->setGraphicClass(get_layerclass(layername));

         if (cur_netname.GetLength())
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
               VT_STRING,cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
            d->setGraphicClass(GR_CLASS_ETCH);
         }

         p1 = poly_l.ElementAt(0);
         p2 = poly_l.ElementAt(polycnt-1);
   
         int   close = (p1.x == p2.x && p1.y == p2.y);
         Graph_Poly(NULL,G.cur_widthindex, 0,0, close);

         for (i=0;i<polycnt;i++)
         {  
            p1 = poly_l.ElementAt(i);

            // eliminate null segment 
            if (!i || p1.x != p2.x || p1.y != p2.y)
            {
               Graph_Vertex(p1.x, p1.y, 0.0);    // p1 coords
           }
            p2 = p1;
         }
      }
   }
   else
   {
      /* here check for pin touch */
      int pinnr = pinnr_from_poly(layername, 0); // pinnr is only done in sub_def
      if (pinnr && !pinlist[pinnr-1].found)  // if .found, that means that the PT was a good pin. 
      {
         if (polycnt == 2)
         {
            // simple aperture
            p1 = poly_l.ElementAt(0);
            p2 = poly_l.ElementAt(1);

            double   x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
            double   len, rot, x,y;
            // here make an aperture
            // Find an aperture with same shape and size without care about name
            // If found return pointer or create one.
   
            // normalize aperture
            x1 = x1 - pinlist[pinnr-1].x;
            y1 = y1 - pinlist[pinnr-1].y;
            x2 = x2 - pinlist[pinnr-1].x;
            y2 = y2 - pinlist[pinnr-1].y;
   
            len = round_accuracy(sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))), accuracy);
            rot = ArcTan2( (y2 - y1), (x2 - x1) );
            x = offset_app(x1, x2); // this is the offset
            y = offset_app(y1, y2); // this is the offset
   
            // must be named and no width
            BlockStruct *b = Graph_FindAperture(cur_filenum, T_OBLONG, len, G.cur_width, 0, 0, rot, true, false);

            PDIF_comppadstack *c = new PDIF_comppadstack;
            comppadstackarray.SetAtGrow(comppadstackcnt,c);  
            comppadstackcnt++;   

            c->x = x+pinlist[pinnr-1].x;
            c->y = y+pinlist[pinnr-1].y;
            c->offsetx = x;
            c->offsety = y;
            c->layer = layernr;  
            c->pinnr = pinnr;
            c->bnum = b->getBlockNumber();
         } // else more than 2 lines found 
         else
         {
            // make a complex aperture
            fprintf(ferr, "Padstack contains multiple line elements -> not implemented at %ld\n", ifp_line);
            display_error++;
         }
      }  // if pinnr
      else
      {  // not pinnr
         DataStruct *d = Graph_PolyStruct(layernr,0L, FALSE);  // poly is always with 0
         d->setGraphicClass(get_layerclass(layername));

         if (cur_netname.GetLength())
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
               VT_STRING, cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
            d->setGraphicClass(GR_CLASS_ETCH);
         }

         p1 = poly_l.ElementAt(0);
         p2 = poly_l.ElementAt(polycnt-1);
   
         int   close = (p1.x == p2.x && p1.y == p2.y);
         Graph_Poly(NULL,G.cur_widthindex, 0,0, close);

         for (i=0;i<polycnt;i++)
         {  
            p1 = poly_l.ElementAt(i);

            // eliminate null segment 
            if (!i || p1.x != p2.x || p1.y != p2.y)
            {
               Graph_Vertex(p1.x, p1.y, 0.0);    // p1 coords
           }
            p2 = p1;
         }
      } // not pinnr
   } // end paddefflg

   poly_l.RemoveAll();
   polycnt = 0;
   return 1;
}

/*****************************************************************************/
/* 
   Series of wires
*/
static int graph_wire()
{
   int      repeat = TRUE,i;
   int      cur_layernr;
   CString  layername;
   Point2   p1, p2;

   layername = layer[G.cur_layer].layer_name;
   layer[G.cur_layer].electrical = TRUE;  // always electrical if part of a wire.

   cur_layernr = Graph_Level(get_maplayer(layername), "", 0);
   
   poly_l.RemoveAll();
   polycnt = 0;

   while (repeat)
   {
      if (!get_tok())   return p_error();
      p1.x = cnv_unit(token);
      if (!get_tok())   return p_error();
      p1.y  = cnv_unit(token);

      poly_l.SetAtGrow(polycnt,p1);
      polycnt++;

      if (!get_tok())   return p_error();
      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_SQUIGLY)
         repeat = FALSE;
      push_tok();
   }

   DataStruct *d = Graph_PolyStruct(cur_layernr,0L, FALSE); // poly is always with 0
   d->setGraphicClass(get_layerclass(layername));

   if (cur_netname.GetLength())
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
         VT_STRING,cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
      d->setGraphicClass(GR_CLASS_ETCH);
   }

   p1 = poly_l.ElementAt(0);
   p2 = poly_l.ElementAt(polycnt-1);

   int   close = (p1.x == p2.x && p1.y == p2.y);
   Graph_Poly(NULL,G.cur_widthindex, 0,0, close);

   for (i=0;i<polycnt;i++)
   {
      p1 = poly_l.ElementAt(i);

      /* eliminate null segment */
      if (!i || p1.x != p2.x || p1.y != p2.y)
      {
         Graph_Vertex(p1.x, p1.y, 0.0);    // p1 coords
      }
      p2 = p1;
   }

   poly_l.RemoveAll();
   polycnt = 0;

   return 1;
}

/*****************************************************************************/
/* 
*/
static int graph_dimension()
{
   CString  layername;
   int      layernr;
   CString  dimname;

   if (!get_tok())   return p_error();
   dimname = token;

   layername = layer[G.cur_layer].layer_name;

   layernr = Graph_Level(get_maplayer(layername), "", 0);

   // Graph_Block_On
   loop_command(dim_lst,SIZ_DIM_LST);
   // Graph_Block_Off
   // Graph_Block_Reference
   return 1;
}

/*****************************************************************************/
/* 
   Series of lines.     
*/
static int graph_poly()
{
   CString  layername;
   int      layernr;

   if (pad_def_flg && BUILD_PADSTACK == 'G') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   if (pad_def_flg)
   {
      // start a block
      CString  pshapename, pxshapename;

      pshapename.Format("COMPLEX_%d", ++complexcnt);
      Graph_Block_On(GBO_APPEND,pshapename, cur_filenum, 0);
      layernr = Graph_Level(get_maplayer(layername), "", 0);
      DataStruct *d = Graph_PolyStruct(Graph_Level("0","",1),0L,FALSE);
      d->setGraphicClass(get_layerclass(layername));
      loop_command(poly_lst,SIZ_POLY_LST);
      Graph_Block_Off();

      pxshapename.Format("PADSHAPE_%s",pshapename);
      Graph_Complex(cur_filenum, pxshapename, 0, pshapename, 0.0, 0.0, 0.0);
      Graph_Block_Reference(pxshapename, NULL, cur_filenum, 0.0, 0.0, 0.0, 0 , 1.0, layernr, TRUE);
      BlockStruct *b = Graph_Block_On(GBO_APPEND,pxshapename,cur_filenum,0);
      b->setBlockType(BLOCKTYPE_PADSHAPE);
      Graph_Block_Off();
   }
   else
   {
      layernr = Graph_Level(get_maplayer(layername), "", 0);
      DataStruct  *d = Graph_PolyStruct(layernr,0L,FALSE);
      d->setGraphicClass(get_layerclass(layername));
      cur_pin_in_poly = 0;

      // remember current DataList;
      CDataList *oldList = GetCurrentDataList();

      if (strlen(cur_netname))
      {
         d->setGraphicClass(GR_CLASS_ETCH);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
            VT_STRING,cur_netname.GetBuffer(0),SA_APPEND, NULL); // x, y, rot, height
      }
      loop_command(poly_lst,SIZ_POLY_LST);

      if (cur_pin_in_poly)
      {
         // the idea is: The Graph_Poly was done by now, but on graph_ol it was normalized to the pin.
         // make a complex aperture and move the polygon from the file to the complex apertue. Also
         // change the layer and make a complex aperture insert layer.

         // start a block
         CString  pshapename, pxshapename;

         pshapename.Format("COMPLEX_%d", ++complexcnt);
         Graph_Block_On(GBO_APPEND,pshapename, cur_filenum, 0);

         // move data
         CDataList *newList = GetCurrentDataList();
         oldList->RemoveAt(oldList->Find(d));
         newList->AddTail(d);
         d->setLayerIndex(Graph_Level("0","",1));
         Graph_Block_Off();

         pxshapename.Format("PADSHAPE_%s", pshapename);
         Graph_Complex(cur_filenum, pxshapename, 0, pshapename, 0.0, 0.0, 0.0);

         BlockStruct *b = Graph_Block_On(GBO_APPEND,pxshapename,cur_filenum,0);
         b->setBlockType(BLOCKTYPE_PADSHAPE);
         Graph_Block_Off();


         PDIF_comppadstack *c = new PDIF_comppadstack;
         comppadstackarray.SetAtGrow(comppadstackcnt,c);  
         comppadstackcnt++;   
         c->x = pinlist[cur_pin_in_poly-1].x;
         c->y = pinlist[cur_pin_in_poly-1].y;
         c->offsetx = 0;
         c->offsety = 0;
         c->layer = layernr;  
         c->pinnr = cur_pin_in_poly;
         c->bnum = b->getBlockNumber();
      }
      cur_pin_in_poly = 0;
   }

   return 1;
}

/*****************************************************************************/
/* 
   Poly outline 
*/
static int poly_ol()
{
   int      repeat = TRUE;
   CString  layername;
   int      i,pinnr;
   Point2   p1, p2;

   layername = layer[G.cur_layer].layer_name;

   if (!get_tok())   return p_error();   /* fill flag */

   poly_l.RemoveAll();
   polycnt = 0;

   while (repeat)
   {
      if (!get_tok())   return p_error();
      p1.x = cnv_unit(token);
      if (!get_tok())   return p_error();
      p1.y = cnv_unit(token);

      poly_l.SetAtGrow(polycnt,p1);
      polycnt++;

      if (!get_tok())   return p_error();
      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_SQUIGLY)
         repeat = FALSE;
      push_tok();
   }

   p1 = poly_l.ElementAt(0);
   p2 = poly_l.ElementAt(polycnt-1);

   // check for close poly
   if (p1.x != p2.x || p1.y != p2.y)
   {
      poly_l.SetAtGrow(polycnt,p1);
      polycnt++;
   }

   /* here check for pin touch */
   pinnr = pinnr_from_poly(layername, 0.0);

   if (cur_pin_in_poly || (pinnr && !pinlist[pinnr-1].found))  // if .found, that means that the PT was a good pin. 
   {
      cur_pin_in_poly = pinnr;

      // normalize the poly. the complete graph_poly is on graph_poly moved to a complex aperture.
      for (i=0;i<polycnt;i++)
      {
         p1 = poly_l.ElementAt(i);
         p1.x = p1.x - pinlist[pinnr-1].x;
         p1.y = p1.y - pinlist[pinnr-1].y;
         poly_l.SetAtGrow(i,p1);
      }
   }

   Graph_Poly(NULL,G.cur_polywidthindex, 1,0,1);
   for (i=0;i<polycnt;i++)
   {
      p1 = poly_l.ElementAt(i);
      Graph_Vertex(p1.x,p1.y,0);
   }

   poly_l.RemoveAll();
   polycnt = 0;

   return 1;
}

/*****************************************************************************/
/* 
   Poly void outline 
*/
static int poly_pv()
{
   int      repeat = TRUE;
   CString  layername;
   int      i;
   Point2   p1,p2;

   layername = layer[G.cur_layer].layer_name;

   poly_l.RemoveAll();
   polycnt = 0;
   while (repeat)
   {
      if (!get_tok())   return p_error();
      p1.x = cnv_unit(token);
      if (!get_tok())   return p_error();
      p1.y = cnv_unit(token);

      poly_l.SetAtGrow(polycnt,p1);
      polycnt++;

      if (!get_tok())   return p_error();
      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_SQUIGLY)
         repeat = FALSE;
      push_tok();

   }

   // check for close poly
   p1 = poly_l.ElementAt(0);
   p2 = poly_l.ElementAt(polycnt-1);

   // check for close poly
   if (p1.x != p2.x || p1.y != p2.y)
   {
      poly_l.SetAtGrow(polycnt,p1);
      polycnt++;
   }

   Graph_Poly(NULL,G.cur_polywidthindex, 1, 1, 1); // void
   for (i=0;i<polycnt;i++)
   {
      p1 = poly_l.ElementAt(i);
      Graph_Vertex(p1.x,p1.y,0);
   }

   poly_l.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/* 
   Rectangle.  
*/
static int graph_rect()
{
   double   x1,y1,x2,y2;
   CString  layername;
   int      layernr;
   Point2   p1;

   if (pad_def_flg && BUILD_PADSTACK == 'F') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   poly_l.RemoveAll();
   polycnt = 0;

   if (!get_tok())   return p_error();
   x1 = cnv_unit(token);
   if (!get_tok())   return p_error();
   y1 = cnv_unit(token);
   if (!get_tok())   return p_error();
   x2 =cnv_unit(token);
   if (!get_tok())   return p_error();
   y2 = cnv_unit(token);

   p1.x = x1;
   p1.y = y1;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x2;
   p1.y = y1;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x2;
   p1.y = y2;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x1;
   p1.y = y2;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x1;
   p1.y = y1;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   layernr = Graph_Level(get_maplayer(layername), "", 0);

   if (pad_def_flg && convert_padrect_2_app)
   {
      double w,h,x,y;
      // here make an aperture
      // Find an aperture with same shape and size without care about name
      // If found return pointer or create one.
      w = round_accuracy(fabs(x2 - x1), accuracy);
      h = round_accuracy(fabs(y2 - y1), accuracy);
      x = offset_app(x1, x2);
      y = offset_app(y1, y2);

      int shape = T_RECTANGLE;
      if (fabs(w - h) < SMALLNUMBER)
      {
         shape = T_SQUARE;
         h = 0;
      }

      // must be named and no width
      BlockStruct *b = Graph_FindAperture(cur_filenum, shape, w, h, 0.0, 0.0, 0.0, true, false);
      Graph_Block_Reference(b->getName(), NULL, 0, x, y, 0.0, 0, 1.0, layernr, TRUE);
   }
   else
   {

      /* here check for pin touch */
      int pinnr = pinnr_from_poly(layername, 0); // pinnr is only done in sub_def

      if (pinnr && !pinlist[pinnr-1].found)  // if .found, that means that the PT was a good pin. 
      {
         double w,h,x,y;
         // here make an aperture
         // Find an aperture with same shape and size without care about name
         // If found return pointer or create one.

         // normalize aperture
         x1 = x1 - pinlist[pinnr-1].x;
         y1 = y1 - pinlist[pinnr-1].y;
         x2 = x2 - pinlist[pinnr-1].x;
         y2 = y2 - pinlist[pinnr-1].y;

         w = round_accuracy(fabs(x2 - x1), accuracy);
         h = round_accuracy(fabs(y2 - y1), accuracy);
         x = offset_app(x1, x2);
         y = offset_app(y1, y2);

         int shape = T_RECTANGLE;
         if (fabs(w - h) < SMALLNUMBER)
         {
            shape = T_SQUARE;
            h = 0;
         }

         // must be named and no width
         BlockStruct *b = Graph_FindAperture(cur_filenum, shape, w, h, 0.0, 0.0, 0.0, true, false);

         PDIF_comppadstack *c = new PDIF_comppadstack;
         comppadstackarray.SetAtGrow(comppadstackcnt,c);  
         comppadstackcnt++;   

         c->x = x+pinlist[pinnr-1].x;
         c->y = y+pinlist[pinnr-1].y;
         c->offsetx = x;
         c->offsety = y;
         c->layer = layernr;  
         c->pinnr = pinnr;
         c->bnum = b->getBlockNumber();
      }
      else
      {
         DataStruct *d = Graph_PolyStruct(layernr, 0, FALSE);
         d->setGraphicClass(get_layerclass(layername));
         Graph_Poly(NULL, G.cur_widthindex, 0, 0, 1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);
      }
   }

   return 1;
}

/*****************************************************************************/
/* 
   Filled rectangle.    
*/
static int graph_fr()
{
   double   x1,y1,x2,y2;
   CString  layername;
   int      layernr;
   Point2   p1;

   if (pad_def_flg && BUILD_PADSTACK == 'F') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   poly_l.RemoveAll();
   polycnt = 0;

   if (!get_tok())   return p_error();
   x1 = cnv_unit(token);
   if (!get_tok())   return p_error();
   y1 = cnv_unit(token);
   if (!get_tok())   return p_error();
   x2 = cnv_unit(token);
   if (!get_tok())   return p_error();
   y2 = cnv_unit(token);

   p1.x = x1;
   p1.y = y1;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x2;
   p1.y = y1;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x2;
   p1.y = y2;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x1;
   p1.y = y2;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;
   p1.x = x1;
   p1.y = y1;
   poly_l.SetAtGrow(polycnt,p1);
   polycnt++;

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   layernr = Graph_Level(get_maplayer(layername), "", 0);

   if (pad_def_flg && convert_padrect_2_app)
   {
      double w,h,x,y;
      // here make an aperture
      // Find an aperture with same shape and size without care about name
      // If found return pointer or create one.
      w = round_accuracy(fabs(x2 - x1), accuracy);
      h = round_accuracy(fabs(y2 - y1), accuracy);
      x = offset_app(x1, x2);
      y = offset_app(y1, y2);

      
      int shape = T_RECTANGLE;
      if (fabs(w - h) < SMALLNUMBER)
      {
         shape = T_SQUARE;
         h = 0;
      }

      // must be named and no width allowed
      BlockStruct *b = Graph_FindAperture(cur_filenum, shape, w, h, 0.0, 0.0, 0.0, true, false);
      Graph_Block_Reference(b->getName(), NULL, 0, x, y, 0.0, 0, 1.0, layernr, TRUE);
   }
   else
   {
      /* here check for pin touch */
      int pinnr = pinnr_from_poly(layername, 0);   // pinnr is only returned in sub_def

      if (pinnr && !pinlist[pinnr-1].found)  // if .found, that means that the PT was a good pin. 
      {
         double w,h,x,y;
         // here make an aperture
         // Find an aperture with same shape and size without care about name
         // If found return pointer or create one.

         // normalize aperture
         x1 = x1 - pinlist[pinnr-1].x;
         y1 = y1 - pinlist[pinnr-1].y;
         x2 = x2 - pinlist[pinnr-1].x;
         y2 = y2 - pinlist[pinnr-1].y;

         w = round_accuracy(fabs(x2 - x1), accuracy);
         h = round_accuracy(fabs(y2 - y1), accuracy);
         x = offset_app(x1, x2);
         y = offset_app(y1, y2);

         // must be named and no width
         int shape = T_RECTANGLE;
         if (fabs(w - h) < SMALLNUMBER)
         {
            shape = T_SQUARE;
            h = 0;
         }

         BlockStruct *b = Graph_FindAperture(cur_filenum, shape, w, h, 0.0, 0.0, 0.0, true, false);

         PDIF_comppadstack *c = new PDIF_comppadstack;
         comppadstackarray.SetAtGrow(comppadstackcnt,c);  
         comppadstackcnt++;   

         c->x = x+pinlist[pinnr-1].x;
         c->y = y+pinlist[pinnr-1].y;
         c->offsetx = x;
         c->offsety = y;
         c->layer = layernr;  
         c->pinnr = pinnr;
         c->bnum = b->getBlockNumber();
      }
      else
      {
         DataStruct *d = Graph_PolyStruct(layernr, 0, FALSE);
         d->setGraphicClass(get_layerclass(layername));
         Graph_Poly(NULL, G.cur_widthindex, TRUE, 0, 1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);
      }
   }

   return 1;
}

/*****************************************************************************/
/* 
   Old Arc.     
*/
static int graph_a()
{
   int      r;
   int      x,y;
   int      sa,ea;
   CString  layername;
   
   if (pad_def_flg && BUILD_PADSTACK == 'F') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   if (!get_tok())   return p_error();
   x = atoi(token);
   if (!get_tok())   return p_error();
   y = atoi(token);
   if (!get_tok())   return p_error();
   r = atoi(token);
   if (!get_tok())   return p_error();
   sa = atoi(token);
   if (!get_tok())   return p_error();
   ea = atoi(token);
   return 1;
}

/*****************************************************************************/
/* 
   Arc.     
*/
static int graph_arc()
{
   double   cx,cy,sx,sy,ex,ey;
   double   sa,da;
   double   r;
   CString  layername;
   int      layernr;
   DataStruct *d = NULL;

   if (pad_def_flg && BUILD_PADSTACK == 'F') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }
   
   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   if (!get_tok())   return p_error();
   cx = cnv_unit(token);
   if (!get_tok())   return p_error();
   cy = cnv_unit(token);
   if (!get_tok())   return p_error();
   sx = cnv_unit(token);
   if (!get_tok())   return p_error();
   sy = cnv_unit(token);
   if (!get_tok())   return p_error();
   ex = cnv_unit(token);
   if (!get_tok())   return p_error();
   ey = cnv_unit(token);

   if (sx == ex && sy == ey)
   {
      // here is a circle
      r = sqrt((sx-cx)*(sx-cx) + (sy-cy)*(sy-cy));
      layernr = Graph_Level(get_maplayer(layername), "", 0);
      d = Graph_Circle(layernr,cx,cy,r,0L, G.cur_widthindex , FALSE, FALSE); 
      d->setGraphicClass(get_layerclass(layername));
   }
   else
   {
      ArcCenter2(sx,sy,ex,ey,cx,cy,&r,&sa,&da,0);
      layernr = Graph_Level(get_maplayer(layername), "", 0);
      d = Graph_Arc(layernr,cx,cy,r,sa, da, 0L, G.cur_widthindex , FALSE); 
      d->setGraphicClass(get_layerclass(layername));
   }

   if (cur_netname.GetLength())
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
         VT_STRING,cur_netname.GetBuffer(0),SA_OVERWRITE, NULL); // x, y, rot, height
      d->setGraphicClass(GR_CLASS_ETCH);
   }

   return 1;
}

/*****************************************************************************/
/* 
   Circle.     
*/                         
static int graph_c()
{
   double   x,y,r;
   CString  layername;
   int      layernr;

   if (pad_def_flg && BUILD_PADSTACK == 'F') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }
   
   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error();
   y = cnv_unit(token);
   if (!get_tok())   return p_error();
   r = cnv_unit(token);

   if (r < SMALLNUMBER) // zero radius
   {
      return 1;
   }

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername,"PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername,"PADTOP";
   }

   layernr = Graph_Level(get_maplayer(layername), "", 0);
   if (pad_def_flg && convert_padcircle_2_app)
   {
      // here make an aperture
      // Find an aperture with same shape and size without care about name
      // If found return pointer or create one.
      r = round_accuracy(r*2 + G.cur_width, accuracy);

      // must be named and no width
      BlockStruct *b = Graph_FindAperture(cur_filenum, T_ROUND, r, 0.0, 0.0, 0.0, 0.0, true, false);
      Graph_Block_Reference(b->getName(), NULL, 0, x, y, 0.0, 0, 1.0, layernr, TRUE);
   }
   else
   {

      /* here check for pin touch */
      int pinnr = pinnr_from_circle(layername, x, y, r); // pinnr is only returned in sub_def

      if (pinnr && !pinlist[pinnr-1].found)  // if .found, that means that the PT was a good pin. 
      {
         // here make an aperture
         // Find an aperture with same shape and size without care about name
         // If found return pointer or create one.
         r = round_accuracy(r, accuracy);
         // must be named and no width
         BlockStruct *b = Graph_FindAperture(cur_filenum, T_ROUND, r*2, 0.0, 0.0, 0.0, 0.0, true, false);

         PDIF_comppadstack *c = new PDIF_comppadstack;
         comppadstackarray.SetAtGrow(comppadstackcnt,c);  
         comppadstackcnt++;   

         c->x = pinlist[pinnr-1].x;
         c->y = pinlist[pinnr-1].y;
         c->offsetx = 0.0;
         c->offsety = 0.0;
         c->layer = layernr;  
         c->pinnr = pinnr;
         c->bnum = b->getBlockNumber();
      }
      else
      {
         DataStruct *d = Graph_Circle(layernr,x,y,r,0L, G.cur_widthindex , FALSE, FALSE); 
         d->setGraphicClass(get_layerclass(layername));
      }
   }

   if (pad_def_flg)
   {
      int ptr = get_derivedrill(layername);
      if (ptr > -1)
         G.cur_drillsize = r * derivedrillarray[ptr]->percent/100;
   }

   return 1;
}

/****************************************************************************/
/*
  Flash.
*/
static int graph_fl()
{
   double   x,y;
   int      app, apptr, pinnr = 0, rot = 0;
   CString  layername;
   int      layernr;

   if (pad_def_flg && BUILD_PADSTACK == 'G') // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   if (comp_def_flg && BUILD_COMPONENT == 'G')  // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   layername = layer[G.cur_layer].layer_name;

   if (pad_def_flg && ignorepadlayer(layername))   // no flash in pads if grapghic in build_padstack
   {
      fskip();
      return 1;
   }

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error();
   y = cnv_unit(token);
   if (!get_tok())   return p_error();
   app = atoi(token);
   rot = 0;

   if (G.cur_pdifversion >= 7)
   {
      if (!get_tok())   return p_error();  // ?? This is new in Version 7
      rot = 90 * atoi(token);
   }

   if (G.cur_smd)
   {
      if (!STRCMPI(layername,"PIN"))   layername = "PINTOP";
      if (!STRCMPI(layername,"PAD"))   layername = "PADTOP";
   }

   if ((apptr = get_apptr(app)) > -1)
   {
      CString  appname;
      appname.Format("APP_%d",aperture[apptr].number);
      aperture[apptr].used = TRUE;

      int err; // need to create here.
      Graph_Aperture(cur_filenum, appname, T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

      layernr = Graph_Level(get_maplayer(layername), "", 0);
      Graph_Block_Reference(appname, NULL, 0, x, y, DegToRad(rot), 0, 1.0, layernr, TRUE);

      if (pad_def_flg)
      {
         //Aperture aa = aperture[apptr];
         int ptr = get_derivedrill(layername);
         if (ptr > -1)
            G.cur_drillsize = aperture[apptr].ow * derivedrillarray[ptr]->percent/100;
      }
   }
   else
   {
#ifdef _DEBUG
      fprintf(ferr, "DEBUG: Can not find Aperture [%d] at %ld\n",app, ifp_line);
      display_error++;
#endif
   }


   return 1;
}

/*****************************************************************************/
/* 
   Circle.     
*/
static int poly_cv()
{
   double   x,y,r;

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error();
   y = cnv_unit(token);
   if (!get_tok())   return p_error();
   r = cnv_unit(token);

   Graph_Poly(NULL,G.cur_polywidthindex, 1,1,1);   // void
   Graph_Vertex(x-r,y,1);
   Graph_Vertex(x+r,y,1);
   Graph_Vertex(x-r,y,0);

   return 1;
}

/****************************************************************************/
/*
   Process a list of Nets.
*/
static int det_net_def()
{
   return loop_command(net_lst,SIZ_NET_LST);
}

/****************************************************************************/
/*
   Process a Net.
*/
static int p_net_name()
{
   if (!get_tok())   return p_error();                    /* Get net name.     */
   cur_netname = token;
   loop_command(nnm_lst,SIZ_NNM_LST);
   cur_netname = "";
   return 1;
}

/****************************************************************************/
/*
   Process a list of wires and vias.
*/
static int nnm_dg()
{
   loop_command(graph_lst,SIZ_GRAPH_LST);
   return 1;
}

/****************************************************************************/
/*
   Process a list of net attributes.
*/
static int nnm_attr()
{
   loop_command(netatr_lst,SIZ_NETATR_LST);
   return 1;
}

/****************************************************************************/
/*
   Process a via.
*/
static int ddg_nm()
{
   int      layernr;
   double   x,y, x1, y1;
   CString  layername;

   if (!get_tok()) return p_error();
   x = cnv_unit(token);
   if (!get_tok()) return p_error();
   y = cnv_unit(token);

   x1 = x;
   y1 = y;

   layername = layer[G.cur_layer].layer_name;

   x = y = 0;
   normalize_text(&x,&y,G.cur_textjust,
                  G.cur_textrot,
                  0,
                  G.cur_textsize,
                  (G.cur_textsize * strlen(cur_netname) * (6.0/8.0)));
   x1 += x;
   y1 += y;

   layernr = Graph_Level(get_maplayer(layername), "", 0);
   double w = G.cur_textsize * 6.0 / 8.0;
   int   prop = TRUE;
   Graph_Text(layernr,cur_netname, x1, y1, G.cur_textsize, w, DegToRad(G.cur_textrot),
               0L, prop, 
               G.cur_textmirror,  
               0, FALSE, 0 /* pen width is small width */,0); // prop flag, mirror flag, oblique angle in deg
   return 1;
}

/******************************************************************************
* ddg_via
   Process a via.
*/
static int ddg_via()
{
   if (!get_tok()) return p_error();
   double x = cnv_unit(token);

   if (!get_tok()) return p_error();
   double y = cnv_unit(token);

   if (!get_tok()) return p_error();
   int typ = atoi(token);

   // get_padname(typ);

   if (!cur_layout_mode)
   {
      // this is a junction point, but this is not defined in PDIF.
      strcpy(token, "{JP}");
      if (Graph_Block_Exists(doc, token, -1) == NULL)
      {
         double junctionwidth = 10 * Units_Factor(UNIT_MILS, pageunits);
         int err;
         char  *name = "{JP_DEF}";
         Graph_Aperture(cur_filenum, name, T_ROUND, junctionwidth, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         BlockStruct *b = Graph_Block_On(GBO_APPEND, token, cur_filenum, 0);
         b->setBlockType(BLOCKTYPE_SCHEM_JUNCTION);
         DataStruct *d = Graph_Block_Reference( name, NULL, cur_filenum, 0, 0, 0, 0, 1.0, Graph_Level("WIRES", "", 1), TRUE);
         Graph_Block_Off();      // just make sure it is there.
      }
   }
   else
   {
      Graph_Block_On(GBO_APPEND, token, cur_filenum, 0);
      Graph_Block_Off();      // just make sure it is there.
   }

   DataStruct *data = Graph_Block_Reference(token, NULL, -1, x, y, DegToRad(0), 0, 1.0, Graph_Level("0", "", 1), TRUE);
   if (!cur_layout_mode)
      data->getInsert()->setInsertType(insertTypeSchematicJunction);
   else
      data->getInsert()->setInsertType(insertTypeVia);

   if (strlen(cur_netname))
      doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, cur_netname.GetBuffer(0), SA_APPEND, NULL);

   return 1;
}

/****************************************************************************/
/*
   Process pad stack information.
*/
static int det_pad_stack()
{
   G.cur_smd = FALSE;
   return loop_command(pdf_lst,SIZ_PDF_LST);
}

/****************************************************************************/
/*
   Process a pad name.
*/
static int pdf_pad()
{
   /* Current pad G.cur_pad      */
   int      pnum;
   CString  name;

   if (!get_tok())   return p_error();                       /* Pad stack number. */
   pnum = atoi(token);

   if (!get_tok())   return p_error();                       /* Pad stack name.   */
   //elim_ext(token);   same name for pads can be the same name for components.
   name = token;

   if (G.max_dpads < MAX_PDEF)
   {
      def_pad[G.max_dpads].pdnum = pnum;
      def_pad[G.max_dpads].smd = FALSE;
      def_pad[G.max_dpads].used = FALSE;
      if ((def_pad[G.max_dpads].pstack_name = STRDUP(name)) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
      }
      G.max_dpads++;
   }

   if (!get_tok())   return p_error();
   if (tok_search(brk_lst,SIZ_BRK_LST) != BRK_B_SQUIGLY)
   {
      /* printf(errmsg[WAR_PAD_NAME]); */
   }
   else
      push_tok();
   return 1;
}

/****************************************************************************/
/*
   Process pad stack information.
*/
static int pdf_pad_def()
{
   int      p;
   CString  tmp;
   char     *lp;

   G.cur_smd = FALSE;

   // accel eda PDIF allows blanks in PAD_DEF
   tmp = cur_line;
	lp = strstr(cur_line,"PAD_DEF");

   cur_new = TRUE;
   cur_line[0] = '\0';

   strcpy(token,&lp[7]);   // strlen("PAD_DEF")

   tmp = token;
   tmp.TrimLeft();
   tmp.TrimRight();

   strcpy(token,tmp);

   // if (!get_tok())   return p_error();                       /* Pad name.   */
   //elim_ext(token);   same name for pads can be the same name for components.

   p = get_padnumber(token);
   if (p < 0)
   {
      // these are NON connected pads, which are not needed.
      fskip();
      return 1;
   }

   BlockStruct *b;
   b = Graph_Block_On(GBO_APPEND, token,cur_filenum,0);
   b->setBlockType(BLOCKTYPE_PADSTACK);

   int found;
   cur_padname = get_padname(p, &found);
   pad_def_flg = TRUE;
   G.cur_drillsize = 0;
   loop_command(pdd_lst,SIZ_PDD_LST);
   pad_def_flg = FALSE;

   if (G.cur_smd)
   {
      if ((p = get_defpad_ptr(cur_padname)) > -1)
         def_pad[p].smd = TRUE;
      doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE,
            NULL,
            SA_OVERWRITE, NULL); // x, y, rot, height
   }
   else
   {
      if (G.cur_drillsize > 0)
      {
         if ((p = get_defpad_ptr(cur_padname)) > -1)
         {
            def_pad[p].drill = G.cur_drillsize;
         }
      }
   }

   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
   Process shape of last pad defined.
*/
static int pdd_pic()
{
   G.cur_width = 0;
   G.cur_widthindex = 0;   // make it small width.

   return loop_command(graph_lst,SIZ_GRAPH_LST);
}

/****************************************************************************/
/*
   Process shape of last pad defined.
*/
static int pdd_atr()
{
   int res = loop_command(pddatr_lst,SIZ_PDDATR_LST);
   return res;
}

/****************************************************************************/
/*
   Process shape of last pad defined.
*/
static int pddatr_in()
{
   return loop_command(in_lst,SIZ_IN_LST);
}

/****************************************************************************/
/*
*/
static int netatr_in()
{
   return loop_command(nain_lst,SIZ_NAIN_LST);
}

/****************************************************************************/
/*
*/
static int netatr_ex()
{
   return loop_command(naex_lst,SIZ_NAEX_LST);
}

/****************************************************************************/
/*
   Process Component and instance definitions to generate a pin list.
*/
static int det_subcomp()
{
   int   i;

   i = loop_command(cmp_lst,SIZ_CMP_LST);
   return i;
}

/****************************************************************************/
/*
*/
static int sort_comppadstack_by_layer()
{
   int   i;
   int   done = FALSE;

   while (!done)
   {
      done = TRUE;
      for (i=1;i<comppadstackcnt;i++)
      {
         if (comppadstackarray[i-1]->layer > comppadstackarray[i]->layer)
         {
            PDIF_comppadstack *c = comppadstackarray[i-1];
            comppadstackarray[i-1] = comppadstackarray[i];
            comppadstackarray[i] = c;
            done = FALSE;
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int get_comppadnameptr(const char *p, int *found)
{
   int   i;

   for (i=0;i<comppadnamecnt;i++)
   {
      if (p == comppadnamearray[i]->stackname)
      {
         *found = TRUE;
         return i;
      }
   }

   PDIF_comppadname *c = new PDIF_comppadname;
   comppadnamearray.SetAtGrow(comppadnamecnt, c);
   comppadnamecnt++; 
   c->stackname = p;

   // fprintf(ferr,"Padstack [%s]\n",p);
   *found = FALSE;

   return comppadnamecnt-1;
}

/******************************************************************************
* cmp_comp_def
   Process a component.
*/
static int cmp_comp_def()
{
   int   i;
   CString  lname; // for debug

   pincnt = 0; /* if pinnames are not included */
   pinlistcnt = 0;
   G.cur_smd = FALSE;
   comppadstackcnt = 0;
   G.cur_ty =0;

   if (!get_tok())   return p_error();                    /* Component name.   */

   clean_libname(token);
   lname = cur_libname = token;

   BlockStruct *b;
   b = Graph_Block_On(GBO_APPEND,cur_libname,cur_filenum,0);

   if (cur_layout_mode)
      b->setBlockType(BLOCKTYPE_PCBCOMPONENT);
   else
      b->setBlockType(BLOCKTYPE_SYMBOL);

   if (shapenamecnt < MAX_SHAPE)
   {
      if ((shapename[shapenamecnt].name = STRDUP(cur_libname)) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
      }
      if ((shapename[shapenamecnt].pinname =
         (char **)calloc(MAX_COM_PINS,sizeof (char *))) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
      }

      comp_def_flg = TRUE;
      loop_command(cpd_lst,SIZ_CPD_LST);
      comp_def_flg = FALSE;

      if (pincnt)
      {
         if ((shapename[shapenamecnt].pinname =
         (char **)realloc(shapename[shapenamecnt].pinname,
                          pincnt*sizeof(char *))) == NULL)
         {
            MemErrorMessage(__FILE__, __LINE__);
         }
      }
      else
      {
         free(shapename[shapenamecnt].pinname);
      }
      shapename[shapenamecnt].pincnt = pincnt;
      shapenamecnt++;
   }
   else
   {
      fprintf(ferr,"Too many Shape definitions\n");
      return -1;
   }
   
   if (cur_layout_mode)
   {
      int   tbound = typeassign.GetUpperBound();
      if (G.cur_ty < tbound)
      {
         int t = typeassign.GetAt(G.cur_ty);
         if (t)   b->setBlockType(t);
      }
      else
      {
         // just to be compatible
         if (G.cur_ty == 0)
         {
            b->setBlockType(BLOCKTYPE_MECHCOMPONENT);
         }
      }  

      if (G.cur_smd)
      {
         doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height
      }
   } // only for layout

   if (G.cur_ty)
   {
      int   ty = G.cur_ty - 32000;
      doc->SetAttrib(&b->getAttributesRef(),doc->RegisterKeyWord("PDIF_TYPE", TRUE, VT_INTEGER),
            VT_INTEGER,
            &ty,  // there is an offset
            SA_OVERWRITE, NULL); // x, y, rot, height
   }

   Graph_Block_Off();

   if (G.cur_smd)
   {
      // here make sure that the padlist is flagged as SMD
      for (i=0;i<pinlistcnt;i++)
      {
         CString  pname;
         int      found, p; 
         pname = get_padname(pinlist[i].pt, &found);

         if ((p = get_defpad_ptr(pname)) > -1)
         {
            // I can not just update a PADSTACK on LAYER "PIN" and than flag it as an SMD !!!
            if (!def_pad[p].smd)
            {
               fprintf(ferr, "Padstack [%s] not flagged as SMD, but used in a SMD Component [%s]\n",
                  pname, b->getName());
               display_error++;
            }
            else
            {
               BlockStruct *b = Graph_Block_On(GBO_APPEND, pname,cur_filenum,0);
               doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
                  VT_NONE, NULL,SA_OVERWRITE, NULL); // x, y, rot, height
               Graph_Block_Off();
            }
         }
      }
   }

   sort_comppadstack_by_layer();

   int   pi;

   for (pi=0;pi<pinlistcnt;pi++)
   {
      if (!pinlist[pi].found) // pin has a good padstack
      {
         CString  newpadname = "$PSTK_";
         int      grcnt = 0;
         for (i=0;i<comppadstackcnt;i++)
         {
            PDIF_comppadstack *c = comppadstackarray[i];
            if (pi+1 != c->pinnr)   continue;   // if pinnr not equal skip it.

            // now build a padstack_name to make sure the padstacks use for the pin are the sam
            // in layer, aperture and offset
            CString  t;
            t.Format(",%d,%d,%0.3f,%0.3f", c->layer, c->bnum, c->offsetx, c->offsety);
            newpadname += t;
            grcnt++;
         }

         if (grcnt)
         {
            int found;
            int ptr = get_comppadnameptr(newpadname, &found);
            CString  pt;
            pt.Format("$STK_%d",ptr);

            if (!found)
            {
               // make one
               //fprintf(ferr,"Shape [%s] Pin [%d] assigned new PadStack [%s]\n", cur_libname, pi+1, pt);
               //display_error++;
               BlockStruct *b = Graph_Block_On(GBO_APPEND,pt,cur_filenum,0);
               b->setBlockType(BLOCKTYPE_PADSTACK);

               for (i=0;i<comppadstackcnt;i++)
               {
                  if (pi+1 != comppadstackarray[i]->pinnr)  continue;   // if pinnr not equal skip it.
                  BlockStruct *block = doc->Find_Block_by_Num(comppadstackarray[i]->bnum);
                  Graph_Block_Reference(block->getName(), NULL, 0, 
                     comppadstackarray[i]->offsetx, comppadstackarray[i]->offsety, 0.0, 0, 1.0,
                     comppadstackarray[i]->layer, TRUE);
               }
               Graph_Block_Off();
               if (G.cur_smd)
               {
                  doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
                     VT_NONE,NULL,SA_OVERWRITE, NULL); // x, y, rot, height
               }
            }
            pinlist[pi].datastruct->getInsert()->setBlockNumber(doc->Get_Block_Num(pt, cur_filenum, 0));
         }
      }
   }

   // here free comppadstackarray
   for (i=0;i<comppadstackcnt;i++)
   {
      delete comppadstackarray[i];
   }
   comppadstackcnt = 0;

   // here check for double pins

   for (i=0;i<pinlistcnt;i++)
   {
      free(pinlist[i].name);
      pinlist[i].name = NULL;
   }
   pinlistcnt = 0;
   G.cur_smd = FALSE;

   return 1;
}

/****************************************************************************/
/*
   Process a pins on a component.
*/
static int cpd_pin_def()
{
   return loop_command(pin_lst,SIZ_PIN_LST);
}

/****************************************************************************/
/*
   Process a pins on a component.
*/
static int cpd_pic_def()
{
   G.cur_width = 0;
   G.cur_widthindex = 0;   // make it small width.

   //SCLG_Shape(cur_libname);
   return loop_command(graph_lst,SIZ_GRAPH_LST);
}

/****************************************************************************/
/*
*/
static int  pin_already_exist(const char *n)
{
   int   i;

   for (i=0;i<pincnt-1;i++)   // do not check itself
   {
      if (!STRCMPI(shapename[shapenamecnt].pinname[i], n))
      {
         return i+1;
      }
   }

   return 0;
}

/****************************************************************************/
/*
   Process a pins on a component.
*/
static int pin_p()
{
   pincnt++;

   if (pincnt >= MAX_COM_PINS)
   {
      CString  err;
      err.Format("Too many Pins for Shape [%s] at %ld\n", shapename[shapenamecnt].name, ifp_line);
      ErrorMessage(err);
      return -1;
   }

   if (!get_tok())   return p_error();
   cur_pinname = token;

   if (USE_PIN_SEQ)
      cur_pinname.Format("%d", pincnt);


   while (pin_already_exist(cur_pinname))
   {
      cur_pinname.Format("%s_%d", token, pincnt);
   }

   if (!USE_PIN_SEQ)
   {
      if (cur_pinname.Compare(token))
      {
         fprintf(ferr, "Component [%s] Pin [%s] already defined -> changed to [%s] \n", 
            cur_libname, token, cur_pinname);
         display_error++;
      }
   }

   if ((shapename[shapenamecnt].pinname[pincnt-1] = STRDUP(cur_pinname)) == NULL)
   {
      MemErrorMessage(__FILE__, __LINE__);
   }

   /* Pin name not currently used.     */
   loop_command(ppn_lst,SIZ_PPN_LST);

   Graph_Block_On(GBO_APPEND,cur_libname,cur_filenum,0);
   // b->blocktype = BLOCKTYPE_PCBCOMPONENT;

   int found;

   DataStruct *d = Graph_Block_Reference(get_padname(pinlist[pinlistcnt].pt, &found), 
         cur_pinname, 0, pinlist[pinlistcnt].x, pinlist[pinlistcnt].y,
         0.0, 0, 1.0, Graph_Level("0", "", 1), TRUE);
   
   // pcad allows pt which are not defined in a padstack.
   BlockStruct *padstack = doc->Find_Block_by_Num(d->getInsert()->getBlockNumber());
   padstack->setBlockType(BLOCKTYPE_PADSTACK);

   if (pin_already_exist(cur_pinname))
   {
      fprintf(ferr, "Component [%s] Pin [%s] already defined -> converted to Mechanical Pin\n", 
         cur_libname, cur_pinname);
      display_error++;
      d->getInsert()->setInsertType(insertTypeMechanicalPin);
   }
   else
      d->getInsert()->setInsertType(insertTypePin);
   pinlist[pinlistcnt].found = found;
   pinlist[pinlistcnt].datastruct = d;

   // do pinnr here
   int   pinnr;
   pinnr = pinlistcnt+1;
   doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, TRUE),
      VT_INTEGER,&pinnr,SA_OVERWRITE, NULL); // x, y, rot, height
   Graph_Block_Off();

   pinlistcnt++;
   if (pinlistcnt >= MAX_COM_PINS)
   {
      fprintf(ferr,"Too many Pins/Comp\n");
      return -1;
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int ppn_lq()
{
   if (!get_tok())   return p_error();
   return 1;
}

/*****************************************************************************/
/*
*/
static int ppn_pt()
{
   int   p,pt;

   if (!get_tok())   return p_error();
   pt = atoi(token);

   pinlist[pinlistcnt].pt  = pt;
   pinlist[pinlistcnt].typ = 0;
   if (is_attr_layer(layer[G.cur_layer].layer_name,'C'))
   {
      p = update_pdef(pt,1);
      pinlist[pinlistcnt].typ = 1;
   }
   else
   if (is_attr_layer(layer[G.cur_layer].layer_name,'S'))
   {
      p = update_pdef(pt,2);
   }
   else
   {
      if ((pt >= 25 && pt <= 50) || pt >= 100) /* always smd */
      {
         p = update_pdef(pt,1);
         pinlist[pinlistcnt].typ = 1;
      }
      else
      {
         p = update_pdef(pt,3);
      }
   }

   if (pinlist[pinlistcnt].name != NULL)
   {
      fprintf(ferr,"Error Pinlist not cleared\n");
      return -1;
   }

   if ((pinlist[pinlistcnt].name = STRDUP(cur_pinname)) == NULL)
   {
      MemErrorMessage(__FILE__, __LINE__);
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int ppn_ploc()
{
   double   x,y;

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error();
   y = cnv_unit(token);
   pinlist[pinlistcnt].x = x;
   pinlist[pinlistcnt].y = y;

   return 1;
}

/****************************************************************************/
/*
*/
static int polyap()
{
   double   w;
   int      err;

   if (!get_tok())   return p_error();
   w = cnv_unit(token); 
   G.cur_polywidth = w;
   if (w == 0)
      G.cur_polywidthindex = 0;  // make it small width.
   else
      G.cur_polywidthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   return 1;
}

/****************************************************************************/
/*
*/
static void clean_atrval(char *v)
{
   unsigned int   i,ii = 0;
   char  v1[1000];

   for (i=0;i<strlen(v);i++)
   {
      if (v[i] == '\\')
      {
         i++;
      }
      v1[ii++] = v[i];
   }
   v1[ii] = '\0';

   // clean quotes if on both sides
   if (strlen(v1))
   {
      if (v1[0] == '"')
      {
         STRREV(v1);                // clean first quote.
         v1[strlen(v1)-1] = '\0';
         STRREV(v1);
         v1[strlen(v1)-1] = '\0';   // clean last quote.

      }
   }

   strcpy(v,v1);
}

/****************************************************************************/
/*
   External Attribute section in {I comp place
   {AT Keyword Value X Y}
*/
static int aex_at()
{
   CString  key;
   char     val[1000];
   char     tmp[1000];
   double   x, y, x1, y1;
   char     *lp;

   tmp[0] = '\0';

   while ((strlen(tmp) + strlen(cur_line)) < 1000)
   {
      strcat(tmp,cur_line);
      if (tmp[strlen(tmp)-1] == '\n')  tmp[strlen(tmp)-1] = '\0';
      if (tmp[strlen(tmp)-1] == '}')   break;
      if (!get_line(cur_line,MAX_LINE))
      {
         fprintf(ferr, "Error in aex_at\n");
         return -1;
      }
      strcat(tmp," ");
   }
   cur_new = TRUE;
   cur_line[0] = '\0';

   if (tmp[strlen(tmp)-1] == '}')
      tmp[strlen(tmp)-1] = '\0'; // take of }

   // get attribute keyword
   lp = strtok(tmp," \t\n");  // {At
   if ((lp = strtok(NULL," \t\n")) != NULL)
   {
      key = lp;
      key.TrimLeft();
      key.TrimRight();

      lp = strtok(NULL,"\n"); // get all
      strcpy(val,lp);
      STRREV(val);

      lp = strtok(val," \t\n");  // y koo
      strcpy(token,lp);
      STRREV(token);
      y = cnv_unit(token);

      lp = strtok(NULL," \t\n");  // y koo
      strcpy(token,lp);
      STRREV(token);
      x = cnv_unit(token);

      if ((lp = strtok(NULL,"\n")) != NULL)    // value may be NULL
      {
         strcpy(val,lp);
         STRREV(val);
         // 5 \% is 5%
         clean_atrval(val);
      }
      else
         val[0] = '\0';

      if (strlen(key) == 0)
      {
         fprintf(ferr, " Attr Keyword wrong at %ld\n",ifp_line);
      }
      else
      if (strlen(val) == 0)
      {
         fprintf(ferr, " Attr Value wrong at %ld\n",ifp_line);
      }
      else
      {
         CString  layername;
         layername = layer[G.cur_layer].layer_name;
         int layernr = Graph_Level(get_maplayer(layername), "", 0);
         CString  mapkey;
         mapkey = get_attrmap(key);

         if (mapkey.CompareNoCase(ATT_TOLERANCE) == 0)
         {
            double   ptol, mtol, tol;
            CString  t;

            check_tolerance(val,&mtol,&ptol,&tol);

            if (tol > -1)
            {
               double tt = tol;
               doc->SetAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_TOLERANCE, TRUE),
                  VT_DOUBLE, &tt, SA_OVERWRITE, NULL); // x, y, rot, height
            }
            if (mtol > -1)
            {
               double tt = -mtol;
               doc->SetAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE),
                  VT_DOUBLE, &tt, SA_OVERWRITE, NULL); // x, y, rot, height
            }           
            if (ptol > -1)
            {
               double tt = ptol;
               doc->SetAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE),
                  VT_DOUBLE, &tt, SA_OVERWRITE, NULL); // x, y, rot, height
            }
         }
         else
         {
            x1 = x; y1 = y;

            /* adjust mirror */
            if (G.cur_mirror) // this is the PS command
            {
               x = -x;
               Rotate(x,y,G.cur_rotation,&x1,&y1);
               // unmirror layer
               if (layer[G.cur_layer].mirror_index > -1)
                  layername = layer[layer[G.cur_layer].mirror_index].layer_name;
               else
               {
                  layername = layer[G.cur_layer].layer_name;
                  if (cur_layout_mode) // schematic has no mirror layers
                  {
                     // component mirrored, but no mirror layer found
                     fprintf(ferr, "Component [%s] mirrored, but no mirrored Layer for [%s] found.\n", 
                        cur_compname, layername);
                     display_error++;
                  }
               }
            }
            else
            {
               Rotate(x,y,360-G.cur_rotation,&x1,&y1);   
               layername = layer[G.cur_layer].layer_name;
            }

            x = y = 0;
            normalize_text(&x,&y,G.cur_textjust,
                  G.cur_textrot,
                  0,
                  G.cur_textsize,
                  (G.cur_textsize * strlen(val) * (6.0/8.0)));
            x1 += x;
            y1 += y;

            /* adjust attribute */
            // add refdes
            layernr = Graph_Level(get_maplayer(layername), "", 0);

            int never_mirror = FALSE;
            if (!cur_layout_mode)   never_mirror = TRUE;

            if (strlen(cur_compname))
            {
               doc->SetUnknownVisAttrib(&cur_compdata->getAttributesRef(),mapkey, val,  
                  x1, y1, DegToRad(G.cur_textrot), 
                  G.cur_textsize, G.cur_textsize*6.0/8.0,1,0,
                  TRUE, SA_OVERWRITE, 
                  0L,layernr,never_mirror, 0, 0);
            }
/*
            doc->SetUnknownAttrib(&cur_compdata->getAttributesRef(), mapkey,  val,
               x,y,0.0,
               G.cur_textsize,G.cur_textsize*6.0/8.0,1,0, FALSE , SA_APPEND, 0L,layernr,0); // x, y, rot, height, visible
*/
         }
      }
   } // keyword found

   // Do this to get back into the scanner
   push_tok();
   strcpy(token,"}");

   return 1;
}

/****************************************************************************/
/*
*/
static int  is_arrow(const char *n)
{
   int   i;

   for (i=0;i<arrownamecnt;i++)
   {
      if (arrownamearray[i]->compname.CompareNoCase(n) == 0)
         return i+1;
   }

   return 0;
}

/****************************************************************************/
/*
   normally ASG has a token
   {ASG A   
   [Ly "REFDES"]

   but here in this example it does not !
   {ASG
   [Ly "REFDES"]

*/
static int cmi_asg()
{
   if (!get_tok())   return p_error();                    
   
   if (tok_search(brk_lst,SIZ_BRK_LST) > -1)
      push_tok();

   loop_command(asg_lst,SIZ_ASG_LST);

   return 0;
}

/******************************************************************************
* cmp_i
   Process instance of a component.
*/
static int cmp_i()
{
   int   refnr = 0;

   current_rd.defined = FALSE;

   if (!get_tok())   return p_error();                    /* Library name.  */
   clean_libname(token);
   cur_libname = token;
   if (!get_tok())   return p_error();                    /* Component name.   */
   cur_compname = token;

   while (datastruct_from_refdes(doc, file->getBlock(), cur_compname))
   {
      cur_compname.Format("%s_%d", token, ++refnr);
   }

   if (refnr)
   {
      fprintf(ferr,"Duplicate reference designator [%s] renamed to [%s] at %ld.\n",
         token, cur_compname, ifp_line);
      display_error++;
   }


   // the component has to be placed here and the placement must be updated, because
   // attributes can appear before placement
   cur_compdata = Graph_Block_Reference(cur_libname, cur_compname , 0, 
      0.0 , 0.0,
      DegToRad(0.0), 0 , 1.0,Graph_Level("0","",1), TRUE);

   BlockStruct *block = doc->Find_Block_by_Num(cur_compdata->getInsert()->getBlockNumber());

   if (is_arrow(cur_compname) || cur_afn.CompareNoCase(cur_libname) == 0)
   {
      cur_compdata->getInsert()->setInsertType(insertTypeUnknown);  // unknown type
   }
   else
   {
      cur_compdata->getInsert()->setInsertType(default_inserttype(block->getBlockType()));
   }

   /* reset global values */
   G.cur_rotation = 0;
   G.cur_mirror = 0;
   loop_command(cmi_lst,SIZ_CMI_LST);

   if (current_rd.defined)
   {
      double   x1, y1, x, y;
      CString  layername;
      x1 = current_rd.x; y1 = current_rd.y;
      x  = current_rd.x; y  = current_rd.y;

      // adjust mirror 
      if (G.cur_mirror) // this is the PS command
      {
         x = -x;
         Rotate(x,y,G.cur_rotation,&x1,&y1);
         // unmirror layer
         if (layer[current_rd.layer].mirror_index > -1)
            layername = layer[layer[current_rd.layer].mirror_index].layer_name;
         else
         {  
            layername = layer[current_rd.layer].layer_name;
         }
      }
      else
      {
         Rotate(x,y,360-G.cur_rotation,&x1,&y1);   
         layername = layer[current_rd.layer].layer_name;
      }

      while (current_rd.textrot < 0)      current_rd.textrot += 360;
      while (current_rd.textrot >= 360)   current_rd.textrot -= 360;

      x = y = 0;

      normalize_text(&x,&y,current_rd.textjust,
                  current_rd.textrot,
                  0,
                  current_rd.textsize,
                  (current_rd.textsize * strlen(current_rd.name) * (6.0/8.0)));
      x1 += x;
      y1 += y;

      // curdata
      if (cur_compdata && strlen(current_rd.name))
      {
         // appearantly on schematic, the absolute rotation must be 0 or 90
         int   absrot;
         absrot = round(current_rd.textrot + G.cur_rotation);
         while (absrot < 0)      absrot += 360;
         while (absrot >= 360)   absrot -= 360;

         if (absrot > 90)
         {
            current_rd.textrot += absrot;

            if (absrot == 180)
            {
               x1 += (current_rd.textsize * strlen(current_rd.name) * (6.0/8.0));
               y1 += current_rd.textsize;
            }
            else
            if (absrot == 270)
            {
               y1 += (current_rd.textsize * strlen(current_rd.name) * (6.0/8.0));
               x1 += current_rd.textsize;
            }
         }

         layername = layer[current_rd.layer].layer_name;
         int layernr = Graph_Level(get_maplayer(layername), "", 0);

         doc->SetUnknownVisAttrib(&cur_compdata->getAttributesRef(),"PDIF_RD", current_rd.name,  
            x1, y1, DegToRad(current_rd.textrot), 
            current_rd.textsize, current_rd.textsize*6.0/8.0,1,0,
            TRUE, SA_OVERWRITE, 
            0L, layernr, TRUE, 0, 0); 
      }
   }


   // here assign a device. PCAD assignes VALUE and TOLERANCES on the Component level, which
   // this does not guaranty a unique device. 
   // in .ATTRIBMAP a device can be assigned, so I check there first.

   block = doc->Find_Block_by_Num(cur_compdata->getInsert()->getBlockNumber());  // this could have been changed with the IPT 
                                                               // copy of geoms.

   if (cur_compdata->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
   {
      Attrib *a =  is_attvalue(doc, cur_compdata->getAttributesRef(), ATT_TYPELISTLINK, 1);
      if (a)
      {
         CString typname;
         typname = get_attvalue_string(doc, a);
         TypeStruct *t = AddType(file,typname);
         t->setBlockNumber( block->getBlockNumber());
      }
      else
      {
         CString  typname;

         if (USE_PART_AS_DEVICE)
            typname = block->getName();
         else
            typname.Format("%s_%s", block->getName(), cur_compname );

         TypeStruct *t = AddType(file,typname);
         t->setBlockNumber( block->getBlockNumber());

         doc->SetUnknownAttrib(&cur_compdata->getAttributesRef(), ATT_TYPELISTLINK, typname, SA_OVERWRITE, NULL); //  
      }
   }

   return 1;
}

/****************************************************************************/
/*
   Load net connections for pins on component.
   Used later to write out pin definitions.

   This processing does not match PDIF 1.0 definition.
*/
static int cmi_cn()
{
   int      i,repeat = TRUE;
   CString  pinname;
   char     netname[80];
   int      shapeptr;
   char     **pinarray;
   int      pinarraycnt = 0;
   int      pinnames_included;
   
   /* here alloc pinarray */
   if ((pinarray = (char **)calloc(MAX_COM_PINS*2,sizeof(char *))) == NULL)
   {
      MemErrorMessage(__FILE__, __LINE__);
   }

   for (i = 0; repeat; ++i)
   {
      CString  tok;
      if (!get_tok())   return p_error();

      //   {Program "P-CAD 2000 PCB Version 15.10.17"}
      //   {CN 2 [C+] 1 IC1 } 
      //         ^
      //         |------ this error happens in this version

      if (tok_search(brk_lst,SIZ_BRK_LST) != BRK_B_SQUIGLY)
      {
         if (pinarraycnt < (MAX_COM_PINS*2))
         {
            if ((pinarray[pinarraycnt] = STRDUP(token)) == NULL)
            {
               MemErrorMessage(__FILE__, __LINE__);
            }
            pinarraycnt++;
         }
         else
         {
            fprintf(ferr,"Too many entries in pinarray\n");
            return -1;
         }
      }
      else
      {
         repeat = FALSE;
         push_tok();
      }
   }

   // returns 0, 1, -1
   pinnames_included = get_pincnt(cur_libname,pinarraycnt,&shapeptr);   

   if (pinnames_included < 0)
   {
      fprintf(ferr,"Syntax Error in CN section at %ld ! No Netlist assigned for Component [%s]\n",
         ifp_line, cur_compname);
      display_error++;
   }
   else
   {
      int   pinseq = 0, pcnt = 0;
      for (i=0;i<pinarraycnt;i++)
      {
         if (pinnames_included)
         {
            if (ACCEL_PDIF)
            {
               // PDIF files generated by accel are different. They go for the
               // pinname, not sequence.
               pinname = pinarray[i];
            }
            else
            {
               /* if pinname_included i = pinname,i+1 = netname */
               if (shapeptr > -1)
                  pinname = shapename[shapeptr].pinname[pcnt++];
               else     
                  pinname = pinarray[i];
            }
            i++;
            strcpy(netname,pinarray[i]);
         }
         else
         {
            pinname = shapename[shapeptr].pinname[i];
            strcpy(netname,pinarray[i]);
         }
         pinseq++;
         if (USE_PIN_SEQ)
            pinname.Format("%d", pinseq);

         // check if it was not an mechanical pin.

         // only make a netlist for electrical components.
         if (cur_compdata->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT ||
             cur_compdata->getInsert()->getInsertType() == INSERTTYPE_SYMBOL)
         {
            // make netlist
            CompPinStruct  *p;
            if (!strcmp(netname,"?"))   /* this in nonconnected pin */
            {
   
               NetStruct *n = add_net(file,NET_UNUSED_PINS);
               n->setFlagBits(NETFLAG_UNUSEDNET);
               if (test_add_comppin(cur_compname, pinname, file) == NULL)
               {
                  p = add_comppin(file, n, cur_compname, pinname);
               }
               else
               {
                  // double pin in UNUSED net is OK;
               }
            }
            else
            {
               NetStruct *n = add_net(file,netname);
               if (test_add_comppin(cur_compname, pinname, file) == NULL)
               {
                  p = add_comppin(file, n, cur_compname, pinname);
               }
               else
               {
                  fprintf(ferr, "Component [%s] Pin [%s] Net [%s] already defined -> can not add to netlist.\n", 
                     cur_compname, pinname, netname);
                  display_error++;
               }
            }
         }  // if pcbcomp
         else
         {
            CompPinStruct  *p;
            if (strcmp(netname,"?"))   /* if not an unconnected pin */
            {
               if (cur_layout_mode)
               {
                  fprintf(ferr,"Trying to add a Net to a NON-PCBCOMPONENT [%s] -> changed to PCBCOMPONENT\n",cur_compname);
                  display_error++;
                  cur_compdata->getInsert()->setInsertType(insertTypePcbComponent);
                  BlockStruct *block = doc->Find_Block_by_Num(cur_compdata->getInsert()->getBlockNumber());
                  block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
               }
               else
               {
                  fprintf(ferr,"Trying to add a Net to a NON-GATE [%s] -> changed to GATE\n",cur_compname);
                  display_error++;
                  cur_compdata->getInsert()->setInsertType(insertTypeSymbol);
                  BlockStruct *block = doc->Find_Block_by_Num(cur_compdata->getInsert()->getBlockNumber());
                  block->setBlockType(BLOCKTYPE_SYMBOL);
               }

               NetStruct *n = add_net(file,netname);
               if (test_add_comppin(cur_compname, pinname, file) == NULL)
                  p = add_comppin(file, n, cur_compname, pinname);
            }
         }
      }
   } // PIN_INCLUDED ERROR

   for (i=0;i<pinarraycnt;i++)
      free(pinarray[i]);
   free(pinarray);

   return 1;
}  // cmi_cn


/****************************************************************************/
/*
*/
static DataStruct *get_pindata_from_comppinnr(BlockStruct *b, int pinnr)
{
   CString  pinnumber;
   pinnumber.Format("%d",pinnr);

   DataStruct *np;
   POSITION pos = b->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = b->getDataList().GetNext(pos);
      
      if (np->getDataType() == T_INSERT)        
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {
            // pinname is in refname if a insert

            // this can not be compares against refname but ATT_COMPPINNR, because is 1 2 is changed to 2 1 
            // then it will b wrong.

            // these are absolute instructions.
            Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_COMPPINNR, 0);
            if (a)
            {
               if (a->getValueType() == VT_INTEGER)
               {
                  // int   pt = a->getIntValue();
                  if (pinnr == a->getIntValue())
                  {
                     return np;
                  }
               }
               else
               if (a->getValueType() == VT_STRING)
               {
                  CString l = a->getStringValue();
                  if (pinnumber.Compare(l) == 0)
                  {
                     return np;
                  }
               }
            }
         }
      }
   }
   return NULL;
}

/******************************************************************************
* cmi_ipt
*/
static int cmi_ipt()
{
   int      i,repeat = TRUE;
   char     pinname[20];
   int      ipt_code;
   int      shapeptr;
   char     **pinarray;
   int      pinarraycnt = 0;
   int      pinnames_included;
   int      do_ipt_code = DO_IPT_CODE;

   if (!DO_IPT_CODE)
   {
      fprintf(ferr,"IPT code for component [%s] not used (see .USE_IPT_CODE in PDIF.IN)\n",  
         cur_compdata->getInsert()->getRefname());
      display_error++;
   }

   /* here alloc pinarray */
   if ((pinarray = (char **)calloc(MAX_COM_PINS*2,sizeof(char *))) == NULL)
   {
      MemErrorMessage(__FILE__, __LINE__);
   }

   for (i = 0; repeat; ++i)
   {
      if (!get_tok())   return p_error();
      if (tok_search(brk_lst,SIZ_BRK_LST) != BRK_B_SQUIGLY)
      {
         if (pinarraycnt < (MAX_COM_PINS*2))
         {
            if ((pinarray[pinarraycnt] = STRDUP(token)) == NULL)
            {
               MemErrorMessage(__FILE__, __LINE__);
            }
            pinarraycnt++;
         }
         else
         {
            fprintf(ferr,"Too many entries in pinarray\n");
            return -1;
         }
      }
      else
      {
         repeat = FALSE;
         push_tok();
      }
   }

   pinnames_included = get_pincnt(cur_libname,pinarraycnt,&shapeptr);   

   // here check if padcode is defined - otherwise do not change geom
   if (do_ipt_code)
   {
      int   defined = TRUE;
      for (i=0;i<pinarraycnt;i++)
      {
         if (pinnames_included)
         {
            /* if pinname_included i = pinname,i+1 = netname */
            i++;
            ipt_code = atoi(pinarray[i]);
         }
         else
         {
            ipt_code = atoi(pinarray[i]);
         }

         if (check_pdef(ipt_code) < 0)
            defined = FALSE;
      }

      if (!defined)
      {
         fprintf(ferr,"Not all IPT codes are defined for %s - IPT not used\n", shapename[shapeptr].name);
         display_error++;
         do_ipt_code = FALSE;
      }
   }
         

   if (do_ipt_code)
   {
      // cur_compdata
      CString  newgeom;
      newgeom.Format("%s_%s", cur_libname, cur_compname);
      BlockStruct *block, *b1;

      if ((block = Graph_Block_Exists(doc, cur_libname, -1)) == NULL)
      {
         fprintf(ferr,"Copy error [%s]\n", cur_libname);
      }
      else
      {
         b1 = Graph_Block_On(GBO_APPEND,newgeom,cur_filenum,0);
         b1->setBlockType(block->getBlockType());
         // loop through every pinnr and update the pinname
         Graph_Block_Copy(block, 0, 0, 0, 0, 1, -1, TRUE);
         Graph_Block_Off();

         doc->SetUnknownAttrib(&b1->getAttributesRef(),ATT_DERIVED, cur_libname,SA_APPEND, NULL); // x, y, rot, height
         cur_compdata->getInsert()->setBlockNumber(b1->getBlockNumber());   // assign new shape
      }

      // returns number of pins
      int   pcnt = 0;
      for (i=0;i<pinarraycnt;i++)
      {
         if (pinnames_included)
         {
            /* if pinname_included i = pinname,i+1 = netname */
            if (shapeptr > -1)
               strcpy(pinname,shapename[shapeptr].pinname[pcnt++]);
            else     
               strcpy(pinname,pinarray[i]);
            i++;
            ipt_code = atoi(pinarray[i]);
         }
         else
         {
            strcpy(pinname,shapename[shapeptr].pinname[i]);
            ipt_code = atoi(pinarray[i]);
         }

         update_pdef(ipt_code,0);   // set to used.

         // here exchange pin - padshape.
         DataStruct *d1 = datastruct_from_refdes(doc, b1, pinname);
         // DataStruct *d = get_pindata_from_comppinnr(b1, i+1)
         CString  pname;
         int      found; 
         pname = get_padname(ipt_code, &found);
         BlockStruct *b2 = Graph_Block_On(GBO_APPEND, pname,cur_filenum,0);
         Graph_Block_Off();
         d1->getInsert()->setBlockNumber(b2->getBlockNumber());
      } // for all pins
   }  // if DO_IPT_CODE

   for (i=0;i<pinarraycnt;i++)
      free(pinarray[i]);
   free(pinarray);

   return 1;
}  // cmi_ipt

/****************************************************************************/
/*
   Load Scale, rotation and mirror atributes.
   This is before the placement records, so I have to remember it here.
*/
static int asg_rd()
{
   double   x,y;
   CString  refname, layername;

   if (!get_tok())   return p_error(); // name
   refname = token;
   if (!get_tok())   return p_error(); // x
   x = cnv_unit(token);
   if (!get_tok())   return p_error(); // y
   y = cnv_unit(token);

   current_rd.defined = TRUE;
   current_rd.name = refname;
   current_rd.x = x;
   current_rd.y = y;
   current_rd.layer = G.cur_layer;
   current_rd.textjust = G.cur_textjust;
   current_rd.textrot = G.cur_textrot;
   current_rd.textsize = G.cur_textsize;

   return 1;
}

/****************************************************************************/
/*
   Load Scale, rotation and mirror atributes.
   This is before the placement records, so I have to remember it here.
*/
static int asg_pn()
{
   double   x,y;
   CString  refname;

   if (!get_tok())   return p_error(); // name
   refname = token;
   if (!get_tok())   return p_error(); // x
   x = cnv_unit(token);
   if (!get_tok())   return p_error(); // y
   y = cnv_unit(token);

   return 1;
}

/****************************************************************************/
/*
   Load Scale, rotation and mirror atributes.
*/
static int cmi_atr()
{
   loop_command(atr_lst,SIZ_ATR_LST);

   if (G.cur_smd)
   {
      doc->SetAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
         VT_NONE,"",SA_OVERWRITE, NULL); // x, y, rot, height
   }
   return 1;
}

/****************************************************************************/
/*
   Scale, rotation and mirror atributes.
*/
static int atr_in()
{
   loop_command(ain_lst,SIZ_AIN_LST);

   /* here place component */
   cur_compdata->getInsert()->setOriginX(G.cur_pos_x); 
   cur_compdata->getInsert()->setOriginY(G.cur_pos_y); 
   cur_compdata->getInsert()->setAngle(DegToRad(G.cur_rotation)); 
   cur_compdata->getInsert()->setMirrorFlags((G.cur_mirror)?(MIRROR_FLIP | MIRROR_LAYERS):0);
   cur_compdata->getInsert()->setPlacedBottom(G.cur_mirror);

   return 1;
}

/****************************************************************************/
/* 
*/
static int atr_ex()
{
   return loop_command(aex_lst,SIZ_AEX_LST);
}

/****************************************************************************/
/* 
   Position.      
*/
static int ain_pl()
{
   if (!get_tok())   return p_error();
   G.cur_pos_x = cnv_unit(token);

   if (!get_tok())   return p_error();
   G.cur_pos_y = cnv_unit(token);
   return 1;
}

/****************************************************************************/
/* 
   Scale factor. Not used in Layout 
*/
static int ain_sc()
{
   if (!get_tok())   return p_error();
   if (!get_tok())   return p_error();
   return 1;
}

/****************************************************************************/
/* 
   Rotation.      
*/
static int ain_ro()
{
   if (!get_tok())   return p_error();
   G.cur_rotation = atoi(token) * 90;
   return 1;
}

/****************************************************************************/
/* 
   Rotation.      
*/
static int ain_pa()
{
   if (!get_tok())   return p_error();
   G.cur_rotation += atoi(token);
   return 1;
}

/****************************************************************************/
/* 
   Mirror.     
*/
static int ain_mr()
{
   if (!get_tok())   return p_error();

   if (toupper(*token) == 'Y')
      G.cur_mirror = TRUE;
   else
      G.cur_mirror = FALSE;
   return 1;
}

/****************************************************************************/
/* 
   Mirror.     
*/
static int ain_ps()
{
   if (!get_tok())   return p_error();

   if (toupper(*token) == 'B')
      G.cur_mirror = TRUE;
   else
      G.cur_mirror = FALSE;
   return 1;
}

/****************************************************************************/
/* 
   Comp Name placement.    
   Pcad component name is absolute to the component. 
   Database is relative to comp placment.
   Therefore the position has to be unrotated to its normal position.
*/
static int ain_nl()
{
   double   x,y,x1,y1;
   CString  layername;
   int      layernr;
   CString  t = cur_compname;

   if (!get_tok())   return p_error();
   x = cnv_unit(token);
   if (!get_tok())   return p_error();
   y = cnv_unit(token);

   x1 = x; y1 = y;

   /* adjust mirror */
   if (G.cur_mirror) // this is the PS command
   {
      x = -x;
      Rotate(x,y,G.cur_rotation,&x1,&y1);
      // unmirror layer
      if (layer[G.cur_layer].mirror_index > -1)
         layername = layer[layer[G.cur_layer].mirror_index].layer_name;
      else
      {
         layername = layer[G.cur_layer].layer_name;
         // component mirrored, but no mirror layer found
         fprintf(ferr, "Component [%s] mirrored, but no mirrored Layer for [%s] found.\n", 
            cur_compname, layername);
         display_error++;
      }
   }
   else
   {
      Rotate(x,y,360-G.cur_rotation,&x1,&y1);   
      layername = layer[G.cur_layer].layer_name;
   }

   x = y = 0;
   normalize_text(&x,&y,G.cur_textjust,
                  G.cur_textrot,
                  0,
                  G.cur_textsize,
                  (G.cur_textsize * strlen(cur_compname) * (6.0/8.0)));
   x1 += x;
   y1 += y;

   /* adjust PLacment name */
   // add refdes
   layernr = Graph_Level(get_maplayer(layername), "", 0);

   if (strlen(cur_compname))
   {
      doc->SetVisAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, TRUE),
         VT_STRING,
         cur_compname.GetBuffer(0),  // this makes a "real" char *
         x1, y1, DegToRad(G.cur_textrot), 
         G.cur_textsize, G.cur_textsize*6.0/8.0,1,0,
         TRUE, SA_OVERWRITE, 
         0L,layernr,0, 0, 0);
   }

   return 1;
}

/****************************************************************************/
/*
   {Iat FIXED 1 }
*/
static int ain_iat()
{
   CString  key;
   CString  val;
   char     *tmp,*lp;

   if ((tmp = (char *)malloc(1000+1)) == NULL)  // for HPUX char *
         MemErrorMessage(__FILE__, __LINE__);

   tmp[0] = '\0';
   while ((strlen(tmp) + strlen(cur_line)) < 1000)
   {
      strcat(tmp,cur_line);
      if (tmp[strlen(tmp)-1] == '\n')  tmp[strlen(tmp)-1] = '\0';
      if (tmp[strlen(tmp)-1] == '}')   break;
      if (!get_line(cur_line,MAX_LINE))
      {
         fprintf(ferr,"Error in iax\n");
         return -1;
      }
      strcat(tmp," ");
   }
   cur_new = TRUE;
   cur_line[0] = '\0';

   if (tmp[strlen(tmp)-1] == '}')
      tmp[strlen(tmp)-1] = '\0'; // take of }

   // get attribute keyword
   lp = strtok(tmp," \t\n");  // {Iat
   if ((lp = strtok(NULL," \t\n")) != NULL)
   {
      key = lp;
      lp = strtok(NULL,"\n"); // get all
      val = lp;

      key.TrimLeft();
      key.TrimRight();
      val.TrimLeft();
      val.TrimRight();

      if (cur_compdata)
      {
         CString  mapkey;
         mapkey = get_attrmap(key);

         if (mapkey.CompareNoCase(ATT_TOLERANCE) == 0)
         {
            double   ptol, mtol, tol;
            CString  t;

            check_tolerance(val,&mtol,&ptol,&tol);

            if (tol > -1)
            {
               double tt = tol;
               doc->SetAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_TOLERANCE, TRUE),
                  VT_DOUBLE, &tt, SA_OVERWRITE, NULL); // x, y, rot, height
            }
            if (mtol > -1)
            {
               double tt = -mtol;
               doc->SetAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_MINUSTOLERANCE, TRUE),
                  VT_DOUBLE,&tt, SA_OVERWRITE, NULL); // x, y, rot, height
            }           
            if (ptol > -1)
            {
               double tt = ptol;
               doc->SetAttrib(&cur_compdata->getAttributesRef(),doc->IsKeyWord(ATT_PLUSTOLERANCE, TRUE),
                  VT_DOUBLE, &tt, SA_OVERWRITE, NULL); // x, y, rot, height
            }
         }
         else
         {
            doc->SetUnknownAttrib(&cur_compdata->getAttributesRef(), mapkey,  val,SA_OVERWRITE, NULL); // x, y, rot, height
         }
      }
   } // keyword found
   free(tmp);

   // Do this to get back into the scanner
   push_tok();
   strcpy(token,"}");

   return 1;
}

/****************************************************************************/
/*
   Search for the token in the layer list.
*/
static int tok_layer()
{
   int      i;

   for (i = 0; i < G.max_layers; ++i)
   {
      if (!STRCMPI(layer[i].layer_name,token))
      {
         layer[i].used = 1;
         return(i);
      }
   }
   return(-1);
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
         if (!get_line(cur_line,MAX_LINE))
            return(FALSE);
         else
            cur_new = TRUE;

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
      case '%':
         return 0;
      case '{':
      case '}':
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
               if (*cp == '"' && token[i] != '\\')
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
         for (; !isspace(*cp) && *cp != '\"' &&
                           *cp != '{' && *cp != '}' &&
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
            return(FALSE);
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
   t  = tmp;
   tmp.Format("A Parsing Error indicates an corrupt PDIF file !\n");
   t += tmp;
   tmp.Format("Please use the PCAD PDIFIN software to verify the integrety\n");
   t +=tmp;
   tmp.Format("of your PDIF file.\n");
   t +=tmp;
   
   ErrorMessage(t, "Fatal PDIF Read Error", MB_OK | MB_ICONHAND);
   
   return -1;
}

/****************************************************************************/
/*
*/
void free_all_mem()
{
   int   i,ii;

   free(aperture);
   aperturecnt = 0;

   for (i=0;i<planenetcnt;i++)
   {
      free(planenet[i].net);
      free(planenet[i].lay);
   }
   free(planenet);

   for (i=0;i<G.max_dpads;i++)
   {
      if (def_pad[i].pstack_name)
         free(def_pad[i].pstack_name);
   }
   free(def_pad);

   for (i=0;i<shapenamecnt;i++)
   {
      for (ii=0;ii<shapename[i].pincnt;ii++)
         free(shapename[i].pinname[ii]);
      if (shapename[i].pincnt)
         free(shapename[i].pinname);
      free(shapename[i].name);
   }
   free(shapename);

   free(pinlist);
   pinlistcnt = 0;

   return;
}

/****************************************************************************/
/*
*/
static int check_pdef(int pdnum)
{
   int   i;

   for (i=0;i<G.max_dpads;i++)
   {
      if (def_pad[i].pdnum == pdnum)
      {
         return(i);
      }
   }
   return -1;
}

/******************************************************************************
* update_pdef
*/
static int update_pdef(int pdnum, int lay)
{
	int i=0;
   for (i=0; i<G.max_dpads; i++)
   {
      if (def_pad[i].pdnum == pdnum)
      {
         def_pad[i].used = TRUE;
         return i;
      }
   }

   if (G.max_dpads < MAX_PDEF)
   {
      def_pad[G.max_dpads].pstack_name = NULL;
      def_pad[G.max_dpads].pdnum = pdnum;
      def_pad[G.max_dpads].smd = FALSE;
      def_pad[G.max_dpads].drill = 0;
      def_pad[i].used = TRUE;

      G.max_dpads++;
   }
   else
   {
      fprintf(ferr, "Too many pad definitions [%d] at %ld\n", G.max_dpads, ifp_line);
      return -1;
   }

   return G.max_dpads-1;
}

/****************************************************************************/
/*
*/
static int get_defpad_ptr(const char *n)
{
   int   i;

   for (i=0;i<G.max_dpads;i++)
   {
      /* padstack name can be NULL */
      if (def_pad[i].pstack_name)
         if (!STRCMPI(n,def_pad[i].pstack_name))
            return(i);
   }

   /* this happens a lot for pads which are used as unconnected pads */
   /* printf("Can not find %s in PAD STACK LIST\n",n); */
   return(-1);
}

/****************************************************************************/
/*
*/
static int get_padnumber(const char *n)
{
   int   i;

   for (i=0;i<G.max_dpads;i++)
   {
      /* padstack name can be NULL */
      if (def_pad[i].pstack_name)
         if (!STRCMPI(n,def_pad[i].pstack_name))
            return(def_pad[i].pdnum);
   }

   /* this happens a lot for pads which are used as unconnected pads */
   /* printf("Can not find %s in PAD STACK LIST\n",n); */
   return(-1);
}

/****************************************************************************/
/*
*/
static int get_padsmd(const char *n,int smd)
{
   int   i;

   for (i=0;i<G.max_dpads;i++)
   {
      if (!STRLEN(def_pad[i].pstack_name))   continue;
      if (!STRCMPI(n,def_pad[i].pstack_name))
      {
         def_pad[i].smd = smd;
         def_pad[i].drill = 0.0; // Do it if generate drill is on
                                    // and name is funny p70x20
         return(def_pad[i].pdnum);
      }
   }

   /* this happens a lot for pads which are used as unconnected pads */
   /* printf("Can not find %s in PAD STACK LIST\n",n); */
   return(-1);
}

/****************************************************************************/
/*
*/
const char  *get_padname(int n, int *found)
{
   int   i;

   *found = TRUE;
   // this is always via
   // vias can not have a PCAD name, because Net-section is before pad
   // definition, and there I do not know the padname yet.

/* Vias are renames to the correct padstack names after the reader.
   if (n == 0 || (n > 50 && n < 100))
   {
      if (padnumber)
         cur_padname.Format("%d",n);
      else
         cur_padname.Format("PAD%d",n);
      return cur_padname;
   }
*/

   for (i=0;i<G.max_dpads && !padnumber;i++)
   {
      if (def_pad[i].pdnum == n)
      {
         if (def_pad[i].pstack_name)
            return(def_pad[i].pstack_name);
      }
   }
   /* this happens a lot for pads which are used as unconnected pads */
   //fprintf(ferr,"Can not find PT %d in PAD STACK LIST\n",n); 
   //display_error++;
   *found = FALSE;

   if (padnumber)
      cur_padname.Format("%d",n);
   else
      cur_padname.Format("PAD%d",n);

   return(cur_padname);
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
void elim_ext(char *n)
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
   double r;

   switch (pdifunit)
   {
      case  U_DECIMM:
         r = (atof(s) * Units_Factor(UNIT_MM, pageunits) / 100);
      break;
      case  U_CMM:
         r = (atof(s) * Units_Factor(UNIT_MM, pageunits));
      break;
      case  U_MIL:
         r = (atof(s) * Units_Factor(UNIT_MILS, pageunits));
      break;
      default:
         ErrorMessage("Unknown PDIF Unit", "Error");
         return 0.0;
      break;
   }

   return round_accuracy(r, accuracy);
}

/****************************************************************************/
/*
   remove .prt extension, but not other ones.
*/
void clean_libname(char *t)
{
   char  *p1;
   char  *p2;
   char  tmp[80];

   strcpy(tmp,t);
   p1 = strtok(tmp,".");
   if ((p2 = strtok(NULL,".")) == NULL)   // no extension
      return;

   if (!STRCMPI(p2,"PRT"))
   {
      strcpy(t,p1);
   }

   to_upper(t);

   return;
}

/****************************************************************************/
/*
*/
static int get_pincnt(const char *s,int cnt,int *shapeptr)
{
   int   i;
   *shapeptr = -1;
   for (i=0;i<shapenamecnt;i++)
   {
      if (!STRCMPI(shapename[i].name,s))
      {
         *shapeptr = i;
         if (shapename[i].pincnt == cnt)        return FALSE;
         if (shapename[i].pincnt == (0.5*cnt))  return TRUE;   // not cnt/2 because it makes 3/2 = 1
         fprintf(ferr,"Error in CN section Component [%s] Shape [%s]\n",
            cur_compname,s);
         return -1;
      }
   }
   fprintf(ferr,"Shape [%s] not found in shapelist\n",s);
   return -1;
}

/*****************************************************************************/
/*
*/
static int get_lay_att(const char *l)
{
   int   i;

   for (i=0;i<layer_attr_cnt;i++)
   {
      if (!STRCMPI(l,layer_attr[i].name))
      {
         return layer_attr[i].attr;
      }
   }
   return LAYTYPE_UNKNOWN;
}

/****************************************************************************/
/*
*/
static int pinnr_from_circle(const char *l,double x1,double y1,double radius)
{
   int   i;
   Point2 ll,tr;

   if ((i = get_lay_att(l)) == 0)   return 0;

   if (i != LAYTYPE_SIGNAL_TOP && i != LAYTYPE_PAD_TOP &&
       i != LAYTYPE_PASTE_TOP && i != LAYTYPE_MASK_TOP)
      return 0;


   ll.x = x1 - radius;
   tr.x = x1 + radius;
   ll.y = y1 - radius;
   tr.y = y1 + radius;
   
   for (i=0;i<pinlistcnt;i++)
   {
      if (pinlist[i].x > (ll.x-0.001) && pinlist[i].x < (tr.x+0.001) &&
          pinlist[i].y > (ll.y-0.001) && pinlist[i].y < (tr.y+0.001))
         return i+1;
   }
   return 0;
}

/****************************************************************************/
/*
   only evaluate certain layers.
*/
static int pinnr_from_poly(const char *l, double w)
{
   int      i;
   int      err = 0;
   int      found = 0;  // this makes sure that only 1 pin is found. 
                        // if it covers multiple pins, it does not belong to 1 pin

   if (pinlistcnt == 0) return 0;

   if ((i = get_lay_att(l)) == 0)   return 0;

   if (i != LAYTYPE_SIGNAL_TOP && i != LAYTYPE_PAD_TOP &&
       i != LAYTYPE_PASTE_TOP && i != LAYTYPE_MASK_TOP)
      return 0;

   Point2   checkpoint;
   Point2   *comppoly = (Point2 *)calloc(polycnt, sizeof(Point2));

   for (i=0;i<polycnt;i++)
   {
      Point2 p1;
      p1 = poly_l.ElementAt(i);  
      comppoly[i].x = p1.x;
      comppoly[i].y = p1.y;
   }     

   for (i=0;i<pinlistcnt;i++)
   {
      checkpoint.x = pinlist[i].x;
      checkpoint.y = pinlist[i].y;

      //*   2  = point p is on polyline vertex
      //*   1  = point p is on polyline segment (not vertex)
      //*   0  = point p is inside polyline
      //*   -1 = point p is outside of polyline
      //*   -2 = polyline is invalid

      int res = PntInPoly(&checkpoint, comppoly, polycnt );
      if (res >= 0)
      {
         err = i+1;
         found++;
      }
   }

   free(comppoly);

   if (found > 1)
      return 0;

   return err;
}

/****************************************************************************/
/*
*/
static int is_layer_used(char *l)
{
   int   i;
   for (i=0;i<G.max_layers;i++)
   {
      if (!STRCMPI(layer[i].layer_name,l))
         return i+1;
   }
   return 0;
}

/****************************************************************************/
/*
*/
static int is_attr_layer(const char *l,char c)
{
   int   i;

   for (i=0;i<layer_attr_cnt;i++)
   {
      if (!STRCMPI(l,layer_attr[i].name))
      {
         return (layer_attr[i].attr == c);
      }
   }
   return 0;
}

/****************************************************************************/
/*
*/
static int unknown_aperture_shape(const char *t)
{  
   int   i;
   // this function stores previous reported and only gives out the message once.

   for (i=0;i<unknownaperturecnt;i++)
   {
      if (unknownaperturearray[i]->name.Compare(t) == 0)
         return 0;   // <== do not report
   }

   PDIFUnknownaperture *a = new PDIFUnknownaperture;
   unknownaperturearray.SetAtGrow(unknownaperturecnt, a);
   unknownaperturecnt++;

   a->name = t;

   return 1;   // <== report
}

/****************************************************************************/
/*
*/
static int get_appshape(char *t)
{
   if (!STRCMPI(t,"ROUND"))      return T_ROUND;
   if (!STRCMPI(t,"SQUARE"))     return T_SQUARE;
   if (!STRCMPI(t,"RECTANGLE"))  return T_RECTANGLE;
   if (!STRCMPI(t,"THERMAL"))    return T_THERMAL;
   if (!STRCMPI(t,"TARGET"))     return T_TARGET;
   if (!STRCMPI(t,"POLYGON"))    return T_ROUND;
   if (!STRCMPI(t,"OVAL"))       return T_OBLONG;

   if (unknown_aperture_shape(t))   // this function stores previous reported and only gives out the message once.
      fprintf(ferr,"Unknown Aperture Shape [%s] -> converted to ROUND.\n",t);
   return T_UNDEFINED;
}

/****************************************************************************/
/*
*/
static int get_apptr(int apnr)
{
   int   i;

   for (i=0;i<aperturecnt;i++)
   {
      if (aperture[i].number == apnr)
         return i;
   }

   return -1;
}

/*****************************************************************************/
/*
*/
static GraphicClassTag get_layerclass(const char *layername)
{
   int   i;

   for (i=0;i<compoutlinecnt;i++)
   {
      if (!STRCMPI(layername,compoutline[i]))
         return graphicClassComponentOutline; 
   }

   if (!STRCMPI(layername,boardoutline))
      return graphicClassBoardOutline;

   return graphicClassNormal;
}

/*****************************************************************************/
/*
*/
static int assign_layers()
{
   int   i, number_of_electricallayers = 0;
   int   n, lt;
   LayerStruct *l;

   Graph_Level_Mirror("PINTOP", "PINBOT", "");  // always there
   Graph_Level_Mirror("PADTOP", "PADBOT", "");  // always there


   for (i = 0; i < G.max_layers; ++i)
   {
      if (layer[i].electrical)
         number_of_electricallayers++;

      // if one of the mirror layers is used, also do the other one.
      if (layer[i].mirror_index > -1)
      {
         if (layer[i].used)
         {
            layer[layer[i].mirror_index].used = TRUE;
         }
      }
   }

   for (i = 0; i < G.max_layers; ++i)
   {
      // if a layer is not used and not electrical
      if (!layer[i].electrical && !layer[i].used)     continue;

      // if any of the mirror index is used, do it.
      if (layer[i].mirror_index > -1)
      {
         CString  l1, l2;
         l1 = get_maplayer(layer[i].layer_name);
         l2 = get_maplayer(layer[layer[i].mirror_index].layer_name);
         Graph_Level(l1,"", 0);
         Graph_Level(l2,"", 0);
         Graph_Level_Mirror(l1, l2, "");
      }

      n =   Graph_Level(get_maplayer(layer[i].layer_name),"", 0);
      l = doc->FindLayer(n);

      lt = get_lay_att(layer[i].layer_name);
      if (lt > 0)
      {
         l->setLayerType(lt);
         l->setComment( layerTypeToString(lt));
      }
   }

   // here assign PHYS number 
   // l->setElectricalStackNumber(ii);
   int   inner = 0;
   for (i = 0; i < G.max_layers; ++i)
   {
      n =   Graph_Level(get_maplayer(layer[i].layer_name),"", 0);
      l = doc->FindLayer(n);
      lt = get_lay_att(layer[i].layer_name);
      if (l->getLayerType() == LAYTYPE_SIGNAL_TOP || l->getLayerType() == LAYTYPE_PAD_TOP)
      {
         l->setElectricalStackNumber(1);
         l->setComment("SIGNAL TOP");
      }
      else
      if (l->getLayerType() == LAYTYPE_SIGNAL_BOT || l->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         l->setElectricalStackNumber(number_of_electricallayers);
         l->setComment("SIGNAL BOTTOM");
      }
      else
      {
         if (layer[i].electrical)   
         {
            l->setElectricalStackNumber(++inner+1);
            l->setLayerType(LAYTYPE_SIGNAL_INNER);
            l->setComment("SIGNAL INNER");
         }
      }
      if (lt > 0)
         l->setComment( layerTypeToString(lt));
   }

   // PIN TOP
   if ((l = doc->FindLayer_by_Name("PINTOP")) != NULL)
   {
      lt = get_lay_att("PINTOP");
      if (lt > 0)
      {
         l->setLayerType(lt);
         l->setComment( layerTypeToString(lt));
      }
      else
      {
         l->setLayerType(LAYTYPE_PAD_TOP);
         l->setComment( layerTypeToString(LAYTYPE_PAD_TOP));
      }
   }
   if ((l = doc->FindLayer_by_Name("PINBOT")) != NULL)
   {
      lt = get_lay_att("PINBOT");
      if (lt > 0)
      {
         l->setLayerType(lt);
         l->setComment( layerTypeToString(lt));
      }
      else
      {
         l->setLayerType(LAYTYPE_PAD_BOTTOM);
         l->setComment( layerTypeToString(LAYTYPE_PAD_BOTTOM));
      }
   }

   if ((l = doc->FindLayer_by_Name("PADTOP")) != NULL)
   {
      lt = get_lay_att("PADTOP");
      if (lt > 0)
      {
         l->setLayerType(lt);
         l->setComment( layerTypeToString(lt));
      }
      else
      {
         l->setLayerType(LAYTYPE_PAD_TOP);
         l->setComment( layerTypeToString(LAYTYPE_PAD_TOP));
      }
   }
   if ((l = doc->FindLayer_by_Name("PADBOT")) != NULL)
   {
      lt = get_lay_att("PADBOT");
      if (lt > 0)
      {
         l->setLayerType(lt);
         l->setComment( layerTypeToString(lt));
      }
      else
      {
         l->setLayerType(LAYTYPE_PAD_BOTTOM);
         l->setComment( layerTypeToString(LAYTYPE_PAD_BOTTOM));
      }
   }
   if ((l = doc->FindLayer_by_Name("PIN")) != NULL)
   {
      lt = get_lay_att("PIN");
      if (lt > 0)
      {
         l->setLayerType(lt);
         l->setComment( layerTypeToString(lt));
      }
      else
      {
         l->setLayerType(LAYTYPE_PAD_ALL);
         l->setComment( layerTypeToString(LAYTYPE_PAD_ALL));
      }
   }
   if ((l = doc->FindLayer_by_Name("PAD")) != NULL)
   {
      lt = get_lay_att("PAD");
      if (lt > 0)
      {
         l->setLayerType(lt);
         l->setComment( layerTypeToString(lt));
      }
      else
      {
         l->setLayerType(LAYTYPE_PAD_ALL);
         l->setComment( layerTypeToString(LAYTYPE_PAD_ALL));
      }
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int get_physnr(char *l,int lphys)
{
   int   i;

   for (i=0;i<layer_attr_cnt;i++)
   {
      if (!STRCMPI(l,layer_attr[i].name))
         return (layer_attr[i].stacknumber);
   }
   fprintf(ferr," Layer [%s] has not been assigned a .LAYER_STACKUP number\n",l);
   return lphys;
}

/*Ende ***********************************************************************/

