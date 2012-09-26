// $Header: /CAMCAD/5.0/read_wrt/Cct_in.cpp 20    3/12/07 12:49p Kurt Van Ness $

/****************************************************************************/
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
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;

/****************************************************************************/
/*
*/
/****************************************************************************/

static int   fnull();      // this writes to log file
static int   fskip();      // fskip does not write to log file

#define  TRUE                    1
#define  FALSE                   0

/* Defines *****************************************************************/

#define  CCTERR                  "cct.log"

#define  FNULL                   fnull

#define  MAX_LINE                2000  /* Max line length.        */
#define  MAX_TOKEN               MAX_LINE

#define  MAX_LAYERS              255   /* Max number of layers.   */
#define  MAX_SHAPE               1000
#define  MAX_PADSHAPE            1000

/* Layer codes.      */
#define  LAY_SIGNAL              1

#define  LAY_ALL_LAYERS          -1
#define  LAY_NULL                -2

#define  STAT_UNKNOWN            0
#define  STAT_SHAPE              1
#define  STAT_DESIGN             2

// this sets G.cur_type
#define  CCT_UNKNOWN             0
#define  CCT_PADSHAPE            1
#define  CCT_IMAGE               2

/* Structures **************************************************************/

typedef struct
{
   CString  name;          // L1..
   int   mirrorindex;      // L1..L255 number
   int   id;               // layer id.
   CString  username;
   DbFlag flg;
   char  used;
   char  used_as_pad;
   int   ppsignal[10];     // powerplane signal number to mark
                           // plane signals
}CCTLayerlist;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
}CCTAttribmap;
typedef CTypedPtrArray<CPtrArray, CCTAttribmap*> CAttribmapArray;

typedef  struct
{
   char  *token;
   int      (*function)();
} List;

/* Bracket definitions.       */
static List  brk_lst[] =
{
   "(",                    FNULL,
   ")",                    FNULL,
};

#define  SIZ_BRK_LST       (sizeof(brk_lst) / sizeof(List))
#define  BRK_ROUND         0
#define  BRK_B_ROUND       1

/* Start of CCT file.  */

static int      start_cct();

static List  start_lst[] =
{
   "pcb",                  start_cct,
};

#define  SIZ_START_LST       (sizeof(start_lst) / sizeof(List))

/* CCT main sections.      */
static   int      cct_library();
static   int      cct_placement();
static   int      cct_resolution();
static   int      cct_structure();
static   int      cct_network();
static   int      cct_wiring();
static   int      cct_parser();
static   int      cct_unit();

static List  cct_lst[] =
{
   "parser",               cct_parser,
   "resolution",           cct_resolution,
   "unit",                 cct_unit,
   "structure",            cct_structure,
   "placement",            cct_placement,
   "library",              cct_library,
   "network",              cct_network,
   "wiring",               cct_wiring,
   "color",                fnull,
};

#define  SIZ_CCT_LST       (sizeof(cct_lst) / sizeof(List))

static int  geometryElementSet();

static   int   cct_polygon();
static   int   cct_rectangle();
static   int   cct_path();
static   int   cct_circle();

static List  geometryElementSet_lst[] =
{
   "poly",                             cct_polygon,
   "polygon",                          cct_polygon,
   "circle",                           cct_circle,
   "circ",                             cct_circle,
   "rect",                             cct_rectangle,
   "path",                             cct_path,
};

#define  SIZ_GEOMETRYELEMENTSET_LST     (sizeof(geometryElementSet_lst) / sizeof(List))

static   int   cct_boundary();
static   int   cct_keepout();
static   int   cct_layer();

static List  structure_lst[] =
{
   "boundary",             cct_boundary,
   "keepout",              cct_keepout,
   "rule",                 fnull,
   "layer",                cct_layer,
};

#define  SIZ_STRUCTURE_LST    (sizeof(structure_lst) / sizeof(List))

static int  cct_string_quote();

static List  parser_lst[] =
{
   "string_quote",         cct_string_quote,
};

#define  SIZ_PARSER_LST      (sizeof(parser_lst) / sizeof(List))

static   int   cct_image();
static   int   cct_padstack();
static List  library_lst[] =
{
   "unit",                 cct_unit,
   "image",                cct_image,
   "padstack",             cct_padstack,
};

#define  SIZ_LIBRARY_LST     (sizeof(library_lst) / sizeof(List))

static   int   cct_net();

static List  network_lst[] =
{
   "net",                  cct_net,
};

#define  SIZ_NETWORK_LST     (sizeof(network_lst) / sizeof(List))

static   int   cct_wire();
static   int   cct_via();

static List  wiring_lst[] =
{
   "wire",                 cct_wire,
   "via",                  cct_via,
};

