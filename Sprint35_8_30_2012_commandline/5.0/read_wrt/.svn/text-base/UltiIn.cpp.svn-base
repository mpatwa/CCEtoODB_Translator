// $Header: /CAMCAD/4.3/read_wrt/UltiIn.cpp 6     8/12/03 7:35p Kurt Van Ness $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
          
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

static CProgressDlg *progress;

/******************************************************************************
* ReadULTIBOARD
*/
void ReadULTIBOARD(const char *f, CCEtoODBDoc *Doc, FormatStruct *Format,  int pageunits)
{

   ErrorMessage("ULTIBOARD reader not yet implemented!");
   return;
}

#ifdef ULTI

/****************************************************************************/
/*
   Converts ULTIMATE ULTIBOARD files to database files.

   Newly created entities:

   Layer =  SHAPENAME;
   Layer =  LIBOUTLINE;
   Layer =  COMPNAME;
   Layer =  PADTOP;
   Layer =  PADINNER;
   Layer =  PADBOTTOM;

   Silkwidthcode always code 0

   Padlayers can not be modified in NETLIST.

   Pad have to be defined on all layers and than will be masked 
   with the library Pad layer setting.

   All pad technology is defined in mil.
   NET for TEXT created.
*/
/****************************************************************************/

#include "common.h"

int   fnull();

/* History
   Version  Date        Remark
   1        19-Oct-93   First Version
   2        26-Oct-94   Little Bug fixed (see "Alex 26.10.94")
   3        27-Feb-95   Poly Fill and Void fixed.
   4        23-Oct-95   There are segments less 1 mil. If TE_SELECT != 3
                        eliminate these segments.
                        GOODNBOARD implemented.
   5        30-Nov-95   Update to Version 4 8
   6        1-Nov-96    Resquence the layer phynsnr.
   7        9-Jan-97    Layer Stackup now in ulti.tec.
   8        25-Mar-97   Pad radius errors fixed.
*/

#define  VERSION   8

/* Defines *****************************************************************/

#define  BOARDLAYER     "BOARDLAYER"
#define  LIBOUTLINE     "LIBOUTLINE"
#define  SHAPENAME      "SHAPENAME"
#define  COMPNAME       "COMPNAME"
#define  TYPENAME       "TYPENAME"
#define  PADTOP         "PADTOP"
#define  PADINNER       "PADINNER"
#define  PADBOTTOM      "PADBOTTOM"
#define  TEXTNET        "$*TEXT*$"
#define  NONET          "$*NONET*$"
#define  BRDNET         "$*BRDNET*$"

#define  TRUE           1
#define  FALSE          0

#define  ULTIUNIT       1200
static   int ULTISUBVERSION = 2;

#define  ULTIVERSION    4
#define  EDFVERSION     1
#define  EDFREVISION    2

#define  FNULL          fnull

#define  MAX_TEXT       128
#define  MAX_LINE       127         /* Max line length.           */
#define  MAX_DEF_PAD    256
#define  MAX_DEFPOLY    2000        /* Poly line def yes there are a lot! */
#define  MAX_NETL       4000

#define  INNER          0
#define  TOP            1
#define  BOTTOM         2

char  *elemlist[] =
{
   "*P",
   "*V",
   "*E",
   "*Q",
   "*R",
   "*D",
   "*TP",
   "*TT",
   "*TC",
   "*TD",
   "*T0",
   "*T1",
   "*T2",
   "*TS",
   "*S",
   "*C",
   "*NL",   /* this must be before netlist */
   "*NP",   /* see read_element */
   "*NT",
   "*NA",
   "*NC",
   "*NV",
   "*N",
   "*X",
};

#define  UNKNOWN           0
#define  HEADER            1
#define  UVERSION          2
#define  UEXTERNAL         3
#define  ULAYER            4
#define  UOPTIONLIST       5
#define  UDIRECTION        6
#define  TPSETTING         7
#define  TTSETTING         8
#define  TCSETTING         9
#define  TDSETTING         10
#define  T0SETTING         11
#define  T1SETTING         12
#define  T2SETTING         13
#define  TSSETTING         14
#define  SHAPEDEF          15
#define  COMPONENT         16
#define  NETLAYER          17
#define  NETPOLY           18
#define  NETTRACE          19
#define  NETARC            20
#define  NETCON            21
#define  NETVIA            22
#define  NETNAME           23
#define  TEXTELEMENT       24

#define  SIZ_ELEM_LIST     24

/* Error messages **********************************************************/

#define  ERR_READ_INPUT             "Error reading UltiBoard file.\n"
#define  ERR_PARSE                  "Error parsing UltiBoard file.\n"
#define  ERR_MEMORY                 "Error not enough memory.\n"

#define  ULTITEC        "ulti.tec"     /* optional pad setup file */

/* Structures **************************************************************/
typedef struct
{
   double   x,y;
} Coor;

typedef struct
{
   int      attr;
   int      layer;
} Ldef;

typedef struct
{
  int x1,x2,y;
  int rad,clear;
} Upaddef;

typedef struct
{
   Upaddef  pad[3];
} Pdef;

typedef struct
{
   int      layer; /* 1 = comp , 2 = solder else all */
   int      code;
   int      rot;
} Paddef;

typedef  struct
{
   int   x,y;
} Line;

typedef  struct
{
  int    type; 
  int    x,y;
  char   *name;
  int    rot;
  long   layerset;
} Pin;

typedef  struct
{
  int    x,y;
} Arc;

typedef  struct
{
  char   *name;
} Netl;

/* Globals *****************************************************************/


static   int      def_smd[MAX_DEF_PAD];       /* Width code for SMD Pads */
static   int      def_smdcnt = 0;

static   Coor     *def_poly;                  /* Array for polylines. */
static   int      def_polycnt = 0;

static   Pdef     def_pad[MAX_DEF_PAD];       /* Array of pad stacks.   layer comp solder inner */
static   int      def_drill[MAX_DEF_PAD];

static   Paddef   pad_def[MAX_DEF_PAD];
static   int      pad_defcnt = 0;

static   FILE     *ifp;                            /* File pointers.*/
static   long     ifp_line = 1;                    /* Current line number. */

static   char  cur_line[MAX_LINE];
static   int   cur_new = TRUE;
static   char  token[MAX_LINE];                 /* Current token.       */
static   int   Push_tok = FALSE;                /* no token pushed. */
static   char  *cp;

