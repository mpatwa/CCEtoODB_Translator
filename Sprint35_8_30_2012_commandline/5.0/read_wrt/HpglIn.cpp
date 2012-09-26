// $Header: /CAMCAD/4.5/read_wrt/HpglIn.cpp 25    4/28/06 6:08p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "format_s.h"
#include "file.h"
#include "geomlib.h"
#include "pcbutil.h"
#include "graph.h"
#include <math.h>
#include "gauge.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress;

/*****************************************************************************/

#undef   UNKNOWN_COMMAND

#define  OK                1
#define  END_OF_TEXT       0x03

#define  MAX_POLY          4096     // I have seen over 255 vertex cnts.
#define  MAX_CIRCLE_2_APP  100

#define  SMALL_DELTADEGREE 1        // 1 degree accurary on delta arcs.

/****************************************************************************/

// #define  HP_COMMAND_CONTINUE  0
#define  HP_CI                1
#define  HP_PM1               2
#define  HP_LINE              3    // penup, down etc ...
#define  HP_PM0               4    // this is first coordinate in poly
#define  HP_CI_FILLED         5
#define  HP_PM2               6    // fill this polygon
#define  HP_ARC               7    // aa command in polymode
#define  HP_RR                8
#define  HP_ER                9

typedef struct
{
   double   smallest;
   double   largest;
   int      pennr;      // 0 is all
}Circle_2_App;

typedef struct
{
   double   x,y, radius;
   char     typ;     //
   char     plot;
   char     plotabs; // plot absolute
} Polyg;

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

static int  hp_init(), hp_iw(), hp_sc(), hp_ip();
static int  hp_pa(), hp_pd(), hp_pu(), hp_lb();
static int  hp_sp(), hp_aa(), hp_ci(), hp_si();
static int  hp_pr(), hp_ar(), hp_dt(), hp_di(), hp_dr();
static int  hp_pm(), hp_fp(), hp_pw(), hp_wu();
static int  hp_ra(), hp_lo(), hp_vs(), hp_ep();
static int  hp_ea(), hp_pe(), hp_pg(), hp_wg();
static int  hp_ew(), hp_sl(), hp_rr(), hp_er();
static int  hp_df(), hp_cp(), hp_sd(), hp_sr();
static int  hp_bp(), hp_pc(), hp_np();
static int  hp_pj(), hp_ft();
static int  hp_ro();

static int  hp_knowncommand();

static List command_lst[] =
{
   {"PA",         hp_pa},  // should be on top for speed resonse
   {"PD",         hp_pd},
   {"PU",         hp_pu},
   {"PR",         hp_pr},
   {"PM",         hp_pm},
   {"FP",         hp_fp},
   {"PW",         hp_pw},
   {"IN",         hp_init},
   {"IW",         hp_iw},
   {"SC",         hp_sc},
   {"LB",         hp_lb},
   {"LO",         hp_lo},
   {"IP",         hp_ip},
   {"SP",         hp_sp},
   {"AA",         hp_aa},
   {"RR",         hp_rr},
   {"ER",         hp_er},
   {"CI",         hp_ci},
   {"SI",         hp_si},
   {"SR",         hp_sr},
   {"AR",         hp_ar},
   {"DT",         hp_dt},
   {"DI",         hp_di},
   {"DR",         hp_dr},
   {"WU",         hp_wu},
   {"RA",         hp_ra},
   {"EA",         hp_ea},
   {"VS",         hp_vs},
   {"EP",         hp_ep},
   {"PE",         hp_pe},
   {"WG",         hp_wg},  
   {"EW",         hp_ew},  
   {"SL",         hp_sl},  
   {"CP",         hp_cp},           // character plot
   {"PS",         hp_knowncommand}, // plot size for hard limits
   {"BP",         hp_knowncommand}, // begin plot
   {"AD",         hp_knowncommand}, // alternate font
   {"PC",         hp_pc},           // pen color
   {"FT",         hp_knowncommand}, // fill type
   {"LA",         hp_knowncommand}, // line attributes, ends etc...
   {"SS",         hp_knowncommand}, // select standard Font
   {"NR",         hp_knowncommand}, // 
   {"SD",         hp_sd},           // text font
   {"PG",         hp_pg},
   {"DF",         hp_df},
   {"BP",         hp_bp},           // begin plot
   {"NP",         hp_np},           // number of pens
   {"PJ",         hp_pj},
   {"FT",         hp_ft},           // fill type
   {"RO",         hp_ro},           // plot rotation
};
#define  SIZ_OF_COMMAND (sizeof(command_lst) / sizeof(List))

/****************************************************************************/
/* Global Commands */
static   int      get_hpcommand();
static   int      is_command(char *);
static   int      read_koo(double *,double *);
static   int      read_real(double *);
static   int      write_polygon(int filled, int fpcommand);
static   int      close_polygon();
static   long     hp_encode(int *cnt, int bit7mode);
static   int      load_hpglsettings(const CString fname);
static   double   xscale(double);
static   double   yscale(double);
static   double   _scale(double);      // no offset etc...
static   void     hp_rotate(double *x, double *y);
static   int      hp_unknown(char *);  // collection of un interpreted commands
static unsigned long check_background(unsigned long c);


static   double   G_cur_rotation;      // rotation in degree
static   int      G_textmirror;
static   double   iw_x_1, iw_y_1;
static   double   iw_x_2, iw_y_2;
static   double   ip_x_1, ip_y_1;
static   double   ip_x_2, ip_y_2;
static   double   sc_x_1, sc_y_1;
static   double   sc_x_2, sc_y_2;
static   double   abs_plot_f_x, abs_plot_f_y;
static   double   offset_x, offset_y;  // in hp units
static   double   cp_spaces, cp_lines;
static   double   polyxold, polyyold, xold,yold, textrot = 0;

static   char     plotabs = TRUE;
static   char     openpoly, plot = FALSE;
static   char     label_terminator;
static   int      hp_in = FALSE;                /* hp init IN found */
static   int      label_origin = 1;
static   int      textoblique = 0;

static   int      CurrFillType;
static   int      CurrLayer;
static   int      CurrPen;
static   int      CurrPenIndex;
static   int      CurrWuMode = 0;
static   int      CurrIpRotation = 0;  // 0 = normal
                                       // 1 = upside down y = -y
                                       // 2 = reverse x = -x
                                       // 3 = upside & reserve
                                       //    x = -x; y = -y;

static   int         hpplot_scaled = FALSE;

static   double      HPscale;          // scale * unit conversions
static   double      CMscale;          // scale factor for Centimeters conversions
static   double      AktTextWidth=0.1;  // Character width
static   double      AktTextHeight=0.1; // Character height
static   char        proportional;
static   int         Circles_2_Aps;    // from format pointer
static   int         number_of_pens;

static   char        prefix[MAXPREFIX+1];

static   Circle_2_App   circle_2_app[MAX_CIRCLE_2_APP];
static   int         circle_2_app_cnt;

static   HPstruct    HPcpy;

static   Polyg       *polygon;
static   int         polygoncnt;

static   int         cur_polygonmode = 2;       // PM2 is exit polygom mode
static   int         hp_end_of_file;
static   int         NoLineWidth;
static   long        total_filelength;
static   int         file_read_error = FALSE;
static   int         SPACE_IS_SEPARATOR = TRUE; // spaces are separators
static   double      CHAR_SPACERATIO = 1.5;     // hpgl.in
static   int         pagecount;                 // plot page cnt updated on BP
static   int         USE_PC_COMMAND = FALSE;

static   CString  filename;
static   FileStruct *file;

static   int         display_error, display_log;
static   FILE        *flog;
static   FILE        *ifp;
static   CCEtoODBDoc  *doc;

static   long        PENCOLOR[8];   

static   int         SMALLWIDTH_INDEX;
static   double      SMALLWIDTH_SIZE;



