// $Header: /CAMCAD/5.0/read_wrt/GenCamIn.cpp 44    5/25/07 6:53p Lynn Phung $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.

   
   Primitives:
   CAMCAD does not support of Grouped Primitives. All CAMCAD Primitives such as 
   Apertures, Linewidth and Tools are global.
   I take the first group as global, for all other groups, the primitive will get
   the groupname prefix.
   CAMCAD does not support LineTypes

   Primitives are apertures unless they are marked with a linetype.
   
   Layers
   CAMCAD does not support of Grouped Layers. All Layers are global.
   I take the first group as global, for all other groups, the layers will get
   the groupname prefix.

   CompPin
   Problem, CAMCAD can not differ between compname dependend on which group it 
   came from.

*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include "gauge.h"
#include "attrib.h"
#include "lyrmanip.h"
#include "GenCamIn.h"
#include "compvalues.h"
#include "CCEtoODB.h"
#include "StandardAperture.h"
#include "PolyLib.h"
#include "PcbLayer.h"
#include "RwUiLib.h"
#include "MergeFiles.h"

#define LineStatusUpdateInterval 200

extern CProgressDlg *progress;

// Graph_Block_Exists
// Graph_Block_On

/****************************************************************************/
/*
   Global Commands
*/

static   int   go_command(List *lst,int siz_lst,int start_indent);
static   int   loop_command(List *lst,int siz_lst,int start_indent);
static   int   go_fnull(int start_ident);
static   int   gencam_attribute(CString *owner, CString *key, CString *val);
static   int   assign_layer();
static   BOOL  Is_Layer_Surface_Bottom(CString layername);

static   FileStruct *file, *global_file;
static   CCEtoODBDoc *doc;

static   FILE     *ferr;
static   int      display_error = 0;

static   int      push_tok;

typedef CArray<Point2, Point2&> CPolyArray;

static   CPolyArray  polyarray;
static   int      polycnt;
static   int      viacnt;

static   FILE     *ifp;
static   char     ifp_line[MAX_LINE];
static   long     ifp_linecnt = 0;
static CFileReadProgress*  fileReadProgress = NULL;

static   char     buf[MAX_LINE]; // tmp line

static   CString  cur_linedesc;
static	bool		cur_fill;
static   int      cur_layer;
static   int      cur_entity;
static   int      cur_lineident;
static   int      cur_linepos;
static   char     cur_tok[MAX_LINE];
static   char     cur_line[MAX_LINE];
static   GENCAMComp  cur_comp;

static   CPaintArray paintArray;


static   CLLayerArray   layerArray;


static   double   scale_factor = 1;
static   int      cur_filenum = 0;
static   int      cur_polyopen;

static   TypeStruct  *cur_type;
static   DataStruct  *cur_data;
static   int      cur_stat;
static   int      routcont;  
static   double   oldx, oldy;
static   int      oldwidthindex;
static   int      oldlay;
static   CString  oldnetname;

static   int      PageUnits;
static   int      AngleUnits;          // 0 = radians, 1 = degree
static   CString  cur_netname;
static   char     last_delimeter;
static   int      header_gencam_found; // make sure that this is a gencam file

//_____________________________________________________________________________
CGenCamReader* s_genCamReader = NULL;

CGenCamReader& getGenCamReader()
{
   return *s_genCamReader;
}

/******************************************************************************
* ReadGenCAM 
*/
void ReadGenCAM(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   delete s_genCamReader;
   s_genCamReader = new CGenCamReader(*Doc);

   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   doc = Doc;

   CString gencamLogFile;
   ferr = getApp().OpenOperationLogFile(GENCAMERR, gencamLogFile);
   if (ferr == NULL) // error message already issued, just return.
      return;

   display_error = 0;

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(f, fname);
   strcat(f, ext);

   PageUnits = pageunits;
   header_gencam_found = FALSE;
   scale_factor = Units_Factor(UNIT_INCHES, PageUnits);  // all units are normalized to INCHES

   polyarray.SetSize(100, 100);
   polycnt = 0;
   viacnt = 0;

   paintArray.SetSize(0, 10);
     

   layerArray.SetSize(0, 10);
     

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(path_buffer,"rt")) == NULL)
   {
      ErrorMessage("Error open file", path_buffer);
      return;
   }

   delete fileReadProgress;
   fileReadProgress = new CFileReadProgress(ifp);

   global_file = file = Graph_File_Start(f, Type_GENCAM);
   file->setShow(false);

   // init globals
   ifp_linecnt = 0;
   push_tok = FALSE;
   AngleUnits = 1;
   cur_data = NULL;
   cur_lineident = 0;
   cur_polyopen = FALSE;
	cur_fill = false;
   cur_filenum = file->getFileNumber();
   cur_layer = Graph_Level("0", "", 1);
   cur_netname = "";
   cur_stat = STAT_NONE;
   cur_linedesc = "";

   // here do a while loop
   while (TRUE)
   {
      int res = loop_command(start_lst, SIZ_START_LST, -1);

      if (res == -3) // not a GenCAM file.
         break;

      if (res == -2) // logical found
         break;

      if (res == -1) // EOF found
      {
         fprintf(ferr, "GENCAM: Premature End of File found at %ld\n", ifp_linecnt);
         display_error++;

         break;
      }
   }

   assign_layer();
	CLayerSettings layerSettings(s_genCamReader->getSettingsFilename());
	layerSettings.Apply(doc);
   doc->RemoveUnusedLayers();

   delete fileReadProgress;
   fileReadProgress = NULL;

   fclose(ifp);

	//doc->OnGeneratePinloc();
   COperationProgress operationProgress;
   operationProgress.updateStatus("Generating pin locations");
   doc->generatePinLocations(true,&operationProgress);

   fprintf(ferr, "\nGenCAM syntax checker available: http://www.gencam.org/ctmupload2.html\n\n\n");
   
   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!gencamLogFile.IsEmpty())
      fclose(ferr);
    
   double accuracy = get_accuracy(doc);

//   for (int i=0; i<doc->getMaxBlockIndex(); i++)
//   {
//      BlockStruct *block = doc->getBlockAt(i);
//
//      if (!block)
//         continue;
//
////      OptimizeComplexApertures_Blocks(doc, block, -1, accuracy, FALSE);
//   }

   polyarray.RemoveAll();


   paintArray.empty();


   layerArray.empty();

   if (display_error && !gencamLogFile.IsEmpty())
      Logreader(gencamLogFile);

   delete s_genCamReader;
   s_genCamReader = NULL;

   return;
}

/******************************************************************************
* int set_group_to_file
*/
static int  set_group_to_file(const char *groupname)
{
   BlockStruct *block = doc->Find_Block_by_Name(groupname, -1);

   if (block == NULL) // does not exist , make a new one.
   {
      file = Graph_File_Start(groupname, Type_GENCAM);
      file->setShow(false);
   }
   else
   {
      CString tmp;
      tmp.Format("Groupname [%s] already defined.\n", groupname);
      ErrorMessage(tmp);
   }

   doc->PrepareAddEntity(file);        

   return 1;
}

/******************************************************************************
* fskip
* a complete record always end with a ;
* only keywords do not, but they start with a $
*/
static int fskip()
{
   return 1;
}

/******************************************************************************
* fnull
*/
static int fnull()
{
   int start_ident = cur_lineident;
   go_fnull(start_ident);

   return 1;
}

/******************************************************************************
* p_error
*/
static int p_error()
{
   fprintf(ferr, "GENCAM: Error at %ld -> token expected.\n", ifp_linecnt);
   display_error++;

   return -1;
}

/******************************************************************************
* cnv_tok
*/
static double cnv_tok(char *l)
{
    double x;

    if (l && strlen(l))
        x = atof(l);
    else
        return 0.0;

    return x * scale_factor;
}

/******************************************************************************
* cnv_rot
*/
static double cnv_rot(char *l)
{
   double x;

   if (l && strlen(l))
      x = atof(l);
   else
      return 0.0;

   if (AngleUnits == 1) // degree
      x = DegToRad(x);

   return x;
}

/******************************************************************************
* get_groupnum
*/
static int get_groupnum(const char *g)
{
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *tmpfile = doc->getFileList().GetNext(pos);

      if (tmpfile->getName() == g)
         return tmpfile->getFileNumber();
   } 

   return -1;
}

/******************************************************************************
* char *gettok
*/
static char *gettok(char *l, char *del)
{
   int cur_tokcnt = 0;
   int texton = 0;

   cur_tok[0] = '\0';

   if (l)
   {
      strcpy(cur_line, l);
      cur_linepos = 0;
   }

   while (cur_linepos < (int)strlen(cur_line) && cur_line[cur_linepos])
   {
      if (cur_line[cur_linepos] == '"')
         texton++;

      if (texton != 1)  // do not check for a delimeter inside a text string.
      {
         if (strchr(del,cur_line[cur_linepos]))
            break;
      }

      cur_tok[cur_tokcnt++] = cur_line[cur_linepos++];
   }

   last_delimeter = cur_line[cur_linepos];
   cur_linepos++; 
   cur_tok[cur_tokcnt] = '\0';
   CString w = cur_tok;
   w.TrimLeft();
   w.TrimRight();

   if (strlen(w) == 0)  
      return NULL;

   strcpy(cur_tok,w);

   // kill text
   if (cur_tok[0] == '"')
   {
      STRREV(cur_tok);
      cur_tok[strlen(cur_tok)-1] = '\0';
      STRREV(cur_tok);
      cur_tok[strlen(cur_tok)-1] = '\0';
   }

   return cur_tok;
}

/******************************************************************************
* gettok_koo
*/
static int gettok_koo(char *l, char *del, double *x, double *y)
{
   int koocnt = 0;
   char *lp = gettok(l, "(,");   // advance to bracket open. may return NULL. It also may be a del, if the field is optional

   if (lp == NULL)
   {
      if (last_delimeter == ',' || last_delimeter == ';')
         return koocnt;
   }

   if ((lp = gettok(l,",")) == NULL)   //get x
      return p_error();

   *x = cnv_tok(lp);
   koocnt++;

   if ((lp = gettok(l,")")) == NULL)   // get_y
      return p_error();

   *y = cnv_tok(lp);
   koocnt++;
   gettok(l, ",");   // advance to bracket open

   return koocnt;
}

/******************************************************************************
* gettok_location
*/
static int gettok_location(char *l, char *del, double *x, double *y, double *rot, int *mir)
{
   char *lp;
   int res;

   *rot = 0;
   *mir = 0;
   *x   = 0;
   *y   = 0;

   if (last_delimeter == ';')
      return 0;      // record was over

   res = gettok_koo(NULL, ",;", x, y);

   if (res == 0)  // this may be optional
      return 0;

   if (res != 2)
      return p_error();

   if ((lp = gettok(l,",;")) != NULL)  // optional
      *rot = cnv_rot(lp);

   if ((lp = gettok(l,",;")) != NULL)  // optional
   {
      if (!STRCMPI(lp,"MIRROR"))
         *mir = TRUE;
   }

   return 1;
}

/******************************************************************************
* gettok_xform
*/
static int gettok_xform(char *l, char *del, double *x, double *y, double *rot, int *mir, double *scale)
{
   char *lp;

   *rot = 0;
   *mir = 0;
   *scale = 1;

   if (gettok_koo(NULL,",;",x, y) != 2) 
      return p_error();

   if ((lp = gettok(l,",;")) != NULL)  // optional
      *rot = cnv_rot(lp);

   if ((lp = gettok(l,",;")) != NULL)  // optional
   {
      if (!STRCMPI(lp,"MIRROR"))
         *mir = TRUE;
   }

   if ((lp = gettok(l,",;")) != NULL)  // optional
      *scale = atof(lp);

   return 1;
}

/******************************************************************************
* gettok_qualstring
* this is either "xx"."xx" or empty
*/
static int gettok_qualstring(char *l, char *del, CString *s1, CString *s2)
{
   char *lp;
   int koocnt = 0;

   if ((lp = gettok(l,".,;")) == NULL) // get first
      return koocnt;

   *s1 = lp;
   koocnt++;

   if ((lp = gettok(l,del)) == NULL)   // get second  to , or ;
      return p_error();

   *s2 = lp;
   koocnt++;

   return koocnt;
}

