
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
#include "lyrmanip.h"
#include "edif_in.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress;

/* Function Define *********************************************************/
static   double   cnv_unit(char *);
static   int      go_command(List *,int);
static   void     free_all_mem();
static   void     init_all_mem();

static   int      get_tok();
static   int      p_error();
static   int      tok_search(List *,int);
static   int      push_tok();
static   int      loop_command(List *,int);
static   int      tok_layer();
static   void     clean_name(char *);
static   int      get_padnumber(char *);
static   int      get_next(char *,int);
static   int      get_line(char *,int);

static   int      get_nexttextline(char *orig,char *l,int maxl);
static   int      get_layerptr(const char *l);
static   int      get_nameindex(const char *n);

static   int      load_edifsettings(const CString fname);

static   int      get_drillindex(double size);
static   int      assign_layers();
static   int      get_logicalinstance(const char *l);
static   DataStruct *getInsertData(CString insertName);

static int FindVersion(CString fileName);
int ReadEDIF300(CString fileName, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits);
int ReadEDIF200(CString fileName, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits);

/* Globals ****************************************************************/
typedef  struct
{
   char     edifversion[3];

   int      cur_id;
   int      cur_status;
   /* Current values that are changed with global  */
   int      cur_layerindex;               /* Layer index */
   unsigned long color;
   int      cur_layerlistptr;             // index in layerlistptr
   int      cur_widthindex;               /* Line width     */
   double   cur_width;
   double   cur_height;                   /* Line height     */
   double   cur_left, cur_right;
   /* Current Component info used for Instance.    */
   double   cur_pos_x;                    /* position.   */
   double   cur_pos_y;
   double   cur_rotation;                 /* Rotation in degree.   */
   char     cur_mirror;                   /* Current mirror.   */
   char     cur_booleanvalue;             // true or false
   char     cur_visible;                  // true or false
   char     cur_placed;
   char     cur_display;
   double   cur_number;
   /* Current pad stack info.    */
   int      cur_pincnt;
   int      cur_pagecnt;
   /* Pad stacks */
   int      cur_textmirror;
   int      cur_textjust;
   int      cur_textrot;
   //
   char     cur_unit;
   char     cur_type;                     // used by layer and padshape 
   char     cur_typeval;                  // used by type value
   char     cur_boardoutline;             // flag is a boardoutline
   char     cur_attribute;                // a string in display() should not be written, because it is a attribute
   //
   int      layer_stackup;
} Global;
static   Global   G;                      // global const. uses memset -> no CString allowed

static   CString  cur_name;      // this is the name returned from rename
static   CString  EDIF_Att_Refname;
static   CString  edifname;      // 
static   CString  libraryname;   // 
static   CString  cellname;      // 
static   CString  cellrefname;   // 
static   CString  unitrefname;
static   CString  librefname;
static   CString  instancename;
static   CString  refName;
static   CString  pcbbareboarddefinitionref;
static   CString  pcbfootprintinstance;
static   CString  pcbfootprintdefinitionref;
static   CString  pcbphysicalfiguremacroref;
static   CString  schematicfiguremacroref;
static   CString  schematicsymbolportref;
static   CString  schematicsymbolref;
static   CString  pcbpadstackdefinitionref;
static   CString  toeprintdefinitionref;
static   CString  pcbphysicalnetname;

static   CFiguregrArray figuregrarray;
static   int         figuregrcnt;

static   CGeometrygrpArray geometrygrarray;
static   int         geometrygrcnt;

static   CNameArray  namearray;     // generic string collection
static   int         namecnt;

static   CLogicalInstanceArray   logicalinstancearray;
static   int         logicalinstancecnt;

static   FILE  *ferr;
static   FILE  *ifp;                            /* File pointers.    */
static   long  ifp_line = 0;                    /* Current line number. */
static   char  token[MAX_TOKEN];                /* Current token.       */
static   int   token_name;
//
static   EDIF_Attrib cur_attrib;

static   CString  cur_figuregroup;
static   CString  cur_layername;

static   char     cur_compname[40]; /* current used component name */
static   char     cur_padname[80];  /* current used padshape name */
static   CString  cur_libname;      /* current used library name */
static   CString  cur_portname;
static   char     cur_pinname[40];  /* current used pinnumber name */
static   CString  cur_netname;      /* current used netname name */
static   CString  cur_text;         // yes, this has to be big.
static   int      cur_libnum;       // edif allows same cell names in diffenrent library
                                    // example schematic, pcb library - both 1208

static   CString  designatorkey;    // the attribute for designator - REFNAME for
                                    // instance, PORT_%s for portinstance
static   CString  propkey;          // used in propdisplay;

/* different pins with same name can not exist */
static   int      Push_tok = FALSE;
static   char     cur_line[MAX_LINE];
static   int      cur_new = TRUE;
static   int      PageUnits;

static   EDIFLayerlist  layerlist[MAX_LAYERS];  // list of found edif400 layers
static   int         layerlistcnt;

static   EdifUnit    edifunit[MAX_EDIFUNITS];
static   int         edifunitcnt = 0;

static   CDrillArray drillarray;
static   int         drillcnt;
static   CString     orig_rename;

static   CRenameArray   renamearray;
static   int         renamecnt;

static   CShapePinNet   shapepinnet;
static   int         shapepinnetcnt;

typedef  CArray<Point2, Point2&> CPolyArray;

static   CAttribmapArray   attribmaparray;
static   int         attribmapcnt = 0;

static   CSettingsArray settingsarray;
static   int         settingscnt = 0;

static   int         polycnt =0 ;
static   CPolyArray  polyarray;

static   int         cur_filenum = 0;
static   FileStruct  *file = NULL;
static   FileStruct  *rootfile;

static   CCEtoODBDoc  *doc;
static   int         display_error = 0;

static   NetStruct   *curnetdata;
static   DataStruct  *curdata; 
static   CPoly       *curpoly;
static   TypeStruct  *curtype;
static   LayerStruct *curlayer;
static   BlockStruct *curblock;
static   double      faktor;
static   double      attribute_height;

static   CString     compoutline;   // name for primary comp outline from .in file

static   double      TEXTHEIGHTCORRECT = 1;
static   int         ATTRIBUTES2TEXT = 1;
static   double      TEXTRATIO = (6.0 / 8.0);
static   int         EDIF_FILETYPE;
static   int         ALLOW_VIEWRENAME;
static   int         ALLOW_CELLRENAME;

static   int         read_edif_version = EDIF200;  // EDIF200 EDIF400
static   int         viaplace;

static   double      dotsize = 0.02;   // in inch
static   int         loop_depth;
static   int         text_prop = 1;
static   int         angle_unit = 0;   // 0 = degree, 1 = radiens
static   DbFlag      attflg;
static   int         attribute_placement; // 0 = absolute, 1 = relative to curdata

static   CDataList   insertDataList(false);

/******************************************************************************
* ReadEDIF
*/
int ReadEDIF(const char *f, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
	int res = 1;
   file = NULL;
   doc = Doc;
   int read_edif_version = EDIF400; // EDIF200 EDIF400

   // this must be done correctly with the numberdefinition statement.
   PageUnits = pageunits;
   faktor = Units_Factor(UNIT_TNMETER, pageunits); // edif units to page units
   attribute_height = 0.07 * Units_Factor(UNIT_INCHES, pageunits);   // edif units to page units

   loop_depth = 0;   
   cur_line[0] = '\0';
   display_error = 0;
   curdata = NULL;
   curtype = NULL;
   curnetdata = NULL;
   cur_netname = "";
   curlayer = NULL;
   Push_tok = FALSE;
   cur_new = TRUE;
   layerlistcnt = 0;
   edifunitcnt = 0;
   viaplace = 0;
   attflg = 0;

   read_edif_version = FindVersion(f);
   if (read_edif_version == EDIF300)
   {
      res = ReadEDIF300(f, doc, Format, pageunits);
      return res;
   }
   else if (read_edif_version == EDIF200)
   {
      res = ReadEDIF200(f, doc, Format, pageunits);
      return res;
   }
   else if (read_edif_version == EDIF400)
      ErrorMessage("The selected EDIF file is in a 4.0 format.\nCAMCAD supports EDIF 2.0 for graphics only.\nNo netlist information will be loaded.\nPlease load an EDIF 3.0 file for complete intelligent Schematic support.", "");
   else
      return res;

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(f, "rt")) == NULL)
   {
      CString t;
      t.Format("Error open [%s] file", f);
      ErrorMessage(t, "Error");
      return res;
   }

   CString edifLogFile = GetLogfilePath(EDIFERR);
   if ((ferr = fopen(edifLogFile, "wt")) == NULL)
   {
      CString t;
      t.Format("Error open [%s] file", edifLogFile);
      ErrorMessage(t, "Error");
      return res;
   }
   fprintf(ferr, "File : %s\n", f);

   init_all_mem();
   insertDataList.RemoveAll();

   file = NULL;

   CString settingsFile( getApp().getImportSettingsFilePath("edif.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nEDIF Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_edifsettings(settingsFile);

   attribute_placement = 0;


   /* initialize scanning parameters */
   rewind(ifp);
   ifp_line = 0; /* Current line number. */

   rootfile = file = Graph_File_Start(f, Type_EDIF);

   cur_filenum = file->getFileNumber();
                              
   // initialize a layer
   G.cur_layerindex = Graph_Level("0", "", 1);
   cur_layername = "0";

   if (loop_command(start_lst, SIZ_START_LST) < 0)
   {
      // printf big prob.
      CString tmp;
      tmp.Format("EDIF syntax error [%s] encountered at %ld\nSee %s for messages.", 
         token, ifp_line, edifLogFile );
      ErrorMessage(tmp, "EDIF Read Error");
   }

   if (read_edif_version == EDIF400)   // EDIF200 EDIF400
   {
      assign_layers();
   }

   insertDataList.RemoveAll();
   free_all_mem();
   fclose(ferr);
   fclose(ifp);

   doc->RemoveUnusedLayers();
   EliminateSinglePointPolys(doc);                

   POSITION pos = doc->getFileList().GetHeadPosition();
   double accuracy = get_accuracy(doc);
   while (pos)
   {
      file = doc->getFileList().GetNext(pos);
      Crack(doc, file->getBlock(), TRUE);         
      EliminateOverlappingTraces(doc, file->getBlock(), TRUE, accuracy);               
   }

   if (display_error)
      Logreader(edifLogFile);

   return res;
}

/******************************************************************************
* FindVersion
*/
static int FindVersion(CString fileName)
{
   CStdioFile fMen;
   if (!fMen.Open(fileName, CFile::modeRead | CFile::typeText))
   {
      ErrorMessage("Error open file", fileName);
      return -1;
   }

   CString line;
   int version = EDIF_UNKNOWN;
   while (fMen.ReadString(line))
   {
      line.MakeUpper();
      int index = line.Find("EDIFVERSION");
      if (index > 0)
      {
         line.Delete(0, index + (int)strlen("edifVersion"));
         line.TrimLeft();
         line.TrimRight();
         line.Replace(" ", "");

         index = line.ReverseFind(')');
         if (index > 0)
            line.Delete(index, 1);

         version = atoi(line);

         switch(version)
         {
         case 200:
            version = EDIF200;
            break;
         case 300:
            version = EDIF300;
            break;
         case 400:
            version = EDIF400;
            break;
         }
         break;
      }
   }

   fMen.Close();
   return version;
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
* char *get_rename
*/
static const char *get_rename(const char *name,int level)
{
   int nameIndex = get_nameindex(name);

   for (int i=0; i<renamecnt; i++)
   {
      EDIFRename rename = renamearray.ElementAt(i);
      if (rename.renameindex == nameIndex ) // && p.loop_depth == level)
         return namearray[rename.orignameindex]->name;
   }

   return name;
}

/******************************************************************************
* update_rename
   a rename must be deleted after the level is higher, because there maybe
   a level 6 rename inside a cell, and a get_rename on a level 6 which
   has nothing to do with the original inside cell rename.
*/
static int update_rename(int level)
{
   if (renamecnt)
   {
      EDIFRename rename = renamearray.ElementAt(renamecnt-1);
      while (rename.loop_depth > level)
      {
         renamearray.RemoveAt(renamecnt-1);
         renamecnt--;
         if (renamecnt)
            rename = renamearray.ElementAt(renamecnt-1);
         else
            return 1;
      }
   }

   return 1;
}

/******************************************************************************
* go_command()
*
*   Call function associated with next token.
*   Tokens enclosed by () are searched for on the local
*   token list.
*/
int go_command(List *tok_lst, int lst_size)
{
   int i, brk, err;
   
   if (!get_tok()) 
      return p_error();

   switch((brk = tok_search(brk_lst, SIZ_BRK_LST)))
   {
   case BRK_ROUND:
      {
         if (get_tok() && (i = tok_search(tok_lst, lst_size)) >= 0) 
            err = (*tok_lst[i].function)();
         else
         {
#ifdef _DEBUG
            fprintf(ferr, "DEBUG: Not in Loop - ");
            fnull(); // unknown command, log and skip
#else
            fskip();
#endif
         }
      }
      break;
   case BRK_B_ROUND:
      {
         push_tok();                                                        
         return 1;
      }
   default:
      return p_error();
   }

   if (err < 0)
      return -1;
   if (!get_tok())
      return p_error();
   if (brk != tok_search(brk_lst, SIZ_BRK_LST) - 1)
      return p_error();

   return 1;
}

/******************************************************************************
* loop_command()
*  - Loop through a section.
*/
static int loop_command(List *list, int size)
{
   loop_depth++;  // counts the level in EDIF ident

   int repeat = TRUE;
   while (repeat)
   {
      if (go_command(list, size) < 0)
         return -1;

      if (!get_tok())
      {
         if (feof(ifp))
            return 0;

         return p_error();
      }

      if (tok_search(brk_lst, SIZ_BRK_LST) == BRK_B_ROUND)
         repeat = FALSE;
      push_tok();
   }

   loop_depth--;
   update_rename(loop_depth); // all renames below this need to be deleted
   return 1;
}

/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next ) endpoint.
*/
static int fnull()
{
   int brk_count = 0;
   CString t = token;   // need for debug

   fprintf(ferr, "Token [%s] unknown from %ld to ", token, ifp_line);
   display_error++;
   while (TRUE)
   {
      if (get_tok())
      {
         t = token;
         switch (tok_search(brk_lst, SIZ_BRK_LST))
         {
         case BRK_ROUND:
            ++brk_count;
            break;
         case BRK_B_ROUND:
            if (brk_count-- == 0)
            {
               push_tok();
               fprintf(ferr, "%ld \n", ifp_line);
               return 1;
            }
            break;
         default:
            // fprintf(ferr," -> Token [%s] at %ld unknown\n",token,ifp_line);
            break;
         }
      }
      else
         return p_error();
   }

   fprintf(ferr, " undefined end found at %ld\n", ifp_line);
   return 0;
}

/******************************************************************************
* fskip
   NULL function.
   Skips over any tokens to next () endpoint.
*/
static int fskip()
{
   int brk_count = 0;

   while (TRUE)
   {
     if (get_tok())
      {
         switch (tok_search(brk_lst, SIZ_BRK_LST))
         {
         case BRK_ROUND:
            ++brk_count;
            break;
         case BRK_B_ROUND:
            if (brk_count-- == 0)
            {
               push_tok();
               return 1;
            }
            break;
         default:
            break;
         }
      }
      else
         return p_error();
   }
   return 0;
}

/******************************************************************************
* write_attributes
   attribute_placement = 0 use the cur_pos as the placement
                         1 make cur_pos relative
*/
static int write_attributes(const char *attkey, const char *attval)
{
   // normalize
   normalize_text(&G.cur_pos_x, &G.cur_pos_y, G.cur_textjust, G.cur_rotation,
         G.cur_mirror, G.cur_height, G.cur_height * TEXTRATIO * strlen(attval));
   
   if (G.cur_visible == FALSE)
   {
      CString l;
      l.Format("%s (not visible)", cur_layername);
      G.cur_layerindex = Graph_Level(l, "", 0);
      LayerStruct *lay = doc->FindLayer(G.cur_layerindex);
      lay->setVisible(false);
   }
   else
   {
      G.cur_layerindex = Graph_Level(cur_layername, "", 0);
   }

   // no mirror on text, but need to adjust non mirror text, 
   // because mirror is written from right to left, non mirror left to right
   if (G.cur_mirror)
   {
      double tx = 0;
      double ty = 0;
      Rotate(G.cur_height * TEXTRATIO * strlen(attval), 0.0, G.cur_rotation, &tx, &ty);
      G.cur_pos_x = G.cur_pos_x - tx;
      G.cur_pos_y = G.cur_pos_y - ty;
   }

   if (ATTRIBUTES2TEXT)
   {
      if (strlen(attval) && G.cur_placed)
      {
         Graph_Text(G.cur_layerindex, attval, G.cur_pos_x, G.cur_pos_y, G.cur_height, G.cur_height * TEXTRATIO,
               DegToRad(G.cur_rotation), 0, text_prop, 0, 0, 0, 0, 0);
      }
   }
   else
   {
      // all attributes are place absolute, and after a instance, an adjustment is made
      if (curdata)
      {
         double x = G.cur_pos_x;
         double y = G.cur_pos_y;
         if (curdata->getInsert() && read_edif_version == EDIF300)
         {
            x -= curdata->getInsert()->getOriginX();
            y -= curdata->getInsert()->getOriginY();
         }
         doc->SetUnknownVisAttrib(&curdata->getAttributesRef(), attkey, attval, x, y,
               DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
               (G.cur_visible & G.cur_placed), SA_OVERWRITE, attflg, G.cur_layerindex, 1, 0, 0);
      }
      else if (curblock)
      {
         double x = G.cur_pos_x - curblock->getXoffset();
         double y = G.cur_pos_y - curblock->getYoffset();
         doc->SetUnknownVisAttrib(&curblock->getAttributesRef(), attkey, attval, G.cur_pos_x, G.cur_pos_y, 
               DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
               (G.cur_visible & G.cur_placed), SA_OVERWRITE, 0l, G.cur_layerindex, 1, 0, 0);
      }
   }
   return 1;
}

/******************************************************************************
* edif_numbervalue
*/
static int edif_numbervalue(double *num)
{
   if (!get_tok())
      return p_error();  // name

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      if (go_command(e_construct_lst, SIZ_E_CONSTRUCT_LST) < 0)
         return -1;

      Push_tok = FALSE;
      *num = G.cur_number;
   }
   else
   {
      *num = atoi(token);
   }                                             

   return 1;
}

/******************************************************************************
* set_figuregroupdefaults
*/
static int set_figuregroupdefaults(const char *fn)
{
   for (int i=0; i<figuregrcnt; i++)
   {
      Figuregrp *figure = figuregrarray[i];
      if (!STRCMPI(figure->name,fn))
      {
         G.cur_height = figure->textheight;
         G.cur_widthindex = figure->pathwidthindex;
         break;
      }
   }
   return 1;
}

/******************************************************************************
* static void  convert_special_char
   name or rename
*/
static void convert_special_char(char *t)
{
   int esc = 0;
   CString n = "";
   CString m = "";

   for (int i=0; i<(int)strlen(t); i++)
   {
      if (t[i] == '%')
      {
         if (esc)
         {
            char tmp[255];
            char *lp;
            strcpy(tmp, m);
            if (lp = strtok(tmp, " \t"))
            {
               n += (char)atoi(lp);
               while (lp = strtok(NULL, " \t"))
               {
                  n += (char)atoi(lp);
               }
            }
            m = "";
            esc = FALSE;
         }
         else
            esc = TRUE;
      }
      else
      {
         if (esc)
            m += t[i];
         else
            n += t[i];
      }
   }

   strcpy(t, n);
   return;
}

