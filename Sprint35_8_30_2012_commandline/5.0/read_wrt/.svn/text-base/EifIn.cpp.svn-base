// $Header: /CAMCAD/5.0/read_wrt/EifIn.cpp 36    6/17/07 8:56p Kurt Van Ness $
 
/****************************************************************************/
/* 
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.

   outline circle
   outline arc
   mirror pad layers pad_1 <> pad_6 etc...
   local cells
   netlist
   partnumber etc...
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
#include "lyrmanip.h"
#include "Eifin.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg  *progress;
//extern LayerTypeInfoStruct layertypes[];
extern FileStruct *currentPCBFile; // from PORT_LIB.CPP

/* Function Prototype ***********************************************************/

static int start_command(List *lst,int siz_lst,int start_indent);
static int go_command(List *lst,int siz_lst,int start_indent);
static int tok_search(char *lp, List *,int);
static char *strword();
static double get_units();
static double cnv_unit(double);
static char *get_keyword(char *buf,int *ident);
static void clean_name(char *n);       // make _ instead of blank.
static void clean_blank(char *n);      // eliminate blanks.
static void clean_level(char *n);      // eliminate of blanks.
static void check_buf(char *b);
static int write_poly();
static void global_init();
static void global_free();
static int read_techfile();
static int read_padfile();
static int read_netfile(), read_scinetfile();
static int write_arc();
static int write_circle();
static int write_text(const char *tmp);

static int overwrite_pad();
static int update_localpad(char *pn);
static int update_eiflevel(const char *l);
static int do_padoverwrite(int shape);

static int eif_null(int start_ident);
static int load_eifsettings(const CString fname);
static int is_file_netlist();
static int do_correspondencefile(const char *cor_file);
static int do_assign_layers();

/* Static Variable ***********************************************************/

static   CPartPinArray  partpinarray;
static   int         partpincnt;

static   CViaCellArray  viacellarray;
static   int         viacellcnt;

static   FILE     *ferr;                    /* scanner error file */
static   char     cur_line[MAX_LINE];        // this is the current line
static   char     cur_filename[255];     // this is the current open file
static   int      cur_ident;
static   long     fpcnt;
static   FILE     *cur_fp;                   // this the current file pointer
static   int      cur_status = FILE_UNKNOWN;
static   int      SMDPADMIRROR = FALSE;

static   Eiflayer *eiflayer;
static   int      eiflayercnt;

static   int         polycnt =0 ;
static   EIFPoly     *poly_l;

static   CDrillArray drillarray;
static   int         drillcnt;

static   int         NumberOfLayers   = 0;
static   int         cur_cellinstance = 0;
static   EIFGlobal   G;
static   int         cur_compcnt = 0;  // needed for unnamed components

static   int         usecorfile = FALSE;

static   Adef        layer_attr[MAX_LAYERS];       /* Array of default layers.   */
static   int         layer_attr_cnt;

static   EIFPoly     last_pinkoo;
static   char        last_padstack[80];
static   int         last_padrotation;

static   int         padoverwrite[PST_MAX];
static   int         padoverwritecnt = 0;
static   int         padoverwritepin = TRUE; // the local pad definition will overwrite the
                                          // pin defintion in the cell.
static   double      MINFIDC;

static   int         mirrorcomponent_replace = FALSE;
static   CString     mirrorcomponent_ending = "";

static   int         curpad_cnt;
static   EIFCurPad   *curpad;

static   Localpad    *localpad;
static   int         localpadcnt;

static   int         cur_fill = FALSE;
static   int         OLDNETLIST = FALSE;

static   CCEtoODBDoc  *doc;
static   int         display_error = 0;

static   int         PageUnits;
static   double      unit_faktor;

static   BlockStruct *curblock;
static   DataStruct  *lastpininsert;

// here is how SMD pin recognition works.
// remember data pointer - if no intpad rename top and bottom to SMD_TOP and SMD_BOT
// and mark as SMD pad.
static   CString     toppadname, botpadname, intpadname;

static   CString     pad_file = "";
static   CString     padstack_file = "";
static   CString     cell_lib_file = "";
static   CString     clearance_file = "";
static   CString     technology_file = "";
static   CString     design_file = "";
static   CString     pdb_ascii_file = "";
static   CString     netlist_file = "";

static   CString     unnamednet[30];
static   int         unnamednetcnt;

static   CString     creator = "";
static   CString     compoutline;

static   int         testpadtop; // force a testpad in cel file to a top definition.
static   int         derived;    // used to copy component for pin_name in part_def in design file.
static   EIFVia      eifvia;
static   int         nonmirrorcomponents;
static   int         fix_smdlayer;

