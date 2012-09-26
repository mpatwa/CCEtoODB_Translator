// $Header: /CAMCAD/4.5/read_wrt/ApRead.cpp 12    2/07/06 4:43p Rick Faltersack $

/****************************************************************************/
/*

;
; This is a macro file to read aperture files.
; (not yet ! every char can also be defined as hex 0x000 to 0x127)
;
; The apertature scanner expect a line driven Aperture list file.
;
; The checking work in sequence. Therefore the most complex .SCANLINE
; should be called first.
; Example .SCANLINE .DCODE
;       .SCANLINE .DCODE .XSIZE
; In this example all lines would find the first scanline, and the second
; one would never be used.
;
; All commands happen between .STARTDEF and .ENDDEF .The Software
; will treat any section seperate, so that you can have i.e. different
; .UNITS for different formats.
;
; .STARTDEF startdefinition name
;    all commands in here
; .ENDDEF
;
;
;
; The following .UNITS values of the input file are implemented:
;   INCH
;   MIL
;   MM
;   TMIL  = 1/10 of mil
;
;
!!!! new implementation for detecting units !!!!
.UNIT_MM    name
.UNIT_MIL   name
.UNIT_INCH  name
.UNIT_TMIL  name
.SCANLINE SKIPWORD .UNIT_VAL
;
.SKIPLINE   name
;
; The following commands are implemented:
;   .UNITS         unit setting
;   .IGNORE        " characters "
;   .TEXTDELIMETER only and always 2 chars
;   .SCANLINE      scanline commands
;   .WORDDELIMETER " characters " - needed to seperate words.
;             i.e 0.2x0.4 x would be a word delimeter
;                 to seperate XSIZE from YSIZE.
            "\"" this delimeter is a quote.
;
;    .LOGICAL_EOF  "char " in detecting this word, scanner stops reading this file.
;   .FORM_ROUND    Name of Round aperture in the input file (can be multiple)
;   .FORM_SQUARE
;   .FORM_RECT
;   .FORM_OBLONG
;   .FORM_THERM
;   .FORM_DONUT
;   .FORM_TARGET
;   .FORM_OCTAGON
;   .FORM_BLANK
;   .FORM_COMPLEX
; The folling .SCANLINE commands are implemented:
;   .XSIZE      Keyword which expects to find a int/float x-coordinate
;   .YSIZE      Keyword which expects to find a int/float y-coordinate
;   .DCODE      Keyword which expects to find a int number with an optional D
;   .APNUM      Keyword which expects to find a int number with an optional A
;   .SKIPWORD   Keyword will skip this entry
;   .FORM       Keyword for form as defined in .FORM section
;
;   .MUST_FORM_ROUND    This Keyword must be a form of a type. This is done
;   .MUST_FORM_SQUARE   to do the following : Form    Size Orient Length 
;   .MUST_FORM_RECT                 Circle  55
;   .MUST_FORM_OBLONG               Finger  50   90     100
;   .MUST_FORM_THERMO               Annulus 50          20
;   .MUST_FORM_DONUT
;   .MUST_FORM_TARGET
;   .MUST_FORM_OCTAGON
;   .MUST_FORM_COMPLEX
;
;   .APROT  Keyword for aperature rotation.
;
.STARTDEF       RPT
        ; this list is for
        ;   1    10    ROUND      50.00x50.00        962        0
        ;.UNITS=inch
        .UNITS=Mil
        ;.UNITS=mm
        ;.UNITS=tmil
        ;
        ; Mapping of Aperture types.
        .FORM_ROUND      ROUND
        .FORM_SQUARE     SQUARE
        .FORM_RECT       RECT
        .FORM_OBLONG     OBLONG
        .FORM_THERM      THERM
        ;
        .IGNORE ""            // inside bracket
        .WORDDELIMETER " \"x,"  // inside bracket
        ;
        ; Usable keywords
        ; XSIZE
        ; YSIZE
        ; SKIPWORD
        ; SKIPALPHA
        ; DCODE
        ; APNUMBER
        ; FORM
        .SCANLINE .APNUMBER .DCODE .FORM .XSIZE .YSIZE
.ENDDEF
;

*/
/****************************************************************************/

#include "StdAfx.h"
#include "General.h"
#include "string.h"
#include "apread.h"
#include "dbutil.h"