/******************************************************************************
* edif_namedef
*/
static int edif_namedef()
{
   CString old_layer = cur_layername;
   cur_layername += "_NAME";
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);
   G.cur_display = FALSE;

   if (!get_tok())
      return p_error();  // EDIF
   cur_name = token;
   orig_rename = token;

   if (token[0] == '(' && !token_name)
   {
      cur_name = "";
      push_tok();
      if (go_command(edif_name_lst, SIZ_EDIF_NAME_LST) < 0)
      {
         cur_layername = old_layer;
         G.cur_layerindex = Graph_Level(cur_layername, "", 0);
         return -1;
      }
   }
   
   cur_layername = old_layer;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   return 1;
}

/******************************************************************************
* edif_nameref()
*/
static int edif_nameref()
{
   CString old_layer = cur_layername;
   cur_layername += "_NAME";
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   if (!get_tok())
      return p_error();  // EDIF
   cur_name = token;
   orig_rename = token;
                                
   if (token[0] == '(' && !token_name)
   {
      push_tok();
      if (go_command(edif_name_lst, SIZ_EDIF_NAME_LST) < 0)
      {
         cur_layername = old_layer;
         G.cur_layerindex = Graph_Level(cur_layername, "", 0);
         return -1;
      }

   }

   cur_layername = old_layer;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   return 1;
}

/******************************************************************************
* edif_propertynameref()
*  - name or rename
*/
static int edif_propertynameref()
{
   if (!get_tok())
      return p_error();  // EDIF
   cur_name = token;

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      if (go_command(edif_name_lst, SIZ_EDIF_NAME_LST) < 0)
         return -1;
   }
   
   return 1;
}

/******************************************************************************
* edif_portname
*/
static int edif_portname()
{
   if (!get_tok())
      return p_error();  
   cur_name = get_rename(token, loop_depth);

   while (TRUE)
   {
      if (!get_tok())
         return p_error();  

      if (token[0] == '(' && !token_name)
      {
         push_tok();
         propkey = cur_layername;
         if (loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST) < 0)
            return -1;

      } 
      else
      {
         push_tok();
         break;
      }
   }

   return 1;
}

/******************************************************************************
* portnameref
*/
static int portnameref()
{
   edif_portname();
   cur_portname = cur_name;

   CString port_key;
   port_key.Format("PORT_%s", cur_name);
/*
   BlockStruct *b = Graph_Block_On(GBO_APPEND, cur_libname, cur_libnum, 0L);// declares and begins defining a block
   Graph_Block_Off();

   // this is newly added.
   if (b)
   {
      // here add to block.
      doc->SetAttrib(&b->getAttributesRef(),doc->RegisterKeyWord(port_key, 0),
            VT_STRING,
            cur_portname.GetBuffer(0),
            G.cur_pos_x, G.cur_pos_y, DegToRad(G.cur_rotation),
            G.cur_height, (G.cur_visible & G.cur_placed), SA_OVERWRITE, 0l,
            G.cur_layerindex,1); // x, y, rot, height
   }
*/
   return 1;
}

/******************************************************************************
* edif_typedvalue
*/
static int edif_typedvalue()
{
   CString name; 
   G.cur_typeval = TYPEVAL_UNKNOWN;

   if (!get_tok())
      return p_error();
   cur_text = token;

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      if (go_command(edif_typedvalue_lst, SIZ_EDIF_TYPEDVALUE_LST) < 0)
         return -1;
   }
   
   return 1;
}

/******************************************************************************
* edif_stringvalue
*/
static int edif_stringvalue()
{
   if (!get_tok())
      return p_error();
   cur_name = token;

   if (token[0] == '(' && !token_name) // name can start with a quote
   {
      push_tok();
      if (go_command(edif_name_lst, SIZ_EDIF_NAME_LST) < 0)
         return -1;
   }
   
   return 1;
}

/******************************************************************************
* edif_rename
*/
static int edif_rename()
{
   if (!get_tok())
      return p_error();  
   cur_name = token;
   orig_rename = token;

   if (!get_tok())
      return p_error();  

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      if (loop_command(edif_stringdisplay_lst, SIZ_EDIF_STRINGDISPLAY_LST) < 0)
         return -1;
   }
   else
   {
      cur_name = token;
   }

   EDIFRename rename;
   rename.renameindex = get_nameindex(orig_rename);
   rename.orignameindex = get_nameindex(cur_name);
   rename.loop_depth = loop_depth;
   renamearray.SetAtGrow(renamecnt++, rename);  

   return 1;
}

/******************************************************************************
* edif_name
*/
static int edif_name()
{
   if (!get_tok())
      return p_error();  
   cur_name = get_rename(token, loop_depth);

   while (TRUE)
   {
      if (!get_tok())
         return p_error();  

      if (token[0] == '(' && !token_name)
      {
         push_tok();
         if (loop_command(display_lst, SIZ_DISPLAY_LST) < 0)
            return -1;
      } 
      else
      {
         push_tok();
         break;
      }
   }

   return 1;
}

/******************************************************************************
* edif_propertydisplay
*/
static int edif_propertydisplay()
{                                                 


   CString old_lay = cur_layername;

   edif_propertynameref();
   CString prop_key = get_attribmap(get_rename(cur_name, loop_depth));
   prop_key.MakeUpper();

   cur_layername = prop_key;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   CString prop_val = "";
   if (curdata)
   {
      Attrib *attrib =  is_attvalue(doc, curdata->getAttributesRef(), prop_key, 0);
      if (attrib)
         prop_val = get_attvalue_string(doc, attrib);

      cur_name = prop_val;
   }
   else if (curblock)
   {
      Attrib *attrib =  is_attvalue(doc, curblock->getAttributesRef(), prop_key, 0);
      if (attrib)
         prop_val = get_attvalue_string(doc, attrib);

      cur_name = prop_val;
   }

   propkey = cur_layername;
   if (loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST) < 0)
      return -1;

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   if (curdata)
   {
      double x = G.cur_pos_x - curdata->getInsert()->getOriginX();
      double y = G.cur_pos_y - curdata->getInsert()->getOriginY();
      doc->SetUnknownVisAttrib(&curdata->getAttributesRef(), prop_key, prop_val, G.cur_pos_x, G.cur_pos_y, 
            DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
            G.cur_visible, SA_OVERWRITE, attflg, G.cur_layerindex, 1, 0, 0);
   }
   else if (curblock)
   {
      // here add to block.
      double x = G.cur_pos_x - curblock->getXoffset();
      double y = G.cur_pos_y - curblock->getYoffset();
      doc->SetUnknownVisAttrib(&curblock->getAttributesRef(), prop_key, prop_val, x, y, 
            DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
            G.cur_visible, SA_OVERWRITE, attflg, G.cur_layerindex, 1, 0, 0);
   }

   return 1;
}

/******************************************************************************
* instance_propertydisplay
*/
static int instance_propertydisplay()
{                                                 
   CString old_lay = cur_layername;
   int atrtribiute2text = ATTRIBUTES2TEXT;
   ATTRIBUTES2TEXT = TRUE;

   edif_propertynameref();
   CString prop_key = get_attribmap(get_rename(cur_name, loop_depth));

   cur_layername = prop_key;
   G.cur_layerindex = Graph_Level(prop_key, "", 0);
   propkey = cur_layername;

   if (loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST) < 0)
      return -1;

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);
   ATTRIBUTES2TEXT = atrtribiute2text;

   return 1;
}

/******************************************************************************
* instance_nameinformation
*/
static int instance_nameinformation()
{                                                 
   CString old_lay = cur_layername;
   int atrtribiute2text = ATTRIBUTES2TEXT;
   ATTRIBUTES2TEXT = TRUE;

   edif_propertynameref();
   CString prop_key = get_attribmap(get_rename(cur_name, loop_depth));

   cur_layername = prop_key;
   G.cur_layerindex = Graph_Level(prop_key, "", 0);
   propkey = cur_layername;

   if (loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST) < 0)
      return -1;

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);
   ATTRIBUTES2TEXT = atrtribiute2text;

   return 1;
}

/******************************************************************************
* start_edif
*/
static int start_edif()
{
   edif_namedef();
   edifname = cur_name;

   return loop_command(edif_lst, SIZ_EDIF_LST);
}

/******************************************************************************
* edif_version
*/
static int edif_version()
{
   if (!get_tok())
      return p_error();  
   G.edifversion[0] = atoi(token);

   if (!get_tok())
      return p_error();  
   G.edifversion[1] = atoi(token);

   if (!get_tok())
      return p_error();  
   G.edifversion[2] = atoi(token);

   int ver = G.edifversion[0] * 100 + G.edifversion[1] * 10 + G.edifversion[2];
   if (ver == 200)
      read_edif_version = EDIF200;
   else if (ver == 300)
      read_edif_version = EDIF300;
   else if (ver == 400)
      read_edif_version = EDIF400;
   else
   {
      read_edif_version = EDIF_UNKNOWN;

      CString tmp;
      tmp.Format("EDIF Version %d %d %d found at %ld", G.edifversion[0], G.edifversion[1], G.edifversion[2], ifp_line);
      ErrorMessage(tmp, "Unsupported EDIF Version");
      return -1;
   }
   return 1;
}

/******************************************************************************
* edif_status
*/
static int edif_status()
{
   return loop_command(status_lst, SIZ_STATUS_LST);
}

/******************************************************************************
* status_written
*/
static int status_written()
{
   return loop_command(written_lst, SIZ_WRITTEN_LST);
}

/******************************************************************************
* get_programsettings
*/
static int get_programsettings(const char *p)
{
   for (int i=0; i<settingscnt; i++)
   {
      EDIFSettings *settings = settingsarray[i];
      if (!settings->programname.CompareNoCase(p))
         return i;
   }

   return -1;
}

/******************************************************************************
* make_programsettings
*/
static int make_programsettings(const char *programName)
{
   for (int i=0; i<settingscnt; i++)
   {
      if (!settingsarray[i]->programname.CompareNoCase(programName))
         return i;
   }

   EDIFSettings *settings = new EDIFSettings;
   settingsarray.SetAtGrow(settingscnt++, settings);
   settings->programname = programName;
   settings->textratio = TEXTRATIO;
   settings->textheightcorrect = TEXTHEIGHTCORRECT;
   settings->attributeplacement = attribute_placement;

   return settingscnt-1;
}