static   int   printtoken = FALSE;        /* test output for read token */
static   int   via_code;
static   char  netname[MAX_TEXT];
static   int   TE_SELECT = 0;

static   Netl  *netl;
static   int   netlcnt;

static   int   process_void = FALSE;
static   int   void_message_done = FALSE; // display erorr message for NP section only once.
static   int   phys_layer[33];  // maximum number of Phys Layers in ULTIBOARD.

/****************************************************************************/
/*
   Function definition
*/
static   char  toupchar(char );
static double ult_2_inch(int);
static int ulti2mil(int );
static int tokcmpi(char * , char * );
static int out_pad(char * , int , int , int );
static int update_paddef(int , int , int );
static void push_tok(void);
static int init_global(void);
static int read_element(void);
static int read_component(void);
static int read_arclist(char * );
static int read_arc(char * , int );
static int read_file(void);
static int read_linelist(char * );
static int read_netarc(void);
static int read_netcon(void);
static int read_netname(void);
static int read_netpoly(void);
static int read_nettrace(void);
static int read_netvia(void);
static int read_pad(int );
static int read_padlist(char * );
static int read_polytrace(void);
static int read_shape(void);
static int read_text(void);
static int read_void(void);
static int write_padstacks(void);
static void swap(int * , int * );
static int check_semicolon(void);
static void get_rot(int , int * , int * );
static int get_line(char * , int );
static void get_next_line(void);
static int get_next(char * , int );
static int get_tok(void);
static char * get_word(char * , int );
static int get_widthint(int );
static int skip_line(void);
static void p_error(void);
static int pad_is_round(int,int);

/****************************************************************************/
/*
*/
load_ulti(FILE *fp)
{
   DbFlag   flg;
   int      i;

   ifp = fp;   /* set filepointer global. */

   if ((def_poly = (Coor *) malloc(MAX_DEFPOLY * sizeof(Coor))) == NULL)
   {
      printf(ERR_MEMORY);
      exit(1);
   }

   /* init globals */
   init_global();

   update_ultitec();

   /* Parse input file.    */
   cur_new = TRUE;
   cur_line[0] = '\0';
   Push_tok = FALSE;
   printf("\t Reading EDF file\r");
   if (read_file() < 0)
      f_exit();
   printf("\n");

   flg = NE_2DLINES;
   L_NetAttr(TEXTNET,inch_2_dbunit(0.0),0,0,0,flg);
   L_NetAttr(NONET,inch_2_dbunit(0.0),0,0,0,flg);

   write_padstacks();
   free(def_poly);

   flg = LY_COMPONENT;
   L_LAttr(PADTOP,flg);
   flg = LY_ALLINNER;
   L_LAttr(PADINNER,flg);
   flg = LY_SOLDER;
   L_LAttr(PADBOTTOM,flg);
   flg = LY_SILK;
   L_LAttr(LIBOUTLINE,flg);
   flg = LY_ALL;
   L_LAttr(BOARDLAYER,flg);

   for (i=0;i<netlcnt;i++)
   {
      free(netl[i].name);
   }
   free(netl);

   return(0);
}

/****************************************************************************/
/*
   read complete file
*/
read_file()
{
   int   done = FALSE;
   int   code;
   int   nonet;
   int   pad_update = FALSE;

   while (!done)
   {
      printf("\r %ld",ifp_line);
      switch (read_element())
      {
         case -2:
            /* printf("End of file found\n"); */
            done = TRUE;
         break;
         case HEADER:
            if (!get_tok())   p_error(); /* EXTERN */
            if (!get_tok())   p_error(); /* ROUTE */
            if (!get_tok())   p_error(); /* grid */
            L_TRoutgrid(inch_2_dbunit(ult_2_inch(atoi(token))));
         break;
         case UVERSION:
            if (!get_tok())   p_error(); /* version */
            if (ULTIVERSION != atoi(token))
            {
               printf(" Unsupported EDF file version [%d] -> proceed with caution\n",
                        atoi(token));
            }
            if (atoi(token) > ULTIVERSION)
            {
               printf("UltiBoard Version [%d] not supported\n",atoi(token));
               f_exit();
            }
            if (!get_tok())   p_error(); /* revision */
            ULTISUBVERSION = atoi(token) / 10;
         break;
         case  UEXTERNAL:
            if (!get_tok())   p_error(); /* version */
            if (atoi(token) != EDFVERSION) 
            {
               printf("File Version [%d] not supported\n",atoi(token));
               f_exit();
            }
            if (!get_tok())   p_error(); /* revision */
            if (atoi(token) != EDFREVISION) 
            {
               printf("File Revision [%d] not supported -> need *E 1 2\n",atoi(token));
               f_exit();
            }
         break;
         case  ULAYER:
         {
            DbFlag flg;
            int    i,num;
            char   tmp[10];

            if (!get_tok())   p_error(); /* layer */
            num = atoi(token);
            L_TLayers(num);
            flg = LY_COMPONENT | LY_ELECTRICAL;
            L_LAttr("1",flg);
            flg = LY_SOLDER | LY_ELECTRICAL;
            L_LAttr("2",flg);

            L_LPhysnr("1",phys_layer[1]);
            L_LPhysnr("2",num);
            for (i=3;i<=num;i++)
            {
               int physnr = phys_layer[i];
               sprintf(tmp,"%d",i);
               flg = LY_INNER | LY_ELECTRICAL;
               L_LAttr(tmp,flg);
               L_LPhysnr(tmp,physnr);
            }
            L_TAttr(TE_PHYSNUMBER);
            skip_line();
         }
         break;
         case  UOPTIONLIST:
            if (!get_tok())   p_error(); /* viacode */
            via_code = atoi(token);
            if (!get_tok())   p_error(); /* viashift */
            if (!get_tok())   p_error(); /* viarot */
            if (!get_tok())   p_error(); /* useviashift */
            done = skip_line();
         break;
         case  UDIRECTION:
            done = skip_line();
         break;
         case  TPSETTING:
            if (!get_tok())   p_error(); /* default layer map */
         break;
         case  TTSETTING:
            if (!get_tok())   p_error(); /* code */
            code = atoi(token);
            if (!get_tok())   p_error(); /* width */
            L_Width(inch_2_dbunit(ult_2_inch(atoi(token))),code);
            if (!get_tok())   p_error(); /* clearance */
         break;
         case  TCSETTING:
            if (!get_tok())   p_error(); /* drill tolerance */
            if (ULTISUBVERSION >= 8)
            {
               if (!get_tok())   p_error(); /* BOARD outline clearance */
            }
         break;
         case  TDSETTING:
               if (!get_tok())   p_error(); /* drill code */
               code = atoi(token);
               if (!get_tok())   p_error(); /* drill radius */
               def_drill[code] = atoi(token);
         break;
         case  T0SETTING: /* inner */
               read_pad(INNER);
         break;
         case  T1SETTING: /* TOP */
               read_pad(TOP);
         break;
         case  T2SETTING:
               read_pad(BOTTOM); /* BOTTOM */
         break;
         case  TSSETTING:
               /* there is only one */
               if (!pad_update)  adjust_pad();
               pad_update = TRUE;
               if (!get_tok())   p_error(); /* solder wave dir */
               if (!get_tok())   p_error(); /* top */
               if (!get_tok())   p_error(); /* bottom */
         break;
         case SHAPEDEF:
            read_shape();
         break;
         case COMPONENT:
            read_component();
         break;
         case  NETNAME:
            if (!read_netname())
            {  /* empty net ??? */
               if (!get_tok())   p_error();
            }
            if (!strcmp(netname,NONET))
               nonet = TRUE;
            else
               nonet = FALSE;
         break;
         case  NETTRACE:
            read_nettrace();
         break;
         case  NETLAYER:
            if (!get_tok())   p_error(); /* skip line */
         break;
         case  NETPOLY:
            read_netpoly();
         break;
         case  NETARC:
            read_netarc();
         break;
         case  NETCON:
            if (!nonet)
               read_netcon();
            else
            {
               done = skip_line();
            }
         break;
         case  NETVIA:
            read_netvia();
         break;
         case  TEXTELEMENT:
            done = read_text();
         break;
         default:
            printf("Unknown element type: %s at %ld\n",token,ifp_line);
         done = skip_line();
         break;
      } /* switch */
   } /* while loop */
   return 1;
}