/******************************************************************************
* write_poly
*/
static int write_poly(int layer, const char *linedesc, int closed, bool fill)
{
   //int fill = 0;
   int widthindex = 0;
   int err;
   
   if (polycnt == 0)
      return 0;

   if (strlen(linedesc))
      widthindex = Graph_Aperture(linedesc, T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   else
      widthindex = 0;

   if (polycnt)
   {
      Graph_Poly(NULL,widthindex, fill, 0, closed);

      for (int i=0; i<polycnt; i++)
      {
         Point2   p2 = polyarray.ElementAt(i);
         Graph_Vertex(p2.x, p2.y, p2.bulge);
      }
   }

   polycnt = 0;

   return NULL;
}

/******************************************************************************
* get_nextcommand
* gets a next command until a ; or starting with $
* lines starting # are remarks until eol
*/
static int get_nextcommand(char *string, unsigned int n, FILE *fp)
{
   char tmp[MAX_LINE];
   CString ws = "";

   string[0] = '\0';

   while (TRUE)
   {
      if (!fgets(tmp, n, fp))
         return 0;

      ifp_linecnt++;

      if ((ifp_linecnt % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
      {
         fileReadProgress->updateProgress();
      }

      if (strlen(string) == 0)   // first token - do not trimleft
      {
         ws = tmp;
         ws.TrimRight();
         strcpy(tmp, ws);
      }
      else
      {
         ws = tmp;
         ws.TrimLeft();
         ws.TrimRight();
         strcpy(tmp,ws);
      }

      if (strlen(tmp) && tmp[strlen(tmp)-1] == '\n')
         tmp[strlen(tmp)-1] = '\0';

      if (strlen(tmp) == 0)
         continue;

      if ((strlen(string) + strlen(tmp)) < n)
      {
         strcat(string,tmp);
      }
      else
      {
         CString  t;
         t.Format("Bad GenCAM Syntax: Line Length error at %ld\n", ifp_linecnt);
         ErrorMessage(t);

         return 0;
      }

      if (tmp[0] == '#')   // remark
      {
         tmp[0] = '\0';
         string[0] = '\0';
      }
      else if (tmp[0] == '$') 
         break;
      else if (tmp[strlen(tmp)-1] == ';') 
         break;
   }

   ws = string[0];

   for (int i=1; i<(int)strlen(string); i++)
   {
      if (string[i-1] == ',' && string[i] == ',')
         ws += " ";

      ws += string[i];
   }

   if (strlen(ws) < MAX_LINE)
      strcpy(string, ws);
   else
   {
      CString t;
      t.Format("Bad GenCAM Syntax: Line Length error at %ld\n", ifp_linecnt);
      ErrorMessage(t);

      return 0;
   }

   // here calculate the current ident of this command. GENCAM is ident dependend.
   int ci = 0;

   while (ci < (int)strlen(string) && isspace(string[ci]))
      ci++;

   cur_lineident = ci;

   return TRUE;
}

/******************************************************************************
* clean_prosa
*/
static void clean_prosa(char *p)
{
   if (strlen(p) == 0)
      return;

   // delete all traling isspace
   while (strlen(p) && isspace(p[strlen(p)-1])) 
         p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')
      p[strlen(p)-1] = '\0';

   STRREV(p);

   while (strlen(p) && isspace(p[strlen(p)-1])) 
      p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')
      p[strlen(p)-1] = '\0';

   STRREV(p);

   return;
}

/******************************************************************************
* int polygonlist_startat
*/
int polygonlist_startat()
{
   double x, y;

   if (polycnt != 0)
   {
      CString tmp;
      tmp.Format("STARTTO command found at %ld and polygon buffer was not empty", ifp_linecnt);
      ErrorMessage(tmp);
   }

   polycnt = 0;

   if (gettok_koo(NULL, ",;", &x, &y) != 2) 
      return p_error();

   Point2 p;
   p.x = x;
   p.y = y;
   p.bulge = 0;
   polyarray.SetAtGrow(polycnt, p); // do it double because this point is used
   polycnt++;

   return 1;
}

/******************************************************************************
* int polygonlist_lineto
*/
int polygonlist_lineto()
{
   double x, y;

   if (gettok_koo(NULL, ",;", &x, &y) != 2) 
      return p_error();

   Point2 p;
   p.x = x;
   p.y = y;
   p.bulge = 0;
   polyarray.SetAtGrow(polycnt,p);  // do it double because this point is used
   polycnt++;

   return 1;
}

/******************************************************************************
* int polygonlist_circarcto
*/
int polygonlist_circarcto()
{
   double xend, yend;
   double xcenter, ycenter;
   int cckw = TRUE;  // counter clock wise
   char *lp;

   if (gettok_koo(NULL, ",;", &xend, &yend) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;", &xcenter, &ycenter) != 2) 
      return p_error();

   if ((lp = gettok(NULL, ",;")) != NULL) 
   {
      if (!STRCMPI(lp,"CLKW"))
         cckw = FALSE;
   }

   Point2   p;
   p = polyarray.GetAt(polycnt-1);  

   double r, sa, da;
   int cw = FALSE;

   if (!cckw)
      cw = TRUE;

   ArcCenter2(p.x, p.y, xend, yend, xcenter, ycenter, &r, &sa, &da, cw);

   if (fabs(p.x - xend) < SMALLNUMBER && fabs(p.y - yend) < SMALLNUMBER)
   {
      // this is a Circle
      p.x = xcenter - r;
      p.y = ycenter;
      p.bulge = 1;
      polyarray.SetAt(polycnt-1, p);  

      p.x = xcenter + r;
      p.y = ycenter;
      p.bulge = 1;
      polyarray.SetAtGrow(polycnt, p); // do it double because this point is used
      polycnt++;

      p.x = xcenter - r;
      p.y = ycenter;
      p.bulge = 0;
      polyarray.SetAtGrow(polycnt,p);  // do it double because this point is used
      polycnt++;
   }
   else
   {
      double bulge = tan(da/4);

      p.bulge = bulge;
      polyarray.SetAt(polycnt-1, p);  

      p.x = xend;
      p.y = yend;
      p.bulge = 0;
      polyarray.SetAtGrow(polycnt, p); // do it double because this point is used
      polycnt++;
   }

   return 1;
}

/******************************************************************************
* int polygonlist_endcircarc
*/
int polygonlist_endcircarc()
{
   double xend, yend;
   double xcenter, ycenter;
   int cckw = TRUE;  // counter clock wise
   char *lp;
   Point2 p;

   p = polyarray.GetAt(0);  
   xend = p.x;
   yend = p.y; 
   p = polyarray.GetAt(polycnt-1);     // last arc koo

   if (gettok_koo(NULL, ",;", &xcenter, &ycenter) != 2) 
      return p_error();

   if ((lp = gettok(NULL, ",;")) != NULL) 
   {
      if (!STRCMPI(lp, "CLKW"))
         cckw = FALSE;
   }

   double r, sa, da;
   int cw = FALSE;

   if (!cckw)
      cw = TRUE;

   ArcCenter2(p.x, p.y, xend, yend, xcenter, ycenter, &r, &sa, &da, cw);

   if (fabs(p.x - xend) < SMALLNUMBER && fabs(p.y - yend) < SMALLNUMBER)
   {
      // this is a Circle
      p.bulge = 1;
      polyarray.SetAt(polycnt-1, p);  

      p.x = xend + r*2;
      p.y = yend;
      p.bulge = 1;
      polyarray.SetAtGrow(polycnt,p);  // do it double because this point is used
      polycnt++;

      p.x = xend;
      p.y = yend;
      p.bulge = 0;
      polyarray.SetAtGrow(polycnt,p);  // do it double because this point is used
      polycnt++;
      
   }
   else
   {
      double bulge = tan(da/4);

      p.bulge = bulge;
      polyarray.SetAt(polycnt-1, p);  

      p.x = xend;
      p.y = yend;
      p.bulge = 0;
      polyarray.SetAtGrow(polycnt, p); // do it double because this point is used
      polycnt++;
   }

   write_poly(cur_layer, "", FALSE, cur_fill);

   cur_polyopen = FALSE;

   return 1;
}

/******************************************************************************
* int polygonlist_endline
*/
int polygonlist_endline()
{
   int widthindex = 0;
   int fill = 0;
   Point2   p;

   if (polycnt)
   {
      Point2 p1 = polyarray.ElementAt(0);
      Point2 p2 = polyarray.ElementAt(polycnt-1);

      if (fabs(p1.x - p2.x) > SMALLNUMBER || fabs(p1.y - p2.y) > SMALLNUMBER)
      {
         p.x = p1.x;
         p.y = p1.y;
         p.bulge = 0;
         polyarray.SetAtGrow(polycnt, p); // do it double because this point is used
         polycnt++;
      }
   }

   write_poly(cur_layer, "", FALSE, cur_fill);
   cur_polyopen = FALSE;

   return 1;
}

/******************************************************************************
* int polygonlist_line
*/
int polygonlist_line()
{
   double x1, y1, x2, y2;

   if (gettok_koo(NULL, ",;", &x1, &y1) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;", &x2, &y2) != 2) 
      return p_error();

   Point2 p;
   p.x = x1;
   p.y = y1;
   p.bulge = 0;
   polyarray.SetAtGrow(polycnt, p); // do it double because this point is used
   polycnt++;

   p.x = x2;
   p.y = y2;
   p.bulge = 0;
   polyarray.SetAtGrow(polycnt, p); // do it double because this point is used
   polycnt++;

   return 1;
}

/******************************************************************************
* int polygonlist_circarc
*/
int polygonlist_circarc()
{
   double xstart, ystart;
   double xend, yend;
   double xcenter, ycenter;
   int cckw = TRUE;  // counter clock wise
   char *lp;

   if (gettok_koo(NULL, ",;", &xstart, &ystart) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;", &xend, &yend) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;", &xcenter, &ycenter) != 2) 
      return p_error();

   if ((lp = gettok(NULL, ",;")) != NULL) 
   {
      if (!STRCMPI(lp, "CLKW"))
         cckw = FALSE;
   }

   double r, sa, da;
   int cw = FALSE;

   if (!cckw)
      cw = TRUE;

   ArcCenter2(xstart, ystart, xend, yend, xcenter, ycenter, &r, &sa, &da, cw);

   double bulge = tan(da/4);

   Point2 p;
   p.x = xstart;
   p.y = ystart;
   p.bulge = bulge;
   polyarray.SetAtGrow(polycnt,p);  // do it double because this point is used
   polycnt++;

   p.x = xend;
   p.y = yend;
   p.bulge = 0;
   polyarray.SetAtGrow(polycnt,p);  // do it double because this point is used
   polycnt++;

   return 1;
}

/******************************************************************************
* int polygonlist_ellip
*/
int polygonlist_ellip()
{
   fprintf(ferr, "GENCAM: polygonlist_elliparc not implemented at %ld\n", ifp_linecnt);
   display_error++;

   return 1;
}

/******************************************************************************
* loop_to_end
* loop always ends at a $end command or at an equal or smaller ident
*/
static int loop_to_end(int start_ident)
{
   char *lp;

   while (get_nextcommand(ifp_line, MAX_LINE,ifp))
   {
      if ((lp = gettok(ifp_line, " \t\n;")) == NULL)
         continue;

      if (!STRNICMP(lp, "$END", 4))
         return 1;

      if (cur_lineident <= start_ident)
         return 2;
   }

   return -1;
}

/******************************************************************************
* Find_Layer
*/
static BOOL Is_Layer_Surface_Bottom(CString layername)
{
   for (int i=0; i < layerArray.GetSize(); i++)
   {
      GENCAMLayer *layer = layerArray.GetAt(i);

      if (!layer->getName().CompareNoCase(layername))
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
            return TRUE;
         else
            return FALSE;
      }
   }

   return FALSE;
}

/******************************************************************************
* do_mirror_layers
* Here make a simulated layer swap on top and bottom of same types.
* Genrad does not produce the layerswap record.
*/
static int do_mirror_layers()
{
   int lcnt = sizeof(gencam_layertype) / sizeof(char *);

   for (int l=0; l<lcnt; l++)
   {
      int tcnt = 0;
      int bcnt = 0;  
      CString top = "";
      CString bottom = "";

      for (int i=0; i < layerArray.GetSize(); i++)
      {
#ifdef _DEBUG
   GENCAMLayer *ll = layerArray.GetAt(i);
#endif
         if (layerArray.GetAt(i)->gencamtyp.CompareNoCase(gencam_layertype[l]) == 0)
         {
            if (!layerArray.GetAt(i)->surface.CompareNoCase("TOP"))
            {
               tcnt++;
               top = layerArray.GetAt(i)->name;
            }
            else if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
            {
               bcnt++;
               bottom = layerArray.GetAt(i)->name;
            }
            
         }
      }

      if (tcnt == 1 && bcnt == 1)
         Graph_Level_Mirror(top, bottom, "");
   }

   return 1;
}

/******************************************************************************
* assign_layer
*/
static int assign_layer()
{
   int top = 0;
   int bottom = 0;
   int electricalcnt = 0;     // count TOP, BOTTOM, INTERNAL conductor

	int i=0;
   for (i=0; i < layerArray.GetSize(); i++)
   {
      if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("CONDUCTOR") == 0)
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("TOP"))
            top++;
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
            bottom++;
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("INTERNAL") ||
				!layerArray.GetAt(i)->surface.CompareNoCase("ALL"))
            electricalcnt++;
      }
   }

   int bottomlayer = electricalcnt + 2; // plus top and bottom
   electricalcnt = 1;

   for (i=0; i < layerArray.GetSize(); i++)
   {
      LayerStruct *l = doc->FindLayer_by_Name(layerArray.GetAt(i)->name);

		if (l == NULL)
			continue;

      if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("HOLEFILL") == 0)
      {
         l->setLayerType(LAYTYPE_DRILL);
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("COMPONENT") == 0)
      {
         // not evaluated. 
         // in GenCAM component placement layers are on a layer, in CAMCAD it is on top/bottom.
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("LEGEND") == 0)
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("TOP"))
            l->setLayerType(LAYTYPE_SILK_TOP);
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
            l->setLayerType(LAYTYPE_SILK_BOTTOM);
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("CONDUCTOR") == 0)
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("TOP"))
         {
            l->setElectricalStackNumber(1);
            l->setLayerType(LAYTYPE_SIGNAL_TOP);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
         {
            l->setElectricalStackNumber(bottomlayer);
            l->setLayerType(LAYTYPE_SIGNAL_BOT);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("INTERNAL"))
         {
            l->setElectricalStackNumber(++electricalcnt);
            l->setLayerType(LAYTYPE_SIGNAL_INNER);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("ALL"))
         {
				l->setElectricalStackNumber(++electricalcnt);
            l->setLayerType(LAYTYPE_SIGNAL_ALL);
         }
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("PIN") == 0)
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("TOP"))
         {
            l->setElectricalStackNumber(1);
            l->setLayerType(LAYTYPE_PAD_TOP);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
         {
            l->setElectricalStackNumber(bottomlayer);
            l->setLayerType(LAYTYPE_PAD_BOTTOM);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("INTERNAL"))
         {
            l->setLayerType(LAYTYPE_PAD_INNER);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("ALL"))
         {
            l->setLayerType(LAYTYPE_PAD_ALL);
         }
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("SOLDERMASK") == 0)
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("TOP"))
         {
            l->setElectricalStackNumber(1);
            l->setLayerType(LAYTYPE_MASK_TOP);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
         {
            l->setElectricalStackNumber(bottomlayer);
            l->setLayerType(LAYTYPE_MASK_BOTTOM);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("ALL"))
         {
            l->setLayerType(LAYTYPE_MASK_ALL);
         }
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("PASTEMASK") == 0)
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("TOP"))
         {
            l->setElectricalStackNumber(1);
            l->setLayerType(LAYTYPE_PASTE_TOP);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("BOTTOM"))
         {
            l->setElectricalStackNumber(bottomlayer);
            l->setLayerType(LAYTYPE_PASTE_BOTTOM);
         }
         else if (!layerArray.GetAt(i)->surface.CompareNoCase("ALL"))
         {
            l->setLayerType(LAYTYPE_PASTE_ALL);
         }
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("BOARDOUTLINE") == 0)
      {
         if (!layerArray.GetAt(i)->surface.CompareNoCase("ALL"))
         {
            l->setLayerType(LAYTYPE_BOARD_OUTLINE);
         }
      }
      else if (layerArray.GetAt(i)->gencamtyp.CompareNoCase("GRAPHIC") == 0)
      {
         // just generic.
      }
      else
      {
#ifdef _DEBUG
         fprintf(ferr, "GENCAM: Unevaluated LayerTypes [%s] [%s] [%s]\n", layerArray.GetAt(i)->name, 
               layerArray.GetAt(i)->gencamtyp, layerArray.GetAt(i)->surface);
         display_error++;
#endif
      }
   }
   
   do_mirror_layers();

   return 1;
}

