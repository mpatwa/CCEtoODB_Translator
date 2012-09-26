// $Header: /CAMCAD/4.5/read_wrt/KongsIn.cpp 12    5/23/05 10:56a Lynn Phung $

/*****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/ 

#include <stdafx.h>
#include <stdio.h>
#include <math.h>

#include "graph.h"
#include "geomlib.h"
#include "format_s.h"
#include "txtscan.h"
#include "gauge.h"
#include "attrib.h"
#include "ee.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;

/* Defines *****************************************************************/


#define  MAX_LINE          8000     /* Max line length */
#define  MAX_MACRO         10000    // max 9999 apertures
#define  MAX_AP_RENAME     10000    

#define  PENUP             2
#define  PENDOWN           1
#define  PENFLASH          3

// macro operations
#define  OP_ASSIGN         0
#define  OP_ADD            1
#define  OP_MINUS          2
#define  OP_DIV            3
#define  OP_MUL            4

// G4 comment type
#define  G4TYPE_UNKNOWN    0
#define  G4TYPE_GERBTOOL   1

/* Structures **************************************************************/

typedef struct       // ap_rename structure is used, if 2 or more gerber files have
                     // same dcode and different values. Happens only on Gerber_X files
{
   char  *name;      // new name
   char  *orig;      // original name
}Ap_Rename;

typedef struct  // gerber point with bulge
{
   double   x,y;
   double   bulge;
}GPoint;
typedef CArray<GPoint, GPoint&> GPointArray;

typedef  struct
{
   double   cur_x,cur_y;
   double   old_x,old_y;
   double   cur_i,cur_j;
   int      lpd;        // true is dark, false is lpc
   int      cur_gcode;
   int      cur_app;
   int      cur_widthindex;
   int      nach_komma;
   int      absolute;
   int      drawmode;   /* 1 = pendown */
                        /* 2 = penup */
                        /* 3 = flash */
                        /* 4 = used as vias */
   int      G36fill;       // Gerber autofill
   int      arc_interpolation;
   int      counterclock;
   int      G75code;
   int      GerberX;        // read applist inside gerber file.
   int      layernum;
   char     done;
   char     libmode;
   int      apr_units;
   long     sequencecnt;
   int      lpcnt;            // counts the number of LPC or LPD commands 
   int      width0index;
   int      parstart, apstart;
} Gerberlocal;


/* Globals *****************************************************************/
static   Gerberlocal G;

static   CCEtoODBDoc  *doc;
static   FILE        *fp;

static   double      cnv_tok(char *);
static   int         cont_flag = FALSE;
static   void        do_kongsberg(const char *fname);
static   void        write_db(const char *layername,int forceout);
static   int         get_macroptr(char *m);

static   double      GBscale, OriginalGBscale;

static   GPointArray gpolyarray;
static   int         gpolycnt = 0;

static   Ap_Rename   *ap_rename;
static   int         ap_renamecnt = 0;

static   int         pageUnits; 
static   double      unitscale = 1;
static   CString     last_draw_ap_name;
static   CString     lastcharstring;
static   int         macro_thermals;

static   CString     cur_layername;
static   int         cur_negative = FALSE;

static   CString     cur_app_name;
static   CString     prefix;

static   int         G4type;
static   CString     G4comment;
static   CString     G4key;
static   int         ATTACH_G4COMMENT;

static   char        token[MAX_LINE+1];   // MAX_LINE

static   int         cur_filenum = 0;

static   int         display_error, display_log;
static   FILE        *flog;


static   FileStruct  *file;               // this is static because on multiple file 
                                          // load I need to remember the last file.