/* Function Prototypes *********************************************************/
void MemErrorMessage(char *file, int line);
static void init_mac();
static void free_mac();
static int read_macfile(const char *fname, const char *apfname, FILE *log);
static int read_apfile(const char *fname, int *ap_units,char *macname);
static int do_scan(char *line, int linecnt, int *ap_units, double *ap_units_factor,char *macname);
static int check_command(int cmd, char *w, Def *def);
static int complete_ap(Def *def, double factor);
static int get_apnum(int d);
static int is_intnumber(char *w);
static int do_def(FILE *ifp, int *linecnt, Def *def, const char *aperture_input_file, FILE *flog);
static int get_tok(List *list, int siz, char *u);
static double cnv_unit(char *l, int u);

/* Static Variables *********************************************************/
static Maclist    *maclist;
static Apdef      apdef;
static int        *apnumarray;
static FILE       *flog;
static int        lastfound;     // initialized in init_mac

static float      GLOBAL_APMIN;  // minumum apersize in MIL
static float      GLOBAL_APMAX;  // maximum size to validate according to Gerber in MIL

List  unit_lst[] =
{
   {"INCH",    APP_UNITS_INCH},
   {"MIL",     APP_UNITS_MIL},
   {"MM",      APP_UNITS_MM},
   {"TMIL",    APP_UNITS_TMIL},
   {"HMIL",    APP_UNITS_HMIL},
   {"MICRON",  APP_UNITS_MICRON},
};

/******************************************************************************
* apread
*/
int apread(char *macrofile, const char *inputfile, char *logfile,int *ap_units, char *macName)
{
   strcpy(macName, ""); // init
   if ((flog = fopen(logfile, "wt")) == NULL)   // rewrite filw
      ErrorMessage(logfile, "Can not open logfile.", MB_ICONEXCLAMATION | MB_OK);

   init_mac();
   int res;
   if ((res = read_macfile(macrofile, inputfile, flog)) > -1)  // -1 was already done in do_def
      read_apfile(inputfile, ap_units, macName);
   else
      *ap_units = UNIT_MILS;

   free_mac();
   fclose(flog);
   return(1);
}