/******************************************************************************
* clean_padprefix
*/
static int clean_padprefix()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFlags() & BL_APERTURE)
      {
         // here now check if a block with the original name exist. If not - change the real name and
         // kill the original name
         //if (Graph_Block_Exists(doc, block->getOriginalName(), -1) == NULL)
         if (getGenCamReader().getBlock(block->getOriginalName(),-1) == NULL)
         {
            block->setName(block->getOriginalName());
            block->setOriginalName("");
         }
      }
   }

   return 1;
}

/******************************************************************************
* tok_search
   Search for the token in a command token list.
*/
static int tok_search(char *lp, List *tok_lst, int tok_size)
{
   for (int i=0; i<tok_size; ++i)
   {
      // this must look for the complete token, because
      // PAD and PADSTACK is a difference
      if (!STRICMP(lp, tok_lst[i].token))  
         return(i);
   }

   return(-1);
}

/******************************************************************************
* loop_command
*/
static int loop_command(List *lst, int siz_lst, int start_ident)
{
   int res;

   while (TRUE)
   {
      res = go_command(lst, siz_lst, start_ident); // this is -1 because go_command returns on equal

      if (res == -3) // not a GenCAM file 
         return res;

      if (res == -2) // logical end found
         return res;

      if (res == -1) // EOF found
         return res;

      if (res == 1)  // $end found
         return res;

      if (res == 2)  // smaller ident found
         return res;
   }

   return res;
}

/******************************************************************************
* go_command
*  return -2 for logical end
*  return -1 for EOF
*  return 1  for ident match
*  return 0  for ???
*/
static int go_command(List *lst, int siz_lst, int start_ident)
{
   char *lp;
   int res = 0;

   while (TRUE)
   {
      if (!push_tok)
      {
         if (get_nextcommand(ifp_line, MAX_LINE, ifp) < 1)   
            return -1;
      }
      else
      {
         strcpy(buf, ifp_line);
         push_tok = FALSE;
      }

      strcpy(buf, ifp_line);

      if ((lp = gettok(ifp_line, ":")) == NULL) 
      {
         ErrorMessage("Bad line\n");
         continue;
      }

      if (!strcmp(lp, "$$"))
         return -2;

      if (!STRNICMP(lp, "$END", 4))
         return 1;

      if (cur_lineident <= start_ident)
      {
         push_tok = TRUE;
         return 2;
      }

      push_tok = FALSE;
      int i = tok_search(lp, lst, siz_lst);

      if (i >= 0)
      {
         res = (*lst[i].function)();

         if (res < -1)
            return res;
      }
      else
      {
         fprintf(ferr, "GENCAM: Unknown Command [%s] skipped at %ld\n", lp, ifp_linecnt);
         display_error++;
      }
   }

   return res;
}

/******************************************************************************
* header_gencam
*/
static int header_gencam()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   double ver = atof(lp);

   if (ver != 1.5)   // right now the same, but this will change
   {
      CString tmp;
      tmp.Format("Supported File Version [1.5] - detected Version [%1.1lf] at %ld", ver, ifp_linecnt);
      ErrorMessage(tmp, "Untested GENCAM File Version - Proceed with caution !", MB_OK | MB_ICONHAND);
   }
   else
   {
      header_gencam_found = TRUE;
   }

   return 1;
}

/******************************************************************************
* boards_attribute
*/
static int boards_attribute()
{
   CString attrib_owner, attrib_key, attrib_val;

   if (gencam_attribute(&attrib_owner, &attrib_key, &attrib_val) != 1)
   {
      fprintf (ferr, "GENCAM: Error getting Attribute at %ld\n", ifp_linecnt);
      display_error++;

      return 0;
   }

   if (cur_stat == STAT_BOARD)
   {
      int start_ident = cur_lineident;
      loop_command(board_lst, SIZ_BOARD_LST, start_ident);
   }
   else
   {
      fnull();
      push_tok = TRUE;
   }

   return 1;
}

/******************************************************************************
* boards_assembly
*/
static int boards_assembly()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   cur_stat = STAT_ASSEMBLY;
   fnull();
   push_tok = TRUE;

   return 1;
}

/******************************************************************************
* boards_board
*/
static int boards_board()
{
   char *lp;
   int start_ident = cur_lineident;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   file = Graph_File_Start(lp, Type_GENCAM);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());
   file->setShow(true);

   doc->PrepareAddEntity(file);        
   cur_stat = STAT_BOARD;
   loop_command(board_lst, SIZ_BOARD_LST, start_ident);

   return 1;
}

/******************************************************************************
* panels_panel
*/
static int panels_panel()
{
   char *lp;
   int start_ident = cur_lineident;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   file = Graph_File_Start(lp, Type_GENCAM);
   file->setBlockType(blockTypePanel);
   file->getBlock()->setBlockType(file->getBlockType());
   file->setShow(false);

   doc->PrepareAddEntity(file);        
   cur_stat = STAT_PANEL;
   loop_command(panel_lst, SIZ_PANEL_LST, start_ident);
   cur_stat = STAT_NONE;

   return 1;
}

/******************************************************************************
* gcam_outline
*/
static int gcam_outline()
{
   int res;
   CString s1, s2;
   char *lp;

   int oldstat = cur_stat;

   if ((lp = gettok(NULL, ",;")) == NULL) // instance name
      return p_error();

   CString name = lp;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0) 
      return p_error();

   cur_layer = Graph_Level(s2, "", 0);
/*
   DataStruct *d = Graph_PolyStruct(cur_layer, 0, FALSE);

   if (strlen(name))
      doc->SetAttrib(&d->getAttributesRef(), doc->IsKeyWord(ATT_NAME, TRUE), VT_STRING, name.GetBuffer(0), SA_OVERWRITE, NULL);
*/
   int start_ident = cur_lineident;

   if (cur_stat == STAT_PANEL)
      cur_stat = STAT_PANELOUTLINE;
   else if (cur_stat == STAT_BOARD)
      cur_stat = STAT_BOARDOUTLINE;

   go_command(closed_shape_lst, SIZ_CLOSED_SHAPE_LST, start_ident);
   cur_stat = oldstat;

   return 1;
}
/******************************************************************************
* gcam_cutout
*/
static int gcam_cutout()
{
   int res;
   CString s1, s2;
   char *lp;

   int oldstat = cur_stat;

   if ((lp = gettok(NULL, ",;")) == NULL) // instance name
      return p_error();

   CString name = lp;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0) 
      return p_error();

   cur_layer = Graph_Level(s2, "", 0);
/*
   DataStruct *d = Graph_PolyStruct(cur_layer, 0, FALSE);

   if (strlen(name))
      doc->SetAttrib(&d->getAttributesRef(), doc->IsKeyWord(ATT_NAME, TRUE), VT_STRING, name.GetBuffer(0), SA_OVERWRITE, NULL);
*/
   int start_ident = cur_lineident;

   if (cur_stat == STAT_PANEL)
      cur_stat = STAT_PANELCUTOUT;
   else if (cur_stat == STAT_BOARD)
      cur_stat = STAT_BOARDCUTOUT;

   go_command(closed_shape_lst, SIZ_CLOSED_SHAPE_LST, start_ident);
   cur_stat = oldstat;

   return 1;
}


/******************************************************************************
* gcam_keepout
*/
static int gcam_keepout()
{
   int res;
   CString s1, s2;
   char *lp;
   int oldstat = cur_stat;

   if ((lp = gettok(NULL, ",;")) == NULL) // instance name
      return p_error();

   CString name = lp;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0) 
      return p_error();

   cur_layer = Graph_Level(s2, "", 0);
   DataStruct *d = Graph_PolyStruct(cur_layer, 0, FALSE);

   if (strlen(name))
      doc->SetAttrib(&d->getAttributesRef(), doc->IsKeyWord(ATT_NAME, TRUE), VT_STRING, name.GetBuffer(0), SA_OVERWRITE, NULL);

   int start_ident = cur_lineident;
   go_command(closed_shape_lst, SIZ_CLOSED_SHAPE_LST, start_ident);
   cur_stat = oldstat;

   return 1;
}

/******************************************************************************
* board_using
*/
static int board_using()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   CString group_ref = lp;

   BlockStruct *b = doc->Find_Block_by_Name(group_ref, -1);
   FileStruct *using_file = doc->Find_File_by_BlockPtr(b);

   // today we link it, but be really need to merge it.
   if (using_file)
   {
      CopyFileContents(using_file, file, doc);
      //Graph_Block_Reference(group_ref, NULL, 0, 0.0,0.0, 0.0, 0, 1.0, -1, TRUE);
   }
   else
   {
      fprintf(ferr, "GENCAM: Undefined USING reference [%s] at %ld\n", group_ref,ifp_linecnt);
      display_error++;
   }

   return 1;
}

/******************************************************************************
* board_hole
*/
static int board_hole()
{
   int      res, mirror;
   CString  hole_name;
   CString  hole_type;
   CString  primitive_ref, primitive_group;
   CString  barrel_ref, barrel_group;
   CString  profile_ref, profile_group;
   CString  s1, s2;
   double   x,y, rotation;
   char     *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   hole_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   hole_type = lp;

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0) 
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &barrel_group, &barrel_ref)) == 0) 
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &profile_group, &profile_ref)) == 0) // optional
   {
      profile_group = "";
      profile_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)      // layer ref - optional
   {
      s1 = "";
      s2 = "DRILLHOLE";
   }

   if (gettok_location(NULL, ",;", &x, &y, &rotation, &mirror) != 1) 
      return p_error();

   int drilllayernum = Graph_Level(s2, "", 0);
   Graph_Block_Reference(primitive_ref, hole_name, get_groupnum(primitive_group), x, y, 0.0, 0, 1.0, drilllayernum, TRUE);

   return 1;
}

/******************************************************************************
* panel_placement
*/
static int panel_placement()
{
   int      mirror;
   CString  boardname;
   double   x, y, rotation;
   char     *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   boardname = lp;

   if (gettok_location(NULL, ",;", &x, &y, &rotation, &mirror) != 1) 
      return p_error();

   DataStruct  *data = Graph_Block_Reference(boardname, NULL, 0, x, y, rotation, mirror, 1.0, -1, TRUE);
   data->getInsert()->setInsertType(insertTypePcb); 

   return 1;
}

/******************************************************************************
* gcam_polygon
   polygon is followed by lines and arc, which do not intersect and is always
   a closed shape. It can consist of 
   LINE
   CIRCARC
   ELLIPARC
*/
static int gcam_polygon()
{
	cur_fill = false;

   int res;
   CString s1, s2, s3, s4;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) != 0)   // linedesc
   {
      // optional linedesc etc..
   }

   if ((res = gettok_qualstring(NULL, ",;", &s3, &s4)) != 0)   // paintdesc
   {
      // optional linedesc etc..
		
		cur_fill = paintArray.isFill(s4);
   }

   int start_ident = cur_lineident;
   DataStruct *data = Graph_PolyStruct(cur_layer, 0, FALSE);

   if (cur_stat == STAT_BOARDOUTLINE)
   {
      data->setGraphicClass(GR_CLASS_BOARDOUTLINE);
   }
   else if (cur_stat == STAT_BOARDCUTOUT)
   {
      data->setGraphicClass(graphicClassBoardCutout);
   }       
   else if  (cur_stat == STAT_PANELOUTLINE)
   {
      data->setGraphicClass(GR_CLASS_PANELOUTLINE);
   }
   else if  (cur_stat == STAT_PANELCUTOUT)
   {
      data->setGraphicClass(graphicClassPanelCutout);
   }
   else if (cur_stat == STAT_ROUTE)
   {
      if (strlen(cur_netname))
      {
         doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
         data->setGraphicClass(GR_CLASS_ETCH);
      }
   }

   go_command(polygonlist_lst, SIZ_POLYGONLIST_LST, start_ident);
   write_poly(cur_layer, "", TRUE, cur_fill); // a polygon is always closed.

   cur_polyopen = FALSE;
	cur_fill = false;

   return 1;
}

