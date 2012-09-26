// $Header: /CAMCAD/4.6/read_wrt/CadpltIn.cpp 11    4/24/07 5:01p Rick Faltersack $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-96. All Rights Reserved.
*/           

// all units are in MIL          

#include "stdafx.h"
#include "CCEtoODB.h"
#include "graph.h"
#include "geomlib.h"
#include "format_s.h"
#include <math.h>
#include "gauge.h"
#include "RwUiLib.h"
extern CProgressDlg *progress;

#define MAX_POLY     2000
#define MAX_TEXT     16
#define  MAX_FIGURE  1000
    
/****************************************************************************/
/*
*/

typedef struct
{
   double   x,y;
   int      fill;    // 0 not
                  // 1 yes
} Poly;

typedef struct
{
   char     *name;
} Figure;

typedef struct
{
    double  height;
    double  width;  // total char width
    double  space;
    int     fontnumber;
    double  slant;
} TextDef;

typedef struct
{
   unsigned short       justification:2;
   unsigned short       unused:5;
   unsigned short       mirror:1;
   unsigned short       textid:8;
}Textparam;

/****************************************************************************/
/*
   Global Commands
*/

static  Poly    *polyline;
static  int     polycnt;

static   Figure  *figure;
static   int       figurecnt;  

static  FILE    *ifp;
static  char    ifp_line[127];
static  long    ifp_linecnt = 0;

static  int     cur_layer;
static  int     cur_entity;

static  double  scale_factor = 1;
static  int     cur_filenum = 0;

static FILE *logfp = NULL;

/****************************************************************************/
/*
*/
static int  write_poly(int layer, double width, int fill)
{
   int i;
   int widthindex = 0;
   int err;

   if (polycnt == 0) return 0;

   if (width == 0)
      widthindex = 0;   // make it small width.
   else
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0,0, BL_WIDTH, 0, &err);

   if (polycnt > 2)
   {
      Graph_PolyStruct(layer,0L,0);
      int   close = (polyline[0].x == polyline[polycnt-1].x && polyline[0].y == polyline[polycnt-1].y);
      Graph_Poly(NULL,widthindex, 0,0,close);
      // here write lines.
      for (i=0;i<polycnt;i++)
         Graph_Vertex(polyline[i].x, polyline[i].y,0.0);
   }
   else
      Graph_Line(layer, polyline[0].x, polyline[0].y,
                  polyline[1].x, polyline[1].y,  
                  0L, widthindex , FALSE); // index of width table

   polycnt = 0;
   return 0;
}

/****************************************************************************/
/*
*/
static double   cnv_tok(char *l)
{
    double x;

    if (l && strlen(l))
    {
        x = atof(l);
    }
    else
        return 0.0;

    // we need this for rounding
    x = floor(x * scale_factor * 1000000.0 + 0.5) / 1000000.0;

    return x;
}

/****************************************************************************/
/*
*/
static int figure_defined(char *name)
{
   int    i;

   for (i=0;i<figurecnt;i++)
   {
     if (!strcmp(figure[i].name,name))
        return 1;

   }

   if (figurecnt < MAX_FIGURE)
   {
     if ((figure[figurecnt].name = STRDUP(name)) == NULL)
       MemErrorMessage(__FILE__, __LINE__);
     figurecnt++;
   }
   else
   {
     // too many defined
   }
   return 0;
}

/****************************************************************************/
/*
*/
static char *get_nextline(char *string,int n,FILE *fp)
{
    ifp_linecnt++;
    return fgets(string,n,fp);
}

