// $Header: /CAMCAD/4.5/read_wrt/ExceIn.cpp 20    9/30/05 3:44p Rick Faltersack $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/ 
          
// Excellon Reader

#include <stdafx.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "format_s.h"
#include "geomlib.h"
#include "file.h"
#include "graph.h"
#include "txtscan.h"
#include "pcbutil.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

/* Defines *****************************************************************/

// this is until we get the tool display worked out (Jul-97)
#undef   NO_TOOL_DISPLAY

#define  TMPFILENAME             "$.$"
#define  EXCELLONERR             "excellon.log"

#define  MAX_LINE                512      /* Max line length. This includes macros */
#define  MAX_WIDTH               100
#define  MAX_POLY                500

#define  PENUP                   2
#define  PENDOWN                 1
#define  PENDRILL                3
#define  PATTERNREPEAT           4

#define  G_UNKNOWN               0
#define  G_INCHES                1

/* Structures **************************************************************/

typedef  struct
{
   double   cur_x,cur_y;    // this are the koos after increment etc...
   double   old_x,old_y;    // this are the koos after increment etc...
   double   coo_x,coo_y;    // this are the koos after axis mode
   double   read_x, read_y; // this are the original read koos.
   double   arc_radius;
   double   zero_x, zero_y;
   double   m25_x, m25_y;     // start of pattern
   double   m25_original_x, m25_original_y;     // start of pattern
   double   prevX, prevY;
   int      cur_gcode;
   int      cur_tool;
   int      vor_komma;        // integer
   int      nach_komma;
   int      absolute;
   int      drawmode;         /* 1 = penup */
                              /* 2 = pendown */
                              /* 3 = drill */
                              /* 4 = pattern repeat */
   int      arc_mode;         // 1 is arcmode 2 is circlemode
   int      layernum;
   int      done;
   int      percentcnt;
   int      programheader;
   int      zeroset;          // g93 command
   int      repeat_pattern;   // R command = 0 is none, otherwise number
   int      blockmode;
   int      zeroleading;
   int      fmat;
   long     sequencecnt;
   long     linecnt;
   int      vermode;          // Axis Orientation
} G_local;

typedef struct
{
   int   g81;
   int   m24;
   int   m26;
   int   m27;
   int   m70;
   int   g92;
}Format1;

/* Globals *****************************************************************/

static   G_local     GX;
static   CString     cur_tool_name;
static   CString     prefix;

static   FILE        *ferr;
static   FILE        *fp;
static   int         display_error = 0;

static   double      cnv_tok(const char *);
static   int         preprocess_excellon(FILE *fp, const char *fname);

static   int         load_excellonsettings(const CString fname);

static   void        adjust_axis();
static   int         cont_flag = FALSE;
static   void        do_excellon(double format_scale, int format_units);
static   void        write_db();
static   double      GBscale;
static   double      UnitScale;
static   Point2      *ppoly;
static   int         ppolycnt = 0;
static   CString     last_draw_ap_name;
static   CString     patternname;
static   int         patterncnt = 0;
static   int         cur_filenum = 0;
static   int         pageUnits; 
static   Format1     format1;
static   double      formatscale;

static   BlockStruct *curblock = NULL; // just for debug

static   int         M70 = 0;          // M70 command, swap x and y koos
static   int         M80_mirrorx = 1;  // M80 command
static   int         M90_mirrory = 1;  // M90 command

static   int         overwrite_format_inch;  // see nach_komma format
BOOL     gbM2Flag    = TRUE;
BOOL     gbFlag      = TRUE;