/******************************************************************************
* ReadEIF
*/
void ReadEIF(const char *fname, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits, int cur_filecnt, int tot_filecnt)
{
// file = NULL;
   doc = Doc;
   display_error = 0;
   PageUnits = pageunits;
   
   CString eifLogFile = GetLogfilePath(EIFERR);
   if (cur_filecnt > 0)
   {
      if ((ferr = fopen(eifLogFile, "a+t")) == NULL)
      {
         CString t;
         t.Format("Error open [%s] file", eifLogFile);
         ErrorMessage(t, "Error");
         return;
      }
   }
   else
   {
      if ((ferr = fopen(eifLogFile, "wt")) == NULL)
      {
         CString t;
         t.Format("Error open [%s] file", eifLogFile);
         ErrorMessage(t, "Error");
         return;
      }
      display_error = 0;
   }

   fprintf(ferr, "Filename: %s\n", fname);

   CTime t = CTime::GetCurrentTime();
   fprintf(ferr,"%s\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));

   global_init();


   if (currentPCBFile == NULL || currentPCBFile->getBlockType() != blockTypePcb || currentPCBFile->getCadSourceFormat() != Type_EIF_Layout)
   {
      char drive[_MAX_DRIVE];
      char dir[_MAX_DIR];
      char filename[_MAX_FNAME];
      char ext[_MAX_EXT];

      _splitpath( fname, drive, dir, filename, ext );

      currentPCBFile = Graph_File_Start(filename, Type_EIF_Layout);
      currentPCBFile->setBlockType(blockTypePcb);
      currentPCBFile->getBlock()->setBlockType(currentPCBFile->getBlockType());
   }
   else
   {
      doc->PrepareAddEntity(currentPCBFile);
   }

   // settings must be loaded after fileinit.
   CString settingsFile( getApp().getImportSettingsFilePath("eif.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nEIF Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_eifsettings(settingsFile);

   Graph_Level("0", "",1);

   int l = update_eiflevel(PAD_TOP);
   eiflayer[l].used = TRUE;
   l = update_eiflevel(PAD_BOT);
   eiflayer[l].used = TRUE;
   l = update_eiflevel(PAD_INT);
   eiflayer[l].used = TRUE;
   
   fpcnt = 0;                  
   G.push_tok = 0;
   NumberOfLayers = 0;
   nonmirrorcomponents = 0;

   strcpy(cur_filename, fname);
   if ((cur_fp = fopen(fname, "rt")) == NULL)
   {
      fprintf(ferr, "Can not open file [%s]\n", fname);
      display_error++;
      return;
   }

   // test here for netlist - does not follow EIF syntax.
   G.filetype = FILETYPE_UNKNOWN;
   if (is_file_netlist())
   {
      switch (G.filetype)
      {
         case FILETYPE_OLDNETLIST:
            //read_oldnetfile();
            break;
         case FILETYPE_NETLIST:
            read_netfile();
            break;
         case FILETYPE_SCINETLIST:
            read_scinetfile();
            break;
         default:
            fprintf(ferr, "Unknown Netlist in [%s]\n", cur_filename);
            display_error++;
            break;
      }
   }
   else
   {
      strcpy(G.filetypename, "");
      G.filetype = FILETYPE_UNKNOWN;
      start_command(file_lst,SIZ_FILE_LST, 0);

      switch (G.filetype)
      {
         case FILETYPE_CORRESPONDENCE:
            do_correspondencefile(fname);
            break;
         case FILETYPE_TECHNOLOGY:
            go_command(tech_lst, SIZ_TECH_LST, 0);
            break;
         case FILETYPE_PAD:
            go_command(pad_lst, SIZ_PAD_LST, 0);
            break;
         case FILETYPE_PADSTACK:
            go_command(stk_lst, SIZ_STK_LST, 0);
            break;
         case FILETYPE_CELL:
            go_command(cel_lst, SIZ_CEL_LST, 0);
            break;
         case FILETYPE_DESIGN:
            go_command(des_lst, SIZ_DES_LST, 0);
            break;
         default:
            fprintf(ferr, "Unknown .FILETYPE [%s] in [%s]\n",G.filetypename, cur_filename);
            display_error++;
            break;
      }
   }  // is_netlist

   if (cur_fp)
      fclose(cur_fp);

   // here assign other layers
   do_assign_layers();
   generate_PADSTACKACCESSFLAG(doc, 1);
   // doc->purgeUnusedBlocks(); 

   EliminateSinglePointPolys(doc);                

   t = CTime::GetCurrentTime();
   fprintf(ferr, "%s\n",t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));

   fprintf(ferr, "Logfile closed\n");
   fclose(ferr);
   global_free();

   if (display_error && cur_filecnt == (tot_filecnt-1))
      Logreader(eifLogFile);

   return;
}

/******************************************************************************
* is_attr_layer
*/
static int is_attr_layer(const char *l, int c)
{
   for (int i=0; i<layer_attr_cnt; i++)
   {
      if (!STRCMPI(l, layer_attr[i].name))
         return (layer_attr[i].attr == c);
   }

   return 0;
}

/******************************************************************************
* get_lay_att
*/
static int get_lay_att(const char *l)
{
   for (int i=0; i<layer_attr_cnt; i++)
   {
      if (!STRCMPI(l, layer_attr[i].name))
         return layer_attr[i].attr;
   }
   return LAYTYPE_UNKNOWN;
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
//
//   return   LAYTYPE_UNKNOWN; 
//}

/******************************************************************************
* load_eifsettings
*/
static int load_eifsettings(const CString fname)
{
   FILE *fp;
   char line[255];
   char *lp;

   layer_attr_cnt = 0;
   unnamednetcnt = 0;
   testpadtop = FALSE;
   padoverwritepin = TRUE; // the local pad definition will overwrite the
   creator = "";
   compoutline = "";
   mirrorcomponent_replace = FALSE;
   mirrorcomponent_ending = "";
   fix_smdlayer = FALSE;

   MINFIDC = 0.05 * Units_Factor(UNIT_INCHES, PageUnits);

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      MessageBox(NULL, tmp,"EIF Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         // here do it.
         if (!STRICMP(lp, ".UNNAMEDNET"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString c = lp;
            c.TrimLeft();
            c.TrimRight();

            if (unnamednetcnt < 30)
            {
               unnamednet[unnamednetcnt] = c;
               unnamednetcnt++;
            }
            else
            {
               fprintf(ferr, "Too many unnamed nets defined.\n");
               display_error++;
            }

         }
         else if (!STRICMP(lp, ".COMPOUTLINE"))
         {
               lp = get_string(NULL, " \t\n");
               if (lp == NULL)
                  continue; 
               compoutline = lp;
               compoutline.TrimLeft();
               compoutline.TrimRight();
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            lp = get_string(NULL, " \t\n");
			if (lp == NULL)
               continue; 
            
			CString eiflayername = lp;
            eiflayername.TrimLeft();
            eiflayername.TrimRight();

            lp = get_string(NULL, " \t\n");
			if (lp == NULL)
               continue; 
            
			CString cclayer = lp;
         cclayer.TrimLeft();
         cclayer.TrimRight();

         int laytype = stringToLayerTypeTag(cclayer);
            
			if (layer_attr_cnt < MAX_LAYERS)
            {
               layer_attr[layer_attr_cnt].name = eiflayername;
               layer_attr[layer_attr_cnt].attr = laytype;
               layer_attr_cnt++;
            }
            else
            {
               fprintf(ferr,"Too many layer attributes\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".TESTPAD"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            testpadtop = !STRCMPI(lp, "TOP");
         }
         else if (!STRICMP(lp, ".FIX_SMDLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               fix_smdlayer = TRUE;   
         }
         else if (!STRICMP(lp, ".PADOVERWRITEPIN"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'n' || lp[0] == 'N')
               padoverwritepin = FALSE;   // the local pad definition will overwrite the
         }
         else if (!STRICMP(lp, ".MINFIDC"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            double c = atof(lp);
            MINFIDC = c * Units_Factor(UNIT_INCHES, PageUnits);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            int u = GetUnitIndex(lp);
            if (u > -1)
               MINFIDC = c * Units_Factor(u, PageUnits);
         }
         else if (!STRICMP(lp, ".MIRRORCOMPONENT_REPLACE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               mirrorcomponent_replace = TRUE;  
         }
         else if (!STRICMP(lp, ".MIRRORCOMPONENT_ENDING"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            mirrorcomponent_ending = lp;  
         }
         else if (!STRICMP(lp, ".OLDNETLIST"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               OLDNETLIST = TRUE;  
         }
         else if (!STRICMP(lp, ".MIRRORLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

			CString eiflayer1 = lp;
            eiflayer1.TrimLeft();
            eiflayer1.TrimRight();

			int layer1 = update_eiflevel(eiflayer1);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

			CString eiflayer2 = lp;
            eiflayer2.TrimLeft();
            eiflayer2.TrimRight();
			
			int layer2 = update_eiflevel(eiflayer2);
            eiflayer[layer1].mirror = layer2;
            eiflayer[layer2].mirror = layer1;
         }
      }
   }

   fclose(fp);
   return 1;
}
                  
/******************************************************************************
* get_drillindex
*/
static int get_drillindex(double size)
{
   if (size == 0)
      return -1;

   EIFDrill drill;
   for (int i=0; i<drillcnt; i++)
   {
      drill = drillarray.ElementAt(i);
      if (drill.d == size)
         return i;         
   }

   CString name;
   name.Format("DRILL_%d", drillcnt);

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);
   drill.d = size;
   drillarray.SetAtGrow(drillcnt++, drill);  

   return drillcnt -1;
}
     
/******************************************************************************
* static int is_file_netlist
*/
static int is_file_netlist()
{
   char buf[MAX_LINE];
   char ttmp[MAX_LINE];
   char *lp;
   int netlist_found = 0;

   while (fgets(buf, MAX_LINE,cur_fp))
   {
      strcpy(ttmp, buf);
      check_buf(buf);

      if (!STRCMPI(buf, "PARTS LIST"))
      {
         G.filetype = FILETYPE_SCINETLIST;
         netlist_found = 1;
         break;
      }

      if ((lp = strtok(buf, WORD_DELIMETER)) == NULL)
         continue;
      if (lp[0] == '.')
         continue;
      if (lp[0] == ';')
         continue;

      if (lp[0] == '%')
      {
         if (!STRCMPI(lp, "%PART"))
         {
            G.filetype = FILETYPE_NETLIST;
            netlist_found = 1;
            break;
         }
         if (!STRCMPI(lp, "%NET"))
         {
            G.filetype = FILETYPE_NETLIST;
            netlist_found = 1;
            break;
         }
         continue;
      }
   }

   fseek(cur_fp, 0L, SEEK_SET);   // set back to start
   return netlist_found;
}

/******************************************************************************
* int do_correspondencefile
*/
static int do_correspondencefile(const char *cor_file)
{
   // get all file types;
   pad_file = "";
   padstack_file = "";
   cell_lib_file = "";
   clearance_file = "";
   technology_file = "";
   design_file = "";
   pdb_ascii_file = "";
   netlist_file = "";

   go_command(correspondence_lst, SIZ_CORRESPONDENCE_LST, 0); 

   fclose(cur_fp);

/* Technology file */
   fpcnt = 0;                  
   G.push_tok = 0;


   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char filename[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath( cor_file, drive, dir, filename, ext );
	CString path = (CString)drive + "\\" + dir;

   strcpy(cur_filename, technology_file);
   if (progress != NULL)
      progress->SetStatus(cur_filename);
   if ((cur_fp = fopen(path + technology_file, "rt")) == NULL)
   {
      CString  tmp;
      tmp.Format("Can not open .TECHNOLOGY [%s]", technology_file);
      ErrorMessage(tmp, "Error in Correspondence file");
      fprintf(ferr, "Can not open file [%s]\n", technology_file);
      display_error++;
   }
   else
   {
      start_command(file_lst, SIZ_FILE_LST, 0);
      go_command(tech_lst, SIZ_TECH_LST, 0);
      fclose(cur_fp);
   }

/* PAD file */
   fpcnt = 0;                  
   G.push_tok = 0;

   strcpy(cur_filename, pad_file);
   if (progress != NULL)
      progress->SetStatus(cur_filename);
   if ((cur_fp = fopen(path + pad_file, "rt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open .PAD [%s]", pad_file);
      ErrorMessage(tmp, "Error in Correspondence file");
      fprintf(ferr, "Can not open file [%s]\n", pad_file);
      display_error++;
      return 1;
   }
   else
   {
      start_command(file_lst, SIZ_FILE_LST, 0);
      go_command(pad_lst, SIZ_PAD_LST, 0);
      fclose(cur_fp);
   }

/* PADSTACK file */
   fpcnt = 0;                  
   G.push_tok = 0;

   strcpy(cur_filename,padstack_file);
   if (progress != NULL)
      progress->SetStatus(cur_filename);
   if ((cur_fp = fopen(path + padstack_file, "rt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open .PADSTACK [%s]", padstack_file);
      ErrorMessage(tmp, "Error in Correspondence file");
      fprintf(ferr, "Can not open file [%s]\n", padstack_file);
      display_error++;
      return 1;
   }
   else
   {
      start_command(file_lst, SIZ_FILE_LST, 0);
      go_command(stk_lst, SIZ_STK_LST, 0);
      fclose(cur_fp);
   }

/* cell_lib_file file */
   fpcnt = 0;                  
   G.push_tok = 0;

   strcpy(cur_filename, cell_lib_file);
   if (progress != NULL)
      progress->SetStatus(cur_filename);
   if ((cur_fp = fopen(path + cell_lib_file, "rt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open .CELL_LIB [%s]", cell_lib_file);
      ErrorMessage(tmp, "Error in Correspondence file");
      fprintf(ferr, "Can not open file [%s]\n", cell_lib_file);
      display_error++;
      return 1;
   }
   else
   {
      start_command(file_lst, SIZ_FILE_LST, 0);
      go_command(cel_lst, SIZ_CEL_LST, 0);
      fclose(cur_fp);
   }

/* design_file file */
   fpcnt = 0;                  
   G.push_tok = 0;

   strcpy(cur_filename, design_file);
   if (progress != NULL)
      progress->SetStatus(cur_filename);
   if ((cur_fp = fopen(path + design_file, "rt")) == NULL)
   {
      CString  tmp;
      tmp.Format("Can not open .DESIGN [%s]", design_file);
      ErrorMessage(tmp, "Error in Correspondence file");
      fprintf(ferr, "Can not open file [%s]\n", design_file);
      display_error++;
      return 1;
   }
   else
   {
      start_command(file_lst, SIZ_FILE_LST, 0);
      go_command(des_lst, SIZ_DES_LST,0);
      fclose(cur_fp);
   }

/* netlist file */
   fpcnt = 0;                  
   G.push_tok = 0;

   strcpy(cur_filename, netlist_file);
   if (progress != NULL)
      progress->SetStatus(cur_filename);
   if (!strlen(netlist_file))
   {
      CString tmp;
      tmp.Format("No Netlist file in [%s]\n", cor_file);
      ErrorMessage(tmp, "Error in Correspondence file");

      fprintf(ferr, "No Netlist file in [%s]\n", cor_file);
      display_error++;
   }
   else if ((cur_fp = fopen(path + netlist_file, "rt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open .NETLIST [%s]", netlist_file);
      ErrorMessage(tmp, "Error in Correspondence file");
      fprintf(ferr, "Can not open file [%s]\n", netlist_file);
      display_error++;
   }
   else
   {
      // test here for netlist - does not follow EIF syntax.
      G.filetype = FILETYPE_UNKNOWN;
      if (is_file_netlist())
      {
         switch (G.filetype)
         {
            case FILETYPE_OLDNETLIST:
               fprintf(ferr, "Unknown Netlist in [%s]\n", cur_filename);
               display_error++;
               break;
            case FILETYPE_NETLIST:
               read_netfile();
               break;
            case FILETYPE_SCINETLIST:
               read_scinetfile();
               break;
            default:
               fprintf(ferr, "Unknown Netlist in [%s]\n", cur_filename);
               display_error++;
               break;
         }
      }
   }

   // do not close the last file - it is done on the end of main.

   return 1;
}

/****************************************************************************/
/*
   make layers more understandable for EIF users.
   do mirror layers
*/
static int do_assign_layers()
{
   LayerStruct *layer;
   LayerStruct *mirrorLayer;
   for (int i=0; i<eiflayercnt; i++)
   {
      if (eiflayer[i].used == 0)
         continue;

      int layerIndex = Graph_Level(eiflayer[i].name, "", 0);
      layer = doc->FindLayer(layerIndex);
      int layerType = get_lay_att(eiflayer[i].name);

      if (layerType > 0)
         layer->setLayerType(layerType);

      if (eiflayer[i].physnr)
      {
         if (eiflayer[i].physnr == 1)
            layer->setLayerType(LAYTYPE_SIGNAL_TOP);
         else if (eiflayer[i].physnr == NumberOfLayers)
            layer->setLayerType(LAYTYPE_SIGNAL_BOT);
         else
            layer->setLayerType(LAYTYPE_SIGNAL_INNER);

         layer->setElectricalStackNumber(eiflayer[i].physnr);
      }

      if (eiflayer[i].mirror != i)
      {
         int mirrorLayerIndex = Graph_Level(eiflayer[eiflayer[i].mirror].name, "", 0);
		 mirrorLayer = doc->FindLayer(mirrorLayerIndex);
         int mirrorLayerType = get_lay_att(eiflayer[eiflayer[i].mirror].name);

         if (mirrorLayerType > 0)
            mirrorLayer->setLayerType(mirrorLayerType);

         Graph_Level_Mirror(eiflayer[i].name, eiflayer[eiflayer[i].mirror].name, "");
      }
   }

   if (NumberOfLayers > 0)
   {
      CString p1, p2;
      p1.Format("PadLayer1");
      p2.Format("PadLayer%d", NumberOfLayers);
      Graph_Level_Mirror(p1, p2, "");
   }

   if (NumberOfLayers > 0)
   {

      CString p1, p2;
      p1.Format("FiducialLayer1");
      p2.Format("FiducialLayer%d", NumberOfLayers);
      if (layer = doc->FindLayer_by_Name(p1))
      {
         Graph_Level_Mirror(p1, p2, "");
         layer->setLayerType(LAYTYPE_TOP);
         layer = doc->FindLayer_by_Name(p2);
         layer->setLayerType(LAYTYPE_BOTTOM);
      }
      else if (layer = doc->FindLayer_by_Name(p2))
      {
         Graph_Level_Mirror(p1, p2, "");
         layer->setLayerType(LAYTYPE_TOP);
         layer = doc->FindLayer_by_Name(p2);
         layer->setLayerType(LAYTYPE_BOTTOM);
      }
   }

   Graph_Level_Mirror(SMD_TOP, SMD_BOT, "");
   int layerIndex = Graph_Level(SMD_TOP, "", 0);
   if (layer = doc->FindLayer(layerIndex))
   {
      layer->setFlagBits(LY_NEVERMIRROR); 
      layer->setLayerType(LAYTYPE_PAD_TOP);
      layer->setElectricalStackNumber(1);
   }

   layerIndex = Graph_Level(SMD_BOT, "", 0);
   if (layer = doc->FindLayer(layerIndex))
   {
      layer->setFlagBits(LY_MIRRORONLY); 
      layer->setLayerType(LAYTYPE_PAD_BOTTOM);
      layer->setElectricalStackNumber(NumberOfLayers);
   }

   layerIndex = Graph_Level(PAD_INT, "", 0);
   if (layer = doc->FindLayer(layerIndex))
   {
      layer->setLayerType(LAYTYPE_PAD_INNER);
   }

   // mirror padstack layers.
   Graph_Level_Mirror(PAD_TOP, PAD_BOT, "");

   layerIndex = Graph_Level(PAD_TOP, "", 0);
   if (layer = doc->FindLayer(layerIndex))
   {
      layer->setLayerType(LAYTYPE_PAD_TOP);
      layer->setElectricalStackNumber(1);
   }

   layerIndex = Graph_Level(PAD_BOT, "", 0);
   if (layer = doc->FindLayer(layerIndex))
   {
      layer->setLayerType(LAYTYPE_PAD_BOTTOM);
      layer->setElectricalStackNumber(NumberOfLayers);
   }

   layerIndex = Graph_Level("DRILLHOLE", "", 0);
   if (layer = doc->FindLayer(layerIndex))
   {
      layer->setLayerType(LAYTYPE_DRILL);
   }

   if (nonmirrorcomponents && fix_smdlayer)
   {
      // this is the old version, where smd components bottom are defined non mirroed.
      // copy SMD_TOP to PAD_TOP
      LayerStruct *fromlayer  = doc->FindLayer_by_Name(SMD_TOP);
      LayerStruct *tolayer  = doc->FindLayer_by_Name(PAD_TOP);
      MoveLayer(doc, fromlayer, tolayer, TRUE);

      // copy SMD_BOT to PAD_BOT
      fromlayer  = doc->FindLayer_by_Name(SMD_BOT);
      tolayer  = doc->FindLayer_by_Name(PAD_BOT);
      MoveLayer(doc, fromlayer, tolayer, TRUE);
   }


   return 1;
}

/******************************************************************************
* global_init
*/
void global_init()
{
   if ((poly_l = (EIFPoly *)calloc(MAX_POLY, sizeof(EIFPoly))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   if ((localpad = (Localpad *)calloc(MAX_LOCALPAD,sizeof(Localpad))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   localpadcnt = 0;

   if ((eiflayer = (Eiflayer *)calloc(MAX_LAYERS,sizeof(Eiflayer))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   eiflayercnt = 0;

   if ((curpad = (EIFCurPad *)calloc(MAX_CURPAD,sizeof(EIFCurPad))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   curpad_cnt = 0;

   viacellarray.SetSize(100, 100);
   viacellcnt = 0;

   partpinarray.SetSize(100, 100);
   partpincnt = 0;

   G.push_tok = FALSE;
   lastpininsert = NULL;
}

/******************************************************************************
* global_free
*/
void global_free()
{
   partpinarray.RemoveAll(); // individual record free is done in cell_??? 

	int i=0;
   for (i=0; i<viacellcnt; i++)
      delete viacellarray[i];
   viacellarray.RemoveAll();
   viacellcnt = 0;

   free(poly_l);
   polycnt = 0;

   for (i=0; i<localpadcnt; i++)
      free(localpad[i].curpad);
   free(localpad);
   localpadcnt = 0;

   for (i=0; i<eiflayercnt; i++)
   {
      // here udpate padlayer
      if (!STRNICMP(eiflayer[i].name, "PADLAYER", strlen("PADLAYER")))
      {
         int layerIndex = Graph_Level(eiflayer[i].name, "", 0);
         LayerStruct *layer = doc->FindLayer(layerIndex);

         int lnr = atoi(&eiflayer[i].name[8]);  
         if (lnr == 1)
         {
            layer->setLayerType(LAYTYPE_PAD_TOP);
            layer->setElectricalStackNumber(1);
         }  
         else if (lnr > 1 && lnr == NumberOfLayers)
         {
            layer->setLayerType(LAYTYPE_PAD_BOTTOM);
            layer->setElectricalStackNumber(NumberOfLayers);
         }
         else if (lnr > 1 && lnr < NumberOfLayers)
         {
            layer->setLayerType(LAYTYPE_PAD_INNER);
            layer->setElectricalStackNumber(lnr);
         }
      }
      free(eiflayer[i].name);
   }
   free(eiflayer);
   eiflayercnt = 0;
   free(curpad);
}

/******************************************************************************
* read_netfile
*/
static int read_netfile()
{
   char buf[MAX_LINE];
   char ttmp[MAX_LINE];
   char *lp;
   int netlist = FALSE;
   char netname[80];
   char compname[80];
   char pinname[80];
   NetStruct *net = NULL;

   fpcnt = 0;
   G.push_tok = 0;

   while (fgets(buf, MAX_LINE, cur_fp))
   {
      strcpy(ttmp, buf);
      check_buf(buf);
      fpcnt++;

      if ((lp = strtok(buf, WORD_DELIMETER)) == NULL)
         continue;
      if (lp[0] == '.')
         continue;
      if (lp[0] == ';')
         continue;

      if (lp[0] == '%')
      {
         if (!STRCMPI(lp, "%PART"))
            netlist = FALSE;
         if (!STRCMPI(lp, "%NET"))
            netlist = TRUE;
         continue;
      }

      if (netlist)
      {
         if (lp[0] != '*')
         {
            if (lp[0] == '\\')
               lp = strtok(ttmp, "\\");
            else if (OLDNETLIST)
			   lp = strtok(ttmp, " \t");
			else
			{
               fprintf(ferr, " Use Tecfile .OLDNETLIST y for this type of netlist.\n");
               display_error++;
			}
            strcpy(netname, lp);
            clean_name(netname);
            net = add_net(currentPCBFile, netname);
         }
         // some netlist are \C1\-\1\
         // others are C1-1
         if (OLDNETLIST)
		 {
			while ((lp = strtok(NULL, "- \t\n")) != NULL)
			{
				strcpy(compname,lp);
				if ((lp = strtok(NULL, "- \t\n")) == NULL)
				{
					fprintf(ferr, "Error in Netlist file [%s] at %ld\n", cur_filename, fpcnt);
					display_error++;
					return -1;
				}
				strcpy(pinname, lp);
				add_comppin(currentPCBFile, net, compname, pinname);
			}
		 }
		 else
		 {
			while ((lp = strtok(NULL, "\\ \t\n")) != NULL)
			{
				strcpy(compname,lp);
				if ((lp = strtok(NULL, "\\ \t-")) == NULL)
				{
					fprintf(ferr, "Error in Netlist file [%s] at %ld\n", cur_filename, fpcnt);
					display_error++;
					return -1;
				}
				strcpy(pinname, lp);
				add_comppin(currentPCBFile, net, compname, pinname);
			}
		 }
      }
   }
   return 1;
}

/******************************************************************************
* read_scinetfile
*/
static int read_scinetfile()
{
   char buf[MAX_LINE];
   char ttmp[MAX_LINE];
   char *lp;
   int netlist = FALSE, cont =FALSE;
   char netname[80];
   char compname[80];
   char pinname[80];
   NetStruct *net = NULL;

   fpcnt = 0;
   G.push_tok = 0;

   while (fgets(buf, MAX_LINE, cur_fp))
   {
      strcpy(ttmp, buf);
      check_buf(buf);
      fpcnt++;

      if (!STRCMPI(buf, "EOS"))
         netlist = FALSE;
      else if (!STRCMPI(buf, "NET LIST"))
         netlist = 1;      // 1 is netlist found, 2 = nodename, 3 is comp pin
      else if (!STRNICMP(buf, "NODENAME", 8))
         netlist = 2;      // 1 is netlist found, 2 = nodename, 3 is comp pin

      switch (netlist)
      {
      case 1:
         // do nothing
         break;
      case 2:
         {
            if ((lp = strtok(buf, " \t\n")) == NULL)
            {
               fprintf(ferr, "Error in Netlist file [%s] at %ld\n", cur_filename, fpcnt);
               display_error++;
               return -1;
            }     

            if ((lp = strtok(NULL, " \t\n")) == NULL)
            {
               fprintf(ferr, "Error in Netlist file [%s] at %ld\n", cur_filename, fpcnt);
               display_error++;
               return -1;
            }     

            strcpy(netname, lp);
            clean_name(netname);
            net = add_net(currentPCBFile, netname);

            // if a contiue $ found, netlist = 3 otherwise 1
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               netlist = 0;
            else
            {
               if (lp[0] == '$')
                  netlist = 3;
            }
         }
         break;
      case 3:
         {        
            if ((lp = strtok(buf, " \t\n")) == NULL)
            {
               fprintf(ferr, "Error in Netlist file [%s] at %ld\n", cur_filename,fpcnt);
               display_error++;
               return -1;
            }     
            strcpy(compname, lp);

            if ((lp = strtok(NULL, " \t\n")) == NULL)
            {
               fprintf(ferr,"Error in Netlist file [%s] at %ld\n", cur_filename, fpcnt);
               display_error++;
               return -1;
            }     
            strcpy(pinname, lp);
            add_comppin(currentPCBFile, net, compname, pinname);

            // if a contiue $ found, netlist = 3 otherwise 1
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               netlist = 0;
            else
            {
               if (lp[0] == '$')
                  netlist = 3;
            }           
         }
         break;
      default:
         break;
      }
   }
   return 1;
}

/******************************************************************************
* is_text
  check if text does not only consist of space
*/
static int is_text(char *n)
{
   if (n == NULL) 
   {
      fprintf(ferr, "Empty text at %ld in %s\n", fpcnt, cur_filename);
      display_error++;
      return FALSE;
   }

   for (int i=0; i<(int)STRLEN(n); i++)
   {
      if (!isspace(n[i]))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* *strword
  first advanve to begin of "
  than get the word
*/
char *strword()
{
   char *lp = strtok(NULL, "\"");     // get the first "
   if (is_text(lp))
      return lp;  

   lp = strtok(NULL, "\"");     // get until next "
   return lp;
}

/******************************************************************************
* get_units
  calc conversion factor to get units to inch
*/
double get_units()
{
   char *lp = strtok(NULL, WORD_DELIMETER);

   if (!STRCMPI(lp, "IN"))           
      return Units_Factor(UNIT_INCHES, PageUnits);

   if (!STRCMPI(lp, "TH"))           
      return Units_Factor(UNIT_MILS, PageUnits);

   if (!STRCMPI(lp, "MM"))
      return Units_Factor(UNIT_MM, PageUnits);

   fprintf(ferr, "Unknown UNITS [%s]\n", lp);
   display_error++;
 
   return 1.0;
}

/******************************************************************************
*   cnv_unit
*/
static double cnv_unit(double x)
{
   return(x * unit_faktor);
}

/******************************************************************************
* *get_keyword
  EIF keyword is ...XXX
  ident is the number of ...
  the return is the pointer to the keyword.
*/
char *get_keyword(char *buf,int *ident)
{
   unsigned int i = 0;
   while (i < strlen(buf) && isspace(buf[i]))
      i++;

   *ident = 0;
   while (i < strlen(buf) && buf[i] == '.')
   {
      *ident = *ident + 1;
      i++;
   }

   char *lp = strtok(&(buf[i]), " \t");
   return lp;
}

/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(char *lp, List *tok_lst,int tok_size)
{
   for (int i = 0; i < tok_size; ++i)
   {
      // this must look for the complete token, because
      // PAD and PADSTACK is a difference
      if (!STRICMP(lp, tok_lst[i].token)) 
         return(i);
   }
   return(-1);
}

/******************************************************************************
* eif_filetype
*/
int eif_filetype()
{
   char  *lp;
   if ((lp = strtok(NULL, WORD_DELIMETER)) == NULL)
   {
      fprintf(ferr, "Big error\n");
      display_error++;
      return -1;
   }

   strcpy(G.filetypename, lp);

   if (!STRCMPI(lp, "TECHNOLOGY"))
      G.filetype = FILETYPE_TECHNOLOGY;
   else if (!STRCMPI(lp, "PADFILE"))
      G.filetype = FILETYPE_PAD;
   else if (!STRCMPI(lp, "PADSTACKFILE"))
      G.filetype = FILETYPE_PADSTACK;
   else if (!STRCMPI(lp, "LIBRARY"))
      G.filetype = FILETYPE_CELL;
   else if (!STRCMPI(lp ,"PCB"))
      G.filetype = FILETYPE_DESIGN;
   else if (!STRCMPI(lp, "CORRESPONDENCE"))
      G.filetype = FILETYPE_CORRESPONDENCE;
   else
      G.filetype = FILETYPE_UNKNOWN;

   return 1;
}

/******************************************************************************
* eif_version
*/
int eif_version()
{
   char  *lp;
   if ((lp = strword()) == NULL)
   {
      fprintf(ferr, "Big error\n");
      display_error++;
      return -1;
   }

   if (!STRCMPI(lp, "1.0"))
   {
   }
   else if (!STRCMPI(lp, "VB14.00"))
   {
   }
   else
   {
      fprintf(ferr, "Unsupported Version [%s] in [%s]\n", lp, cur_filename);
      display_error++;
      return -1;
   }

   return 1;
}

/******************************************************************************
* int corfile_pad
*/
int corfile_pad()
{
   char *lp = strtok(NULL, TEXT_DELIMETER);
   pad_file = lp;

   pad_file.TrimLeft();
   pad_file.TrimRight();

   if (!strlen(pad_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      pad_file = lp;
   }

   return 1;
}

/******************************************************************************
* int corfile_padstack
*/
static int corfile_padstack()
{
   char *lp = strtok(NULL, TEXT_DELIMETER);
   padstack_file = lp;

   padstack_file.TrimLeft();
   padstack_file.TrimRight();

   if (!strlen(padstack_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      padstack_file = lp;
   }

   return 1;
}

/******************************************************************************
* corfile_cell_lib
*/
static int corfile_cell_lib()
{
   char *lp = strtok(NULL, TEXT_DELIMETER);
   cell_lib_file = lp;

   cell_lib_file.TrimLeft();
   cell_lib_file.TrimRight();

   if (!strlen(cell_lib_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      cell_lib_file = lp;
   }

   return 1;
}

/******************************************************************************
* int corfile_clearance
*/
static int corfile_clearance()
{
   char *lp = strtok(NULL, TEXT_DELIMETER);
   clearance_file = lp;

   clearance_file.TrimLeft();
   clearance_file.TrimRight();

   if (!strlen(clearance_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      clearance_file = lp;
   }

   return 1;
}

/******************************************************************************
* int corfile_technology
*/
static int corfile_technology()
{
   char *lp = strtok(NULL, TEXT_DELIMETER);
   technology_file = lp;

   technology_file.TrimLeft();
   technology_file.TrimRight();

   if (!strlen(technology_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      technology_file = lp;
   }

   return 1;
}

/******************************************************************************
* int corfile_design
*/
static int corfile_design()
{
   char *lp = strtok(NULL, TEXT_DELIMETER);
   design_file = lp;

   design_file.TrimLeft();
   design_file.TrimRight();

   if (!strlen(design_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      design_file = lp;
   }

   return 1;
}

/******************************************************************************
* int corfile_pdb_ascii
*/
static int corfile_pdb_ascii()
{
   char *lp;

   lp = strtok(NULL, TEXT_DELIMETER);
   pdb_ascii_file = lp;

   pdb_ascii_file.TrimLeft();
   pdb_ascii_file.TrimRight();

   if (!strlen(pdb_ascii_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      pdb_ascii_file = lp;
   }

   return 1;
}

/******************************************************************************
* int corfile_netlist
*/
static int corfile_netlist()
{
   char *lp = strtok(NULL, TEXT_DELIMETER);
   netlist_file = lp;

   netlist_file.TrimLeft();
   netlist_file.TrimRight();

   if (!strlen(netlist_file))
   {
      lp = strtok(NULL, TEXT_DELIMETER);
      netlist_file = lp;
   }

   return 1;
}

/******************************************************************************
* tech_num_layers
*/
int tech_num_layers()
{
   char *lp = strtok(NULL, WORD_DELIMETER);
   NumberOfLayers = atoi(lp);

   return 1;
}

/******************************************************************************
* tech_default_trc_wdt
*/
int tech_default_trc_wdt()
{
   char *lp = strtok(NULL, WORD_DELIMETER);
   //L_TLineWidth(get_widthcode(cnv_unit(atof(lp))));
   return 1;
}

/******************************************************************************
* tech_default_via
*/
int tech_default_via()
{
   char *lp = strword();
   return 1;
}

/******************************************************************************
* eif_padstack
*/
int eif_padstack()
{
   char *lp = strword();
   strcpy(G.symbol, lp);
   clean_name(G.symbol);

   return 1;
}

/******************************************************************************
* eif_padrot
*/
int eif_padrot()
{
   char *lp = strtok(NULL, WORD_DELIMETER);
   G.rotation = atof(lp);

   if (G.rotation >= 360)
      G.rotation -= 360;
   if (G.rotation < 0)
      G.rotation += 360;

   return 1;
}

/******************************************************************************
* eif_pinname
*/
int eif_pinname()
{
   char *lp = strword();
   strcpy(G.pinname, lp);
   clean_name(G.pinname);

   return 1;
}

/******************************************************************************
* eif_xy
  The syntax is
      ...XY (x y)
            (x y)
*/
static int eif_xy()
{
   char *lp;
   char buf[MAX_LINE];
   int id = 0;
   int try_cnt = 0;

   while (TRUE)
   {
      if (try_cnt)
      {
         if ((fgets(buf, MAX_LINE, cur_fp)) == NULL)
            return -1;

         check_buf(buf);
         strcpy(cur_line, buf);
         fpcnt++;

         // check if this is a keyword. If yes, you are done.
         if ((lp = get_keyword(buf, &id)) != NULL && id > 0)
         {
            G.push_tok = TRUE;
            break;
         }
         else
         {
            strcpy(buf, cur_line);   // just reset this here.
            lp = strtok(buf, "(");
         }
      }

      // here do it also for XY () () ()
      while (TRUE)
      {
         if (polycnt < MAX_POLY)
         {
            if ((lp = strtok(NULL, " (,\t")) == NULL)
            {
               break;
            }
            double x = cnv_unit(atof(lp));

            if ((lp = strtok(NULL, " \t)")) == NULL)
            {
               long l = fpcnt;
               break;
            }
            double y = cnv_unit(atof(lp));

            poly_l[polycnt].x = x;
            poly_l[polycnt].y = y;
            poly_l[polycnt].bulge = 0.0;
            polycnt++;
         }
         else
         {
            fprintf(ferr, "Too many polylines\n");
            display_error++;
            return -1;
         }
      }
      try_cnt++;
   }

   return 1;
}

/******************************************************************************
* eif_lvl
*/
static int eif_lvl()
{
   if (cur_status == FILE_PLANE_OUTL)     return 1;   // use LYR and not LVL
   if (cur_status == FILE_PLANE_HOLE)     return 1;   // use LYR and not LVL
   if (cur_status == FILE_VIA_OBS)        return 1;   // use LYR and not LVL
   if (cur_status == FILE_ASSM_OUTL)      return 1;   // use LYR and not LVL
   if (cur_status == FILE_PACK_OUTL)      return 1;   // use LYR and not LVL
   if (cur_status == FILE_SILK_ARTWORK)   return 1;   // use LYR and not LVL
   if (cur_status == FILE_ROUTE_OBS)      return 1;   // use LYR and not LVL
   if (cur_status == FILE_BOARD_OUTL)     return 1;   // fixed layer name BOARD_OUTL
   if (cur_status == FILE_ROUTE_OUTL)     return 1;   // fixed layer name ROUTE_OUTL

   char *lp = strword();
   strcpy(G.level,lp);
   // clean_level(G.level);

   int l = update_eiflevel(G.level);
   eiflayer[l].used = TRUE;

   G.levelnr = Graph_Level(G.level, "", 0); 

   return 1;
}

/******************************************************************************
* eif_lyr
*/
static int eif_lyr()
{
   char *lp = strword();
   strcpy(G.level, lp);
   // clean_level(G.level);
   
   int l = update_eiflevel(G.level);
   eiflayer[l].used = TRUE;

   G.lyrnr = Graph_Level(G.level, "", 0); 

   return 1;
}

/******************************************************************************
* eif_net
*/
static int eif_net()
{
   char *lp = strword();
   strcpy(G.name, lp);
   // clean_level(G.name);

   return 1;
}

/******************************************************************************
* eif_skip
  Skip this one to the next level.
*/
static int eif_skip()
{
   if (eif_null(cur_ident) < 0)  
      return -1; // skip until next on same level.

   return 1;
}

/******************************************************************************
* eif_creator
*/
static int eif_creator()
{
   char *lp = strword();
   creator = lp;

   return 1;
}

/******************************************************************************
* eif_units
*/
static int eif_units()
{
   unit_faktor = get_units();
   return 1;
}

/******************************************************************************
* go_command
     return -1 for EOF
     return 1  for ident match
     return 0  for ???
*/
static int go_command(List *lst, int siz_lst, int start_ident)
{
   char buf[MAX_LINE];
   char *lp;
   int i;

   while (TRUE)
   {
      if (!G.push_tok)
      {
         if ((fgets(buf, MAX_LINE, cur_fp)) == NULL)
            return -1;

         check_buf(buf);
         strcpy(cur_line, buf);
         fpcnt++;
         if ((lp = get_keyword(buf, &cur_ident)) == NULL)
            continue;
      }
      else
      {
         // reload strtok
         strcpy(buf, cur_line);
         lp = get_keyword(buf, &cur_ident);
      }

      if (cur_ident <= start_ident)
      {
         G.push_tok = TRUE;
         return 1;
      }
      G.push_tok = FALSE;

      if ((i = tok_search(lp, lst, siz_lst)) >= 0)
      {
         (*lst[i].function)();
      }
      else
      {
         fprintf(ferr, "Unknown Command [%s] in [%s] at %ld -> skipped til", lp, cur_filename,fpcnt);
         display_error++;
         if (eif_null(cur_ident) < 0)
            return -1; // skip until next on same level.
         fprintf(ferr, " %ld\n", fpcnt);
      }
   }
   return 0;
}

/******************************************************************************
* start_command
     return -1 for EOF
     return 1  for ident match
     return 0  for ???
*/
static int start_command(List *lst,int siz_lst,int start_ident)
{
   char buf[MAX_LINE];
   char *lp;
   int i;

   while (TRUE)
   {
      if (!G.push_tok)
      {
         if ((fgets(buf, MAX_LINE, cur_fp)) == NULL)
            return -1;

         //printf("%s %8ld\n",cur_filename,fpcnt);

         check_buf(buf);
         strcpy(cur_line, buf);
		 char check = cur_line[0];
		 if (check == '!') continue;
         fpcnt++;
         if ((lp = get_keyword(buf,&cur_ident)) == NULL)
            continue;
      }
      else
      {
         // reload strtok
         strcpy(buf, cur_line);
         lp = get_keyword(buf, &cur_ident);
      }

      if (cur_ident <= start_ident)
      {
         G.push_tok = TRUE;
         return 1;
      }
      G.push_tok = FALSE;

      if ((i = tok_search(lp, lst, siz_lst)) >= 0)
      {
         (*lst[i].function)();
      }
      else
      {
         G.push_tok = TRUE;
         return 1;
      }
   }
   return 0;
}

/******************************************************************************
* eif_null
     return -1 for EOF
     return 1  for ident match
     return 0  for ???
*/
static int eif_null(int start_ident)
{
   char buf[MAX_LINE];
   char *lp;
   
   while (TRUE)
   {
      if (!G.push_tok)
      {
         if ((fgets(buf, MAX_LINE, cur_fp)) == NULL)
            return -1;
         fpcnt++;
         check_buf(buf);
         clean_blank(buf);
         // XY poly may not have a . in the beginning
         if (buf[0] != '.')
            continue;
         strcpy(cur_line, buf);
         if ((lp = get_keyword(buf, &cur_ident)) == NULL)
            continue;
      }
      else
      {
         // reload strtok
         strcpy(buf, cur_line);
         lp = get_keyword(buf, &cur_ident);
      }

      if (cur_ident <= start_ident)
      {
         G.push_tok = TRUE;
         return 1;
      }
      G.push_tok = FALSE;
   }

  return 0;
}

/******************************************************************************
* tech_layer_stackup
*/
static int tech_layer_stackup()
{
   int id = cur_ident;

   go_command(laystack_lst, SIZ_LAYSTACK_LST, id);

   return 1;
}

/******************************************************************************
* tech_via_package
*/
static int tech_via_package()
{
   int id = cur_ident;

   go_command(via_package_lst, SIZ_VIA_PACKAGE_LST, id);

   return 1;
}

/******************************************************************************
* laystack_layer_num
   here do NAME, ROUTING ... etc.
*/
static int laystack_layer_num()
{
   char *lp = strtok(NULL, WORD_DELIMETER);
   int physnr = atoi(lp);

   int id = cur_ident;
   go_command(lay_lst, SIZ_LAY_LST, id);

   // here you have the layername in G.name and the physnr

   int i = update_eiflevel(G.name);
   eiflayer[i].used = TRUE;
   eiflayer[i].physnr = physnr;

   // here make the corresponding PAD layers.
   CString tmp;
   tmp.Format("PadLayer%d", physnr);
   i = update_eiflevel(tmp);
   eiflayer[i].used = TRUE;
   eiflayer[i].physnr = physnr;

   return 1;
}

/******************************************************************************
* eif_name
*/
static int eif_name()
{
   char *lp = strword();
   strcpy(G.name, lp);
   clean_name(G.name);

   return 1;
}

/******************************************************************************
* eif_layer
*/
static int eif_layer()
{
   char *lp = strword();
   strcpy(G.name, lp);
   // clean_level(G.name);

   return 1;
}

/******************************************************************************
* clean_name
   name _ instead of blank
*/
static void clean_name(char *n)
{
   // eliminame leading and trailing blanks
   while (strlen(n) > 0 && isspace(n[strlen(n)-1]))
      n[strlen(n)-1] = '\0';
   STRREV(n);

   while (strlen(n) > 0 && isspace(n[strlen(n)-1]))
      n[strlen(n)-1] = '\0';
   STRREV(n);

   for (int i=0; i<(int)strlen(n); i++)
   {
      if (n[i] == ' ')
         n[i] = '_';
   }
   return;
}

/******************************************************************************
* clean_blank
*/
static void clean_blank(char *n)
{
   // eliminame leading and trailing blanks
   while (strlen(n) > 0 && isspace(n[strlen(n)-1]))
      n[strlen(n)-1] = '\0';
   STRREV(n);

   while (strlen(n) > 0 && isspace(n[strlen(n)-1]))
      n[strlen(n)-1] = '\0';
   STRREV(n);
   return;
}

/******************************************************************************
* clean_level
  elimitade blank: I have seen examples with
   "Trace Level 1" and "Trace Level  1"
*/
void clean_level(char *n)
{
   int t = 0;
   char tmp[80];

   for (int i= 0 ;i<(int)strlen(n); i++)
   {
      if (!isspace(n[i]))
         tmp[t++] = n[i];
   }
   tmp[t] = '\0';
   strcpy(n, tmp);
   return;
}

/******************************************************************************
* pad_pad
*/
static int pad_pad()
{
   char *lp = strword();
   strcpy(G.name, lp);
   clean_name(G.name);

   G.geomtyp = T_UNDEFINED;
   G.xdim = 0;
   G.ydim = 0;
   G.xoff = 0;
   G.yoff = 0;

   int id = cur_ident;
   go_command(geom_lst, SIZ_GEOM_LST, id);

   int err;

   // here make an aperture
   if (G.geomtyp == T_PLUS)
   {

#ifdef _DEBUG
      fprintf(ferr, "Here need to make a PLUS aperture\n");
#endif

      Graph_Aperture(G.name, T_ROUND, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0, BL_APERTURE, TRUE, &err);

   }
   else if (G.geomtyp == T_TRIANGLE)
   {

#ifdef _DEBUG
      fprintf(ferr, "Here need to make a TRIANGLE aperture\n");
#endif

      Graph_Aperture(G.name, T_ROUND, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0, BL_APERTURE, TRUE, &err);

   }
   else if (G.geomtyp == T_THERMAL)
   {
      if (G.ydim == 0)
         G.ydim = G.xdim /2;
      Graph_Aperture(G.name, G.geomtyp, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0, BL_APERTURE, TRUE, &err);
   }
   else
      Graph_Aperture(G.name, G.geomtyp, G.xdim, G.ydim, G.xoff, G.yoff, 0.0, 0, BL_APERTURE, TRUE, &err);
   
   return 1;
}

/******************************************************************************
* stk_padstack
*/
static int stk_padstack()
{
   char *lp = strword();
   strcpy(G.name, lp);
   clean_name(G.name);

   G.diameter = 0;
   G.notplated = FALSE;
   G.padstackshape = 0;
   
   BlockStruct *curblock = Graph_Block_On(GBO_APPEND, G.name, -1, 0);
   curblock->setBlockType(BLOCKTYPE_PADSTACK);

   toppadname = "";
   botpadname = "";
   intpadname = "";

   int id = cur_ident;
   go_command(padstack_lst,SIZ_PADSTACK_LST,id);

   // if this is an SMD pad, than only write the TOP_PAD layer.
   // SMD Pad is a PAD which has no drill hole.
   if (strlen(intpadname) == 0)  
      G.diameter = 0;

   // the problem on the CADNETIX version is that components on the bottom layer
   // are not mirrored, but defined from the bottom.
   // therefore I can not use the SMD_TOP/BOT, because this is dependend on the mirror.
 
   if (strlen(intpadname) == 0)
   {
      // here smd pad stuff
      if (strlen(toppadname))
      {
         int layerindex;

         // if there is no bottom name, it may be a pad definition for only a bottom cell definition
         if (strlen(botpadname))
            layerindex = Graph_Level(SMD_TOP, "", 0);
         else
            layerindex = Graph_Level(PAD_TOP, "", 0);

         Graph_Block_On(GBO_APPEND, toppadname, -1, 0);
         Graph_Block_Off();
         Graph_Block_Reference(toppadname, NULL, 0, 0, 0, 0, 0, 1, layerindex, TRUE);
      }

      if (strlen(botpadname))
      {
         int layerindex;

         // if there is no top name, it may be a pad definition for only a bottom cell definition
         if (strlen(toppadname))
            layerindex = Graph_Level(SMD_BOT, "", 0);
         else
            layerindex = Graph_Level(PAD_BOT, "", 0);

         Graph_Block_On(GBO_APPEND, botpadname, -1, 0);
         Graph_Block_Off();
         Graph_Block_Reference(botpadname, NULL, 0, 0, 0, 0, 0, 1, layerindex, TRUE);
      }

      // true smd if top and bot to it can be mirrored.
      if (strlen(toppadname) ^ strlen(botpadname))
         doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);
      else
         doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_BLINDSHAPE, 0), VT_NONE, NULL, SA_OVERWRITE, NULL);
   }
   else
   {
      if (strlen(toppadname))
      {
         int layerindex = Graph_Level(PAD_TOP, "", 0);
         Graph_Block_On(GBO_APPEND, toppadname, -1, 0);
         Graph_Block_Off();
         Graph_Block_Reference(toppadname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
      if (strlen(botpadname))
      {
         int layerindex = Graph_Level(PAD_BOT, "", 0);
         Graph_Block_On(GBO_APPEND,botpadname, -1, 0);
         Graph_Block_Off();
         Graph_Block_Reference(botpadname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
      if (strlen(intpadname))
      {
         int layerindex = Graph_Level(PAD_INT, "", 0);
         Graph_Block_On(GBO_APPEND,intpadname,-1,0);
         Graph_Block_Off();
         Graph_Block_Reference(intpadname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerindex, TRUE);
      }
   }

   if (G.diameter)
   {
      CString drillname;
      int layernum = Graph_Level("DRILLHOLE", "", 0);
      drillname.Format("DRILL_%d",get_drillindex(G.diameter));
      Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
   }

   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* cel_cell
   Cells are non electrical Parts.
*/
static int cel_cell()
{
   //here update last curpad struct
   overwrite_pad();

   lastpininsert = NULL;

   char *lp = strword();
   if (lp != NULL)
      strcpy(G.name, lp);
   else
      G.name[0] = '\0';

   clean_name(G.name);
   G.geomtyp = PART_CELL;
   
   curblock = Graph_Block_On(GBO_APPEND, G.name, -1, 0);
   go_command(celpart_lst, SIZ_CELPART_LST, cur_ident);
   Graph_Block_Off();

   lastpininsert = NULL;
   polycnt = 0;

   for (int i=0; i<partpincnt; i++)
      delete partpinarray[i];
   partpincnt = 0;

   return 1;
}

/******************************************************************************
* cel_part
*/
static int cel_part()
{
   overwrite_pad();

   char *lp = strtok(NULL, WORD_DELIMETER); // part type SMD or THRU or EDGECONNECTOR

   if (!STRCMPI(lp, "SMD"))
      G.geomtyp = PART_SMD;
   else if (!STRCMPI(lp, "THRU"))
      G.geomtyp = PART_PART;
   else if (!STRCMPI(lp, "VIA"))
      G.geomtyp = PART_VIA;
   else if (!STRCMPI(lp, "EDGE_CONNECTOR"))
      G.geomtyp = PART_EDGE_CONNECTOR;
   else if (!STRCMPI(lp, "DISCRETE"))
      G.geomtyp = PART_PART;
   else if (!STRCMPI(lp, "TESTPAD"))
      G.geomtyp = PART_TESTPAD;
   else
   {
      fprintf(ferr, "Unknown Parttype [%s]\n", lp);
      display_error++;
      G.geomtyp = PART_UNKNOWN;
   }

   G.pincnt = 0;

   curblock = Graph_Block_On(GBO_APPEND, "NONAME", -1, 0);
   curblock->setBlockType(BLOCKTYPE_PCBCOMPONENT);
   go_command(celpart_lst, SIZ_CELPART_LST, cur_ident);
   Graph_Block_Off();

   for (int i=0; i<partpincnt; i++)
      delete partpinarray[i];
   partpincnt = 0;

   return 1;
}

/******************************************************************************
* cel_mhshape
*/
static int cel_mhshape()
{
   overwrite_pad();

   G.geomtyp = PART_MECHANICAL;
   G.pincnt = 0;

   curblock = Graph_Block_On(GBO_APPEND, "NONAME", -1, 0);
   go_command(celpart_lst, SIZ_CELPART_LST, cur_ident);
   Graph_Block_Off();

   for (int i=0; i<partpincnt; i++)
      delete partpinarray[i];
   partpincnt = 0;

   return 1;
}

/******************************************************************************
* cel_via
*/
static int cel_via()
{
   overwrite_pad();

   char *lp = strtok(NULL, WORD_DELIMETER); // part type PC

   G.geomtyp = PART_VIA;
   G.pincnt = 0;
   eifvia.shapename = "";
   eifvia.padname = "";

   curblock = NULL;
   go_command(celvia_lst, SIZ_CELVIA_LST, cur_ident);

   char padname[80]; // this is the padstack name
   int ptr;
   if (curpad_cnt == 0)
      strcpy(padname, eifvia.padname);
   else
      ptr = update_localpad(padname);

   // do not copy if they are the same name.
   // also need to watch out that the shapename is not identical to a PAD name
   if (strcmp(padname, eifvia.shapename) && Graph_Block_Exists(doc, eifvia.shapename, -1) == NULL)
   {
      BlockStruct *b = Graph_Block_Exists(doc, padname, -1);
      curblock = Graph_Block_On(GBO_APPEND, eifvia.shapename, -1, 0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      // here copy local padname
      if (b)
         Graph_Block_Copy(b, 0, 0, 0, 0, 1, -1, FALSE);

      Graph_Block_Off();
   }
   else
   {
   /*   EIFViaCell *v = new EIFViaCell;
      viacellarray.SetAtGrow(viacellcnt++, v);
      v->cellname = eifvia.shapename;
      v->vianame  = padname;

      if (STRCMPI(eifvia.shapename, padname) == 1)
	  {
		  // here remember that the vianame is not the cellname, but the padstack name.
          if (Graph_Block_Exists(doc, eifvia.shapename, -1))
          {
             fprintf(ferr, "Via Cell name [%s] already defined as PAD, PADSTACK or SHAPE at %ld\n", padname, fpcnt);
             display_error++;
          }
	  }
      else if (!strcmp(padname, eifvia.shapename))
      {
         fprintf(ferr, "Via Cell name and Padstack [%s] are the same at %ld\n", padname, fpcnt);
         display_error++;
      } */
   }

   return 1;
}

/******************************************************************************
* des_part
*/
static int des_part()
{
   char *tok = strtok(NULL, WORD_DELIMETER); // part type SMD or THRU

   int insertType;
   if (!STRCMPI(tok, "UNUSED"))
      insertType = -1;
   else if (!STRCMPI(tok, "THRU"))
      insertType = INSERTTYPE_PCBCOMPONENT;
   else if (!STRCMPI(tok, "DISCRETE"))
      insertType = INSERTTYPE_PCBCOMPONENT;
   else if (!STRCMPI(tok, "SMD"))
      insertType = INSERTTYPE_PCBCOMPONENT;
   else if (!STRCMPI(tok, "TESTPAD"))
      insertType = INSERTTYPE_TESTPOINT;
   else
   {
      fprintf(ferr, "Unknown PART_INSTANCE [%s]\n", tok);
      display_error++;
      insertType = 0;
   }

   G.name[0] = '\0';    // refdes
   G.name_1[0] = '\0';  // part number
   G.mirror = 0; 
   G.sidebot = 0;
   G.rotation = 0;
   G.pincnt = 0;
   polycnt = 0;
   derived = FALSE;

   go_command(part_instance_lst, SIZ_PART_INSTANCE_LST, cur_ident);

   if (insertType == -1) 
   {
      fprintf(ferr, "UNUSED PART_INSTANCE skipped in [%s] at %ld\n", cur_filename, fpcnt);
      display_error++;
      return -1;
   }

   cur_compcnt++;

   // NULL_STRING = emtpy name
   if (!STRCMPI(G.name, "NULL_STRING")) 
      G.name[0] = '\0';

   // check for unnamed (unused) components
   if (STRLEN(G.name) == 0)
      sprintf(G.name, "#C_%d", cur_compcnt);

   // make sure 
   BlockStruct *geom = Graph_Block_On(GBO_APPEND, G.symbol, -1, 0);
   Graph_Block_Off();

   // here write part

   BOOL OddSMD = FALSE;

   if (G.sidebot && !G.mirror)
   {
      if (mirrorcomponent_replace && strlen(mirrorcomponent_ending))
      {
         // check if ending is correct
         CString symbolName = G.symbol;
         CString endOfSymbolName = symbolName.Right(strlen(mirrorcomponent_ending));
         if (endOfSymbolName.CompareNoCase(mirrorcomponent_ending) == 0)
         {
            // check if a non mirror definition is avail
            symbolName = symbolName.Left(strlen(symbolName) - strlen(endOfSymbolName));

            if (Graph_Block_Exists(doc, symbolName, -1))
            {
               strcpy(G.symbol, symbolName);
               G.mirror = TRUE;
            }
         }  
      }
   }
   else
   {
      if (G.mirror)
         G.rotation = 360 - G.rotation;
   }

   while (G.rotation < 0)
      G.rotation += 360;

   while (G.rotation >= 360)
      G.rotation -= 360;

   DataStruct *data = Graph_Block_Reference(G.symbol, G.name, 0, poly_l[0].x, poly_l[0].y, DegToRad(G.rotation), G.mirror, 1, Graph_Level("0", "", 1), TRUE);
   data->getInsert()->setInsertType(insertType);
   data->getInsert()->setPlacedBottom(G.sidebot);  // - Joanne When the insert is on top side and mirrored, Graph_Block_Reference function call will set the place_bottom to true 
                                          // and also set mirror to true.  The insert should be on top and the mirror needs to be set to true, so reset place_bottom.

   if (G.sidebot && !G.mirror)
   {
      fprintf(ferr, "Component [%s] placed on BOTTOM layer and not mirrored!\n", G.name);
      display_error++;
      OddSMD = TRUE;
      nonmirrorcomponents++;
   }

   if (insertType == INSERTTYPE_PCBCOMPONENT && strlen(G.name_1))
   {
      TypeStruct *type = AddType(currentPCBFile, G.name_1);
      type->setBlockNumber( geom->getBlockNumber());

      int keyword = doc->IsKeyWord(ATT_TYPELISTLINK, TRUE);
      doc->SetAttrib(&data->getAttributesRef(), keyword, VT_STRING, G.name_1, SA_OVERWRITE, NULL);
   }

   if (OddSMD)
   {
      RemoveAttrib(doc->IsKeyWord(ATT_SMDSHAPE, TRUE), &data->getAttributesRef());
      doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_BLINDSHAPE, "", SA_OVERWRITE, NULL);
   }

   polycnt = 0;

   return 1;
}

/******************************************************************************
* des_cell
*/
static int des_cell()
{
   G.name[0] = '\0'; // refdes
   G.name_1[0] = '\0';
   G.mirror = 0; // top
   G.sidebot = 0;
   G.rotation = 0;
   polycnt = 0;

   go_command(part_instance_lst, SIZ_PART_INSTANCE_LST, cur_ident);

   sprintf(G.name, "CELL_%d", ++cur_cellinstance);
   // here write cell - cell has no G.name

   // make sure 
   Graph_Block_On(GBO_APPEND, G.symbol, -1, 0);
   Graph_Block_Off();

   // here write part
   DataStruct *d = Graph_Block_Reference(G.symbol, G.name, 0, poly_l[0].x, poly_l[0].y,
         DegToRad(G.rotation), G.mirror, 1.0, Graph_Level("0","",1), TRUE);
   polycnt = 0;

   return 1;
}

/******************************************************************************
* char *get_viacellmap
*/
static const char *get_viacellmap(const char *v)
{
   for (int i=0;i<viacellcnt;i++)
   {
      if (viacellarray[i]->cellname.Compare(v) == 0)
         return viacellarray[i]->vianame.GetBuffer(0);
   }

   return v;
}

/******************************************************************************
* des_via
*/
static int des_via()
{
   polycnt = 0;
   strcpy(G.name, "");

   go_command(via_instance_lst, SIZ_VIA_INSTANCE_LST, cur_ident);
   
   // here write via
   if (polycnt)
   {
      const char *vianame = get_viacellmap(G.symbol);
      // make sure via exist
      Graph_Block_On(GBO_APPEND, vianame, -1, 0);
      Graph_Block_Off();

      DataStruct *data = Graph_Block_Reference(vianame, NULL, -1, poly_l[0].x, poly_l[0].y, 0.0, 0, 1.0, Graph_Level("0","",1), TRUE);
      data->getInsert()->setInsertType(insertTypeVia);
      if (strlen(G.name))
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, G.name, SA_APPEND, NULL); 
   }
   polycnt = 0;
   strcpy(G.name, "");

   return 1;
}

/****************************************************************************/
/*
*/
static int des_mh()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0;
   go_command(mh_instance_lst,SIZ_MH_INSTANCE_LST,id);

   CString  bname;
   bname.Format("MH_%lg",G.diameter);

   if (Graph_Block_Exists(doc, bname, -1) == NULL)
   {
      CString  drillname;
      int layernum = Graph_Level("DRILLHOLE","",0);
      drillname.Format("DRILL_%d",get_drillindex(G.diameter));
      Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
      BlockStruct *b = Graph_Block_On(GBO_APPEND, bname,-1,0);
      Graph_Block_Reference( drillname, NULL, 0, 0, 0, 0.0, 0 , 1.0, layernum, TRUE);
      Graph_Block_Off();
      b->setBlockType(BLOCKTYPE_TOOLING);
   }

   DataStruct *d = Graph_Block_Reference(bname, NULL, 0,  poly_l[0].x,poly_l[0].y, 0.0, 0 , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeDrillTool);

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int des_fiducial()
{
   int   id = cur_ident;
   
   G.rotation = 0;
   polycnt = 0;
   go_command(fiducial_instance_lst,SIZ_FIDUCIAL_INSTANCE_LST,id);

   int      mir = 0;
   CString  bname;
   bname.Format("FIDC_%lg",G.radius*2);

   // just make sure, it's there.
   if (Graph_Block_Exists(doc, bname, -1) == NULL)
   {
      BlockStruct *b;
      b = Graph_Block_On(GBO_APPEND,bname,-1,0);

      int err;
      CString  appname;
      appname.Format("FIDAPP_%lg",G.radius*2);
      G.levelnr = Graph_Level("FiducialLayer1","",0);
      Graph_Aperture(appname, T_TARGET, G.radius*2, G.radius, 0.0, 0.0, 0.0, 0, 
         BL_APERTURE, TRUE, &err);
      Graph_Block_Reference(appname, NULL, 0, 0, 0, 0.0, 0 , 1.0, G.levelnr, TRUE);
      Graph_Block_Off();
      b->setBlockType(BLOCKTYPE_FIDUCIAL);
   }

   CString  fidmirror;
   fidmirror.Format("FiducialLayer%d", NumberOfLayers);
   if (!STRCMPI(G.level, fidmirror))
      mir = TRUE;

   DataStruct *d = Graph_Block_Reference(bname, NULL, 0, poly_l[0].x,poly_l[0].y,
               DegToRad(G.rotation), mir , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeFiducial);

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int part_part_def()
{
   int      id = cur_ident;
   double   compx, compy, comprot;
   int      compmirror, compsidebot;
   CString  refname;

   // store comp info - could be overwritten in instance
   refname = G.name;
   compx = poly_l[0].x;
   compy = poly_l[0].y;
   comprot = G.rotation;
   compsidebot = G.sidebot;
   compmirror = G.mirror;
   polycnt = 0;

   go_command(part_def_lst,SIZ_PART_DEF_LST,id);

   // restore compinfo
   strcpy(G.name, refname);
   G.sidebot = compsidebot;
   G.rotation = comprot;
   G.mirror = compmirror;
   poly_l[0].x = compx; 
   poly_l[0].y = compy; 
   polycnt = 1;

   return 1;
}

/****************************************************************************/
/*
*/
static int part_cell_def()
{
   int   id = cur_ident;

//   fprintf(ferr,"CELL_DEF at %ld in %s not  supported", fpcnt,cur_filename);
   eif_null(id);
//   fprintf(ferr," -> skipped to %ld\n",fpcnt);
// display_error++;

   return 1;
}

/****************************************************************************/
/*
*/
static int part_via_def()
{
   int   id = cur_ident;


//   fprintf(ferr,"VIA_DEF at %ld in %s not  supported", fpcnt,cur_filename);
   eif_null(id);
//   fprintf(ferr," -> skipped to %ld\n",fpcnt);
// display_error++;

   return 1;
}


/****************************************************************************/
/*
*/
static int part_instance_shape_name()
{
   char  *lp;

   lp = strword();
   strcpy(G.symbol,lp);
   clean_name(G.symbol);

   return 1;
}

/****************************************************************************/
/*
*/
static int part_instance_ref_des()
{
   int   id = cur_ident;
   char  *lp;

   if ((lp = strword()) != NULL)
   {
      strcpy(G.name,lp);
      clean_name(G.name);
   }
   else
   {
      fprintf(ferr,"Unnamed REF_DES in [%s] at %ld\n",cur_filename,fpcnt);
      sprintf(G.name,"NONAME_%d",++G.noname_refdes);
   }

   //
   go_command(refdes_lst,SIZ_REFDES_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int part_instance_part_number()
{
   char  *lp;

   if ((lp = strword()) != NULL)
   {
      strcpy(G.name_1,lp);
      clean_name(G.name_1);
   }
   
   return 1;
}

/****************************************************************************/
/*
*/
static int eif_diameter()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.diameter = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_radius()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_circlediameter()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp)) / 2;

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_fidc()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.radius = cnv_unit(atof(lp)) / 2;

   if (G.radius * 2 < MINFIDC)
      G.radius = MINFIDC/2;

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_startangle()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER); // in degrees
   G.startangle = atof(lp);

   return 1;
}

/****************************************************************************/
/*
  ...SW_ANG -359.750
  ...ST_ANG 0.000
*/
static int eif_deltaangle()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER); // in degrees
   G.deltaangle = round(atof(lp));   // round angles to 1 degree accuracy
   
   return 1;
}

/****************************************************************************/
/*
*/
static int eif_height()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.height = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_top()
{
   char  *lp;
   char  tmp[80];
  
   lp = strword();
   strcpy(tmp,lp);
   clean_name(tmp);
   toppadname = tmp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_plated()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER); // Yes or No
   if (!STRCMPI(lp,"NO"))
      G.notplated = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_shape()
{
   char  *lp;
   
   lp = strtok(NULL,WORD_DELIMETER); // Yes or No

   if (!STRCMPI(lp,"X"))      // treat like crosshair -> smd
      G.padstackshape = PST_CROSS_HAIR;
   else
   if (!STRCMPI(lp,"PLUS")) // treat like crosshair -> smd
      G.padstackshape = PST_CROSS_HAIR;
   else
   if (!STRCMPI(lp,"CROSS_HAIR"))
      G.padstackshape = PST_CROSS_HAIR;
   else
   if (!STRCMPI(lp,"ROUND"))
      G.padstackshape = PST_ROUND;
   else
   if (!STRCMPI(lp,"ROUND_HOLLOW"))
      G.padstackshape = PST_ROUND;
   else
   if (!STRCMPI(lp,"ROUND_ANNULAR"))
      G.padstackshape = PST_ROUND;
   else
   if (!STRCMPI(lp,"SQUARE"))
      G.padstackshape = PST_SQUARE;
   else
   if (!STRCMPI(lp,"SQUARE_HOLLOW"))
      G.padstackshape = PST_SQUARE;
   else
   {
/* Not needed !
      fprintf(ferr,"Unknown Padshape [%s] in [%s] at %ld\n",lp,cur_filename,fpcnt);
      //display_error++;
*/
      G.padstackshape = PST_UNKNOWN;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_bot()
{
   char  *lp;
   char  tmp[80];

   lp = strword();
   strcpy(tmp,lp);
   clean_name(tmp);
   botpadname = tmp;

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_int()
{
   char  *lp;
   char  tmp[80];

   lp = strword();
   strcpy(tmp,lp);
   clean_name(tmp);
   intpadname = tmp;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_shapename()
{
   char  *lp;

   if ((lp = strword()) != NULL)
      strcpy(G.name,lp);
   else
      G.name[0] = '\0';

   clean_name(G.name);

   curblock->setName(G.name);

   if (G.geomtyp != PART_VIA)
   {
      if (G.geomtyp == PART_SMD)
      {
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int celvia_shapename()
{
   char  *lp;

   if ((lp = strword()) != NULL)
      strcpy(G.name,lp);
   else
      G.name[0] = '\0';

   clean_name(G.name);

   eifvia.shapename = G.name;

   return 1;
}

/****************************************************************************/
/*
*/
static int update_curpad(const char *name, int laynr, double x, double y, double rot)
{        
   // first check if layer already has a padstack
   int   i;

   for (i=0;i<curpad_cnt;i++)
   {
      if (curpad[i].layerptr == laynr)
      {
         curpad[i].rotation = rot;
         curpad[i].layerptr = laynr;   // padstack has lyr number, not level number
         strcpy(curpad[i].padgeomname, name);
         curpad[i].x = x;
         curpad[i].y = y;
         return i;
      }
   }

   if (curpad_cnt < MAX_CURPAD)
   {
      curpad[curpad_cnt].rotation = rot;
      curpad[curpad_cnt].layerptr = laynr;   // padstack has lyr number, not level number
      strcpy(curpad[curpad_cnt].padgeomname, name);
      curpad[curpad_cnt].x = x;
      curpad[curpad_cnt].y = y;
      curpad_cnt++;
   }
   else
   {
      fprintf(ferr,"Too many padstack overwrites at %ld\n",fpcnt);
      display_error++;
      return -1;
   }
   return curpad_cnt-1;
}

/****************************************************************************/
/*
   expand the pincode into the pad array
*/
static int init_curpad(const char *p)
{
   DataStruct  *np;

   if (NumberOfLayers == 0)
   {
      fprintf(ferr, "No technology file loaded - padlayers for [%s] can not be expanded correctly\n", p);
      return 0;
   }

   BlockStruct *b = doc->Find_Block_by_Name(p, -1);
   if (b == NULL) return 1;

   // loop throu file
   POSITION pos = b->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = b->getDataList().GetNext(pos);

      if (np->getDataType() == T_INSERT)
      {

         // get padshape name and layer
         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
         LayerStruct *l = doc->FindLayer(np->getLayerIndex());
      
         int fromlayer, tolayer;

         if (!STRCMPI(l->getName(),SMD_TOP))
         {
            fromlayer = 1;
            tolayer   = 1;
         }
         else
         if (!STRCMPI(l->getName(),SMD_BOT))
         {
            fromlayer = NumberOfLayers;
            tolayer   = NumberOfLayers;
         }
         else
         if (!STRCMPI(l->getName(),PAD_TOP))
         {
            fromlayer = 1;
            tolayer   = 1;
         }
         else
         if (!STRCMPI(l->getName(),PAD_BOT))
         {
            fromlayer = NumberOfLayers;
            tolayer   = NumberOfLayers;
         }
         else  
         if (!STRCMPI(l->getName(),PAD_INT))
         {
            fromlayer = 2;
            tolayer   = NumberOfLayers-1;
         }
         else              
         {
            update_curpad(block->getName(), np->getLayerIndex(), np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), 
                          RadToDeg(np->getInsert()->getAngle()));
            continue;
         }

         // here now make the layers correct
         for (int i=fromlayer;i<=tolayer;i++)
         {
            CString  tmp;
            tmp.Format("PadLayer%d",i);
            int   lyrnr = Graph_Level(tmp,"", 0);
            update_curpad(block->getName(), lyrnr, np->getInsert()->getOriginX(), np->getInsert()->getOriginY(), RadToDeg(np->getInsert()->getAngle()));
         }
      }
   }

   return 0;
}

/****************************************************************************/
/*
*/
static int pin_already_defined(const char *pinname, int pinnr)
{
   int   i;

   if (strlen(pinname) == 0)  return FALSE;

   for (i=0;i<partpincnt;i++)
   {
      if (partpinarray[i]->pinname.Compare(pinname) == 0)
      {
         return TRUE;
      }
   }

   EIFPartPin *p = new EIFPartPin;
   partpinarray.SetAtGrow(partpincnt, p);
   partpincnt++;
   p->pinname = pinname;
   p->pinnr = pinnr;

   return FALSE;
}

/****************************************************************************/
/*
*/
static int celpart_pin()
{
   int   pinnr = -1;
   int   id = cur_ident;
   char  *lp;

   //here update last curpad struct
   overwrite_pad();

   polycnt = 0;
   G.rotation = 0;
   G.pincnt++;
   lp = strtok(NULL,WORD_DELIMETER);
   strcpy(G.pinname,lp);  // pin_number -> not the pin_name
   pinnr = atoi(lp);

   go_command(celpin_lst,SIZ_CELPIN_LST,id);

   // here write pin
   if (G.geomtyp == PART_PART || G.geomtyp == PART_SMD ||
       G.geomtyp == PART_EDGE_CONNECTOR || G.geomtyp == PART_MECHANICAL ||
       G.geomtyp == PART_TESTPAD)
   {
      // here update G.symbol with pad rotation and change G.symbol name
      // only do it if the pin code was a shape cross hair.
      //strcpy(orig_padstack,G.symbol);
      //set_padlistused(G.symbol,G.rotation);

      // make sure pin exist
      Graph_Block_On(GBO_APPEND,G.symbol,-1,0);
      Graph_Block_Off();

      init_curpad(G.symbol);

      // eif allows 2 pins to have the same name.
      // give an error message and make the second one a mechanical pin without name.
      
      if (pin_already_defined(G.pinname, pinnr))
      {
         fprintf(ferr,"Component [%s] PinName [%s] Pinnr [%d] already defined\n", G.name, G.pinname, pinnr);
         display_error++;
         strcpy(G.pinname,"");
      }

      // here place pin
      DataStruct *d = Graph_Block_Reference(G.symbol, G.pinname, 0, 
         poly_l[0].x,poly_l[0].y,
         DegToRad(G.rotation), 0 , 1.0,Graph_Level("0","",1), TRUE);
      if (strlen(G.pinname))
         d->getInsert()->setInsertType(insertTypePin);
      else
         d->getInsert()->setInsertType(insertTypeMechanicalPin);

      lastpininsert = d;

      // do pinnr here
      if (strlen(G.pinname))
      {
         if (pinnr > 0)
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, TRUE),
               VT_INTEGER, &pinnr,SA_OVERWRITE, NULL); // x, y, rot, height
         }
      }
   }
   else
   if (G.geomtyp == PART_VIA)
   {

      Graph_Block_On(GBO_APPEND,G.symbol,-1,0);
      Graph_Block_Off();

      init_curpad(G.symbol);

      // here place pin
      DataStruct *d = Graph_Block_Reference(G.symbol, NULL, 0, 
         poly_l[0].x,poly_l[0].y,
         DegToRad(G.rotation), 0 , 1.0,Graph_Level("0","",1), TRUE);
      //d->getInsert().inserttype = INSERTTYPE_PIN;
      lastpininsert = d;
   }
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int update_pinname(BlockStruct *b, int pinnr, const char *pinname)
{
   int      found = 0;

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

            // these are absolute instructions.
            Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_COMPPINNR, 0);
            if (a)
            {
               if (a->getValueType() == VT_INTEGER)
               {
                  // int   pt = a->getIntValue();
                  if (pinnr == a->getIntValue())
                  {
                     np->getInsert()->setRefname(STRDUP(pinname));
                     found++;
                     break;
                  }
               }
               else
               if (a->getValueType() == VT_STRING)
               {
                  CString l = a->getStringValue();
                  if (pinnumber.Compare(l) == 0)
                  {
                     np->getInsert()->setRefname(STRDUP(pinname));
                     found++;
                     break;
                  }
               }
            }
         }
      }
   } 
   if (!found)
   {
      fprintf(ferr,"Update error : Geometry [%s] Pinnr [%d] Pinname [%s] at %ld in [%s]\n",
         b->getName(), pinnr, pinname, fpcnt, cur_filename);
      display_error++;
   }

   return found;
}

/****************************************************************************/
/*
*/
static int despart_pin()
{
   int   id = cur_ident;
   char  *lp;
   CString  pinnr;

   // do not initialize G.Rotation, this is used for component placement.
   polycnt = 0;
   G.pincnt++;
   lp = strtok(NULL,WORD_DELIMETER);
   strcpy(G.pinname,lp);  // pin_number -> not the pin_name
   pinnr = G.pinname;   // this is updated in PIN_NAME field


   // can not use celpin_lst, because this overwrites the xy koo and rotation
   go_command(despin_lst,SIZ_DESPIN_LST,id);

   if (pinnr.Compare(G.pinname) != 0)
   {
      BlockStruct *b1;
      BlockStruct *block = Graph_Block_On(GBO_APPEND,G.symbol,-1,0);
      Graph_Block_Off();

      // here the pinname is changed. Need to copy the component and start changing the pins.
      if (derived == FALSE)
      {
         // curdata is the current insert
         CString  newgeom;
         newgeom.Format("%s_%s", G.symbol, G.name);
         b1 = Graph_Block_On(GBO_APPEND,newgeom,-1,0);
         b1->setBlockType(BLOCKTYPE_PCBCOMPONENT);
         // loop through every pinnr and update the pinname
         Graph_Block_Copy(block, 0, 0, 0, 0, 1, -1, FALSE);
         Graph_Block_Off();

         doc->SetUnknownAttrib(&b1->getAttributesRef(),ATT_DERIVED, G.symbol, SA_OVERWRITE, NULL); //  
         strcpy(G.symbol, newgeom);
         derived = TRUE;
      }
      else
      {  
         b1 = block;
      }
      // ATT_COMPPINNR is the pin number == G.cur_id
      update_pinname(b1, atoi(pinnr), G.pinname);
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_pad()
{
   int   id = cur_ident;
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   G.lyrnr = -1;
   G.rotation = 0;
   polycnt = 0;
   go_command(celpad_lst,SIZ_CELPAD_LST,id);

   // G.name   = shapename
   // G.symbol = pad graphic
   // G.level  = layer
   // poly_l[0].x = Xkoo
   // poly_l[0].y = Ykoo
   if (polycnt && G.geomtyp != PART_VIA)
   {
      if (padoverwritepin || (last_pinkoo.x == poly_l[0].x && last_pinkoo.y == poly_l[0].y))
      {
         if (G.lyrnr < 0)
         {
            fprintf(ferr,"PAD Componnent entry widthout LYR at %ld\n",fpcnt);
            display_error++;
            return -1;
         }
         else
         {
            if (padoverwritepin)
            {
               // always make a testpad layer top. PadLayer 1
               if (G.geomtyp == PART_TESTPAD && testpadtop)
               {
                  if (!STRNICMP(G.level,"PadLayer",strlen("PadLayer")))
                  {
                     strcpy(G.level, "PadLayer1");
                     G.lyrnr = Graph_Level(G.level,"",0); 
                  }
               }
               update_curpad(G.symbol, G.lyrnr, poly_l[0].x, poly_l[0].y,  G.rotation);
            }
         }
      }
      else
      {
         //SCLG_Shape(G.name);
         // write_padgraphic(G.pincnt, G.level,G.symbol, poly_l[0].x, poly_l[0].y, G.rotation);
      }
   }
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int celvia_pin()
{
   int   id = cur_ident;
   char  *lp;

   //here update last curpad struct - not needed because via has only one pin.
   //overwrite_pad();

   polycnt = 0;
   G.rotation = 0;
   G.pincnt++;
   lp = strtok(NULL,WORD_DELIMETER);
   strcpy(G.pinname,lp);  // pin_number -> not the pin_name

   go_command(celpin_lst,SIZ_CELPIN_LST,id);

   eifvia.padname = G.symbol;
 
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int celvia_pad()
{
   int   id = cur_ident;
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   G.lyrnr = -1;
   G.rotation = 0;
   polycnt = 0;
   go_command(celpad_lst,SIZ_CELPAD_LST,id);

   if (padoverwritepin)
      update_curpad(G.symbol, G.lyrnr, poly_l[0].x, poly_l[0].y,  G.rotation);

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_mh()
{
   int   id = cur_ident;
   
   polycnt = 0;
   G.diameter = 0;
   go_command(mh_instance_lst,SIZ_MH_INSTANCE_LST,id);

   CString  bname;
   bname.Format("MH_%lg",G.diameter);

   if (Graph_Block_Exists(doc, bname, -1) == NULL)
   {
      CString  drillname;
      int layernum = Graph_Level("DRILLHOLE","",0);
      drillname.Format("DRILL_%d",get_drillindex(G.diameter));
      Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
      BlockStruct *b = Graph_Block_On(GBO_APPEND, bname,-1,0);
      Graph_Block_Reference( drillname, NULL, 0, 0, 0, 0.0, 0 , 1.0, layernum, TRUE);
      Graph_Block_Off();
      b->setBlockType(BLOCKTYPE_TOOLING);
   }

   DataStruct *d = Graph_Block_Reference(bname, NULL, 0, poly_l[0].x, poly_l[0].y, 0.0, 0 , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeDrillTool);

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_asm()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   
   cur_status = FILE_ASSM_OUTL;

   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_silk()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SILK_ARTWORK;

   //G.lyrnr = Graph_Level("SILK_ARTWORK","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_pack()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PACK_OUTL;

   //G.lyrnr = Graph_Level("SILK_ARTWORK","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
   Outlines and polies are not in sequence
*/
static int write_poly()
{
   int   widthindex;
   int   i, err;

   int   fill = 0;
   int   voidtype = 0;
   int   closed = 0;

   if (polycnt < 2)  
   {
      polycnt = 0;
      return 1;
   }

   if (G.diameter == 0)
        widthindex = 0; // make it small width.
   else
        widthindex = Graph_Aperture("", T_ROUND, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *curdata = Graph_PolyStruct(G.lyrnr, 0L, 0);

   // here need to do TRACE_OBS
   //                 VIA_OBS
   //                 PLAC_OBS

   if (polycnt > 2)  // can only happen on minimum 3 points.
      closed = (poly_l[0].x == poly_l[polycnt-1].x && poly_l[0].y == poly_l[polycnt-1].y);

   if (cur_status == FILE_POS_PLANE)
   {
      curdata->setGraphicClass(GR_CLASS_ETCH);
      
      if (strlen(G.name))  // G.name is netname
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING, G.name, SA_APPEND, NULL); // x, y, rot, height
   }
   else
   if (cur_status == FILE_SHAPE)
   {
      closed = 1;
      fill = cur_fill;
   }
   else
   if (cur_status == FILE_PLANE_OUTL)
   {
      closed = 1;
      fill = 1;
      curdata->setGraphicClass(GR_CLASS_ETCH);
      
      if (strlen(G.name))  // G.name is netname
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING, G.name, SA_APPEND, NULL); // x, y, rot, height
   }
   else
   if (cur_status == FILE_PLANE_HOLE)
   {
      closed = 1;
      fill = 1;
      voidtype = 1;
      curdata->setGraphicClass(GR_CLASS_ETCH);
      
      if (strlen(G.name))  // G.name is netname
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
            VT_STRING, G.name, SA_APPEND, NULL); // x, y, rot, height
   }
   else
   if (cur_status == FILE_ROUTE_OUTL)
      curdata->setGraphicClass(GR_CLASS_ROUTKEEPIN);
   else
   if (cur_status == FILE_BOARD_OUTL)
   {
      curdata->setGraphicClass(GR_CLASS_BOARDOUTLINE);
   }
   else
   if (cur_status == FILE_ASSM_OUTL)
   {
      curdata->setGraphicClass(GR_CLASS_COMPOUTLINE);
   }
   else
   if (cur_status == FILE_ROUTE_OBS)
      curdata->setGraphicClass(GR_CLASS_ROUTKEEPOUT);
   else
   if (cur_status == FILE_PLACE_OBS)
      curdata->setGraphicClass(GR_CLASS_PLACEKEEPOUT);
   else
   if (cur_status == FILE_VIA_OBS)
   {
      curdata->setGraphicClass(GR_CLASS_VIAKEEPOUT);
   }
   else
   if (cur_status == FILE_TRACE)
   {
      curdata->setGraphicClass(GR_CLASS_ETCH);
      if (strlen(G.name))  // G.name is netname
      {
         doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
            VT_STRING, G.name, SA_OVERWRITE, NULL); //  
      }
   }


   Graph_Poly(NULL,  widthindex, fill, voidtype, closed);   // fillflag , negative, closed
   for (i=0;i<polycnt;i++)
   {
      if (i)
      {
         if (fabs(poly_l[i-1].x - poly_l[i].x) < SMALLNUMBER &&  fabs(poly_l[i-1].y - poly_l[i].y) < SMALLNUMBER)
         {
            continue;
         }
      }
      Graph_Vertex(poly_l[i].x, poly_l[i].y, poly_l[i].bulge);
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int des_trace()
{
   int   id = cur_ident;
   char  *lp;
   int   oldstat = cur_status;

   lp = strtok(NULL," \t");

   strcpy(G.name,"");

   cur_status = FILE_TRACE;

   polycnt = 0;
   go_command(trace_instance_lst,SIZ_TRACE_INSTANCE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;
   strcpy(G.name,"");

   return 1;
}

/****************************************************************************/
/*
*/
static int des_line()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   
   polycnt = 0;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_line_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   
   polycnt = 0;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int graph_line_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   
   polycnt = 0;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_sym()
{
   int   id = cur_ident;
   eif_null(id);
   return 1;
}

/****************************************************************************/
/*
*/
static int des_shape()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   
   cur_status = FILE_SHAPE;

   polycnt = 0;
   cur_fill = 0;
   go_command(graphshape_lst,SIZ_GRAPHSHAPE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_arc()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   DbFlag flg = 0;

   cur_status = FILE_TRACE;

   polycnt = 0;
   G.diameter = 0;
   go_command(grapharc_lst,SIZ_GRAPHARC_LST,id);

   write_arc();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_circle()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   DbFlag flg = 0;

   cur_status = FILE_TRACE;

   polycnt = 0;
   G.diameter = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);
   write_circle();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_text()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   char  *lp;
   CString  prosa;

   lp = strword();

   if (lp == NULL)
      prosa = "";
   else
      prosa = lp;

   //cur_status = FILE_TRACE;

   G.mirror = 0;
   G.sidebot = 0;
   G.rotation = 0;
   polycnt = 0;

   go_command(graphtext_lst,SIZ_GRAPHTEXT_LST,id);

   write_text(prosa);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_text()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   char  *lp;
   CString  prosa;

   prosa = "";
   if ((lp = strword()) != NULL) // there can be empty text
      prosa = lp;
   G.mirror = 0;
   G.sidebot =0;
   G.rotation = 0;
   polycnt = 0;

   //SCLG_Shape(G.name);
   go_command(graphtext_lst,SIZ_GRAPHTEXT_LST,id);

   write_text(prosa);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_trace_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_TRACE_OBS;

   G.lyrnr = Graph_Level("TRACE_OBS","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_via_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_VIA_OBS;

   //G.lyrnr = Graph_Level("VIA_OBS","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_route_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_ROUTE_OBS;

   //G.lyrnr = Graph_Level("ROUTE_OBS","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_line_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   cur_status = FILE_LINE;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;
   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_shape()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   cur_fill = 0;
   go_command(graphshape_lst,SIZ_GRAPHSHAPE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_cmplx_shape()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   char  *lp;

   cur_status = FILE_CMPLX_SHAPE;

   lp = strtok(NULL," \t");   // HOLE ???

   polycnt = 0;
   cur_fill  = FALSE;
   go_command(outline_lst,SIZ_OUTLINE_LST,id);
   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_line()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   cur_status = FILE_LINE;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);
   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;
   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_trace()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   char  *lp;

   lp = strtok(NULL," \t");   // FANOUT ???

   polycnt = 0;
   go_command(trace_instance_lst,SIZ_TRACE_INSTANCE_LST,id);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_arc()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_LINE;

   polycnt = 0;
   G.diameter = 0;
   go_command(grapharc_lst,SIZ_GRAPHARC_LST,id);
   G.lyrnr = Graph_Level(G.level,"",0);

   write_arc();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int celpart_circle()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   polycnt = 0;
   G.diameter = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   G.lyrnr = Graph_Level(G.level,"",0);
   write_circle();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_route_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_ROUTE_OBS;

   //G.lyrnr = Graph_Level("ROUTE_OBS","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_trace_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_TRACE_OBS;

   G.lyrnr = Graph_Level("TRACE_OBS","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_via_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_VIA_OBS;

   G.lyrnr = Graph_Level("VIA_OBS","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_place_obs()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_PLACE_OBS;

   G.lyrnr = Graph_Level("PLACE_OBS","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_contour()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   //fprintf (ferr,"Contour not implemented in .DESIGN at %ld\n", fpcnt);

   cur_status = FILE_CONTOUR;

   go_command(contour_lst,SIZ_CONTOUR_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_route_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_ROUTE_OUTL;
   G.lyrnr = Graph_Level("ROUTE_OUTL","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_pos_plane()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_POS_PLANE;
   G.name[0] = '\0';

   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_conductive_artwork()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_CONDUCTIVE_ARTWORK;

   G.lyrnr = Graph_Level("CONDUCTIVE_ARTWORK","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);  

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_silk_artwork()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_SILK_ARTWORK;
   //G.lyrnr = Graph_Level("SILK_ARTWORK","",0);
   go_command(outline_lst,SIZ_OUTLINE_LST,id);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_board_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_BOARD_OUTL;

   G.lyrnr = Graph_Level("BOARD_OUTL","",0);
   LayerStruct *l = doc->FindLayer(G.lyrnr);
   l->setLayerType(LAYTYPE_BOARD_OUTLINE);

   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int des_plane_outl()
{
   int   id = cur_ident;
   int   oldstat = cur_status;
   char  *lp;

   cur_status = FILE_PLANE_OUTL;

   lp = strtok(NULL," \t");   // HOLE ???
   if (!STRCMPI(lp,"HOLE"))
      cur_status = FILE_PLANE_HOLE;

   // hole or solid
   //G.lyrnr = Graph_Level("PLANE_OUTL","",0);
   go_command(outl_lst,SIZ_OUTL_LST,id);

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_cmplx_line()
{
   int   id = cur_ident;
   int   oldstat = cur_status;

   cur_status = FILE_CMPLX_LINE;
   cur_fill  = FALSE;
   go_command(outline_lst,SIZ_OUTLINE_LST,id);
   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   cur_status = oldstat;

   return 1;
}

/****************************************************************************/
/*
  This get a reference name position for a shape
*/
static int eif_ref()
{
   int   id = cur_ident;

   G.mirror = FALSE;
   go_command(text_lst,SIZ_TEXT_LST,id);

   if (polycnt == 0) return 1;
/*
   if (cur_status == FILE_CELL)
   {
      if ((res = L_ShapeNamePos (G.name,"REFNAME",
                  inch_2_dbunitpoint2(poly_l[0].x,poly_l[0].y),
                  (unsigned char) 0,degree_2_dbunit(0.0),
                  inch_2_dbunit(0.01),inch_2_dbunit(G.height))) != 1)
      {
         fprintf(ferr,"Error %d in L_ShapeNamePos\n",res);
      }
   }
*/
   return 1;
}

/****************************************************************************/
/*
*/
static int eif_just()
{
   // not implemented
   return 1;
}

/****************************************************************************/
/*
*/
static int eif_font()
{
   // not implemented
   return 1;
}

/****************************************************************************/
/*
  This get a partnumber position for a shape
*/
static int eif_partno()
{
   int   id = cur_ident;
   
   G.mirror = FALSE;
   go_command(text_lst,SIZ_TEXT_LST,id);

   if (polycnt == 0) return 1;

   return 1;
}

/****************************************************************************/
/*
*/
static int outl_side()
{
   int   id = cur_ident;
   char  *lp;

   if ((lp = strtok(NULL,WORD_DELIMETER)) == NULL) // side top or bottom
      return 0;
   
   switch (cur_status)
   {
      case FILE_SILK_ARTWORK:
      {
         Graph_Level_Mirror("SILK_ARTWORK_TOP", "SILK_ARTWORK_BOTTOM" , "");

         CString  tmp;
         tmp.Format("SILK_ARTWORK_%s",lp);
         G.lyrnr = Graph_Level(tmp,"",0);
      }
      break;
      case FILE_PACK_OUTL:
      {
         Graph_Level_Mirror("PACK_OUTL_TOP", "PACK_OUTL_BOTTOM" , "");

         CString  tmp;
         tmp.Format("PACK_OUTL_%s",lp);
         G.lyrnr = Graph_Level(tmp,"",0);
      }
      break;
      case FILE_ASSM_OUTL:
      {
         Graph_Level_Mirror("ASSM_OUTL_TOP", "ASSM_OUTL_BOTTOM" , "");

         CString  tmp;
         tmp.Format("ASSM_OUTL_%s",lp);
         G.lyrnr = Graph_Level(tmp,"",0);
      }
      break;
      default:
         // what side for what status is this ?
         int y = 0;
      break;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int outl_outline()
{
   int   id = cur_ident;

   cur_fill  = FALSE;

   polycnt = 0;
   G.diameter = 0;
   go_command(outline_lst,SIZ_OUTLINE_LST,id);
   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
*/
static int des_boardorg()
{
   int   id = cur_ident;

   fprintf(ferr,"BOARD ORIGIN not implemented\n");
   go_command(board_org_lst,SIZ_BOARD_ORG_LST,id);

   return 1;
}

/****************************************************************************/
/*
*/
static int geom_typ()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);

   if (!STRCMPI(lp,"X"))      // treat like crosshair -> smd
      G.padstackshape = PST_CROSS_HAIR;
   else
   if (!STRCMPI(lp,"TRIANGLE_HOLLOW"))
      G.geomtyp = T_TRIANGLE;
   else
   if (!STRCMPI(lp,"TRIANGLE"))
      G.geomtyp = T_TRIANGLE;
   else
   if (!STRCMPI(lp,"PLUS"))
      G.geomtyp = T_PLUS;
   else
   if (!STRCMPI(lp,"RECTANGLE"))
      G.geomtyp = T_RECTANGLE;
   else
   if (!STRCMPI(lp,"RECT_HOLLOW"))
      G.geomtyp = T_RECTANGLE;
   else
   if (!STRCMPI(lp,"EDGE_FINGER"))
      G.geomtyp = T_RECTANGLE;
   else
   if (!STRCMPI(lp,"EDGE_FINGER_HOLLOW"))
      G.geomtyp = T_RECTANGLE;
   else
   if (!STRCMPI(lp,"OBLONG"))
      G.geomtyp = T_OBLONG;
   else
   if (!STRCMPI(lp,"OBLONG_HOLLOW"))
      G.geomtyp = T_OBLONG;
   else
   if (!STRCMPI(lp,"SHAVED_ROUND"))
      G.geomtyp = T_OBLONG;
   else
   if (!STRCMPI(lp,"ROUND"))
      G.geomtyp = T_ROUND;
   else
   if (!STRCMPI(lp,"ROUND_ANNULAR"))
      G.geomtyp = T_ROUND;
   else
   if (!STRCMPI(lp,"ROUND_HOLLOW"))
      G.geomtyp = T_ROUND;
   else
   if (!STRCMPI(lp,"OCTAGON"))
      G.geomtyp = T_OCTAGON;
   else
   if (!STRCMPI(lp,"OCTAGON_HOLLOW"))
      G.geomtyp = T_OCTAGON;
   else
   if (!STRCMPI(lp,"THERMAL_RND_4X"))
      G.geomtyp = T_THERMAL;
   else
   {
      fprintf(ferr,"Unknown geometry type [%s]\n",lp);
      display_error++;
      G.geomtyp = T_UNDEFINED;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int geom_xdim()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.xdim = cnv_unit(atof(lp));
   return 1;
}

/****************************************************************************/
/*
*/
static int geom_ydim()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.ydim = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int geom_xoffset()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.xoff = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int geom_yoffset()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.yoff = cnv_unit(atof(lp));

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_rot()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   G.rotation = atof(lp);

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_mir()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   
   if (!STRCMPI(lp,"YES"))
      G.mirror = TRUE;
   else
      G.mirror = FALSE;

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_side()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   
   if (STRCMPI(lp,"TOP"))  // if not top
      G.sidebot = TRUE;
   else
      G.sidebot = FALSE;

   return 1;
}

/****************************************************************************/
/*
*/
static int eif_fill()
{
   char  *lp;

   lp = strtok(NULL,WORD_DELIMETER);
   cur_fill  = TRUE;

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int graph_outl()
{
   int   id = cur_ident;
              
   polycnt = 0;
   go_command(outl_lst,SIZ_OUTL_LST,id);
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int graph_shape()
{
   int   id = cur_ident;
   
   polycnt = 0;
   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int poly_line()
{
   int   id = cur_ident;

   go_command(graphline_lst,SIZ_GRAPHLINE_LST,id);

   // plane and complex guarantee a good line sequence. Others do not !!!
   if (cur_status != FILE_PLANE_OUTL && cur_status != FILE_CMPLX_LINE && cur_status != FILE_CMPLX_SHAPE)
   {
      if (polycnt)
         write_poly();
   }

   return 1;
}

/****************************************************************************/
/*
  line consists of
       XY
       LVL
       WDT
*/
static int outl_line()
{
   int   id = cur_ident;
   
   polycnt = 0;          // not here
   go_command(outline_lst,SIZ_OUTLINE_LST,id);
   G.lyrnr = Graph_Level(G.level,"",0);

   if (polycnt)
      write_poly();

   return 1;
}

/****************************************************************************/
/*
  text consists of
       XY
       LVL
       WDT
*/
static int graph_text()
{
   int   id = cur_ident;
   char  *lp;
   CString prosa;

   prosa = "";
   if ((lp = strword()) == NULL)
   {
      fprintf(ferr,"Error");
      display_error++;
   }
   else
   {
      prosa = lp;
   }

   polycnt = 0;
   G.mirror = 0;
   G.rotation = 0;

   go_command(graphtext_lst,SIZ_GRAPHTEXT_LST,id);

   write_text(prosa);

   return 1;
}

/****************************************************************************/
/*
  arc consists of
       PRI
       SEC
       XY
       ST_ANG
       SW_ANG
       LVL      
*/
static int poly_arc()
{
   int   id = cur_ident;
   
   go_command(grapharc_lst,SIZ_GRAPHARC_LST,id);

   int      p1 = polycnt;
   double   x1,y1,x2,y2,bulge;
   double   center_x = poly_l[polycnt-1].x;
   double   center_y = poly_l[polycnt-1].y;

   double   startangle = DegToRad(G.startangle);
   double   deltaangle = DegToRad(G.deltaangle);
   double   radius = G.radius;

   if (fabs(radius) > SMALLNUMBER)
   {
      // check if arc has a G.deltangle of 360. If yes, it is a circle
      if (fabs(fabs(G.deltaangle) - 360) < 1)
      {
         poly_l[polycnt-1].x = center_x-radius;
         poly_l[polycnt-1].y = center_y;
         poly_l[polycnt-1].bulge = 1;

         poly_l[polycnt].x = center_x+radius;
         poly_l[polycnt].y = center_y;
         poly_l[polycnt].bulge = 1;
         polycnt++;

         poly_l[polycnt].x = center_x-radius;
         poly_l[polycnt].y = center_y;
         poly_l[polycnt].bulge = 0;
         polycnt++;
      }  
      else
      {
         bulge = tan(deltaangle/4);
         x1 = center_x + radius * cos(startangle);
         y1 = center_y + radius * sin(startangle);
         x2 = center_x + radius * cos(startangle+deltaangle);
         y2 = center_y + radius * sin(startangle+deltaangle);

         poly_l[polycnt-1].x = x1;
         poly_l[polycnt-1].y = y1;
         poly_l[polycnt-1].bulge = bulge;

         poly_l[polycnt].x = x2;
         poly_l[polycnt].y = y2;
         poly_l[polycnt].bulge = 0;
         polycnt++;
      }
   }

   // plane and complex guarantee a good line sequence. Others do not !!!
   if (cur_status != FILE_PLANE_OUTL && cur_status != FILE_CMPLX_LINE && cur_status != FILE_CMPLX_SHAPE)
   {
      if (polycnt)
         write_poly();
   }

   return 1;
}

/****************************************************************************/
/*
  arc consists of
       PRI
       SEC
       XY
       ST_ANG
       SW_ANG
       LVL      
*/
static int outl_arc()
{
   int   id = cur_ident;
   int   p0 = polycnt;

   polycnt = 0;
   go_command(grapharc_lst,SIZ_GRAPHARC_LST,id);

   if (cur_status != FILE_UNKNOWN)
   {
      write_arc();
   }

/*
   int      p1 = polycnt;
   double   x1,y1,x2,y2,bulge;
   double   center_x = poly_l[polycnt-1].x;
   double   center_y = poly_l[polycnt-1].y;

   double   startangle = DegToRad(G.startangle);
   double   deltaangle = DegToRad(G.deltaangle);
   double   radius = G.radius;

   bulge = tan(deltaangle/4);
   x1 = center_x + radius * cos(startangle);
   y1 = center_y + radius * sin(startangle);
   x2 = center_x + radius * cos(startangle+deltaangle);
   y2 = center_y + radius * sin(startangle+deltaangle);

   poly_l[polycnt-1].x = x1;
   poly_l[polycnt-1].y = y1;
   poly_l[polycnt-1].bulge = bulge;

   poly_l[polycnt].x = x2;
   poly_l[polycnt].y = y2;
   poly_l[polycnt].bulge = 0;
   polycnt++;
*/
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
  circle consists of
       XY
       DIA
       LVL
*/
static int poly_circle()
{
   int   id = cur_ident;
   DbFlag   flg = 0L;
   
   //polycnt = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   if (cur_status != FILE_UNKNOWN)
   {
      write_circle();
   }
   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
  circle consists of
       XY
       DIA
       LVL
*/
static int outl_circle()
{
   int   id = cur_ident;
   
   polycnt = 0;
   go_command(graphcircle_lst,SIZ_GRAPHCIRCLE_LST,id);

   if (cur_status != FILE_UNKNOWN)
   {
      write_circle();
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int write_arc()
{
   int   err, widthindex;

   if (G.diameter == 0)
        widthindex = 0; // make it small width.
   else
     widthindex = Graph_Aperture("", T_ROUND, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   double   startangle = DegToRad(G.startangle);
   double   deltaangle = DegToRad(G.deltaangle);

   if (fabs(G.deltaangle - 360) < 1)
      Graph_Circle(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 
         0L, widthindex , FALSE, TRUE); 
   else
      Graph_Arc(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 
         startangle, deltaangle,       
         0L, widthindex , FALSE); 

   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int write_circle()
{
   int   err, widthindex;

   if (G.diameter == 0)
        widthindex = 0; // make it small width.
   else
     widthindex = Graph_Aperture("", T_ROUND, G.diameter , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   Graph_Circle(G.lyrnr,poly_l[0].x,poly_l[0].y, G.radius, 
         0L, widthindex , FALSE, TRUE); 

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int write_text(const char *tmp)
{
   int   prop = TRUE;   // veribest text is proportional
                        // also a text H125 W125 seems to be H125 W100

   if (polycnt && strlen(tmp))
   {
      if (G.mirror)
         G.rotation = 360 - G.rotation;
   
      int laynr = Graph_Level(G.level,"",0); 
      Graph_Text(laynr,tmp,poly_l[0].x,poly_l[0].y,G.height,G.diameter*TEXT_CORRECT,
               DegToRad(G.rotation),
               0L, prop, 
               G.mirror,  
               0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
  Check if a line ends with \n, meaning that the whole line was read.
*/
void check_buf(char *b)
{
   if (b[strlen(b)-1] != '\n')
   {
      fprintf(ferr,"Error, Line at %ld in [%s] not completely read.\n",
         fpcnt,cur_filename);
      display_error++;
   }
   else
      b[strlen(b)-1] = '\0';
   return;
}

/****************************************************************************/
/*
*/
static int overwrite_pad()
{
   int   ptr;
   char  padname[80];

   if (curpad_cnt == 0)  return 1;
   if (G.geomtyp == PART_VIA)
   {
      curpad_cnt = 0;
      return 1;
   }
   ptr = update_localpad(padname);

   if (lastpininsert)
   {
      lastpininsert->getInsert()->setBlockNumber(doc->Get_Block_Num(padname, -1, 0));
   }

   curpad_cnt = 0;
   return 1;
}

/******************************************************************************
* update_localpad
*/
static int update_localpad(char *padName)
{
   for (int i=0; i<localpadcnt; i++)
   {
      int found = FALSE;
      for (int i2=0; i2<localpad[i].curpadcnt && !found; i2++)
      {
         // to find an already defined pad, not only every layer must match,
         // but also the total number of layers.
         if (curpad_cnt != localpad[i].curpadcnt)
            continue;

         for (int i3=0; i3<curpad_cnt; i3++)
         {
            if (localpad[i].curpad[i2].layerptr == curpad[i3].layerptr &&
                !strcmp(localpad[i].curpad[i2].padgeomname, curpad[i3].padgeomname))
            {
                found = TRUE;
                break;
            }
         }
      }
      if (found)
      {
         sprintf(padName, "XPAD_%d", i);
         return i;
      }
   }

   sprintf(padName, "XPAD_%d", localpadcnt);
   if (localpadcnt < MAX_LOCALPAD)
   {
      localpad[localpadcnt].curpadcnt = curpad_cnt;
      if ((localpad[localpadcnt].curpad = (EIFCurPad *)calloc(curpad_cnt,sizeof(EIFCurPad))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

      memcpy(localpad[localpadcnt].curpad, curpad, curpad_cnt * sizeof(EIFCurPad));

      BlockStruct *curblock = Graph_Block_On(GBO_APPEND, padName, -1, 0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      int xpadFlag = 0;

      // also watch out for rotation
      for (int i=0; i<curpad_cnt; i++)
      {
         if (strlen(curpad[i].padgeomname))
         {
            Graph_Block_Reference(curpad[i].padgeomname, NULL, 0, 0, 0, 0, 0, 1, curpad[i].layerptr, TRUE);

            LayerStruct *layer = doc->FindLayer(curpad[i].layerptr);
            // here do SMT detect
            if (!STRNICMP(layer->getName(), "PadLayer", strlen("PadLayer")))
            {
               int layerNum = atoi(layer->getName().Right(strlen(layer->getName()) - strlen("PadLayer")));

               if (layerNum == 1)
                  xpadFlag |= 1;
               else if (layerNum == NumberOfLayers)
                  xpadFlag |= 2;
               else
                  xpadFlag |= 4;
            }
         }
      }

      // here check for SMD
      if (xpadFlag == 1 || xpadFlag == 2) // SMD
         doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);
/*    if (xpadflg == 1) // Top SMD
         doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);
      else if (xpadflg == 2) // Bottom only
         doc->SetAttrib(&curblock->getAttributesRef(), doc->IsKeyWord(ATT_BLINDSHAPE, TRUE), VT_NONE, NULL, SA_OVERWRITE, NULL);
*/
      Graph_Block_Off();

      localpadcnt++;
   }
   else
   {
      fprintf(ferr, "Too many local pads\n");
      display_error++;
      return -1;
   }

   return localpadcnt-1;
}

/****************************************************************************/
/*
*/
static int update_eiflevel(const char *l)
{
   int i;

   for (i=0;i<eiflayercnt;i++)
   {
      if (!strcmp(eiflayer[i].name, l))
         return i;
   }

   if (eiflayercnt < MAX_LAYERS)
   {
      eiflayer[eiflayercnt].mirror = eiflayercnt;
      if ((eiflayer[eiflayercnt].name = STRDUP(l)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      eiflayercnt++;
   }
   else
   {
      fprintf(ferr,"Too many Layers\n");
      display_error++;
      return -1;
   }

   return eiflayercnt-1;
}

/****************************************************************************/
/*
*/
static int do_padoverwrite(int shape)
{
   int i;

   for (i=0;i<padoverwritecnt;i++)
   {
      if (padoverwrite[i] == shape)
         return TRUE;
   }
   return FALSE;
}


/*Ende **********************************************************************/