/******************************************************************************
* gcam_polyline
   polygon is followed by lines and arcs.It can consist of 
   LINE
   CIRCARC
   ELLIPARC
*/
static int gcam_polyline()
{
   cur_polyopen = TRUE;
	cur_fill = false;
   int start_ident = cur_lineident;

   DataStruct *data = Graph_PolyStruct(cur_layer, 0, FALSE);

   if (cur_stat == STAT_ROUTE)
   {
      if (strlen(cur_netname))
      {
         doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL);
         data->setGraphicClass(GR_CLASS_ETCH);
      }
   }

   polycnt = 0;
   go_command(polygonlist_lst, SIZ_POLYGONLIST_LST, start_ident);
   write_poly(cur_layer, cur_linedesc, FALSE, cur_fill);

   cur_polyopen = FALSE;

   return 1;
}

/******************************************************************************
* gcam_line
*/
static int gcam_line()
{
   double x, y;

   if (gettok_koo(NULL, ",;",&x, &y) != 2) 
      return p_error();
   
   if (!cur_polyopen)
   {
      Graph_PolyStruct(cur_layer, 0, FALSE);
      Graph_Poly(NULL, 0, 0, 0, TRUE);
   }

   Graph_Vertex(x, y, 0.0);

   if (gettok_koo(NULL, ",;",&x, &y) != 2) 
      return p_error();

   Graph_Vertex(x, y, 0.0);

   return 1;
}

/******************************************************************************
* gcam_rectcornerref
*/
static int gcam_rectcornerref()
{
   double   x, y;
   CString  primitive_group, primitive_ref;
   CString  linedesc_group, linedesc_ref;
   CString  paint_group, paint_ref;
   CString  color_group, color_ref;
   int      res;

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0)      
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)     
      return p_error();

   // paint
   if ((res = gettok_qualstring(NULL, ",;", &paint_group, &paint_ref)) == 0)     
   {     
      paint_group = "";
      paint_ref = "";
   }

   // color
   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)     
   {     
      color_group = "";
      color_ref = "";
   }

   double   rotation;
   double   scale;
   int      mirror;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   Graph_Block_Reference(primitive_ref, NULL, get_groupnum(primitive_group), x, y, rotation, mirror, scale, -1, TRUE);

   return 1;
}

/******************************************************************************
* gcam_rectcenterref
*/
static int gcam_rectcenterref()
{
   double   x, y;
   CString  primitive_group, primitive_ref;
   CString  linedesc_group, linedesc_ref;
   CString  paint_group, paint_ref;
   CString  color_group, color_ref;
   int      res;

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0)      
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)     
      return p_error();

   // paint
   if ((res = gettok_qualstring(NULL, ",;", &paint_group, &paint_ref)) == 0)     
   {     
      paint_group = "";
      paint_ref = "";
   }

   // color
   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)     
   {     
      color_group = "";
      color_ref = "";
   }

   double   rotation;
   double   scale;
   int      mirror;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   Graph_Block_Reference(primitive_ref, NULL, get_groupnum(primitive_group), x, y, rotation, mirror, scale, -1, TRUE);

   return 1;
}

/******************************************************************************
* gcam_polygonref
*/
static int gcam_polygonref()
{
   double   x, y;
   CString  primitive_group, primitive_ref;
   CString  linedesc_group, linedesc_ref;
   CString  paint_group, paint_ref;
   CString  color_group, color_ref;
   int      res;

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0)      
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)     
      return p_error();

   // paint
   if ((res = gettok_qualstring(NULL, ",;", &paint_group, &paint_ref)) == 0)     
   {     
      paint_group = "";
      paint_ref = "";
   }

   // color
   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)     
   {     
      color_group = "";
      color_ref = "";
   }

   double   rotation;
   double   scale;
   int      mirror;

   if (gettok_xform(NULL, ",;",&x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   Graph_Block_Reference(primitive_ref, NULL, get_groupnum(primitive_group), x, y, rotation, mirror, scale, -1, TRUE);

   return 1;
}

/******************************************************************************
* gcam_text
*     TEXT: "105",(555.00,669.00),(555.00,669.00),,"PRIMITIVE"."COL1",(555.00,690.00),0.00,;
*/
static int gcam_text()
{
   double   x, y, x1, y1, x2, y2;
   CString  primitive_group, primitive_ref;
   CString  linedesc_group, linedesc_ref;
   CString  font_group, font_ref;
   CString  color_group, color_ref;
   int      res;
   CString  prosa;
   char     *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // string
      return p_error();

   prosa = lp;

   if (gettok_koo(NULL, ",;",&x1, &y1) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;",&x2, &y2) != 2) 
      return p_error();

   // fontref
   if ((res = gettok_qualstring(NULL, ",;", &font_group, &font_ref)) == 0)    
   {     
      font_group = "";
      font_ref = "";
   }

   // color
   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)     
   {     
      color_group = "";
      color_ref = "";
   }

   double   rotation;
   int      mirror;

   if (gettok_location(NULL, ",;", &x, &y, &rotation, &mirror) != 1) 
      return p_error();

   double   height = 0.1;
   double   width = 0.05;
   int      prop = 0;

   DataStruct *d = Graph_Text(cur_layer, prosa, x, y, height, width, DegToRad(rotation), 0, prop, mirror, 0, FALSE, -1, 0);

   return 1;
}

/******************************************************************************
* gcam_artworkref
*/
static int gcam_artworkref()
{
   CString  s1, s2;    
   CString  artwork_group, artwork_ref;
   CString  color_group, color_ref;
   double   x, y, rotation;
   int      res, mirror;

   if ((res = gettok_qualstring(NULL, ",;", &artwork_group, &artwork_ref)) == 0)    // this is the artwork ref
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)      // this is the layer ref
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)     // this is the color
   {
      color_group = "";
      color_ref = "";
   }

   if (gettok_location(NULL, ",;", &x, &y, &rotation, &mirror) != 1) 
      return p_error();

   int layernr = Graph_Level(s2, "", FALSE);

   Graph_Block_Reference(artwork_ref, NULL, get_groupnum(artwork_group), x, y, rotation, mirror, 1.0, layernr, TRUE);

   return 1;
}

/******************************************************************************
* gcam_circarc
*/
static int gcam_circarc()
{
   double xstart, ystart;
   double xend, yend;
   double xcenter, ycenter;
   int cckw = TRUE;  // counter clock wise
   char *lp;
   double bulge;

   if (gettok_koo(NULL, ",;",&xstart, &ystart) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;",&xend, &yend) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;",&xcenter, &ycenter) != 2) 
      return p_error();

   if ((lp = gettok(NULL, ",;")) != NULL) 
   {
      if (!STRCMPI(lp, "CLKW"))
         cckw = FALSE;
   }

   double r, sa, da;
   int cw = FALSE;

   if (!cckw)
      cw = TRUE;

   ArcCenter2(xstart, ystart, xend, yend, xcenter, ycenter, &r, &sa, &da, cw);
   bulge = tan(da/4);

   Graph_PolyStruct(cur_layer, 0, FALSE);
   Graph_Poly(NULL, 0, 0, 0, TRUE);
   Graph_Vertex(xstart, ystart, bulge);
   Graph_Vertex(xend, yend, 0.0);

   return 1;
}

/******************************************************************************
* gcam_circle
*/
static int gcam_circle()
{
   double   x, y;
   char     *lp;
   double   diameter;
   int      res;
   CString  linedesc_group, linedesc_ref;
   CString  paintdesc_group, paintdesc_ref;
   CString  color_group, color_ref;

   if ((lp = gettok(NULL, ",;")) == NULL) // diameter
      return p_error();

   diameter = cnv_tok(lp);

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)  // linedesc ref, optional
   {
      linedesc_group = "";
      linedesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &paintdesc_group, &paintdesc_ref)) == 0)   // paintdesc ref  
   {
      paintdesc_group = "";
      paintdesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)  // color ref   
   {
      color_group = "";
      color_ref = "";
   }

   if (gettok_koo(NULL, ",;", &x, &y) != 2) 
      return p_error();

   Graph_Circle(cur_layer, x, y, diameter / 2, 0, 0, FALSE, FALSE);    // index of width table

   return 1;
}

/******************************************************************************
* closed_shape_rectcenter
*/
static int closed_shape_rectcenter()
{
   double   width, height;
   double   x, y, rotation;
   int      mirror, res;
   char     *lp;
   CString  linedesc_group, linedesc_ref;
   CString  paintdesc_group, paintdesc_ref;
   CString  color_group, color_ref;

   if ((lp = gettok(NULL, ",;")) == NULL) // 
      return p_error();

   width = cnv_tok(lp);

   if ((lp = gettok(NULL, ",;")) == NULL) // 
      return p_error();

   height = cnv_tok(lp);

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)  // linedesc ref, optional
   {
      linedesc_group = "";
      linedesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &paintdesc_group, &paintdesc_ref)) == 0)   // paintdesc ref  
   {
      paintdesc_group = "";
      paintdesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)  // color ref   
   {
      color_group = "";
      color_ref = "";
   }

   double scale;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   width *= scale;
   height*= scale;
   int widthindex = 0;
   int fill = FALSE;
   double x1 = -width/2+x;
   double y1 = -height/2+y;
   double x2 = width/2+x;
   double y2 = height/2+y;

   Rotate(x1, y1, RadToDeg(rotation), &x, &y);
   x1 = x;
   y1 = y;
   Rotate(x2, y2, RadToDeg(rotation), &x, &y);
   x2 = x;
   y2 = y;

   Graph_Poly(NULL, widthindex, fill, 0, TRUE);
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);

   return 1;
}

/******************************************************************************
* gcam_rectcenter
*/
static int gcam_rectcenter()
{
   double   width, height;
   double   x, y, rotation;
   int      mirror, res;
   char     *lp;
   CString  linedesc_group, linedesc_ref;
   CString  paintdesc_group, paintdesc_ref;
   CString  color_group, color_ref;

   if ((lp = gettok(NULL, ",;")) == NULL) // 
      return p_error();

   width = cnv_tok(lp);

   if ((lp = gettok(NULL, ",;")) == NULL) // 
      return p_error();

   height = cnv_tok(lp);

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)  // linedesc ref, optional
   {
      linedesc_group = "";
      linedesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &paintdesc_group, &paintdesc_ref)) == 0)   // paintdesc ref  
   {
      paintdesc_group = "";
      paintdesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)  // color ref   
   {
      color_group = "";
      color_ref = "";
   }

   double   scale;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   width *= scale;
   height*= scale;
   int widthindex = 0;
   int fill = FALSE;
   double x1 = -width/2+x;
   double y1 = -height/2+y;
   double x2 = width/2+x;
   double y2 = height/2+y;

   Rotate(x1, y1, RadToDeg(rotation), &x, &y);
   x1 = x;
   y1 = y;
   Rotate(x2, y2, RadToDeg(rotation), &x, &y);
   x2 = x;
   y2 = y;

   DataStruct *d = Graph_PolyStruct(0, 0, FALSE);
   Graph_Poly(NULL, widthindex, fill, 0, TRUE);
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);

   return 1;
}

/******************************************************************************
* gcam_rectcorner
*/
static int gcam_rectcorner()
{
   double   x1, y1, x2, y2, x, y;
   double   rotation;
   int      mirror, res;
   CString  linedesc_group, linedesc_ref;
   CString  paintdesc_group, paintdesc_ref;
   CString  color_group, color_ref;

   if (gettok_koo(NULL, ",;",&x1, &y1) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;",&x2, &y2) != 2) 
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)  // linedesc ref, optional
   {
      linedesc_group = "";
      linedesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &paintdesc_group, &paintdesc_ref)) == 0)   // paintdesc ref  
   {
      paintdesc_group = "";
      paintdesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)  // color ref   
   {
      color_group = "";
      color_ref = "";
   }

   double scale;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   int widthindex = 0;
   int fill = FALSE;

   Rotate(x1, y1, RadToDeg(rotation), &x, &y);
   x1 = x;
   y1 = y;
   Rotate(x2, y2, RadToDeg(rotation), &x, &y);
   x2 = x;
   y2 = y;

   DataStruct *d = Graph_PolyStruct(0,0,FALSE);
   Graph_Poly(NULL, widthindex, fill, 0, TRUE);
   Graph_Vertex(x1 * scale, y1 * scale, 0.0);
   Graph_Vertex(x2 * scale, y1 * scale, 0.0);
   Graph_Vertex(x2 * scale, y2 * scale, 0.0);
   Graph_Vertex(x1 * scale, y2 * scale, 0.0);
   Graph_Vertex(x1 * scale, y1 * scale, 0.0);

   return 1;
}