/******************************************************************************
* init_mac
*/
void init_mac()
{
   // assign mac
   if ((maclist = (Maclist *)calloc(1, sizeof(Maclist))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if ((apnumarray = (int *)calloc(MAX_APNUM+1,sizeof(int))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   for (int i=0; i<MAX_APNUM; i++)
      apnumarray[i] = i;

   lastfound = -1;
}

/******************************************************************************
* free_mac
*/
void free_mac()
{
   for (int i=0; i<maclist->defcnt; i++)
   {
      if (maclist->def[i].ignore)
         free(maclist->def[i].ignore);
      if (maclist->def[i].worddel)
         free(maclist->def[i].worddel);
      if (maclist->def[i].logical_eof)
         free(maclist->def[i].logical_eof);
      if (maclist->def[i].space)
         free(maclist->def[i].space);

		int ii=0;
      for (ii=0; ii<maclist->def[i].formcnt; ii++)
         if (maclist->def[i].form[ii].formname)
            free(maclist->def[i].form[ii].formname);

      for (ii=0;ii<maclist->def[i].skiplinecnt;ii++)
         if (maclist->def[i].skipline[ii])
            free(maclist->def[i].skipline[ii]);    
   }
   free(apnumarray);
   free(maclist);
}

/******************************************************************************
* clean_quote
*/
char *clean_quote(char *l)
{
   // find first quote.
   unsigned int i;
   static char tmp[255];
   int tmpcnt = 0;

   i = 0;
   while (i < strlen(l) && l[i] != '"')
      i++;

   // now do tmp
   i++;
   while (i < strlen(l) && l[i] != '"')
   {
      // do "\""
      if (l[i] == '\\')
         i++;
      if (tmpcnt < 254)
         tmp[tmpcnt++] = l[i++];
   }

   if (!tmpcnt) 
      return NULL;
   tmp[tmpcnt] = '\0';
   return tmp;
}

/******************************************************************************
* read_apfile
*/
int read_apfile(const char *fname, int *ap_units, char *macname)
{
   FILE *ifp;
   char line[255];
   int linecnt = 0;
   int res;
   double ap_unit_factor = 1;

   if ((ifp = fopen(fname, "rt")) != NULL)
   {
      linecnt = 0;
      while (fgets(line, 255, ifp))
      {
         if (strlen(line) && line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
         linecnt++;
         clean_blank(line);
         if (strlen(line) == 0)
            continue;   // no empty lines.
         if ((res = do_scan(line,linecnt, ap_units, &ap_unit_factor, macname)) == FALSE)
            fprintf(flog, "%3d:Could not find a match for [%s]\n", linecnt, line);
         if (res == -1) // logical eof
            break;
      }
      fclose(ifp);
   }
   else
   {
      ErrorMessage(fname, "Can not open file for reading.", MB_ICONEXCLAMATION | MB_OK);
   }
   return 1;
}

/****************************************************************************/
/*
  Rules to scan a line
  while (!eol)
  {
     read a char
     - if ignore-> continue;
       if isspace
          finish word
       else
          add to word
  }
  // now here is a list of words
  // match them against scanline.
*/
int do_scan( char *line, int linecnt, int *ap_units, double *ap_units_factor, char *macname)
{
   int      i, m, s, mm;
   char     *wordarray[MAX_COMMAND];
   int      wordarraycnt = 0 ;
   char     tmp[255]; // max size of a word, same size as line read, because the line can be a !------ remark.
   int      tmpcnt;  // pointer to word
   int      llen = strlen(line);
   int      lcnt = 0; // pointer to line
   char     c;
   int      result = FALSE;
   int      matchfound = FALSE;
   int      skipfound  = FALSE;
   int      textstart  = FALSE;

   memset(&apdef,0,sizeof(Apdef));

   // here loop throu all maclist->def until matchfound.
   for (mm=-1;mm<maclist->defcnt && !matchfound;mm++)
   {
      // here make sure, that the last found is the first to be tested against.
      if (lastfound == -1 && mm == -1) continue;

      if (lastfound > -1)  // once a selection is made, do not skip anymore.
         m = lastfound;
      else
         m = mm;

      // here tokenize words
      tmpcnt = 0;
      wordarraycnt = 0;
      lcnt = 0;
      while (lcnt < llen)
      {
         c = line[lcnt++];
         if (c == '\t')   c = ' ';

         if (maclist->def[m].ignore &&  // check for null
             strchr(maclist->def[m].ignore,c)) continue;

         // lock against each other for same start and end.
         if (!textstart && maclist->def[m].txtdel[0] == c)
            textstart = TRUE;
         else
         if (textstart && maclist->def[m].txtdel[1] == c)
            textstart = FALSE;

         if (maclist->def[m].logical_eof && !STRICMP(maclist->def[m].logical_eof,tmp))
         {
            return -1; // eof found
         }

         // on textstart do not check for spaces.
         if (((maclist->def[m].worddel && strchr(maclist->def[m].worddel,c))
               || isspace(c)) && !textstart)
         {
            if (tmpcnt) // word was started
            {
               tmp[tmpcnt] = '\0';
               if (wordarraycnt < MAX_COMMAND)
               {
                  if ((wordarray[wordarraycnt] = STRDUP(tmp)) == NULL)
                     MemErrorMessage(__FILE__, __LINE__);
                  wordarraycnt++;
               }
               else
               {
                  // no error here, because more commands would not
                  // be examined anyway
               }
               // end of word
               tmpcnt = 0;
            }
            else
            {
               // no word was started, no harm done
            }
         }
         else
         {
            tmp[tmpcnt++] = c;
         }
      }
      // last word
      if (tmpcnt) // word was started
      {
         tmp[tmpcnt] = '\0';
         if (wordarraycnt < MAX_COMMAND)
         {
            if ((wordarray[wordarraycnt] = STRDUP(tmp)) == NULL)
               MemErrorMessage(__FILE__, __LINE__);
            wordarraycnt++;
         }
      }

      result = 0;
      for (s=0;s<maclist->def[m].scanlinecnt && !matchfound && !skipfound;s++)
      {
         result = 0;
         skipfound = FALSE;
         // first check if this is a skipline command.
         if (wordarraycnt)
         {
            for (i=0;i<maclist->def[m].skiplinecnt;i++)
            {         
               // only check the first word.
               if (!STRNICMP(wordarray[0],maclist->def[m].skipline[i],
                     strlen(maclist->def[m].skipline[i])))
               {
                  // here skip this
                  skipfound = TRUE;
                  matchfound = TRUE;
                  lastfound = m;
                  break;
               }
            }
         }

         // here compare throu all scanlines.
         if (!skipfound && wordarraycnt >= maclist->def[m].scanline[s].commandcnt)  
            // found number of words and scanline commands.
         {

            // it is allowed to be more, because the end of line must not
            // be defined complete
            matchfound = FALSE;
            for (i=0;i< maclist->def[m].scanline[s].commandcnt;i++)
            {
               int r = check_command(maclist->def[m].scanline[s].command[i],
                   wordarray[i],&maclist->def[m]);

               if (r < 0)
               {
                  matchfound = TRUE;
                  lastfound  = m;
                  break;   // this result is not an aperture
                           // scan, but otherwise a correct scan line
               }
               else
               if (r > 0)
               {
                  result++;
               }
               else
               {
                  result = 0;
                  break;
               }
               if (result == maclist->def[m].scanline[s].commandcnt)
               {
                  // here found and all good.

                  matchfound = TRUE;
                  lastfound  = m;
                  CpyStr(macname,maclist->def[m].listname,64);

                  switch (maclist->def[m].units)
                  {
                     case APP_UNITS_INCH:
                        *ap_units = UNIT_INCHES;
                        *ap_units_factor = 1;
                     break;
                     case APP_UNITS_MM:
                        *ap_units = UNIT_MM;
                        *ap_units_factor = 1;
                     break;
                     case APP_UNITS_MICRON:
                        *ap_units = UNIT_MM;
                        *ap_units_factor = 0.001;
                     break;
                     case APP_UNITS_TMIL:
                        *ap_units = UNIT_MILS;
                        *ap_units_factor = 0.1;
                     break;
                     case APP_UNITS_HMIL:
                        *ap_units = UNIT_MILS;
                        *ap_units_factor = 0.01;
                     break;
                     default:
                        *ap_units = UNIT_MILS;
                        *ap_units_factor = 1;
                     break;

                  }

                  complete_ap(&maclist->def[m], *ap_units_factor);
                  fprintf(flog,"[%s] %3d:Match found ! -> %s\n", maclist->def[m].listname,linecnt,line);
                  output_app(&apdef);

                  break;
               }
            }
         }
      } // for all scanlines
      // here free wordarray
      for (i=0;i<wordarraycnt;i++)
      {
         free(wordarray[i]);
         wordarray[i] = NULL;
      }
      wordarraycnt = 0;
   } // loop thru all maclist

   return result;  // no match found;
}

/****************************************************************************/
/*
   here needs to be updated - if app and no dcode
   here needs to be updated - form and type

   A1..10 = D10..19
   A11..12= D70..71
   A13..22= D20..29
   A23..24= D72..73
   A25..  = D30..69
   A      = D74..

*/
int complete_ap(Def *def, double factor)
{
   int   i;
   Apdef tmp;

   memcpy(&tmp,&apdef,sizeof(Apdef));  // just for debug.

   // here update appform
   for (i=0;i<def->formcnt;i++)
   {
      if (apdef.appform && !STRCMPI(apdef.appform,def->form[i].formname))
      {
         apdef.apptype = def->form[i].formtype;
         break;
      }
   }
  
   //if there is a local max/min in the macro, then use this value
   if (def->localApSizeSet)
   {
      apdef.localApMax = def->localApMax;
      apdef.localApMin = def->localApMin;
   }
   //otherwise, use the global max/min.
   else
   {
      apdef.localApMax = GLOBAL_APMAX;
      apdef.localApMin = GLOBAL_APMIN;
   }

   apdef.xsize *= factor;
   apdef.ysize *= factor;
   apdef.xoffset *= factor;
   apdef.yoffset *= factor;

   // here update apnum from dcode
   if (apdef.dcode && !apdef.apnum)
   {
      apdef.apnum = get_apnum(apdef.dcode);
   }
   else
   // here update dcode from apnum
   if (apdef.apnum && !apdef.dcode)
   {
      apdef.dcode = apnumarray[apdef.apnum];
   }

   // optimize
   switch (apdef.apptype)
   {
      case FORM_RECT:
         if (apdef.xsize == apdef.ysize)
         {
            apdef.apptype = FORM_SQUARE;
         }
         if (apdef.ysize == 0.0)
         {
            apdef.apptype = FORM_SQUARE;
         }
      break;
      case FORM_OBLONG:
         if (apdef.xsize == apdef.ysize)
         {
            apdef.apptype = FORM_ROUND;
         }
         if (apdef.ysize == 0.0)
         {
            apdef.apptype = FORM_ROUND;
         }
      break;
      case FORM_THERM:  // if only one size is supplied
         if (apdef.ysize > apdef.xsize)
         {
            // on thermals, the Y must be smaller x.
            double tmp = apdef.ysize;
            apdef.ysize = apdef.xsize;
            apdef.xsize = tmp;
         }
      
         // do not allow same x or y size.
         if (apdef.ysize == apdef.xsize)
            apdef.ysize = 0.8 * apdef.xsize;
         else
         if (apdef.ysize == 0)
            apdef.ysize = 0.8 * apdef.xsize;
      break;
   }

   return 0;
}

/****************************************************************************/
/*
  here now check if this w fits this command
*/
int check_command(int cmd, char *w, Def *def)
{
   int   ret = 0;
   int   i;

   switch (cmd)
   {
      case SCAN_UNIT_VAL:
      {
         // This return needs special treatment, because the scanline is
         // good, but not an aperture.
         if (def->def_unit_mm)
         {
            if (!STRCMPI(def->def_unit_mm,w))
            {
               // seletect unit is MM update def->units
               def->units = APP_UNITS_MM;
               return -1;
            }
         }
         
         if (def->def_unit_mil)
         {
            if (!STRCMPI(def->def_unit_mil,w))
            {
               // seletect unit is MM update def->units
               def->units = APP_UNITS_MIL;
               return -1;
            }
         }

         if (def->def_unit_tmil)
         {
            if (!STRCMPI(def->def_unit_tmil,w))
            {
               // seletect unit is MM update def->units
               def->units = APP_UNITS_TMIL;
               return -1;
            }
         }

         if (def->def_unit_hmil)
         {
            if (!STRCMPI(def->def_unit_hmil,w))
            {
               // seletect unit is MM update def->units
               def->units = APP_UNITS_HMIL;
               return -1;
            }
         }

         if (def->def_unit_micron)
         {
            if (!STRCMPI(def->def_unit_hmil,w))
            {
               // seletect unit is MM update def->units
               def->units = APP_UNITS_MICRON;
               return -1;
            }
         }
         
         if (def->def_unit_inch)
         {
            if (!STRCMPI(def->def_unit_inch,w))
            {
               // seletect unit is MM update def->units
               def->units = APP_UNITS_INCH;
               return -1;  
            }
         }
      }
      break;
      case SCAN_APROTATION:
         if (is_number(w))
         {
            apdef.rotation = atoi(w);
            ret++;
         }
      break;
      case SCAN_XSIZE:
         if (is_number(w))
         {
            double z;
            apdef.xsize = atof(w);  // apertures are in native units
            z = cnv_unit(w,def->units);   // the check is in mil
            // here check units
         if (def->localApSizeSet)
         {
                if (z == 0 || (z >= def->localApMin && z <= def->localApMax))
               ret++;
         }
         else
                if (z == 0 || (z >= GLOBAL_APMIN && z <= GLOBAL_APMAX))
               ret++;
         }
      break;
      case SCAN_YSIZE:
         if (is_number(w))
         {
            double z;
            apdef.ysize = atof(w);
            z = cnv_unit(w, def->units);
         if (def->localApSizeSet)
         {
            if (z == 0 || (z >= def->localApMin && z <= def->localApMax))
               ret++;
         }
         else
            if (z == 0 || (z >= GLOBAL_APMIN && z <= GLOBAL_APMAX))
               ret++;
         }
      break;
      case SCAN_DCODE:
         if (w[0] == 'D' || w[0] == 'd')  w[0] = '0'; // some Dcodes have a Dxx
                                       // others only xx
         if (is_intnumber(w))
         {
            if (atoi(w) > 9 && atoi(w) < 1000)
            {
               apdef.dcode = atoi(w);
               ret++;
            }
         }
      break;
      case SCAN_APNUM:
         if (w[0] == 'A' || w[0] == 'a')  w[0] = '0'; // some Apnum have a Axx
                                       // others only xx
         if (is_intnumber(w))
         {
            if (atoi(w) > 0 && atoi(w) < 1000)
            {
               apdef.apnum = atoi(w);
               ret++;
            }
         }
      break;
      case SCAN_FORM:
         CpyStr(apdef.appform,w,20);
         ret++;
      break;
      case SCAN_SKIPWORD:
         ret++;          // this is always ok.
      break;
      case SCAN_MUST_FORM_ROUND:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_ROUND && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_SQUARE:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_SQUARE && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_RECT:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_RECT && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_OBLONG:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_OBLONG && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_THERM:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_THERM && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_DONUT:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_DONUT && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_TARGET:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_TARGET && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_OCTAGON:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_OCTAGON && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_BLANK:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_BLANK && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      case SCAN_MUST_FORM_COMPLEX:
         // here update appform
         for (i=0;i<def->formcnt;i++)
         {
            if (def->form[i].formtype == FORM_COMPLEX && !STRCMPI(w,def->form[i].formname))
            {
               CpyStr(apdef.appform,w,20);
               ret++;
               break;
            }
         }
      break;
      default:
      {
         char  t[80];
         sprintf(t, "Command [%d] not checked\n",cmd);
         ErrorMessage(t, "Error", MB_ICONEXCLAMATION | MB_OK);
      }
      break;
   }
   return ret;
}

/****************************************************************************/
/*
*/
int read_macfile(const char *fname, const char *apfname, FILE *flog)
{
   FILE  *ifp;
   char  line[255];
   int   linecnt = 0, res = 0;
   char  *lp;

   GLOBAL_APMIN = (float)MIN_APSIZE;
   GLOBAL_APMAX = (float)MAX_APSIZE;

   if ((ifp = fopen(fname,"rt")) != NULL)
   {
      linecnt = 0;
      while (fgets(line,255,ifp) && res > -1)   // res -1 means, that the reader was executed in do_def
      {
         if (strlen(line) && line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
         linecnt++;
         clean_blank(line);
         if (line[0] != '.')  continue;

         if ((lp = strtok(line,"=: \t\n")) == NULL)  continue;

         // here wait for start command
         if (!STRCMPI(lp,".ASSIGN"))
         {
            int ap,dc;
            if ((lp = strtok(NULL,"=: \t\n")) == NULL)  continue;
            ap = atoi(lp);
            if ((lp = strtok(NULL,"=: \t\n")) == NULL)  continue;
            dc = atoi(lp);
            if (ap < 0 || ap > MAX_APNUM)               continue;
            apnumarray[ap] = dc;
         }
         else
         if (!STRCMPI(lp,".STARTDEF"))
         {
            if ((lp = strtok(NULL,"=: \t\n")) == NULL)  continue;
            if (maclist->defcnt < MAX_DEF)
            {
               strcpy(maclist->def[maclist->defcnt].listname,lp);
               if ((do_def(ifp, &linecnt ,&maclist->def[maclist->defcnt], apfname, flog)) < 0)
               {
                  res = -1;
                  continue;
               }
               maclist->defcnt++;
            }
            else
               printf("Too many STARTDEF\n");
         }
         else if (!STRCMPI(lp, ".GLOBALAPMIN"))
         {
            //this section reads the global minimum aperture size 
            if ((lp = strtok(NULL,"=: \t\n")) == NULL)  
               continue;
                GLOBAL_APMIN = (float)atof(lp);
            char *temp = lp;
            if ((lp = strtok(NULL,"=: \t\n")) == NULL)  
               continue;
            int unit = APP_UNITS_MIL;
            if (!STRICMP(lp, "IN"))
               unit = APP_UNITS_INCH;
            if (!STRICMP(lp, "MM"))
               unit = APP_UNITS_MM;
            GLOBAL_APMIN = (float)cnv_unit(temp, unit );
         }
         else if (!STRCMPI(lp, ".GLOBALAPMAX"))
         {
            //this section reads the global maximum aperture size 
            if ((lp = strtok(NULL,"=: \t\n")) == NULL)  
               continue;
                GLOBAL_APMAX = (float)atof(lp);
            char *temp = lp;
            if ((lp = strtok(NULL,"=: \t\n")) == NULL)  
               continue;
            int unit = APP_UNITS_MIL;
            if (!STRICMP(lp, "IN"))
               unit = APP_UNITS_INCH;
            if (!STRICMP(lp, "MM"))
               unit = APP_UNITS_MM;
            GLOBAL_APMAX = (float)cnv_unit(temp, unit );
         }
      } // while
      fclose(ifp);
   }
   else
   {
      ErrorMessage(fname, "Can not open file for reading.", MB_ICONEXCLAMATION | MB_OK);
   }
   return res;
}

/****************************************************************************/
/*
*/
int do_def(FILE *ifp,int *linecnt,Def *def, const char *apfname, FILE *flog)
{
   char  line[255];
   char  *lp;
   int   tokptr;

// printf(" Reading [%s]\n",def->listname);
   def->localApSizeSet = FALSE;
   while (fgets(line,255,ifp))
   {
      if (strlen(line) && line[strlen(line)-1] == '\n')
         line[strlen(line)-1] = '\0';
      *linecnt = *linecnt + 1;
      clean_blank(line);
      if (line[0] != '.')  continue;

      if ((lp = strtok(line,"=: \t\n")) == NULL)  continue;

      if (!STRCMPI(lp,".ENDDEF"))                 return 1;

      switch (tokptr = get_tok(tok_lst,SIZ_TOK_LST,lp))
      {
         case TOK_SKIPLINE:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            if (def->skiplinecnt < MAX_SKIPLINE)
            {
               if ((def->skipline[def->skiplinecnt] = STRDUP(lp)) == NULL)
               {
                  MemErrorMessage(__FILE__, __LINE__);
               }
               def->skiplinecnt++;
            }
            else
            {
               ErrorMessage("Too many skiplines.", "Error", MB_ICONEXCLAMATION | MB_OK);
            }
         break;
         case TOK_UNIT_MM:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            CpyStr(def->def_unit_mm,lp,20);
         break;
         case TOK_UNIT_MIL:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            CpyStr(def->def_unit_mil,lp,20);
         case TOK_UNIT_INCH:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            CpyStr(def->def_unit_inch,lp,20);
         break;
         case TOK_UNIT_TMIL:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            CpyStr(def->def_unit_tmil,lp,20);
         break;
         case TOK_UNIT_HMIL:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            CpyStr(def->def_unit_hmil,lp,20);
         break;
         case TOK_UNIT_MICRON:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            CpyStr(def->def_unit_micron,lp,20);
         break;
         case TOK_UNITS:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            if ((def->units = get_tok(unit_lst,SIZ_UNIT_LST,lp)) == SEARCH_UNKNOWN)
            {
               printf("Error : Unknown .UNITS [%s] at %d\n",lp,linecnt);
            }
         break;
         case TOK_IGNORE:
            // the problem is a user error to type ""
            if ((lp = strtok(NULL,"\n")) == NULL)  continue;
            if ((lp = clean_quote(lp))   == NULL)  continue;
            if ((def->ignore = STRDUP(lp)) == NULL)
            {
               MemErrorMessage(__FILE__, __LINE__);
            }
         break;
         case TOK_WRDDEL:
            // the problem is a user error to type
            if ((lp = strtok(NULL,"\n")) == NULL)  continue;
            if ((lp = clean_quote(lp))   == NULL)  continue;
            if ((def->worddel = STRDUP(lp)) == NULL)
            {
               MemErrorMessage(__FILE__, __LINE__);
            }
         break;
         case TOK_LOGICAL_EOF:
            // here is eof 
            if ((lp = strtok(NULL,"\n")) == NULL)  continue;
            if ((lp = clean_quote(lp))   == NULL)  continue;
            if ((def->logical_eof = STRDUP(lp)) == NULL)
            {
               MemErrorMessage(__FILE__, __LINE__);
            }
         break;
         case TOK_TXTDEL:
            if ((lp = strtok(NULL,":= \t\n")) == NULL)  continue;
            if (strlen(lp) != 2)
            {
               ErrorMessage("Error in Textdelimeter : must have 2 char (start and end).",
                  "Error", MB_ICONEXCLAMATION | MB_OK);
            }
            else
            {
               def->txtdel[0] = lp[0];
               def->txtdel[1] = lp[1];
            }
         break;
         case TOK_SCANLINE:
            if (def->scanlinecnt < MAX_SCANLINE)
            {
               def->scanline[def->scanlinecnt].commandcnt = 0;
               while ((lp = strtok(NULL,":= \t\n")) != NULL)
               {
                  if (def->scanline->commandcnt < MAX_COMMAND)
                  {
                     if ((def->scanline[def->scanlinecnt].command[def->scanline[def->scanlinecnt].commandcnt] =
                        get_tok(scan_lst,SIZ_SCAN_LST,lp)) == SEARCH_UNKNOWN)
                     {
                        printf("Unknown command [%s] found in line %d\n",
                           lp,*linecnt);
                     }
                     def->scanline[def->scanlinecnt].commandcnt++;
                  }
                  else
                  {
                     ErrorMessage("Too many commands in scanline.", "Error", MB_ICONEXCLAMATION | MB_OK);
                  }
               }
               def->scanlinecnt++;
            }
            else
            {
               ErrorMessage("Too many scanlines.", "Error", MB_ICONEXCLAMATION | MB_OK);
            }
         break;
         case FORM_ROUND:
         case FORM_SQUARE:
         case FORM_RECT:
         case FORM_OBLONG:
         case FORM_THERM:
         case FORM_DONUT:
         case FORM_TARGET:
         case FORM_OCTAGON:
         case FORM_BLANK:
         case FORM_COMPLEX:
            if ((lp = strtok(NULL," \t")) == NULL)  continue;
            if (def->formcnt < MAX_FORM)
            {
               if ((def->form[def->formcnt].formname = STRDUP(lp)) == NULL)
               {
                  MemErrorMessage(__FILE__, __LINE__);
               }
               def->form[def->formcnt].formtype = tokptr;
               def->formcnt++;
            }
            else
            {
               MemErrorMessage(__FILE__, __LINE__);
            }
         break;
         case READ_TERADYNE_1:
            read_teradyne_1(apfname,flog);
            return -1; // return -1 is MIL units
         break;
         case READ_OLD_PCGERBER:
            read_old_pcgerber(apfname,flog);
            return -1;  // return -1 is mil units
         break;
         case SEARCH_UNKNOWN:
         {
            char  t[80];
            sprintf(t,"Unknown Token [%s] -> %d - %s\n",lp, *linecnt,line);
            ErrorMessage(t, "Error in MAC file.", MB_ICONEXCLAMATION | MB_OK);
         }
         break;
         case TOK_LOCALAPMIN:
            {
               //to set the local min aperture size.
               if ((lp = strtok(NULL," \t")) == NULL)  
                  continue;
               def->localApMin =  (float)atof(lp); 
               char *temp = lp;

               if ((lp = strtok(NULL," \t")) == NULL)  
                  continue;
               int unit = APP_UNITS_MIL;
               if (!STRICMP(lp, "IN"))
                  unit = APP_UNITS_INCH;
               if (!STRICMP(lp, "MM"))
                  unit = APP_UNITS_MM;
               def->localApMin = (float)cnv_unit(temp, unit ); //cnv it to mils

               def->localApSizeSet = TRUE;
            }
            break;
         case TOK_LOCALAPMAX:
            {
               //to set the local max aperture size.
               if ((lp = strtok(NULL," \t")) == NULL)  
                  continue;
               def->localApMax = (float)atof(lp);
               char *temp = lp;

               if ((lp = strtok(NULL," \t")) == NULL)  
                  continue;
               int unit = APP_UNITS_MIL;
               if (!STRICMP(lp, "IN"))
                  unit = APP_UNITS_INCH;
               if (!STRICMP(lp, "MM"))
                  unit = APP_UNITS_MM;
               def->localApMax = (float)cnv_unit(temp, unit );  //cmv it to mils

               def->localApSizeSet = TRUE;
            }
            break;
         default:
         {
            char  t[80];
            sprintf(t,"Not implemented [%s] -> %d - %s\n",lp,*linecnt,line);
            ErrorMessage(t, "Error in MAC file.", MB_ICONEXCLAMATION | MB_OK);
         }
         break;
      }
   }// while
   return 1;
}

/****************************************************************************/
/*
*/
int get_tok(List *list, int siz, char *u)
{
   int   i;

   for (i=0;i<siz;i++)
   {                  
      if (!STRNICMP(u,list[i].token,strlen(u)))
         return list[i].type;
   }
   return SEARCH_UNKNOWN;
}

/****************************************************************************/
/*
  Number must be Interger, so that Size and Ap or DCode are different.
*/
int is_intnumber(char *w)
{
   unsigned int   i;

   if (strchr(w,'.'))
      return FALSE;
   for (i=0;i<strlen(w);i++)
   {
      if (!isdigit(w[i]))   return FALSE;
   }

   return TRUE;
}

/****************************************************************************/
/*
  normalize output to MIL - used only for checking
*/
double cnv_unit(char *l, int u)
{
   double x = atof(l);

   switch (u)
   {
      case APP_UNITS_INCH:
         x *= 1000;
      break;
      case APP_UNITS_MIL:
      break;
      case APP_UNITS_TMIL:
         x *= 0.1;
      break;
      case APP_UNITS_HMIL:
         x *= 0.01;
      break;
      case APP_UNITS_MM:
         x *= 100/2.54; // 39.xxx
      break;
      case APP_UNITS_MICRON:
         x *= 100/2.54; // 39.xxx
         x *= 0.001;
      break;
   }
   return x;
}

/****************************************************************************/
/*
*/
int get_apnum(int d)
{
   int   i;
   for (i=0;i<MAX_APNUM;i++)
   {
      if (apnumarray[i] == d)
         return i;
   }
   return 0;
}


/****************************************************************************/
/*
*/
/****************************************************************************/