/******************************************************************************
* ReadExcellon
   Absolute = Format.GR.type;         0-Absolute; 1-Incremental
   zeroleading = Format.GR.zero;      0-Leading suppressed; 1-Trailing suppressed; 2-None
   units = Format.GR.units;           0-English; 1-Metric
*/
void ReadExcellon(const char *fname, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{
   FileStruct *file;
   double      unitscale;

   CString excellonLogFile = GetLogfilePath(EXCELLONERR);
   if ((ferr = fopen(excellonLogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", excellonLogFile);
      ErrorMessage(t, "Error");
      return;
   }
   display_error = 0;

   memset(&GX,0,sizeof(GX));  // set to zero

   pageUnits = PageUnits;

   CString settingsFile( getApp().getImportSettingsFilePath("excellon.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nExcellon Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_excellonsettings(settingsFile);

   prefix = Format->prefix;
   formatscale = Format->Scale;

   // metrix - mil is done in scaleUnitsFactor.
   if (Format->EX.units == 0)
   {
      unitscale = Units_Factor(UNIT_INCHES, pageUnits);  // this is inches because 2.3 means inches
      //m_units = UNIT_INCHES;
      GBscale = formatscale * Units_Factor(UNIT_INCHES, pageUnits);  //
		UnitScale = GBscale;
      // inch are in 6 digits 00.0000
      if (overwrite_format_inch > 0)   // overwrite from .in file.
         GX.vor_komma = overwrite_format_inch;
      else
		  GX.vor_komma = 2;
      GX.nach_komma = 4;
   }
   else
   {
      unitscale = Units_Factor(UNIT_MM, pageUnits);
      GBscale = formatscale * Units_Factor(UNIT_MM, pageUnits);   //
		UnitScale = GBscale;
      GX.zeroleading = TRUE;
      GX.vor_komma = 3;
      GX.nach_komma = 3;
      //m_units = UNIT_MM;
   }

   GX.zeroleading = Format->EX.zeroSuppression;
   GX.absolute = !Format->EX.type;
   GX.vermode = 1;  // default axis orientation

   if (GX.zeroleading == 0)  // if leading zero suppressed to nachkomma.
   {
      for (int ii=0;ii<GX.nach_komma;ii++)
         GBscale /=10;
   }

   GX.fmat = 2;       // default is 2 and than I test if I find format 1 commands

   FILE *ifp;
   if ((ifp = fopen(TMPFILENAME,"wt")) == NULL)
   {
      ErrorMessage("Error open file","Error");
      return;
   }

   format1.g81 = 0;
   format1.g92 = 0;
   format1.m24 = 0;
   format1.m26 = 0;
   format1.m27= 0;
   format1.m70= 0;

   if (!preprocess_excellon(ifp,fname))
   {
      ErrorMessage("Error preprocessing file","Error");
      return;
   }

   fclose(ifp);

   if ((fp = fopen(TMPFILENAME,"rt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s]",TMPFILENAME);
      ErrorMessage(t,"Error");
      return;
   }

   if ((ppoly = (Point2 *) calloc(MAX_POLY,sizeof(Point2))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   ppolycnt = 0;

   file = Graph_File_Start(fname, fileTypeExcellon);
   cur_filenum = file->getFileNumber();

   Graph_Level("0", "", 1); // floating layer

   char drive[_MAX_DRIVE]; 
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], filname[_MAX_FNAME];
   char ext[_MAX_EXT];

   // need to do this, because panel only knows the path.ext
   _splitpath( fname, drive, dir, filname, ext );
   strcpy(f,filname);
   strcat(f,ext);

   GX.layernum = Graph_Level(f, "", 0); // makes every excellon file a different color

   // assign as a drill file.
   LayerStruct *l;
   if (l = Doc->FindLayer(GX.layernum))
   {
      l->setComment("Drill");
      l->setLayerType(LAYTYPE_DRILL);
   }

   do_excellon(Format->Scale, Format->EX.units);

   fclose(fp);

   free(ppoly);

   UNLINK(TMPFILENAME);

   fclose(ferr);
   if (display_error)
      Logreader(excellonLogFile);

}

/*****************************************************************************/
/*
*/
static int check_format1(const char *f)
{
   if (STRNICMP(f,"G81", 3) == 0)   
   {
      if (!format1.g81)
         fprintf(ferr,"G81 code detected -> switched to Format 1\n");
      format1.g81 = TRUE;
      GX.fmat = 1;
   }
/*
   if (STRNICMP(f,"G92", 3) == 0)   
   {
      if (!format1.g92)
         fprintf(ferr,"G92 code detected -> switched to Format 1\n");
      format1.g92 = TRUE;
      GX.fmat = 1;
   }
*/
   if (STRNICMP(f,"M24", 3) == 0)   
   {
      if (!format1.m24)
         fprintf(ferr,"M24 code detected -> switched to Format 1\n");
      format1.m24 = TRUE;
      GX.fmat = 1;
   }
   if (STRNICMP(f,"M26", 3) == 0)   
   {
      if (!format1.m26)
         fprintf(ferr,"M26 code detected -> switched to Format 1\n");
      format1.m26 = TRUE;
      GX.fmat = 1;
   }
   if (STRNICMP(f,"M27", 3) == 0)   
   {
      if (!format1.m27)
         fprintf(ferr,"M27 code detected -> switched to Format 1\n");
      format1.m27 = TRUE;
      GX.fmat = 1;
   }
   if (STRNICMP(f,"M70", 3) == 0)   
   {
      if (!format1.m70)
         fprintf(ferr,"M70 code detected -> switched to Format 1\n");
      format1.m70 = TRUE;
      GX.fmat = 1;
   }

   return 1;
}

/*****************************************************************************/
/*
   This is used if EXCELLON file has external include files.
   return 0 : error happened 
   return 1 : all is OK.
*/
static int preprocess_excellon(FILE *ofp,const char *fname)
{                                                                     
   FILE  *ifp;
   char  line[MAX_LINE];
   char  tmp[MAX_LINE];
   char  *lp;

   if ((ifp = fopen(fname,"rt")) == NULL)
   {
      ErrorMessage("Error open file","Error");
      return 0;
   }

   while (fgets(line,MAX_LINE,ifp) != NULL)
   {
      strcpy(tmp,line);

      check_format1(tmp);

      if ((lp = strtok(tmp,",")) != NULL)
      {
         if (!STRCMPI(lp,"M99"))
         {
            // here M99 found
            if ((lp = strtok(NULL,"\n")) != NULL)
            {
               // name is DM:xxx.t02
               strcpy(tmp,lp);
               STRREV(tmp);
               if ((lp = strtok(tmp,":")) == NULL) continue;
               sprintf(tmp,"%s",lp);
               STRREV(tmp);
               preprocess_excellon(ofp,tmp);
               continue;               
            }

         }
      }

      if (fputs(line,ofp) == EOF)
         return 0;
   }

   fclose(ifp);

   return 1;
}

/*****************************************************************************/
/*
*/
static int do_header(char *t, int *m_units)
{
   char  tmp[255+1];
   char  *lp;

   if (strlen(t) == 0)   return 0;
   if (strlen(t) > 255) return 0;

   strcpy(tmp,t);

   if (!STRNICMP(tmp,"VER",3))
   {
      if ((lp = strtok(tmp,",")) == NULL)    return 0;
      if ((lp = strtok(NULL,"\n")) == NULL)  return 0; 
      GX.vermode = atoi(lp);
   }
   else
   if (!STRNICMP(tmp,"FMAT",4))
   {
      if ((lp = strtok(tmp,",")) == NULL)    return 0;
      if ((lp = strtok(NULL,"\n")) == NULL)  return 0;
      GX.fmat = atoi(lp);
   }
   else
   if (!STRNICMP(tmp,"AFS",4))
   {
      return 1;   // AFS no used
   }
   else
   if (!STRNICMP(tmp,"INCH",4))
   {
      // LZ is Leading Zero usage, TZ is trailing zero usage
      if (strstr(tmp,"TZ"))   // if trailing zero suppressed to nachkomma.
         GX.zeroleading = FALSE;
      else
      if (strstr(tmp,"LZ"))   // if leading zero suppressed to nachkomma.
         GX.zeroleading = TRUE;

      m_units = UNIT_INCHES;
      GBscale = formatscale * Units_Factor(UNIT_INCHES, pageUnits);  //
		UnitScale = GBscale;
      // inch are in 6 digits 00.0000
      GX.vor_komma = 2;
      GX.nach_komma = 4;

      // this is very specicif for one customer !
      if (overwrite_format_inch > 0)   // overwrite from .in file.
         GX.nach_komma = overwrite_format_inch;

      if (GX.zeroleading == 0)  // if leading zero suppressed to nachkomma.
      {
         for (int ii=0;ii<GX.nach_komma;ii++)
            GBscale /=10;
      }
   }
   else
   if (!STRNICMP(tmp,"METRIC",4))
   {
      // default 
      GX.zeroleading = TRUE;
      GX.vor_komma = 3;
      GX.nach_komma = 3;
      
      // LZ is Leading Zero usage, TZ is trailing zero usage
      if (strstr(tmp,"TZ"))   // if trailing zero suppressed to nachkomma.
         GX.zeroleading = FALSE;
      else
      if (strstr(tmp,"LZ"))   // if leading zero suppressed to nachkomma.
         GX.zeroleading = TRUE;

      // METRIC,LZ,000.000
      // can be either 000.00 five digit 10 micron
      //               0000.00 10 micron
      //               000.000 micron

      char *lp = strrchr(tmp, ',');
      if (lp)
      {
         if (*lp && *lp+1)
         {
            lp++;
            if (!STRCMPI(lp,"000.00"))
            {
               GX.vor_komma = 3;
               GX.nach_komma = 2;
            }
            else
            if (!STRCMPI(lp,"0000.00"))
            {
               GX.vor_komma = 4;
               GX.nach_komma = 2;
            }
            else
            if (!STRCMPI(lp,"000.000"))
            {
               GX.vor_komma = 3;
               GX.nach_komma = 3;
            }
         }  // if lp
      }


      // here this is metric mode
      *m_units = UNIT_MM;
      GBscale = formatscale * Units_Factor(UNIT_MM, pageUnits);   
		UnitScale = GBscale;

      if (GX.zeroleading == 0)  // if leading zero suppressed to nachkomma.
      {
         for (int ii=0;ii<GX.nach_komma;ii++)
            GBscale /=10;
      }
   }
   else
   if (!STRNICMP(tmp,"EXDA",4))
   {
      // here is soft extended axis on off
   }
   else
   if (!STRNICMP(tmp,"ZC",2))
   {
      // here this is zero coordinate
   }
   else
   if (!STRCMPI(tmp,"R,CP"))
   {
      // here this reset program clock
   }
   else
   if (!STRCMPI(tmp,"R,H"))
   {
      // here this is reset hit count
      int err = 0;
   }
   else
   if (tmp[0] == 'T' && isdigit(tmp[1]))
   {
      int   err;
      lp = tmp;
      // tool command 
      lp++;
      GX.cur_tool= err = atoi(lp);
      // watch out for T01 and T1 must be the same
      cur_tool_name.Format("T%02d",GX.cur_tool);
      
      while (TRUE)
      {
         // T1F00S00C0.025
         // T4C.1234
         while (*lp && isdigit(*lp))   lp++; // skip to C or H 
         if (*lp == NULL)
         {
            break;
         }
         else
         if (*lp == 'C')
         {
            double   diam;
            // here is tool diameter
            lp++;
            if (lp == NULL)   break;
            
            if (strchr(lp,'.'))
            {
               diam = atof(lp) * Units_Factor(*m_units, pageUnits); // in inch or micron
            }
            else
            {
               diam = cnv_tok(lp);
            }

            Graph_Tool(cur_tool_name, GX.cur_tool, diam, 0, 0, 0, 0L);

            // until the new tool graphic and assigment works.
#ifdef NO_TOOL_DISPLAY
            Graph_Aperture(cur_tool_name, T_ROUND, diam, 0.0,0.0, 0.0, 0.0, GX.cur_tool,
               BL_APERTURE, FALSE, &err); 
#endif
         }
         else
         {
            lp++; // advance 1 step
         }
      } // while parse lp
   }
   else
   if (tmp[0] == '%')
   {
      GX.programheader = FALSE;
   }
   else
   {
      // i do not know
   }

   return 1;
}

/*****************************************************************************/
/* 
   This converts Format 1 to Format 2
*/
static int cnv_format(char mode, int code)
{
   if (GX.fmat == 1)
   {
      if (mode == 'M')
      {
         switch (code)
         {
            case 2:  return 0;
            case 24: return 1;
            case 26: return 2;
            case 1:  return 6;
            case 27: return 8;
            case 0:  return 9;
            case 23: return 70;
            case 70: return 72;
            case 21: return 80;
            case 22: return 90;
         }
      }
      else
      if (mode == 'G')
      {
         switch (code)
         {
            case 81: return 5;
            case 92: return 93;
         }
      }
   }

   return code;
}


/*****************************************************************************/
/* Do all scan
*/
#define  PROGRAM_DELIMETER  ",%XYMGTRAFSBCZH\n"

void do_excellon(double fscale, int funits)
{
   char  token[MAX_LINE];
   char  lastch;
   int   res;
   int   m_units;       // these are used for tool sizes.
   int   message_displayed = FALSE;
   int   m30 = FALSE;   // m30 is end of program

   if (funits == 0)
      m_units = UNIT_INCHES;  // 
   else
      m_units = UNIT_MM;      // 

   Set_Comment( "/" , "\n" );
   Set_Text_Delimeter( "\001" );
   Set_Word_Delimeters( PROGRAM_DELIMETER );
   Set_White_Characters( "" );   // white char will terminate the token.
   Set_Non_Characters( "\t " );
   Set_Linecnt(0L);
   rewind(fp);

   lastch = ' ';
   GX.drawmode = PENDRILL;    // some files just have xy in there. start drill asap.
   GX.repeat_pattern = 0;
   GX.m25_x = GX.m25_y = 0;
   GX.zero_x = GX.zero_y = 0;
   GX.done = TRUE;
   GX.sequencecnt = 0;
   GX.linecnt = 0;

#ifdef _DEBUG
   patterncnt = 0; // this should not be reset, because multiple files can be done.
#endif
 
   GX.blockmode = FALSE;

   M80_mirrorx = 1;  // M80 command
   M90_mirrory = 1;  // M90 command

   // init tool - some just start drilling
   GX.cur_tool = 1;
   cur_tool_name.Format("T%02d",GX.cur_tool);

   while ((res = Read_Token(fp,token, MAX_LINE)) != EOF_CHARACTER && !m30)
   {
      if (res == STOP_TEXT) // this is happening when a file start %AMxxx
      {
         lastch = Delimeter();
      }

      // this are remarks /%...... to end of line
      if (strlen(token) && token[0] == '/' && lastch != '\n')
      {
         char c;
         while ((res = Read_Char(fp,&c)) != EOF_CHARACTER)
         {
            if (c == '\n')
                break;
         }
         continue;
      }

      switch(lastch)
      {
         case  '%':
            GX.programheader = FALSE;
            Set_Word_Delimeters( PROGRAM_DELIMETER );
         break;
         case  'X':
            GX.done = FALSE;
            gbM2Flag = TRUE;

            // here check if leading zeros are propable
            if (token[0] == '0' && token[strlen(token)-1] != '0' && !GX.zeroleading)
            {
               if (!message_displayed)
               {
                  CString tmp;
                  tmp.Format("Excellon Format [X%s] detected but \"Suppress Leading Zeros\" selected!\n",token);
                  MessageBox(NULL, tmp,"Excellon Settings", MB_OK | MB_ICONHAND);
                  message_displayed = TRUE;
               }
            }

            // pattern repeat is always incremental
            if (strlen(token))
            {
               GX.read_x = cnv_tok(token) * M80_mirrorx;
            }
            else
            {
               if (GX.drawmode == PATTERNREPEAT)
               {
                  GX.cur_x = GX.m25_original_x;
                  GX.m25_x = GX.m25_original_x;
               }
               GX.read_x = 0;
            }
         break;
         case  'Y':
            GX.done = FALSE;
            gbM2Flag = TRUE;

            if (strlen(token))
            {
               GX.read_y = cnv_tok(token) * M90_mirrory;
            }
            else
            {
               if (GX.drawmode == PATTERNREPEAT)
               {
                  GX.cur_y = GX.m25_original_y;
                  GX.m25_y = GX.m25_original_y;
               }
               GX.read_y = 0;
            }
         break;
         case 'A':
            if (strlen(token))
               GX.arc_radius = cnv_tok(token);
            //GX.arc_mode = TRUE;
         break;
         case 'T':
         {
            // tool select T0101 is tool 1 with compensation 1
            if (strlen(token) > 2)   // the last two are for index and compentation
               token[strlen(token)-2] = '\0';
            GX.cur_tool = atoi(token);

            // watch out for T01 and T1 must be the same
            cur_tool_name.Format("T%02d",GX.cur_tool);

            GX.drawmode = PENDRILL;
            GX.done = TRUE;   // when there is a pen select, wait for next xy
         }
         break;
         case 'R':   // repeat a pattern
         {
            GX.repeat_pattern = atoi(token);
            GX.read_x = GX.read_y = 0;
            gbM2Flag = TRUE;

         }
         break;
         case 'G':
         {
            // T0101 is tool 1 with compensation 1
            // T101  is tool 1 with compensation 1
            GX.cur_gcode = atoi(token);

            switch (cnv_format('G',GX.cur_gcode))
            {
               case 0:
                  // this is route mode on - drill off
                  // M 15 is pen down
                  GX.drawmode = PENUP;
                  GX.arc_mode = 0;
               break;
               case 1:  // linear move
                  GX.arc_mode = 0;
               break;
               case 2:  // circular clock
                  GX.arc_mode = 1;
               break;
               case 3:  // circular couter clock
                  GX.arc_mode = 1;
               break;
               case 5:
                  // this is drill on - route off
                  GX.drawmode = PENDRILL;
               break;
               case 41:
                  fprintf(ferr,"G41 : Cutter Compensation Left not implemented\n");
                  display_error++;
               case 42:
                  fprintf(ferr,"G42 : Cutter Compensation Right not implemented\n");
                  display_error++;
               break;
               case 90:
                  GX.absolute = TRUE;
               break;
               case 91:
                  GX.absolute = FALSE;
               break;
               case 92:
                  GX.drawmode = PENUP; // the next xy are zero point, but not drills
               break;
               case 93:
                  GX.zeroset = TRUE;
               break;
               case 32:
               case 33:
                  GX.arc_mode = 2;
                  GX.drawmode = PENDOWN;
               break;
            }
         }
         break;
         case 'M':
         {
            int   m = atoi(token);
            switch (cnv_format('M',m))
            {
               case 47: // operator message
               {
                  char c = 0;
                  while ((res = Read_Char(fp,&c)) != EOF_CHARACTER)
                  {
                     if (c == '\n')
                        break;
                  }
                  continue;
               }                 
               break;
               case 48:
                  GX.programheader = TRUE;
                  Set_Word_Delimeters( "\n" );
               break;
               case 25: // begin of pattern
                  if (GX.blockmode) // tool change ends an M25 command.
                  {
                     Graph_Block_Off();
                     GX.blockmode = FALSE;
                     Graph_Block_Reference(patternname, NULL, cur_filenum, 0.0, 0.0,
                        DegToRad(0.0), 0, 1.0, -1, FALSE);
                  }
                  patternname.Format("PATTERN_%d",++patterncnt);
                  curblock = Graph_Block_On(GBO_APPEND,patternname,cur_filenum, 0);
                  GX.blockmode = TRUE;
                  GX.m25_x = GX.old_x;
                  GX.m25_y = GX.old_y;
                  if (GX.absolute)
                  {
                     //GX.m25_x = 0;
                     //GX.m25_y = 0;
                     GX.m25_original_x = 0;
                     GX.m25_original_y = 0;
                  }
                  else
                  {
                     GX.m25_original_x = GX.old_x;
                     GX.m25_original_y = GX.old_y;
                  }
               break;
               case 1:  // Format1 M24 end of pattern

                  // end of pattern means:
                  // close the existing one
                  // make a new - place the existing into the new one
                  // continue with new.
                  gbM2Flag = FALSE;

                  if (GX.blockmode) // only allow end of patterns if blockmode is on.
                  {
                     Graph_Block_Off();
                     GX.blockmode = FALSE;
                     patternname.Format("PATTERN_%d",patterncnt); // place the last one and not the current one.

                     // here close the M25 to M1
                     // now make a new pattern, because this can have multiple M1 following one M25
                     CString  pname;
                     pname.Format("PATTERN_%d",++patterncnt);
                     curblock = Graph_Block_On(GBO_APPEND,pname,cur_filenum, 0);
                     GX.blockmode = TRUE;
                     GX.m25_x = GX.m25_original_x;
                     GX.m25_y = GX.m25_original_y;
                     GX.drawmode = PATTERNREPEAT;
                     GX.read_x = GX.read_y = 0;
                     GX.done  = FALSE;
                     // the pattern is placed with a M02XY <=== without any coos.
                     //Graph_Block_Reference(patternname, NULL, cur_filenum, GX.m25_x, GX.m25_y, DegToRad(0.0), 0, 1.0, -1, FALSE);
                  }
               break;
               case 2:  // Format1 M26 repeat with pattern offset

                  patternname.Format("PATTERN_%d",patterncnt-1);  // place the last one and not the current one.

/*
                  if (GX.blockmode) // only allow end of patterns if blockmode is on.
                  {
                     // here no end of pattern detected. Finish it and place it.
                     Graph_Block_Off();
                     //Graph_Block_Reference(patternname, NULL, cur_filenum, GX.m25_x, GX.m25_y, DegToRad(0.0), 0, 1.0, -1, FALSE);
                  }
                  GX.blockmode = FALSE;
*/
                  // repeat from last M25 on 
                  GX.drawmode = PATTERNREPEAT;
                  //GX.repeat_pattern = 1; // R2M02X0575
                  GX.read_x = GX.read_y = 0;
                  GX.done  = FALSE;
                  gbM2Flag = FALSE;
               break;
               case 30:
                  // end of program
                  if (GX.blockmode)
                  {
                     Graph_Block_Off();
                     GX.blockmode = FALSE;
                     patternname.Format("PATTERN_%d",patterncnt); // place the current one.
                     GX.m25_x = GX.m25_original_x;
                     GX.m25_y = GX.m25_original_y;
                     Graph_Block_Reference(patternname, NULL, cur_filenum, GX.m25_x, GX.m25_y, DegToRad(0.0), 0, 1.0, -1, FALSE);
                  }
                  m30 = TRUE;
               break;
               case 15:
                  // tool down
                  GX.drawmode = PENDOWN;
               break;
               case 16:
               case 17:
                  GX.drawmode = PENUP;
                  GX.done     = FALSE; // need this for writedb command
               break;
               case 8: // Format1 M27
                  // here do a placement of the last pattern
                  gbM2Flag = FALSE;
                  if (GX.blockmode)
                  {
                     Graph_Block_Off();
                     GX.blockmode = FALSE;
                  
                     patternname.Format("PATTERN_%d",patterncnt); // place the current one.
                     GX.m25_x = GX.m25_original_x;
                     GX.m25_y = GX.m25_original_y;
                     Graph_Block_Reference(patternname, NULL, cur_filenum, GX.m25_x, GX.m25_y, DegToRad(0.0), 0, 1.0, -1, FALSE);
                  }
                  GX.old_x = GX.m25_original_x;
                  GX.old_y = GX.m25_original_y;

/*
                  Graph_Block_Reference(patternname, NULL, cur_filenum, 0.0, 0.0,
                           DegToRad(0.0), 0, 1.0, -1, FALSE);
*/
               break;
               case 71:
                  m_units = UNIT_MM;
                  GBscale = formatscale * Units_Factor(m_units, pageUnits);   // this is inches because 2.3 means inches
						UnitScale = GBscale;
                  if (GX.zeroleading == 0)  // if leading zero suppressed to nachkomma.
                  {
                     for (int ii=0;ii<GX.nach_komma;ii++)
                        GBscale /=10;
                  }
               break;
               case 72:
                  m_units = UNIT_INCHES;
                  GBscale = formatscale * Units_Factor(m_units, pageUnits);   // this is inches because 2.3 means inches
						UnitScale = GBscale;
                  if (GX.zeroleading == 0)  // if leading zero suppressed to nachkomma.
                  {
                     for (int ii=0;ii<GX.nach_komma;ii++)
                        GBscale /=10;
                  }
               break;
               case 70:
               {
                  M70 = TRUE;
                  gbM2Flag = TRUE;
               }
               break;
               case 80:
                  {
                     M80_mirrorx *= -1;
                     gbM2Flag = TRUE;
                  }
               break;
               case 90:
                  {
                     M90_mirrory *= -1;
                     gbM2Flag = TRUE;
                  }
               break;
               case 97:
                  // text 0 degree
                  Set_Word_Delimeters( "\n" );
                  res = Read_Token(fp,token, MAX_LINE);
                  Set_Word_Delimeters( PROGRAM_DELIMETER );
                  GX.done = TRUE;
               break;
               case 98:
                  // text 90 degree rotated
                  Set_Word_Delimeters( "\n" );
                  res = Read_Token(fp,token, MAX_LINE);
                  Set_Word_Delimeters( PROGRAM_DELIMETER );
                  GX.done = TRUE;
               break;
            }
         }
         break;
         case 'F':
            // to data page ???
         break;
         case 'C':  // this is tool size.
         {
            double diameter;

            if (token[0] == '.')
            {
               diameter = atof(token) * Units_Factor(m_units, pageUnits); // in inch or micron
            }
            else
            {
               diameter = cnv_tok(token);
            }


            Graph_Tool(cur_tool_name, GX.cur_tool, diameter, 0, 0, 0, 0L);
#ifdef NO_TOOL_DISPLAY
            int err;
            Graph_Aperture(cur_tool_name, T_ROUND, diameter, 0.0,0.0, 0.0, 0.0, 
               GX.cur_tool,
               BL_APERTURE , TRUE, &err);
#endif
         }
         break;
         case '\n':
            // this are ignore codes
            GX.linecnt++;
            if (GX.programheader)
            {
               do_header(token, &m_units);
               // this is updated in do_header
               if (!GX.programheader)
                  Set_Word_Delimeters( PROGRAM_DELIMETER );
               break;
            }
            if (GX.done)   break;

            // here update axis orientation.
            adjust_axis();

            if (GX.absolute && GX.drawmode != PATTERNREPEAT)   // pattern repeat is always incremental
            {
               GX.cur_x = GX.coo_x;
               GX.cur_y = GX.coo_y;
            }
            else
            {
               GX.cur_x += GX.coo_x;
               GX.cur_y += GX.coo_y;
            }

            if (GX.zeroset)
            {
               GX.zero_x = GX.cur_x;
               GX.zero_y = GX.cur_y;
               GX.zeroset = FALSE;
               GX.done = TRUE;
            }
            else
            {
               //GX.cur_x += GX.zero_x;
               //GX.cur_y += GX.zero_y;

               write_db();
               GX.old_x = GX.cur_x;
               GX.old_y = GX.cur_y;
               GX.sequencecnt++;
            }
            GX.repeat_pattern = 0;
            M80_mirrorx = 1;  // M80 command, used only in pattern
            M90_mirrory = 1;  // M90 command
            M70 = FALSE;
         break;
         default:
            // there can be other unknown commands.
         break;
      }
/*
      if (GX.programheader)
         Set_Word_Delimeters( "\n" );
      else
         Set_Word_Delimeters( PROGRAM_DELIMETER );
*/
      lastch = Delimeter();
      if(!gbM2Flag&&lastch=='\n')
         gbFlag = TRUE;
   }

   // write out the last one, if one exists
   GX.drawmode = PENUP;
   GX.done = FALSE;
   write_db();

   if (GX.blockmode)  // just make sure that a started block is finished.
   {
      Graph_Block_Off();
      GX.blockmode = FALSE;
      patternname.Format("PATTERN_%d",patterncnt); // place the current one.

      Graph_Block_Reference(patternname, NULL, cur_filenum, 0.0, 0.0,
               DegToRad(0.0), 0, 1.0, -1, FALSE);
   }
   return;
}

/****************************************************************************/
/*
                    IN
                    |
                    |
         Left   ----|----   Right
                    |
                    |
                    OUT
*/
void adjust_axis()
{
    double  tmpx = GX.read_x, tmpy = GX.read_y;
    switch (GX.vermode)
    {
        case 1:
            // x pos right, y pos in
            GX.coo_x = tmpx;
            GX.coo_y = tmpy;
        break;
        case 2:
            // x pos in, y pos right
            GX.coo_x = tmpy;
            GX.coo_y = tmpx;
        break;
        case 3:
            // x pos in, y pos left
            GX.coo_x = -tmpy;
            GX.coo_y = tmpx;
        break;
        case 4:
            // x pos left, y pos in
            GX.coo_x = -tmpx;
            GX.coo_y = tmpy;
        break;
        case 5:
            // x pos out, y pos left
            GX.coo_x = -tmpy;
            GX.coo_y = -tmpx;
        break;
        case 6:
            // x pos left, y pos out
            GX.coo_x = -tmpx;
            GX.coo_y = -tmpy;
        break;
        case 7:
            // x pos right, y pos out
            GX.coo_x = tmpx;
            GX.coo_y = -tmpy;
        break;
        case 8:
            // x pos out, y pos right
            GX.coo_x = tmpy;
            GX.coo_y = -tmpx; 
        break;
        default:
          // keep it.
            GX.coo_x = tmpx;
            GX.coo_y = tmpy;
        break;
    }
    return;
}

/****************************************************************************/
/*
   here also do leading zero
*/
double   cnv_tok(const char *tt)
{
   double   x = atof(tt);
   char     tmp[80], t[80];
   int      tcnt;
   unsigned int i;
   int      minussign = FALSE;

   strcpy(t, tt); // temp copy because it could be a CString

   // here check if it is decimal mode
   if (strchr(t,'.'))
   {
		// if there is a decimal point in the coordinate, than this is an absolute value
      x = atof(t);
      return(x*UnitScale);   // this is in INCH
   }

   if (t[0] == '-')
   {
      minussign = TRUE;
      STRREV(t);
      t[strlen(t)-1] = '\0';
      STRREV(t);
   }
   else
   if (t[0] == '+')
   {
      STRREV(t);
      t[strlen(t)-1] = '\0';
      STRREV(t);
   }

   if (GX.zeroleading) // trailing suppressed
   {
      // a string can be shorter than leading.
      while ((int)strlen(t) < GX.vor_komma)
      { 
         int   tlen = strlen(t);
         t[tlen] = '0';
         t[tlen+1] = '\0';
      }

      tcnt = 0;
      for (i=0;i<(unsigned int)GX.vor_komma && i < strlen(t); i++)
         tmp[tcnt++] = t[i];
      tmp[tcnt++] = '.';
      for (;i < strlen(t); i++)
         tmp[tcnt++] = t[i];
      tmp[tcnt] = '\0';
      x = atof(tmp);
   }

   if (minussign) x = -1 * fabs(x) ;

   return(x*GBscale);   // this is in page units
}

/****************************************************************************/
/*
   PENDOWN  =  1
   PENUP    =  2
*/
void write_db()
{
   int   err;

   if (GX.done)   
      return;     /* already written */

   // also qwrite polylines if there is an arc
   if ((GX.drawmode != PENDOWN || GX.arc_mode > 1) && ppolycnt > 1)
   {
      // here need to understand how a nc route works.
      int widthIndex = Graph_Aperture(last_draw_ap_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, GX.cur_tool,
            BL_APERTURE /*| BL_DRILL*/, FALSE, &err);

      Graph_PolyStruct(GX.layernum,  0L, FALSE);
   
      int   close = (ppoly[0].x == ppoly[ppolycnt-1].x && ppoly[0].y == ppoly[ppolycnt-1].y);
      Graph_Poly(NULL, widthIndex,0,0,close);
      for (int i=0;i<ppolycnt;i++)
         Graph_Vertex(ppoly[i].x,ppoly[i].y, ppoly[i].bulge);
      ppolycnt = 0;
   }

   switch (GX.drawmode)
   {
      case PENDOWN:
      {
         if (GX.arc_mode == 1)
         {
            // Arc old_x, old_y is start point
            //     cur_x, cur_y is end point
            double rad, sa, da, cx,cy, cx1,cy1;
            int    res;
            // GX.cur_gcode 3 counter clock
            // GX.cur_gcode 2 clock wise
            // Arc is always the arc <= 180
            // if res == 2 2 arcs where possible. we need the Arc which
            // has less than 180 degrees
            res = ArcFrom2Pts(GX.old_x, GX.old_y, GX.cur_x, GX.cur_y,GX.arc_radius,
                       &cx, &cy, &cx1, &cy1);   // ArcFrom2Pts gives clock and 
                                                // counterclock center points

            if (res == 0)  // no valid arc is found. That can happen on a rounding error
            {
               double rad = sqrt((GX.cur_x - GX.old_x)*(GX.cur_x - GX.old_x) +
                                 (GX.cur_y - GX.old_y)*(GX.cur_y - GX.old_y))/2;
               GX.arc_radius = rad;

               // onemore time with updated radius
               res = ArcFrom2Pts(GX.old_x, GX.old_y, GX.cur_x, GX.cur_y,GX.arc_radius,
                                &cx, &cy, &cx1, &cy1);   // ArcFrom2Pts gives clock and 
                                                // counterclock center points
            }

            if (res == 0)
            {
               // bad arc !!!
               fprintf(ferr," Bad arc Start [%lf, %lf] End [%lf, %ld] Radius [%lf]\n",
                  GX.old_x, GX.old_y, GX.cur_x, GX.cur_y,GX.arc_radius);
               display_error++;
               da = 0;
            }
            else
            {
               ArcCenter2(GX.old_x, GX.old_y, GX.cur_x, GX.cur_y,cx,cy,&rad,&sa,&da,0);
            
               if (res == 2 && fabs(da) > PI)
               {
                  // arc found with angle greater 180 degree -> try other center
                  ArcCenter2(GX.old_x, GX.old_y, GX.cur_x, GX.cur_y,cx1,cy1,&rad,&sa,&da,0);
                  cx = cx1;
                  cy = cy1;
               }
            }
            // here adjust for clock - counter direction.
            int widthcode = Graph_Aperture(cur_tool_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, GX.cur_tool,
               BL_APERTURE /*| BL_DRILL*/, FALSE, &err);

            if (!ppolycnt)
            {
               ppoly[ppolycnt].x = GX.old_x;
               ppoly[ppolycnt].y = GX.old_y;
               ppoly[ppolycnt].bulge = 0.0;
               ppolycnt++;
            }
            if (GX.cur_gcode == 2)  // clockwise
               ppoly[ppolycnt-1].bulge = tan(-da/4);
            else
               ppoly[ppolycnt-1].bulge = tan(da/4);
         }
         else
         if (GX.arc_mode == 2)
         {
            int widthcode = Graph_Aperture(cur_tool_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, GX.cur_tool,
               BL_APERTURE /*| BL_DRILL*/, FALSE, &err);

            Graph_Circle(GX.layernum,
               GX.cur_x, GX.cur_y,
               GX.arc_radius, 0, // flg
               widthcode, FALSE, FALSE);
               ppolycnt = 0;
         }
         // here update polycnt with next entry, because a polyline can follow
         // an arc
         if ( ppolycnt < MAX_POLY)
         {
            ppoly[ppolycnt].x = GX.cur_x;
            ppoly[ppolycnt].y = GX.cur_y;
            ppoly[ppolycnt].bulge = 0.0;
            ppolycnt++;
         }
         last_draw_ap_name = cur_tool_name; // remember this aperture name when on next aperture and drawing this one
         // GX.done = TRUE;
         GX.arc_mode = FALSE;
      }
      break;
      case PENDRILL:
      {
         Graph_Tool(cur_tool_name,GX.cur_tool, 0, 0, 0, 0, 0L);
#ifdef NO_TOOL_DISPLAY
         Graph_Aperture(cur_tool_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, GX.cur_tool,
               BL_APERTURE , FALSE, &err);
#endif
         // if repeat pattern, the xy coos is the delta and not a new koordinate
         if (GX.repeat_pattern)
         {
            int i;
            double x,y, dx,dy;
            
            x = ppoly[0].x;
            y = ppoly[0].y;
            dx = GX.coo_x; // just for debug
            dy = GX.coo_y;
            for (i=0;i<=GX.repeat_pattern;i++)  // drill the starting one and than the numbers of repeats
            {
               Graph_Block_Reference(cur_tool_name, NULL, cur_filenum, x, y,
                               0.0, 0, 1.0, GX.layernum, TRUE);
               x += GX.coo_x;
               y += GX.coo_y;
            }
            GX.read_x = GX.coo_x = GX.cur_x = x;   // this GX.read is fixed, because it must work even if
            GX.read_y = GX.coo_y = GX.cur_y = y;   // after an repeat, there in only an X, but no Y.
         }
         else
         {
            Graph_Block_Reference(cur_tool_name, NULL, cur_filenum, GX.cur_x, GX.cur_y,
                               0.0, 0, 1.0, GX.layernum, TRUE);

         }
         // always store last position. Because after a flash a
         // pendown can happen.
         GX.repeat_pattern = 0;
         ppoly[0].x = GX.cur_x;
         ppoly[0].y = GX.cur_y;
         ppoly[0].bulge = 0;
         ppolycnt = 1;
         // GX.done = TRUE;
      }
      break;
      case PENUP:
      {
         // this is penup
         cont_flag = FALSE;
         ppoly[0].x = GX.cur_x;
         ppoly[0].y = GX.cur_y;
         ppoly[0].bulge = 0;
         ppolycnt = 1;
      }
      break;
      case PATTERNREPEAT:
      {
         // always executer min. 1 M02 command. 
         // repeat_pattern can be set by R command.
         if (GX.repeat_pattern == 0)   GX.repeat_pattern = 1;

         // in GX.repeat_pattern is the count
         // cyrxy is the offset
         // pattern_name is the current pattern.
         double x,y;
         if(gbFlag)
            x = GX.m25_x, y = GX.m25_y;
         else
            x = GX.prevX, y = GX.prevY;
         double rot = 0;   
         int mir = 0;
         if (M70)
         {
            rot-=90;
            mir ^= MIRROR_FLIP;                       
         }

         if (M80_mirrorx == -1)  
         {
            mir ^= MIRROR_FLIP;
         }
         if (M90_mirrory == -1)  
         {
            rot+=180;
            if (mir) 
               mir = 0;
            else
               mir ^= MIRROR_FLIP;
         }

         CString patternName;
         patternName.Format("PATTERN_%d", patterncnt-1); // place the last one and not the current one.
         for (int i=0; i<GX.repeat_pattern; i++)
         {
            x += GX.coo_x;
            y += GX.coo_y;
            DataStruct *data = Graph_Block_Reference(patternName, NULL, cur_filenum, x, y, DegToRad(rot), mir, 1.0, -1, FALSE);
            data->getInsert()->setMirrorFlags(mir);
         }
         GX.repeat_pattern = 0;
         GX.drawmode = PENUP;
         GX.prevX = x;
         GX.prevY = y;
         //GX.m25_x = x;
         //GX.m25_y = y;
      }
      break;
   }    // switch

   GX.done = TRUE;   // need another XY to execute anything
   gbFlag = FALSE;
   return;
}
/****************************************************************************/
/*
*/
static int load_excellonsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   overwrite_format_inch = -1;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found. This is only needed on very
      // special occasions.
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      CString  w = line;
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".FORMAT_INCH"))
         {
            if ((lp = get_string(NULL," ;\n")) == NULL)  continue; 
            overwrite_format_inch = atoi(lp);
         }
      }
   }
   fclose(fp);
   return 1;
}
/*Ende **********************************************************************/