/****************************************************************************/
/*
   read pad record
*/
read_pad(int n)
{
   int   code,x1,x2,y,rad,clear;

   if (!get_tok())   p_error(); /* code */
   code = atoi(token);
   if (code == via_code)
      update_paddef(code,0,0xf);

   if (!get_tok())   p_error(); /* x1 */
   x1 = atoi(token);
   if (!get_tok())   p_error(); /* x2 */
   x2 = atoi(token);
   if (!get_tok())   p_error(); /* y */
   y = atoi(token);
   if (!get_tok())   p_error(); /* radius */
   rad = atoi(token);
   if (!get_tok())   p_error(); /* clear */
   clear = atoi(token);
   skip_line();
   if (code < MAX_DEF_PAD)
   {
      if (y > 0)
      {
         def_pad[code].pad[n].x1 = x1;
         def_pad[code].pad[n].x2 = x2;
         def_pad[code].pad[n].y = y;
         def_pad[code].pad[n].rad = rad;
         def_pad[code].pad[n].clear = clear;
      }
   }
   else
   {
      printf("Pad code %d out of range at %ld\n",code,ifp_line);
      f_exit();
   }
   return 1;
}

/****************************************************************************/
/*
   read attribute list
   read a list until ;
*/
read_attributelist()
{
   int   done = FALSE;

   while (!done)
   {
      if (!get_tok())   p_error();  /* ??? */
      if ((done = check_semicolon()) == FALSE)
      {
         // I do not know the contents
      }
   }
   return 1;
}

/****************************************************************************/
/*
   read shape section
*/
read_shape()
{
   char  shape[MAX_TEXT];
   double   x,y,h,thick;
   int   rot,mir;
   char  *layer;
   DbFlag   flg;
   int   linecnt;
   /* read not new token for shape name because
      there is no deliminator */
   /* name this first 2 char will be ignored */
   strcpy(shape,&token[2]);

   if (!get_tok())   p_error(); /* x */
   x = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* y */
   y = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* heigth */
   h = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* rot */
   get_rot(atoi(token),&rot,&mir);
   if (!get_tok())   p_error(); /* width */
   if (!get_tok())   p_error(); /* thickness */
   thick = h * atof(token)/1000;
   if (strcmp(shape,"BOARD"))
   {
      L_Shape(shape);
      L_ShapeNamePos(shape,SHAPENAME,inch_2_dbunitpoint2(x,y),
            (char)mir,degree_2_dbunit(rot),inch_2_dbunit(thick),
            inch_2_dbunit(h));
   }

   /* alias */
   if (!get_tok())   p_error(); /* x */
   if (!get_tok())   p_error(); /* y */
   if (!get_tok())   p_error(); /* heigth */
   if (!get_tok())   p_error(); /* rot */
   if (!get_tok())   p_error(); /* width */
   if (!get_tok())   p_error(); /* thickness */
   if (!get_tok())   p_error(); /* Rth_junc_board */
   if (!strcmp(shape,"BOARD"))
   {
      layer = BOARDLAYER;
      flg   = NE_BOARD;
      L_NetAttr(BRDNET,inch_2_dbunit(0.0),0,0,0,flg);
      SCLG_Netlist(BRDNET);
   }
   else
   {
      layer = LIBOUTLINE;
      SCLG_Shape(shape);
   }
   linecnt = read_linelist(layer);  /* doit */
   read_padlist(shape);   /* doit */
   if (!strcmp(shape,"BOARD"))
      SCLG_Netlist(BRDNET);
   else
      SCLG_Shape(shape);
   linecnt += read_arclist(layer);   /* doit */

   if (ULTISUBVERSION >= 8)
      read_attributelist();

   if (linecnt == 1 && !strcmpi(shape,"BOARD"))
      L_NAttr(BRDNET,(DbFlag)NE_GOODBOARD);

   return 1;
}