/******************************************************************************
* written_program
*/
static int written_program()
{
   edif_string();

   int pp = get_programsettings(cur_text);

   // in esiwrite, a (view (rename ic_2D_mpc850_2D_lv "ic-mpc850-lv") is out of
   // scope in the instance 
   //    (instance (rename I_24_12022 "I$12022")
   //       (viewRef ic_2D_mpc850_2D_lv (cellRef ic_2D_mpc850_2D_lv_pg))

   //if (!STRCMPI(cur_text,"ESIWRITE"))
   ALLOW_VIEWRENAME = FALSE;
   ALLOW_CELLRENAME = FALSE;

   if (pp > -1)
   {
      TEXTHEIGHTCORRECT = settingsarray[pp]->textheightcorrect;
      TEXTRATIO = settingsarray[pp]->textratio;
      attribute_placement = settingsarray[pp]->attributeplacement;
   }
   else
   {
#ifdef _DEBUG
      fprintf(ferr, "DEBUG: Unevaluated Status : %s\n", cur_text);
      display_error++;
#endif
   }

   fskip();

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_level()
{
   int   level;

   if (!get_tok())   return p_error();  
   level = atoi(token);

   if (level > 0)
   {
      CString  tmp;

      tmp.Format("EDIF Level %d found at %ld\nOnly Level 0 is supported.",
         level, ifp_line);

      ErrorMessage(tmp, "Unsupported EDIF Level",  MB_OK | MB_ICONHAND);
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_header()
{
   return loop_command(header_lst,SIZ_HEADER_LST);
}

/****************************************************************************/
/*
*/
static int edif_library()
{
   edif_namedef();
   libraryname = cur_name;

   switch(read_edif_version)
   {
      case EDIF400:
         file = Graph_File_Start(libraryname, Type_EDIF);
         file->setShow(false); 
         //panelfile = Graph_File_Start(cur_name);
         //cur_filenum = file->getFileNumber(); // this needs to stay with the original 
         cur_libnum = file->getFileNumber();

         file->setBlockType(blockTypeUnknown); // BLOCKTYPE_LIBRARY
         if (loop_command(library400_lst,SIZ_LIBRARY400_LST) < 0)
            return -1;
      break;
      case EDIF200:
      {
         int   err;

         file = Graph_File_Start(libraryname, Type_EDIF);
         //panelfile = Graph_File_Start(cur_name);
         //cur_filenum = file->getFileNumber(); // this needs to stay with the original 
         cur_libnum = file->getFileNumber();

         file->setBlockType(blockTypeLibrary);
         file->setShow(false); 

         double   xsize = dotsize * Units_Factor(UNIT_INCHES, PageUnits);
         Graph_Aperture("EDIF_DOT", T_ROUND, xsize, 0.0,0.0, 0.0, 0.0, 0,
                     BL_APERTURE, FALSE, &err); // ensure that aperature is in database

         if (loop_command(library200_lst,SIZ_LIBRARY200_LST) < 0)
            return -1;
      }
      break;      
      case EDIF300:
      {
         int   err;

         file = Graph_File_Start(libraryname, Type_EDIF);
         //panelfile = Graph_File_Start(cur_name);
         //cur_filenum = file->getFileNumber(); // this needs to stay with the original 
         cur_libnum = file->getFileNumber();

         file->setBlockType(blockTypeLibrary);
         file->setShow(false); 

         double   xsize = dotsize * Units_Factor(UNIT_INCHES, PageUnits);
         Graph_Aperture("EDIF_DOT", T_ROUND, xsize, 0.0,0.0, 0.0, 0.0, 0,
                     BL_APERTURE, FALSE, &err); // ensure that aperature is in database

         if (loop_command(library300_lst,SIZ_LIBRARY300_LST) < 0)
            return -1;
      }
      break;
      default:
         ErrorMessage("Unknown EDIF Version");
         fnull();
         return -1;
      break;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int joined200()
{
   return fskip();
}

/****************************************************************************/
/*
*/
static int page200()
{
   edif_namedef();

   // cur_library is the cell with the view. there 
   libraryname.Format("%s_%s", cur_libname, cur_name);   // cells can have pages 

   file = Graph_File_Start(libraryname, Type_EDIF);
   //panelfile = Graph_File_Start(cur_name);
   //cur_filenum = file->getFileNumber(); // this needs to stay with the original 
   file->setBlockType(blockTypeSheet); 
   file->setShow(G.cur_pagecnt == 0);  
   cur_libnum = file->getFileNumber();

   curblock = NULL;
   curdata = NULL;

   if (loop_command(page200_lst,SIZ_PAGE200_LST) < 0)
      return -1;

   curblock = NULL;
   curdata = NULL;
   G.cur_pagecnt++;
   return 1;
}

/******************************************************************************
* logicalConnectivity
*/
static int logicalConnectivity()
{
   curblock = NULL;
   curdata = NULL;

   if (loop_command(logicalConnectivity_lst, SIZ_LOGICALCONNECTIVITY_LST) < 0)
      return -1;

   curblock = NULL;
   curdata = NULL;
   return 1;
}

/****************************************************************************/
/*
*/
static int schematicImplementation()
{
   curblock = NULL;
   curdata = NULL;

   if (loop_command(schematicImplementation_lst,SIZ_SCHEMATICIMPLEMENTATION_LST) < 0)
      return -1;

   curblock = NULL;
   curdata = NULL;
   return 1;
}

/****************************************************************************/
/*
*/
static int page300()
{
   edif_namedef();
   libraryname.Format("%s_%s", cellname, cur_name);   // cells can have pages 

   file = Graph_File_Start(libraryname, Type_EDIF);
   //panelfile = Graph_File_Start(cur_name);
   //cur_filenum = file->getFileNumber(); // this needs to stay with the original 
   file->setBlockType(blockTypeSheet); 
   file->setShow(G.cur_pagecnt == 0);  
   cur_libnum = file->getFileNumber();

   curblock = NULL;
   curdata = NULL;

   if (loop_command(page300_lst,SIZ_PAGE300_LST) < 0)
      return -1;

   curblock = NULL;
   curdata = NULL;
   G.cur_pagecnt++;
   return 1;
}

/****************************************************************************/
/*
*/
static int edif200_library()
{
   edif_namedef();
   libraryname = cur_name;

   file = Graph_File_Start(libraryname, Type_EDIF);
   //panelfile = Graph_File_Start(cur_name);
   //cur_filenum = file->getFileNumber(); // this needs to stay with the original 
   file->setBlockType(blockTypeLibrary);

   cur_libnum = file->getFileNumber();

   return loop_command(library200_lst,SIZ_LIBRARY200_LST);

}

/****************************************************************************/
/*
*/
static int unitDefinitions()
{
   int   res;

   res = loop_command(unitDefinitions_lst,SIZ_UNITDEFINITIONS_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pcbGeometricAttributeDefault()
{
   CString  attname;

   edif_namedef();
   attname = cur_name;

   int res = loop_command(pcbGeometricAttributeDefault_lst,SIZ_PCBGEOMETRICATTRIBUTDEFAULT_LST);

   Geometrygrp *g = new Geometrygrp;

   g->name = attname;
   g->widthindex = G.cur_widthindex;

   geometrygrarray.SetAtGrow(geometrygrcnt,g);
   geometrygrcnt++;

   return 1;
}

/****************************************************************************/
/*
*/
static int physicalScaling()
{
   int   res;

   res = loop_command(physicalScaling_lst,SIZ_PHYSICALSCALING_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int physicalDefaults()
{
   int   res;

   res = loop_command(physicalDefaults_lst,SIZ_PHYSICALDEFAULTS_LST);

   return res;
}


/****************************************************************************/
/*
*/
static int setDistance()
{
   int   res;
   int   i;

   unitrefname = "";
   res = loop_command(setDistance_lst,SIZ_SETDISTANCE_LST);

   for (i=0;i<edifunitcnt;i++)
   {
      if (!strcmp(edifunit[i].name, unitrefname))
      {
         // SI units are in METER 
         faktor = Units_Factor(UNIT_MM, PageUnits);   // METER to page units
         faktor = faktor * (edifunit[i].n2 / edifunit[i].n1) * 1000;
         return 1;
      }
   }

   fprintf(ferr,"UnitRef [%s] not found at %ld\n", unitrefname, ifp_line);
   display_error++;

   return res;
}

/****************************************************************************/
/*
*/
static int setAngle()
{
   int   res;

   unitrefname = "";
   res = loop_command(setAngle_lst,SIZ_SETANGLE_LST);

   if (unitrefname.CompareNoCase("DEGS") == 0)
   {
      angle_unit = 0;
   }
   else
   {
      fprintf(ferr,"UnitRef for Angle [%s] not known at %ld\n", unitrefname, ifp_line);
      display_error++;
   }

   return res;
}

/****************************************************************************/
/*
*/
static int pcbLayoutUnits()
{
   int   res;

   res = loop_command(pcbLayoutUnits_lst,SIZ_PCBLAYOUTUNITS_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicUnits()
{
   int   res;

   res = loop_command(schematicUnits_lst,SIZ_SCHEMATICUNITS_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicRequiredDefaults()
{
   int   res;

   res = loop_command(schematicRequiredDefaults_lst,SIZ_SCHEMATICREQUIREDDEFAULTS_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicMetric()
{
   int   res;

   res = loop_command(schematicMetric_lst,SIZ_SCHEMATICMETRIC_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicSymbolHeader()
{
   int   res;

   res = loop_command(schematicSymbolHeader_lst,SIZ_SCHEMATICSYMBOLHEADER_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int library300_schematicFigureMacro()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(schematicFigureMacro_lst,SIZ_SCHEMATICFIGUREMACRO_LST);
   Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int library300_pageTitleBlockTemplate()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   curblock = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(schematicFigureMacro_lst,SIZ_SCHEMATICFIGUREMACRO_LST);
   Graph_Block_Off();
   curblock = NULL;
   return res;
}

/****************************************************************************/
/*
*/
static int library300_pageBorderTemplate()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   curblock = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(schematicFigureMacro_lst,SIZ_SCHEMATICFIGUREMACRO_LST);
   Graph_Block_Off();
   curblock = NULL;

   return res;
}

/****************************************************************************/
/*
*/
static int library300_schematicSymbolPortTemplate()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(schematicFigureMacro_lst,SIZ_SCHEMATICFIGUREMACRO_LST);
   Graph_Block_Off();
   
   return res;
}


/****************************************************************************/
/*
*/
static int library300_schematicGlobalPortTemplate()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(schematicFigureMacro_lst,SIZ_SCHEMATICFIGUREMACRO_LST);
   Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int library300_schematicOnPageConnectorTemplate()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(schematicFigureMacro_lst,SIZ_SCHEMATICFIGUREMACRO_LST);
   Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int library300_schematicOffPageConnectorTemplate()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(schematicFigureMacro_lst,SIZ_SCHEMATICFIGUREMACRO_LST);
   Graph_Block_Off();
   
   return res;
}

/******************************************************************************
* instanceNameDisplay
*/
static int instanceNameDisplay()
{
   propkey = ATT_REFNAME;

   int res = loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST);

   return res;
}

/******************************************************************************
* schematicSymbolPortImplementation
*/
static int schematicSymbolPortImplementation()
{
   int res;

   edif_namedef();   // cur_name 
   CString portname = cur_name;

   curdata = doc->getCamCadData().getNewDataStruct(dataTypeInsert);
   //curdata->getPolyList() = NULL;
   //curdata->getText() = NULL;
   //curdata->getInsert() = NULL;
   //curdata->getPoint() = NULL;
   //curdata->getBlob() = NULL;
   //curdata->getAttributesRef() = NULL;

   schematicsymbolportref = "";

   res = loop_command(schematicPortMacro_lst, SIZ_SCHEMATICPORTMACRO_LST);

   if (strlen(schematicsymbolportref))
   {
      DataStruct *data = Graph_Block_Reference(schematicsymbolportref, portname, 0, G.cur_pos_x, G.cur_pos_y, 
                         DegToRad(G.cur_rotation), G.cur_mirror, 1.0, -1, TRUE);  

      data->getInsert()->setInsertType(insertTypePortInstance);

      if (curdata->getAttributesRef())
         data->getAttributesRef() = curdata->getAttributesRef();
   }

   delete curdata;
   curdata = NULL;
   return res;
}

/******************************************************************************
* portAttributeDisplay
*/
static int portAttributeDisplay()
{
   double tmpX = G.cur_pos_x;
   double tmpY = G.cur_pos_y;
   double tmpRot = G.cur_rotation;
   int tmpMir = G.cur_mirror;

   CString tmpDesignator = designatorkey;

   designatorkey.Format("PORTDES %s", cur_name);
   designatorkey.TrimLeft();
   designatorkey.TrimRight();

   int res = loop_command(portAttributeDisplay_lst, SIZ_PORTATTRIBUTEDISPLAY_LST);

   designatorkey = tmpDesignator;
   G.cur_pos_x = tmpX;
   G.cur_pos_y = tmpY;
   G.cur_rotation = tmpRot;
   G.cur_mirror = tmpMir;

   return res;
}

/******************************************************************************
* portNameDisplay
*  - set the portRef as visible attribute REFNAME  to the insert
*  - also set the portRef as visible attribute PORT_portRef to the geometry
*/
static int portNameDisplay()
{
   propkey = ATT_REFNAME;

   int res = loop_command(propertyDisplayOverride_lst, SIZ_PROPERTYDISPLAYOVERRIDE_LST);
   
   // Also add the attribute to the geometry
   if (curblock)
   {
      CString key = "";
      key.Format("PORT_%s", cur_name);
      doc->SetUnknownVisAttrib(&curblock->getAttributesRef(), key, cur_name,  G.cur_pos_x, G.cur_pos_y, 
            DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
            G.cur_visible, SA_OVERWRITE, attflg, G.cur_layerindex, 1, 0, 0);
   }

   return 0;
}

/******************************************************************************
* instancePortDesignatorDisplay
*/
static int portDesignatorDisplay300()
{
   propkey = designatorkey;
   cur_name = "";

   if (curdata)
   {
      Attrib *a =  is_attvalue(doc, curdata->getAttributesRef(), propkey, 0);
      if (a)
         cur_name = get_attvalue_string(doc, a);
   }

   int res = loop_command(propertyDisplayOverride_lst, SIZ_PROPERTYDISPLAYOVERRIDE_LST);
   return res;

}

/****************************************************************************/
/*
*/
static int library400_cell()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   res = loop_command(cell400_lst,SIZ_CELL400_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int library300_cell()
{
   int   res;

   edif_namedef();
   cellname = cur_name;

   res = loop_command(cell400_lst,SIZ_CELL400_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int library400_header()
{
   int   res;

   res = loop_command(libraryheader400_lst,SIZ_LIBRARYHEADER400_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int library300_header()
{
   int   res;

   res = loop_command(libraryheader300_lst,SIZ_LIBRARYHEADER300_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int technology400()
{
   int   res;

   res = loop_command(technology400_lst,SIZ_TECHNOLOGY400_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int technology300()
{
   int   res;

   res = loop_command(technology300_lst,SIZ_TECHNOLOGY300_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int library200_cell()
{
   int   res;

   edif_namedef();
   if (ALLOW_CELLRENAME)
      cellname = cur_name; // this is after rename
   else
      cellname = orig_rename; 

   // need to do in view, because a cell can have multiple views
   //BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   res = loop_command(cell200_lst,SIZ_CELL200_LST);
   //Graph_Block_Off();
   cellname = "";

   return res;
}

/****************************************************************************/
/*
*/
static int library200_technology()
{
   int res;
   
   res = loop_command(technology200_lst,SIZ_TECHNOLOGY200_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int edif_numberdefinition()
{
   int   res;
   
   res = loop_command(numberdefinition_lst,SIZ_NUMBERDEFINITION_LST);

// here do the scaling  

   return res;
}

/****************************************************************************/
/*
*/
static int edif_figuregroup()
{
   int   res;

   Figuregrp   *f = new Figuregrp;

   edif_namedef();
   f->name = cur_name;

   G.cur_height = 0;
   G.cur_widthindex = 0;

   res = loop_command(figuregr_lst,SIZ_FIGUREGR_LST);
   f->textheight = G.cur_height;
   f->pathwidthindex = G.cur_widthindex;

   figuregrarray.SetAtGrow(figuregrcnt,f);
   figuregrcnt++;

   return res;
}

/****************************************************************************/
/*
*/
static int unit400()
{
   double   n1, n2;
   int      res = 1;
   CString  unitname;

   G.cur_unit = U_UNKNOWN;

   if (!get_tok())   return p_error();  // EDIF
   unitname = token;

   edif_numbervalue(&n1);
   edif_numbervalue(&n2);

   //fnull();
   fskip();
   //res = loop_command(scale_lst,SIZ_SCALE_LST);

   if (edifunitcnt < MAX_EDIFUNITS)
   {
      edifunit[edifunitcnt].name = unitname;
      edifunit[edifunitcnt].n1 = n1;
      edifunit[edifunitcnt].n2 = n2;
      edifunitcnt++;
   }
   else
   {
      fprintf(ferr,"Too many unit defintitions at %ld\n",ifp_line);
      display_error++;
   }

   return res;
}

/******************************************************************************
* edif_scalex
*/
static int edif_scalex()
{
   double n1, n2;

   edif_numbervalue(&n1);
   edif_numbervalue(&n2);

   // it seems in EDIF V3 that a scaleX -1 1 mirrors a instance.
   if (n1 < 0)
      G.cur_mirror = TRUE;

   return 1;
}

/******************************************************************************
* edif_scaley()
*/
static int edif_scaley()
{
   double n1, n2;

   edif_numbervalue(&n1);
   edif_numbervalue(&n2);

   if (n1 < 0)
   {
      G.cur_mirror = TRUE;
      G.cur_rotation += 180;
   }

   return 1;
}

/******************************************************************************
* edif_scale()
*/
static int edif_scale()
{
   double   n1, n2;
   double   s;
   int      res;

   G.cur_unit = U_UNKNOWN;

   edif_numbervalue(&n1);
   edif_numbervalue(&n2);

   res = loop_command(scale_lst, SIZ_SCALE_LST);

// here do the scaling  
   switch (G.cur_unit)
   {
      case U_DISTANCE:
         // SI units are in METER 
         faktor = Units_Factor(UNIT_MM, PageUnits);   // METER to page units
         faktor = faktor * (n2 / n1) * 1000;
         s = faktor;
      break;
      default:
         // not implemented
      break;
   }
   return res;
}

/****************************************************************************/
/*
   mantissa
   exponent
*/
static int edif_e()
{
   int   i, n1, n2;
   double f = 1;

   if (!get_tok())   return p_error();  // EDIF
   n1 = atoi(token);
   if (!get_tok())   return p_error();  // EDIF
   n2 = atoi(token);

   if (n2 < 0)
   {
      for (i=0;i<abs(n2);i++)
         f = f / 10;
   }
   else
   {
      for (i=0;i<abs(n2);i++)
         f = f * 10;
   }

   G.cur_number = n1 * f;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_unit()
{
   CString  uname;

   if (!get_tok())   return p_error();  // EDIF
   uname = token;

   if (!STRCMPI(token,"distance"))
      G.cur_unit = U_DISTANCE;
   else
   {
      fprintf(ferr,"Unknown unit [%s] at %ld\n",token, ifp_line);
      G.cur_unit = U_UNKNOWN;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cluster()
{
   CString  clustername;

   if (!get_tok())   return p_error();  // EDIF
   clustername = token;

   int res = loop_command(cluster_lst,SIZ_CLUSTER_LST);

   return res;

}

/****************************************************************************/
/*
*/
static int cell200_celltype()
{
   if (!get_tok())   return p_error();  // cell type


   return 1;
}

/****************************************************************************/
/*
   I have seen in esiwrite, that the view and vireref rename outside of the 
   scope is 
*/
static int cell200_view()
{
   CString  viewname;
   CString  cell;
   long     l = ifp_line;

   edif_namedef();
   
   if (ALLOW_VIEWRENAME)
      viewname = cur_name; // this is after rename
   else
      viewname = orig_rename; 


   cell.Format("%s %s",cellname, viewname);
   cur_libname = cell;
   // need to do in view, because a cell can have multiple views
   curblock = Graph_Block_On(GBO_APPEND, cell, cur_libnum, 0L);// declares and begins defining a block
   curblock->setBlockType(BLOCKTYPE_SYMBOL);

   int res = loop_command(view200_lst,SIZ_VIEW200_LST);
   Graph_Block_Off();
   curblock = NULL;  

   return res;
}

/****************************************************************************/
/*
*/
static int view_contents()
{
   int res = loop_command(contents200_lst,SIZ_CONTENTS200_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int commentgraphics()
{
   int res = loop_command(commentgraphics_lst,SIZ_COMMENTGRAPHICS_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int net200()
{
   CString  old_lay = cur_layername;

   cur_layername = "NET";
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   edif_namedef();
   cur_netname = cur_name;
           
   if (strlen(cur_netname))
      NetStruct *n = add_net(file,cur_netname);
   int res = loop_command(net200_lst,SIZ_NET200_LST);
   cur_netname = "";

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   return res;
}

/****************************************************************************/
/*
*/
static int netbundle200()
{
   CString  old_lay = cur_layername;

   cur_layername = "NET";
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   edif_namedef();
   cur_netname = "";    // netbundle does not get a netname.
/*           
   if (strlen(cur_netname))
      NetStruct *n = add_net(file,cur_netname);
*/
   int res = loop_command(net200_lst,SIZ_NET200_LST);
   cur_netname = "";

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   return res;
}


/******************************************************************************
* instance200()
*/
static int instance200()
{
   CString old_lay = cur_layername;

   edif_nameref();
   instancename = cur_name;
   designatorkey = ATT_REFNAME;

   curdata = NULL;

   attflg = ATTRIB_TMP_FLAG;
   int res = loop_command(instance200_lst, SIZ_INSTANCE200_LST);
   attflg = 0;

   // here place, because the transform function is used by also other
   // properties etc... and I need the datapointer to add properties. 
   if (curdata)
   {
      // update inserttype
      BlockStruct *block = doc->Find_Block_by_Num(curdata->getInsert()->getBlockNumber());
      curdata->getInsert()->setInsertType(default_inserttype(block->getBlockType()));

      if (curdata->getAttributesRef() == NULL)
         return 1;
      
      for (POSITION pos = curdata->getAttributesRef()->GetStartPosition();pos != NULL;)
      {
         WORD keyword;
         Attrib* attrib;
         curdata->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

         if (attrib->getFlags() & ATTRIB_TMP_FLAG)
         {
            // normalize data
            double x = attrib->getX() - curdata->getInsert()->getOriginX();
            double y = attrib->getY() - curdata->getInsert()->getOriginY();
            double xrot;
            double yrot;

            // do not adjust for mirror !!!
            Rotate(x , y , 360-RadToDeg(curdata->getInsert()->getAngle()), &xrot, &yrot);
            attrib->incRotationRadians(- curdata->getInsert()->getAngle());
            attrib->setCoordinate(xrot,yrot);
            attrib->clearFlagBits(ATTRIB_TMP_FLAG);
         }
      }
   }

   curdata = NULL;
   cur_layername = old_lay;

   return res;
}

/******************************************************************************
* logicalConnectivity_instance300()
*/
static int logicalConnectivity_instance300()
{
   edif_nameref();
   instancename = cur_name;

   librefname = "";
   cellrefname = "";
   cur_name = "";
   curblock = NULL;
   // this is only used to hold the attributes
   curdata = Graph_Block_Reference("attribholder", "", 0, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   int res = loop_command(logicalinstance300_lst, SIZ_LOGICALINSTANCE300_LST);
   attflg = 0;

   //this will be placed in schematicInstanceImplementation
   EDIFLogicalInstance *g = new EDIFLogicalInstance;
   logicalinstancearray.SetAtGrow(logicalinstancecnt,g);
   logicalinstancecnt++;
   g->data = curdata;
   g->instance = instancename;
   g->cellref = cellrefname;
   g->libraryref = librefname;
   g->nameinformation = refName;
   g->datalist = GetCurrentDataList();

   return res;
}

/******************************************************************************
* instancePortAttributes
*/
static int instancePortAttributes()
{
   edif_nameref();
   designatorkey.Format("PORTDES %s", cur_name);
   designatorkey.TrimLeft();
   designatorkey.TrimRight();

   int res = loop_command(instancePortAttributes_lst, SIZ_INSTANCEPORTATTRIBUTES_LST);
   return res;
}

/******************************************************************************
* portDesignator300
*/
static int portDesignator300()
{
   edif_string();
   CString val = cur_text;

   CString cl = cur_layername;
   cl += "_DESIGNATOR";
   G.cur_layerindex = Graph_Level(cl,"", 0);

   if (curdata)
      doc->SetUnknownAttrib(&curdata->getAttributesRef(), designatorkey, val, SA_OVERWRITE, NULL);

   return 0;
}

/******************************************************************************
* view_viewtype
*/
static int view_viewtype()
{
   int res = 0;
   if (!get_tok())   return p_error();  // netlist, 

   if (!STRCMPI(token, "Netlist"))
   {
      CString  tmp;

      tmp = "EDIF File format Error:\n\n";
      tmp += "This EDIF file is a NETLIST viewtype only and does not contain any graphic!";
      tmp += "\nUse OMNINET for netlist translation!";

      ErrorMessage(tmp, "EDIF Reader");

      fprintf(ferr, tmp);
      display_error++;

      return -1;
   }

   return res;
}

/******************************************************************************
* view_interface
*/
static int view_interface()
{
   int res = loop_command(interface200_lst,SIZ_INTERFACE200_LST);

   return res;
}

/******************************************************************************
* interface_symbol
*/
static int interface_symbol()
{

   cur_layername = "SYMBOL";
   G.cur_layerindex = Graph_Level(cur_layername,"", 0);

   int res = loop_command(symbol200_lst,SIZ_SYMBOL200_LST);

   return res;
}

/******************************************************************************
* interface_port
*/
static int interface_port()
{
   edif_namedef();

   // can have a property 
   if (!get_tok())   return p_error();  
   if (token[0] == '(' && !token_name)
   {
      push_tok();
#ifdef X_DEBUG
      fprintf(ferr, "Additional Info in Port at %ld\n", ifp_line);
      display_error++;
#endif
      fskip(); // owner, unit, property, comment
   }
   else
      push_tok();

   return 1;
}

/******************************************************************************
* edif_property
*/
static int edif_property()
{
   CString old_lay = cur_layername;
   cur_layername += "_PROPERTY";
   G.cur_textjust = 0;
   G.cur_visible = FALSE;
   G.cur_placed = FALSE;
   G.cur_attribute = TRUE;

   edif_namedef();
   CString prop_key = get_attribmap(get_rename(cur_name, loop_depth));
   prop_key.MakeUpper();
   if (prop_key == "REF")
   {
      int i = 0;
   }


   int prop_visible = G.cur_visible;   // if the property is visible FALSE, no display

   edif_typedvalue();
   CString prop_val = "";

   if (G.cur_typeval == TYPEVAL_STRING)
      prop_val = cur_text;
               
   if (!get_tok())
      return p_error();  

   if (token[0] == '(' && !token_name)
   {
      push_tok();
#ifdef _DEBUG
      fprintf(ferr, "DEBUG: Additional Info in Property at %ld\n", ifp_line);
      display_error++;
#endif
      fskip(); // owner, unit, property, comment
      //go_command(edif_stringdisplay_lst,SIZ_EDIF_STRINGDISPLAY_LST);
   }
   else
      push_tok();

   // normalize
   if (G.cur_mirror)
   {
      double tx = 0, ty = 0;
      Rotate(G.cur_height * TEXTRATIO * strlen(prop_val), 0.0, G.cur_rotation, &tx, &ty);
      G.cur_pos_x = G.cur_pos_x - tx;
      G.cur_pos_y = G.cur_pos_y - ty;
   }

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   if (curdata)
   {
      doc->SetUnknownVisAttrib(&curdata->getAttributesRef(), prop_key, prop_val, G.cur_pos_x, G.cur_pos_y, 
            DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
            G.cur_visible, SA_OVERWRITE, attflg, G.cur_layerindex, 1, 0, 0);
   }
   else if (curblock)
   {
      // here add to block.
      doc->SetUnknownVisAttrib(&curblock->getAttributesRef(), prop_key, prop_val, G.cur_pos_x, G.cur_pos_y, 
            DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
            G.cur_visible, SA_OVERWRITE, 0l, G.cur_layerindex, 1, 0, 0);
   }

   G.cur_typeval = TYPEVAL_UNKNOWN;
   G.cur_attribute = FALSE;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_portproperty()
{
   CString prop_key, prop_val;

   G.cur_textjust = 0;
   G.cur_visible = FALSE;
   G.cur_placed = FALSE;
   cur_layername = "PORTPROPERTY";
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   edif_namedef();
   prop_key.Format("%s_%s", designatorkey, cur_name);

   edif_typedvalue();
   prop_val = "";

   if (G.cur_typeval == TYPEVAL_STRING)
      prop_val = cur_text;
               
   if (!get_tok())
      return p_error(); 
   
   if (token[0] == '(' && !token_name)
   {
      push_tok();
      fskip(); //  
   }
   else
      push_tok();

   write_attributes(prop_key, prop_val);

   G.cur_layerindex = Graph_Level(cur_layername,"", 0);
   G.cur_typeval = TYPEVAL_UNKNOWN;

   return 1;
}

/******************************************************************************
* edif_designator
*/
static int edif_designator()
{
   CString  prop_key, prop_val;
   CString  old_lay = cur_layername;

   G.cur_textjust =  0;
   G.cur_visible = FALSE;
   G.cur_placed = FALSE;
   G.cur_attribute = TRUE; // do not write in string display

   edif_string();
   prop_val  = cur_text;
   prop_key = "DESIGNATOR";

   CString  cl = cur_layername;

   cl += "_DESIGNATOR";
   G.cur_layerindex = Graph_Level(cl,"", 0);

   write_attributes(prop_key, prop_val);

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername,"", 0);
   G.cur_typeval = TYPEVAL_UNKNOWN;
   G.cur_attribute = FALSE;   // write in string display

   return 1;
}

/******************************************************************************
* edif_portdesignator
*/
static int edif_portdesignator()
{
   CString  prop_key, prop_val;
   CString  old_lay = cur_layername;

   G.cur_textjust =  0;
   G.cur_visible = FALSE;
   G.cur_placed = FALSE;
   G.cur_attribute = TRUE; // do not write in string display.

   edif_string();
   prop_val  = cur_text;
   prop_key.Format("PORTDES %s", cur_text);

   CString  cl = cur_layername;

   cl += "_DESIGNATOR";
   G.cur_layerindex = Graph_Level(cl,"", 0);

   write_attributes(prop_key, prop_val);

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername,"", 0);
   G.cur_typeval = TYPEVAL_UNKNOWN;
   G.cur_attribute = FALSE; // do not write in string display.

   return 1;
}

/******************************************************************************
* edif_portinstance
*/
static int edif_portinstance()
{
   edif_nameref();
   designatorkey.Format("PORT_%s", cur_name);

   cur_layername = "PORTINSTANCE";
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   int res = loop_command(portinstance200_lst, SIZ_PORTINSTANCE200_LST);

   return 1;
}

/******************************************************************************
* schematicSymbol
*/
static int schematicSymbol()
{
   if (!get_tok())
      return p_error();  // EDIF
   CString symbolname = token;

   // in edif300 a cluster can have multiple schematic symbol graphics.
   CString cell_name;
   cell_name.Format("%s %s", cellname, symbolname);

   curblock = Graph_Block_On(GBO_APPEND, cell_name, cur_libnum, 0L);// declares and begins defining a block
   curblock->setBlockType(BLOCKTYPE_SYMBOL);
   int res = loop_command(schematicsymbol_lst, SIZ_SCHEMATICSYMBOL_LST);
   Graph_Block_Off();
   curblock = NULL;

   return res;
}

/****************************************************************************/
/*
*/
static int pcbMcmView()
{
   CString  pcbmcmname;

   if (!get_tok())   return p_error();  // EDIF
   pcbmcmname = token;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block

   int res = loop_command(pcbMcmView_lst,SIZ_PCBMCMVIEW_LST);
   Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbMcmImplementation()
{
   // here make a file from the last cell

   int res = loop_command(pcbMcmImplementation_lst,SIZ_PCBMCMIMPLEMENTATION_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicView()
{
   CString schematicname;

   if (!get_tok())
      return p_error();
   schematicname = token;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block

   int res = loop_command(schematicview_lst,SIZ_SCHEMATICVIEW_LST);
   Graph_Block_Off();

   return res;
}

/****************************************************************************/
/*
*/
static int pcbAssembledBoard()
{
   return loop_command(pcbAssembledBoard_lst,SIZ_PCBASSEMBLEDBOARD_LST);
}

/****************************************************************************/
/*
*/
static int pcbBareBoard()
{
   pcbbareboarddefinitionref = "";
   int res = loop_command(pcbBareBoard_lst,SIZ_PCBBAREBOARD_LST);
   
   // here copy bareboard into current design, because if there is no assembly view,
   // the design will show up.
   BlockStruct *b = Graph_Block_Exists(doc, pcbbareboarddefinitionref, -1);
   Graph_Block_Copy(b, 0, 0, 0, 0, 1, -1, FALSE);

   return res;
}

/****************************************************************************/
/*
*/
static int pcbDrawingAnnotation()
{
   if (!get_tok())   return p_error();  // name

   int res = loop_command(pcbDrawingAnnotation_lst,SIZ_PCBDRAWINGANNOTATION_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalLayerHeader()
{
   cur_name = "";
   int res = loop_command(pcbPhysicalLayerHeader_lst,SIZ_PCBPHYSICALLAYERHEADER_LST);
   
   if (strlen(cur_name))
      layerlist[G.cur_layerlistptr].username = cur_name;

   return res;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalNetHeader()
{
   cur_name = "";
   int res = loop_command(pcbPhysicalNetHeader_lst,SIZ_PCBPHYSICALNETHEADER_LST);

   if (strlen(cur_name))
      cur_netname = cur_name;

   return res;
}

/****************************************************************************/
/*
*/
static int Stackup_pcbPhysicalLayerRef()
{
   CString  lname;

   if (!get_tok())   return p_error();  // name
   lname = token;
   G.layer_stackup++;

   G.cur_layerindex = Graph_Level(lname,"",0);
   LayerStruct *lay = doc->FindLayer(G.cur_layerindex);

   lay->setArtworkStackNumber(G.layer_stackup);

   G.cur_layerlistptr = get_layerptr(lname);
   layerlist[G.cur_layerlistptr].artworkstacknumber = G.layer_stackup;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcbConductingLayer()
{
   layerlist[G.cur_layerlistptr].conductive = TRUE;
   return 1;
}

/****************************************************************************/
/*
*/
static int pcbNonConductingLayer()
{
   int res = loop_command(pcbNonConductingLayer_lst,SIZ_PCBNONCONDUCTINGLAYER_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int pcbNonConductingLayerSilkscreen()
{
   layerlist[G.cur_layerlistptr].nonconductivetype = EDIF_SILKSCREEN;
   return 1;
}

/****************************************************************************/
/*
*/
static int pcbNonConductingLayerSolderMask()
{
   layerlist[G.cur_layerlistptr].nonconductivetype = EDIF_SOLDERMASK;
   return 1;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalLayer()
{
   CString  lname;

   if (!get_tok())   return p_error();  // name
   lname = token;

   G.cur_layerindex = Graph_Level(lname,"",0);

   G.cur_layerlistptr = get_layerptr(lname);

   int res = loop_command(pcbPhysicalLayer_lst,SIZ_PCBPHYSICALLAYER_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pcbdrawingfigure()
{
   int res = loop_command(pcbDrawingFigure_lst,SIZ_PCBDRAWINGFIGURE_LST);
   return res;
}


/****************************************************************************/
/*
*/
static int pcbdrawingtextdisplay()
{
   int res = loop_command(pcbDrawingTextDisplay_lst,SIZ_PCBDRAWINGTEXTDISPLAY_LST);
   return res;
}

/******************************************************************************
* nameInformation()
*/
static int nameInformation()
{
   cur_name = "";
   int res = loop_command(nameInformation_lst, SIZ_NAMEINFORMATION_LST);

   return res;
}

/******************************************************************************
* primaryName()
*/
static int primaryName()
{
   //if (!get_tok())   return p_error();  // name
   edif_name();

   refName = cur_name;

   return 1;
}

/****************************************************************************/
/*
*/
static int pcbdrawingtext()
{
   CString  prosa;

   if (!get_tok())   return p_error();  // name
   prosa = token;

   int res = loop_command(pcbDrawingText_lst,SIZ_PCBDRAWINGTEXT_LST);

   // here do text
   char  *lp;

   lp = strtok(prosa.GetBuffer(0),"\n");

   while (lp)
   {
      Graph_Text(G.cur_layerindex, lp, G.cur_pos_x, G.cur_pos_y,
         G.cur_height, G.cur_height*TEXTRATIO, DegToRad(G.cur_rotation), 0,
         text_prop, G.cur_mirror, 0, 0,      
         0, 0);
      lp = strtok(NULL,"\n");
      G.cur_pos_y -= G.cur_height;
   }

   return res;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalText()
{
   CString  prosa;

   if (!get_tok())   return p_error();  // name
   prosa = token;

   int res = loop_command(pcbPhysicalText_lst,SIZ_PCBPHYSICALTEXT_LST);

   // here do text
   char  *lp;

   lp = strtok(prosa.GetBuffer(0),"\n");

   if (G.cur_height == 0)
   {
      fprintf(ferr,"text height wrong for [%s] at %ld\n",lp, ifp_line);
      G.cur_height = 0.1;
   }

   while (lp)
   {
      Graph_Text(G.cur_layerindex, lp, G.cur_pos_x, G.cur_pos_y,
         G.cur_height, G.cur_height*TEXTRATIO, DegToRad(G.cur_rotation), 0,
         text_prop, G.cur_mirror, 0, 0,      
         0, 0);
      lp = strtok(NULL,"\n");
      G.cur_pos_y -= G.cur_height;
   }

   return res;
}

/****************************************************************************/
/*
*/
static int pcbDrawing()
{
   if (!get_tok())   return p_error();  // name

   int res = loop_command(pcbDrawing_lst,SIZ_PCBDRAWING_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbSimpleFabricateFigureCollectorRef()
{  
   if (!get_tok())   return p_error();  // name
   G.cur_layerindex = Graph_Level(token,"",0);

   return 1;
}


/****************************************************************************/
/*
*/
static int library_pcbfootprint()
{
   if (!get_tok())   return p_error();  // EDIF
   cellname = token;

   G.cur_status = STAT_FOOTPRINT;

   curblock = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   curblock->setBlockType(BLOCKTYPE_PCBCOMPONENT);
   int res = loop_command(footprint_lst,SIZ_FOOTPRINT_LST);
   Graph_Block_Off();
   
   curblock = NULL;
   G.cur_status = STAT_UNKNOWN;

   return res;
}


/****************************************************************************/
/*
*/
static int library_pcbpadstackdefintion()
{
   if (!get_tok())   return p_error();  // EDIF
   cellname = token;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   int res = loop_command(padstack_lst,SIZ_PADSTACK_LST);
   Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int library_pcbtoeprintdefintion()
{
   if (!get_tok())   return p_error();  // EDIF
   cellname = token;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   c->setBlockType(BLOCKTYPE_PADSTACK);

   int res = loop_command(toeprint_lst,SIZ_TOEPRINT_LST);
   Graph_Block_Off();
   
   return res;
}

/******************************************************************************
* pcbSubLayoutInstancePhysicalNetRef
*/
static int pcbSubLayoutInstancePhysicalNetRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   cur_portname = token;

   int res = loop_command(pcbSubLayoutInstancePhysicalNetRef_lst, PCBSUBLAYOUTINSTANCEPHYSICALNETREF_LST);

   return res;
}

/******************************************************************************
* pcbNonSpecificLayoutShape
*/
static int pcbNonSpecificLayoutShape()
{
   return loop_command(pcbNonSpecificLayoutShape_lst, SIZ_PCBNONSPECIFICLAYOUTSHAPE_LST);
}

/******************************************************************************
* pcbFabricateTextSet
*/
static int pcbFabricateTextSet()
{
   return loop_command(pcbFabricateTextSet_lst, SIZ_PCBFABRICATETEXTSET_LST);
}

/******************************************************************************
* library_pcbphysicalfiguremacro
*/
static int library_pcbphysicalfiguremacro()
{
   if (!get_tok())
      return p_error();  // EDIF
   CString figuremacro = token;

   Graph_Block_On(GBO_APPEND, figuremacro, cur_libnum, 0L);// declares and begins defining a block
   int res = loop_command(pcbPhysicalFigureMacro_lst, SIZ_PCBPHYSICALFIGUREMACRO_LST);
   Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int library_pcbbareboarddefinition()
{
   CString  bareboardname;

   if (!get_tok())   return p_error();  // EDIF
   bareboardname = token;

   // current library becomes PCB design.
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());

   file->setRefName(bareboardname);

   // I need to make bareboard a geometry, because layer I need this
   // again for an assembly view.
   curblock = Graph_Block_On(GBO_APPEND, bareboardname, cur_libnum, 0L);// declares and begins defining a block
   curblock->setBlockType(blockTypePcb);
   int res = loop_command(pcbbareboarddefinition_lst,SIZ_PCBBAREBOARDDEFINITION_LST);
   Graph_Block_Off();
   
   // here copy bareboard into current design, because if there is no assembly view,
   // the design will show up.
   Graph_Block_Copy(curblock, 0, 0, 0, 0, 1, -1, FALSE);

   return res;
}

/******************************************************************************
* library_pcbbareboardtechnology
*/
static int library_pcbbareboardtechnology()
{
   if (!get_tok())
      return p_error();  // EDIF
   CString baretechname = token;

   int res = loop_command(pcbbareboardtechnology_lst, SIZ_PCBBAREBOARDTECHNOLOGY_LST);
   
   return res;
}

/******************************************************************************
* pcbBareBoardOutline
*/
static int pcbBareBoardOutline()
{
   G.cur_boardoutline = TRUE;
   int res = loop_command(pcbBareBoardOutline_lst, SIZ_PCBBAREBOARDOUTLINE_LST);
   G.cur_boardoutline = FALSE;   
   return res;
}

/******************************************************************************
* find_pin_from_netpin
*/
static int find_pin_from_netpin(int shapeindex, int netpinindex)
{
   for (int i=0; i<namecnt; i++)
   {
      EDIFShapePinNet p = shapepinnet.GetAt(i);
      if (p.shapeindex == shapeindex && p.netpinindex == netpinindex)
         return p.pinindex;
   }

   return -1;
}

/******************************************************************************
* pcbSubLayoutInstanceRef
*/
static int pcbSubLayoutInstanceRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   CString name = token;

   if (G.cur_status == STAT_FOOTPRINT)
   {
      int shapeindex = get_nameindex(curblock->getName());
      int pinindex = get_nameindex(name);
      int netpinindex = get_nameindex(pcbphysicalnetname);

      // here add to the shape_pin_net list
      EDIFShapePinNet p;
      p.shapeindex = shapeindex;
      p.pinindex = pinindex;
      p.netpinindex = netpinindex;
      shapepinnet.SetAtGrow(shapepinnetcnt++, p);  
      return 1;
   }

   // if this is a via, find it and assign cur_netname
   DataStruct *data = datastruct_from_refdes(doc, curblock, name);
   if (data)
   {
      if (data->getInsert()->getInsertType() == INSERTTYPE_VIA)
      {
         if (strlen(cur_netname))
            doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
      }
      else if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
      {
         // here make a netlist
         // cur_portname = pinnetname
         // cur_netname = netname
         // name = instancename d->getInsert()->getBlockNumber() is block which gets the shapename
         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         int pinindex = find_pin_from_netpin(get_nameindex(block->getName()), get_nameindex(cur_portname));
         
         if (pinindex > -1)
         {
            CString pname = namearray[pinindex]->name;
            if (strlen(cur_netname))
            {
               NetStruct *net = add_net(file,cur_netname);
               add_comppin(file, net, name, pname);
            }
         }
         else
         {
            fprintf(ferr, "Netlist [%s] [%s] pinindex error at %ld\n", cur_netname, name, ifp_line);
            display_error++;
         }
      }
   }
   else
   {
      fprintf(ferr, "Could not find record [%s]\n", name);
      display_error++;
   }

   return 1;
}

/******************************************************************************
* pcbLayoutFeature
*/
static int pcbLayoutFeature()
{
   if (!get_tok())
      return p_error();  // EDIF
   CString name = token;

   int res = loop_command(pcbLayoutFeature_lst, SIZ_PCBLAYOUTFEATURE_LST);
   
   return res;
}

/******************************************************************************
* pcbInstanceNets
*/
static int pcbInstanceNets()
{
   return loop_command(pcbInstanceNets_lst, SIZ_PCBINSTANCENETS_LST);
}

/******************************************************************************
* pcbLayoutText
*/
static int pcbLayoutText()
{
   if (!get_tok())
      return p_error();  // EDIF
   CString name = token;

   int res = loop_command(pcbLayoutText_lst, SIZ_PCBLAYOUTTEXT_LST);
   
   return res;
}

/******************************************************************************
* pcbStackedLayerGroup
*/
static int pcbStackedLayerGroup()
{
   G.layer_stackup = 0;
   int res = loop_command(pcbStackedLayerGroup_lst, SIZ_PCBSTACKEDLAYERGROUP_LST);
   
   return res;
}

/******************************************************************************
* pcbBareBoardLayers
*/
static int pcbBareBoardLayers()
{
   G.layer_stackup = 0;
   int res = loop_command(pcbBareBoardLayers_lst, SIZ_PCBBAREBOARDLAYERS_LST);
   
   return res;
}

/******************************************************************************
* pcbBareBoardPhysicalNet
*/
static int pcbBareBoardPhysicalNet()
{
   if (!get_tok())
      return p_error();
   cur_netname = token; // this netname can be changed with netnameheader
 
   int res = loop_command(pcbBareBoardPhysicalNet_lst, SIZ_PCBBAREBOARDPHYSICALNET_LST);
   cur_netname = "";

   return res;
}

/******************************************************************************
* pcbFootprintInstance
*/
static int pcbFootprintInstance()
{
   if (!get_tok())
      return p_error();
   pcbfootprintinstance = token;
 
   pcbfootprintdefinitionref = "";
   int res = loop_command(pcbFootprintInstance_lst, SIZ_PCBFOOTPRINTINSTANCE_LST);

   if (strlen(pcbfootprintdefinitionref))
   {
      DataStruct *data = Graph_Block_Reference(pcbfootprintdefinitionref, pcbfootprintinstance, 
            0, G.cur_pos_x, G.cur_pos_y, DegToRad(G.cur_rotation), G.cur_mirror, 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypePcbComponent);
   }

   return res;
}

/******************************************************************************
* pcbPadstackInstance
*/
static int pcbPadstackInstance()
{
   if (!get_tok())
      return p_error();
   CString pcbpadstackinstance = token;
 
   pcbpadstackdefinitionref = "";
   viaplace = FALSE;
   int res = loop_command(pcbPadStackInstance_lst, SIZ_PCBPADSTACKINSTANCE_LST);

   if (strlen(pcbpadstackdefinitionref))
   {
      DataStruct *data = Graph_Block_Reference(pcbpadstackdefinitionref, pcbpadstackinstance, 0, 
            G.cur_pos_x, G.cur_pos_y, DegToRad(G.cur_rotation), G.cur_mirror, 1.0, 0, TRUE);
      if (viaplace)
         data->getInsert()->setInsertType(insertTypeVia);
   }
   viaplace = FALSE;

   return res;
}

/******************************************************************************
* pcbToeprintInstance
*/
static int pcbToeprintInstance()
{
   if (!get_tok())
      return p_error();
   CString pcbtoeprintinstance = token;
 
   toeprintdefinitionref = "";
   int res = loop_command(pcbToeprintInstance_lst, SIZ_PCBTOEPRINTINSTANCE_LST);

   if (strlen(toeprintdefinitionref))
   {
      DataStruct *data = Graph_Block_Reference(toeprintdefinitionref, pcbtoeprintinstance, 0, 
            G.cur_pos_x, G.cur_pos_y, DegToRad(G.cur_rotation), G.cur_mirror, 1.0, 0, TRUE);
      data->getInsert()->setInsertType(insertTypePin);
   }

   return res;
}

/******************************************************************************
* pcbPhysicalFigureMacroRef
*/
static int pcbPhysicalFigureMacroRef()
{
   if (!get_tok())
      return p_error(); 
   pcbphysicalfiguremacroref = token;
 
   return 1;
}

/******************************************************************************
* schematicFigureMacroRef
*/
static int schematicFigureMacroRef()
{
   if (!get_tok())
      return p_error();
   schematicfiguremacroref = token;
 
   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   return 1;
}

/******************************************************************************
* schematicSymbolPortTemplateRef
*/
static int schematicSymbolPortTemplateRef()
{
   if (!get_tok())
      return p_error(); 
   schematicsymbolportref = token;
 
   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   return 1;
}

/******************************************************************************
* clusterref
*/
static int clusterref()
{
   if (!get_tok())
      return p_error();
   CString clusterref = token;
 
   int res = loop_command(clusterref_lst, SIZ_CLUSTERREF_LST);

   return 1;
}

/******************************************************************************
* schematicOffPageConnectorTemplateRef
*/
static int schematicOffPageConnectorTemplateRef()
{
   if (!get_tok())
      return p_error(); 
   schematicsymbolref = token;

   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   if (strlen(schematicsymbolref))
      curdata = Graph_Block_Reference(schematicsymbolref, instancename, 0, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   return 1;
}

/******************************************************************************
* schematicMasterPortTemplateRef
*/
static int schematicMasterPortTemplateRef()
{
   if (!get_tok())
      return p_error();
   schematicsymbolref = token;

   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   if (strlen(schematicsymbolref))
      curdata = Graph_Block_Reference(schematicsymbolref, instancename, 0, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   return 1;
}

/******************************************************************************
* schematicGlobalPortTemplateRef
*/
static int schematicGlobalPortTemplateRef()
{
   if (!get_tok())
      return p_error();
   schematicsymbolref = token;

   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   if (strlen(schematicsymbolref))
      curdata = Graph_Block_Reference(schematicsymbolref, instancename, 0, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   return 1;
}

/******************************************************************************
* schematicOnPageConnectorTemplateRef
*/
static int schematicOnPageConnectorTemplateRef()
{
   if (!get_tok())
      return p_error(); 
   schematicsymbolref = token;

   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   if (strlen(schematicsymbolref))
      curdata = Graph_Block_Reference(schematicsymbolref, instancename, 0, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   return 1;
}

/******************************************************************************
* pageTitleBlockTemplateRef
*/
static int pageTitleBlockTemplateRef()
{
   if (!get_tok())
      return p_error();
   schematicsymbolref = token;

   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   if (strlen(schematicsymbolref))
      curdata = Graph_Block_Reference(schematicsymbolref, "",  0, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   return 1;
}

/******************************************************************************
* pageBorderTemplateRef
*/
static int pageBorderTemplateRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   CString borderref = token;

   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   if (strlen(borderref))
      curdata = Graph_Block_Reference(borderref, "",  0, 0.0, 0.0, 0.0, 0 , 1.0, -1, TRUE);

   return 1;
}

/******************************************************************************
* schematicSymbolRef
*/
static int schematicSymbolRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   schematicsymbolref = token;
 
   CString cell_schematicsymbolref = "";
   int logicalinstanceptr = get_logicalinstance(instancename);

   if (logicalinstanceptr > -1)
   {
      EDIFLogicalInstance *logicalInstance = logicalinstancearray[logicalinstanceptr];
      cell_schematicsymbolref.Format("%s %s", logicalInstance->cellref, schematicsymbolref);
   }
   else
   {
      fprintf(ferr,"Instance [%s] not found in Logical Instance list at %ld\n", instancename, ifp_line);
      display_error++;
   }

   if (strlen(cell_schematicsymbolref))
   {
      curdata = Graph_Block_Reference(cell_schematicsymbolref, instancename, 0, 0.0, 0.0, 0.0, 0, 1.0, -1, TRUE);
      BlockStruct *block = doc->Find_Block_by_Num(curdata->getInsert()->getBlockNumber());
      curdata->getInsert()->setInsertType(default_inserttype(block->getBlockType()));

      insertDataList.AddTail(curdata);

      if (logicalinstanceptr > -1)
      {
         EDIFLogicalInstance *logicalInstance = logicalinstancearray[logicalinstanceptr]; 
         doc->CopyAttribs(&curdata->getAttributesRef(), logicalInstance->data->getAttributesRef());  // copy attribute from Place holder data to curdata
          
         if (strlen(logicalInstance->nameinformation))  // nameinformation
         {
            Attrib *attrib = is_attvalue(doc, curdata->getAttributesRef(), ATT_REFNAME, 0);
            if (attrib)
            {
               attrib->setValueType(VT_STRING);
               attrib->setStringValueIndex(doc->RegisterValue(logicalInstance->nameinformation));
            }  
            else
            {
               doc->SetUnknownAttrib(&curdata->getAttributesRef(), ATT_REFNAME, logicalInstance->nameinformation, SA_APPEND, NULL);
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* instanceRef
*/
static int instanceRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   instancename = token;
 
   return 1;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalTextSet()
{
   int res = loop_command(pcbPhysicalTextSet_lst,SIZ_PCBPHYSICALTEXTSET_LST);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalTextImplementation()
{
   int res = loop_command(pcbPhysicalTextImplementation_lst,SIZ_PCBPHYSICALTEXTIMPLEMENTATION_LST);

   return 1;
}

/****************************************************************************/
/*
*/
static int pcbTextAppearance()
{
   int res = loop_command(pcbTextAppearance_lst,SIZ_PCBPHYSICALTEXTAPPEARANCE_LST);

   return 1;
}

/******************************************************************************
* pcbPadstackDefinitionRef
*/
static int pcbPadstackDefinitionRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   pcbpadstackdefinitionref = token;
 
   // library ref
   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);
   
   return 1;
}

/******************************************************************************
* pcbPadstackInstanceRoutingViaPlace
*/
static int pcbPadstackInstanceRoutingViaPlace()
{
   viaplace = TRUE;
   return 1;
}

/******************************************************************************
* pcbToeprintDefinitionRef
*/
static int pcbToeprintDefinitionRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   toeprintdefinitionref = token;
 
   return 1;
}

/******************************************************************************
* pcbFootprintDefinitionRef
*/
static int pcbFootprintDefinitionRef()
{
   if (!get_tok())
      return p_error();  // EDIF
   pcbfootprintdefinitionref = token;
 
   // library ref
   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);

   return 1;
}

/******************************************************************************
* pcbFabricateFigure
*/
static int pcbFabricateFigure()
{
   int res = loop_command(pcbFabricateFigure_lst, SIZ_PCBFABRICATEFIGURE_LST);
   
   return res;
}

/******************************************************************************
* pcbFabricateText
*/
static int pcbFabricateText()
{

   int res = loop_command(pcbFabricateText_lst, SIZ_PCBFABRICATETEXT_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pcbPlane()
{
   CString  name;

   if (!get_tok())      return p_error();  // EDIF
   name = token;

   //BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   int res = loop_command(pcbPlane_lst,SIZ_PCBPLANE_LST);
   //Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbTrace()
{
   //BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   int res = loop_command(pcbTrace_lst,SIZ_PCBTRACE_LST);
   //Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbPad()
{
   //BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, cur_libnum, 0L);// declares and begins defining a block
   int res = loop_command(pcbPad_lst,SIZ_PCBPAD_LST);
   //Graph_Block_Off();
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalNet()
{
   if (!get_tok())      return p_error();  // EDIF
   pcbphysicalnetname = token;

   int res = loop_command(pcbPhysicalNet_lst,SIZ_PCBPHYSICALNET_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int get_geometrygrp(const char *g)
{
   int   i;

   for (i=0;i<geometrygrcnt;i++)
   {
      if (geometrygrarray[i]->name.Compare(g) == 0)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pcbPhysicalFigure()
{
   CString  name;

   if (!get_tok())      return p_error();  // EDIF

   if (token[0] == '(' && !token_name)
   {
      name = "";
      push_tok();
   }
   else
   {
      name = token;

      int   g = get_geometrygrp(token);

      if (g > -1)
         G.cur_widthindex = geometrygrarray[g]->widthindex;
      else
      {
         G.cur_widthindex = 0;
         fprintf(ferr,"pcbFigureAttributeOverride [%s] not found\n",token);
         display_error++;
      }
   }

   int res = loop_command(pcbPhysicalFigure_lst,SIZ_PCBPHYSICALFIGURE_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbComplexPhysicalFigure()
{
   CString  name;

   if (!get_tok())      return p_error();  // EDIF

   if (token[0] == '(' && !token_name)
   {
      name = "";
      push_tok();
   }
   else
      name = token;

   pcbphysicalfiguremacroref = "";
   int res = loop_command(pcbComplexPhysicalFigure_lst,SIZ_PCBCOMPLEXPHYSICALFIGURE_LST);

   CString  lname;
   curlayer = doc->FindLayer(G.cur_layerindex);
   lname = curlayer->getName();

   //xxx here place
   if (strlen(pcbphysicalfiguremacroref))
   {
      Graph_Block_Reference(pcbphysicalfiguremacroref, lname, 
         0, G.cur_pos_x, G.cur_pos_y, DegToRad(G.cur_rotation), G.cur_mirror , 1.0, G.cur_layerindex, TRUE);
   }

   return res;
}

/****************************************************************************/
/*
   schematicsymbolref = "";
   curdata 
*/
static int pageTitleBlockAttributes()
{

   int res = loop_command(pageTitleBlockAttributes_lst,SIZ_PAGETITLEBLOCKATTRIBUTES_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int pageTitleBlockAttributeDisplay()
{
   int res = loop_command(pageTitleBlockAttributeDisplay_lst,SIZ_PAGETITLEBLOCKATTRIBUTEDISPLAY_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int companyname()
{
   if (!get_tok())      return p_error();  // EDIF

   if (curblock)
   {
      doc->SetUnknownAttrib(&curblock->getAttributesRef(),"COMPANYNAME" , token, SA_APPEND, NULL); 
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int desigator()
{
   if (!get_tok())      return p_error();  // EDIF

   if (curblock)
   {
      doc->SetUnknownAttrib(&curblock->getAttributesRef(),"COMPANYNAME" , token, SA_APPEND, NULL); 
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int drawingsize()
{
   if (!get_tok())      return p_error();  // EDIF

   if (curblock)
   {
      doc->SetUnknownAttrib(&curblock->getAttributesRef(),"DRAWINGSIZE" , token, SA_APPEND, NULL); 
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int drawingDescription()
{
   propkey = "DRAWINGDESCRIPTION";

   if (!get_tok())      return p_error();  // EDIF

   if (curdata && curdata->getDataType() == T_INSERT)
   {
      Attrib *a =  is_attvalue(doc, curdata->getAttributesRef(), propkey, 0);
      if (a)
      {
         a->setValueType(VT_STRING);
         a->setStringValueIndex(doc->RegisterValue(token));
      }  
      else
      {
         doc->SetUnknownAttrib(&curdata->getAttributesRef(), propkey, token, SA_APPEND, NULL); 
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int revision()
{
   propkey = "REVISION";

   if (!get_tok())      return p_error();  // EDIF

   if (curdata && curdata->getDataType() == T_INSERT)
   {
      Attrib *a =  is_attvalue(doc, curdata->getAttributesRef(), propkey, 0);
      if (a)
      {
         a->setValueType(VT_STRING);
         a->setStringValueIndex(doc->RegisterValue(token));
      }  
      else
      {
         doc->SetUnknownAttrib(&curdata->getAttributesRef(), propkey, token, SA_APPEND, NULL); 
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pageTitle()
{
   propkey = "PAGETITLE";

   if (!get_tok())      return p_error();  // EDIF

   if (curdata && curdata->getDataType() == T_INSERT)
   {
      Attrib *a =  is_attvalue(doc, curdata->getAttributesRef(), propkey, 0);
      if (a)
      {
         a->setValueType(VT_STRING);
         a->setStringValueIndex(doc->RegisterValue(token));
      }  
      else
      {
         doc->SetUnknownAttrib(&curdata->getAttributesRef(), propkey, token, SA_APPEND, NULL); 
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int pageIdentification()
{
   propkey = "PAGEIDENTIFICATION";

   if (!get_tok())      return p_error();  // EDIF

   if (curdata && curdata->getDataType() == T_INSERT)
   {
      Attrib *a =  is_attvalue(doc, curdata->getAttributesRef(), propkey, 0);
      if (a)
      {
         a->setValueType(VT_STRING);
         a->setStringValueIndex(doc->RegisterValue(token));
      }  
      else
      {
         doc->SetUnknownAttrib(&curdata->getAttributesRef(), propkey, token, SA_APPEND, NULL); 
      }
   }

   return 1;
}

/******************************************************************************
* companynamedisplay
*/
static int companynamedisplay()
{
   propkey = "COMPANYNAME";
   cur_name = "";
   // get the company name attribute
   if (curblock)
   {
      Attrib *a =  is_attvalue(doc, curblock->getAttributesRef(), propkey, 0);
      if (a)
      {
         cur_name = get_attvalue_string(doc, a);
      }  
   }

   int res = loop_command(addDisplay_lst,SIZ_ADDDISPLAY_LST);
   return 1;
}

/******************************************************************************
* designatorDisplay
*/
static int designatorDisplay()
{
   propkey = "DESIGNATOR";
   cur_name = "";

   if (curblock)
   {
      Attrib *a =  is_attvalue(doc, curblock->getAttributesRef(), propkey, 0);
      if (a)
         cur_name = get_attvalue_string(doc, a);
   }

   int res = loop_command(addDisplay_lst,SIZ_ADDDISPLAY_LST);
   return 1;
}

/******************************************************************************
* drawingsizedisplay
*/
static int drawingsizedisplay()
{
   propkey = "DRAWINGSIZE";
   cur_name = "";

   // get the attribute
   if (curblock)
   {
      Attrib *a =  is_attvalue(doc, curblock->getAttributesRef(), propkey, 0);
      if (a)
      {
         cur_name = get_attvalue_string(doc, a);
      }  
   }

   int res = loop_command(addDisplay_lst,SIZ_ADDDISPLAY_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int drawingDescriptiondisplay()
{
   propkey = "DRAWINGDESCRIPTION";
   cur_name = "";

   // get the attribute
   if (curblock)
   {
      Attrib *a =  is_attvalue(doc, curblock->getAttributesRef(), propkey, 0);
      if (a)
      {
         cur_name = get_attvalue_string(doc, a);
      }  
   }

   int res = loop_command(addDisplay_lst,SIZ_ADDDISPLAY_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int revisionDisplay()
{
   propkey = "REVISION";
   cur_name = "";

   // get the attribute
   if (curblock)
   {
      Attrib *a =  is_attvalue(doc, curblock->getAttributesRef(), propkey, 0);
      if (a)
      {
         cur_name = get_attvalue_string(doc, a);
      }  
   }

   int res = loop_command(addDisplay_lst,SIZ_ADDDISPLAY_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int pageTitleDisplay()
{
   propkey = "PAGETITLE";
   cur_name = "";

   // get the attribute
   if (curblock)
   {
      Attrib *a =  is_attvalue(doc, curblock->getAttributesRef(), propkey, 0);
      if (a)
      {
         cur_name = get_attvalue_string(doc, a);
      }  
   }

   int res = loop_command(addDisplay_lst,SIZ_ADDDISPLAY_LST);
   return 1;
}

/****************************************************************************/
/*
*/
static int pageIdentificationDisplay()
{
   propkey = "PAGEIDENTIFICATION";
   cur_name = "";

   // get the attribute
   if (curblock)
   {
      Attrib *a =  is_attvalue(doc, curblock->getAttributesRef(), propkey, 0);
      if (a)
      {
         cur_name = get_attvalue_string(doc, a);
      }  
   }

   int res = loop_command(addDisplay_lst,SIZ_ADDDISPLAY_LST);
   return 1;
}

/******************************************************************************
* schematicComplexFigure
*/
static int schematicComplexFigure()
{
   if (!get_tok())
      return p_error();  // EDIF

   CString name = "";
   if (token[0] == '(' && !token_name)
      push_tok();
   else
      name = token;

   schematicfiguremacroref = "";
   int res = loop_command(schematicComplexFigure_lst, SIZ_SCHEMATICCOMPLEXFIGURE_LST);

   if (strlen(schematicfiguremacroref))
   {
      Graph_Block_Reference(schematicfiguremacroref, "", 0, G.cur_pos_x, G.cur_pos_y, 
            DegToRad(G.cur_rotation), G.cur_mirror, 1.0, -1, TRUE);
   }

   return res;
}

/******************************************************************************
* get_logicalinstance
*/
static int get_logicalinstance(const char *l)
{
   for (int i=0; i<logicalinstancecnt; i++)
   {
      if (logicalinstancearray[i]->instance.CompareNoCase(l) == 0)
         return i;
   }

   return -1;
}

/******************************************************************************
* schematicMasterPortImplementation
*/
static int schematicMasterPortImplementation()
{
   if (!get_tok())
      return p_error();  // EDIF

   CString name = "";
   if (token[0] == '(' && !token_name)
      push_tok();
   else
      name = token;

   schematicsymbolref = "";
   instancename = name;
   curdata = NULL;

   // here are attribbutes mentioned. 
   int res = loop_command(schematicInstanceImplementation_lst, SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST);

   return res;
}

/******************************************************************************
* instancePortAttributeDisplay
*/
static int instancePortAttributeDisplay()
{
   edif_nameref();
   designatorkey.Format("PORTDES %s", cur_name);
   designatorkey.TrimLeft();
   designatorkey.TrimRight();

   int res = loop_command(instancePortAttributeDisplay_lst, SIZ_INSTANCEPORTATTRIBUTEDISPLAY_LST);
   return res;
}

/******************************************************************************
* schematicGlobalPortImplementation
*/
static int schematicGlobalPortImplementation()
{
   if (!get_tok())
      return p_error();  // EDIF

   CString name = "";
   if (token[0] == '(' && !token_name)
      push_tok();
   else
      name = token;

   schematicsymbolref = "";
   instancename = name;
   curdata = NULL;

   // here are attribbutes mentioned. 
   int res = loop_command(schematicInstanceImplementation_lst, SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST);

   return res;
}

/******************************************************************************
* schematicOffPageConnectorImplementation
*/
static int schematicOffPageConnectorImplementation()
{
   if (!get_tok())
      return p_error();  // EDIF

   CString name = "";
   if (token[0] == '(' && !token_name)
      push_tok();
   else
      name = token;

   schematicsymbolref = "";
   instancename = name;
   curdata = NULL;

   // here are attribbutes mentioned. 
   int res = loop_command(schematicInstanceImplementation_lst, SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST);

   return res;
}

/******************************************************************************
* schematicOnPageConnectorImplementation
*/
static int schematicOnPageConnectorImplementation()
{
   CString  name;

   if (!get_tok())      return p_error();  // EDIF

   if (token[0] == '(' && !token_name)
   {
      name = "";
      push_tok();
   }
   else
      name = token;

   schematicsymbolref = "";
   instancename = name;

   curdata = NULL;

   // here are attribbutes mentioned. 
   int res = loop_command(schematicInstanceImplementation_lst,SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicInstanceImplementation()
{
   CString  name;

   if (!get_tok())
      return p_error();

   if (token[0] == '(' && !token_name)
   {
      name = "";
      push_tok();
   }
   else
      name = token;

   schematicsymbolref = "";
   instancename = "";

   curdata = NULL;

   // here are attribbutes mentioned. 
   int res = loop_command(schematicInstanceImplementation_lst,SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST);

/******* this is done schematicsymbolRef because attributes are following, which I need to attach to the instance.
   int logicalinstanceptr = get_logicalinstance(instancename);
   if (logicalinstanceptr > -1)
   {
      EDIFLogicalInstance *l = logicalinstancearray[logicalinstanceptr];   // for debug only
      cell_schematicsymbolref.Format("%s %s", logicalinstancearray[logicalinstanceptr]->cellref, schematicsymbolref);
   }
   else
   {
      fprintf(ferr,"Instance [%s] not found in Logical Instance list at %ld\n", 
         instancename, ifp_line);
      display_error++;
   }

   CString  lname;
   curlayer = doc->FindLayer(G.cur_layerindex);
   lname = curlayer->name;

   // schematicsymbolref_
   // here place
   if (strlen(cell_schematicsymbolref))
   {
      Graph_Block_Reference(cell_schematicsymbolref, instancename, 
         0, G.cur_pos_x, G.cur_pos_y, DegToRad(G.cur_rotation), G.cur_mirror , 1.0, G.cur_layerindex, TRUE);
   }
******/

   return res;
}


/****************************************************************************/
/*
*/
static int pageTitleBlock()
{
   CString  name;

   if (!get_tok())      return p_error();  // EDIF

   if (token[0] == '(' && !token_name)
   {
      name = "";
      push_tok();
   }
   else
      name = token;

   schematicsymbolref = "";
   instancename = "";

   curdata = NULL;
   // here are attributes mentioned. 
   int res = loop_command(schematicInstanceImplementation_lst,SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pageBorder()
{
   CString  name;

   if (!get_tok())      return p_error();  // EDIF

   if (token[0] == '(' && !token_name)
   {
      name = "";
      push_tok();
   }
   else
      name = token;

   schematicsymbolref = "";
   instancename = "";

   curdata = NULL;

   // here are attribbutes mentioned. 
   int res = loop_command(schematicInstanceImplementation_lst,SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int pageHeader()
{
   // here are attribbutes mentioned. 
   int res = loop_command(pageheader_lst,SIZ_PAGEHEADER_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicNet()
{
   edif_namedef();
   cur_netname = cur_name;

   NetStruct *n = add_net(file,cur_netname);
   int res = loop_command(schematicNet_lst,SIZ_SCHEMATICNET_LST);
   cur_netname = "";

   return res;
}

/****************************************************************************/
/*
*/
static int schematicBus()
{
   edif_namedef();
   cur_netname = cur_name;

   int res = loop_command(schematicBus_lst,SIZ_SCHEMATICBUS_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicSubNet()
{
   edif_namedef();

   int res = loop_command(schematicNet_lst,SIZ_SCHEMATICNET_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int schematicSubNetSet()
{
   int res = loop_command(schematicSubNetSet_lst,SIZ_SCHEMATICSUBNETSET_LST);
   return res;
}

/****************************************************************************/
/*
*/
static int schematicNetDetails()
{
   int res = loop_command(schematicNetGraphics_lst,SIZ_SCHEMATICNETGRAPHICS_LST);
   return res;
}

/****************************************************************************/
/*
*/
static int schematicBusDetails()
{
   int res = loop_command(schematicBusGraphics_lst,SIZ_SCHEMATICBUSGRAPHICS_LST);
   return res;
}

/****************************************************************************/
/*
*/
static int schematicNetGraphics()
{
   int res = loop_command(connectlocation_lst,SIZ_CONNECTLOCATION_LST);
   return res;
}

/****************************************************************************/
/*
*/
static int schematicBusGraphics()
{
   int res = loop_command(connectlocation_lst,SIZ_CONNECTLOCATION_LST);
   return res;
}

/****************************************************************************/
/*
*/
static int fixedScaleTransform()
{

   G.cur_pos_x = 0;           /* position.   */
   G.cur_pos_y = 0;     
   G.cur_rotation = 0;        /* Rotation.   */
   G.cur_mirror = 0;          /* Current mirror.   */

   int res = loop_command(fixedScaleTransform_lst,SIZ_FIXEDSCALETRANSFORM_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int edif_transform()
{

   G.cur_pos_x = 0;           /* position.   */
   G.cur_pos_y = 0;     
   G.cur_rotation = 0;        /* Rotation in degree.   */
   G.cur_mirror = 0;          /* Current mirror.   */

   int res = loop_command(transform_lst,SIZ_TRANSFORM_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int instance_transform()
{

   G.cur_pos_x = 0;           /* position.   */
   G.cur_pos_y = 0;     
   G.cur_rotation = 0;        /* Rotation in degree.   */
   G.cur_mirror = 0;          /* Current mirror.   */

   int res = loop_command(transform_lst,SIZ_TRANSFORM_LST);
   
   if (curdata)
   {
      curdata->getInsert()->setOriginX(G.cur_pos_x);
      curdata->getInsert()->setOriginY(G.cur_pos_y);
      curdata->getInsert()->setAngle(DegToRad(G.cur_rotation));
      curdata->getInsert()->setMirrorFlags(G.cur_mirror); 
   }

   return res;
}

/****************************************************************************/
/*
*/
static int edif_figuregroupoverride()
{
   if (!get_tok())   return p_error();  // name

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      //G.cur_layerindex = 0;
      cur_figuregroup = "";
   }
   else
   {
      //G.cur_layerindex = Graph_Level(token,"",0);
      cur_figuregroup = get_rename(token, loop_depth);
      set_figuregroupdefaults(cur_figuregroup);
   }
   int res = loop_command(figuregroupoverride_lst,SIZ_FIGUREGROUPOVERRIDE_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbContributesToFabrication()
{
   int res = loop_command(pcbContributesToFabrication_lst,SIZ_PCBCONTRIBUTESTOFABRICATION_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbFigureAttributeOverride()
{
   CString  t;
   if (!get_tok())   return p_error(); 
   t = token;

   int   g = get_geometrygrp(token);

   if (g > -1)
      G.cur_widthindex = geometrygrarray[g]->widthindex;
   else
   {
      G.cur_widthindex = 0;
      fprintf(ferr,"pcbFigureAttributeOverride [%s] not found\n",token);
      display_error++;
   }

   int res = loop_command(pcbFigureAttributeOverride_lst,SIZ_PCBFIGUREATTRIBUTEOVERRIDE_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int pcbDrawingTextDrawingAttributeOverride()
{
   CString  t;
   if (!get_tok())   return p_error(); 
   t = token;

   int res = loop_command(pcbDrawingTextDrawingAttributeOverride_lst,
               SIZ_PCBDRAWINGTEXTDRAWINGATTRIBUTEOVERRIDE_LST);
   
   return res;
}


/****************************************************************************/
/*
*/
static int geometryElementSet()
{
   int res = loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int figure200()
{
   CString  fname;
   int      oldindex = G.cur_layerindex;


   if (!get_tok())   return p_error();  // name
   fname = token;


   if (token[0] == '(' && !token_name)
   {
      push_tok();
      //G.cur_layerindex = 0;
   }
   else
   {
      fname = get_rename(token, loop_depth);
      set_figuregroupdefaults(fname);
   }

   cur_layername = fname;
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   int res = loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   
   G.cur_layerindex = oldindex;

   return res;
}

/****************************************************************************/
/*
*/
static int pagesize()
{
   G.cur_layerindex = Graph_Level("PAGESIZE","",0);

   int res = loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int stringdisplay()
{
   CString  sd;

   sd = cur_name;
   edif_stringvalue();
   //cur_name = token;
   G.cur_visible = TRUE;

   int res = loop_command(display_lst,SIZ_DISPLAY_LST);

   return res;
}

/****************************************************************************/
/*
   This type of function is when there is either a name or a set of () 
   check if name or not .
      if name ok
      else loop.
*/
static int display()
{
   CString  d;

   G.cur_display = TRUE;
   G.cur_height = 0.1;
   G.cur_visible = TRUE;      // default for cur_visible
   G.cur_pos_x = 0;           // position.   
   G.cur_pos_y = 0;     
   G.cur_rotation = 0;        // Rotation.   
   G.cur_mirror = 0;          // Current mirror.   
   G.cur_display = TRUE;      // yes do display
   G.cur_textjust = 0;
   G.cur_booleanvalue = TRUE;

   if (!get_tok())   return p_error();  // name
   d = token;

   if (token[0] == '(' && !token_name)
   {
      push_tok();
   }
   else
   {
      cur_figuregroup = get_rename(token, loop_depth);
      set_figuregroupdefaults(cur_figuregroup);
   }

   int res = loop_command(edif_display_lst,SIZ_EDIF_DISPLAY_LST);

   if (G.cur_attribute)
      return res;

   // normalize
   normalize_text(&G.cur_pos_x, &G.cur_pos_y, G.cur_textjust,
                  G.cur_rotation,
                  G.cur_mirror,
                  G.cur_height,
                  G.cur_height * TEXTRATIO * strlen(cur_name));
   
   if (G.cur_visible == FALSE)
   {
      CString  l;
      l.Format("%s (not visible)",cur_layername);
      G.cur_layerindex = Graph_Level(l,"", 0);
      LayerStruct *lay = doc->FindLayer(G.cur_layerindex);
      lay->setVisible(false);
   }
   else
   {
      G.cur_layerindex = Graph_Level(cur_layername,"", 0);
   }

   // no mirror on text, but need to adjust non mirror text, 
   // because mirror is written from right to left, non mirror left to right
   if (G.cur_mirror)
   {
      double tx = 0, ty = 0;
      Rotate(G.cur_height*TEXTRATIO*strlen(cur_name),0.0, G.cur_rotation,&tx,&ty);
      G.cur_pos_x = G.cur_pos_x - tx;
      G.cur_pos_y = G.cur_pos_y - ty;
   }

   if (G.cur_display) 
   {
      Graph_Text(G.cur_layerindex, cur_name, G.cur_pos_x, G.cur_pos_y,
               G.cur_height, G.cur_height*TEXTRATIO, DegToRad(G.cur_rotation), 0,
               text_prop, 0, 0, 0,  // no mirror
               0, 0);
   }

   G.cur_layerindex = Graph_Level(cur_layername,"", 0);

   return res;
}

/****************************************************************************/
/*
   This type of function is when there is either a name or a set of () 
   check if name or not .
      if name ok
      else loop.
*/
static int propdisplay()
{
   CString  d;
   CString  prop_key = propkey;

   G.cur_display = TRUE;

   G.cur_visible = TRUE;      // default for cur_visible
   G.cur_pos_x = 0;           // position.   
   G.cur_pos_y = 0;     
   G.cur_rotation = 0;        // Rotation.   
   G.cur_mirror = 0;          // Current mirror.   
   G.cur_display = TRUE;      // yes do display
   G.cur_textjust = 0;
   G.cur_height = 0.1;
   G.cur_booleanvalue = TRUE;

   if (!get_tok())
      return p_error();  // name
   d = token;

   if (token[0] == '(' && !token_name)
   {
      push_tok();
   }
   else
   {
      cur_figuregroup = get_rename(token, loop_depth);
      set_figuregroupdefaults(cur_figuregroup);
   }

   int res = loop_command(edif_display_lst, SIZ_EDIF_DISPLAY_LST);

   write_attributes(prop_key, cur_name);

   G.cur_layerindex = Graph_Level(cur_layername,"", 0);

   return res;
}

/****************************************************************************/
/*
*/
static int edif_annotate()
{
   int         res;
   CString     old_layer;

   old_layer = cur_layername;

   G.cur_textjust =  0;
   G.cur_visible = FALSE;
   G.cur_placed = FALSE;
   cur_layername = "ANNOTATE";

   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   edif_string();
   cur_name = cur_text;

   res = loop_command(display_lst,SIZ_DISPLAY_LST);

   cur_layername = old_layer;
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   return res;
}

/****************************************************************************/
/*
*/
static int page_portimplementation()
{
   edif_nameref();
   int res = loop_command(page_portimplementation_lst,SIZ_PAGE_PORTIMPLEMENTATION_LST);

   return res;
}

/****************************************************************************/
/*
*/
static int portimplementation200()
{
   CString  old_layer = cur_layername;
   CString  portname = "Port";

   if (!get_tok())   return p_error();  // portname or (name ...
   if (token[0] == '(' && !token_name)
   {
      push_tok();
   }

   // here is a cur_libname
   if (curblock)
      Graph_Block_Off();   // current cell

   G.cur_pos_x = 0;           /* position.   */
   G.cur_pos_y = 0;     
   G.cur_rotation = 0;        /* Rotation.   */
   G.cur_mirror = 0;          /* Current mirror.   */
   G.cur_textjust = 0;
   G.cur_height = 0.1;
   G.cur_booleanvalue = TRUE;
   cur_portname = "";
   cur_layername += "_PORTIMPLEMENTATION";
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   // guaranty that port is unique
   portname.Format("%s_%ld",cur_libname,ifp_line);

   curblock = Graph_Block_On(GBO_APPEND, portname, cur_libnum, 0L);// declares and begins defining a block
   curblock->setBlockType(BLOCKTYPE_GATEPORT);
   int res = loop_command(portimplementation_lst,SIZ_PORTIMPLEMENTATION_LST);
   Graph_Block_Off();

   // reopen the cell and continue
   curblock = Graph_Block_On(GBO_APPEND, cur_libname, cur_libnum, 0L);

   DataStruct *d = Graph_Block_Reference(portname, cur_portname, 0, 
         0.0 , 0.0, 0.0, 0, 1.0, 0, 1);
   d->getInsert()->setInsertType(insertTypePortInstance);

   cur_layername = old_layer;
   G.cur_layerindex = Graph_Level(cur_layername,"",0);

   return res;
}

/****************************************************************************/
/*
*/
static int connectlocation()
{
   int res = loop_command(connectlocation_lst,SIZ_CONNECTLOCATION_LST);
   
   return res;
}

/****************************************************************************/
/*
*/
static int viewref200()
{
   CString  viewname;
   CString  cell;

   edif_nameref();
   if (ALLOW_VIEWRENAME)
      viewname = get_rename(cur_name, loop_depth);
   else
      viewname = cur_name;

   int res = loop_command(viewref200_lst,SIZ_VIEWREF200_LST);
   
   cell.Format("%s %s",cellrefname, viewname); // this is the current used cell.

   //xxx here place
   if (strlen(instancename))
   {
      curdata = Graph_Block_Reference(cell, instancename, 
         0, 0.0, 0.0, 0.0, 0 , 1.0, 0, TRUE);
   }

   return res;
}

/******************************************************************************
* cellRef
*/
static int cellRef()
{
   CString cn = cur_name;
   edif_nameref();

   if (ALLOW_CELLRENAME)
      cellrefname = get_rename(cur_name, loop_depth);
   else
      cellrefname = orig_rename; 

   cur_name = cn;
   int res = loop_command(cellRef_lst, SIZ_CELLREF_LST);
   
   return res;
}

/******************************************************************************
* libraryRef
*/
static int libraryRef()
{

   if (!get_tok())
      return p_error(); 
   librefname = token;
   
   return 1;
}

/******************************************************************************
* designHierarchy
*/
static int designHierarchy()
{
   if (!get_tok())
      return p_error();

   int res = loop_command(designHierarchy_lst, SIZ_DESIGNHIERARCHY_LST);

   return res;
}

/******************************************************************************
* occurrenceHierarchyAnnotate
*/
static int occurrenceHierarchyAnnotate()
{
   int res = loop_command(occurrenceHierarchyAnnotate_lst, SIZ_OCCURRENCEHIERARCHYANNOTATE_LST);

   return res;
}

/******************************************************************************
* occurrenceAnnotate
*/
static int occurrenceAnnotate()
{
   if (!get_tok())
      return p_error(); 
   CString instanceName = token;

   DataStruct *previousData = curdata;
   curdata = getInsertData(instanceName);
   int res = loop_command(occurrenceAnnotate_lst, SIZ_OCCURRENCEANNOTATE_LST);
   curdata = previousData;

   return res;
}

/******************************************************************************
* leafOccurrenceAnnotate
*/
static int leafOccurrenceAnnotate()
{
   if (!get_tok())
      return p_error(); 
   CString instanceName = token;

   DataStruct *previousData = curdata;
   curdata = getInsertData(instanceName);
   int res = loop_command(leafOccurrenceAnnotate_lst, SIZ_LEAFOCCURRENCEANNOTATE_LST);
   curdata = previousData;

   return res;
}

/******************************************************************************
* instancePropertyOverride
*/
static int instancePropertyOverride()
{
   CString old_lay = cur_layername;
   cur_layername += "_PROPERTY";
   G.cur_textjust = 0;
   G.cur_visible = FALSE;
   G.cur_placed = FALSE;
   G.cur_attribute = TRUE;

   edif_namedef();
   CString propertyKey = get_attribmap(get_rename(cur_name, loop_depth));
   propertyKey.MakeUpper();

   edif_typedvalue();
   CString propertyValue = "";
   if (G.cur_typeval == TYPEVAL_STRING)
      propertyValue = cur_text;
               
   if (!get_tok())
      return p_error();  

   push_tok();
   if (token[0] == '(' && !token_name)
      fskip();

   if (curdata)
   {
      Attrib *attrib = is_attvalue(doc, curdata->getAttributesRef(), propertyKey, 0);
      if (attrib)
      {
         attrib->setValueType(VT_STRING);
         attrib->setStringValueIndex(doc->RegisterValue(propertyValue));
      }  
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int unitRef()
{

   if (!get_tok())   return p_error();  // EDIF
   unitrefname = token;
   
   return 1;
}

/****************************************************************************/
/*
*/
static int pcbBareBoardDefinitionRef()
{

   if (!get_tok())   return p_error();  // EDIF
   pcbbareboarddefinitionref = token;

   return 1;
}

//////////////////////


/****************************************************************************/
/*
*/
static int systemgrid()
{
   double c;

   if (!get_tok())   return p_error();  // EDIF
   c = cnv_unit(token);
   // L_TGrid(inch_2_dbunit(c));
   return 1;
}

/****************************************************************************/
/*
*/
static int padtopad()
{
   double c;

   if (!get_tok())   return p_error();  // EDIF
   c = cnv_unit(token);
   // L_TClearancePP(inch_2_dbunit(c));
   return 1;
}

/****************************************************************************/
/*
*/
static int padtotrack()
{
   double c;

   if (!get_tok())   return p_error();  // EDIF
   c = cnv_unit(token);
   // L_TClearancePT(inch_2_dbunit(c));
   return 1;
}

/****************************************************************************/
/*
*/
static int tracktotrack()
{
   double c;

   if (!get_tok())   return p_error();  // EDIF
   c = cnv_unit(token);
   // L_TClearanceTT(inch_2_dbunit(c));
   return 1;
}

/****************************************************************************/
/*
*/
static int location()
{
   G.cur_mirror = FALSE;
   G.cur_rotation = 0.0;   // in degree
   return loop_command(location_lst,SIZ_LOCATION_LST);
}

/****************************************************************************/
/*
*/
static int edif_format()
{
   int   majv, minv;

   if (!get_tok())   return p_error();  // EDIF
   if (STRCMPI(token,"EDIF"))
   {
      fprintf(ferr,"Format [%s] wrong -> EDIF expected at %ld\n",token,ifp_line);
      return -1;
   }
   if (!get_tok())   p_error();  // major
   majv = atoi(token);

   if (majv < 3 || majv > 4)
   {
      fprintf(ferr,"Version [%d] wrong -> 3 or 4 expected at %ld\n",majv,ifp_line);
      return -1;
   }

   if (!get_tok())   p_error();  // minor
   minv = atoi(token);

   fprintf(ferr,"Version: EDIF %d %d\n",majv,minv);

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_pt()
{
   Point2   p;

   if (!get_tok())   return p_error();  // x
   p.x = cnv_unit(token);
   if (!get_tok())   return p_error();  // y
   p.y = cnv_unit(token);
   p.bulge = 0;
   polyarray.SetAtGrow(polycnt,p);
   polycnt++;
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_numberpoint()
{
   Point2   p;

   edif_numbervalue(&p.x);
   edif_numbervalue(&p.y);

   // instead of cnv_unit
   p.x *=faktor;
   p.y *=faktor;

   p.bulge = 0;
   polyarray.SetAtGrow(polycnt,p);
   polycnt++;
   return 1;
}

/****************************************************************************/
/*
   1. start of arc
   2. any point on arc
   3. end of arc
*/
static int edif_arc()
{
   // this fill the arc structure
   // arc in edif is from point to bulge and in
   // CAMCAD the first coo gets the bulge

   int res = loop_command(arc_lst,SIZ_ARC_LST);

   if (polycnt == 0)
   {
      fprintf(ferr,"Arc in start of Poly at %ld\n",ifp_line);
   }
   else
   if (polycnt < 3)
   {
      fprintf(ferr,"Arc error at %ld\n",ifp_line);
   }
   else
   {
      // 
      double sa,da,cx,cy,r;
      Point2 p1 = polyarray.ElementAt(polycnt-3);
      Point2 p2 = polyarray.ElementAt(polycnt-2);
      Point2 p3 = polyarray.ElementAt(polycnt-1);

      ArcPoint3( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y,
                  &cx, &cy, &r, &sa, &da);

      double c=tan(da/4);
      p1.bulge = c;
      polyarray.SetAt(polycnt-3,p1);
      p2.x = p3.x;
      p2.y = p3.y;
      p2.bulge = 0;
      polyarray.SetAt(polycnt-2,p2);
      polycnt--;  // kill last one.
   }

   return res;
}

/****************************************************************************/
/*
*/
static int edif_mirrored()
{
   G.cur_mirror = TRUE;
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_position()
{
   int   res;

   polyarray.RemoveAll();
   polycnt = 0;
   res = loop_command(pt_lst,SIZ_PT_LST);

   if (polycnt == 1)
   {
      Point2   p = polyarray.ElementAt(0);
      G.cur_pos_x = p.x;
      G.cur_pos_y = p.y;
   }
   else
   {
      fprintf(ferr,"Position error\n");
   }

   return res;
}

/****************************************************************************/
/*
*/
static int attrib_position()
{
   int   res;

   polyarray.RemoveAll();
   polycnt = 0;
   res = loop_command(pt_lst,SIZ_PT_LST);

   if (polycnt == 1)
   {
      Point2   p = polyarray.ElementAt(0);
      cur_attrib.x = p.x;
      cur_attrib.y = p.y;
   }
   else
   {
      fprintf(ferr,"Position error\n");
   }

   return res;
}

/****************************************************************************/
/*
*/
static int edif_string()
{

   if (!get_tok())   return p_error();  // name or stringdisplay
   cur_text = token;
   if (token[0] == '(' && !token_name) // name could start with a ()
   {
      push_tok();
      if (go_command(edif_stringdisplay_lst,SIZ_EDIF_STRINGDISPLAY_LST) < 0)
         return -1;
      cur_text = cur_name;
   }

   G.cur_typeval = TYPEVAL_STRING;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_powersignal()
{
   doc->SetAttrib(&curnetdata->getAttributesRef(),doc->IsKeyWord(ATT_POWERNET, 0),
            VT_NONE, NULL, SA_OVERWRITE, NULL);
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_justify()
{
   CString  o;
   if (!get_tok())   return p_error();  //
   o = token;

   G.cur_textjust = 0;

   if (!STRCMPI(token,"UPPERLEFT"))
   {
      G.cur_textjust = GRTEXT_H_T | GRTEXT_W_L;
   }
   else
   if (!STRCMPI(token,"UPPERCENTER"))
   {
      G.cur_textjust = GRTEXT_H_T | GRTEXT_W_C;
   }
   else
   if (!STRCMPI(token,"UPPERRIGHT"))
   {
      G.cur_textjust = GRTEXT_H_T | GRTEXT_W_R;
   }
   else
   if (!STRCMPI(token,"CENTERLEFT"))
   {
      G.cur_textjust = GRTEXT_H_C | GRTEXT_W_L;
   }
   else
   if (!STRCMPI(token,"CENTERCENTER"))
   {
      G.cur_textjust = GRTEXT_H_C | GRTEXT_W_C;
   }
   else
   if (!STRCMPI(token,"CENTERRIGHT"))
   {
      G.cur_textjust = GRTEXT_H_C | GRTEXT_W_R;
   }
   else
   if (!STRCMPI(token,"LOWERLEFT"))
   {
      G.cur_textjust = GRTEXT_H_B | GRTEXT_W_L;
   }
   else
   if (!STRCMPI(token,"LOWERCENTER"))
   {
      G.cur_textjust = GRTEXT_H_B | GRTEXT_W_C;
   }
   else
   if (!STRCMPI(token,"LOWERRIGHT"))
   {
      G.cur_textjust = GRTEXT_H_B | GRTEXT_W_R;
   }
   else
   {
      fprintf(ferr,"Unknown justification [%s] at %ld\n",token, ifp_line);
      G.cur_textjust = 0;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_orientation()
{
   CString  o;
   if (!get_tok())   return p_error();  //
   o = token;

   G.cur_rotation = 0;        /* Rotation in degree.   */
   G.cur_mirror = 0;          /* Current mirror.   */

   if (!STRCMPI(token,"R0"))
   {
      G.cur_rotation = 0;
      G.cur_mirror = 0;
   }
   else
   if (!STRCMPI(token,"R90"))
   {
      G.cur_rotation = 90;
      G.cur_mirror = 0;
   }
   else
   if (!STRCMPI(token,"R180"))
   {
      G.cur_rotation = 180;
      G.cur_mirror = 0;
   }
   else
   if (!STRCMPI(token,"R270"))
   {
      G.cur_rotation = 270;
      G.cur_mirror = 0;
   }
   else
   if (!STRCMPI(token,"MY"))
   {
      G.cur_rotation = 0;
      G.cur_mirror = 1;
   }
   else
   if (!STRCMPI(token,"MYR90"))
   {
      G.cur_rotation = 90;
      G.cur_mirror = 1;
   }
   else
   if (!STRCMPI(token,"MXR90"))
   {
      G.cur_rotation = 270;
      G.cur_mirror = 1;
   }
   else
   if (!STRCMPI(token,"MX"))
   {
      G.cur_rotation = 180;
      G.cur_mirror = 1;
   }
   else
   {
      fprintf(ferr,"Unknown orientation [%s] at %ld\n",token, ifp_line);
      G.cur_rotation = 0;
      G.cur_mirror = 0;
   }

   return 1;
}

/******************************************************************************
* edif_rotation
*/
static int edif_rotation()
{
   if (!get_tok())   return p_error();  //
   G.cur_rotation = atof(token);        /* Rotation in degree.   */

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_height()
{
   if (!get_tok())   return p_error();  //
   G.cur_height = cnv_unit(token);
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_width()
{
   double w;
   int   err;

   if (!get_tok())   return p_error();  //

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      fskip();
      w = 0;
   }
   else
      w = cnv_unit(token);

   if (w == 0)
      G.cur_widthindex = 0;
   else
      G.cur_widthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0,
                           BL_WIDTH, 0, &err);

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_textheight()
{
   if (!get_tok())   return p_error();  //

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      fnull();
      G.cur_height = 0;
   }
   else
      G.cur_height = cnv_unit(token) * TEXTHEIGHTCORRECT;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_textwidth()
{
   if (!get_tok())   return p_error();  //

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      fnull();
      G.cur_width = 0;
   }
   else
      G.cur_width = cnv_unit(token) * TEXTHEIGHTCORRECT;

   return 1;
}

/****************************************************************************/
/*
*/
static int booleanvalue_true()
{
   G.cur_booleanvalue = TRUE;
   return 1;
}

/****************************************************************************/
/*
*/
static int booleanvalue_false()
{
   G.cur_booleanvalue = FALSE;
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_color()
{
   int   red, blue, green;
   if (!get_tok())   return p_error();  //
   red = atoi(token);
   if (!get_tok())   return p_error();  //
   green = atoi(token);
   if (!get_tok())   return p_error();  //
   blue = atoi(token);
   G.color = RGB(red, green, blue);

   return 1;
}

/******************************************************************************
* horizontalJustification
*/
static int horizontalJustification()
{
   int err = go_command(horizontalJustification_lst, SIZ_HORIZONTALJUSTIFICATION_LST);
   return err;
}

/******************************************************************************
* leftJustify
*/
static int leftJustify()
{
   G.cur_textjust |= GRTEXT_W_L;
   return 1;
}

/******************************************************************************
* centerJustify
*/
static int centerJustify()
{
   G.cur_textjust |= GRTEXT_W_C;
   return 1;
}

/******************************************************************************
* rightJustify
*/
static int rightJustify()
{
   G.cur_textjust |= GRTEXT_W_R;
   return 1;
}

/******************************************************************************
* verticalJustification
*/
static int verticalJustification()
{
   int err = go_command(verticalJustification_lst, SIZ_VERTICALJUSTIFICATION_LST);
   return err;
}

/******************************************************************************
* topJustify
*/
static int topJustify()
{
   G.cur_textjust |= GRTEXT_H_T;
   return 1;
}

/******************************************************************************
* middleJustify
*/
static int middleJustify()
{
   G.cur_textjust |= GRTEXT_H_C;
   return 1;
}

/******************************************************************************
* bottomJustify
*/
static int bottomJustify()
{
   G.cur_textjust |= GRTEXT_H_B;
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_visible()
{
   int err = go_command(booleanvalue_lst, SIZ_BOOLEANVALUE_LST);  // do not use loop, this returns an error, 
   G.cur_visible = G.cur_booleanvalue;
   return err;
}

/******************************************************************************
* edif_displayattributes
*/
static int edif_displayattributes()
{
   G.cur_visible = TRUE;
   int err = loop_command(displayattributes_lst, SIZ_DISPLAYATTRIBUTES_LST);  //  
   return err;
}

/******************************************************************************
* propertyOverride
*/
static int propertyOverride()
{
   G.cur_attribute = TRUE;

   edif_namedef();
   CString prop_key = get_attribmap(get_rename(cur_name, loop_depth));
   prop_key.MakeUpper();
   if (prop_key == "REF")
   {
      int i = 0;
   }

   edif_typedvalue();
   CString prop_val = "";

   if (G.cur_typeval == TYPEVAL_STRING)
      prop_val = cur_text;
               
   if (!get_tok())
      return p_error();  

   if (token[0] == '(' && !token_name)
   {
      push_tok();
      fskip();
   }
   else
      push_tok();

   if (curdata)
   {
      Attrib *attrib =  is_attvalue(doc, curdata->getAttributesRef(), prop_key, 0);
      if (attrib)
      {
         attrib->setValueType(VT_STRING);
         attrib->setStringValueIndex(doc->RegisterValue(prop_val));
      }
      cur_name = prop_val;
   }
   else if (curblock)
   {
      Attrib *attrib =  is_attvalue(doc, curblock->getAttributesRef(), prop_key, 0);
      if (attrib)
      {
         attrib->setValueType(VT_STRING);
         attrib->setStringValueIndex(doc->RegisterValue(prop_val));
      }
      cur_name = prop_val;
   }

   G.cur_typeval = TYPEVAL_UNKNOWN;
   G.cur_attribute = FALSE;

   return 1;

}

/******************************************************************************
* propertyDisplayOverride
*/
static int propertyDisplayOverride()
{
   CString old_lay = cur_layername;

   edif_propertynameref();
   CString prop_key = get_attribmap(get_rename(cur_name, loop_depth));
   prop_key.MakeUpper();

   cur_layername = prop_key;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   CString prop_val = "";
   if (curdata)
   {
      Attrib *attrib =  is_attvalue(doc, curdata->getAttributesRef(), prop_key, 0);
      if (attrib)
         prop_val = get_attvalue_string(doc, attrib);

      cur_name = prop_val;
   }
   else if (curblock)
   {
      Attrib *attrib =  is_attvalue(doc, curblock->getAttributesRef(), prop_key, 0);
      if (attrib)
         prop_val = get_attvalue_string(doc, attrib);

      cur_name = prop_val;
   }

   propkey = cur_layername;
   if (loop_command(propertyDisplayOverride_lst, SIZ_PROPERTYDISPLAYOVERRIDE_LST) < 0)
      return -1;

   cur_layername = old_lay;
   G.cur_layerindex = Graph_Level(cur_layername, "", 0);

   if (curdata)
   {
      double x = G.cur_pos_x - curdata->getInsert()->getOriginX();
      double y = G.cur_pos_y - curdata->getInsert()->getOriginY();
      doc->SetUnknownVisAttrib(&curdata->getAttributesRef(), prop_key, prop_val, x, y, 
            DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
            G.cur_visible, SA_OVERWRITE, attflg, G.cur_layerindex, 1, 0, 0);
   }
   else if (curblock)
   {
      double x = G.cur_pos_x - curblock->getXoffset();
      double y = G.cur_pos_y - curblock->getYoffset();
      doc->SetUnknownVisAttrib(&curblock->getAttributesRef(), prop_key, prop_val, x, y, 
            DegToRad(G.cur_rotation), G.cur_height, G.cur_height * TEXTRATIO, 1, 0,
            G.cur_visible, SA_OVERWRITE, attflg, G.cur_layerindex, 1, 0, 0);
   }
   
   return 1;

// if (loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST) < 0)
   
}

/******************************************************************************
* addDisplay
*/
static int addDisplay()
{
   int res = loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST);
   return res;
}

/******************************************************************************
* replaceDisplay
*/
static int replaceDisplay()
{
   int res = loop_command(propdisplay_lst, SIZ_PROPDISPLAY_LST);
   return res;
}

/******************************************************************************
* removeDisplay
*/
static int removeDisplay()
{
   if (curblock)
   {
      Attrib *attrib =  is_attvalue(doc, curblock->getAttributesRef(), cur_layername, 0);
      if (attrib)
      {
         attrib->setVisible(false);
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_pointList()
{

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_curveList()
{

   if (loop_command(shapept_lst,SIZ_SHAPEPT_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int mirror_noMirroring()
{
   G.cur_mirror = FALSE;   
   return 1;
}

/****************************************************************************/
/*
*/
static int mirror_Mirroring()
{
   G.cur_mirror = TRUE; 
   return 1;
}

/****************************************************************************/
/*
*/
static int edif_mirroring()
{

   if (loop_command(mirroring_lst,SIZ_MIRRORING_LST) < 0)
      return -1;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_origin()
{
   G.cur_placed = TRUE;
   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt == 0) return 1;

   Point2   p1 = polyarray.ElementAt(0);

   G.cur_pos_x = p1.x;
   G.cur_pos_y = p1.y;

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_path()
{
   int   i;

   polyarray.RemoveAll();         
   polycnt = 0;

   if (loop_command(pointList_lst,SIZ_POINTLIST_LST) < 0)
      return -1;

   if (polycnt == 0) return 1;

   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);

   if (strlen(cur_netname))
   {
      curdata->setGraphicClass(GR_CLASS_ETCH);

      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING,
            cur_netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
   }

   curpoly = Graph_Poly(NULL, G.cur_widthindex, 0, 0, 0);   // fillflag , negative, closed

   for (i=0;i<polycnt;i++)
   {
      Point2   p1 = polyarray.ElementAt(i);

      if (curdata)
         Graph_Vertex(p1.x, p1.y, p1.bulge);
   }
   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_polygon()
{
   int   i;
   int   fill = 0;
   int   voidtype = 0;
   
   polyarray.RemoveAll();
   polycnt = 0;

   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);

   if (G.cur_boardoutline)
      curdata->setGraphicClass(GR_CLASS_BOARDOUTLINE);

   curpoly = Graph_Poly(NULL,G.cur_widthindex,fill, voidtype, 1); // fillflag , negative, closed

   if (loop_command(pointList_lst,SIZ_POINTLIST_LST) < 0)
      return -1;

   if (polycnt == 0) return 1;

   Point2   p1 = polyarray.ElementAt(0);
   Point2   p2 = polyarray.ElementAt(polycnt-1);

   if (p1.x != p2.x || p1.y != p2.y)
   {
      p2.x = p1.x;
      p2.y = p1.y;
      p2.bulge = 0;
      polyarray.SetAtGrow(polycnt,p2);  // fill, type
      polycnt++;
   }

   for (i=0;i<polycnt;i++)
   {
      Point2   p1 = polyarray.ElementAt(i);

      if (curdata)
         Graph_Vertex(p1.x, p1.y, p1.bulge);
   }
   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_unfilledRectangle()
{
   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt == 0) return 1;

   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);
   curpoly = Graph_Poly(NULL, G.cur_widthindex,0, 0, 1); // fillflag , negative, closed

   Point2   p1 = polyarray.ElementAt(0);
   Point2   p2 = polyarray.ElementAt(polycnt-1);

   Graph_Vertex(p1.x, p1.y, 0.0);
   Graph_Vertex(p2.x, p1.y, 0.0);
   Graph_Vertex(p2.x, p2.y, 0.0);
   Graph_Vertex(p1.x, p2.y, 0.0);
   Graph_Vertex(p1.x, p1.y, 0.0);

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
   circle is filled in EDIF 400.
   unfilled in EDIF200
*/
static int edif_circleByCenter()
{
   double diam;
   int    fill;

   if (read_edif_version == EDIF200)
      fill = 0;
   else
      fill = 1;

   polyarray.RemoveAll();
   polycnt = 0;

   if (go_command(pt_lst,SIZ_PT_LST) < 0) // do not use loop, this returns an error, 
      return -1;                          // because the next command is the diameter.

   if (polycnt == 0) return 1;

   if (!get_tok())   return p_error();  //
   diam = cnv_unit(token);
 
   // has no width
   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);
   curpoly = Graph_Poly(NULL,0, fill, 0, 1); // fillflag , negative, closed

   Point2 p = polyarray.ElementAt(0);
   Graph_Vertex(p.x-diam/2, p.y , 1);
   Graph_Vertex(p.x+diam/2, p.y , 1);
   Graph_Vertex(p.x-diam/2, p.y , 0);


   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
   rectangle is filled in EDIF 400.
   unfilled in EDIF200
*/
static int edif_rectangle()
{
   int   fill;

   if (read_edif_version == EDIF200)
      fill = 0;
   else
   if (read_edif_version == EDIF300)
      fill = 0;
   else
      fill = 1;

   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt == 0) return 1;

   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);

   // seems to have with 0
   curpoly = Graph_Poly(NULL, 0, fill, 0, 1);   // fillflag , negative, closed

   Point2   p1 = polyarray.ElementAt(0);
   Point2   p2 = polyarray.ElementAt(polycnt-1);

   Graph_Vertex(p1.x, p1.y, 0.0);
   Graph_Vertex(p2.x, p1.y, 0.0);
   Graph_Vertex(p2.x, p2.y, 0.0);
   Graph_Vertex(p1.x, p2.y, 0.0);
   Graph_Vertex(p1.x, p1.y, 0.0);

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_circle200()
{
   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   if (polycnt != 2)
   {
      fprintf(ferr, "Circle format error ([%d] polypoints) at %ld\n",
         polycnt, ifp_line);
      display_error++;
      return 1;
   }

   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);
   curpoly = Graph_Poly(NULL, G.cur_widthindex, 0, 0, 1);   // width, fillflag , negative, closed

   Point2   p1 = polyarray.ElementAt(0);
   Point2   p2 = polyarray.ElementAt(1);

   Graph_Vertex(p1.x, p1.y, 1.0);
   Graph_Vertex(p2.x, p2.y, 1.0);
   Graph_Vertex(p1.x, p1.y, 0.0);

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}
/****************************************************************************/
/*
*/
static int edif_dot200()
{
   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(pt_lst,SIZ_PT_LST) < 0)
      return -1;

   Point2   p1 = polyarray.ElementAt(0);

   Graph_Block_Reference("EDIF_DOT", NULL,
      0, p1.x , p1.y, 0.0, 0, 1.0, G.cur_layerindex, 1);

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
  last koo must be closed
  shape is always filled.
*/
static int edif_shape()
{
   int   i;
   int   voidtype = 0;
   int   fill = 1;

   if (read_edif_version == EDIF200)
      fill = 0;
   else
      fill = 1;

   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(curveList_lst,SIZ_CURVELIST_LST) < 0)
      return -1;

   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);

   // seems to have always zero width
   curpoly = Graph_Poly(NULL, G.cur_widthindex, fill, voidtype, 1);  // fillflag , negative, closed

   Point2   p1 = polyarray.ElementAt(0);
   Point2   p2 = polyarray.ElementAt(polycnt-1);

   if (p1.x != p2.x || p1.y != p2.y)
   {
      p2.x = p1.x;
      p2.y = p1.y;
      p2.bulge = 0;
      polyarray.SetAtGrow(polycnt,p2);  // fill, type
      polycnt++;
   }

   for (i=0;i<polycnt;i++)
   {
      Point2   p1 = polyarray.ElementAt(i);
      Graph_Vertex(p1.x, p1.y, p1.bulge);
   }
   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int edif_openshape()
{
   int   i;
   int   fill = 0;

   polyarray.RemoveAll();
   polycnt = 0;

   if (loop_command(curveList_lst,SIZ_CURVELIST_LST) < 0)
      return -1;

   curdata = Graph_PolyStruct(G.cur_layerindex,0L,0);

   // seems to have no width
   curpoly = Graph_Poly(NULL, G.cur_widthindex, fill, 0, 0);   // fillflag , negative, NOT closed

   for (i=0;i<polycnt;i++)
   {
      Point2   p = polyarray.ElementAt(i);
      Graph_Vertex(p.x, p.y, p.bulge);
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
   the cellRef
*/
static int edif_design()
{
   edif_namedef();
   CString designname = cur_name;

   cellrefname = ""; // the cell is part of the library
   librefname  = "";

   int res = loop_command(design_lst, SIZ_DESIGN_LST);

   // on edif 200 the design is recognized in pages and cells
   // on edif 400 the design is recognized in cluster

   if (read_edif_version == EDIF300)
   {
      doc->PrepareAddEntity(rootfile); 
      DataStruct *data = Graph_Block_Reference(cellrefname, designname, 0, 0.0, 0.0, 0.0, 0, 1.0, 0, 1);
      rootfile->setRefName(designname);

   }
   else if (read_edif_version == EDIF400)
   {     
      doc->PrepareAddEntity(rootfile);    
      BlockStruct *block = Graph_Block_Exists(doc, cellrefname, -1);
      Graph_Block_Copy(block, 0, 0, 0, 0, 1, -1, FALSE);
      rootfile->setBlockType(blockTypePcb);
      rootfile->setRefName(designname);
   }

   return res;
}

/****************************************************************************/
/*
*/
static int pcb_pcbprintsettings()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int pcb_drillsymsettings()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int pcb_gerbersettings()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int pcb_ncdrillsettings()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int pcb_programstate()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int compdef_compheader()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}

/****************************************************************************/
/*
*/
static int compdef_comppin()
{
   fskip();
   return 1; //loop_command(pcb_lst,SIZ_PCB_LST);
}


/****************************************************************************/
/*
*/
static int edif_id()
{
   if (!get_tok())   return p_error();
   G.cur_id = atoi(token);
   return 1;
}

/****************************************************************************/
/*
*/
static int attrib_visible()
{
   cur_attrib.visible = TRUE;
   return 1;
}

/****************************************************************************/
/*
*/
static int paddrill()
{
   int res = loop_command(drill_lst,SIZ_DRILL_LST);
   return res;
}


/****************************************************************************/
/*
*/
static int drillsize()
{
   if (!get_tok())   return p_error();
   G.cur_width  = cnv_unit(token);

   if (curblock && G.cur_width > 0)
   {
      double d = G.cur_width;
      CString  drillname;

      int drilllayer = Graph_Level("DRILLHOLE","",0);
      int drillindex = get_drillindex(G.cur_width);   

      drillname.Format("DRILL_%d",drillindex);
      // must be filenum 0, because apertures are global.
      Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                         0, 1.0, drilllayer, TRUE);
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int drillplated()
{
/* a drill is plated unless it says non plated
   if (curblock)
   {
      doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(DRILL_PLATED, 0),
            VT_NONE,
            NULL,
            0.0,0.0,0.0,attribute_height, 0.0,0,0, FALSE, SA_OVERWRITE, 0l,0,1); // x, y, rot, height
   }
*/
   return 1;
}

/****************************************************************************/

/******************************************************************************
* tok_search
*  - Search for the token in a command token list.
*/
static int tok_search(List *tok_lst, int tok_size)
{
   for (int i=0; i<tok_size; ++i)
      if (!STRCMPI(token,tok_lst[i].token))
         return i;

   return -1;
}

/******************************************************************************
* get_tok()
*  - Get a token from the input file.
*/
static int get_tok()
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

static int push_tok()
{
   return (Push_tok = TRUE);
}


/****************************************************************************/
/*
   Get the next token from line.
*/
static int get_next(char *lp,int newlp)
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
      case '(':
      case ')':
         token[i++] = *(cp++);
      break;
      case '\"':
         token[i] = '\0';
         token_name = TRUE;
         for (++cp; *cp != '\"'; ++cp, ++i)
         {
            if (i >= MAX_TOKEN-1)
            {
               fprintf(ferr,"Error in Get Token - Token too long at %ld\n", ifp_line);
               display_error++;
            }

            if (*cp == '\n')
            {
               /* text spans over 1 line */
               if (!get_line(cur_line,MAX_LINE))
               {
                  fprintf(ferr,"Error in Line Read\n");
                  display_error++;
               }
               cp = cur_line;
               if (*cp == '\"')  // new line starts with a end_quote.
               {
                  break;
               }
            }

            token[i] = *cp;
         }
         ++cp;
      break;
      default:
         for (; !isspace(*cp) && *cp != '\"' &&
                           *cp != '(' && *cp != ')' &&
                           *cp != '\0'; ++cp, ++i)
         {
            if (*cp == '\\')
               ++cp;
            token[i] = *cp;
         }
      break;
   }

   if (!i && !token_name)
      return(FALSE);

   token[i] = '\0';

   convert_special_char(token);

   return(TRUE);
}

/****************************************************************************/
/*
   Get a line from the input file.
*/
static int get_line(char *cp,int size)
{
   if (fgets(cp,size,ifp) == NULL)
   {
      if (!feof(ifp))
      {
         ErrorMessage("Read Error","Fatal EDIF Parse Error", MB_OK | MB_ICONHAND);
         return FALSE;
      }
      else
      {
         cp[0] = '\0';
         return(FALSE);
      }
   }
   else
   {
      if (cp[strlen(cp)-1] != '\n')
      {
         //printf(" No end of line found at %ld\n",ifp_line);
         fprintf(ferr, " No end of line found at %ld\n",ifp_line);
			fprintf(ferr, " Probable internal error, buffer overrun. Pleas go to http://www.mentor.com/supportnet for technical support.\n");
         return (FALSE);
      }
   }
   ++ifp_line;

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

   fprintf(ferr,"Fatal Parsing Error : Token \"%s\" on line %ld\n",token,ifp_line);
   display_error++;

   return -1;
}

/****************************************************************************/
/*
*/
void init_all_mem()
{
   figuregrarray.SetSize(100,100);
   figuregrcnt = 0;

   namearray.SetSize(100,100);
   namecnt = 0;

   geometrygrarray.SetSize(100,100);
   geometrygrcnt = 0;

   polyarray.SetSize(100,100);
   polycnt = 0;

   drillarray.SetSize(100,100);
   drillcnt = 0;

   renamearray.SetSize(100,100);
   renamecnt = 0; 

   shapepinnet.SetSize(100,100);
   shapepinnetcnt = 0;

   attribmaparray.SetSize(100,100);
   attribmapcnt = 0;

   settingsarray.SetSize(100,100);
   settingscnt = 0;

   logicalinstancearray.SetSize(100,100);
   logicalinstancecnt = 0;

   // not allowed
   memset(&G,0,sizeof(Global));

   return;
}

/****************************************************************************/
/*
*/
void free_all_mem()
{
   int   i;

   renamearray.RemoveAll();
   drillarray.RemoveAll();
   shapepinnet.RemoveAll();

   for (i=0;i<figuregrcnt;i++)
   {
      delete figuregrarray[i];      
   }
   figuregrarray.RemoveAll();

   for (i=0;i<geometrygrcnt;i++)
   {
      delete geometrygrarray[i];    
   }
   geometrygrarray.RemoveAll();

   for (i=0;i<namecnt;i++)
   {
      delete namearray[i];    
   }
   namearray.RemoveAll();

   for (i=0;i<attribmapcnt;i++)
   {
      delete attribmaparray[i];
   }
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   for (i=0;i<settingscnt;i++)
   {
      delete settingsarray[i];
   }
   settingsarray.RemoveAll();
   settingscnt = 0;

   for (i=0;i<logicalinstancecnt;i++)
   {
      EDIFLogicalInstance  *logicalInstance = logicalinstancearray[i];
      if (logicalInstance->data)
         RemoveOneEntityFromDataList(doc, logicalInstance->datalist, logicalInstance->data);
      delete logicalInstance;
   }
   logicalinstancearray.RemoveAll();
   logicalinstancecnt = 0;

   BlockStruct *b = Graph_Block_Exists(doc, "attribholder", -1);
   if (b)   doc->RemoveBlock(b);

}

/****************************************************************************/
/*
*/
static int get_drillindex(double size)
{
   EDIFDrill   p;
   int         i;

   if (size == 0) return -1;

   for (i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d",drillcnt);
   //BlockStruct  *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
   //curblock->blocktype = BLOCKTYPE_PADSHAPE;

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

/*
   Graph_Aperture(name, T_ROUND, size, 0.0, 0.0, 0.0, 0.0, 0, 
            BL_DRILL, TRUE, &err);
*/
   p.d = size;
   drillarray.SetAtGrow(drillcnt,p);  
   drillcnt++;

   return drillcnt -1;
}

/****************************************************************************/
/*
   NULL string will return 0  
   This units come in inches.
   edif file is in 10-8 meters = 0.01 micros = inch * 2540000

   Units_Factor(format.PortFileUnits, Settings.PageUnits)

*/
double   cnv_unit(char *s)
{
   double x;
   if (STRLEN(s) == 0)  return 0.0;
   x = atof(s);
   return x * faktor;
}

/****************************************************************************/
/*
*/
void clean_name(char *n)
{
   unsigned int   i;

   // elim from and back
   while (strlen(n) && isspace(n[strlen(n)-1]))
         n[strlen(n)-1] = '\0';
   STRREV(n);

   while (strlen(n) && isspace(n[strlen(n)-1]))
         n[strlen(n)-1] = '\0';
   STRREV(n);

   for (i=0;i<strlen(n);i++)
   {
      if (n[i] == ' ')  n[i] = '_';
   }
   return;
}

/****************************************************************************/
/*
  the sequence \n makes a new line
*/
static int get_nexttextline(char *orig,char *l,int maxl)
{
   unsigned int   i;
   int   lcnt = 0;
   int   newline = FALSE;
   int   mess = FALSE;

   l[0] = '\0';

   for (i=0;i<strlen(orig);i++)
   {
      if (orig[i] == '\n')
      {
         newline = TRUE;
         break;
      }
      else
      {
         if (lcnt < maxl)
         {
            l[lcnt++] = orig[i];
         }
         else
         {
            if (!mess)     // only write message once
            {
               printf("\rTextstring [%d] too long at %ld\n", maxl, ifp_line);
            }
            fprintf(ferr,"Textstring [%d] too long at %ld\n", maxl, ifp_line);
            l[maxl-1] = '\0';
            mess = TRUE;
         }
      }
   }

   if (lcnt)
   {
      l[lcnt] = '\0';
      if (newline)
      {
         STRREV(orig);
         orig[strlen(orig)-lcnt-1] = '\0';
         STRREV(orig);
      }
      else
         orig[0] = '\0';
   }

   return lcnt;
}

/****************************************************************************/
/*
*/
static int get_layerptr(const char *l)
{
   int   i;

   for (i=0;i<layerlistcnt;i++)
   {
      if (!STRCMPI(layerlist[i].name,l))
         return i;
   }   

   if (layerlistcnt < MAX_LAYERS)
   {
      layerlist[layerlistcnt].name = l;
      layerlist[layerlistcnt].username = "";
      layerlist[layerlistcnt].conductive = 0;
      layerlist[layerlistcnt].nonconductivetype = 0;
      layerlistcnt++;
      return layerlistcnt-1;
   }
   else
   {
      fprintf(ferr, "Too many layers\n");
      display_error++;
   }

   return -1;
}

/******************************************************************************
* load_edifsettings
*/
static int load_edifsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   compoutline = "";
   TEXTHEIGHTCORRECT = 1;
   ATTRIBUTES2TEXT = 0;
   dotsize = 0.02;
   text_prop = TRUE;
   TEXTRATIO = (6.0 / 8.0);
   EDIF_FILETYPE = FILETYPE_UNKNOWN;
   attribute_placement = 0;
   ALLOW_VIEWRENAME = TRUE;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp, "Edif Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] != '.')
         continue;
   
      if (!STRICMP(lp, ".COMPOUTLINE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         compoutline = lp;
      }
      else if (!STRICMP(lp, ".ATT_REFNAME"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         EDIF_Att_Refname = lp;
      }
      else if (!STRICMP(lp, ".TEXTHEIGHTCORRECT"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         double th = atof(lp);

         if (lp = get_string(NULL, " \t\n")) 
         {
            int p = make_programsettings(lp);
            settingsarray[p]->textheightcorrect = th;
         }
         else
         {
            TEXTHEIGHTCORRECT = th;
         }
      }
      else if (!STRICMP(lp, ".ATTRIBUTEPLACEMENT"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int ap = atoi(lp);

         if (lp = get_string(NULL, " \t\n")) 
         {
            int p = make_programsettings(lp);
            settingsarray[p]->attributeplacement = ap;
         }
         else
         {
            attribute_placement = ap;
         }
      }
      else if (!STRICMP(lp, ".TEXTRATIO"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         double tr = atof(lp);

         if (lp = get_string(NULL, " \t\n")) 
         {
            int p = make_programsettings(lp);
            settingsarray[p]->textratio = tr;
         }
         else
         {
            TEXTRATIO = tr;
         }
      }
      else if (!STRICMP(lp, ".DOTSIZE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         dotsize = atof(lp);
      }
      else if (!STRICMP(lp, ".PROPORTIONALTEXT"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (lp[0] == 'n' || lp[0] == 'N')
            text_prop = FALSE;
      }
      else if (!STRICMP(lp, ".ATTRIBUTES2TEXT"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (lp[0] == 'y' || lp[0] == 'Y')
            ATTRIBUTES2TEXT = TRUE;
      }
      else if (!STRICMP(lp, ".FILETYPE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         if (!STRCMPI(lp, "VIEWLOGIC"))
            EDIF_FILETYPE = FILETYPE_VIEWLOGIC;
      }
      else if (!STRICMP(lp, ".ATTRIBMAP"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attrib = lp;
         attrib.MakeUpper();
         attrib.TrimLeft();
         attrib.TrimRight();

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString mapAttrib = lp;
         mapAttrib.MakeUpper();
         mapAttrib.TrimLeft();
         mapAttrib.TrimRight();

         EDIFAttribmap  *attribMap = new EDIFAttribmap;
         attribmaparray.SetAtGrow(attribmapcnt++, attribMap);
         attribMap->attrib = attrib;
         attribMap->mapattrib = mapAttrib;
         
      }
   }

   fclose(fp);
   return 1;
}

/*****************************************************************************/
/*
   short    layertype;              // see dbutil.h dbutil.cpp
   short    artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
   short    electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
   short    physicalstacknumber;    // physical manufacturing stacking of layers, 
                                    // signaltop / dialectric / signal_1 / dia / power_1 .... / solder
   
   unsigned long attr;              // layer attributes

*/
static int assign_layers()
{
   int   electrical = 0;
   int   maxstackup = 0;
   int   i, ii;
   int   maxconductive = 0;
   int   silktop = 0;
   int   soldermasktop = 0;

   // 1. make physical layer stacking

   // find max stackup
   for (i=0;i<layerlistcnt;i++)
   {
      if (layerlist[i].artworkstacknumber > maxstackup)
         maxstackup = layerlist[i].artworkstacknumber;
      if (layerlist[i].conductive)
         maxconductive++;
      if (strlen(layerlist[i].username))
      {
         LayerStruct *l = doc->FindLayer_by_Name(layerlist[i].name);
         l->setComment(layerlist[i].username);
      }
      if (layerlist[i].nonconductivetype == EDIF_SILKSCREEN)
      {
         if (silktop == 0)
         {
            LayerStruct *l = doc->FindLayer_by_Name(layerlist[i].name);
            l->setLayerType(LAYTYPE_SILK_TOP);
            silktop++;
         }
         else
         if (silktop == 1)
         {
            LayerStruct *l = doc->FindLayer_by_Name(layerlist[i].name);
            l->setLayerType(LAYTYPE_SILK_BOTTOM);
            silktop++;
         }
      }
      if (layerlist[i].nonconductivetype == EDIF_SOLDERMASK)
      {
         if (soldermasktop == 0)
         {
            LayerStruct *l = doc->FindLayer_by_Name(layerlist[i].name);
            l->setLayerType(LAYTYPE_MASK_TOP);
            soldermasktop++;
         }
         else
         if (soldermasktop == 1)
         {
            LayerStruct *l = doc->FindLayer_by_Name(layerlist[i].name);
            l->setLayerType(LAYTYPE_MASK_BOTTOM);
            soldermasktop++;
         }
      }
   }

   for (i=1;i<=maxstackup;i++)
   {
      for (ii=0;ii<layerlistcnt;ii++)
      {
         if (layerlist[ii].artworkstacknumber == i)
         {
            if (layerlist[ii].conductive)
            {
               electrical++;
               LayerStruct *l = doc->FindLayer_by_Name(layerlist[ii].name);
               l->setElectricalStackNumber(electrical);
               if (electrical == 1)
               {
                  l->setLayerType(LAYTYPE_SIGNAL_TOP);
               }
               else
               if (electrical == maxconductive)
               {
                  l->setLayerType(LAYTYPE_SIGNAL_BOT);
               }
               else
               {
                  l->setLayerType(LAYTYPE_SIGNAL_INNER);
               }
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* get_nameindex
*/
static int get_nameindex(const char *n)
{
   for (int i=0;i<namecnt;i++)
   {
      if (namearray[i]->name.Compare(n) == 0)
         return i;
   }

   EDIFName *g = new EDIFName;
   g->name = n;
   namearray.SetAtGrow(namecnt++, g);

   return namecnt-1;
}

/******************************************************************************
* *getInsertData
*/
static DataStruct *getInsertData(CString insertName)
{
   POSITION dataPos = insertDataList.GetHeadPosition();
   while(dataPos)
   {
      DataStruct *data = insertDataList.GetNext(dataPos);
      if (data->getInsert()->getRefname() == insertName)
         return data;
   }
   return NULL;
}
