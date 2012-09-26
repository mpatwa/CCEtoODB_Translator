// $Header: /CAMCAD/4.5/read_wrt/DmisIn.cpp 11    5/20/05 5:19p Lynn Phung $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
          
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include "ogp.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern   CProgressDlg *progress;
#define  MAX_LINE       64000 // yes 64k

#define  ANGDEC         0
#define  ANGRAD         1

/****************************************************************************/
/*
*/

typedef struct
{
   CString  name;
   CString  inhalt;  
} Tolerancebuf;
typedef CTypedPtrArray<CPtrArray, Tolerancebuf*> TolerancebufArray;

typedef struct
{
   CString  command;
   CString  name; 
} Outputbuf;
typedef CTypedPtrArray<CPtrArray, Outputbuf*> OutputbufArray;

typedef struct
{
   CString     featurename;
   DataStruct  *d;   
} Feature;
typedef CTypedPtrArray<CPtrArray, Feature*> FeatureArray;

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

static int  dmis_skip();
static int  dmis_dmismn();
static int  dmis_units();
static int  dmis_feature();
static int  dmis_tolerance(), dmis_construct();
static int  dmis_meas(), dmis_endmes();
static int  dmis_output();
static int  dmis_datdef();
static int  dmis_snset();

static List command_lst[] =
{
   {"$$",         dmis_skip},          // remark
   {"DMISMN",     dmis_dmismn},        // filename
   {"UNITS",      dmis_units},         // 
   {"T",          dmis_tolerance},     // 
   {"F",          dmis_feature},       // 
   {"FA",         dmis_feature},       // 
   {"MEAS",       dmis_meas},          // this loops until ENDMES
   {"ENDMES",     dmis_endmes},
   {"OUTPUT",     dmis_output},
   {"CONST",      dmis_construct},
   {"DATDEF",     dmis_datdef},
   {"SNSET",      dmis_snset},
};
#define  SIZ_OF_COMMAND (sizeof(command_lst) / sizeof(List))

/****************************************************************************/
/*
   Global Commands
*/
static   int         do_dmis();
static   int         load_dmissettings(const CString fname);

static int           display_error;
static FILE          *flog;

static   FILE        *ifp;
static   char        *ifp_line;
static   long        ifp_linecnt = 0;
static   char        *curlp;
static   char        last_delimeter;
static   CString     tmp_line;   // last input line

static   CCEtoODBDoc  *doc;
static   FileStruct  *file;         // this is the board file

static   double      scale_factor = 1;
static   int         angle_unit = ANGDEC; 
static   int         page_unit;

static   int         cur_layernum;
static   int         SNSET_PROFILE;
static   int         SNSET_RING;
static   int         SNSET_SURFACE;

static   DataStruct  *lastdata;

static   TolerancebufArray tolerancebufarray;
static   int         tolerancebufcnt;

static   OutputbufArray outputbufarray;
static   int         outputbufcnt;

static   FeatureArray   featurearray;
static   int         featurecnt;