/****************************************************************************/
/*
   read component section
*/
read_component()
{
   char  cname[MAX_TEXT],sname[MAX_TEXT],aname[MAX_TEXT];
   int   mir,rot;
   double   x,y;
   double   h,w,thick;

   if (!get_tok())   p_error(); /* name */
   strcpy(cname,token);
   if (!get_tok())   p_error(); /* alias */
   strcpy(aname,&token[1]);
   if (!get_tok())   p_error(); /* shape */
   strcpy(sname,token);
   L_Component(cname,sname);
   if (!get_tok())   p_error(); /* x */
   x = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* y */
   y = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* rot */
   get_rot(atoi(token),&rot,&mir);
   L_CompPos(cname,inch_2_dbunitpoint2(x,y),
      (char)mir,degree_2_dbunit((double)rot));

   if (!get_tok())   p_error(); /* temp case */
   if (!get_tok())   p_error(); /* temp junc */
   if (!get_tok())   p_error(); /* power */
   if (!get_tok())   p_error(); /* junc_board */

   if (!get_tok())   p_error(); /* name x */
   x = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* name y */
   y = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* name rot */
   get_rot(atoi(token),&rot,&mir);
   if (!get_tok())   p_error(); /* name width */
   w = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* name height */
   h = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* name thick */
   thick = w * atof(token)/1000;
   L_CompNamePos(cname,COMPNAME,inch_2_dbunitpoint2(x,y),
      (char)mir,degree_2_dbunit((double)rot),
      inch_2_dbunit(thick),inch_2_dbunit(h));

   if (!get_tok())   p_error(); /* alias x */
   x = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* alias y */
   y = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* alias rot */
   get_rot(atoi(token),&rot,&mir);
   if (!get_tok())   p_error(); /* alias width */
   w = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* alias height */
   h = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* alias thick */
   thick = w * atof(token)/1000;

   if (strlen(aname))
   {
      L_CompTypeNamePos(cname,aname,TYPENAME,inch_2_dbunitpoint2(x,y),
         (char)mir,degree_2_dbunit((double)rot),
         inch_2_dbunit(thick),inch_2_dbunit(h));
   }
   return 1;
}

/****************************************************************************/
/*
   read netname section
*/
read_netname()
{
   int      tcode;
   DbFlag   flg;

   if (!get_tok())   p_error(); /* name eliminate first char */
   strcpy(netname,&token[1]);
   if (!get_tok())   p_error(); /* tracecode */
   tcode = atoi(token);
   if (!strcmpi(netname,NONET))
      flg = 0;
   else
      flg = NE_ELECTRICAL;

   if (strlen(netname) > 0)
   {
      L_NetAttr(netname,inch_2_dbunit(0.0),tcode,tcode,tcode,flg);
      SCLG_Netlist(netname);

      if (netlcnt < MAX_NETL)
      {
         if ((netl[netlcnt].name = strdup(netname)) == NULL)
         {
            printf("No men\n");
            DEB();
            exit(1);
         }
         netlcnt++;
      }
      else
      {
         printf("Too many netnames\n");
         exit(1);
      }
   }

   return(TRUE);
}

/****************************************************************************/
/*
   read text
*/
read_text()
{
   char  prosa[MAX_TEXT];
   double   x,y,h,w,thick;
   int   rot,mir;
   char  layer[MAX_TEXT];
   DbFlag   flg;

   if (!get_tok())   p_error(); /* xpos */
   x  = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* ypos */
   y  = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* height */
   h  = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* width */
   w  = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* thick */
   thick  = h * atof(token) / 1000;
   if (!get_tok())   p_error(); /* ori */
   get_rot(atoi(token),&rot,&mir);
   if (!get_tok())   p_error(); /* layer */
   if (atoi(layer) == 0)
      strcpy(layer,LIBOUTLINE);
   else
      strcpy(layer,token);

   strcpy(prosa,++cp); /* text */
   prosa[strlen(prosa)-1] = '\0';
   SCLG_Netlist(TEXTNET);
   flg = GRTEXT_W_C | GRTEXT_H_C;      /* center text */
   LG_Text(inch_2_dbunitpoint2(x,y),flg,layer,
      (char)mir,degree_2_dbunit((double)rot),
      inch_2_dbunit(thick),inch_2_dbunit(h),prosa);

   return(skip_line());
}

/****************************************************************************/
/*
   read nettrace section
*/
read_nettrace()
{
      int width,i;
      DbFlag   flg;
      char  layer[10];

      if (!get_tok())   p_error(); /* layer */
      strcpy(layer,token);
      if (!get_tok())   p_error(); /* track code */
      width = atoi(token);
      if (!get_tok())   p_error(); /* track type */
      /* read poly line */
      read_polytrace();

      /* write  poly */
      flg = 0;
      for (i=1;i<def_polycnt;i++)
      {
         // this is width code.
         if (fabs(def_poly[i-1].x - def_poly[i].x) < 0.001 &&
             fabs(def_poly[i-1].y - def_poly[i].y) < 0.001 &&
             TE_SELECT != 3)  
               continue;  
         
         LG_Segm(0,inch_2_dbunitpoint2(def_poly[i-1].x,def_poly[i-1].y),
                 flg,layer,width,
                 inch_2_dbunit(def_poly[i].x - def_poly[i-1].x),
                 inch_2_dbunit(def_poly[i].y - def_poly[i-1].y));
         flg = GR_CONTINUE;

      }
   return 1;
}

/****************************************************************************/
/*
   read nettrace section
*/
read_netpoly()
{
   int width = 0,i;
   DbFlag   flg;
   char  layer[10];
   int   poly_void = 0;
   int   netnr;

   if (!get_tok())   p_error(); /* layer */
   strcpy(layer,token);
   if (!get_tok())   p_error(); /* netnr */
   netnr = atoi(token);

   if (!get_tok())   p_error(); /* pattern */
   if (!get_tok())   p_error(); /* dist */
   if (!get_tok())   p_error(); /* code */
   if (!get_tok())   p_error(); /* min cleancance */

   /* read poly line */
   while (read_void())
   {
      // flg = GR_COPPERLINE; /* GR_CLOSEDPOLY; */

      if (!poly_void)
         flg = GR_CLOSEDPOLY | GR_FILLED;
      else
         flg = GR_CLOSEDPOLY | GR_VOID;

      if (!process_void && poly_void && !void_message_done)  
      {
         printf (" Voids in *NP section are skipped\n");
         void_message_done = TRUE;
         continue;
      }
      for (i=1;i<def_polycnt;i++)
      {
          LG_Segm(0,inch_2_dbunitpoint2(def_poly[i-1].x,def_poly[i-1].y),
           flg,layer,width,
           inch_2_dbunit(def_poly[i].x - def_poly[i-1].x),
           inch_2_dbunit(def_poly[i].y - def_poly[i-1].y));
          flg = GR_CONTINUE;
      }
      // here check if closed
      if (fabs(def_poly[0].x - def_poly[def_polycnt-1].x) > 0 ||
          fabs(def_poly[0].y - def_poly[def_polycnt-1].y) > 0)
      {
         LG_Segm(0,inch_2_dbunitpoint2(def_poly[def_polycnt-1].x,def_poly[def_polycnt-1].y),
              flg,layer,width,
              inch_2_dbunit(def_poly[0].x - def_poly[def_polycnt-1].x),
              inch_2_dbunit(def_poly[0].y - def_poly[def_polycnt-1].y));

      }
      // no do process voids. Ultiboard has a problem in the following
      // case : 2 voids apperantly invert to a fill ???
      def_polycnt = 0;
      poly_void++;
   }

   // here must be minumum 1 entity
   if (poly_void)
   {
      flg = GR_SHAPEEND;
      LG_Segm(0,inch_2_dbunitpoint2(0.0,0.0),
           flg,layer,width,
           inch_2_dbunit(0.0),inch_2_dbunit(0.0));
   }

   return 1;
}