#define  SIZ_WIRING_LST     (sizeof(wiring_lst) / sizeof(List))

static   int   cct_component();

static List  placement_lst[] =
{
   "unit",                 cct_unit,
   "component",            cct_component,
};

#define  SIZ_PLACEMENT_LST     (sizeof(placement_lst) / sizeof(List))

static   int   image_side();
static   int   image_pin();
static   int   image_outline();

static List  image_lst[] =
{
   "unit",                 cct_unit,
   "side",                 image_side,
   "pin",                  image_pin,
   "outline",              image_outline,
};

#define  SIZ_IMAGE_LST     (sizeof(image_lst) / sizeof(List))

static   int   component_place();

static List  component_lst[] =
{
   "place",                   component_place,
};

#define  SIZ_COMPONENT_LST     (sizeof(component_lst) / sizeof(List))

static   int   net_pins();

static List  net_lst[] =
{
   "pins",                    net_pins,
};

#define  SIZ_NET_LST     (sizeof(net_lst) / sizeof(List))

static   int   cct_netname();

static List  wire_lst[] =
{
   "path",                    cct_path,
   "net",                     cct_netname,
   "poly",                    cct_polygon,
   "polygon",                 cct_polygon,
};

#define  SIZ_WIRE_LST     (sizeof(wire_lst) / sizeof(List))

static   int   layer_type();

static List  layer_lst[] =
{
   "type",                    layer_type,
};

#define  SIZ_LAYER_LST     (sizeof(layer_lst) / sizeof(List))


static List  via_lst[] =
{
   "net",                     cct_netname,
};

#define  SIZ_VIA_LST     (sizeof(via_lst) / sizeof(List))

static   int   padstack_shape();

static List  padstack_lst[] =
{
   "shape",                   padstack_shape,
};

#define  SIZ_PADSTACK_LST     (sizeof(padstack_lst) / sizeof(List))

static   int   cct_property();

static List  property_lst[] =
{
   "property",                cct_property,
};

#define  SIZ_PROPERTY_LST     (sizeof(property_lst) / sizeof(List))

static   int   cct_rotate();

static List  rotate_lst[] =
{
   "rotate",                  cct_rotate,
};

#define  SIZ_ROTATE_LST     (sizeof(rotate_lst) / sizeof(List))


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
static   int      get_layerptr(char *l);
static   int      load_cctsettings(const CString fname);
static   int      update_mirrorlayers();
static   int      pad_circle(double w);

/* Globals ****************************************************************/

typedef struct
{
   double w,h;
} CCTPdshape;

typedef  struct
{
   int      cur_status;
   /* Current values that are changed with global  */
   int      cur_widthindex;                        /* Line width     */
   int      cur_layerstackup;
   int      cur_layertype;
   double   cur_rotation;                          // rotation in degree
   int      cur_fill;                              // to fill a polygon;
   int      cur_boundary;                          // start of boundary
} CCTGlobal;
static   CCTGlobal   G;                            /* global const. */

static   CString  cctname;    // 
static   CString  cellname;      // 

static   FILE     *ferr = NULL;
static   FILE     *ifp;                            /* File pointers.    */
static   long     ifp_line = 0;                    /* Current line number. */
static   char     token[MAX_TOKEN];                /* Current token.       */

//

static   char     cur_compname[40];                /* current used component name */
static   char     cur_libname[40];                 /* current used library name */
static   char     cur_pinname[40];                 /* current used pinnumber name */
static   CString  cur_netname;                     /* current used netname name */

/* different pins with same name can not exist */
static   int      Push_tok = FALSE;
static   char     cur_line[MAX_LINE];
static   int      cur_new = TRUE;

static   CCTLayerlist   layerlist[MAX_LAYERS];
static   int         layerlistcnt;

static   CAttribmapArray   attribmaparray;
static   int         attribmapcnt = 0;

typedef CArray<Point2, Point2&> CPolyArray;

static   int         polycnt =0 ;
static   CPolyArray  polyarray;

static   int         ppsignal[10]; // netptr+1 array for layer list
static   int         ppsignalcnt = 0;

static   int         cur_filenum = 0;
static   FileStruct *file = NULL;
static   CCEtoODBDoc *doc;
static   int         display_error = 0;

static   NetStruct   *curnetdata;
static   DataStruct  *curdata; 
static   CPoly       *curpoly;
static   TypeStruct  *curtype;
static   LayerStruct *curlayer;
static   BlockStruct *curblock;
static   double      faktor;
static   int         page_units;

static   CString     LayerRange1, TopElecLayer;
static   CString     LayerRange2, BotElecLayer;

static   CCTPdshape  pd_circle[MAX_PADSHAPE];
static   CCTPdshape  pd_rect[MAX_PADSHAPE];
static   int         pd_circlecnt, pd_rectcnt;