/******************************************************************************
* header_units
*/
static int header_units()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   if (!STRCMPI(lp, "INCH"))
   {
      scale_factor = Units_Factor(UNIT_INCHES, PageUnits);
   }
   else if (!STRCMPI(lp, "MM"))
   {
      scale_factor = Units_Factor(UNIT_MM, PageUnits);
   }
   else if (!STRCMPI(lp, "UM"))  // micrometer
   {
      scale_factor = Units_Factor(UNIT_MM, PageUnits) / 1000;
   }
   else if (!STRCMPI(lp, "THOU"))
   {
      scale_factor = Units_Factor(UNIT_MILS, PageUnits);
   }
   else if (!STRCMPI(lp, "THOU10")) // 1/10.000 inch
   {
      scale_factor = Units_Factor(UNIT_MILS, PageUnits) /10;
   }
   else
   {
      fprintf(ferr, "GENCAM: Unknown UNIT command [%s] unknown at %ld\n", lp, ifp_linecnt);
      display_error++;
   }

   return 1;
}

/******************************************************************************
* header_angleunits
*/
static int header_angleunits()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   if (!STRCMPI(lp, "RADIANS"))
   {
      AngleUnits = 0;
   }
   else if (!STRCMPI(lp, "DEGREES"))
   {
      AngleUnits = 1;
   }
   else
   {
      fprintf(ferr, "GENCAM: Unknown ANGLEUNIT command [%s] unknown at %ld\n", lp, ifp_linecnt);
      display_error++;
   }

   return 1;
}

/******************************************************************************
* gcam_header
*/
static int gcam_header()
{
   int start_ident = cur_lineident;

   loop_command(header_lst, SIZ_HEADER_LST, start_ident);

   if (!header_gencam_found)  
   {
      ErrorMessage("This file is not a GenCAM file!", "GenCAM Format Error");
      display_error++;

      return -3;
   }

   return 1;
}