/****************************************************************************/
/*
   read netarc section
*/
read_netarc()
{
   char layer[10];
   int width;

   if (!get_tok())   p_error(); /* layer */
   strcpy(layer,token);
   if (!get_tok())   p_error(); /* track code */
   width = atoi(token);
   if (!get_tok())   p_error(); /* track type */
   /* read arc */
   read_arc(layer,width);
   return 1;
}

/****************************************************************************/
/*
   read netvia section
*/
read_netvia()
{
   double x,y;
   char  padname[10];
   DbFlag   flg = 0;
   int      code;

   if (!get_tok())   p_error(); /* x */
   x = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* y */
   y = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* code */
   code = atoi(token);
   sprintf(padname,"%s-f-0",token); /* this is the via setting */
   if (!get_tok())   p_error(); /* rot */
   if (!get_tok())   p_error(); /* layer setting */
   /* here write vias */
   L_ViaCoord(netname,padname,
         inch_2_dbunitpoint2(x,y),flg);

   update_paddef(code,0,0xf);
   L_PAttr(padname,(DbFlag)PD_VIA);    // set to via

   return 1;
}

/****************************************************************************/
/*
   read netcon section
*/
read_netcon()
{
   char  pinname[MAX_TEXT],compname[MAX_TEXT];
   int   code,rot;

   if (!get_tok())   p_error(); /* pin name  */
   strcpy(pinname,token);
   if (!get_tok())   p_error(); /* comp name */
   strcpy(compname,token);
   if (!get_tok())   p_error(); /* pad code  */
   code = atoi(token);
   if (!get_tok())   p_error(); /* pad rot */
   rot = atoi(token);
   if (!get_tok())   p_error(); /* layer */
   L_NetPinComp(netname,compname,pinname);
   return 1;
}


/****************************************************************************/
/*
   read poly_trace structure into def_poly
*/
read_polytrace()
{
   long  start_line;

   start_line = ifp_line;

   def_polycnt = 0;
   
   while (TRUE)
   {
      if (read_element() != UNKNOWN)
      {
         push_tok();
         return(1);
      }
      /* token = x */
      if (def_polycnt < MAX_DEFPOLY)
      {
         def_poly[def_polycnt].x = ult_2_inch(atoi(token));
         if (!get_tok())   p_error();  /* y koo */
         def_poly[def_polycnt].y = ult_2_inch(atoi(token));
         def_polycnt++;
      }
      else
         printf("Too many poly entries in trace at %ld\n",ifp_line);
   }
   printf("Error end of poly trace expected starting at %ld\n",start_line);
   f_exit();
   return 1;
}

/****************************************************************************/
/*
   read void structure into def_poly
   return 0 = no more;
   return 1 = more to come
*/
read_void()
{
   int   doit = TRUE;
   long  start_line = ifp_line;

   def_polycnt = 0;
   
   while (doit)
   {
      if (read_element() != UNKNOWN)
      {
         push_tok();
         return(0);
      }
      /* token = x */
      if (def_polycnt < MAX_DEFPOLY)
      {
         def_poly[def_polycnt].x = ult_2_inch(atoi(token));
         if (!get_tok())   p_error();  /* y koo */
         if (token[strlen(token)-1] == '/')
         {
            doit = FALSE;
            token[strlen(token)-1] = '\0';
         }
         def_poly[def_polycnt].y = ult_2_inch(atoi(token));
         def_polycnt++;
      }
      else
         printf("Too many poly entries in void at %ld\n",ifp_line);
   }
   return(1);
}

/****************************************************************************/
/*                                  
   read arc_trace structure into nothing
   width in ULTI UNITS
*/
read_arc(char *layer,int width)
{                                
   double x,y,rad;
   double sa,da;

   if (!get_tok())   p_error(); /* xcenter */
   x = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* ycenter */
   y = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* radius */
   rad = ult_2_inch(atoi(token));
   if (!get_tok())   p_error(); /* startarc */
   sa = atof(token) / 64;
   if (sa < 0) sa += 360;

   if (!get_tok())   p_error(); /* deltaarc */
   da = atof(token) / 64;

   if (fabs(da - 360.0) < 1.0)
      LG_Circle(0,inch_2_dbunitpoint2(x,y),0L,layer,
               width,inch_2_dbunit(rad));
   else
      LG_Arc(0,inch_2_dbunitpoint2(x,y),0L,layer,
               width,inch_2_dbunit(rad),
               degree_2_dbunit(sa),degree_2_dbunit(da));

   return 1;
}

/****************************************************************************/
/*
   read line list
   read a list until ;
   return 0 for not end of shape
          1 for end of shape

   only if shapename is BOARD
   then line width = b_dist
        layer = all

*/
read_linelist(char *layer)
{
   int   done = FALSE,cnt,cont;
   double   x,y,xold = 0,yold = 0;
   DbFlag   flg = 0;
   int      startcnt = 0;

   def_polycnt = 0;

   while (!done)
   {
      if (!get_tok())   p_error();  /* x koo */
      x = atoi(token);
      if (atoi(token) & 1)
      {
         cont = FALSE;
         cnt = 0;
         startcnt++;
         write_brdpoly(layer);
      }
      else
      {
         if (!startcnt) startcnt = 1;  // sometimes it starts with a even koo.
         cont = TRUE;
      }
      x = ult_2_inch(atoi(token));
      if ((done = check_semicolon()) == FALSE)
      {
         if (!get_tok())   p_error();  /* y koo */
         y = ult_2_inch(atoi(token));
         done = check_semicolon();

         if (def_polycnt < MAX_DEFPOLY)
         {                       
            def_poly[def_polycnt].x = x;
            def_poly[def_polycnt].y = y;
            def_polycnt++;
         }
         else
         {
            printf("Too many Poly\n");
            exit(1);
         }
         cnt++;
      }
   }
   write_brdpoly(layer);
   return startcnt;
}