/******************************************************************************
* ReadDMIS
*/
void ReadDMIS(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   display_error = FALSE;
   
   doc = Doc;
   file = NULL;

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(f,fname);
   strcat(f,ext);
   
   SNSET_PROFILE = -1;
   SNSET_RING = -1;
   SNSET_SURFACE = -1;

   // all units are normalized to INCHES
   scale_factor = Units_Factor(UNIT_INCHES, pageunits); // just to initialize
   page_unit = pageunits;

   if ((ifp_line = (char  *)calloc(MAX_LINE,sizeof(char))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   tolerancebufarray.SetSize(100,100);
   tolerancebufcnt = 0;

   outputbufarray.SetSize(100,100);
   outputbufcnt = 0;

   featurearray.SetSize(100,100);
   featurecnt = 0;

   if ((ifp = fopen(path_buffer,"rt")) == NULL)
   {
      ErrorMessage("Error open file", path_buffer);
      return;
   }

   CString logFile = GetLogfilePath("DMIS.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage(logFile, "Can not open Logfile", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   ifp_linecnt = 0;
   file = Graph_File_Start(fname, Type_DMIS);

   CString settingsFile( getApp().getImportSettingsFilePath("dmis.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nDMIS Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_dmissettings(settingsFile);

   cur_layernum = Graph_Level("DRAWING","",0);
   lastdata = NULL;
   do_dmis();

   fclose(ifp);

	int i=0;
   for (i=0;i<tolerancebufcnt;i++)
      delete tolerancebufarray[i];
   tolerancebufarray.RemoveAll();

   // the individual record delete is done in the output routine
   outputbufarray.RemoveAll();

   for (i=0;i<featurecnt;i++)
      delete featurearray[i];
   featurearray.RemoveAll();
   featurecnt = 0;

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*
*/
static double   cnv_tok(char *l)
{
   double x;

   if (l == NULL)
   {
      return 0.0;
   }

   if (l && strlen(l))
   {
      x = atof(l);
   }
   else
     return 0.0;

   x = x * scale_factor;

   return x;
}

/****************************************************************************/
/*
*/
static double   cnv_angle(char *l)
{
   double x;

   if (l == NULL)
   {
      return 0.0;
   }

   if (l && strlen(l))
   {
      x = atof(l);
   }
   else
     return 0.0;

   if (angle_unit == ANGDEC)
   {
      x = DegToRad(x);
   }
   else
   {
      // radians
   }

   return x;
}

/****************************************************************************/
/*
   there can be a name ending in a - sign
*/
static int contline(const char *t)
{
   if (strlen(t) && t[strlen(t)-1] == '$' && isspace(t[strlen(t)-2]))
      return 1;
   return 0;
}

/****************************************************************************/
/*
*/
static char *get_nextline(char *string,int n,FILE *fp)
{
   char   *res;
   char   tmp[MAX_LINE];
   CString  t;

   ifp_linecnt++;
   res = fgets(tmp,MAX_LINE,fp);

   t = tmp;
   t.TrimLeft();
   t.TrimRight();
   strcpy(tmp,t);
   strcpy(string,t);

   if (contline(string))
   {
      string[strlen(string)-1] = '\0';
   }

   while (contline(tmp))
   {
      // kill -
      tmp[strlen(tmp)-1] = '\0';

      ifp_linecnt++;
      res = fgets(tmp,MAX_LINE,fp);
      t = tmp;
      t.TrimLeft();
      t.TrimRight();
      strcpy(tmp,t);

      if ((int)strlen(string)+(int)strlen(tmp) < n)
      {
         strcat(string,tmp);
         if (string[strlen(string)-1] == '-')
            string[strlen(string)-1] = '\0';
      }
      else
      {
         CString  w;
         w.Format(" At line %ld",ifp_linecnt);
         ErrorMessage("Error in Linelength", w);
      }
   }

   return res;
}

/****************************************************************************/
/*
*/
static int isdelimiter(char c)
{

   if (c == '/')  
   {
      last_delimeter = c;
      return TRUE;
   }

   if (c == ',')  
   {
      last_delimeter = c;
      return TRUE;
   }

   if (c == '\'') 
   {
      last_delimeter = c;
      return TRUE;
   }

   if (c == '\"') 
   {
      last_delimeter = c;
      return TRUE;
   }

   if (c == '(')  
   {
      last_delimeter = c;
      return TRUE;
   }

   if (c == ')')  
   {
      last_delimeter = c;
      return TRUE;
   }

   if (c == '=')  
   {
      last_delimeter = c;
      return TRUE;
   }

   if (isspace(c))
   {
      last_delimeter = c;
      return TRUE;
   }
   return FALSE;
}

/****************************************************************************/
/*
   like strtok, lp1 is the first and NULL to continue
*/
static int get_nextword(char *lp1,char *token,int maxtok)
{
   int      res = 0;
   int      cnt;

   if (lp1)
      curlp = lp1;

   // advance to the next non-delimiter
   while (*curlp && isdelimiter(*curlp))  *curlp++;

   // do until white space
   cnt = 0;
   while (*curlp && !isdelimiter(*curlp)) 
   {
      if(cnt < maxtok-1)
      token[cnt++] = *curlp;
      *curlp++;
      res = 1; // need to read min one char.
   }
   token[cnt] = '\0';

   CString  tmp;
   tmp = token;
   tmp.TrimLeft();
   tmp.TrimRight();

   strcpy(token, tmp);

   return res;
}

/****************************************************************************/
/*
*/
static int load_dmissettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp,"DMIS Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         // here commands
      }
   }

   fclose(fp);
   return 1;
}

/****************************************************************************/
/*
*/
static int is_command(char *l,List *c_list, int siz_of_command)
{
   int   i;

   for (i=0;i<siz_of_command;i++)
   {
      if (!STRCMPI(c_list[i].token,l))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int do_dmis()
{
   char  *lp;
   char  tok[80];
   int   code;
   long  tmp_linecnt; 

   // here do a while loop
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      tmp_linecnt = ifp_linecnt; // only debug
      tmp_line = ifp_line;
      if ((lp = strtok(ifp_line,"\n")) == NULL) continue;
      if (get_nextword(lp,tok,80) == 0)   continue;

      // logical end of file.
      if (!STRCMPI(tok, "ENDFIL"))  break;

      if ((code = is_command(tok,command_lst,SIZ_OF_COMMAND)) < 0)
      {
         fprintf(flog,"Unknown DMIS Token [%s] at %ld\n",tok, ifp_linecnt);
         display_error++;
      }
      else
      {
         (*command_lst[code].function)();
      }
   }

   return 1;
}

/****************************************************************************/
/*
   Parsing error.
*/
static int p_error(const char *mess)
{
   fprintf(flog,"Fatal Parsing Error : \"%s\" on line %ld\n",mess,ifp_linecnt);
   
   return -1;
}

/****************************************************************************/
/*
*/
static int dmis_do_snset(DataStruct *d)
{
   if (SNSET_PROFILE > 0)
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_Light_Profile, TRUE),
            VT_INTEGER,
            &SNSET_PROFILE, SA_APPEND, NULL); // x, y, rot, height
   }
   if (SNSET_SURFACE > 0)
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_Light_Surface, TRUE),
            VT_INTEGER,
            &SNSET_SURFACE, SA_APPEND, NULL); // x, y, rot, height
   }
   if (SNSET_RING > 0)
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_Light_Ring, TRUE),
            VT_INTEGER,
            &SNSET_RING,  SA_APPEND, NULL); // x, y, rot, height
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_skip()
{
   // do nothing
   
   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_dmismn()
{
   char  tok[80];

   if (get_nextword(NULL,tok,80) == 0) return p_error("dmismn expected");
   file->setName(tok);

   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_circle()
{
   char  tok[80];

   if (get_nextword(NULL,tok,80) == 0) return p_error("circle var_1 expected");
   // INNER, OUTER

   if (!STRCMPI(tok,"INNER"))
   {
      // here var 2 follows
      if (get_nextword(NULL,tok,80) == 0) return p_error("circle var_2 expected");
      // CART, POL
      if (!STRCMPI(tok,"CART"))
      {
         double   x,y,z,i,j,k, diam;
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle x expected");
         x = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle y expected");
         y = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle z expected");
         z = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle i expected");
         i = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle j expected");
         j = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle k expected");
         k = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle diam expected");
         diam = cnv_tok(tok);
         lastdata = Graph_Circle(cur_layernum,x, y, diam/2, 0, 0, 0, 0);
         dmis_do_snset(lastdata);
      }
      else
      if (!STRCMPI(tok,"POL"))
      {
         // r,a,h,i,j,k
         fprintf(flog,"POL CIRCLE not implemented at %ld\n", ifp_linecnt);
         display_error++;
         return -1;
      }
      else
      {
         fprintf(flog,"Unknown CIRCLE var_2 [%s] found at %ld\n", tok, ifp_linecnt);
         display_error++;
         return -1;
      }
   }
   else
   if (!STRCMPI(tok,"OUTER"))
   {
      // here var 2 follows
      if (get_nextword(NULL,tok,80) == 0) return p_error("circle var_2 expected");
      // CART, POL
      if (!STRCMPI(tok,"CART"))
      {
         double   x,y,z,i,j,k, diam;
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle x expected");
         x = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle y expected");
         y = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle z expected");
         z = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle i expected");
         i = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle j expected");
         j = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle k expected");
         k = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("circle diam expected");
         diam = cnv_tok(tok);
         lastdata = Graph_Circle(cur_layernum,x, y, diam/2, 0, 0, 0, 0);
         dmis_do_snset(lastdata);
      }
      else
      if (!STRCMPI(tok,"POL"))
      {
         // r,a,h,i,j,k
         fprintf(flog,"POL CIRCLE not implemented at %ld\n", ifp_linecnt);
         display_error++;
         return -1;
      }
      else
      {
         fprintf(flog,"Unknown CIRCLE var_2 [%s] found at %ld\n", tok, ifp_linecnt);
         display_error++;
         return -1;
      }
   }
   else
   {
      fprintf(flog,"Unknown CIRCLE var_1 [%s] found at %ld\n", tok, ifp_linecnt);
      display_error++;
      return -1;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_point()
{
   char  tok[80];

   if (get_nextword(NULL,tok,80) == 0) return p_error("point var_1 expected");
   // CART, POL
   if (!STRCMPI(tok,"CART"))
   {
      double   x,y,z,i,j,k;
      if (get_nextword(NULL,tok,80) == 0) return p_error("point x expected");
      x = cnv_tok(tok);
      if (get_nextword(NULL,tok,80) == 0) return p_error("point y expected");
      y = cnv_tok(tok);
      if (get_nextword(NULL,tok,80) == 0) return p_error("point z expected");
      z = cnv_tok(tok);
      if (get_nextword(NULL,tok,80) == 0) return p_error("point i expected");
      i = cnv_tok(tok);
      if (get_nextword(NULL,tok,80) == 0) return p_error("point j expected");
      j = cnv_tok(tok);
      if (get_nextword(NULL,tok,80) == 0) return p_error("point k expected");
      k = cnv_tok(tok);

      // i, j ,k
      // 1, 0 ,0 -> Strong right
      // -1, 0 ,0 -> Strong left
      // 0, 1, 0  -> Strong down
      // 0, -1, 0 -> Strong up
      // 0, 0, 1 -> max Contrast Focus

      // if a vector is closed to a given direction - use it.
      // on point, the light value MAX_CONTRAST_GRID from grid value LITDEF/GRID
      lastdata = Graph_Point(x, y, cur_layernum, 0, 0);
      dmis_do_snset(lastdata);
   }
   else
   if (!STRCMPI(tok,"POL"))
   {
      // r,a,h,i,j,k
      fprintf(flog,"POL POINT not implemented at %ld\n", ifp_linecnt);
      display_error++;
      return -1;
   }
   else
   {
      fprintf(flog,"Unknown POINT var_1 [%s] found at %ld\n", tok, ifp_linecnt);
      display_error++;
      return -1;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static double get_deltaangle(double sa, double ea)
{
   double da;

   da = ea - sa;

   return da;
}

/****************************************************************************/
/*
*/
static int dmis_arc()
{
   char  tok[80];

   if (get_nextword(NULL,tok,80) == 0) return p_error("arc var_1 expected");
   // INNER, OUTER

   if (!STRCMPI(tok,"INNER"))
   {
      // here var 2 follows
      if (get_nextword(NULL,tok,80) == 0) return p_error("arc var_2 expected");
      // CART, POL
      if (!STRCMPI(tok,"CART"))
      {
         double   x,y,z,i,j,k, rad, ang1, delta;
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc x expected");
         x = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc y expected");
         y = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc z expected");
         z = cnv_tok(tok);

         if (get_nextword(NULL,tok,80) == 0) return p_error("arc i expected");
         i = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc j expected");
         j = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc k expected");
         k = cnv_tok(tok);

         if (get_nextword(NULL,tok,80) == 0) return p_error("arc rad expected");
         rad = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc ang1 expected");
         ang1 = cnv_angle(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc ang2 expected");
         delta = cnv_angle(tok);

         lastdata = Graph_Arc(cur_layernum, x, y, rad, ang1, delta, 0, 0, 0);
         dmis_do_snset(lastdata);
      }
      else
      if (!STRCMPI(tok,"POL"))
      {
         // r,a,h,i,j,k
         fprintf(flog,"POL ARC not implemented at %ld\n", ifp_linecnt);
         display_error++;
         return -1;
      }
      else
      {
         fprintf(flog,"Unknown ARC var_2 [%s] found at %ld\n", tok, ifp_linecnt);
         display_error++;
         return -1;
      }
   }
   else
   if (!STRCMPI(tok,"OUTER"))
   {
      // here var 2 follows
      if (get_nextword(NULL,tok,80) == 0) return p_error("arc var_2 expected");
      // CART, POL
      if (!STRCMPI(tok,"CART"))
      {
         double   x,y,z,i,j,k, rad, ang1, delta;
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc x expected");
         x = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc y expected");
         y = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc z expected");
         z = cnv_tok(tok);

         if (get_nextword(NULL,tok,80) == 0) return p_error("arc i expected");
         i = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc j expected");
         j = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc k expected");
         k = cnv_tok(tok);

         if (get_nextword(NULL,tok,80) == 0) return p_error("arc rad expected");
         rad = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc ang1 expected");
         ang1 = cnv_angle(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("arc ang2 expected");
         delta = cnv_angle(tok);

         lastdata = Graph_Arc(cur_layernum, x, y, rad, ang1, delta, 0, 0, 0);
         dmis_do_snset(lastdata);
      }
      else
      if (!STRCMPI(tok,"POL"))
      {
         // r,a,h,i,j,k
         fprintf(flog,"POL ARC not implemented at %ld\n", ifp_linecnt);
         display_error++;
         return -1;
      }
      else
      {
         fprintf(flog,"Unknown ARC var_2 [%s] found at %ld\n", tok, ifp_linecnt);
         display_error++;
         return -1;
      }
   }
   else
   {
      fprintf(flog,"Unknown ARC var_1 [%s] found at %ld\n", tok, ifp_linecnt);
      display_error++;
      return -1;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_line()
{
   char  tok[80];

   if (get_nextword(NULL,tok,80) == 0) return p_error("line var_1 expected");
   // BND, UNBND

   if (!STRCMPI(tok,"UNBND"))
   {
      // here var 2 follows
      if (get_nextword(NULL,tok,80) == 0) return p_error("line var_2 expected");
      // CART, POL
      if (!STRCMPI(tok,"CART"))
      {
         double   x,y,z,i,j,k;
         if (get_nextword(NULL,tok,80) == 0) return p_error("line x expected");
         x = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line y expected");
         y = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line z expected");
         z = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line i expected");
         i = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line j expected");
         j = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line k expected");
         k = cnv_tok(tok);
         lastdata = Graph_Line(cur_layernum,x, y, x+i, y+j, 0, 0, FALSE);  
         dmis_do_snset(lastdata);
      }
      else
      if (!STRCMPI(tok,"POL"))
      {
         // r,a,h,i,j,k
         fprintf(flog,"POL LINE not implemented at %ld\n", ifp_linecnt);
         display_error++;
         return -1;
      }
      else
      {
         fprintf(flog,"Unknown LINE var_2 [%s] found at %ld\n", tok, ifp_linecnt);
         display_error++;
         return -1;
      }
   }
   else
   if (!STRCMPI(tok,"BND"))
   {
      // here var 3 follows
      if (get_nextword(NULL,tok,80) == 0) return p_error("line var_2 expected");
      // CART, POL
      if (!STRCMPI(tok,"CART"))
      {
         double   e1x, e1y, e1z, e2x, e2y, e2z;
         if (get_nextword(NULL,tok,80) == 0) return p_error("line e1x expected");
         e1x = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line e1y expected");
         e1y = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line e1z expected");
         e1z = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line e2x expected");
         e2x = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line e2y expected");
         e2y = cnv_tok(tok);
         if (get_nextword(NULL,tok,80) == 0) return p_error("line e2z expected");
         e2z = cnv_tok(tok);
         lastdata = Graph_Line(cur_layernum, e1x, e1y, e2x, e2y, 0, 0, FALSE);   
         dmis_do_snset(lastdata);
      }
      else
      if (!STRCMPI(tok,"POL"))
      {
         // r1r, e1a, e1h, e2r, e2a, e2h
         fprintf(flog,"POL LINE not implemented at %ld\n", ifp_linecnt);
         display_error++;
         return -1;
      }
      else
      {
         fprintf(flog,"Unknown LINE var_3 [%s] found at %ld\n", tok, ifp_linecnt);
         display_error++;
         return -1;
      }
   }
   else
   {
      fprintf(flog,"Unknown LINE var_1 [%s] found at %ld\n", tok, ifp_linecnt);
      display_error++;
      return -1;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_feature()
{
   char     tok[80];
   CString  name;

   lastdata = NULL;
   // feature name
   if (get_nextword(NULL,tok,80) == 0) return p_error("feature expected");
   name = tok;
   // feat keyword
   if (get_nextword(NULL,tok,80) == 0) return p_error("feature expected");

   if (STRCMPI(tok,"FEAT"))   // if not
   {
      fprintf(flog,"FEA keyword expected -> found [%s] at %ld\n", tok, ifp_linecnt);
      display_error++;
      return -1;
   }

   // feature
   if (get_nextword(NULL,tok,80) == 0) return p_error("feature expected");

   if (!STRCMPI(tok,"LINE"))
   {
      dmis_line();
   }
   else
   if (!STRCMPI(tok,"CIRCLE"))
   {
      dmis_circle();
   }
   else
   if (!STRCMPI(tok,"ARC"))
   {
      dmis_arc();
   }
   else
   if (!STRCMPI(tok,"POINT"))
   {
      dmis_point();
   }
   else
   {
      fprintf(flog,"Unknown FEATURE [%s] found at %ld\n", tok, ifp_linecnt);
      display_error++;
      return -1;
   }

   if (lastdata)
   {
      if (strlen(name))
      {
         doc->SetUnknownAttrib(&lastdata->getAttributesRef(), ATT_NAME, name,
            SA_OVERWRITE, NULL); // x, y, rot, height
         Feature *a = new Feature;
         featurearray.SetAtGrow(featurecnt, a);
         featurecnt++;
         a->featurename = name;  
         a->d = lastdata;

      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_units()
{
   char  tok[80];

   if (get_nextword(NULL,tok,80) == 0) return p_error("units expected");

   // MM
   // CM
   // M
   // INCH
   // FEET
   if (!STRCMPI(tok,"MM"))
      scale_factor = Units_Factor(UNIT_MM, page_unit); 
   else
   if (!STRCMPI(tok,"CM"))
      scale_factor = Units_Factor(UNIT_MM, page_unit)*10; 
   else
   if (!STRCMPI(tok,"M"))
      scale_factor = Units_Factor(UNIT_MM, page_unit)*100; 
   else
   if (!STRCMPI(tok,"INCH"))
      scale_factor = Units_Factor(UNIT_INCHES, page_unit); 
   else
   {
      fprintf(flog, "Unknown Units [%s] at %ld\n", tok, ifp_linecnt);
      return p_error("unit error");
   }

   // ANGDEC
   // ANGRAD
   if (get_nextword(NULL,tok,80) == 0) return p_error("units expected");
   if (!STRCMPI(tok,"ANGDEC"))
      angle_unit = ANGDEC; 
   else
   if (!STRCMPI(tok,"ANGRAD"))
      angle_unit = ANGRAD; 
   else
   {
      fprintf(flog, "Unknown Angle Units [%s] at %ld\n", tok, ifp_linecnt);
      return p_error("unit error");
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_meas()
{
   char  *lp;
   char  tok[80];
   long  startline = ifp_linecnt;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"\n")) == NULL) continue;
      if (get_nextword(lp,tok,80) == 0)   continue;

      // logical end of measure.
      if (!STRCMPI(tok, "ENDMES"))  return 1;
   }

   fprintf( flog, "No ENDMES found for MEAS start at %ld\n", startline);
   display_error++;

   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_endmes()
{
   // do nothing
   return 1;
}

/****************************************************************************/
/*
*/
/*
static int  get_label(int lab)
{
   int   i;

   for (i=0;i<recordbufcnt;i++)
   {
      if (recordbufarray[i]->label == lab)
         return i+1; 
   }

   return 0;
}
*/

/****************************************************************************/
/*
*/
static int  get_feature(const char *f)
{
   int   i;

   for (i=0;i<featurecnt;i++)
   {
      if (featurearray[i]->featurename.CompareNoCase(f) == 0)
         return i;   
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int  get_tolerance(const char *f)
{
   int   i;

   for (i=0;i<tolerancebufcnt;i++)
   {
      if (tolerancebufarray[i]->name.CompareNoCase(f) == 0)
         return i;   
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int assign_feature_tol(const char *fname, const char *tname)
{
   // find feature
   int   fptr = get_feature(fname); 
   int   tptr = get_tolerance(tname); 

   if (fptr < 0)  
   {
      fprintf(flog, "Feature [%s] definition not found\n", fname);
      display_error++;
      return 0;
   }
   if (tptr < 0)  
   {
      fprintf(flog, "Tolerance [%s] definition not found\n", tname);
      display_error++;
      return 0;
   }

   char   tmp[MAX_LINE];
   char   tok[80];
   strcpy(tmp, tolerancebufarray[tptr]->inhalt);

   if (get_nextword(tmp,tok,80) == 0)  return p_error("get tolerance");

   if (!STRCMPI(tok,"TOL"))
   {
      if (get_nextword(NULL,tok,80) == 0) return p_error("get tolerance");
      if (!STRCMPI(tok,"DIAM"))
      {
         CString  low, upper;
         if (get_nextword(NULL,tok,80) == 0) return p_error("tol diam lower");
         low = tok;
         if (get_nextword(NULL,tok,80) == 0) return p_error("tol diam upper");
         upper = tok;

         doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_fLower, low,
            SA_OVERWRITE, NULL); // x, y, rot, height

         doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_fUpper, upper,
            SA_OVERWRITE, NULL); // x, y, rot, height
      }
      else
      if (!STRCMPI(tok,"ANG"))
      {
         CString  low, upper;
         if (get_nextword(NULL,tok,80) == 0) return p_error("tol diam lower");
         low = tok;
         if (get_nextword(NULL,tok,80) == 0) return p_error("tol diam upper");
         upper = tok;

         doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_fLower, low,
            SA_OVERWRITE, NULL); // x, y, rot, height

         doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_fUpper, upper,
            SA_OVERWRITE, NULL); // x, y, rot, height
      }
      else
      if (!STRCMPI(tok,"CORTOL"))
      {
         if (get_nextword(NULL,tok,80) == 0) return p_error("tol cortol");

         if (!STRCMPI(tok, "XAXIS"))
         {
            CString  low, upper;
            if (get_nextword(NULL,tok,80) == 0) return p_error("tol cortol xaxis lower");
            low = tok;
            if (get_nextword(NULL,tok,80) == 0) return p_error("tol cortol xaxis  upper");
            upper = tok;

            doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_xLower, low,
               SA_OVERWRITE, NULL); // x, y, rot, height

            doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_xUpper, upper,
               SA_OVERWRITE, NULL); // x, y, rot, height
         }
         else
         if (!STRCMPI(tok, "YAXIS"))
         {
            CString  low, upper;
            if (get_nextword(NULL,tok,80) == 0) return p_error("tol cortol yaxis lower");
            low = tok;
            if (get_nextword(NULL,tok,80) == 0) return p_error("tol cortol yaxis  upper");
            upper = tok;

            doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_yLower, low,
               SA_OVERWRITE, NULL); // x, y, rot, height

            doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_yUpper, upper,
               SA_OVERWRITE, NULL); // x, y, rot, height
         }
         else
         if (!STRCMPI(tok, "ZAXIS"))
         {
            CString  low, upper;
            if (get_nextword(NULL,tok,80) == 0) return p_error("tol cortol zaxis lower");
            low = tok;
            if (get_nextword(NULL,tok,80) == 0) return p_error("tol cortol zaxis  upper");
            upper = tok;

            doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_zLower, low,
               SA_OVERWRITE, NULL); // x, y, rot, height

            doc->SetUnknownAttrib(&featurearray[fptr]->d->getAttributesRef(),ATT_Tol_zUpper, upper,
               SA_OVERWRITE, NULL); // x, y, rot, height
         }
         else
         {
            fprintf(flog, "Unknown TOL [%s] -> [%s]\n", tolerancebufarray[tptr]->inhalt, tok);
            display_error++;
         }
      }
      else
      {
         fprintf(flog, "Unknown TOL [%s] -> [%s]\n", tolerancebufarray[tptr]->inhalt, tok);
         display_error++;
      }
   }
   else
   {
      fprintf(flog, "Unknown TOL [%s]\n", tolerancebufarray[tptr]->inhalt);
      display_error++;
   }

   return 1;
}

/****************************************************************************/
/*
   FA
   F
   TA
   T
   R
   SA
   
   Can have multiple features and multiple tolerances

*/
static int dmis_output()
{
   char  tok[80];
   int   i, ii;

   while (get_nextword(NULL,tok,80))
   {
      CString  w = tok;
      if (get_nextword(NULL,tok,80) == 0) return p_error("output name");

      Outputbuf *a = new Outputbuf;
      outputbufarray.SetAtGrow(outputbufcnt, a);
      outputbufcnt++;
      a->command = w;
      a->name = tok;
   }

   // for every feature assign all tolerances
   for (i=0;i<outputbufcnt;i++)
   {
      CString  fname;
      if (outputbufarray[i]->command.Left(1) == "F")
      {
         fname = outputbufarray[i]->name;
         for (ii=0;ii<outputbufcnt;ii++)
         {
            if (outputbufarray[ii]->command.Left(1) == "T")
            {
               assign_feature_tol(fname, outputbufarray[ii]->name);
            }
         }
      }
   }

   for (i=0;i<outputbufcnt;i++)
      delete outputbufarray[i];
   outputbufcnt = 0;

   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_construct()
{
   // do nothing
   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_datdef()
{
   // do nothing
   return 1;
}

/****************************************************************************/
/*
*/
static int dmis_snset()
{
   char  tok[80];

   if (get_nextword(NULL,tok,80) == 0) return p_error("snset var1");

   if (!STRCMPI(tok,"VL")) // video light
   {
      if (get_nextword(NULL,tok,80) == 0) return p_error("snset VL var1");
      if (!STRCMPI(tok,"Profile"))
      {
         if (get_nextword(NULL,tok,80) == 0) return p_error("snset Profile ");
         SNSET_PROFILE = round(atof(tok) * 255);
      }
      if (!STRCMPI(tok,"Surface"))
      {
         if (get_nextword(NULL,tok,80) == 0) return p_error("snset Surface");
         SNSET_SURFACE = round(atof(tok) * 255);
      }
      if (!STRCMPI(tok,"Ring"))
      {
         if (get_nextword(NULL,tok,80) == 0) return p_error("snset Ring");
         SNSET_RING = round(atof(tok) * 255);
      }
   }

   return 1;
}

/****************************************************************************/
/*
   T(label) = TOL ... <- use dmis_tol function
*/
static int dmis_tolerance()
{
   char     tok[80];
   CString  name, inhalt;

   if (get_nextword(NULL,tok,80) == 0) return p_error("tol");

   name = tok;
   inhalt = curlp;   // ) end of bracket from name = TOL / DIAM , ws,x ,zx etc...

   CString  res;
   res = inhalt.Right(strlen(inhalt) - inhalt.Find("=") - 1);

   Tolerancebuf *a = new Tolerancebuf;
   tolerancebufarray.SetAtGrow(tolerancebufcnt, a);
   tolerancebufcnt++;
   a->name = name;   
   a->inhalt = res;

   return 1;
}

/*Ende **********************************************************************/