/******************************************************************************
* gcam_boards
*/
static int gcam_boards()
{
   int start_ident = cur_lineident;
   loop_command(boards_lst, SIZ_BOARDS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_panels
*/
static int gcam_panels()
{
   int start_ident = cur_lineident;
   loop_command(panels_lst, SIZ_PANELS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_artworks
*/
static int gcam_artworks()
{
   int start_ident = cur_lineident;
   loop_command(artworks_lst, SIZ_ARTWORKS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_primitives
*/
static int gcam_primitives()
{
   int start_ident = cur_lineident;
   loop_command(primitives_lst, SIZ_PRIMITIVES_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_layers
*/
static int gcam_layers()
{
   int start_ident = cur_lineident;
   loop_command(layers_lst, SIZ_LAYERS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_routes
*/
static int gcam_routes()
{
   int start_ident = cur_lineident;
   loop_command(routes_lst, SIZ_ROUTES_LST, start_ident);

   return 1;
}

/******************************************************************************
* component_attribute
*/
static int component_attribute()
{  
   CString attrib_owner, attrib_key, attrib_val;

   if (gencam_attribute(&attrib_owner, &attrib_key, &attrib_val) != 1)
   {
      fprintf (ferr, "GENCAM: Error getting Attribute at %ld\n", ifp_linecnt);
      display_error++;

      return 0;
   }

	if (cur_data)
		doc->SetUnknownAttrib(&cur_data->getAttributesRef(), getGenCamReader().mapName(attrib_key), attrib_val, SA_APPEND, NULL);
	else
		getGenCamReader().QueuedAttributes.Add(new CGENCAMAtt(attrib_key, attrib_val));

   return 1;
}

/******************************************************************************
* gcam_component
*/
static int gcam_component()
{
	getGenCamReader().QueuedAttributes.RemoveAll();

   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // component name etc....
      return p_error();

   cur_comp.refdes = lp;

   if (gettok_qualstring(NULL, ",;", &cur_comp.layergroup, &cur_comp.layername) == 0)     // place layer
      return p_error();

   if (gettok_location(NULL, ",;", &cur_comp.x, &cur_comp.y, &cur_comp.rotation, &cur_comp.mirror) != 1) 
      return p_error();

   cur_comp.type = INSERTTYPE_PCBCOMPONENT;
   cur_comp.device_ref = "";
   cur_comp.device_group = "";

   cur_data = NULL;
   loop_command(component_lst, SIZ_COMPONENT_LST, start_ident);
   
   if (cur_data) // gets cur_data if a pattern was attached
   {
		double compHeight = -1;

      if (cur_comp.type == INSERTTYPE_PCBCOMPONENT && !strlen(cur_comp.device_ref))
      {
         fprintf(ferr, "GENCAM: Component [%s] without a DEVICEREF at %ld\n", cur_comp.refdes, ifp_linecnt);
         display_error++;

         cur_comp.device_ref.Format("%s_%s", cur_comp.pattern_ref, cur_comp.refdes); 
         cur_type = AddType(file, cur_comp.device_ref);
         cur_type->setBlockNumber( cur_data->getInsert()->getBlockNumber());  
      }
      else
      {
         if (strlen(cur_comp.device_ref))
         {
            // here we need to copy the device to the component file. Otherwise we can not find the device 
            // anymore, because the component has only a Name as a reference.
            BlockStruct *block = doc->Find_Block_by_Name(cur_comp.device_group, -1);

				if (block)
				{
					FileStruct *device_file = doc->Find_File_by_BlockPtr(block); 

					if (device_file)
					{
                  TypeStruct *torig = AddType(device_file, cur_comp.device_ref);
						TypeStruct *tmod = AddType(file, cur_comp.device_ref);

						if (tmod && torig)
							doc->CopyAttribs(&tmod->getAttributesRef(), torig->getAttributesRef());

						// Get package from device
						if (torig && torig->getAttributesRef() != NULL )
						{
							// Get package group
							Attrib *attrib = NULL;
							WORD packageGroupKey = (WORD)doc->RegisterKeyWord("PACKAGE_GROUP", 0, VT_STRING);
							torig->getAttributesRef()->Lookup(packageGroupKey, attrib);
							CString packageGroup = get_attvalue_string(doc, attrib);

							// Get package ref
							attrib = NULL;
							WORD packageRefKey = (WORD)doc->RegisterKeyWord("PACKAGE_REF", 0, VT_STRING);
							torig->getAttributesRef()->Lookup(packageRefKey, attrib);
							CString packageRef = get_attvalue_string(doc, attrib);

							// Get the package
							BlockStruct *block = doc->Find_Block_by_Name(packageRef, get_groupnum(packageGroup), blockTypeUndefined);

							if (block && block->getAttributes() != NULL)
							{
								// Get package comp height
								WORD compHeightKey = (WORD)doc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);

								if (block->getAttributes()->Lookup(compHeightKey, attrib) && attrib != NULL)
									compHeight = attrib->getDoubleValue();
							}
						}
					}
				}
         }
      }

      cur_data->getInsert()->setInsertType(cur_comp.type);

      if (strlen(cur_comp.device_ref))
      {
         doc->SetAttrib(&cur_data->getAttributesRef(), doc->IsKeyWord(ATT_TYPELISTLINK, TRUE), VT_STRING,
            cur_comp.device_ref.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
      }

		if (compHeight > -1)
			doc->SetAttrib(&cur_data->getAttributesRef(), doc->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE, &compHeight, SA_OVERWRITE, NULL);

		if (getGenCamReader().QueuedAttributes.GetCount() > 0)
		{
			for (int i = 0; i < getGenCamReader().QueuedAttributes.GetCount(); i++)
			{
				CGENCAMAtt *att = getGenCamReader().QueuedAttributes.GetAt(i);
				doc->SetUnknownAttrib(&cur_data->getAttributesRef(), getGenCamReader().mapName(att->GetKey()), att->GetVal(), SA_APPEND, NULL);
			}
			getGenCamReader().QueuedAttributes.RemoveAll();
		}
	}
   else
   {
      fprintf(ferr, "GENCAM: Error: Component [%s] without a PATTERN at %ld\n", cur_comp.refdes, ifp_linecnt);
      display_error++;
   }

   cur_data = NULL;

   return 1;
}

/******************************************************************************
* gcam_components
*/
static int gcam_components()
{
   int start_ident = cur_lineident;

   loop_command(components_lst, SIZ_COMPONENTS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_patterndef
*/
static int gcam_patterndef()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL,",;")) == NULL)  // get group name
      return p_error();

   CString pattern = lp;

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, pattern, file->getFileNumber(), 0);
   //block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
   BlockStruct* block = getGenCamReader().graphBlockOn(pattern,file->getFileNumber(),blockTypePcbComponent);

   loop_command(pattern_lst, SIZ_PATTERN_LST, start_ident);
   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* gcam_padstack
*/
static int gcam_padstack()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // get group name
      return p_error();

   CString padstack = lp;

   //BlockStruct *b = Graph_Block_On(GBO_APPEND, padstack, file->getFileNumber(), 0);
   //b->setBlockType(BLOCKTYPE_PADSTACK);
   BlockStruct* b = getGenCamReader().graphBlockOn(padstack,file->getFileNumber(),blockTypePadstack);

   loop_command(padstack_lst, SIZ_PADSTACK_LST, start_ident);
   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* gcam_holedef
*/
static int gcam_holedef()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // get group name
      return p_error();

   CString hole = lp;

   // holetype
   if ((lp = gettok(NULL, ",;")) == NULL) //  
      return p_error();

   // prim_ref
   CString primitive_ref, primitive_group;
   CString barrel_ref, barrel_group;
   CString profile_ref, profile_group;
   CString layer_ref, layer_group;
   CString display_ref, display_group;
   int res;

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0)      
      return p_error();

   // barreldesc_ref
   if ((res = gettok_qualstring(NULL, ",;", &barrel_group, &barrel_ref)) == 0)      
   {
   }

   // profiledesc_ref
   if ((res = gettok_qualstring(NULL, ",;", &profile_group, &profile_ref)) == 0)    
   {
   }

   // layers_ref
   if ((res = gettok_qualstring(NULL, ",;", &layer_group, &layer_ref)) == 0)     
   {
   }

   // display_graphics_ref
   if ((res = gettok_qualstring(NULL, ",;", &display_group, &display_ref)) == 0)    
   {
   }

   return 1;
}

/******************************************************************************
* gcam_paddef
*/
static int gcam_paddef()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // 
      return p_error();

   CString padname = lp;

   // prim_ref
   CString primitive_ref, primitive_group;
   CString layer_ref, layer_group;
   int res;

   res = gettok_qualstring(NULL,",;", &layer_group, &layer_ref); // layers_ref      
   res = gettok_qualstring(NULL,",;", &primitive_group, &primitive_ref); // pad_primitive_ref      

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, padname, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(padname,file->getFileNumber(),blockTypeUnknown);

   int layernr = Graph_Level(layer_ref,"",0);

   Graph_Block_Reference(primitive_ref, NULL, get_groupnum(primitive_group), 0.0, 0.0, 0.0, 0, 1.0, layernr, TRUE);
   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* gcam_package
*/
static int gcam_package()
{
   int start_ident = cur_lineident;
   char *lp;

	// Get group name
   if ((lp = gettok(NULL, ",;")) == NULL) 
      return p_error();

   CString package = lp;

	// Get the type and ignore it
   lp = gettok(NULL, ",;");				 

	// Get comp height if it is there
	double compHeight = -1;

   if ((lp = gettok(NULL, ",;")) != NULL && strlen(lp) > 0) 
		compHeight = atof(lp);

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, package, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(package,file->getFileNumber(),blockTypeUnknown);

   loop_command(package_lst, SIZ_PACKAGE_LST, start_ident);
   Graph_Block_Off();

	if (compHeight > -1)
	{
		int compHeightKey = doc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);
		doc->SetAttrib(&block->getAttributesRef(), compHeightKey, VT_UNIT_DOUBLE, &compHeight, SA_OVERWRITE, NULL);
	}

   return 1;
}

/******************************************************************************
* gcam_device
*/
static int gcam_device()
{
   int start_ident = cur_lineident;
   CString device, devicetype;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // device name
      return p_error();

   device = lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // device type
      return p_error();

   devicetype = lp;
   
   CString package_ref, package_group;
   CString family_ref, family_group;
   int res;

   if ((res = gettok_qualstring(NULL, ",;", &package_group, &package_ref)) == 0)    
      return p_error();

   res = gettok_qualstring(NULL, ",;", &family_group, &family_ref);     

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, package_ref, get_groupnum(package_group), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(package_ref,get_groupnum(package_group),blockTypeUnknown);
   Graph_Block_Off();

   TypeStruct *cur_type = AddType(file, device);

   if (cur_type && cur_type->getBlockNumber() > -1 && (cur_type->getBlockNumber() != block->getBlockNumber()))
   {      
      // problem - a device has different shapes
      BlockStruct *typeRef = doc->Find_Block_by_Num(cur_type->getBlockNumber());
      fprintf(ferr, "GENCAM: Device [%s] on Component [%s] already has Package [%s] assigned !\n", cur_type->getName(), package_ref, typeRef->getName());
      display_error++;
      device += "_";
      device += package_ref;
      cur_type = AddType(file, device);
   }

	if (cur_type)
	{
      cur_type->setBlockNumber( block->getBlockNumber());

		int packageGroupKey = doc->RegisterKeyWord("PACKAGE_GROUP", 0, VT_STRING);
		int packageRefKey = doc->RegisterKeyWord("PACKAGE_REF", 0, VT_STRING);
		doc->SetAttrib(&cur_type->getAttributesRef(), packageGroupKey, package_group, SA_OVERWRITE, NULL);
		doc->SetAttrib(&cur_type->getAttributesRef(), packageRefKey, package_ref, SA_OVERWRITE, NULL);
	}

   loop_command(device_lst, SIZ_DEVICE_LST, start_ident);

   return 1;
}

/****************************************************************************/
/*
<attribute_def>   ::=   ATTRIBUTE: <attribute_owner>, <attribute_name>, <attribute_value>,
                [<change_history>];
   <attribute_owner>    ::=   string
   <attribute_name>  ::=   string
   <attribute_value> ::=   string
   <change_history>  ::=   p_integer

The parameters of an ATTRIBUTE statement are defined as follows:

<attribute_owner> the registered owner of the GenCAM attribute name.
<attribute_name>  the name of the attribute registered by the owner.  
<attribute_value> the value of the attribute.  
[<change_history>]   the GenCAM file history revision number to which the modification was made.

*/
static int gencam_attribute(CString *owner, CString *key, CString *val)
{
   char  *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // 
      return p_error();

   *owner = lp;

   if ((lp = gettok(NULL, ",;")) == NULL) //
      return p_error();

   *key = lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // 
      return p_error();

   *val = lp;

   return 1;
}

/******************************************************************************
* device_attribute
*/
static int device_attribute()
{  
   CString attrib_owner, attrib_key, attrib_val;

   if (gencam_attribute(&attrib_owner, &attrib_key, &attrib_val) != 1)
   {
      fprintf (ferr, "GENCAM: Error getting Attribute at %ld\n", ifp_linecnt);
      display_error++;

      return 0;
   }

   if (cur_type)
      doc->SetUnknownAttrib(&cur_type->getAttributesRef(), attrib_key, attrib_val, SA_OVERWRITE, NULL); // x, y, rot, height

   return 1;
}

/******************************************************************************
* device_part
*/
static int device_part()
{  
   char  *lp;
   CString part_ref, part_id;

   if ((lp = gettok(NULL, ",;")) == NULL) // enterprise ref ?
      return p_error();

   if ((lp = gettok(NULL, ",;")) == NULL) // part_id
      return p_error();

   part_id = lp;

   if (lp = gettok(NULL, ",;"))           // part_revision
      part_ref = lp;

   if (strlen(part_id) && cur_type)
      doc->SetUnknownAttrib(&cur_type->getAttributesRef(), ATT_PARTNUMBER, part_id, SA_APPEND, NULL); // x, y, rot, height

   return 1;
}

/******************************************************************************
* device_pindesc
*/
static int device_pindesc()
{
   return 1;
}

/******************************************************************************
* device_value
*/
static int device_value()
{
   char *lp;
   CString ntol, ptol;
   CString valtype, val;
   double mvalue;

   if ((lp = gettok(NULL, ",;")) == NULL) // measure value
      return p_error();

   mvalue = atof(lp);

   if ((lp = gettok(NULL, ",;")) == NULL) // value_type
      return p_error();

   valtype = lp; // OHM, FARAD, HENRY, AMP, WATT, HERTZ, JOULE, LUMEN

   valtype.TrimLeft();
   valtype.TrimRight();

   if (cur_type && ComponentValues::GetPrintableString(mvalue, valtype, val))
   {
      doc->SetUnknownAttrib(&cur_type->getAttributesRef(), ATT_VALUE, val, SA_OVERWRITE, NULL); // x, y, rot, height
   }
   else
   {
      fprintf(ferr, "GENCAM: Can not convert Value [%lg %s]\n", mvalue, valtype);
      display_error++;
   }

   ntol = "";

   if (lp = gettok(NULL,",;"))
      ntol = lp;

   ptol = "";

   if (lp = gettok(NULL,",;")) 
      ptol = lp;

   if (cur_type && strlen(ntol))
      doc->SetUnknownAttrib(&cur_type->getAttributesRef(), ATT_MINUSTOLERANCE, ntol, SA_OVERWRITE, NULL); // x, y, rot, height

   if (cur_type && strlen(ptol))
      doc->SetUnknownAttrib(&cur_type->getAttributesRef(), ATT_PLUSTOLERANCE, ptol, SA_OVERWRITE, NULL); // x, y, rot, height

   return 1;
}

/******************************************************************************
* gcam_patterns
*/
static int gcam_patterns()
{
   int start_ident = cur_lineident;
   loop_command(patterns_lst, SIZ_PATTERNS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_padstacks
*/
static int gcam_padstacks()
{
   int start_ident = cur_lineident;
   loop_command(padstacks_lst, SIZ_PADSTACKS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_packages
*/
static int gcam_packages()
{
   int start_ident = cur_lineident;
   loop_command(packages_lst, SIZ_PACKAGES_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_devices
*/
static int gcam_devices()
{
   int start_ident = cur_lineident;
   loop_command(devices_lst, SIZ_DEVICES_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_testconnects
*/
static int gcam_testconnects()
{
   int start_ident = cur_lineident;
   loop_command(testconnects_lst, SIZ_TESTCONNECTS_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_administration
*/
static int gcam_administration()
{
   int start_ident = cur_lineident;
   loop_command(administration_lst, SIZ_ADMINISTRATION_LST, start_ident);

   return 1;
}

/******************************************************************************
* gcam_drawings
*/
static int gcam_drawings()
{
   int start_ident = cur_lineident;
   loop_to_end(start_ident);

   return 1;
}

/******************************************************************************
* gcam_changes
*/
static int gcam_changes()
{
   int start_ident = cur_lineident;
   loop_to_end(start_ident);

   return 1;
}

/******************************************************************************
* gcam_mechanicals
*/
static int gcam_mechanicals()
{
   int start_ident = cur_lineident;
   loop_to_end(start_ident);

   return 1;
}

/******************************************************************************
* gcam_fixtures
*/
static int gcam_fixtures()
{
   int start_ident = cur_lineident;
   loop_to_end(start_ident);

   return 1;
}

/******************************************************************************
* gcam_families
*/
static int gcam_families()
{
   int start_ident = cur_lineident;
   loop_to_end(start_ident);

   return 1;
}

/******************************************************************************
* gcam_power
*/
static int gcam_power()
{
   int start_ident = cur_lineident;
   loop_to_end(start_ident);

   return 1;
}

/******************************************************************************
* routes_group
*/
static int routes_group()
{
   int start_ident = cur_lineident;
   CString groupname;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // get group name
      return p_error();

   groupname = lp;
   set_group_to_file(groupname);

   loop_command(routes_route_lst, SIZ_ROUTES_ROUTE_LST, start_ident);

   return 1;
}

/******************************************************************************
* components_group
*/
static int components_group()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // get group name
      return p_error();

   CString groupname = lp;
   set_group_to_file(groupname);

   loop_command(components_component_lst, SIZ_COMPONENTS_COMPONENT_LST, start_ident);

   return 1;
}

/******************************************************************************
* patterns_group
*/
static int patterns_group()
{
   int start_ident = cur_lineident;
   CString groupname;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // get group name
      return p_error();

   groupname = lp;
   set_group_to_file(groupname);

   loop_command(patterns_pattern_lst, SIZ_PATTERNS_PATTERN_LST, start_ident);

   return 1;
}

/******************************************************************************
* padstacks_group
*/
static int padstacks_group()
{
   int start_ident = cur_lineident;
   CString groupname;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // get group name
      return p_error();

   groupname = lp;
   set_group_to_file(groupname);

   loop_command(padstacks_padstack_lst, SIZ_PADSTACKS_PADSTACK_LST, start_ident);

   return 1;
}

/******************************************************************************
* packages_group
*/
static int packages_group()
{
   int start_ident = cur_lineident;
   CString groupname;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // get group name
      return p_error();

   groupname = lp;
   set_group_to_file(groupname);

   loop_command(packages_package_lst, SIZ_PACKAGES_PACKAGE_LST, start_ident);

   return 1;
}

/******************************************************************************
* devices_group
*/
static int devices_group()
{
   int start_ident = cur_lineident;
   CString groupname;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // get group name
      return p_error();

   groupname = lp;
   set_group_to_file(groupname);

   loop_command(devices_device_lst, SIZ_DEVICES_DEVICE_LST, start_ident);

   return 1;
}

/******************************************************************************
* artworks_group
*/
static int artworks_group()
{
   int start_ident = cur_lineident;
   CString groupname;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // get group name
      return p_error();

   groupname = lp;

   set_group_to_file(groupname);

   loop_command(artworks_artwork_lst, SIZ_ARTWORKS_ARTWORK_LST, start_ident);

   return 1;
}

/******************************************************************************
* primitives_group
*/
static int primitives_group()
{
   int start_ident = cur_lineident;
   CString groupname;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // get group name
      return p_error();

   groupname = lp;

   // need to make a primitiveDef array, so that different primitives can be 
   // seperated
   set_group_to_file(groupname);

   loop_command(primitive_lst, SIZ_PRIMITIVE_LST,start_ident);

   return 1;
}

/******************************************************************************
* layers_group
*/
static int layers_group()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // get group name
      return p_error();

   CString groupname = lp;

   // need to make a layerDef array, so that different layers can be 
   // seperated

   doc->PrepareAddEntity(global_file);       

   loop_command(layer_lst, SIZ_LAYER_LST, start_ident);

   return 1;
}

/******************************************************************************
* route_attribute
*/
static int route_attribute()
{  
   CString attrib_owner, attrib_key, attrib_val;

   if (gencam_attribute(&attrib_owner, &attrib_key, &attrib_val) != 1)
   {
      fprintf (ferr,"GENCAM: Error getting Attribute at %ld\n", ifp_linecnt);
      display_error++;

      return 0;
   }

   if (strlen(cur_netname))
   {
      NetStruct *net = add_net(file,cur_netname);
      doc->SetUnknownAttrib(&net->getAttributesRef(), attrib_key, attrib_val, SA_APPEND, NULL); //  
   }

   return 1;
}

/******************************************************************************
* gcam_route
*/
static int gcam_route()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // netname
      return p_error();

   cur_netname = lp;

   if (!STRCMPI(lp, "GENCAMEXTNC"))
   {
      cur_netname = NET_UNUSED_PINS;
      NetStruct *net = add_net(file, NET_UNUSED_PINS);
      net->setFlagBits(NETFLAG_UNUSEDNET);
   }
   else
   {
      NetStruct *net = add_net(file,cur_netname);
   }

   loop_command(route_lst, SIZ_ROUTE_LST, start_ident);

   return 1;
}

/******************************************************************************
* route_path
*/
int route_path()  
{
   int res;
   int start_ident = cur_lineident;
   CString s1, s2;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)    // this is layer
      return p_error();

   cur_layer = Graph_Level(s2, "", 0);

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)    // this is linedesc
      return p_error();

   cur_linedesc = s2;
   cur_stat = STAT_ROUTE;
   DataStruct *data = Graph_PolyStruct(cur_layer, 0, FALSE);
   data->setGraphicClass(GR_CLASS_ETCH);

   if (cur_stat == STAT_ROUTE)
   {
      if (strlen(cur_netname))
      {
         doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
               VT_STRING, cur_netname.GetBuffer(0),  SA_OVERWRITE, NULL);
         data->setGraphicClass(GR_CLASS_ETCH);
      }
   }

   go_command(polyline_lst, SIZ_POLYLINE_LST, start_ident);
   cur_stat = STAT_NONE;
   cur_linedesc = "";

   return 1;
}

/******************************************************************************
* route_plane
*/
int route_plane()  
{
   int res;
   int start_ident = cur_lineident;
   CString s1, s2;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)    // this is layer
      return p_error();

   cur_layer = Graph_Level(s2, "", 0);

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)    // this is linedesc
      return p_error();

   cur_stat = STAT_ROUTE;
   DataStruct *data = Graph_PolyStruct(cur_layer, 0, FALSE);

   if (cur_stat == STAT_ROUTE)
   {
      if (strlen(cur_netname))
      {
         doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
               VT_STRING, cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); 
         data->setGraphicClass(GR_CLASS_ETCH);
      }
   }

   go_command(polygon_lst, SIZ_POLYGON_LST, start_ident);
   cur_stat = STAT_NONE;

   return 1;
}

/******************************************************************************
* route_via
*/
int route_via()
{
   CString  s1, s2;    
   double   x, y;
   int      res;

   char     *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // pinname
      return p_error();

   CString vianame = lp;

   CString  padstack_group, padstack_ref;

   if ((res = gettok_qualstring(NULL, ",;", &padstack_group, &padstack_ref)) == 0)      // this is the padtsack ref
      return p_error();

   double x1,y1;

   if (gettok_koo(NULL, ",;", &x1, &y1) != 2) 
   {
      // testpoint x,y - not needed
   }

   if ((lp = gettok(NULL, ",;")) == NULL)  // probe/noprobe
   {
      // no probe - optional
   }

	double rotation;
	int mirror;
   double scale;

   if ((res = gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale)) != 1) 
      return p_error();

   int layernr = -1;

   if(vianame.IsEmpty())
      vianame.Format("via%d", ++viacnt);

   DataStruct  *data = Graph_Block_Reference(padstack_ref, vianame, get_groupnum(padstack_group), 
      x, y, rotation, mirror, scale, layernr, TRUE);
   data->getInsert()->setInsertType(insertTypeVia);

   if (strlen(cur_netname))
   {
      doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING,
            cur_netname.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
   }

   return 1;
}

/******************************************************************************
* route_testpad
*/
int route_testpad()
{
   fprintf(ferr, "GENCAM: Not implemented [%s] at %ld\n", "route_testpad", ifp_linecnt);
   display_error++;
   fskip();

   return 1;
}

/******************************************************************************
* route_comppin
*/
int route_comppin()
{
   CString  comp_group, comp_ref;
   char *lp;
   int res;

   if ((res = gettok_qualstring(NULL, ",;", &comp_group, &comp_ref)) == 0)     // comppin_ref
      return p_error();

   if ((lp = gettok(NULL, ",;")) == NULL)  // pattern_pin_ref
      return p_error();

   CString pinname = lp;

   double x, y, rotation;
   int mirror;
   res = gettok_location(NULL, ",;", &x, &y, &rotation, &mirror);   // testpoint_xy

   NetStruct *net = add_net(file, cur_netname);
   CompPinStruct *comppin = add_comppin(file, net, comp_ref, pinname);

   return 1;
}

/******************************************************************************
* route_connpin
*/
int route_connpin()
{
   CString comp_group, comp_ref;
   char *lp;
   int res;

   if ((res = gettok_qualstring(NULL, ",;", &comp_group, &comp_ref)) == 0)     // this is layer
      return p_error();

   if ((lp = gettok(NULL, ",;")) == NULL)  // netname
      return p_error();

   CString pinname = lp;

   double x, y, rotation;
   int mirror;
   res = gettok_location(NULL, ",;", &x, &y, &rotation, &mirror);

   NetStruct *net = add_net(file, cur_netname);
   CompPinStruct *comppin = add_comppin(file, net, comp_ref, pinname);

   return 1;
}


/******************************************************************************
* route_highpottest
*/
int route_highpottest()
{
   fprintf(ferr, "GENCAM: Not implemented [%s] at %ld\n", "route_highpottest", ifp_linecnt);
   display_error++;
   fskip();

   return 1;
}

/******************************************************************************
* gcam_artworkdef
*/
int gcam_artworkdef()
{
   int start_ident = cur_lineident;
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // get group name
      return p_error();

   CString artwork = lp;

   cur_layer = Graph_Level("0", "", 1);  // always floating

   //BlockStruct *b = Graph_Block_On(GBO_APPEND, artwork, file->getFileNumber(), 0);
   BlockStruct* b = getGenCamReader().graphBlockOn(artwork,file->getFileNumber(),blockTypeUnknown);

   loop_command(artworkbuilder_lst, SIZ_ARTWORKBUILDER_LST, start_ident);
   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* gcam_logodef
*/
int gcam_logodef()
{
   fprintf(ferr, "GENCAM: Not implemented [%s] at %ld\n", "gcam_logodef", ifp_linecnt);
   display_error++;
   fskip();

   return 1;
}

/******************************************************************************
* gcam_featuredef
*/
int gcam_featuredef()
{
   fprintf(ferr, "GENCAM: Not implemented [%s] at %ld\n", "gcam_featuredef", ifp_linecnt);
   display_error++;
   fskip();

   return 1;
}

/******************************************************************************
* gcam_targetdef
*/
int gcam_targetdef()
{
   fprintf(ferr, "GENCAM: Not implemented [%s] at %ld\n", "gcam_targetdef", ifp_linecnt);
   display_error++;
   fskip();

   return 1;
}

/******************************************************************************
* primitive_ovaldef
*/
int primitive_ovaldef()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString oval_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double width = cnv_tok(lp);

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double height = cnv_tok(lp);

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, oval_name, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(oval_name,file->getFileNumber(),blockTypeUnknown);

   int widthindex = 0;
   int fill = FALSE;

   DataStruct *data = Graph_PolyStruct(0,0, FALSE);
   Graph_Poly(NULL,widthindex, fill, 0, TRUE);
   Graph_Vertex(-(width-height)/2, -height/2, 0.0);
   Graph_Vertex((width-height)/2, -height/2, 1.0);
   Graph_Vertex((width-height)/2, height/2, 0.0);
   Graph_Vertex(-(width-height)/2, height/2, 1.0);
   Graph_Vertex(-(width-height)/2, -height/2, 0.0);

   Graph_Block_Off();

   getGenCamReader().defineOvalPrimitive(oval_name,file->getFileNumber(),width,height);

   return 1;
}

/******************************************************************************
* primitive_rectcenterdef
*/
int primitive_rectcenterdef()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString rectangle_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double width = cnv_tok(lp);

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double height = cnv_tok(lp);

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, rectangle_name, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(rectangle_name,file->getFileNumber(),blockTypeUnknown);

   int widthindex = 0;
   int fill = FALSE;

   DataStruct *data = Graph_PolyStruct(0,0,FALSE);
   Graph_Poly(NULL,widthindex, fill, 0, TRUE);
   Graph_Vertex(-width/2, -height/2, 0.0);
   Graph_Vertex(width/2, -height/2, 0.0);
   Graph_Vertex(width/2, height/2, 0.0);
   Graph_Vertex(-width/2, height/2, 0.0);
   Graph_Vertex(-width/2, -height/2, 0.0);

   Graph_Block_Off();

   getGenCamReader().defineRectanglePrimitive(rectangle_name,file->getFileNumber(),width,height);

   return 1;
}

/******************************************************************************
* primitive_dshapedef
   This is really a Round, Fillet or Chamfer aoperture (simular to BULLET) in Redac
*/
int primitive_dshapedef()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString dshape_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double width = cnv_tok(lp);

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double height = cnv_tok(lp);

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, dshape_name, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(dshape_name,file->getFileNumber(),blockTypeUnknown);

   int widthindex = 0;
   int fill = FALSE;

   DataStruct *d = Graph_PolyStruct(0,0, FALSE);
   Graph_Poly(NULL,widthindex, fill, 0, TRUE);
   Graph_Vertex(-width/2, -height/2, 0.0);
   Graph_Vertex(width/2, -height/2, 0.0);
   Graph_Vertex(width/2, height/2, 0.0);
   Graph_Vertex(-width/2, height/2, 0.0);
   Graph_Vertex(-width/2, -height/2, 0.0);

   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* primitive_polygondef
*/
int primitive_polygondef()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString polygon_name = lp;

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, polygon_name, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(polygon_name,file->getFileNumber(),blockTypeUnknown);

   int widthindex = 0;
   int fill = FALSE;
   int start_ident = cur_lineident;
   int cur_layer = Graph_Level("0", "", 1);  // global layer

   polycnt = 0;

   DataStruct *data = Graph_PolyStruct(cur_layer,0,FALSE);
   go_command(polygonlist_lst, SIZ_POLYGONLIST_LST, start_ident);
   Graph_Block_Off();

   if (data->getPolyList()->GetCount() == 1)
   {
      CPoly* poly = data->getPolyList()->GetHead();
      double llx,lly,urx,ury;

      if (PolyIsRectangle(poly,&llx,&lly,&urx,&ury))
      {
         if (fpeq(-llx,urx) && fpeq(-lly,ury))
         {
            double width  = 2. * urx;
            double height = 2. * ury;

            getGenCamReader().defineRectanglePrimitive(polygon_name,file->getFileNumber(),width,height);
         }
      }
   }

   return 1;
}

/******************************************************************************
* primitive_rectcornerdef
*/
int primitive_rectcornerdef()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString rectangle_name = lp;

   double x1,y1,x2,y2;

   if (gettok_koo(NULL, ",;", &x1, &y1) != 2) 
      return p_error();

   if (gettok_koo(NULL, ",;", &x2, &y2) != 2) 
      return p_error();

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, rectangle_name, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(rectangle_name,file->getFileNumber(),blockTypeUnknown);

   int widthindex = 0;
   int fill = FALSE;

   DataStruct *data = Graph_PolyStruct(0,0, FALSE);
   Graph_Poly(NULL,widthindex, fill, 0, TRUE);
   Graph_Vertex(x1, y1, 0.0);
   Graph_Vertex(x2, y1, 0.0);
   Graph_Vertex(x2, y2, 0.0);
   Graph_Vertex(x1, y2, 0.0);
   Graph_Vertex(x1, y1, 0.0);

	Graph_Block_Off();

   return 1; 
}

/******************************************************************************
* primitive_barreldesc
*/
int primitive_barreldesc()
{
/*
   fprintf(ferr,"GENCAM: Not implemented [%s] at %ld\n","primitive_barreldesc", ifp_linecnt);
   display_error++;
*/
   fskip();

   return 1;
}

/******************************************************************************
* primitive_circledef
*/
int primitive_circledef()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString circledesc_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double radius = cnv_tok(lp)/2.;

   //BlockStruct *block = Graph_Block_On(GBO_APPEND, circledesc_name, file->getFileNumber(), 0);
   BlockStruct* block = getGenCamReader().graphBlockOn(circledesc_name,file->getFileNumber(),blockTypeUnknown);

   Graph_Circle(0, 0.0, 0.0, radius,0, 0, FALSE, FALSE); 
   Graph_Block_Off();

   getGenCamReader().defineCirclePrimitive(circledesc_name,file->getFileNumber(),radius);

   return 1;
}

/******************************************************************************
* primitive_linedesc
*/
int primitive_linedesc()
{
   char *lp;
   int err;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString linedesc_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double width = cnv_tok(lp);

   doc->PrepareAddEntity(global_file);    // apertures are always global, but I also need the real definition
   Graph_Aperture(linedesc_name, T_ROUND, width, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   doc->PrepareAddEntity(file);        

   return 1;
}

/******************************************************************************
* primitive_thermaldef
   outer, inner, spokecount, spokewidth, spokeangle, spokeendshape
*/
int primitive_thermaldef()
{
   char *lp;
   int err;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString thermaldesc_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double outer = cnv_tok(lp);

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   double inner = cnv_tok(lp);

   doc->PrepareAddEntity(global_file);    // apertures are always global, but I also need the real definition
   Graph_Aperture(thermaldesc_name, T_THERMAL, outer, inner, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   doc->PrepareAddEntity(file);        

   return 1;
}

/******************************************************************************
* primitive_paintdesc
*/
int primitive_paintdesc()
{
   char *lp;
   int fill = 0;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   CString paintdesc_name = lp;

   if ((lp = gettok(NULL, ",;")) == NULL)  // 
      return p_error();

   if (!STRCMPI(lp, "VOID"))
      fill = PAINT_VOID;
   else if (!STRCMPI(lp, "HOLLOW"))
      fill = PAINT_HOLLOW;
   else if (!STRCMPI(lp, "FILL"))
      fill = PAINT_FILL;
   else if (!STRCMPI(lp, "HATCH"))
      fill = PAINT_HATCH;
   else if (!STRCMPI(lp, "MESH"))
      fill = PAINT_MESH;
   else
   {
      fprintf(ferr, "GENCAM: Paint Desc type [%s] not implemented at %ld\n", lp, ifp_linecnt);
      display_error++;
   }

   GENCAMPaint *p = new GENCAMPaint; 
   paintArray.Add(p);
   p->name = paintdesc_name;
   p->typ = fill;

   return 1;
}

/******************************************************************************
*/

bool CPaintArray::isFill(CString paintdesc)
{
	bool fill = false;

	for (int i = 0; i < paintArray.GetCount(); i++)
	{
		GENCAMPaint *p = paintArray.GetAt(i);
		if (p != NULL && p->name.CompareNoCase(paintdesc) == 0)
		{
			switch (p->typ)
			{
			case PAINT_VOID:
			case PAINT_HOLLOW:
				fill = false;
				break;

			case PAINT_FILL:
			case PAINT_MESH:
			case PAINT_HATCH:
				fill = true;
				break;

			default:
				fill = false;
				break;
			}
		}
	}

	return fill;
}

/******************************************************************************
* layer_layersingle
*/
int layer_layersingle()
{
   char *lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // layername
      return p_error();

   CString layername = lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // surface
      return p_error();

   CString surface = lp;

   if ((lp = gettok(NULL, ",;")) == NULL) // GENCAM_Layer_type
      return p_error();

   CString gencamtype = lp;

   // from here it is all optional
/*
   if ((lp = gettok(NULL,",;"))  // material

   if ((lp = gettok(NULL,",;")) == NULL)  // material_code
      return p_error();
   if ((lp = gettok(NULL,",;")) == NULL)  // thickness
      return p_error();
   if ((lp = gettok(NULL,",;")) == NULL)  // profile_desc_ref
      return p_error();
   if ((lp = gettok(NULL,",;")) == NULL)  // color_ref
      return p_error();
*/
   int laynr = Graph_Level(layername, "", 0);

   GENCAMLayer *l = new GENCAMLayer;
   layerArray.Add(l);
   l->name = layername;
   l->gencamtyp = gencamtype;
   l->surface = surface;

   return 1;
}

/******************************************************************************
* layer_layerswap
*/
int layer_layerswap()
{
   char *lp;
   CString swapname, l1_group, l1_ref, l2_group, l2_ref;
   int res;

   if ((lp = gettok(NULL, ",;")) == NULL)  // swapname
      return p_error();

   swapname = lp;

   if ((res = gettok_qualstring(NULL, ",;", &l1_group, &l1_ref)) == 0)       
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &l2_group, &l2_ref)) == 0)       
      return p_error();

   Graph_Level_Mirror(l1_ref, l2_ref, "");

   return 1;
}