/****************************************************************************/
/*
*/
write_brdpoly(char *layer)
{
   int   i;
   DbFlag flg = 0;

   if (def_polycnt == 0)   return;

   if (fabs(def_poly[0].x - def_poly[def_polycnt-1].x) < 0.001 &&
       fabs(def_poly[0].y - def_poly[def_polycnt-1].y) < 0.001)
   {
      def_poly[0].x = def_poly[def_polycnt-1].x;
      def_poly[0].y = def_poly[def_polycnt-1].y;
      flg = GR_CLOSEDPOLY;
   }

   for (i=1;i<def_polycnt;i++)
   {
      LG_Segm(0,inch_2_dbunitpoint2(def_poly[i-1].x,def_poly[i-1].y),flg,layer,
              0,inch_2_dbunit(def_poly[i].x - def_poly[i-1].x),
                inch_2_dbunit(def_poly[i].y - def_poly[i-1].y));
      flg = GR_CONTINUE;
   }
   def_polycnt = 0;
   return OK;
}

/****************************************************************************/
/*
   read pad list
   read a list until ;
*/
read_padlist(char *sname)
{
   int   done = FALSE;
   int   code;
   char  padcode[MAX_TEXT];
   double   x,y;
   int   rot;
   int   layer;
   int   pinnr = 0;
   while (!done)
   {
      if (!get_tok())   p_error();  /* pad code */
      code = atoi(token);
      if ((done = check_semicolon()) == FALSE)
      {
         if (!get_tok())   p_error(); /* pad rot */
         rot = (atoi(token) / 64) / 90;
         if (!get_tok())   p_error(); /* pad layerset */
         sscanf(token,"%x",&layer);
         layer = layer & 0xf;
         
         /* set rot to 0 is pad is round */
         if (pad_is_round(code,0) && pad_is_round(code,1) && pad_is_round(code,2))
            rot = 0;
            
         update_paddef(code,rot,layer);
         if (!get_tok())   p_error(); /* pad x */
         x = ult_2_inch(atoi(token));
         if (!get_tok())   p_error(); /* pad y */
         y = ult_2_inch(atoi(token));
         if (!get_tok())   p_error(); /* pad name */
         done = check_semicolon();
         sprintf(padcode,"%d-%x-%d",code,layer,rot);
         pinnr++;
         L_Pin(token,sname,padcode,pinnr);
         L_PinPos(token,sname,inch_2_dbunitpoint2(x,y),
            (char)0,degree_2_dbunit(0.0));
      }
   }
   return 1;
}

/****************************************************************************/
/*
   read arc list
   read a list until ;
*/
read_arclist(char *layer)
{
   int   done = FALSE;
   double   x,y,rad,sa,da;
   int   linecnt = 0;

   while (!done)
   {
      if (!get_tok())   p_error();  /* x koo */
      if ((done = check_semicolon()) == FALSE)
      {
         x = ult_2_inch(atoi(token));
         if (!get_tok())   p_error();  /* y koo */
         y = ult_2_inch(atoi(token));
         if (!get_tok())   p_error();  /* rad */
         rad = ult_2_inch(atoi(token));
         if (!get_tok())   p_error();  /* start angle */
         sa = atof(token) / 64;
         if (sa < 0) sa += 360;

         if (!get_tok())   p_error();  /* include angle */
         da = atof(token) / 64;
         done = check_semicolon();
         
         if (fabs(da - 360.0) < 1.0)
            LG_Circle(0,inch_2_dbunitpoint2(x,y),0L,layer,
               0,inch_2_dbunit(rad));
         else
            LG_Arc(0,inch_2_dbunitpoint2(x,y),0L,layer,
               0,inch_2_dbunit(rad),degree_2_dbunit(sa),degree_2_dbunit(da));
         linecnt++;
      }
   }
   return linecnt;
}

/****************************************************************************/
/*
   Check semicolon
   returns true if semicolon found
*/
check_semicolon()
{
   if (token[strlen(token) -1] == ';')
      {
         token[strlen(token)-1] = '\0';
         return(TRUE);
      }
   return(FALSE);
}

/****************************************************************************/
/*
   Skip_line and read next new line
*/
skip_line()
{
   if (!get_line(cur_line,MAX_LINE))
   {
      return(TRUE); /* done */
   }
   cur_new = TRUE;
   Push_tok = FALSE;

   return(FALSE);
}

/****************************************************************************/
/*
   read ELEMENTTYPE
*/
read_element()
{
      int   i;

      if (!get_tok()) return(-2);
      /* printf("%8ld\r",ifp_line); */

      for (i = 0; i < SIZ_ELEM_LIST; i++)
         if (!tokcmpi(elemlist[i],token))
            return(i+1);

   return(UNKNOWN);
}

/****************************************************************************/
/*
   init global
*/
init_global()
{
      int   i;

      for (i= 0; i < MAX_DEF_PAD; i++)
      {
         def_pad[i].pad[0].x1 = 0;
         def_pad[i].pad[0].x2 = 0;
         def_pad[i].pad[0].y  = 0;
         def_pad[i].pad[0].rad= 0;
         def_pad[i].pad[0].clear= 0;
         def_pad[i].pad[1].x1 = 0;
         def_pad[i].pad[1].x2 = 0;
         def_pad[i].pad[1].y  = 0;
         def_pad[i].pad[1].rad= 0;
         def_pad[i].pad[1].clear= 0;
         def_pad[i].pad[2].x1 = 0;
         def_pad[i].pad[2].x2 = 0;
         def_pad[i].pad[2].y  = 0;
         def_pad[i].pad[2].rad= 0;
         def_pad[i].pad[2].clear= 0;
      }

   if ((netl = (Netl *) malloc(MAX_NETL * sizeof(Netl))) == NULL)
   {
      printf(ERR_MEMORY);
      exit(1);
   }
   netlcnt = 0;

   return 1;
}

/****************************************************************************/
/*
   get next line
*/
void get_next_line()
{
   if (!get_line(cur_line,MAX_LINE))
      return;
   cur_new = TRUE;
   Push_tok = FALSE;
}