/****************************************************************************/
/*
   a read \t in 2 char but a '\t' char is 0x09
*/
static   void  change_to_controlcode(char *l)
{
   char  tmp[200];
   unsigned int   i;
   int   tmpcnt = 0;

   for (i=0;i<strlen(l);i++)
   {
      if (l[i] == '\\')
      {
         // here start of control char
         i++;
         if (l[i] == 'a')
         {
            tmp[tmpcnt] = '\a';
            tmpcnt++;
         }
         else
         if (l[i] == 'b')
         {
            tmp[tmpcnt] = '\b';
            tmpcnt++;
         }
         else
         if (l[i] == 'f')
         {
            tmp[tmpcnt] = '\f';
            tmpcnt++;
         }
         else
         if (l[i] == 'n')
         {
            tmp[tmpcnt] = '\n';
            tmpcnt++;
         }
         else
         if (l[i] == 'r')
         {
            tmp[tmpcnt] = '\r';
            tmpcnt++;
         }
         else
         if (l[i] == 't')
         {
            tmp[tmpcnt] = '\t';
            tmpcnt++;
         }
         else
         if (l[i] == 'v')
         {
            tmp[tmpcnt] = '\v';
            tmpcnt++;
         }
         else
         if (l[i] == '\\')
         {
            tmp[tmpcnt] = '\\';
            tmpcnt++;
         }
      }
      else
      {
         tmp[tmpcnt] = l[i];        
         tmpcnt++;
      }
   }
   tmp[tmpcnt] = '\0';
   strcpy(l,tmp);
   return;
}