static   char        string_quote = '"';

/****************************************************************************/
/*
*/
static const char *get_attribmap(const char *c)
{
   int      i;
   static   CString  tmp;

   for (i=0;i<attribmapcnt;i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return   attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   tmp = c;
   tmp.MakeUpper();
   return tmp.GetBuffer(0);
}

/****************************************************************************/
/*
   In CCT, a PADSTACK can have the same name as other things
*/
static int clean_padstackprefix()
{
   int   i;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)      continue;
      if (block->getBlockType() == BLOCKTYPE_PADSTACK) 
      {
         // here now check if a block with the original name exist. If not - change the real name and
         // kill the original name
         if (Graph_Block_Exists(doc, block->getOriginalName(), -1) == NULL)
         {
            block->setName(block->getOriginalName());
            block->setOriginalName("");
         }
      }
   }

   return 1;
}

/******************************************************************************
* ReadCCT
*/
void ReadCCT(const char *f, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   file = NULL;
   doc = Doc;
   page_units = pageunits;

   faktor = 1;

   display_error = 0;
   curdata = NULL;
   curtype = NULL;
   curnetdata = NULL;
   curlayer = NULL;
   Push_tok = FALSE;
   cur_new = TRUE;
   layerlistcnt = 0;
   pd_circlecnt = pd_rectcnt = 0;
   G.cur_layerstackup = 0;

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(f,"rt")) == NULL)
   {
     ErrorMessage("Error open file", "Error");
     return;
   }

   CString logFile = GetLogfilePath(CCTERR);
   if ((ferr = fopen(logFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", logFile);
      ErrorMessage(t, "Error");
      return;
   }

   init_all_mem();

   file = Graph_File_Start(f, Type_CCT_Layout);

   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());
   cur_filenum = file->getFileNumber();
   Graph_Level("0","",1);

   CString settingsFilename( getApp().getImportSettingsFilePath("cct.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nCCT Reader: Settings file [%s].\n", settingsFilename);
   getApp().LogMessage(settingsFileMsg);
   load_cctsettings(settingsFilename);

   /* initialize scanning parameters */
   rewind(ifp);
   ifp_line = 0;                    /* Current line number. */

   if (loop_command(start_lst,SIZ_START_LST) < 0)
   {
      // printf big prob.
      ErrorMessage("CCT Design read error", "Error");
   }

   update_mirrorlayers();
   clean_padstackprefix(); 

   free_all_mem();
   fclose(ferr);

   if (display_error)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*
   Call function associated with next tolen.
   Tokens enclosed by () are searched for on the local
   token list.
*/
int go_command(List *tok_lst,int lst_size)
{
   int      i,brk;
   
   if (!get_tok()) 
      return p_error();

   switch((brk = tok_search(brk_lst,SIZ_BRK_LST)))
   {
      case BRK_ROUND:
         if (get_tok() && (i = tok_search(tok_lst,lst_size)) >= 0)
            (*tok_lst[i].function)();
         else
         {
#ifdef _DEBUG
            fprintf(ferr,"DEBUG: Not in Loop - ");
#endif
            fnull(); // unknown command, log and skip

            //fprintf(ferr,"Out of Loop\n");
            //return 1;
         }
      break;
      case BRK_B_ROUND:
         push_tok();
      return 1;
      default:
         return p_error();
      break;
   }

   if (!get_tok()) return p_error();
   if (brk != tok_search(brk_lst,SIZ_BRK_LST) - 1)
      return p_error();
   return 1;
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
      if (go_command(list,size) < 0)
         return -1;

      if (!get_tok())
      {
         if (feof(ifp))
            return 0;
         return p_error();
      }
      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_ROUND)
         repeat = FALSE;
      push_tok();
   }
   return 1;
}