/****************************************************************************/
/*
   Get a token from the input file.
*/
get_tok()
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
void push_tok()
{
   Push_tok = TRUE;
}


/****************************************************************************/
/*
   Get the next token from line.
*/
get_next(char *lp,int newline)
{
   int      i = 0;

   if (newline)   cp = lp;

   for (; (isspace(*cp) || *cp == ',') && *cp != '\0'; ++cp);

   switch(*cp)
   {
      case '\0':
         return(FALSE);

      default:
         for (; !isspace(*cp) && *cp != '\0' && *cp != ','; ++cp, ++i)
                  token[i] = *cp;
         break;
   }

   if (!i)
      return(FALSE);

   token[i] = '\0';

if (printtoken) printf("Token >%s\n",token);

   return(TRUE);
}

/****************************************************************************/
/*
   Get a line from the input file.
*/
get_line(char *cp,int size)
{
   if (fgets(cp,size,ifp) == NULL)
   {
      if (!feof(ifp))
      {
         printf(ERR_READ_INPUT);
         exit(1);
      }
      else
      {
         cp[0] = '\0';
         return(FALSE);
      }
   }
   ++ifp_line;
   return(TRUE);
}


/****************************************************************************/
/*
   Parsing error.
*/
void p_error()
{
   printf("%sToken \"%s\" on line %ld\n",ERR_PARSE,token,ifp_line);
   exit(1);
}

/****************************************************************************/
/*
   Compare token against string only with length of token
*/
tokcmpi(char *t,char *s2)
{
   int   i;
   for (i =0;i<strlen(t);i++)
      if (toupchar(t[i]) != toupchar(s2[i]))
         return(TRUE);
   return(FALSE);
}

static char  toupchar(char x)
{
   if (islower(x))   return(toupper(x));
   return x;
}

/****************************************************************************/
/*
   Swap two integers.
*/
void swap(int *a,int *b)
{
   int      c;

   c = *a;
   *a = *b;
   *b = c;
}

/****************************************************************************/
/*
   Get the next word from lp and trunc lp.
*/
char *get_word(char *lp,int newline)
{
   int i = 0;
   static   char  wort[MAX_LINE];

   if (newline)   cp = lp;

   for (; isspace(*cp) && *cp != '\0'; ++cp) ;
   for (; !isspace(*cp) && *cp != '\0'; ++cp)
            wort[i++] = *cp;

   wort[i] = '\0';
   return(wort);
}

/****************************************************************************/
/*
*/
double   ult_2_inch(int x)
{
   double   x1 = x;
   return(x1/ULTIUNIT);
}

ulti2mil(int u)
{
   return((int)floor( ((double) u * (1000.0 / ULTIUNIT)) + 0.5 ));
}

/****************************************************************************/
/*
*/
void get_rot(int ur,int *r,int *m)
{
   if (ur < 0)
      *m = 1;
   else
      *m = 0;
   *r = abs(ur) / 64;
   if (*r >= 360) *r = *r - 360;
}

/****************************************************************************/
/*
*/
write_padstacks()
{
   int   i,f;
   char  padname[30];

   for (i=0;i<pad_defcnt;i++)
   {
      f = 0;
      sprintf(padname,"%d-%x-%d",pad_def[i].code,pad_def[i].layer,pad_def[i].rot);
      L_PadStack(padname);
      if (pad_def[i].code == via_code)
         L_PAttr(padname,(DbFlag)(PD_VIA));
      L_PDrill(padname,inch_2_dbunit(ult_2_inch(def_drill[pad_def[i].code])));
      if (pad_def[i].layer & 1)
         f |= out_pad(padname,TOP,pad_def[i].code,pad_def[i].rot);
      if (pad_def[i].layer & 2)
         f |= out_pad(padname,BOTTOM,pad_def[i].code,pad_def[i].rot);
      if (pad_def[i].layer & ~3)
         f |= out_pad(padname,INNER,pad_def[i].code,pad_def[i].rot);
//    if (!f)
//       printf("Pad [%d] on Layer [%x] with Rotation [%x] used but not defined\n",
//            pad_def[i].code,pad_def[i].layer,pad_def[i].rot);               
   }
   return(0);
}

/****************************************************************************/
/*
*/
get_widthint(int w)
{
   int   i;

   for (i=0;i<def_smdcnt;i++)
   {
      if (def_smd[i] == w)
         return(i+16);
   }

   if (def_smdcnt < MAX_DEF_PAD)
   {
      L_Width(mil_2_dbunit((double)w),def_smdcnt+16);
      def_smd[def_smdcnt] = w;
      def_smdcnt++;
   }
   else
   {
      printf("Too many SMD codes\n");
      f_exit();
   }
   return(def_smdcnt+16-1);
}
/****************************************************************************/
/*
*/
out_pad(char *padname,int l,int code,int rot)
{
   char  layername[MAX_TEXT];
   DbFlag   flg;
   int   x1,x2,y;
   int   used = 0;

   if (def_pad[code].pad[l].y > 0)
   {
      switch (l)
      {
         case  INNER:
            SCLG_PadStack(padname,PADINNER);
            strcpy(layername,PADINNER);
         break;
         case  TOP:
            SCLG_PadStack(padname,PADTOP);
            strcpy(layername,PADTOP);
         break;
         case  BOTTOM:
            SCLG_PadStack(padname,PADBOTTOM);
            strcpy(layername,PADBOTTOM);
         break;
      }
      // Alex 26.10.94
      // (( Y ) - ( X1 + X2 ) ñ 1 )   << ñ 1  ist fr die Umrechnungs-
      //                                 Ungenauigkeit z.B. bei mm >> inch
      if (fabs(def_pad[code].pad[l].y - (def_pad[code].pad[l].x1 + def_pad[code].pad[l].x2)) < 2)
      {
         int   rad = def_pad[code].pad[l].y /2; // always y/2 because
               // the radius is the chamfering of corners.
         /* pad is either round or square */
         if (def_pad[code].pad[l].rad)
         {
            /* round */
            flg = 0;
         }
         else
         {
            /* square */
            flg = GR_CIRCLESQUARE;
         }
         LG_Circle(0,inch_2_dbunitpoint2(0.0,0.0),flg,layername,get_widthint(0),
                  mil_2_dbunit((double)ulti2mil(rad)));
         used = 1;
      }
      else
      {
         /* pad is oblong */
         /* pad is either round or square */
         y  = def_pad[code].pad[l].y;
         x1 = def_pad[code].pad[l].x1-y/2;
         x2 = def_pad[code].pad[l].x2-y/2;

         if (def_pad[code].pad[l].rad)
         {
            /* round */
            flg = 0;
         }
         else
         {
            /* square */
            flg = GR_CORFLAT;
         }
         switch (rot)
         {
            case  0:
               LG_Segm(0,mil_2_dbunitpoint2(-ulti2mil(x1),0.0),
                     flg,layername,get_widthint(ulti2mil(y)),
                     mil_2_dbunit(ulti2mil(x1+x2)),mil_2_dbunit(0.0));
               used = 1;
            break;
            case  1:
               LG_Segm(0,mil_2_dbunitpoint2(0.0,-ulti2mil(x1)),
                     flg,layername,get_widthint(ulti2mil(y)),
                     mil_2_dbunit(0.0),mil_2_dbunit(ulti2mil(x1+x2)));
               used = 1;
            break;
            case  2:
               LG_Segm(0,mil_2_dbunitpoint2(-ulti2mil(x2),0.0),
                     flg,layername,get_widthint(ulti2mil(y)),
                     mil_2_dbunit(ulti2mil(x1+x2)),mil_2_dbunit(0.0));
               used = 1;
            break;
            case  3:
               LG_Segm(0,mil_2_dbunitpoint2(0.0,-ulti2mil(x2)),
                     flg,layername,get_widthint(ulti2mil(y)),
                     mil_2_dbunit(0.0),mil_2_dbunit(ulti2mil(x1+x2)));
               used = 1;
            break;
         }
      }
   }
   return (used);
}