/******************************************************************************
* pattern_padstackref
*/
int pattern_padstackref()
{
   CString padstack_group, padstack_ref;
   int res;
   char *lp;

   if ((res = gettok_qualstring(NULL, ",;", &padstack_group, &padstack_ref)) == 0)      // this is the padtsack ref
      return p_error();

   if ((lp = gettok(NULL, ",;")) == NULL)  // pinname
      return p_error();

   CString pinname = lp;

   double x, y;
	double rotation;
	int mirror;
   double scale;

   if (gettok_xform(NULL,",;",&x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   int layernr = -1;

   DataStruct  *data = Graph_Block_Reference(padstack_ref, pinname, get_groupnum(padstack_group), x, y, 
      rotation, mirror, scale, layernr, TRUE);

   data->getInsert()->setInsertType(insertTypePin);

   return 1;
}

/******************************************************************************
* fill_primitive
*/
static BlockStruct *fill_primitive(const char *primitive_group, const char *primitive_ref)
{
   //BlockStruct *block = Graph_Block_Exists(doc, primitive_ref, get_groupnum(primitive_group));
   BlockStruct* block = getGenCamReader().getBlock(primitive_ref,get_groupnum(primitive_group));

   if (block)
   {
      // loop and make all closed polys filled.
      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *np = block->getDataList().GetNext(pos);

         if (np->getDataType() != T_POLY)
				continue;

         CPoly *poly;
         POSITION polyPos = np->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            poly = np->getPolyList()->GetNext(polyPos);

            if (poly->isClosed()) poly->setFilled(true);
         }  
      }
   }

   return block;
}