/****************************************************************************/
/*
*/
void read_kongsberg(const char *path_buffer,CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits,
                 int cur_filecnt, int tot_filecnt)
{                 
   int         i;
   char drive[_MAX_DRIVE]; 
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   doc = Doc;  
   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(f,fname);
   strcat(f,ext);

   display_error = FALSE;
   display_log   = FALSE;

   CString logFile = GetLogfilePath("kongsberg.log");
   if (cur_filecnt > 0)
   {
      if ((flog = fopen(logFile, "a+t")) == NULL)  // rewrite file
      {
         ErrorMessage(logFile, "Can not open Logfile", MB_ICONEXCLAMATION | MB_OK);
         return;
      }
   }
   else
   {
      if ((flog = fopen(logFile, "a+t")) == NULL)  // rewrite file
      {
         ErrorMessage(logFile, "Can not open Logfile !", MB_ICONEXCLAMATION | MB_OK);
         return;
      }
      file = Graph_File_Start(f, Type_Gerber);
      cur_filenum = file->getFileNumber();
   }
   
   pageUnits = PageUnits;

   cur_negative = FALSE;
   G.lpd = TRUE;
   G.parstart = FALSE;
   G.apstart = FALSE;
   prefix = Format->prefix;

   // metrix - mil is done in scaleUnitsFactor.
   OriginalGBscale = GBscale = Format->Scale;

   if (Format->GR.units == 0)
      unitscale = Units_Factor(UNIT_INCHES, PageUnits);
   else
      unitscale = Units_Factor(UNIT_MM, PageUnits);

   G.nach_komma = 3;
   G.absolute = !Format->GR.type;

   for (int ii=0;ii<G.nach_komma;ii++)
      GBscale /=10;

   G.G75code = Format->GR.circleMode; // 0=Sectorize 1=360° circular interpolation - switch off by G74

   if ((fp = fopen(path_buffer,"rb")) == NULL)
   {
      ErrorMessage("Error open file","Error");
      return;
   }

   gpolyarray.SetSize(100,100);
   gpolycnt = 0;

   if ((ap_rename = (Ap_Rename *) calloc(MAX_AP_RENAME,sizeof(Ap_Rename))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   ap_renamecnt = 0;

   G.width0index = -1;  // initialize to -1 (not defined)

   G.layernum = Graph_Level(f, "", 0); // makes every gerber file a different color
   G.lpcnt = 0;
   G.layernum = -1;
   do_kongsberg(f);

   fclose(fp);

   gpolyarray.RemoveAll();
   gpolycnt = 0;

   for (i=0;i<ap_renamecnt;i++)
   {
      if (ap_rename[i].name)  free(ap_rename[i].name);
      if (ap_rename[i].orig)  free(ap_rename[i].orig);
   }
   free(ap_rename);

   fclose(flog);

   if (display_error && display_log && cur_filecnt == (tot_filecnt-1))
      Logreader(logFile);

   return;
}

/****************************************************************************
*
*/
static void rename_app(const char *newname, const char *orgname)
{
   if (ap_renamecnt < MAX_AP_RENAME)
   {
      if ((ap_rename[ap_renamecnt].name = STRDUP(newname)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      if ((ap_rename[ap_renamecnt].orig = STRDUP(orgname)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      ap_renamecnt++;
   }
   else
   {
      ErrorMessage("Too many aperture renames");
      // error - too many ap_renames - so rename is ignored.
   }
}

/****************************************************************************
*
   Do not overwrite an existing aperture
*/
static int Check_Aperture(const char *name, int apptyp, double sizea, double sizeb, 
                          double xc, double yc, double rot,
                          int  appcode, DbFlag flg)
{
   int   code, err;
   CString  tmp;
   int   errcnt = 0;
   int   changed = FALSE;
   int   dcode = appcode;

   tmp = name;

   while (TRUE)
   {
      code = Graph_Aperture(tmp, apptyp, sizea, sizeb, xc, yc, rot, dcode, flg, FALSE, &err);
      if (err)
      {
         tmp.Format("%s_%d",name,++errcnt);
         changed = TRUE;
         dcode = 0;
      }
      else
         break;
   }

   if (changed)   // update ap_rename list
   {
      rename_app(tmp, name);
   }

   return code;
}

/****************************************************************************
*
   Do not overwrite an existing aperture
*/
static int Check_ComplexAperture(const char *name, int apptyp, double sizea, double sizeb, 
                          double xc, double yc, double rot,
                          int  appcode, DbFlag flg)
{
   int   code, err;
   CString  tmp;
   int   errcnt = 0;
   int   changed = FALSE;
   int   dcode = appcode;

   tmp = name;

   while (TRUE)
   {
      code = Graph_Aperture(tmp, apptyp, sizea, sizeb, xc, yc, rot, dcode, flg, FALSE, &err);
      if (err)
      {
         tmp.Format("%s_%d",name,++errcnt);
         changed = TRUE;
         dcode = 0;
      }
      else
         break;
   }

   if (changed)   // update ap_rename list
   {
      rename_app(tmp, name);
   }

   return code;
}

/****************************************************************************
* 
*/
static void get_ap_rename_name(char *orig,int *app)
{
   int   i;

   for (i=0;i<ap_renamecnt;i++)
   {
      if (!strcmp(ap_rename[i].orig,orig))
      {
         strcpy(orig,ap_rename[i].name);
         *app = 0;
         return;
      }
   }

   return;
}

/****************************************************************************
*
*/
static int Kongsberg_Read_Token( FILE *text_file, char *token, int max_length )
{
   int   res, cnt = 0, ncnt = 0;
   char  c;
   token[0] = '\0';

   lastcharstring = "";
   while ((res = Read_Char(text_file,&c)) == NORMAL_TEXT)
   {
      lastcharstring += Get_ReadCharString();

      if (cnt < max_length-1) // still get the last char,
      {
         token[cnt] = c;
         cnt++;
      }
      else
      {
         //ErrorMessage("Token length Error","Error in Gerber Read");
         token[cnt] = c;
         cnt++;
         res = -99;
         break;
      }
   }
   lastcharstring += Get_ReadCharString();

   token[cnt] = '\0';

   return res;
}

/****************************************************************************
* 
*/
static void do_blockend()
{
   // this are ignore codes
   if (G.cur_gcode != 4   &&     // remark
       G.cur_gcode != 28  &&     // remark
       G.cur_gcode != 53  &&     // remark
       G.cur_gcode != 59  &&     // remark
       G.cur_gcode != 54)        // tool select
   {
      write_db(cur_layername,0);
      G.old_x = G.cur_x;
      G.old_y = G.cur_y;
   }
   else
   {
      // I must keep G code for Arc and others
      // but not remark or tool select
      G.cur_gcode = 0;
   }
   G.cur_i = 0.0;
   G.cur_j = 0.0;
   G.sequencecnt++;
}

/****************************************************************************
* 
*/
static void make_comment()
{
   switch (G4type)
   {
      case G4TYPE_GERBTOOL:
      {
         if (G4comment.Left(2) == "_C")
         {
            G4key = "COMMENT";
            G4comment.Delete(0,2);
            G4comment.TrimLeft();
            G4comment.TrimRight();
         }
         else
         if (G4comment.Left(2) == "_U")
         {
            // G4_U plt plate_base:*
            G4comment.Delete(0,2);
            G4comment.TrimLeft();
            G4comment.TrimRight();
            char  *lp, *t = STRDUP(G4comment);
            G4key = "USER";
            if (lp = strtok(t," \t"))
            {
               CString  k1;
               k1 = lp;
               if (lp = strtok(NULL," \t"))
               {
                  G4key = k1;
                  G4key.MakeUpper();
                  G4comment = lp;
               }
            }
            free(t);
         }
         else
         if (G4comment.Left(2) == "_N")
         {
            G4key = ATT_NETNAME;
            G4comment.Delete(0,2);
            G4comment.TrimLeft();
            G4comment.TrimRight();
         }
      }
      break;
      default:
         G4key = "COMMENT";
      break;
   }

   if (G4comment.Right(1) == '*' || G4comment.Right(1) == '$')
   {
      G4comment.Delete(strlen(G4comment)-1,1);
   }
   return;
}

/*****************************************************************************/
/* 
   Do all scan
*/
void do_kongsberg(const char *fname)
{
   char     lastch;
   int      res, lastres;
   int      steprep = FALSE;

   Set_Comment( "%" , "@" );
   Set_Text_Delimeter( "\001" );
   Set_Word_Delimeters( "@XYMPD" ); 
   Set_White_Characters( "" );   // white char will terminate the token.
   Set_Non_Characters( "\t\r\a\b\v\n" );
   //Set_Non_Characters( "\t \r\a\b\v\n" );
   Set_Linecnt(0L);
   rewind(fp);

   G4comment = "";
   G4key = "COMMENT";
   lastch = ' ';                        
   G.drawmode = PENUP;
   G.done = TRUE;
   G.arc_interpolation = FALSE;
   G.sequencecnt = 0;
   G.cur_app = 10;   // initialize to default. gerberfile may start with nothing
   cur_app_name.Format("%sD%d", prefix, G.cur_app);
   last_draw_ap_name = cur_app_name;
   G.cur_x = 0.0;
   G.cur_y = 0.0;

   // just in case
   cur_layername = fname;

   while ((res = Kongsberg_Read_Token(fp,token, MAX_LINE)) != EOF_CHARACTER)
   {
      lastres = res; // res -99 is that token was longer than MAX_LINE, which only should happen in a Macro 
                     // definition;
#ifdef _DEBUG
      CString test_tok = token;  // for debugging
#endif
      if (res == STOP_TEXT) // this is happening when a file start %AMxxx
      {
         lastch = Delimeter();
      }

      switch(lastch)
      {
         case  '%':
         {
            // needs to be written and saved 
            write_db(cur_layername,1);  // force out
            G4comment = "";
            gpolyarray.RemoveAll();
            gpolycnt = 0;   // this must be 0, no continuation.
            G.old_x = G.cur_x;
            G.old_y = G.cur_y;

            CString  macrotoken;
            macrotoken = token;

         }
         break;
         case  'G':
         break;   // case G
         case  'X':
            G.done = FALSE;
            G.cur_x = cnv_tok(token);
         break;
         case  'Y':
            G.done = FALSE;
            G.cur_y = cnv_tok(token);
         break;
         case '@':
            do_blockend();
         break;
/*
               // write force out everything before a new aperture is selected
               write_db(cur_layername,1);

               char  t[80];
               G.cur_app = atoi(token);
               cur_app_name.Format("%sD%d", prefix, G.cur_app);

               strcpy(t,cur_app_name); // here I do not know how to change a CString in a function
               get_ap_rename_name(t,&G.cur_app);   
               cur_app_name = t;
               // name will be overwritten, if changed and G.cur_app is set to Zero.

               G.cur_widthindex = Graph_Aperture(cur_app_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, G.cur_app,
                     BL_APERTURE, FALSE, &err); // ensure that aperature is in database
               cont_flag = FALSE;
               G.drawmode = PENUP;
            }
*/
         case  'D':
         {
            // D01 is plot, D02 is move, D03 is flash.
            int   dmode = atoi(token);

            if (dmode == PENFLASH && G.drawmode != PENFLASH)
            {
               // write force out everything before a flash is done.
               write_db(cur_layername,2); // force out = 2
               G.done = FALSE;   // a flash always flashes the current position.
            }
            G.drawmode = dmode;
         }
         break;
         case 'M':
            G.done = TRUE;
         break;
         default:
            // there can be other unknown commands, GerberX etc...
         break;
      }
     lastch = Delimeter();
   }

   // write out the last one, if one exists
   write_db(cur_layername,1);

   // close step and repeat block
   if (steprep)   Graph_Block_Off();

   return;
}

/****************************************************************************/
/*
*/
double   cnv_tok(char *t)
{
   double   x;
       
   x = atof(t);
   return(x*GBscale*unitscale);
}

/****************************************************************************/
/*
   PENDOWN  =  1
   PENUP    =  2
*/
void write_db(const char *layername,int forceout)
{
   int   err;
   int   fill = G.G36fill;

   // also write polylines if there is an arc
   if (((forceout && G.drawmode == PENDOWN) || (G.drawmode != PENDOWN)) && gpolycnt > 1)
   {
      int widthIndex = Graph_Aperture(last_draw_ap_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, G.cur_app,
            BL_APERTURE, FALSE, &err);

      // if current mode is pen down and a force, the gpolyarray needs
      // to be updated
      // This is incorrect, cause if it comes from a flash force, cur_xy is the flash position.
      if (forceout == 1 && G.drawmode == PENDOWN)
      {
         GPoint   p;

         p = gpolyarray.ElementAt(gpolycnt-1);  // always greater 1

         // do not repeat is no coo has changed
         if (p.x != G.cur_x || p.y != G.cur_y)
         {
            p.x = G.cur_x;
            p.y = G.cur_y;
            p.bulge = 0;
            gpolyarray.SetAtGrow(gpolycnt,p);
            gpolycnt++;
         }
      }

      // according to CAM350, the fill is uses a zero line
      if (fill) 
      {
         if (G.width0index < 0)
         {
            int err;
            G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
         }
         widthIndex = G.width0index; // if a G36 is detected, the polyline gets a 0 width boundary.
      }
      if (gpolycnt && G.layernum < 0)
         G.layernum = Graph_Level(layername, "", 0); // makes every gerber file a different color

      GPoint   p1,p2;
      p1 = gpolyarray.ElementAt(0);
      p2 = gpolyarray.ElementAt(gpolycnt-1);

      int   close = ((p1.x == p2.x) && (p1.y == p2.y));

      if ((gpolycnt == 2) && close)
      {
         // shutter on/off on same koo is a flash
         Graph_Aperture(last_draw_ap_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, G.cur_app, 
            BL_APERTURE, FALSE, &err);
         // must be filenum 0, because apertures are global.
         DataStruct *d = Graph_Block_Reference(last_draw_ap_name, NULL, 0, p1.x, p1.y, 0.0,
                               0, 1.0, G.layernum, TRUE);
         if (ATTACH_G4COMMENT)
         {
            if (strlen(G4comment))
            {
               doc->SetUnknownAttrib(&d->getAttributesRef(),G4key, G4comment, SA_OVERWRITE, NULL);
               G4comment = "";
            }
         }
      }
      else
      {
         Graph_PolyStruct(G.layernum,  0, cur_negative);
         Graph_Poly(NULL,widthIndex, fill, 0,close);

         GPoint   lastp;
         for (int i=0;i<gpolycnt;i++)
         {
            GPoint   p;
            p = gpolyarray.ElementAt(i);

            if (i && p.x == lastp.x && p.y == lastp.y && p.bulge == lastp.bulge)
               continue;

            Graph_Vertex(p.x, p.y, p.bulge);
            lastp = p;
         }
      }
      // remember the last koo i.e
      // x1y1PENDOWN
      // PENUP
      // x2y2PENDOWN
      
      p1.x = G.cur_x;
      p1.y = G.cur_y;
      p1.bulge = 0;
      gpolyarray.SetAtGrow(0,p1);
      gpolycnt = 1;
      G.done = TRUE;
   }

   if (G.done) return;     /* already written */

   if (G.layernum < 0)
      G.layernum = Graph_Level(layername, "", 0); // makes every gerber file a different color

   switch (G.drawmode)
   {
      case PENDOWN: 
      {
         double bulge = 0;
         if (G.arc_interpolation)
         {
            double rad, sa, da;
         
            // G.cur_gcode 3, 30, 31 counter clock
            // G.cur_gcode 2, 20, 21 clock wise
            // ArcCenter2 is always counter clock from old to cur
            ArcCenter2(G.old_x, G.old_y, G.cur_x, G.cur_y,G.cur_i, G.cur_j,
                       &rad, &sa, &da, 0);

            if (G.cur_gcode == 2 || G.cur_gcode == 20 || G.cur_gcode == 21)
            {
               // clockwise arc
               da = da - PI2;
            }
            else
            {
               // counter clockwise arc
            }

            if (gpolycnt < 1)
            {
               // error !!!!!
               fprintf(flog,"Arc error !!!\n");
               display_error++;
            }
            else
            {
               if (da == 0 || fabs(da) == PI2)
               {
                  // here is circle
                  GPoint   p;
                  p = gpolyarray.ElementAt(gpolycnt-1);
                  p.bulge = 1;
                  gpolyarray.SetAt(gpolycnt-1,p);

                  // only for debug !
                  // find if center is left from x, down, right, up
                  //double x1,y1,x2,y2,xc,yc;
                  //x1 = G.old_x;
                  //y1 = G.old_y;
                  //x2 = G.cur_x;
                  //y2 = G.cur_y;
                  //xc = G.cur_i;
                  //yc = G.cur_j;

                  double   distx, disty;
                  distx = G.old_x - G.cur_i;
                  disty = G.old_y - G.cur_j;

                  p.x = G.cur_x - 2*distx;
                  p.y = G.cur_y - 2*disty;
                  p.bulge = 1;
                  gpolyarray.SetAtGrow(gpolycnt,p);
                  gpolycnt++;
               }
               else
               {
                  bulge = tan(da / 4);
                  GPoint   p;
                  p = gpolyarray.ElementAt(gpolycnt-1);
                  p.bulge = bulge;
                  gpolyarray.SetAt(gpolycnt-1,p);
               }
            }
            bulge = 0;
         }
         // here update gpolycnt with next entry, because a polyline can follow
         // an arc
         GPoint p;
         p.x = G.cur_x;
         p.y = G.cur_y;
         p.bulge = bulge;
         gpolyarray.SetAtGrow(gpolycnt,p);
         gpolycnt++;
         last_draw_ap_name = cur_app_name; // remember this aperture name when on next aperture and drawing this one
         G.done = TRUE;
         //G.arc_interpolation = FALSE;
      }
      break;
      case PENFLASH:
      {
         if (cur_negative)
         {
            fprintf(flog,"Flash a Negative Aperture ???\n");
            display_error++;
         }

         Graph_Aperture(cur_app_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, G.cur_app, 
            BL_APERTURE, FALSE, &err);

         // must be filenum 0, because apertures are global.
         DataStruct *d = Graph_Block_Reference(cur_app_name, NULL, 0, G.cur_x, G.cur_y,0.0,
                               0, 1.0, G.layernum, TRUE);
         if (ATTACH_G4COMMENT)
         {
            if (strlen(G4comment))
            {
               doc->SetUnknownAttrib(&d->getAttributesRef(),G4key, G4comment, SA_OVERWRITE, NULL);
               G4comment = "";
            }
         }
         // always store last position. Because after a flash a
         // pendown can happen.
         GPoint p1;
         p1.x = G.cur_x;
         p1.y = G.cur_y;
         p1.bulge = 0;
         gpolyarray.SetAtGrow(0,p1);
         gpolycnt = 1;
         G.done = TRUE;
         //G.drawmode = PENUP;
      }
      break;
      case PENUP:
      {
         // this is penup
         cont_flag = FALSE;

         GPoint p1;
         p1.x = G.cur_x;
         p1.y = G.cur_y;
         p1.bulge = 0;
         gpolyarray.SetAtGrow(0,p1);
         gpolycnt = 1;
         last_draw_ap_name = cur_app_name; // remember this aperture name when on next aperture and drawing this one
         G.drawmode = PENUP;
      }
      break;
   }    // switch
   return;
}

// end KONGSIN.CPP