/****************************************************************************/
/*
*/
update_paddef(int code,int rot,int layer)
{
   int   i;

   for (i=0;i<pad_defcnt;i++)
   {
      if (pad_def[i].rot == rot &&
          pad_def[i].code == code &&
          pad_def[i].layer == layer)
         return(i);
   }
   if (pad_defcnt < MAX_DEF_PAD)
   {
      pad_def[pad_defcnt].rot = rot;      
      pad_def[pad_defcnt].code = code;    
      pad_def[pad_defcnt].layer = layer;     
      pad_defcnt++;
   }
   else
   {
      printf("Too many pad definitions\n");
      exit(1);
   }
}

/****************************************************************************/
/*
*/
static int pad_is_round(int code,int layer)
{
   if ((def_pad[code].pad[0].x1 == def_pad[code].pad[layer].x2) &&
       (def_pad[code].pad[0].x1 == def_pad[code].pad[layer].rad) &&
       (def_pad[code].pad[0].y == def_pad[code].pad[layer].x1 * 2))
            return TRUE;
   return FALSE;        
}

/****************************************************************************/
/*
   Ultiboard defines padlayers in library definitions.
   but ultiboard can also have pad defined only on solder layer 
   and in library on comp and then mirror this component so 
   that pads ends up on solder. Therefore all unset paddefinitions 
   on solder will be updated from comp and reverse !
*/
adjust_pad()
{
   int   i;

   for (i=0;i<MAX_DEF_PAD;i++)
   {
      /* 1 = top 2 = solder */
      if (def_pad[i].pad[1].y == 0)
      {
         /* update from other layer */
         def_pad[i].pad[1].x1    = def_pad[i].pad[2].x1;
         def_pad[i].pad[1].x2    = def_pad[i].pad[2].x2;
         def_pad[i].pad[1].y     = def_pad[i].pad[2].y;
         def_pad[i].pad[1].rad   = def_pad[i].pad[2].rad;
         def_pad[i].pad[1].clear = def_pad[i].pad[2].clear;
      }
      if (def_pad[i].pad[2].y == 0)
      {
         /* update from other layer */
         def_pad[i].pad[2].x1    = def_pad[i].pad[1].x1;
         def_pad[i].pad[2].x2    = def_pad[i].pad[1].x2;
         def_pad[i].pad[2].y     = def_pad[i].pad[1].y;
         def_pad[i].pad[2].rad   = def_pad[i].pad[1].rad;
         def_pad[i].pad[2].clear = def_pad[i].pad[1].clear;
      }
   }
}

/****************************************************************************/
/*
*/
update_ultitec()
{
   FILE  *ulttec;
   char  tmp[255],*lp;
   int   i;

   phys_layer[1] = 1;
   phys_layer[2] = 0;
   for (i=3;i<33;i++)
   {
      phys_layer[i] = i-1;
   }

   if ((ulttec = fopen(ULTITEC,OPEN_READ_TEXT)) != NULL)
   {
      while (fgets(tmp,255,ulttec))
      {
         if (tmp[0] == '.')
         {
            lp = strtok(tmp," \t\n");
            if (!strcmpi(lp,".LAYER_STACKUP"))
            {
               int ult, phys;
               if ((lp = strtok(NULL," \t\n")) == NULL)
               {
                  printf("Error in .LAYER_STACKUP\n");
                  exit(1);
               }
               ult = atoi(lp);      
               if ((lp = strtok(NULL," \t\n")) == NULL)
               {
                  printf("Error in .LAYER_STACKUP\n");
                  exit(1);
               }
               phys = atoi(lp);      
               if (ult > 0 && ult < 33)
                  phys_layer[ult] = phys;
            }
            if (!strcmpi(lp,".PROCESS_VOID"))
            {
               if ((lp = strtok(NULL," \t\n")) == NULL)
               {
                  printf("Error in .PROCESS_VOID\n");
                  exit(1);
               }
               if (lp[0] == 'Y' || lp[0] == 'y')
                  process_void = TRUE;
            }
            if (!strcmpi(lp,".TE_SELECT"))
            {
               /* lines are not cracked !!! */
               if ((lp = strtok(NULL," \t\n")) == NULL)
               {
                  printf("Error in .TE_SELECT\n");
                  exit(1);
               }
               TE_SELECT = atoi(lp);
               L_TAttr((DbFlag)TE_SELECT);
            }
         }
      }
      fclose(ulttec);
   }
   else
      printf("Can not open file [%s]\n",ULTITEC);

   return OK;
}

/****************************************************************************/
/*
*/
get_inversion()
{
   return VERSION;
}

/****************************************************************************/
/*
   End ULTIN.C
*/
/****************************************************************************/

#endif

/*Ende **********************************************************************/