/******************************************************************************
* padstack_pad
*/
int padstack_pad()
{
   CString  s1, s2;    
   CString  primitive_group, primitive_ref;
   CString  linedesc_group, linedesc_ref;
   CString  paintdesc_group, paintdesc_ref;
   CString  profiledesc_group, profiledesc_ref;
   CString  color_group, color_ref;
   double   x, y, rotation;
   int      res, mirror;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)    // this is the layer ref
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0) // primitive ref  
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &linedesc_group, &linedesc_ref)) == 0)   // linedesc ref, optional
   {
      linedesc_group = "";
      linedesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &paintdesc_group, &paintdesc_ref)) == 0) // paintdesc ref  
   {
      paintdesc_group = "";
      paintdesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &profiledesc_group, &profiledesc_ref)) == 0)   // profiledesc ref   
   {
      profiledesc_group = "";
      profiledesc_ref = "";
   }

   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)   // color ref   
   {
      color_group = "";
      color_ref = "";
   }

   double scale;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   int layernr = Graph_Level(s2, "", 0);

   // use aperture primitive if available
   CString padshapename;
   BlockStruct* primitiveBlock = getGenCamReader().getPrimitive(primitive_ref);

   if (primitiveBlock != NULL)
   {
      padshapename = primitiveBlock->getName();
   }
   else
   {
      // here make that primitive an aperture.
      padshapename.Format("CLX %s", primitive_ref);   
      BlockStruct *subblock = fill_primitive(primitive_group, primitive_ref);

      int grpnumAKAfilenum = get_groupnum(primitive_group);
      if (subblock != NULL)
         grpnumAKAfilenum = subblock->getFileNumber();

      Graph_Complex(grpnumAKAfilenum, padshapename, 0, primitive_ref, 0.0, 0.0, 0.0);
   }

   Graph_Block_Reference(padshapename, NULL, 0, x, y, rotation, mirror, scale, layernr, TRUE);

   return 1;
}

/******************************************************************************
* padstack_holeref
*/
int padstack_holeref()
{
   CString primitive_group, primitive_ref;
   double x, y, rotation;
   int res, mirror;

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0) // primitive ref  
      return p_error();

   double scale;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   Graph_Block_Reference(primitive_ref, NULL, get_groupnum(primitive_group), x, y, rotation, mirror, scale, -1, TRUE);

   return 1;
}

/******************************************************************************
* padstack_padref
*/
int padstack_padref()
{
   CString primitive_group, primitive_ref;
   double x, y, rotation;
   int res, mirror;

   if ((res = gettok_qualstring(NULL, ",;", &primitive_group, &primitive_ref)) == 0) // primitive ref  
      return p_error();

   double scale;

   if (gettok_xform(NULL, ",;", &x, &y, &rotation, &mirror, &scale) != 1) 
      return p_error();

   Graph_Block_Reference(primitive_ref, NULL, get_groupnum(primitive_group), x, y, rotation, mirror, scale, -1, TRUE);

   return 1;
}

/******************************************************************************
* pattern_artwork
*/
int pattern_artwork()
{
   CString s1, s2;    
   CString color_group, color_ref;
   double x, y, rotation;
   int res, mirror;
   int start_ident = cur_lineident;

   if ((res = gettok_qualstring(NULL, ",;", &s1, &s2)) == 0)    // this is the layer ref
      return p_error();

   if ((res = gettok_qualstring(NULL, ",;", &color_group, &color_ref)) == 0)      // this is the color
   {
      color_group = "";
      color_ref = "";
   }

   if (gettok_location(NULL, ",;", &x, &y, &rotation, &mirror) != 1) 
      return p_error();

   cur_layer = Graph_Level(s2,"",0);

   loop_command(artworkbuilder_lst, SIZ_ARTWORKBUILDER_LST, start_ident);

   return 1;
}

/******************************************************************************
* component_mechanical
*/
int component_mechanical()
{
   cur_comp.type = INSERTTYPE_MECHCOMPONENT;
   fprintf(ferr, "GENCAM: Not implemented [%s] at %ld\n", "component_device", ifp_linecnt);
   display_error++;

   return 1;
}

/******************************************************************************
* component_deviceref
*/
int component_deviceref()
{
   cur_comp.type = INSERTTYPE_PCBCOMPONENT;

   CString device_ref, device_group;
   int res;

   if ((res = gettok_qualstring(NULL, ",;", &device_group, &device_ref)) == 0)    
      return p_error();

   cur_comp.device_ref = device_ref;
   cur_comp.device_group = device_group;

   return 1;
}

/******************************************************************************
* component_patternref
   Pattern or Mechanical needs to be first after the component record.
*/
int component_patternref()
{
   CString  pattern_ref, pattern_group;
   int      res;

   if ((res = gettok_qualstring(NULL,",;", &pattern_group, &pattern_ref)) == 0)     
      return p_error();

   // here place component.
   
   int laynr = Graph_Level(cur_comp.layername, "", 0);
   cur_data = Graph_Block_Reference(pattern_ref, cur_comp.refdes, get_groupnum(pattern_group), 
         cur_comp.x, cur_comp.y, cur_comp.rotation, cur_comp.mirror, 1.0, laynr, TRUE);

   if (Is_Layer_Surface_Bottom(cur_comp.layername))
      cur_data->getInsert()->setPlacedBottom(true);
   else
      cur_data->getInsert()->setPlacedBottom(false);

   cur_comp.pattern_group = pattern_group;
   cur_comp.pattern_ref = pattern_ref;
   cur_data->getInsert()->setInsertType(cur_comp.type);

   return 1;
}

/******************************************************************************
* package_body
*/
int package_body()
{

   //fprintf(ferr,"GENCAM: Not implemented [%s] at %ld\n","package_body", ifp_linecnt);
   //display_error++;

   return 1;
}

/******************************************************************************
* component_keepout
*/
int component_keepout()
{
   fprintf(ferr, "GENCAM: Not implemented [%s] at %ld\n", "component_keepout", ifp_linecnt);
   display_error++;

   return 1;
}

/******************************************************************************
* package_pin
*/
int package_pin()
{
   //fprintf(ferr,"GENCAM: Not implemented [%s] at %ld\n","package_pin", ifp_linecnt);
   //display_error++;

   return 1;
}

/******************************************************************************
* go_fnull
*/
static int go_fnull(int start_ident)
{
   char  *lp;

   while (TRUE)
   {
      if (!push_tok)
      {
         if (get_nextcommand(ifp_line, MAX_LINE,ifp) == NULL)  
            return -1;
      }
      else
      {
         strcpy(buf, ifp_line);
      }

      strcpy(buf, ifp_line);

      if ((lp = gettok(ifp_line, ":")) == NULL) 
      {
         ErrorMessage("Bad line\n");

         continue;
      }

      if (!strcmp(lp, "$$"))
         return -2;

      if (!STRNICMP(lp, "$END", 4))
         return 1;

		// kill this if ident level is not needed anymore.
      if (cur_lineident <= start_ident)
      {
         push_tok = TRUE;

         return 2;
      }

      push_tok = FALSE;
   }

   return 0;
}

//_____________________________________________________________________________

CGenCamReader::CGenCamReader(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
, m_primitiveMap(nextPrime2n(200),false)
, m_settingsFilename(getApp().getImportSettingsFilePath("GenCam.In"))
{
   CString msg;
   msg.Format("\nGENCAM: Settings file [%s].\n", m_settingsFilename);
   getApp().LogMessage(msg);

	LoadSettings(m_settingsFilename);
}

BlockStruct* CGenCamReader::graphBlockOn(const CString& name,int fileNumber,BlockTypeTag blockType)
{
   BlockStruct* block = m_camCadDatabase.getDefinedBlock(name,blockType,fileNumber);

   Graph_Block_On(block);

   return block;
}

BlockStruct* CGenCamReader::getBlock(const CString& name,int fileNumber)
{
   BlockStruct* block = m_camCadDatabase.getBlock(name,fileNumber);

   return block;
}

BlockStruct* CGenCamReader::getPrimitive(const CString& primitiveName)
{
   BlockStruct* primitiveBlock = NULL;

   m_primitiveMap.Lookup(primitiveName,primitiveBlock);

   return primitiveBlock;
}

void CGenCamReader::defineCirclePrimitive(const CString& primitiveName,int fileNumber,double radius)
{
   BlockStruct* primitiveBlock = NULL;
   
   if (! m_primitiveMap.Lookup(primitiveName,primitiveBlock))
   {
      CStandardApertureCircle standardAperture(m_camCadDatabase.getPageUnits(),
         isEnglish(m_camCadDatabase.getPageUnits()) ? 0 : 2);

      standardAperture.setDiameter(radius * 2.);

      CString apertureName = "AP_" + standardAperture.getDescriptor();

      primitiveBlock = m_camCadDatabase.getBlock(apertureName);

      if (primitiveBlock == NULL)
      {
         primitiveBlock = m_camCadDatabase.getDefinedAperture(apertureName,
                             apertureRound,
                             standardAperture.getDimension(0));
      }

      m_primitiveMap.SetAt(primitiveName,primitiveBlock);
   }
}

void CGenCamReader::defineRectanglePrimitive(const CString& primitiveName,int fileNumber,double width,double height)
{
   BlockStruct* primitiveBlock = NULL;
   
   if (! m_primitiveMap.Lookup(primitiveName,primitiveBlock))
   {
      CStandardApertureRectangle standardAperture(m_camCadDatabase.getPageUnits(),
         isEnglish(m_camCadDatabase.getPageUnits()) ? 0 : 2);

      standardAperture.setWidth(width);
      standardAperture.setHeight(height);

      CString apertureName = "AP_" + standardAperture.getDescriptor();

      primitiveBlock = m_camCadDatabase.getBlock(apertureName);

      if (primitiveBlock == NULL)
      {
         primitiveBlock = m_camCadDatabase.getDefinedAperture(apertureName,
                             apertureRectangle,
                             standardAperture.getDimension(0),
                             standardAperture.getDimension(1));
      }

      m_primitiveMap.SetAt(primitiveName,primitiveBlock);
   }
}

void CGenCamReader::defineOvalPrimitive(const CString& primitiveName,int fileNumber,double width,double height)
{
   BlockStruct* primitiveBlock = NULL;
   
   if (! m_primitiveMap.Lookup(primitiveName,primitiveBlock))
   {
      CStandardApertureOval standardAperture(m_camCadDatabase.getPageUnits(),
         isEnglish(m_camCadDatabase.getPageUnits()) ? 0 : 2);

      standardAperture.setWidth(width);
      standardAperture.setHeight(height);

      CString apertureName = "AP_" + standardAperture.getDescriptor();

      primitiveBlock = m_camCadDatabase.getBlock(apertureName);

      if (primitiveBlock == NULL)
      {
         primitiveBlock = m_camCadDatabase.getDefinedAperture(apertureName,
                             apertureOblong,
                             standardAperture.getDimension(0),
                             standardAperture.getDimension(1));
      }

      m_primitiveMap.SetAt(primitiveName,primitiveBlock);
   }
}


void CGenCamReader::LoadSettings(CString settingsFileName)
{
   FILE *fp = fopen(settingsFileName, "rt");

   if (!fp)
   {
      CString msg;
      msg.Format("GENCAM: Open settings file [%s] failed.\n\n", settingsFileName);
      getApp().LogMessage(msg);

      return;
   }
              
   char line[255];

   while (fgets(line, 255, fp))
   {
      char *tok;

      if ((tok = get_string(line, " \t\n")) == NULL)
         continue;

      if (tok[0] != '.')
         continue;

      if (!STRICMP(tok, ".ATTRIBMAP"))
      {
         if ((tok = get_string(NULL, " \t\n")) == NULL)
            continue;

         CString a1 = tok;
         a1.MakeUpper();
         a1.TrimLeft();
         a1.TrimRight();

         if ((tok = get_string(NULL, " \t\n")) == NULL)
            continue;

         CString a2 = tok;
         a2.MakeUpper();
         a2.TrimLeft();
         a2.TrimRight();

			if (!a1.IsEmpty() && !a2.IsEmpty())
				AttribNameMap.SetAt(a1, a2);

      }
   }

   fclose(fp);
}

/****************************************************************************/