/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next ) endpoint.
*/
static int fnull()
{
   int      brk_count = 0;
   CString  t = token;  // need for debug

#ifdef _DEBUG
   fprintf(ferr,"DEBUG: Token [%s] unknown from %ld to ",token,ifp_line);
   display_error++;
#endif

   while (TRUE)
   {
      if (get_tok())
      {
         t = token;
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_ROUND:
               ++brk_count;
            break;
            case BRK_B_ROUND:
               if (brk_count-- == 0)
               {
                  push_tok();
#ifdef _DEBUG
                  fprintf(ferr,"%ld \n",ifp_line);
#endif
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

   fprintf(ferr," undefined end found at %ld\n", ifp_line);
   return 0;
}

/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next () endpoint.
*/
static int fskip()
{
   int      brk_count = 0;

/*
   fprintf(ferr,"Token [%s] at %ld skipped\n",token,ifp_line);
   display_error++;
*/
   while (TRUE)
   {
     if (get_tok())
      {
         switch (tok_search(brk_lst,SIZ_BRK_LST))
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

/****************************************************************************/
/*
*/
static int start_cct()
{
   if (!get_tok())   return p_error();  
   cctname = token;

   return loop_command(cct_lst,SIZ_CCT_LST);
}

/****************************************************************************/
/*
*/
static int cct_resolution()
{
   CString  res;
   long     u;

   if (!get_tok())   return p_error();  
   res = token;
   if (!get_tok())   return p_error();  
   u = atol(token);  // not needed

   // here calc factor
   if (!STRCMPI(res,"Inch"))
   {
      faktor = Units_Factor(UNIT_INCHES, page_units); // default units to page units
   }
   else
   if (!STRCMPI(res,"Mil"))
   {
      faktor = Units_Factor(UNIT_MILS, page_units);   // default units to page units
   }
   else
   if (!STRCMPI(res,"mm"))
   {
      faktor = Units_Factor(UNIT_MM, page_units);  // default units to page units
   }
   else
   {
      // unknown unit
      CString t;
      t.Format("Unknown Units [%s]",res);
      ErrorMessage(t, "Unit Error", MB_OK | MB_ICONHAND);
      return -1;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_unit()
{
   CString  res;

   if (!get_tok())   return p_error();  
   res = token;

   // here calc factor
   if (!STRCMPI(res,"Inch"))
   {
      faktor = Units_Factor(UNIT_INCHES, page_units); // default units to page units
   }
   else
   if (!STRCMPI(res,"Mil"))
   {
      faktor = Units_Factor(UNIT_MILS, page_units);   // default units to page units
   }
   else
   if (!STRCMPI(res,"mm"))
   {
      faktor = Units_Factor(UNIT_MM, page_units);  // default units to page units
   }
   else
   {
      // unknown unit
      CString t;
      t.Format("Unknown Units [%s]",res);
      ErrorMessage(t, "Unit Error", MB_OK | MB_ICONHAND);
      return -1;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_image()
{
   if (!get_tok())   return p_error();  
   cellname = token;

   G.cur_status = CCT_IMAGE;

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, -1, 0L);// declares and begins defining a block
   c->setBlockType(BLOCKTYPE_PCBCOMPONENT);
   loop_command(image_lst,SIZ_IMAGE_LST);
   Graph_Block_Off();
   
   G.cur_status = CCT_UNKNOWN;

   return 1;
}

/****************************************************************************/
/*
  (string_quote ")
  cur_line  has the complete line
*/
static int cct_string_quote()
{
   // do not use get_tok, because the starting quote looks for an ending quote.
   //if (!get_tok())   return p_error();  

   char  *lp, *t;

   t = _strlwr(cur_line);
   lp = strstr(t,"string_quote");

   if (lp)
   {
      lp += strlen("string_quote"); // move ptr to end of string_quote

      while (*lp && isspace(*lp))   lp++; 

      string_quote = *lp;
      lp++;
   }
   strcpy(cur_line, lp);   
   cur_new = TRUE;

   return 1;
}

/****************************************************************************/
/*
*/
static int cct_component()
{
   if (!get_tok())   return p_error();  
   cellname = token;

   // just make shure it is there.
   Graph_Block_On(GBO_APPEND, cellname, -1, 0L);// declares and begins defining a block
   Graph_Block_Off();

   loop_command(component_lst,SIZ_COMPONENT_LST);
   
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_net()
{
   if (!get_tok())   return p_error();  
   cur_netname = token;

   add_net(file,token);
   loop_command(net_lst,SIZ_NET_LST);
   
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_netname()
{
   if (!get_tok())   return p_error();  
   cur_netname = token;
   return 1;
}

/****************************************************************************/
/*
*/
static int layer_type()
{
   CString  l;
   if (!get_tok())   return p_error();  
   l = token;
   G.cur_layertype = 0;
   if (STRCMPI(l,"SIGNAL"))
   {
      G.cur_layertype = LAYTYPE_SIGNAL;
   }
   else
   if (STRCMPI(l,"MIXED"))
   {
      G.cur_layertype = LAYTYPE_SIGNAL;
   }
   else
   if (STRCMPI(l,"POWER"))
   {
      G.cur_layertype = LAYTYPE_POWERNEG;
   }
   else
   {
      fprintf(ferr,"Unknown layertype [%s]\n",l);
      display_error++;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cct_wire()
{
   curdata = NULL;
   cur_netname = "";
   loop_command(wire_lst,SIZ_WIRE_LST);
   // here need to assign netname
   if (curdata && strlen(cur_netname))
   {
      curdata->setGraphicClass(GR_CLASS_ETCH);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
         VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
   }
   curdata = NULL;
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_via()
{
   CString  ps;
   double   x,y;

   if (!get_tok())   return p_error();  
   ps.Format("PST_%s", token);      // can have the same name as a image

   if (!get_tok())   return p_error();  
   x = cnv_unit(token);
   if (!get_tok())   return p_error();  
   y = cnv_unit(token);

   cur_netname = "";
   loop_command(via_lst,SIZ_VIA_LST);

   DataStruct *d = Graph_Block_Reference(ps, NULL, -1, x,y,
                           DegToRad(0), 0 , 1.0,Graph_Level("0","",1), TRUE);
   d->getInsert()->setInsertType(insertTypeVia);
   if (strlen(cur_netname))
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING,
            cur_netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_padstack()
{
   if (!get_tok())   return p_error();  
   cellname.Format("PST_%s", token);      // can have the same name as a image

   BlockStruct *c = Graph_Block_On(GBO_APPEND, cellname, -1, 0L);// declares and begins defining a block
   c->setBlockType(BLOCKTYPE_PADSTACK);
   c->setOriginalName(token);
   loop_command(padstack_lst,SIZ_PADSTACK_LST);
   Graph_Block_Off();
   
   return 1;
}

/****************************************************************************/
/*
*/
static int image_side()
{
   if (!get_tok())   return p_error();  //side

   if (!STRCMPI(token, "back"))
   {
      Graph_Block_Off();   // old one block off

      CString cname;
      cname.Format("%s (back)", cellname);
      BlockStruct *c = Graph_Block_On(GBO_APPEND, cname, -1, 0L);// declares and begins defining a block
      c->setBlockType(BLOCKTYPE_PCBCOMPONENT);
      cellname = cname;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int image_pin()
{
   CString  ps, pn;
   double   x,y;

   if (!get_tok())   return p_error();  //Padstack
   ps = token;
   ps.Format("PST_%s", token);      // can have the same name as a image

   // just make shure it is there.
   Graph_Block_On(GBO_APPEND, ps, -1, 0L);// declares and begins defining a block
   Graph_Block_Off();

   if (!get_tok())   return p_error();  // Pinname, number
   pn = token;
   if (!get_tok())   return p_error();  // x
   x = cnv_unit(token);
   if (!get_tok())   return p_error();  // y
   y = cnv_unit(token);
   
   G.cur_rotation = 0;
   // here is a rotate option
   loop_command(rotate_lst,SIZ_ROTATE_LST);

   // here place
   curdata = Graph_Block_Reference(ps, pn, 0, x, y, DegToRad(G.cur_rotation), 0 , 1.0, 
      Graph_Level("0","",1), TRUE);
   curdata->getInsert()->setInsertType(insertTypePin);

   // do pinnr here
   if (strlen(pn))
   {
      int   pnr = atoi(pn);
      doc->SetAttrib(&curdata->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, 0),
         VT_INTEGER, &pnr, SA_OVERWRITE, NULL); // x, y, rot, height
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int image_outline()
{
   G.cur_fill = FALSE;
   loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   return 1;
}

/****************************************************************************/
/*
   Compnane
   x
   y
   back / front
   angle in degree

*/
static int component_place()
{
   CString  cn;
   double   x, y, rot;
   int      mir;

   if (!get_tok())   return p_error();  // Compname
   cn = token;

   if (!get_tok())   return p_error();  // x
   x = cnv_unit(token);
   if (!get_tok())   return p_error();  // y
   y = cnv_unit(token);
   
   if (!get_tok())   return p_error();  // mir
   mir = !STRCMPI(token,"back");

   if (!get_tok())   return p_error();  // angle
   rot = atof(token);

   int bottom = FALSE;
   if (mir)
   {
      // now check if there is a 
      CString cname;
      cname.Format("%s (back)", cellname);
      if (Graph_Block_Exists(doc, cname, -1) == NULL)
      {
         cellname = cname;
         bottom = TRUE;
      }
   }

   // here I know, the block exist
   curdata = Graph_Block_Reference(cellname, cn, 0, x, y, DegToRad(rot) , mir , 1.0, 
      Graph_Level("0","",1), TRUE);
   curdata->getInsert()->setInsertType(insertTypePcbComponent);

   if (bottom)
   {
      curdata->getInsert()->setMirrorFlags(0);
      curdata->getInsert()->setPlacedBottom(true);
   }

// component property
   loop_command(property_lst,SIZ_PROPERTY_LST);

   return 1;
}

/****************************************************************************/
/*
*/
static int geometryElementSet()
{
   loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_boundary()
{
   G.cur_boundary = TRUE;
   loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   G.cur_boundary = FALSE;

   return 1;
}

/****************************************************************************/
/*
*/
static int cct_rotate()
{
   if (!get_tok())   return p_error();  
   G.cur_rotation = atof(token);
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_layer()
{
   CString  layername;
   int      li;
   
   if (!get_tok())   return p_error();  
   layername = token;
   layername.MakeUpper();

   G.cur_layerstackup++;
   loop_command(layer_lst,SIZ_LAYER_LST);
   
   li = Graph_Level(layername,"",0);

   curlayer = doc->FindLayer(li);
   curlayer->setLayerType(G.cur_layertype);
   curlayer->setElectricalStackNumber(G.cur_layerstackup);

   return 1;
}

/****************************************************************************/
/*
*/
static int padstack_shape()
{

   G.cur_status = CCT_PADSHAPE;
   G.cur_fill = TRUE;
   loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   G.cur_fill = FALSE;
   G.cur_status = CCT_UNKNOWN;
   
   return 1;
}

/****************************************************************************/
/*
   (key val)
*/
static int cct_property()
{
   int      brk_count = 0;

   while (TRUE)
   {
     if (get_tok())
      {
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_ROUND:
            {
               CString  key, val;
               ++brk_count;
               if (!get_tok())   return p_error();  
               key = token;
               key.TrimLeft();
               key.TrimRight();
               key.MakeUpper();

               if (!get_tok())   return p_error();  
               val = token;
               val.TrimLeft();
               val.TrimRight();

               if (curdata)
               {
                  doc->SetUnknownAttrib(&curdata->getAttributesRef(), get_attribmap(key), val,
                     SA_APPEND, NULL); //
               }
            }
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

/****************************************************************************/
/*
*/
static int cct_keepout()
{

   loop_command(geometryElementSet_lst,SIZ_GEOMETRYELEMENTSET_LST);
   
   return 1;
}

/****************************************************************************/
/*
*/
static int cct_width()
{
   double w;
   int   err;

   if (!get_tok())   return p_error();  //
   w = cnv_unit(token);

   if (w == 0)
      G.cur_widthindex = 0;
   else
      G.cur_widthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   return 1;
}

/****************************************************************************/
/*
*/
static int cct_path()
{
   int      i, lindex;
   Point2   p;
   CString  l, t;

   polyarray.RemoveAll();         
   polycnt = 0;

   if (!get_tok())   return p_error();  // layer
   l = token;
   l.MakeUpper();    // layernames are not case sensitive

   cct_width();

   while (TRUE)
   {
      if (!get_tok())   return p_error();  // x
      t = token;
      if (!STRCMPI(token,")"))
      {
         push_tok();
         break;
      }
      p.x = cnv_unit(token);
      if (!get_tok())   return p_error();  // y
      p.y = cnv_unit(token);
      p.bulge = 0;
      polyarray.SetAtGrow(polycnt,p);
      polycnt++;
   }

   if (polycnt == 0) return 1;

   int close = FALSE;
   if (G.cur_boundary)
   {
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
      close = TRUE;
   }

   lindex = Graph_Level(l,"",0);

   curdata = Graph_PolyStruct(lindex,0L,0);
   Graph_Poly(NULL,G.cur_widthindex, 0, 0, close); // fillflag , negative, closed

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
static int net_pins()
{
   CString  t;
   CString  compname, pinname;

   while (TRUE)
   {
      if (!get_tok())   return p_error();  // x
      t = token;
      if (!STRCMPI(token,")"))
      {
         push_tok();
         break;
      }
      compname = strtok(token,"-");
      pinname = strtok(NULL,"\n");
      NetStruct *n = add_net(file,cur_netname);
      add_comppin(file, n, compname, pinname);
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int cct_polygon()
{
   int      i, lindex;
   Point2   p;
   CString  l, t;

   polyarray.RemoveAll();         
   polycnt = 0;

   if (!get_tok())   return p_error();  // layer
   l = token;
   l.MakeUpper(); // layernames are not case sensitive

   switch (G.cur_status)
   {
      case CCT_IMAGE:
         // on image the component outline ia also called signal
         if (!l.CompareNoCase("SIGNAL"))
            l = "OUTLINE_TOP";
      break;
      default:
      break;
   }

   cct_width();

   while (TRUE)
   {
      if (!get_tok())   return p_error();  // x
      t = token;
      if (!STRCMPI(token,")"))
      {
         push_tok();
         break;
      }
      p.x = cnv_unit(token);
      if (!get_tok())   return p_error();  // y
      p.y = cnv_unit(token);
      p.bulge = 0;
      polyarray.SetAtGrow(polycnt,p);
      polycnt++;
   }

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

   lindex = Graph_Level(l,"",0);

   Graph_PolyStruct(lindex,0L,0);
   Graph_Poly(NULL,G.cur_widthindex, G.cur_fill, 0, 1);  // fillflag , negative, closed

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
   layer 
   diam
   x
   y
*/
static int cct_circle()
{
   int      p_ptr, lindex;
   Point2   p;
   double   diam;
   CString  l, pshapename;

   polyarray.RemoveAll();         
   polycnt = 0;

   if (!get_tok())   return p_error();  // layer
   l = token;
   l.MakeUpper(); // layernames are not case sensitive

   if (!get_tok())   return p_error();  // layer
   diam = cnv_unit(token);

   if (!get_tok())   return p_error();  // x
   p.x = cnv_unit(token);
   if (!get_tok())   return p_error();  // y
   p.y = cnv_unit(token);

   lindex = Graph_Level(l,"",0);
 
   if ((p_ptr = pad_circle(diam)) < 0)
   {
      pshapename.Format("PS_CIRC_%d",pd_circlecnt-1);
      int   err;
      Graph_Aperture(pshapename, T_ROUND, diam, 0.0, 0.0, 0.0, 0.0, 0, 
            BL_APERTURE, FALSE, &err);
   }
   else
      pshapename.Format("PS_CIRC_%d",p_ptr);

   Graph_Block_Reference(pshapename, NULL, 0, p.x, p.y, 0.0, 0 , 1.0, lindex, TRUE);

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int pad_circle(double w)
{
   int   i;
   for (i=0;i<pd_circlecnt;i++)
   {
      if (pd_circle[i].w == w)
         return i;
   }

   if (pd_circlecnt < MAX_PADSHAPE)
   {
      pd_circle[pd_circlecnt].w = w;
      pd_circlecnt++;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pad_rect(double w, double h)
{
   int   i;
   for (i=0;i<pd_rectcnt;i++)
   {
      if (pd_rect[i].w == w && pd_rect[i].h == h)
         return i;
   }

   if (pd_rectcnt < MAX_PADSHAPE)
   {
      pd_rect[pd_rectcnt].w = w;
      pd_rect[pd_rectcnt].h = h;
      pd_rectcnt++;
   }

   return -1;
}


/****************************************************************************/
/*
*/
static int cct_rectangle()
{
   CString  l;
   double   x1,y1,x2,y2;
   int      lindex;

   if (!get_tok())   return p_error();  // layer
   l = token;
   l.MakeUpper();

   if (!get_tok())   return p_error();  // llx
   x1 = cnv_unit(token);
   if (!get_tok())   return p_error();  // lly
   y1 = cnv_unit(token);
   if (!get_tok())   return p_error();  // trx
   x2 = cnv_unit(token);
   if (!get_tok())   return p_error();  // try
   y2 = cnv_unit(token);

   lindex = Graph_Level(l,"",0);
   curdata = Graph_PolyStruct(lindex,0L,0);

   int   fill = 0;
   if (G.cur_status == CCT_PADSHAPE)   fill = TRUE;

   curpoly = Graph_Poly(NULL, 0, fill, 0, 1);   // fillflag , negative, closed

   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);

   return 1;
}

/****************************************************************************/
/*
*/
static int cct_structure()
{
   return loop_command(structure_lst,SIZ_STRUCTURE_LST);
}

/****************************************************************************/
/*
*/
static int cct_library()
{
   return loop_command(library_lst,SIZ_LIBRARY_LST);
}

/****************************************************************************/
/*
*/
static int cct_parser()
{
   return loop_command(parser_lst,SIZ_PARSER_LST);
}

/****************************************************************************/
/*
*/
static int cct_network()
{
   return loop_command(network_lst,SIZ_NETWORK_LST);
}

/****************************************************************************/
/*
*/
static int cct_wiring()
{
   return loop_command(wiring_lst,SIZ_WIRING_LST);
}

/****************************************************************************/
/*
*/
static int cct_placement()
{
   return loop_command(placement_lst,SIZ_PLACEMENT_LST);
}

/****************************************************************************/

/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(List *tok_lst,int tok_size)
{
   int      i;

   for (i = 0; i < tok_size; ++i)
      if (!STRCMPI(token,tok_lst[i].token))
         return(i);
   return(-1);
}

/****************************************************************************/
/*
   Get a token from the input file.
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
   int      token_name = FALSE;

   if (newlp)  cp = lp;

   for (; isspace(*cp) && *cp != '\0'; ++cp) ;

   if (*cp == '\0')
   {
      return 0;
   }
   else
   if (*cp == '(' || *cp == ')')
   {
      token[i++] = *(cp++);
   }
   else
   if (*cp == string_quote)
   {
      token[i] = '\0';
      token_name = TRUE;
      for (++cp; *cp != string_quote; ++cp, ++i)
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
         }
         else
            token[i] = *cp;
      }
      ++cp;
   }
   else
   {
      for (; !isspace(*cp) && *cp != string_quote &&
                           *cp != '(' && *cp != ')' &&
                           *cp != '\0'; ++cp, ++i)
      {
         if (*cp == '\\')
            ++cp;
         token[i] = *cp;
      }
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
static int get_line(char *cp,int size)
{
   CString  l;

   do
   {
      if (fgets(cp,size,ifp) == NULL)
      {
         if (!feof(ifp))
         {
            ErrorMessage("Read Error","Fatal CCT Parse Error", MB_OK | MB_ICONHAND);
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
         if (cp[strlen(cp)-1] != '\n') // this is allowed - the last character could be the end of the file.
         {
            //printf(" No end of line found at %ld\n",ifp_line);
            //fprintf(ferr," No end of line found at %ld\n",ifp_line);
            // return (FALSE);
         }
      }
      ++ifp_line;

      // check if line is a remark line.
      l = cp;
      l.TrimLeft();
   } while (strlen(l) == 0 || l[0] == '#');  // continue read a line as long as the line was empty or a remark.
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

   polyarray.SetSize(100,100);
   polycnt = 0;

   memset(&G,0,sizeof(CCTGlobal));

   attribmaparray.SetSize(100,100);
   attribmapcnt = 0;

   return;
}

void free_all_mem()
{
   int   i;

   for (i=0;i<attribmapcnt;i++)
   {
      delete attribmaparray[i];
   }
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

}

/****************************************************************************/
/*
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
static int get_layerptr(char *l)
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

/****************************************************************************/
/*
*/
static int load_cctsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      char tmp[80];
      sprintf(tmp,"File [%s] not found",fname);
      ErrorMessage(tmp, "CCT Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".ATTRIBMAP"))
         {
            CString  a1,a2;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            a1 = lp;
            a1.MakeUpper();
            a1.TrimLeft();
            a1.TrimRight();
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            a2 = lp;
            a2.MakeUpper();
            a2.TrimLeft();
            a2.TrimRight();

            CCTAttribmap   *c = new CCTAttribmap;
            attribmaparray.SetAtGrow(attribmapcnt,c);

            c->attrib = a1;
            c->mapattrib = a2;
            attribmapcnt++;
            
         }
         else
         if (!STRICMP(lp,".MIRRORLAYER"))
         {
            int   l1,l2;
            CString  lay1,lay2;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            lay1 = lp;
            l1 = get_layerptr(lp);
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            lay2 = lp;
            l2 = get_layerptr(lp);
            layerlist[l1].mirrorindex = l2;
            layerlist[l1].name = lay1; // need to default a name
            // there is not neccesarily a mirror for L51 etc..
            layerlist[l2].mirrorindex = l1;
            layerlist[l2].name = lay2;
         }
      }
   }

   fclose(fp);
   return 1;
}

/****************************************************************************/
/*
*/
static int update_mirrorlayers()
{
   int      i;
   CString  top, bottom;

   LayerStruct *l;
   top = "";
   bottom = "";

   for (i=0; i< doc->getMaxLayerIndex(); i++)  
   {
      l = doc->getLayerArray()[i];
      if (l == NULL) continue; // could have been deleted.

      if (l->getElectricalStackNumber() == 1)
      {
         top = l->getName();
         l->setLayerType(LAYTYPE_SIGNAL_TOP);
      }
      else
      if (l->getElectricalStackNumber() == G.cur_layerstackup)
      {
         bottom= l->getName();
         l->setLayerType(LAYTYPE_SIGNAL_BOT);
      }
      else
      if (l->getElectricalStackNumber())
      {
         l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
   }

   if (strlen(top) && strlen(bottom))
   {
      Graph_Level_Mirror(top, bottom, "");
   }

   // here component outline
   Graph_Level_Mirror("OUTLINE_TOP", "OUTLINE_BOT", "");

   int layernr = Graph_Level("OUTLINE_TOP","",0);
   l = doc->FindLayer(layernr);
   l->setComment("Component_Outline_Top");
   l->setLayerType(LAYTYPE_SILK_TOP);
   
   layernr = Graph_Level("OUTLINE_BOT","",0);
   l = doc->FindLayer(layernr);
   l->setComment("Component_Outline_Bottom");
   l->setLayerType(LAYTYPE_SILK_BOTTOM);

   // board outline
   layernr = Graph_Level("PCB","",0);
   l = doc->FindLayer(layernr);
   l->setComment("Board Outline");
   l->setLayerType(LAYTYPE_BOARD_OUTLINE);

   return 1;
}

/*Ende **********************************************************************/