/****************************************************************************/
/*
         Graph_PolyStruct(layer,
                  polycnt-1, fill, 0L,// polycnt must be number of line segments, not points
                  widthindex, // index of width table
                  FALSE);
         Graph_Line(layer,
                  polyline[i-1].x, polyline[i-1].y,
                  polyline[i].x, polyline[i].y,  
                  0L, widthindex , FALSE); // index of width table
          Graph_Circle(cur_layer, xc, yc, rad, 0L, widthindex, 0, 0);


*/
static int do_figure(int figcode, double width, double height, double x, double y, int lay)
{
   int    widthindex = 0;
   int    res = 0, err;
   char   figname[80];

   switch (figcode)
   {
     case 1:  // point
     {
       sprintf(figname,"Point_%1.3lf",width);
       do
       {
         Graph_Aperture(figname, T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         if (err)
            sprintf(figname,"Point_%1.3lf_%d",width,++res);
       }while (err);
       Graph_Block_Reference(figname, NULL, cur_filenum,x, y,0.0,
                  0, 1.0, lay, TRUE);
     }
     break;
     case 2:  // circle
     {
       sprintf(figname,"Circle_%1.3lf",width);
       do
       {
         Graph_Aperture(figname, T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         if (err)
            sprintf(figname,"Circle_%1.3lf_%d",width,++res);
       }while (err);
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     }
     break;
     case 3:  // Octagon
       sprintf(figname,"Octagon_%1.3lf", width);
       do
       {
         Graph_Aperture(figname, T_OCTAGON, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         if (err)
            sprintf(figname,"Octagon_%1.3lf_%d",width,++res);
       }while (err);
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 4:  // Cross
       sprintf(figname,"Cross_%1.3lf_%1.3lf",width,height);
       if (!figure_defined(figname))
       {
         Graph_Block_On(GBO_APPEND,figname,cur_filenum, BL_GLOBAL);
         Graph_Line(lay,-width/2,0.0,width/2,0.0,
                  0L, widthindex , FALSE); // index of width table
         Graph_Line(lay,0.0,-height/2,0.0,height/2,
                  0L, widthindex , FALSE); // index of width table
         Graph_Block_Off();
       }
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 5:  // square
       sprintf(figname,"Square_%1.3lf", width);
       do
       {
         Graph_Aperture(figname, T_SQUARE, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         if (err)
            sprintf(figname,"Square_%1.3lf_%d",width,++res);
       }while (err);
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 6:  // rectangle
       sprintf(figname,"Rectangle_%1.3lf_%1.3lf",width,height);
       do
       {
         Graph_Aperture(figname,T_RECTANGLE, width, height,0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         if (err)
            sprintf(figname,"Rectangle_%1.3lf_%1.3lf_%d",width,height,++res);
       }while (err);
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 7:  // diamond18
       sprintf(figname,"Diamond_%1.3lf_%1.3lf",width,height);
       if (!figure_defined(figname))
       {
         // here define it
         Graph_Block_On(GBO_APPEND,figname,cur_filenum,BL_GLOBAL);

         Graph_PolyStruct(lay,0, FALSE);
         Graph_Poly(NULL,widthindex,  0,0,1);
         Graph_Vertex(-width/2,0.0,0.0);
         Graph_Vertex(0.0,height/2,0.0);
         Graph_Vertex(width/2,0.0,0.0);
         Graph_Vertex(0.0,-height/2,0.0);
         Graph_Vertex(-width/2,0.0,0.0);

         Graph_Block_Off();
       }
       Graph_Block_Reference(figname, NULL,  cur_filenum,x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 8:  // DRC error flag
       Graph_Aperture("DRC", 0, 0.0, 0.0,0.0, 0.0, 0.0, figcode, 
            BL_APERTURE, FALSE, &err);
       Graph_Block_Reference("DRC", NULL,  cur_filenum,x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 9:  // antipad
       Graph_Aperture("Antipad", 0, 0.0, 0.0,0.0, 0.0, 0.0, figcode, 
            BL_APERTURE, FALSE, &err);
       Graph_Block_Reference("Antipad", NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 10: // Thermal pad
       Graph_Aperture("Thermal", 0, 0.0, 0.0,0.0, 0.0, 0.0, figcode, 
            BL_APERTURE, FALSE, &err);
       Graph_Block_Reference("Thermal", NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;  
     case 11: // oblong x
     case 12: // oblong y
       sprintf(figname,"Oblong_%1.3lf_%1.3lf",width,height);
       do
       {
         Graph_Aperture(figname, T_OBLONG, width, height,0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         if (err)
            sprintf(figname,"Oblong_%1.3lf_%1.3lf_%d",width,height,++res);
       }while (err);
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 13: // butterfly x
     case 14: // butterfly y
       Graph_Aperture("Butterfly", 0, 0.0, 0.0,0.0, 0.0, 0.0, figcode, 
            BL_APERTURE, FALSE, &err);
       Graph_Block_Reference("Butterfly", NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 15: // Hexagon x
     case 16: // Hexagon y
       sprintf(figname,"Hexagon_%1.3lf_%1.3lf",width,height);
       if (!figure_defined(figname))
       {
         double s = sin(DegToRad(30));
         // here define it
         Graph_Block_On(GBO_APPEND,figname,cur_filenum, BL_GLOBAL);

         Graph_PolyStruct(lay, 0, FALSE);
         Graph_Poly(NULL,widthindex, 0,0,1);
         Graph_Vertex(-width/2,0.0,0.0);
         Graph_Vertex(-width*s/2,height/2,0.0);
         Graph_Vertex(width*s/2,height/2,0.0);
         Graph_Vertex(width/2,0.0,0.0);
         Graph_Vertex(width*s/2,-height/2,0.0);
         Graph_Vertex(-width*s/2,-height/2,0.0);
         Graph_Vertex(-width/2,0.0,0.0);

         Graph_Block_Off();
       }
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 17: // testpoint
     {
       sprintf(figname,"TestPoint_%1.3lf",width);
       do
       {
         Graph_Aperture(figname, T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         if (err)
            sprintf(figname,"TestPoint_%1.3lf_%d",width,++res);
       }while (err);
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     }
     break;
     case 18: // triangle 1
       sprintf(figname,"Triangle1_%1.3lf_%1.3lf",width,height);
       if (!figure_defined(figname))
       {
         // here define it
         Graph_Block_On(GBO_APPEND,figname,cur_filenum, BL_GLOBAL);

         Graph_PolyStruct(lay, 0, FALSE);
         Graph_Poly(NULL, widthindex,0,0,1);
         Graph_Vertex(-width/2,-height/2,0.0);
         Graph_Vertex(0.0,height/2,0.0);
         Graph_Vertex(width/2,-height/2,0.0);
         Graph_Vertex(-width/2,-height/2,0.0);

         Graph_Block_Off();
       }
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 19: // triangle 2
       sprintf(figname,"Triangle2_%1.3lf_%1.3lf",width,height);
       if (!figure_defined(figname))
       {
         // here define it
         Graph_Block_On(GBO_APPEND,figname, cur_filenum, BL_GLOBAL);

         Graph_PolyStruct(lay,0,FALSE);
         Graph_Poly(NULL,widthindex,0,0,0);
         Graph_Vertex(-width/2,-height/2,0.0);
         Graph_Vertex(0.0,height/2,0.0);
         Graph_Vertex(width/2,-height/2,0.0);
         Graph_Vertex(-width/2,-height/2,0.0);
         Graph_Circle(lay,0.0,0.0,width/2,
                  0L, widthindex , 0,0); // index of width table

         Graph_Block_Off();
       }
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 20: // triangle 3
       sprintf(figname,"Triangle3_%1.3lf_%1.3lf",width,height);
       if (!figure_defined(figname))
       {
         // here define it
         Graph_Block_On(GBO_APPEND,figname, cur_filenum, BL_GLOBAL);

         Graph_PolyStruct(lay, 0, FALSE);
         Graph_Poly(NULL, widthindex, 0,0,1);
         Graph_Vertex(-width/2,-height/2,0.0);
         Graph_Vertex(0.0,height/2,0.0);
         Graph_Vertex(width/2,-height/2,0.0);
         Graph_Vertex(0.0,-height/2,0.0);
         Graph_Vertex(-width/2,-height/2,0.0);
         Graph_Block_Off();
       }
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     case 21: // triangle 4
       sprintf(figname,"Triangle4_%1.3lf_%1.3lf",width,height);
       if (!figure_defined(figname))
       {
         // here define it
         Graph_Block_On(GBO_APPEND,figname,cur_filenum, BL_GLOBAL);

         Graph_PolyStruct(lay,0,FALSE);
         Graph_Poly(NULL,widthindex, 0,0,1);
         Graph_Vertex(-width/2,-height/2,0.0);
         Graph_Vertex(0.0,height/2,0.0);
         Graph_Vertex(width/2,-height/2,0.0);
         Graph_Vertex(0.0,-height/2,0.0);
         Graph_Vertex(-width/2,-height/2,0.0);
         Graph_Block_Off();
       }
       Graph_Block_Reference(figname, NULL, cur_filenum, x, y,0.0,
                  0, 1.0, lay, TRUE);
     break;
     default:
     {
       // unknown entity
       fprintf(logfp, "Unknown Figure %d at %ld\n", figcode, ifp_linecnt);
       fflush(logfp);
     }
     break;
   }  // end switch

   return 1;
}

/****************************************************************************/
/*
*/
static void format_error()
{
   CString  l;
   l.Format("File format invalid. Please check Format");
   ErrorMessage(l,"Format Error", MB_OK | MB_ICONHAND);

   return;
}

/******************************************************************************
* ReadCADENCE_Plot
*/
void ReadCADENCE_Plot(const char *f, FormatStruct *Format, double scaleUnitsFactor)
{
   FileStruct *file;
   int text_size_cnt;
   int ent;    // entity
   char    *lp;

   CTypedPtrArrayContainer<TextDef*> textdeflist;

   scale_factor = scaleUnitsFactor;
   ifp_linecnt = 0;

   if ((polyline = (Poly  *)calloc(MAX_POLY,sizeof(Poly))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   if ((figure = (Figure  *)calloc(MAX_FIGURE,sizeof(Figure))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   figurecnt = 0;

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(f,"rt")) == NULL)
   {
      ErrorMessage(f, "Can not open input file",MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // Open the log file
   CString logFile = GetLogfilePath("AllegroIPF.log");

   if ((logfp = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      ErrorMessage(logFile, "Can not open Logfile", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   file = Graph_File_Start(f, Type_CADENCE_Plot);
   cur_filenum = file->getFileNumber();     
   // here get headerline Drawing size x1 y1 x2 y2 
   get_nextline(ifp_line,127,ifp);

   // Window size
   get_nextline(ifp_line,127,ifp);

   // Number of Text size
   get_nextline(ifp_line,127,ifp);
   if ((lp = strtok(ifp_line," \t\n")) == NULL)
   {
      format_error();
      return;
   }

   text_size_cnt = atoi(lp);

   // here loop throu all textsizes
	int i=0; 
   for (i=0; i<text_size_cnt; i++)
   {
      TextDef *td = new TextDef;
      textdeflist.Add(td);

      get_nextline(ifp_line,127,ifp);
      if ((lp = strtok(ifp_line," \t\n")) == NULL)   // height
      {
         format_error();
         return;
      }
      td->height = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)       // width
      {
         format_error();
         return;
      }
      td->width  = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)       // space
      {
         format_error();
         return;
      }
      td->space  = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)       // fontnumber
      {
         format_error();
         return;
      }
      td->fontnumber = atoi(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)       // slant
      {
         format_error();
         return;
      }
      td->slant = atof(lp);
   }

   // here do a while loop
   while (get_nextline(ifp_line,127,ifp))
   {
      // first is the entity number
      char    lev[80];


      if ((lp = strtok(ifp_line," \t\n")) == NULL)    break;      // end of file
      if (!strlen(lp))                                continue;   // empty line

      ent = atoi(lp);

      sprintf(lev,"IPF_%d",ent & 0xff);
      cur_layer = Graph_Level(lev,"", 0);
      cur_entity = ((ent & 0xff00) >> 8);

      switch (cur_entity)
      {
      case    1: // line
         {
            int vertexcnt;
            char    *l;
            double  width;

            get_nextline(ifp_line,127,ifp);
            // here are width and vertexcnt
            lp = strtok(ifp_line," \t\n");    // width
            width = cnv_tok(lp);
            lp = strtok(NULL," \t\n");
            vertexcnt = atoi(lp);
            get_nextline(ifp_line,127,ifp);
            polycnt = 0;
            l = ifp_line;
            // here now get all vertex entities
            while (polycnt < vertexcnt)
            {
               while ((lp = strtok(l," \t\n")) == NULL)
               {
                  get_nextline(ifp_line,127,ifp);
                  l = ifp_line;
               }
               l = NULL;
               if (polycnt < MAX_POLY)
               {
                  polyline[polycnt].x = cnv_tok(lp);
                  lp = strtok(NULL," \t\n");    // y
                  polyline[polycnt].y = cnv_tok(lp);
                  polycnt++;
               }
               else
               {
                  fprintf(logfp, "Too many Polylines %d at %ld\n", vertexcnt, ifp_linecnt);
                  fflush(logfp);
               }
            }
            write_poly(cur_layer,width,0);
         }
         break;
      case    2:  // arc
         {
            int     widthindex, err;
            double  width;
            double  xc,yc,rad,sa,ea;

            get_nextline(ifp_line,127,ifp); // need to implement
            lp = strtok(ifp_line," \t\n");  // width
            width = cnv_tok(lp);
            lp = strtok(NULL," \t\n");      // xc
            xc = cnv_tok(lp);
            lp = strtok(NULL," \t\n");      // yc
            yc = cnv_tok(lp);
            lp = strtok(NULL," \t\n");      // rad
            rad = cnv_tok(lp);
            lp = strtok(NULL," \t\n");      // start angle
            sa = atof(lp);
            lp = strtok(NULL," \t\n");      // end angle
            ea = atof(lp);

            if (sa > ea)    // if start id at 350 degree and end is at 10 degree  
               ea += PI2;

            if (width == 0)
               widthindex = 0; // make it small width.
            else
               widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

            if (fabs(fabs(ea-sa) - PI2) < 0.000001)
               Graph_Circle(cur_layer, xc, yc, rad, 0L, widthindex, 0, 0);
            else
               Graph_Arc(cur_layer, xc, yc, rad, sa, (ea - sa),
               0L, widthindex, 0);
         }
         break;
      case    3:  // text
         {
            double         x, y, rot;
            unsigned int   charcnt;
            Textparam      param;
            unsigned short dummy;     // must be same size as Textparam

            get_nextline(ifp_line,127,ifp);
            lp = strtok(ifp_line," \t\n");      // Param_Code
            dummy = atoi(lp);
            memcpy(&param,&dummy,sizeof(Textparam));
            lp = strtok(NULL," \t\n");          // Rotation in Radians
            rot = atof(lp);
            lp = strtok(NULL," \t\n");          // X
            x = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // Y
            y = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // Count : numver of text char
            charcnt = atoi(lp);
            lp = strtok(NULL,"\n");             // String

            if (charcnt != strlen(lp))
            {
               // unknown entity
               fprintf(logfp, "Char count error at %ld\n", ifp_linecnt);
               fflush(logfp);
            }
            else
            {
               double xoff = 0,yoff = 0;
               if (param.justification)
               {
                  double  x,y;
                  // justification 0 = left
                  //               1 = center
                  //               2 = right
                  switch (param.justification)
                  {
                  case 1:
                     xoff = (textdeflist.GetAt(param.textid-1)->width * charcnt ) /2;
                     break;
                  case 2:
                     xoff = (textdeflist.GetAt(param.textid-1)->width * charcnt );
                     break;
                  default:
                     break;
                  }
                  if (param.mirror)
                     xoff = -xoff;
                  Rotate(xoff,yoff,RadToDeg(rot),&x,&y);
                  xoff = x;
                  yoff = y;
               }
               Graph_Text(cur_layer, lp, x-xoff, y-yoff,
                  textdeflist.GetAt(param.textid-1)->height, textdeflist.GetAt(param.textid-1)->width, 
                  rot, 0L,
                  1, (char)param.mirror, (int)(textdeflist.GetAt(param.textid-1)->slant), 
                  0,0, 0);
            }
         }
         break;
      case    4:  // filled rectangle
         {
            // filled rectange
            double  x1,y1,x2,y2;
            get_nextline(ifp_line,127,ifp); // 
            lp = strtok(ifp_line," \t\n");
            x1 = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // y
            y1 = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // y
            x2 = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // y
            y2 = cnv_tok(lp);
            polyline[0].x = x1;
            polyline[0].y = y1;
            polyline[1].x = x2;
            polyline[1].y = y1;
            polyline[2].x = x2;
            polyline[2].y = y2;
            polyline[3].x = x1;
            polyline[3].y = y2;
            polyline[4].x = x1;
            polyline[4].y = y1;
            polycnt = 5;
            write_poly(cur_layer,0.0,TRUE);
         }
         break;
      case    5:  // figures
         {
            int   figcode;
            double    width, height, x,y;

            get_nextline(ifp_line,127,ifp); // 
            lp = strtok(ifp_line," \t\n");
            figcode = atoi(lp);
            lp = strtok(NULL," \t\n");          // width
            width = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // height
            height = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // x
            x = cnv_tok(lp);
            lp = strtok(NULL," \t\n");          // y
            y = cnv_tok(lp);
            do_figure(figcode,width,height,x,y,cur_layer);
         }
         break;
      case    6:  // polygons
         {
            // Case dts0100386720
            // This entity was previously unsupported, treated as "unknown entity".
            // Implementation here is from reverse engineering of data sample, not from any spec.
            // Yes, that  makes it a guess, and proven only on a sample of one.

            get_nextline(ifp_line, 127, ifp); 
            lp = strtok(ifp_line, " \t\n");
            int vertexcnt = atoi(lp);
            lp = strtok(NULL, " \t\n");
            double probablywidth = cnv_tok(lp);

            // If there are too many points, report it now.
            // Read all the lines anyway, otherwise parsing gets out of sync.
            // Gather as many of the points as fits.

            if (vertexcnt > MAX_POLY)
            {
               fprintf(logfp, "Too many points in poly,  %d at %ld\n", vertexcnt, ifp_linecnt);
               fflush(logfp);
            }

            for (polycnt = 0; polycnt < vertexcnt; polycnt++)
            {
               get_nextline(ifp_line,127,ifp);
               lp = strtok(ifp_line, " \t\n");
               double x = cnv_tok(lp);
               lp = strtok(NULL, " \t\n");
               double y = cnv_tok(lp);
               // there is one more datum on the line, don't know what it is, ignore it for now

               if (polycnt < MAX_POLY)
               {
                  polyline[polycnt].x = x;
                  polyline[polycnt].y = y;
               }
            }

            write_poly(cur_layer, probablywidth, 0);
         }
         break;

      default:
         {
            // unknown entity
            fprintf(logfp, "Unknown entity %d at %ld (%d | %d)\n", cur_entity, ifp_linecnt, ent, ifp_linecnt);
            fflush(logfp);
         }
         break;
      }
   }

   fclose(ifp);
   fclose(logfp);

   free(polyline);
   for (i=0;i<figurecnt;i++)
      free(figure[i].name);
   free(figure);
}

// end CADPLIN.CPP