/******************************************************************************
* ReadHPGL
*/
void ReadHPGL(const char *f, CCEtoODBDoc *Doc, FormatStruct *Format,  int PageUnits)
{
   int code;
   long command_cnt = 0;
   double scaleUnitsFactor = Units_Factor(Format->PortFileUnits, PageUnits);

   doc = Doc;
   display_error = FALSE;
   display_log   = FALSE;

   CString logFile = GetLogfilePath("hp.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage(logFile, "Can not open Logfile", MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   
   HPcpy = Format->HP;
   SPACE_IS_SEPARATOR = Format->HP.HPspace;

   // change the smallest with to SMALLWIDTH


   CMscale = Units_Factor(UNIT_MM, PageUnits) * Format->Scale * 10; // convert from MM to CM
   HPscale = Format->Scale * scaleUnitsFactor;
   proportional = Format->HP.Proportional;
   strcpy(prefix, Format->prefix);
   NoLineWidth = Format->ignorePenWidth;
   Circles_2_Aps = Format->HP.Circles_2_Aps;

   if ((polygon = (Polyg*)calloc(MAX_POLY,sizeof(Polyg))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   polygoncnt = 0;
   circle_2_app_cnt = 0;

   CString settingsFile( getApp().getImportSettingsFilePath("hpgl.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nHPGL Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_hpglsettings(settingsFile);

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(f, "r")) == NULL)
   {
      ErrorMessage("Error open file", f);
      return;
   }

   rewind(ifp);
   filename = f;  
   file = Graph_File_Start(f, Type_HPGL);

   // some HP files do not have IN command at the begining of the file.
   hp_init();

   CurrLayer = Graph_Level(HPcpy.HpPens[0]. layer, /*pennumber,*/ prefix, 0);
// here color
   LayerStruct *layer = doc->FindLayer(CurrLayer);
   layer->setColor( check_background(HPcpy.HpPens[0].color));
   hp_end_of_file = FALSE;
   command_cnt = 0;

   while (!hp_end_of_file && !file_read_error)
   {
      command_cnt++;

      if ((code = get_hpcommand()) < 0)
         hp_end_of_file = TRUE;
      else
      {
         // if not pd or pa do write_poly();
         (*command_lst[code].function)();
      }
   }

   if (pagecount > 1)
   {
      fprintf(flog, "Multiple Pages have been found! (See FileList Menu)\n");
      display_error++;
   }

   // this can happen
   EliminateSinglePointPolys(doc);                

   // only written in FP EP CI write_polygon(1);
   fclose(ifp);
   free(polygon);
   fclose(flog);

   // copy back HPcpy.HpPens[i].color
   for (int i=0; i<MAX_HPPENS; i++)
      Format->HP.HpPens[i].color = HPcpy.HpPens[i].color;

   if (display_error && display_log)
      Logreader(logFile);

   return;
}

/******************************************************************************
* circle_is_aperture
*/
static int circle_is_aperture(double r)
{
   if (!Circles_2_Aps)
      return 0;   // format has to be switched.

   for (int i=0; i<circle_2_app_cnt; i++)
   {
      if (r >= circle_2_app[i].smallest && r <= circle_2_app[i].largest)
      {
         if (circle_2_app[i].pennr == 0 || circle_2_app[i].pennr == CurrPen)
             return 1;
      }
   }
   return 0;
}

/******************************************************************************
* long check_background
*/
static unsigned long check_background(unsigned long c)
{
   BYTE  b1 = GetBValue(doc->getSettings().Foregrnd);
   BYTE  g1 = GetGValue(doc->getSettings().Foregrnd);
   BYTE  r1 = GetRValue(doc->getSettings().Foregrnd);
   BYTE  b2 = GetBValue(c);
   BYTE  g2 = GetGValue(c);
   BYTE  r2 = GetRValue(c);
   if (c == doc->getSettings().Foregrnd)
      c = RGB(~r2, ~g2, ~b2); // reverse colors

   return c;
}

/******************************************************************************
* load_hpglsettings
*/
static int load_hpglsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   CHAR_SPACERATIO = 1.5;
   pagecount = 0;
   USE_PC_COMMAND = FALSE;

   PENCOLOR[0] = RGB(255, 255, 255);   // White
   PENCOLOR[1] = RGB(0, 0, 0);         // Black
   PENCOLOR[2] = RGB(255, 0, 0);       // Red
   PENCOLOR[3] = RGB(0, 255, 0);       // Green
   PENCOLOR[4] = RGB(255, 255, 0);     // Yellow
   PENCOLOR[5] = RGB(0, 0, 255);       // Blue
   PENCOLOR[6] = RGB(255, 0, 255);     // Magenta
   PENCOLOR[7] = RGB(0, 255, 255);     // Cyan

   if (doc->getBlockAt(0)->getFlags() & BL_SMALLWIDTH)
      SMALLWIDTH_SIZE = doc->getBlockAt(0)->getSizeA();
   else
      SMALLWIDTH_SIZE = 0;

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp, "HPGL Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = strtok(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         // here do it.
         if (!STRCMPI(lp, ".CIRCLE_2_APERTURE"))
         {
            if (circle_2_app_cnt < MAX_CIRCLE_2_APP)
            {
               if ((lp = strtok(NULL, " \t\n")) != NULL)    // smallest size
               {
                  circle_2_app[circle_2_app_cnt].smallest = atof(lp);
                  circle_2_app[circle_2_app_cnt].largest = atof(lp);
                  circle_2_app[circle_2_app_cnt].pennr = 0; // 0 is all pens
               }
               if ((lp = strtok(NULL, " \t\n")) != NULL)    // largest size
                  circle_2_app[circle_2_app_cnt].largest = atof(lp);
               if ((lp = strtok(NULL, " \t\n")) != NULL)    // pen nr
                  circle_2_app[circle_2_app_cnt].pennr = atoi(lp);   // 0 is all pens
               circle_2_app_cnt++;
            }
            else
            {
               fprintf(flog, "Too many circle_2_app\n");
               display_error++;
            }
         }
         else
         if (!STRCMPI(lp, ".CHAR_SPACERATIO"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            CHAR_SPACERATIO = atof(lp);
         }
         else
         if (!STRCMPI(lp, ".DISPLAYLOG"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "y", 1))
               display_log = TRUE;
         }
         else
         if (!STRCMPI(lp, ".USEPENCOLORCOMMAND"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            if (!STRNICMP(lp, "y", 1))
               USE_PC_COMMAND = TRUE;
         }
         else
         if (!STRCMPI(lp, ".PENCOLOR"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            int pennr = atoi(lp);
            if ((lp = strtok(NULL, " ,\t\n")) == NULL)
               continue;
            int c1 = atoi(lp);
            if ((lp = strtok(NULL, " ,\t\n")) == NULL)
               continue;
            int c2 = atoi(lp);
            if ((lp = strtok(NULL, " ,\t\n")) == NULL)
               continue;
            int c3 = atoi(lp);
            if (pennr >= 0 && pennr <= 7)
               PENCOLOR[pennr] = RGB(c1, c2, c3);
         }
      }
   }

   fclose(fp);
   return 1;
}

/******************************************************************************
* get_hpcommand
*/
static int get_hpcommand()
{
   int chBufCount = 2;
   BOOL StartEsc = FALSE;

   char chBuf[3];
   chBuf[2] = '\0'; // set end of string

   while (!file_read_error)
   {
      char ch = fgetc(ifp);

      if (ch == EOF)
         return -1;

      if (!StartEsc && !isprint(ch) && ch != 0x1b)
         continue;

      // if there are other char than spaces between a command reset.
      // % is from escape sequence
      if (ch != '%' && !isalpha(ch) && !isspace(ch))  
         chBufCount = 2; 
                 
      if (ch != '%' && ch != 0x1b && !isalpha(ch))         
         continue;

      // switch esc on and off
      if (ch == 0x1b) 
         StartEsc = !StartEsc;

      chBuf[1] = ch;
      chBufCount--;
      if (!chBufCount)
      {
         // buffer full and now check for command 
         // if not command could be found rotate buffer and set chBufCount = 1
         int functionCode = is_command(chBuf);
      
         if (functionCode >= 0)
            return functionCode;

         if (chBuf[1] == '\0')
            chBufCount = 2; // clear all buffer
         else
            chBufCount = 1;
      }
      chBuf[0] = chBuf[1];
   }

   return -1;
}

/******************************************************************************
* is_command
*/
static int is_command(char *command)
{
   for (int i=0; i<SIZ_OF_COMMAND; i++)
   {
      if (!STRCMPI(command_lst[i].token, command))
         return i;
   }

// if (isalpha(command[0]) && isalpha(command[1]))
   hp_unknown(command);

   return -1;
}

/******************************************************************************
* read_koo
*/
static int read_koo(double *x, double *y)
{
   if (!read_real(x))
      return 0;

   if (!read_real(y))
      return 0;

   return OK;
}

/******************************************************************************
* read_real
*/
static int read_real(double *x)
{
   char buf[80];
   int bufCount = 0;

   *x = 0.0;

   while (bufCount < 80)
   {
      char ch = fgetc(ifp);
      if (ch == EOF)
         return FALSE;

      // spaces can be used as delimter in HPGL, but in Corel, they are not delimeters
      if (!SPACE_IS_SEPARATOR)
      {
         if (isspace(ch))
            continue;
      }
      else
      {
         if (bufCount == 0 && isspace(ch))
            continue;
      }

      if (ch == '!')
         continue;   // This was in some files PA!2.1

      if (bufCount == 0 && ch == ',')
         continue;   // There are PD,x,y structures ??

      if (isdigit(ch) || ch == '-' || ch == '+' || ch == '.')
      {
         buf[bufCount++] = ch;
      }
      else
      {
         /* push last char back on stack if it is a next command */
         if (ch != ';' && ch != ',')  
         {
            if (ungetc(ch, ifp) == EOF)
            {
               ErrorMessage("Problem with unget");
               return -1;
            }
         }
         
         buf[bufCount] = '\0';

         if (bufCount)
         {
            *x = atof(buf);
            return TRUE;
         }
         else
         {
            return FALSE;
         }
      }
   }

   return TRUE;
}

/******************************************************************************
* read_until_alpha
*/
static int read_until_alpha()
{
   while (TRUE)
   {
      char ch = fgetc(ifp);
      
      if (ch == EOF)
         return 0;

      if (isalpha(ch))
      {
         // push it back on streem
         if (ungetc(ch, ifp) == EOF)
         {
            ErrorMessage("Problem with unget");
            return -1;
         }
         return 1;
      }
   }
}

/******************************************************************************
* hp_unknown
*  - These are the skipped HP commands
*/
static int hp_unknown(char *c)
{
   char cc;
   char tmp[20+1];
   int tmpcnt = 0;

   if (c[0] == 0x1B)  // Esc sequences
   {
      //NR;
      //<esc>E****** Failed to open log-file </var/adm/log-HPLJ4>, Error Code  2 ******

      if (c[1] == 'E')  // reset command
      {
         while ((cc = fgetc(ifp)) != EOF)
         {
            if (cc == ';')
               break;
         }
      }
      else
      if (c[1] == '%')
      {
         c[1] = '\0';
         // now go including A, B, X
         while ((cc = fgetc(ifp)) != EOF)
         {
            tmp[tmpcnt] = cc;
            tmpcnt++;
            if (tmpcnt >= 20)
               tmpcnt = 0;
            if (isalpha(cc))
               break;
         }
         tmp[tmpcnt] = '\0';
         // both commands enter hpgl/2 mode.
         if (!STRCMPI(tmp,"-1B") || !STRCMPI(tmp,"0B") || !STRCMPI(tmp,"1B") || 
             !STRCMPI(tmp,"2B") || !STRCMPI(tmp,"3B"))
         {
            //this is an HP-GL/2 code.
            // only make a new page is afer a escape sequence, also a INit code was found.
            // otherwise, this may be empty pages.
            if (hp_in)
            {
               if (pagecount)
               {
                  CString  p;
                  p.Format("%s_Page_%d", filename, pagecount+1);
                  file = Graph_File_Start(p, Type_HPGL);
               }
               pagecount++;
            }
            hp_in = FALSE;
            return 1;
         }
      }
      else
      {
         // not implemented esc sequence
      }
   }
   else
   if (c[0] == 'M' && c[1] == 'C') // MC Merge clipp
   {
      // do nothing
      read_until_alpha();
      c[1] = '\0';
   }
   else
   if (c[0] == 'N' && c[1] == 'P') // NP number of Pens
   {
      // do nothing
      read_until_alpha();
      c[1] = '\0';
   }
   else
   if (c[0] == 'C' && c[1] == 'R')  // CR set color range
   {
      // do nothing
      read_until_alpha();
      c[1] = '\0';
   }
   else
   if (c[0] == 'L' && c[1] == 'T')  // LT line type
   {
      // do nothing
      read_until_alpha();
      c[1] = '\0';
   }
   else
   if (c[0] == 'P' && c[1] == 'C')  // PC Pencolore assign
   {
      // do nothing
      read_until_alpha();           
      c[1] = '\0';
   }
   else
   if (isalpha(c[0]))
   {
      // here is the start of an unknown command
      fprintf(flog, "Unknown HP command [%s]\n", c);
      display_error++; // do not display log file
      return 1;
   }

   // here is only a partial HP command in the buffer
   return 1;
}

/******************************************************************************
* hp_init
*/
static int hp_init()
{
   int   err;

   // a new init is found and datas have been loaded before in this page -> make a new page
   int d = file->getBlock()->getDataList().GetCount();
   if (d)
   {
      CString  p;
      p.Format("%s_Page_%d", filename, pagecount+1);
      file = Graph_File_Start(p, Type_HPGL);
      pagecount++;
   }

   label_terminator = END_OF_TEXT;
   hp_in = TRUE;
   plot = FALSE;
   openpoly = FALSE;
   plotabs = TRUE;
   xold = yold = 0.0;
   abs_plot_f_x = 1.0;
   abs_plot_f_y = 1.0;
   ip_x_1 = ip_y_1 = 0.0;
   sc_x_1 = sc_y_1 = 0.0;
   ip_x_2 = ip_y_2 = 0.0;
   sc_x_2 = sc_y_2 = 0.0;
   iw_x_1 = iw_y_1 = 0.0;
   iw_x_2 = iw_y_2 = 0.0;
   offset_x = offset_y = 0.0; // in hpunits
   cp_spaces = cp_lines = 0;
   number_of_pens = MAX_HPPENS;
   cur_polygonmode = 2;
   polygoncnt = 0;
   label_origin = 1;
   G_cur_rotation = 0;
   CurrPen = 1;

   if (NoLineWidth || HPcpy.HpPens[CurrPen-1].width <= SMALLWIDTH_SIZE)
      CurrPenIndex = 0;
   else
      CurrPenIndex = Graph_Aperture("", T_ROUND, HPcpy.HpPens[CurrPen-1].width, 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, FALSE, &err);

   CurrIpRotation = 0;
   CurrWuMode = 0;
   CurrFillType = 0;
   AktTextHeight  = 0.375 * CMscale; // cm
   AktTextWidth   = 0.285 * CMscale; // cm

   // the space from one char to the next is done with a 10% spaceratio in CAMCAD
   AktTextWidth = AktTextWidth * CHAR_SPACERATIO;   // hp width is width without spacing

   if (HPcpy.HpPens[0].color == 0)
      HPcpy.HpPens[0].color = PENCOLOR[0];   // White
   if (HPcpy.HpPens[1].color == 0)
      HPcpy.HpPens[1].color = PENCOLOR[1];   // Black
   if (HPcpy.HpPens[2].color == 0)
      HPcpy.HpPens[2].color = PENCOLOR[2];   // Red
   if (HPcpy.HpPens[3].color == 0)
      HPcpy.HpPens[3].color = PENCOLOR[3];   // Green
   if (HPcpy.HpPens[4].color == 0)
      HPcpy.HpPens[4].color = PENCOLOR[4];   // Yellow
   if (HPcpy.HpPens[5].color == 0)
      HPcpy.HpPens[5].color = PENCOLOR[5];   // Blue
   if (HPcpy.HpPens[6].color == 0)
      HPcpy.HpPens[6].color = PENCOLOR[6];   // Magenta
   if (HPcpy.HpPens[7].color == 0)
      HPcpy.HpPens[7].color = PENCOLOR[7];   // Cyan

   return OK;
}

/******************************************************************************
* hp_pg
*/
static int hp_pg()
{
   fprintf(flog, "New page command\n");
   display_error++;
   return OK;
}

/****************************************************************************
* hp_df
*  set defaults
*/
static int hp_df()
{
   return OK;
}

/****************************************************************************
* hp_bp
*  begin plot
*/
static int hp_bp()
{
   return OK;
}

/******************************************************************************
* hp_iw
*/
static int hp_iw()
{

   if (!read_koo(&iw_x_1, &iw_y_1))
      return OK;
   if (!read_koo(&iw_x_2, &iw_y_2))
      return OK;

   // this here are clipping commands !
   return OK;
}

/******************************************************************************
* hp_vs
*/
static int hp_vs()
{
   return OK;
}

/******************************************************************************
* hp_knowncommand
*/
static int hp_knowncommand()
{
   return OK;
}

/******************************************************************************
* hp_ft
*/
static int hp_ft()
{
   double   p;

   if (!read_real(&p))  // pen
      return(0);

   CurrFillType = round(p);

   return OK;
}

/******************************************************************************
* hp_pc
*/
static int hp_pc()
{
   int pen;
   int red = 0, green = 0, blue = 0;
   double p; 
   
   // pen up.
   plot = FALSE;

   if (!read_real(&p))  // pen
      return(0);

   pen = round(p);

   if (!read_real(&p))  // red
   {
      red = round(p);
      if (read_real(&p))   // green
      {
         green = round(p);
         if (read_real(&p))   // blue
         {
            blue = round(p);
         }
      }
   }

   if (USE_PC_COMMAND)
   {
      CurrPen = ((pen-1) % (MAX_HPPENS -1)) + 1;
      if (CurrPen < 1)
         CurrPen = 1;   // must be always 1 to MAX_HPPENS
      if (CurrPen > MAX_HPPENS)
         CurrPen = MAX_HPPENS;
      HPcpy.HpPens[CurrPen-1].color = RGB(red, green, blue);
   }

   return OK;
}

/******************************************************************************
* hp_sp
*/
static int hp_sp()
{
   int err;
   double p; 

   // pen up.
   plot = FALSE;
   if (!read_real(&p))
      return(0);

   int pen = round(p);
   CurrPen = ((pen-1) % (MAX_HPPENS -1)) + 1;

   if (CurrPen < 1)
      CurrPen = 1;   // must be always 1 to MAX_HPPENS
   if (CurrPen > MAX_HPPENS)
      CurrPen = MAX_HPPENS;

   CString tmp =  HPcpy.HpPens[CurrPen - 1].layer;
   CurrLayer = Graph_Level(tmp, /*pennumber,*/ prefix, 0);
   LayerStruct *l = doc->FindLayer(CurrLayer);
   l->setColor( check_background(HPcpy.HpPens[CurrPen - 1].color));

   if (NoLineWidth || HPcpy.HpPens[CurrPen-1].width <= SMALLWIDTH_SIZE)
      CurrPenIndex = 0;
   else
      CurrPenIndex = Graph_Aperture("", T_ROUND, HPcpy.HpPens[CurrPen-1].width, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE, &err);

   return OK;
}

/****************************************************************************
* hp_lo  
*   Label Origin 
*/
static int hp_lo()
{
   double p;

   if (!read_real(&p))
      return(0);
   label_origin = (int)p;

   return OK;
}

/****************************************************************************
* hp_np
*   Number of Pens
*/
static int hp_np()
{
   double p;

   if (!read_real(&p))
      return(0);
   number_of_pens = (int)p;

   if (number_of_pens > MAX_HPPENS)
      number_of_pens = MAX_HPPENS;

   return OK;
}

/****************************************************************************
*  hp_ro
*  Plot rotation
*/
static int hp_ro()
{
   double      p;

   if (!read_real(&p))
   {
      G_cur_rotation = 0;
      return(0);
   }

   switch (round(p))
   {
      case 90:
         G_cur_rotation = p;
      break;
      case 180:
         G_cur_rotation = p;
      break;
      case 270:
         G_cur_rotation = p;
      break;
      default:
         G_cur_rotation = 0;
      break;
   }

   return OK;
}

/******************************************************************************
* hp_pm
*/
static int hp_pm()
{
   double polymode;   // 0 1 2

   if (!read_real(&polymode))
      polymode = 0;

   // cur_polygonmode 2 is exit polygonmode
   cur_polygonmode = (int)polymode;
   switch (cur_polygonmode)
   {
      case 1:
         if (polygoncnt < MAX_POLY)
         {
            // if last polygon was plotted -> close it
            polygon[polygoncnt].typ = HP_PM1;
            polygon[polygoncnt].plotabs = plotabs;
            polygon[polygoncnt].plot = FALSE;
            polygoncnt++;
         }
         else
         {
            fprintf(flog, "Polygon overflow in HP\n");
            write_polygon(0, 0);
         }
         plot = FALSE;
      break;
      case 2:
         // ends polymode - plot buffer
         // here check for close
         // plot = FALSE;
         // needs to close polygon
         if (polygoncnt < MAX_POLY)
         {
            // if last polygon was plotted -> close it
            polygon[polygoncnt].typ = HP_PM2;
            polygon[polygoncnt].plotabs = plotabs;
            polygon[polygoncnt].plot = FALSE;
            polygoncnt++;
         }
         else
         {
            fprintf(flog, "Polygon overflow in HP\n");
            write_polygon(0, 0);
         }
         plot = FALSE;
      break;
      default: // 0
         // clear last buffer
         polygon[0].typ = HP_PM0;
         polygon[0].plot = FALSE;
         polygon[0].plotabs = TRUE;
         polygon[0].x = xold; // remember last pen koo before entering polygon mode
         polygon[0].y = yold;
         polygoncnt=1;
     break;
   }

   return OK;
}

/******************************************************************************
* hp_wu
*/
static int hp_wu()
{
   double wumode;   // 0 1
   int err;

   if (!read_real(&wumode))
      CurrWuMode = 0;

   CurrWuMode= (int)wumode;
   // need to reset PW command.
   if (NoLineWidth || HPcpy.HpPens[CurrPen-1].width <= SMALLWIDTH_SIZE)
      CurrPenIndex =0;
   else
      CurrPenIndex = Graph_Aperture("", T_ROUND, HPcpy.HpPens[CurrPen-1].width, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE, &err);
   return OK;
}

/******************************************************************************
* hp_fp
*/
static int hp_fp()
{
   // fp always plots the polygon in buffer.
   for (int p=0; p<polygoncnt; p++)                 
      polygon[p].plot = TRUE;

   write_polygon(1, 1);  // FP fills and writes the polygon
   return OK;
}

/******************************************************************************
* hp_ep
*/
static int hp_ep()
{
   write_polygon(0, 0);  // EP hollow and writes the polygon
   return OK;
}

/******************************************************************************
* hp_sc
* Scale has the paramter xmin, xmax, ymin, ymax
*/
static int hp_sc()
{
   //   return OK; // there is something wrong in my understanding of SC and IP
   abs_plot_f_x = 1.0;
   abs_plot_f_y = 1.0;
   hpplot_scaled = TRUE;

   if (!read_koo(&sc_x_1,&sc_x_2))
   {
//    ErrorMessage("Koo expected SC","Error");
      sc_x_1 = sc_y_1 = sc_x_2 = sc_y_2 = 0.0;
      return 0;
   }

   if (!read_koo(&sc_y_1,&sc_y_2))
   {
//    ErrorMessage("Koo expected SC","Error");
      sc_x_1 = sc_y_1 = sc_x_2 = sc_y_2 = 0.0;
      return 0;
   }

   hp_rotate(&sc_x_1, &sc_y_1);
   hp_rotate(&sc_x_2, &sc_y_2);
   
   if (fabs(ip_x_2-ip_x_1) > 0 && fabs(ip_y_2-ip_y_1) > 0 && fabs(sc_x_2-sc_x_1) > 0 && fabs(sc_y_2-sc_y_1) > 0)
   {
      abs_plot_f_x = ((ip_x_2-ip_x_1)/(sc_x_2-sc_x_1));
      abs_plot_f_y = ((ip_y_2-ip_y_1)/(sc_y_2-sc_y_1));
   }

   if (hpplot_scaled)
   {
      offset_x = ip_x_1;
      offset_y = ip_y_1;
   }
   return OK;
}

/******************************************************************************
* hp_ip
* Here get only the P1, P2 points and calc new scale.
*/
static int hp_ip()
{
   // return OK; // there is something wrong in my understanding of SC and IP
   if (!read_koo(&ip_x_1,&ip_y_1))
   {
//    ErrorMessage("Koo expected IP","Error");
      ip_x_1 = ip_y_1 = ip_x_2 = ip_y_2 = 0.0;
      return 0;
   }
   if (!read_koo(&ip_x_2,&ip_y_2))
   {
//    ErrorMessage("Koo expected IP","Error");
      ip_x_1 = ip_y_1 = ip_x_2 = ip_y_2 = 0.0;
      return 0;
   }

   hp_rotate(&ip_x_1, &ip_y_1);
   hp_rotate(&ip_x_2, &ip_y_2);

   // this seems to be correct/ may offset_x = ip_x_1 always if ip_x_1 == 0
   // but this needs to be tested.
   if (ip_x_1 < 0 || ip_y_1 < 0)
   {
      offset_x = (ip_x_2 + ip_x_1) /2;
      offset_y = (ip_y_2 + ip_y_1) /2;
   }
   else
   {
      offset_x = ip_x_1;
      offset_y = ip_y_1;
   }

   // here determine rotation
   CurrIpRotation = 0;
   // CurrIpRoation = 0
   //     P2
   // P1

   // CurrIpRotation = 1
   // P1
   //    P2
   if (ip_y_1 > ip_y_2) CurrIpRotation |= 1;
   //if (ip_y_1 > ip_y_2) G_cur_rotation = 90;

   // CUrrIpRotation = 2
   // P2
   //     P1
   if (ip_x_1 > ip_x_2) CurrIpRotation |= 2;
   //if (ip_x_1 > ip_x_2) G_cur_rotation = 180;

   // CurrIpRoation = 3
   //     P1
   // P2
   //hp_rotate(&offset_x, &offset_y);

   polygoncnt = 0;

   return OK;
}

/******************************************************************************
* hp_pa
* Plot absolute
*/
static int hp_pa()
{
   double x, y;

   plotabs = TRUE;
   while (TRUE)
   {
      if (!read_koo(&x, &y))
         return 0;

      hp_rotate(&x, &y);

      if (cur_polygonmode == 2)
      {
         x = xscale(x);
         y = yscale(y);

         if (!openpoly && plot)
         {
            Graph_PolyStruct(CurrLayer, 0, 0 );
            Graph_Poly(NULL, CurrPenIndex, 0, 0, 0);
            Graph_Vertex(xold, yold, 0);
            openpoly = TRUE;
         }
         if (plot)         
         {
            if (fabs(x - xold) > SMALLNUMBER || fabs(y - yold) > SMALLNUMBER)
               Graph_Vertex(x, y, 0);
         }
         xold = x;
         yold = y;
      }
      else
      {
         if (polygoncnt >= MAX_POLY)
         {
            fprintf(flog, "PA-Polyline overflow [%d] in HP", MAX_POLY);
            display_error++;
            write_polygon(0, 0);
         }
         polygon[polygoncnt].typ = HP_LINE;
         polygon[polygoncnt].plot= plot;
         polygon[polygoncnt].x = x;
         polygon[polygoncnt].y = y;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
      }
   }
   return TRUE;
}

/******************************************************************************
* hp_ra
* Plot absolute filled rectangle
*/
static int hp_ra()
{
   double x, y;
   int rmabs = plotabs;
   int rmplot = plot;

   plotabs = TRUE;
   plot = TRUE;

   while (TRUE)
   {
      if (!read_koo(&x, &y))
      {
         plot = rmplot;
         plotabs = rmabs;
         return 0;
      }
      hp_rotate(&x, &y);
      x = xscale(x);
      y = yscale(y);
      if (x == xold || y == yold)
      {
         write_polygon(0, 0);
         openpoly = FALSE;

         // plot a line, but not a zero segment
         if (x != xold || y != yold)   
         {
            polygoncnt = 0;
            polygon[polygoncnt].typ = HP_PM0;
            polygon[polygoncnt].plot= TRUE;
            polygon[polygoncnt].x = xold;
            polygon[polygoncnt].y = yold;
            polygoncnt++;
            polygon[polygoncnt].typ = HP_PM0;
            polygon[polygoncnt].plot= FALSE;
            polygon[polygoncnt].x = x;
            polygon[polygoncnt].y = y;
            polygoncnt++;
            Graph_Line(CurrLayer, xold, yold, x, y, 0, CurrPenIndex, FALSE);     
         }
      }
      else
      {
         //command EP can be called out after RA to outline the polygon, so save points in polygon for EP         
         polygoncnt = 0;
         polygon[polygoncnt].typ = HP_PM0;
         polygon[polygoncnt].plot= TRUE;
         polygon[polygoncnt].x = xold;
         polygon[polygoncnt].y = yold;
         polygoncnt++;

         polygon[polygoncnt].typ = HP_PM0;
         polygon[polygoncnt].plot= FALSE;
         polygon[polygoncnt].x = x;
         polygon[polygoncnt].y = yold;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;

         polygon[polygoncnt].typ = HP_PM0;
         polygon[polygoncnt].plot= FALSE;
         polygon[polygoncnt].x = x;
         polygon[polygoncnt].y = y;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;

         polygon[polygoncnt].typ = HP_PM0;
         polygon[polygoncnt].plot= FALSE;
         polygon[polygoncnt].x = xold;
         polygon[polygoncnt].y = y;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;

         polygon[polygoncnt].typ = HP_PM0;
         polygon[polygoncnt].plot= FALSE;
         polygon[polygoncnt].x = xold;
         polygon[polygoncnt].y = yold;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;

         Graph_PolyStruct(CurrLayer, 0, 0);
         Graph_Poly(NULL,CurrPenIndex, 1, 0, 1);
         Graph_Vertex(xold, yold, 0);
         Graph_Vertex(x, yold, 0);
         Graph_Vertex(x, y, 0);
         Graph_Vertex(xold, y, 0);
         Graph_Vertex(xold, yold, 0);
      }
   }
   plot = rmplot;
   plotabs = rmabs;
   return TRUE;
}

/****************************************************************************/
/* hp_ea
   Plot absolute hollow rectangle
   Clears Polygon buffer
*/
static int hp_ea()
{
   double x, y;
   int rmabs = plotabs;
   int rmplot = plot;

   //write_polygon(0);
   polygoncnt = 0;   // clears buffer.
   plotabs = TRUE;
   plot = TRUE;
   while (TRUE)
   {
      if (!read_koo(&x, &y))
      {
         plot = rmplot;
         plotabs = rmabs;
         return 0;
      }
      hp_rotate(&x, &y);
      x = xscale(x);
      y = yscale(y);
      if (x == xold || y == yold)
      {
         write_polygon(0, 0);
         openpoly = FALSE;

         // plot a line, but not a zero segment
         if (x != xold || y != yold)
            Graph_Line(CurrLayer, xold, yold, x, y, 0, CurrPenIndex, FALSE);
      }
      else
      {
         Graph_PolyStruct(CurrLayer, 0, 0);
         Graph_Poly(NULL, CurrPenIndex, 0, 0, 1);
         Graph_Vertex(xold, yold, 0);
         Graph_Vertex(x, yold, 0);
         Graph_Vertex(x, y, 0);
         Graph_Vertex(xold, y, 0);
         Graph_Vertex(xold, yold, 0);
      }
   }
   plot = rmplot;
   plotabs = rmabs;
   return TRUE;
}

/****************************************************************************/
/* hp_rr
   Plot relative filled rectangle
   clears polygon buffer
*/
static int hp_rr()
{
   double x, y;
   int rmabs = plotabs;
   int rmplot = plot;

   write_polygon(0, 0);
   polygoncnt = 0;
   plotabs = FALSE;
   plot = TRUE;
   while (TRUE)
   {
      if (!read_koo(&x, &y))
      {
         plot = rmplot;
         plotabs = rmabs;
         return 0;
      }
      hp_rotate(&x, &y);
      if (cur_polygonmode == 2)
      {
         x = xscale(x);
         y = yscale(y);
         if (x == xold || y == yold)
         {
            write_polygon(0, 0);
            openpoly = FALSE;

            // plot a line, but not a zero segment
            if (x != xold || y != yold)
               Graph_Line(CurrLayer, xold,yold, x, y, 0, CurrPenIndex, FALSE);
         }
         else
         {
            Graph_PolyStruct(CurrLayer, 0, 0);
            Graph_Poly(NULL,CurrPenIndex, 1, 0, 1);
            Graph_Vertex(xold, yold, 0);
            Graph_Vertex(x, yold, 0);
            Graph_Vertex(x, y, 0);
            Graph_Vertex(xold, y, 0);
            Graph_Vertex(xold, yold, 0);
         }
      }
      else
      {
         polygon[polygoncnt].typ = HP_RR;
         polygon[polygoncnt].plot= plot;
         polygon[polygoncnt].x = xold;
         polygon[polygoncnt].y = yold;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
         polygon[polygoncnt].typ = HP_RR;
         polygon[polygoncnt].plot= plot;
         polygon[polygoncnt].x = x;
         polygon[polygoncnt].y = y;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
      }
   }
   plot = rmplot;
   plotabs = rmabs;
   return TRUE;
}

/****************************************************************************/
/* hp_er
   Plot relative hollow rectangle
   clears polygon buffer
*/
static int hp_er()
{
   double x, y;
   int rmabs = plotabs;
   int rmplot = plot;

   write_polygon(0, 0);
   polygoncnt = 0;
   plotabs = FALSE;
   plot = TRUE;
   while (TRUE)
   {
      if (!read_koo(&x, &y))
      {
         plot = rmplot;
         plotabs = rmabs;
         return 0;
      }
      hp_rotate(&x, &y);
      if (cur_polygonmode == 2)
      {
         x = xscale(x);
         y = yscale(y);
         if (x == xold || y == yold)
         {
            write_polygon(0,0);
            openpoly = FALSE;

            // plot a line, but not a zero segment
            if (x != xold || y != yold)
               Graph_Line(CurrLayer, xold,yold, x, y, 0, CurrPenIndex, FALSE);
         }
         else
         {
            Graph_PolyStruct(CurrLayer, 0, 0);
            Graph_Poly(NULL,CurrPenIndex, 1, 0, 0);
            Graph_Vertex(xold, yold, 0);
            Graph_Vertex(x, yold, 0);
            Graph_Vertex(x, y, 0);
            Graph_Vertex(xold, y, 0);
            Graph_Vertex(xold, yold, 0);
         }
      }
      else
      {
         polygon[polygoncnt].typ = HP_ER;
         polygon[polygoncnt].plot= plot;
         polygon[polygoncnt].x = xold;
         polygon[polygoncnt].y = yold;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
         polygon[polygoncnt].typ = HP_RR;
         polygon[polygoncnt].plot= plot;
         polygon[polygoncnt].x = x;
         polygon[polygoncnt].y = y;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
      }
   }
   plot = rmplot;
   plotabs = rmabs;
   return TRUE;
}

/******************************************************************************
* hp_ar
*/
static int hp_ar()
{
   double x, y, da;
   double chord, radius, sa;
   int oldabs = plotabs;

   plotabs = FALSE;
   plot = TRUE;
   if (!read_koo(&x, &y)) // center coos
   {
      plotabs = oldabs;
      return 0;
   }
   hp_rotate(&x, &y);
   x = xscale(x);
   y = yscale(y);
   if (read_real(&da)) // arc in degree
   {
      openpoly = FALSE;
      radius = sqrt((x-xold) * (x-xold) + (y-yold) * (y-yold) );
      if (fabs(fabs(da) - 360) < 1)
      {
         // circle
         if (circle_is_aperture(radius))
         {
            double size_a = radius * 2;
            CString n;
            int err;

            n.Format("ROUND_%2.3lf", size_a);
            Graph_Aperture(n, T_ROUND, size_a, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE | BL_GLOBAL, TRUE, &err);  
            Graph_Block_On(GBO_APPEND, n, -1, 0);  // acad allows the reference of a block before defintion
            Graph_Block_Off();
            Graph_Block_Reference(n, NULL, 0, x, y, 0.0, 0, 1.0, CurrLayer, TRUE);
         }
         else
            Graph_Circle(CurrLayer, x, y, radius, 0, CurrPenIndex, 0, FALSE);
      }
      else
      {
         sa = ArcTan2(yold-y, xold-x);    // start angle
         da = DegToRad(da);         // end angle
         if (fabs(da) > DegToRad(1))   // must be 1 degree accuracy
            Graph_Arc(CurrLayer, x, y, radius, sa, da, 0, CurrPenIndex, FALSE);
         xold = x + radius * cos(sa+da);
         yold = y + radius * sin(sa+da);
      }
      read_real(&chord); // I do not need that
   }
   plotabs = oldabs;
   return TRUE;
}

/******************************************************************************
* hp_aa
*/
static int hp_aa()
{
   int oldplot = plot;
   double centerX, centerY;
   double centerXinHP, centerYinHP;

   plot = TRUE;
   plotabs = TRUE;
   if (!read_koo(&centerX, &centerY)) // center coordinates
      return 0;

   hp_rotate(&centerX, &centerY);
   centerXinHP = centerX;
   centerYinHP = centerY;
   centerX = xscale(centerX);
   centerY = yscale(centerY);

   double da;
   if (!read_real(&da)) // arc in degree
      return TRUE;

   double chord; // not used
   read_real(&chord);

   if (CurrIpRotation)
      da = -da;

   double radius = sqrt( (centerX - xold) * (centerX-xold) + (centerY - yold) * (centerY - yold) );
   if (fabs(fabs(da) - 360) < 1) // if circle
   {
      // circle
      if (cur_polygonmode == 2)
      {
         //write_polygon(0);
         openpoly = FALSE;

         if (circle_is_aperture(radius))
         {
            double size_a = radius * 2;
            CString n;
            int err;

            n.Format("ROUND_%2.3lf", size_a);
            Graph_Aperture(n, T_ROUND, size_a, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE | BL_GLOBAL, TRUE, &err);  
            Graph_Block_On(GBO_APPEND, n, -1, 0);  // acad allows the reference of a block before defintion
            Graph_Block_Off();
            Graph_Block_Reference(n, NULL, 0, centerX, centerY, 0.0, 0, 1.0, CurrLayer, TRUE);
         }
         else
            Graph_Circle(CurrLayer, centerX, centerY, radius, 0L, CurrPenIndex, 0, FALSE);
      }
      else
      {
         // store in PM
      }
      return TRUE;
   }

   double sa = atan2(yold - centerY, xold - centerX); // start angle

   while (sa < 0)
      sa += PI2;

   da = DegToRad(da); // sweep angle

   if (fabs(da) < DegToRad(1)) // if sweep is less than 1 degree, put a line
   {
      double endX = centerX + radius * cos(sa+da);
      double endY = centerY + radius * sin(sa+da);

      if (cur_polygonmode == 2)
      {
         if (!openpoly && plot)
         {
            Graph_PolyStruct(CurrLayer, 0, 0);
            Graph_Poly(NULL, CurrPenIndex, 0, 0, 0);
            Graph_Vertex(xold, yold, 0);
            openpoly = TRUE;
         }
         if (plot)         
         {
            if (fabs(endX - xold) > SMALLNUMBER || fabs(endY - yold) > SMALLNUMBER)
               Graph_Vertex(endX, endY, 0);
         }
         xold = endX;
         yold = endY;
      }
      else
      {
         if (polygoncnt >= MAX_POLY)
         {
            fprintf(flog, "AA-Polyline overflow [%d] in HP", MAX_POLY);
            display_error++;
            write_polygon(0, 0);
         }
         polygon[polygoncnt].typ = HP_LINE;
         polygon[polygoncnt].plot= plot;
         polygon[polygoncnt].x = endX;
         polygon[polygoncnt].y = endY;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
      }
      return TRUE;
   }

   if (cur_polygonmode == 2)
   {
      int first = TRUE;
      if (openpoly && oldplot)
         first = FALSE;

      double bulge = tan(da/4);
      double ex = centerX + radius * cos(sa+da);
      double ey = centerY + radius * sin(sa+da);

      if (first)
      {
         Graph_PolyStruct(CurrLayer, 0, 0);  // start a new poly
         Graph_Poly(NULL, CurrPenIndex, 0, 0, 0);
         openpoly = TRUE;
      }
      Graph_Vertex(xold, yold, bulge); // here may be a zero element, if the poly was already started.      
      Graph_Vertex(ex, ey, 0); // here need to get last vertex
   }
   else
   {
      // store in PM
      if (polygoncnt+2 >= MAX_POLY)
      {
         fprintf(flog, "PA-Polyline overflow [%d] in HP", MAX_POLY);
         display_error++;
         write_polygon(0, 0);
      }
      polygon[polygoncnt].typ = HP_ARC;
      polygon[polygoncnt].plot= plot;
      polygon[polygoncnt].x = centerXinHP;  // it will get the right scale when writing polygon (write_polygon) - Joanne  
      polygon[polygoncnt].y = centerYinHP;
      polygon[polygoncnt].radius = radius;
      polygon[polygoncnt].plotabs = plotabs;
      polygoncnt++;
      polygon[polygoncnt].typ = HP_ARC;
      polygon[polygoncnt].plot= plot;
      polygon[polygoncnt].x = sa;
      polygon[polygoncnt].y = da;
      polygon[polygoncnt].plotabs = plotabs;
      polygoncnt++;
   }

   xold = centerX + radius * cos(sa+da);
   yold = centerY + radius * sin(sa+da);
   return TRUE;
}

/******************************************************************************
* hp_ci
*/
static int hp_ci()
{
   double radius;
   int plot_mode = plotabs;
   double x, y;

   // make it absolute
   plotabs = TRUE;
   read_real(&radius);
   radius = _scale(radius);

   if (radius == 0)  
   {
      plotabs = plot_mode;
      return OK;
   }

   if (cur_polygonmode == 2)
   {
      //write_polygon(0);  // does not execute polygon buffer. This buffer changes xold, yold.
                           // cur_polygonmode 2 is stop buffer !
      openpoly = FALSE;
      if (circle_is_aperture(radius))
      {
         double size_a = radius * 2;
         CString n;
         int err;

         n.Format("ROUND_%2.3lf", size_a);
         Graph_Aperture(n, T_ROUND , size_a, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE | BL_GLOBAL, TRUE, &err);  
         Graph_Block_On(GBO_APPEND, n, -1, 0);  // acad allows the reference of a block before defintion
         Graph_Block_Off();
         Graph_Block_Reference(n, NULL, 0, xold, yold, 0.0, 0, 1.0, CurrLayer, TRUE);
      }
      else
         Graph_Circle(CurrLayer, xold, yold, radius, 0, CurrPenIndex, 0, FALSE);
   }
   else
   {

      if (polygoncnt < MAX_POLY-3)
      {
         // circle closed first polygon
         polygon[polygoncnt].typ = HP_CI;
         if (polygoncnt > 1)
         {
            x = polygon[polygoncnt-2].x;
            y = polygon[polygoncnt-2].y;
         }
         else
         {
            x = xold;
            y = yold;
         }

         if (CurrIpRotation & 1)
            y = -y;
         if (CurrIpRotation & 2)
            x = -x;

         polyxold = polygon[polygoncnt].x   = x;
         polyyold = polygon[polygoncnt].y   = y;
         polygon[polygoncnt].radius = radius;
         polygon[polygoncnt].plot= TRUE;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
     }
      else
      {
         fprintf(flog, "PA-Polyline overflow [%d] in HP", MAX_POLY);
         display_error++;
         write_polygon(0, 0);
      }
   }
   plotabs = plot_mode;
   return OK;
}

/******************************************************************************
* hp_cp
*/
static int hp_cp()
{
   double  x;

   cp_spaces = 0;
   cp_lines = 0;
   if (read_real(&x))
   {
      cp_spaces = x;
      xold += x * AktTextWidth;  // character plot cell with is 1.5 of char, this is already calculated in _si
      if (read_real(&x))
      {
         cp_lines = x * 1.2;  // linefeed from char size. According to HP Manual
         yold += x * AktTextHeight * 2; // next line is 2 of char.
      }
   }
   return OK;
}

/****************************************************************************
* hp_wg
* Filled Wedge
*/
static int hp_wg()
{
   double radius;
   double sa, da, chord;
   double x, y;

   // make it absolute
   read_real(&radius);
   radius = _scale(radius); // in degrees
   read_real(&sa);
   read_real(&da);
   read_real(&chord);

   if (cur_polygonmode == 2)
   {
      write_polygon(0, 0);
      openpoly = FALSE;

      //fprintf(flog,"WG needs to be filled\n");
      if (fabs(fabs(da) - 360) < 1) // check is nearly 360
      {
         if (circle_is_aperture(radius))
         {
            double size_a = radius * 2;
            CString n;
            int err;

            n.Format("ROUND_%2.3lf", size_a);
            Graph_Aperture(n, T_ROUND, size_a, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE | BL_GLOBAL, TRUE, &err);  
            Graph_Block_On(GBO_APPEND, n, -1, 0);  // acad allows the reference of a block before defintion
            Graph_Block_Off();
            Graph_Block_Reference(n, NULL, 0, xold, yold, 0.0, 0, 1.0, CurrLayer, TRUE);
         }
         else
            Graph_Circle(CurrLayer, xold, yold, radius, 0, CurrPenIndex,0, TRUE);  // TRUE = filled
      }
      else
      {
         da = DegToRad(da);         // end angle
         sa = DegToRad(sa);

         int fill = TRUE;     // fill type not evaluated.
         int closed = TRUE;
         double bulge = tan(da/4);
         double x1 = xold + radius * cos(sa);
         double y1 = yold + radius * sin(sa);
         double x2 = xold + radius * cos(sa+da);
         double y2 = yold + radius * sin(sa+da);

         Graph_PolyStruct(CurrLayer, 0, 0);
         Graph_Poly(NULL, CurrPenIndex, fill, 0, closed);
         Graph_Vertex(xold, yold, 0);
         Graph_Vertex(x1, y1, bulge);
         Graph_Vertex(x2, y2, 0);
         Graph_Vertex(xold, yold, 0);
      }
   }
   else
   {
      if (fabs(fabs(da) - 360) > 1)
        return OK;  // Filled ARC's polymode is not supported
      if (polygoncnt < MAX_POLY-3)
      {
         // circle closed first polygon
         close_polygon();
         polygon[polygoncnt].typ = HP_CI_FILLED;

         x = polygon[polygoncnt-2].x;
         y = polygon[polygoncnt-2].y;

         if (CurrIpRotation & 1)
            y = -y;
         if (CurrIpRotation & 2)
            x = -x;

         polyxold = polygon[polygoncnt].x   = x;
         polyyold = polygon[polygoncnt].y   = y;
         polygon[polygoncnt].radius = radius;
         polygon[polygoncnt].plot= TRUE;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
         // now make a now poly start
         polygon[polygoncnt].typ = HP_PM0;
         polygon[polygoncnt].x = polyxold;
         polygon[polygoncnt].y = polyyold;
         polygon[polygoncnt].plot = FALSE;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
      }
      else
      {
         fprintf(flog, "PA-Polyline overflow [%d] in HP", MAX_POLY);
         display_error++;
         write_polygon(0, 0);
      }
   }
   return OK;
}

/****************************************************************************
* hp_ew
* Empty Wedge
*/
static int hp_ew()
{
   double radius;
   double sa, da, chord;
   double x, y;

   // make it absolute
   read_real(&radius);
   radius = _scale(radius); // in degrees
   read_real(&sa);
   read_real(&da);
   read_real(&chord);

   if (cur_polygonmode == 2)
   {
      write_polygon(0, 0);
      openpoly = FALSE;
      if ((fabs(da) - 360) < 1)
      {
         if (circle_is_aperture(radius))
         {
            double size_a = radius * 2;
            CString n;
            int err;

            n.Format("ROUND_%2.3lf", size_a);
            Graph_Aperture(n, T_ROUND , size_a, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE | BL_GLOBAL, TRUE, &err);  
            Graph_Block_On(GBO_APPEND, n, -1, 0);  // acad allows the reference of a block before defintion
            Graph_Block_Off();
            Graph_Block_Reference(n, NULL, 0, xold, yold, 0.0, 0, 1.0, CurrLayer, TRUE);
         }
         else
            Graph_Circle(CurrLayer, xold, yold, radius, 0, CurrPenIndex, 0, FALSE);  // FALSE = filled
      }
      else
      {
         da = DegToRad(da);         // end angle
         sa = DegToRad(sa);
         Graph_Arc(CurrLayer, xold, yold, radius, sa, da, 0, CurrPenIndex, FALSE);
     }
   }
   else
   {
     if ((fabs(da) - 360) > 1) return OK;  // Filled ARC's polymode is not supported
      if (polygoncnt < MAX_POLY-3)
      {
         // circle closed first polygon
         close_polygon();
         polygon[polygoncnt].typ = HP_CI;
         x = polygon[polygoncnt-2].x;
         y = polygon[polygoncnt-2].y;

         if (CurrIpRotation & 1)
            y = -y;
         if (CurrIpRotation & 2)
            x = -x;

         polyxold = polygon[polygoncnt].x   = x;
         polyyold = polygon[polygoncnt].y   = y;
         polygon[polygoncnt].radius = radius;
         polygon[polygoncnt].plot= TRUE;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
         // now make a now poly start
         polygon[polygoncnt].typ = HP_PM0;
         polygon[polygoncnt].x = polyxold;
         polygon[polygoncnt].y = polyyold;
         polygon[polygoncnt].plot = FALSE;
         polygon[polygoncnt].plotabs = plotabs;
         polygoncnt++;
      }
      else
      {
         fprintf(flog, "PA-Polyline overflow [%d] in HP", MAX_POLY);
         display_error++;
         write_polygon(0, 0);
      }
   }
   return OK;
}

/****************************************************************************
* hp_pu
* pen up
*/
static int hp_pu()
{
   double x, y;

   plot = FALSE;
   openpoly = FALSE;

   while (TRUE)
   {
      if (!read_koo(&x, &y))
         return 0;

      hp_rotate(&x, &y);
      if (cur_polygonmode == 2)
      {
         xold = xscale(x);
         yold = yscale(y);
      }
      else
      {
         if (polygoncnt < MAX_POLY)
         {
            polygon[polygoncnt].typ = HP_LINE;
            polygon[polygoncnt].x   = x;
            polygon[polygoncnt].y   = y;
            polygon[polygoncnt].plot= plot;
            polygon[polygoncnt].plotabs = plotabs;
            polygoncnt++;
         }
         else
         {
            fprintf(flog, "PU-Polyline overflow [%d] in HP", MAX_POLY);
            display_error++;
            write_polygon(0, 0);
         }
      }
   }
   return OK;
}

/****************************************************************************
* hp_pr
* This is plot relative
*/
static int hp_pr()
{
   double x, y;

   plotabs = FALSE;
   while (TRUE)
   {
      if (!read_koo(&x,&y))
         return 0;

      hp_rotate(&x, &y);
      if (cur_polygonmode == 2)
      {
         x = xscale(x);
         y = yscale(y);
         if (!openpoly && plot)
         {
            Graph_PolyStruct(CurrLayer, 0, 0 );
            Graph_Poly(NULL,  CurrPenIndex, 0, 0, 0);
            Graph_Vertex(xold, yold, 0);
            openpoly = TRUE;
         }
         if (plot)         
            Graph_Vertex(x, y, 0);
         xold = x;
         yold = y;
      }
      else
      {
         if (polygoncnt < MAX_POLY)
         {
            polygon[polygoncnt].typ = HP_LINE;
            polygon[polygoncnt].x   = x;
            polygon[polygoncnt].y   = y;
            polygon[polygoncnt].plot= plot;
            polygon[polygoncnt].plotabs = plotabs;
            polygoncnt++;
         }
         else
         {
            fprintf(flog, "PR-Polyline overflow [%d] in HP", MAX_POLY);
            display_error++;
            write_polygon(0, 0);
         }
     }
   }
   return TRUE;
}

/****************************************************************************
* hp_si
* Absolute Character Size
*/
static int hp_si()
{
   double w, h;

   G_textmirror = FALSE;
   if (!read_real(&w))
      return TRUE;

   if (w < 0)
   {
      G_textmirror = TRUE;
      w = fabs(w);
   }

   AktTextWidth = w * CMscale;

   // the space from one char to the next is done with a 10% spaceratio in CAMCAD
   AktTextWidth = AktTextWidth * CHAR_SPACERATIO;   // hp width is width without spacing
   if (!read_real(&h))
      return TRUE;

   AktTextHeight = h * CMscale;
   return OK;
}

/****************************************************************************/
/* hp_sr 
      Relative Character Size
      ACTWIDTH = WIDTH/100 * (px2 - px1);
      AktHeight = height/100 *(py2-py1);

      (ip_x_2 + ip_x_1)
      (ip_y_2 + ip_y_1)

*/
static int hp_sr()
{
   double w, h;

   G_textmirror = FALSE;
   if (!read_real(&w))
      return TRUE;

   if (w < 0)
   {
      G_textmirror = TRUE;
      w = fabs(w);
   }

   if (fabs(sc_x_2 - sc_x_1) > 0)
      w = (w/100) * fabs(sc_x_2 - sc_x_1);
   else
   if (fabs(ip_x_2 - ip_x_1) > 0)
      w = (w/100) * fabs(ip_x_2 - ip_x_1);
   
   // the HP text is proportional, but the exact proportional definition between CAMCAD, Windows and
   // HP text fonts may vary, and therefore it is not all the time on the mark.
    AktTextWidth = w * fabs(abs_plot_f_x) * HPscale;  // hp width is width without spacing
   AktTextHeight = AktTextWidth*1.5;                 // just default here. 

   if (!read_real(&h))
      return TRUE;

   if (fabs(sc_y_2 - sc_y_1) > 0)
      h = (h/100) * fabs(sc_y_2 - sc_y_1);
   else
   if (fabs(ip_y_2 - ip_y_1) > 0)
      h = (h/100) * fabs(ip_y_2 - ip_y_1);
   
   AktTextHeight = h * fabs(abs_plot_f_y) * HPscale;  
   hp_rotate(&AktTextHeight, &AktTextWidth);
   AktTextHeight = fabs(AktTextHeight);
   AktTextWidth = fabs(AktTextWidth) * CHAR_SPACERATIO;
   return OK;
}

/****************************************************************************
* hp_pj
  Advance to next line

   .%-12345X@PJL JOB NAME = "Cover_Page.dwg"
   @PJL SET RENDERMODE= COLOR
   @PJL SET RESOLUTION=300
   @PJL SET PAPERLENGTH= 24725
   @PJL SET PAPERWIDTH= 15404
   @PJL SET ORIENTATION= PORTRAIT
   @PJL ENTER LANGUAGE = HPGL2
*/
static int hp_pj()
{
   char  c;

   while (TRUE)
   {
      if ((c = fgetc(ifp)) == EOF)
         return FALSE;
      if (c == 0x0A) 
         break;
   }
   return TRUE;
}

/******************************************************************************
* hp_lb
*/
static int hp_lb()
{
   char c;
   char tmp[400+1];
   int tmpcnt=0;
   double xtext = 0;
   double ytext = 0;
   double xoff = 0;

   while (TRUE)
   {
      if ((c = fgetc(ifp)) == EOF)
         return FALSE;
      if (c == label_terminator)
      {
         tmp[tmpcnt] = '\0';
         // here write label
         if (strlen(tmp))
         {
            switch (label_origin)
            {
               case 11:
               case 1:  // BB
                  // nothing
               break;
               case 12:
               case 2:  // BC
                  ytext -= AktTextHeight / 2;
               break;
               case 13:
               case 3:  // BT
                  ytext -= AktTextHeight;
               break;
               case 14:
               case 4:  // CB
                  xtext -= strlen(tmp) * AktTextWidth / 2;
               break;
               case 15:
               case 5:  // CC
                  xtext -= strlen(tmp) * AktTextWidth / 2;
                  ytext -= AktTextHeight / 2;
               break;
               case 16:
               case 6:  // CT
                  xtext -= strlen(tmp) * AktTextWidth / 2;
                  ytext -= AktTextHeight;
               break;
               case 17:
               case 7:  // TB
                  xtext -= strlen(tmp) * AktTextWidth;
               break;
               case 18:
               case 8:  // TC
                  xtext -= strlen(tmp) * AktTextWidth;
                  ytext -= AktTextHeight / 2;
               break;
               case 19:
               case 9:  // TT
                  xtext -= strlen(tmp) * AktTextWidth;
                  ytext -= AktTextHeight;
               break;
            }
            // kill all non printable characters from the end of the string
            while ( strlen(tmp) && !isprint( tmp[strlen(tmp)-1] ))
               tmp[strlen(tmp)-1] = '\0'; 
            
            if (strlen(tmp))
            {
               double x, y;
               Rotate(xtext, ytext, RadToDeg(textrot), &x, &y);
               x += xold;
               y += yold;

               // CP command
               x += (cp_spaces * AktTextWidth);
               y += (cp_lines * AktTextHeight);

               double tr = textrot + DegToRad(G_cur_rotation);

               Graph_Text(CurrLayer, tmp, x, y, AktTextHeight, AktTextWidth, tr, 0, proportional, G_textmirror, 
                     textoblique, FALSE, CurrPenIndex, 0); // supposed to be h,w,ang,charwidth,flg
            }
         }
         break;
      }
      else
      {
         if (tmpcnt < 400)
          tmp[tmpcnt++] = c;
      }
   }

   return OK;
}

/****************************************************************************/
/* hp_sd
   SD/Heletica;
   this is a command I have seen, which is different from the book.
*/
static int hp_sd()
{
   char  c;

   while (TRUE)
   {
      if ((c = fgetc(ifp)) == EOF)
         return FALSE;
      if (c == ';')  break;
   }           
   return OK;
}

/******************************************************************************
* hp_encode
   return a koo from an encoded PE command
*/
long hp_encode(int *encodedCharacter, int bit7mode)
{
   long value = 0;
   int character = *encodedCharacter;

   if (bit7mode)
   {
      BOOL TerminatorFound = FALSE;
      int shiftAmount = 0;

      while (!TerminatorFound)
      {
         if (character >= 63 && character <= 254)
         {
            if (character >= 95)
            {
               character = character - 32;
               TerminatorFound = TRUE;
            }

            long tempValue = character - 63; // needs long not to overflow
            value += tempValue << shiftAmount;
            shiftAmount += 5;
         }
         else
         {
            fprintf(flog, "Unknown PE command [%d]", character);
            display_error++;
         }

         // get next character
         if ((character = fgetc(ifp)) == EOF)
         {
            ErrorMessage("Unexpected EOF", "HPGL Error", MB_OK | MB_ICONHAND);
            file_read_error = TRUE;
            return FALSE;
         }
      }
   }
   else
   {
      BOOL TerminatorFound = FALSE;
      int shiftAmount = 0;

      while (!TerminatorFound)
      {
         if (character >= 63 && character <= 254)
         {
            if (character >= 191)
            {
               character -= 128;
               TerminatorFound = TRUE;
            }

            long tempValue = (character - 63);
            value += tempValue << shiftAmount;
            shiftAmount += 6;
         }
         else
         {
            fprintf(flog, "Unknown PE command [%d]", character);
            display_error++;
         }

         // get next character
         if ((character = fgetc(ifp)) == EOF)
         {
            ErrorMessage("Unexpected EOF", "HPGL Error", MB_OK | MB_ICONHAND);
            file_read_error = TRUE;
            return FALSE;
         }
      }
   }

   if (value & 1) // check for negative
      value = -(value - 1); // take off negative bit and make x negative

   value = value / 2; // shift over to ignore sign bit (least significant bit)
   *encodedCharacter = character;
   return value;
}

/******************************************************************************
* hp_pe
*/
static int hp_pe()
{
   int bit7mode = FALSE;
   int pabs = plotabs;
   int num_of_frac = 0;
   double x, y;
   BOOL done = FALSE;
   BOOL first = TRUE;

   plot = TRUE;      // All koos which are not penup are pendown
   plotabs = FALSE;  //it is relative unless = is set.

   while (!done)
   {
      int c = fgetc(ifp);
      if (c == EOF)
         return FALSE;

      // 7 bit mode has the 8th bit as parity
      if (bit7mode)
         c = c & 0x7F;

      switch (c)
      {
      case ';':   // this is the end of a command, but a command gets extecuted while reading
         done = TRUE;
         break;
      case '7':
         bit7mode = TRUE;
         break;
      case '8':
         bit7mode = FALSE;
         break;
      case '=':
         plotabs = TRUE;
         break;
      case ':':
         // select pen
         if ((c = fgetc(ifp)) == EOF)
         {
            ErrorMessage("Unexpected EOF", "HPGL Error", MB_OK | MB_ICONHAND);
            // big error
            return FALSE;
         }
         x = hp_encode(&c, bit7mode);
         if (ungetc(c, ifp) == EOF)
         {
            ErrorMessage("Problem with unget");
            return -1;
         }
         // unget
         break;
      case '<':   // pen up
         plot = FALSE;
         first = TRUE;
         break;
      case '>':
         {
            // number of fractions
            double frac = hp_encode(&c, bit7mode);

            // unget
            if (ungetc(c, ifp) == EOF)
            {
               ErrorMessage("Problem with unget");
               return -1;
            }

            if (frac != 1)
            {
               fprintf(flog, "Fraction not evaluated [%1.1lf]", frac);
               display_error++;
            }
         }
         break;
      default:
         // get 1 endoced pair
         if (c >= 63 && c <= 254)
         {
            x = hp_encode(&c, bit7mode);
            y = hp_encode(&c, bit7mode);

            // unget
            if (ungetc(c, ifp) == EOF)
            {
               ErrorMessage("Problem with unget");
               return -1;
            }

            // no output
            hp_rotate(&x, &y);

            if (cur_polygonmode == 2)
            {
               x = xscale(x);
               y = yscale(y);

               if (plot)
               {
                  if (fabs(x - xold) > SMALLNUMBER || fabs(y - yold) > SMALLNUMBER)
                  {
                     if (first)
                     {
                        Graph_PolyStruct(CurrLayer, 0, 0);
                        Graph_Poly(NULL, CurrPenIndex, 0, 0, 0);
                        Graph_Vertex(xold, yold, 0);
                        first = FALSE;
                     }

                     Graph_Vertex (x, y, 0);
                  }
               }

               xold = x;
               yold = y;
            }
            else
            {
               if (polygoncnt >= MAX_POLY)
               {
//                   ErrorMessage("PE-Polygon overflow in HP","Warning");
                  write_polygon(0, 0);
               }
               polygon[polygoncnt].typ = HP_LINE;
               polygon[polygoncnt].plot= plot;
               polygon[polygoncnt].x = x;
               polygon[polygoncnt].y = y;
               polygon[polygoncnt].plotabs = plotabs;
               polygoncnt++;
            }
            plotabs = FALSE;
            plot = TRUE;
         }
         else
         {
            fprintf(flog, "Unknown PE command [%d]", c);
            display_error++;
         }
         break;
      }
   }
   
   plotabs = pabs;

   return OK;
}

/******************************************************************************
* hp_dt
*/
static int hp_dt()
{
   char c;
   int cnt = 0;

   if ((c = fgetc(ifp)) == EOF)
      return FALSE;
   if (c == ';')
      return OK;

   // 0, 5, 27, 59 is not allowed
   if (c != 0 && c != 5 && c != 27 && c != 59)
      label_terminator = c;   

   // here even that the book says it must end with a ;
   // here are examples where it doesn't.
   cnt++;
   while (TRUE)
   {
      if ((c = fgetc(ifp)) == EOF)
         return FALSE;
      if (c == ';')
         return OK;

      // do not store label_term is it does not pass the test.
      if (cnt && isalpha(c))  // do not check next label char
      {
         // push it back on streem
         if (ungetc(c,ifp) == EOF)
         {
            ErrorMessage("Problem with unget");
            return -1;
         }
         return OK;
      }
      cnt++;
   }
   return OK;
}

/******************************************************************************
* hp_pd
* Plot pen down.
*/
static int hp_pd()
{
   double x, y;
   int first = TRUE;
   int oldplot = plot;

   plot = TRUE;
   while (TRUE)
   {
      if (!read_koo(&x, &y))
      {
/* just a pen down does not do nothing.
         if (first)
         {
            Graph_PolyStruct(CurrLayer, CurrPenIndex, 0,0); // start a new poly
            Graph_Poly(NULL, 0,0,0);
            Graph_Vertex(xold, yold, 0);
            openpoly = TRUE;
         }
*/
         return 0;
      }
      hp_rotate(&x, &y);

      if (cur_polygonmode == 2)
      {
         // this is to optimize PDx,yPDx,yPDx,y
         // if is was pendown and a poly was started, do not do it again.
         if (oldplot && openpoly)
            first = FALSE;

         if (first)
         {
            Graph_PolyStruct(CurrLayer, 0, FALSE);
            Graph_Poly(NULL, CurrPenIndex, FALSE, FALSE, FALSE);
            Graph_Vertex(xold, yold, 0);
            openpoly = TRUE;
         }
         first = FALSE;
         x = xscale(x);
         y = yscale(y);

         if (fabs(x - xold) > SMALLNUMBER || fabs(y - yold) > SMALLNUMBER)
            Graph_Vertex(x, y, 0);
         xold = x;
         yold = y;
      }
      else
      {
         // polygon mode
         if (polygoncnt < MAX_POLY)
         {
            polygon[polygoncnt].typ = HP_LINE;
            polygon[polygoncnt].plot= plot;
            polygon[polygoncnt].plotabs = plotabs;
            polygon[polygoncnt].x = x;
            polygon[polygoncnt].y = y;
            polygoncnt++;
         }
         else
         {
//          ErrorMessage("PD-Polygon overflow in HP","Warning");
            write_polygon(0, 0);
         }
      }
   }
   return OK;
}

/******************************************************************************
* double _scale
*/
double _scale(double x)
{
   // HPscale takes care of this
   return(x * abs_plot_f_x * HPscale);
}

/******************************************************************************
* hp_rotate
*/
void hp_rotate(double *x, double *y)
{
   double tmp;
   double x1 = *x;
   double y1 = *y;
   int rot = round(G_cur_rotation / 90);

   for (int i=0; i<rot; i++)
   {
      tmp = *y;
      *y = *x;
      *x = -tmp;
   }
   x1 = *x;
   y1 = *y;
}

/******************************************************************************
* double xscale
*/
double xscale(double x)
{
   // HPscale takes care of this
   //if (CurrIpRotation & 2) x = -x;
   double xx;
   if (plotabs)
      xx = ((x * abs_plot_f_x + offset_x) * HPscale);
   else
      xx = ((x * abs_plot_f_x) * HPscale + xold);  // xold has the offset
   
   return xx;
}

/******************************************************************************
* double yscale
*/
double yscale(double y)
{
   // HPscale takes care of this
   //if (CurrIpRotation & 1) y = -y;
   double yy;
   if (plotabs)
      yy = ((y * abs_plot_f_y + offset_y) * HPscale);
   else
      yy = ((y * abs_plot_f_y) * HPscale + yold); // yold has the offset
   
   return yy;
}

/******************************************************************************
* hp_di
* this is the cos()
*/
static int hp_di()
{
   double run, rise;

   textrot = 0;
   if (read_real(&run))
   {
      if (read_real(&rise))
         textrot = ArcTan2(rise,run);
   }
   return TRUE;
}

/******************************************************************************
* hp_sl
*/
static int hp_sl()
{
   double t, at;

   textoblique = 0;
   if (read_real(&t))                        
   {
       at = atan(t);
       textoblique = round(RadToDeg(at));
   }
   return TRUE;
}

/******************************************************************************
* hp_dr
* this is the cos()
*/
static int hp_dr()
{
   double run, rise;

   textrot = 0;
   if (read_real(&run))
   {
      if (read_real(&rise))
         textrot = ArcTan2(rise,run);
   }
   return TRUE;
}

/******************************************************************************
* hp_pw
* Pen width - overrites user defintions
*/
static int hp_pw()
{
   double w;
   int err;

   if (read_real(&w))
   {
     // CurrWu 0 = metric (like text size)
     //        1 = scaled
     switch (CurrWuMode)
     {
       case 1:  // relative
         w = _scale(w);
       break;
       default:
         w =  w * CMscale / 10;  // Cm is in centimeter, need to be in mm
       break;
     }

     // this must be an unnamed pen, so that the width gets done.

      if (w <= SMALLWIDTH_SIZE)
         CurrPenIndex =  0;
      else
         CurrPenIndex = Graph_Aperture("", T_ROUND, w, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE, &err);
   }

   return TRUE;
}

/******************************************************************************
* close_polygon
*/
static int close_polygon()
{
   /* close does not work with relative koordinates. */
   // this is done with HP_SUBCLOSE command
   return 1;
}

/******************************************************************************
* write_polygon
*/
static int write_polygon(int fill, int fpcommand)
{
   int sub = 0;
   int pa = plotabs;
   int start_index = 0;
   int plotpm = FALSE;  // if PM PA xy xy PM2 FP than plot buffer even that
                              // there in no PD
   int closed = fill;
   int vertexmode = 0;
   int pm1 = FALSE;
   double lastx = xold;
   double lasty = yold;
   CPnt *lastvertex = NULL;

   for (int p=0; p<polygoncnt ;p++)                 
   {
      plotabs = polygon[p].plotabs;
      if (!polygon[p].plot)   // if not plot, it can not be in the vertex mode.
         vertexmode = 0;

      switch (polygon[p].typ)
      {
         case HP_PM0:   // start of polygon mode
            start_index = p;

            lastx = polygon[p].x;   // unscaled last koos before entering into polymode
            lasty = polygon[p].y;

            if (polygon[p].plot)
            {
               Graph_PolyStruct(CurrLayer, 0, FALSE);
               Graph_Poly(NULL, CurrPenIndex, fill, FALSE, closed);
               lastvertex = Graph_Vertex(lastx, lasty, 0.0);
               vertexmode = TRUE;
               plotpm = TRUE;
            }
            xold = polygon[p].x;  // old is needed to releative / abs function
            yold = polygon[p].y;
            pm1 = FALSE;
         break;
         case HP_LINE:  
         {
            if (polygon[p].plot && !vertexmode && !pm1)
            {
               Graph_PolyStruct(CurrLayer, 0, FALSE);
               Graph_Poly(NULL, CurrPenIndex, fill, FALSE, closed);
               lastvertex = Graph_Vertex(lastx, lasty, 0.0);
               vertexmode = TRUE;
            }
                                            
            lastx = xscale(polygon[p].x);
            lasty = yscale(polygon[p].y);

            if (fabs(lastx - xold) > SMALLNUMBER || fabs(lasty - yold) > SMALLNUMBER)
            {
               if ((polygon[p].plot || plotpm) && !pm1)
                  lastvertex = Graph_Vertex(lastx, lasty, 0.0);
            }     
            xold = lastx;  // old is needed to releative / abs function
            yold = lasty;
            pm1 = FALSE;
         }
         break;
         case HP_PM1:
            // Close a polygon to the start PM command
            // here needs to be penup
            // pm1 can be without a pm0.
            vertexmode = FALSE;
            plotpm = FALSE;
            pm1 = TRUE;
         break;
         case HP_CI:
            // circle take the coo of PM0
            if (circle_is_aperture(polygon[p].radius))
            {
               double size_a = polygon[p].radius * 2;
               CString n;
               int err;

               n.Format("ROUND_%2.3lf", size_a);
               Graph_Aperture(n, T_ROUND , size_a, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE | BL_GLOBAL, TRUE, &err);  
               Graph_Block_On(GBO_APPEND, n, -1, 0);  // acad allows the reference of a block before defintion
               Graph_Block_Off();
               Graph_Block_Reference(n, NULL, 0, lastx, lasty, 0.0, 0, 1.0, CurrLayer, TRUE);
            }
            else
               Graph_Circle(CurrLayer, lastx, lasty, polygon[p].radius, 0, CurrPenIndex, FALSE, FALSE);
            vertexmode = 0;
            pm1 = FALSE;
         break;
         case HP_ARC:
         {
            double cx = xscale(polygon[p].x);
            double cy = yscale(polygon[p].y);
            double sa = ArcTan2(yold - cy, xold - cx);    // start angle
            double radius = sqrt( (cx - lastx) * (cx - lastx) + (cy - lasty) * (cy - lasty) );
            if (vertexmode)
            {
               // in vertex mode
               double da = polygon[p+1].y;
               lastvertex->bulge = (DbUnit)tan(polygon[p+1].y / 4);
               lastx = cx + radius * cos(sa + polygon[p+1].y);
               lasty = cy + radius * sin(sa + polygon[p+1].y);
               lastvertex = Graph_Vertex(lastx, lasty, 0.0);
            }
            else
            {
               Graph_Arc(CurrLayer, cx, cy, radius, sa, polygon[p+1].y, 0, CurrPenIndex, FALSE);
               lastx = cx + radius * cos(sa + polygon[p+1].y);
               lasty = cy + radius * sin(sa + polygon[p+1].y);
            }
            p= p + 1;
            pm1 = FALSE;
         }
         break;
         case HP_CI_FILLED:
            // circle take the coo of PM0
            Graph_Circle(CurrLayer, lastx, lasty, polygon[p].radius, 0, CurrPenIndex, FALSE, TRUE);
            vertexmode = 0;
            pm1 = FALSE;
         break;
         case HP_PM2:
            pm1 = FALSE;
         break;
         case HP_RR:
         {
            fprintf(flog, "RR Polygon command\n");
            p++;  // has 2 commands
            pm1 = FALSE;
         }
         break;
         case HP_ER:
         {
            fprintf(flog, "RR Polygon command\n");
            p++;  // has 2 commands
            pm1 = FALSE;
         }
         break;
         default:  
            fprintf(flog, "Unknown Polygon command\n");
            pm1 = FALSE;
         break;
      }  // switch
   }
   plotabs = pa;
   xold = lastx;
   yold = lasty;
   return OK;
}
// end G_HPGLIN.C

