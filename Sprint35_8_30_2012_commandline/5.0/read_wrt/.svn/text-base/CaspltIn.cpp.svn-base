// $Header: /CAMCAD/4.5/read_wrt/CaspltIn.cpp 11    11/01/05 2:35p Rick Faltersack $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-96. All Rights Reserved.
*/           

// all units are translated into mil
// need to implement textsize, textfont setup.

#include "stdafx.h"
#include "graph.h"
#include "geomlib.h"
#include "format_s.h"
#include <math.h>
#include "gauge.h"

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
} Textlist;

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

static   Poly    *polyline;
static   int       polycnt;

static   Figure  *figure;
static   int       figurecnt;  

static   Textlist   textlist[MAX_TEXT];

static   FILE   *ifp;
static   char   ifp_line[127];
static   long   ifp_linecnt = 0;

static   int       cur_layer;
static   int       cur_entity;

static   double    scale_factor = 1;
static   int       cur_filenum = 0;

static   int      units = 960; // 960 or 1000
static   int      wminx, wminy, wmaxx, wmaxy;
static   double   startx, starty;

/****************************************************************************/
/*
*/
static int  write_poly(int layer, double width,int fill)
{
    int i;
    int widthindex = 0;
    int err;

    if (polycnt == 0)   return 0;

    if (width == 0)
        widthindex = 0; // make it small width.
    else
        widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

    if (polycnt > 2)
    {
      Graph_PolyStruct(layer, 0, 0);
      int close = (polyline[0].x == polyline[polycnt-1].x && polyline[0].y == polyline[polycnt-1].y);
      Graph_Poly(NULL, widthindex, fill, 0, close);
      // here write lines.
      for (i=0;i<polycnt;i++)
         Graph_Vertex(polyline[i].x, polyline[i].y,0.0);
    }
    else
         Graph_Line(layer,
                  polyline[0].x, polyline[0].y,
                  polyline[1].x, polyline[1].y,  
                  0L, widthindex , FALSE); // index of width table

    polycnt = 0;
    return 0;
}

/****************************************************************************/
/*
   return inch
*/
static double   cnv_tok_x(char *l)
{
    double x;

    if (l && strlen(l))
    {
        x = atof(l);
    }
    else
        return 0.0;

    return x / units;   // makes it into inch
}

/****************************************************************************/
/*
   return inch
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

    return x / units;   // makes it into inch
}

/****************************************************************************/
/*
   return inch
*/
static double   cnv_tok_y(char *l)
{
    double y;

    if (l && strlen(l))
    {
        y = wmaxy - atof(l);
    }
    else
        return 0.0;

    return y / units;   // makes it into inch
}

/****************************************************************************/
/*
*/
static char *get_nextline(char *string,int n,FILE *fp)
{
   ifp_linecnt++;
   return fgets(string,n,fp);
}

/******************************************************************************
* ReadCASE_Plot
*/
void ReadCASE_Plot(const char *f, FormatStruct *Format, double scaleUnitsFactor)
{
   FileStruct  *file;
   int         ent;     // entity
   char        *lp;
   int         widthindex = 0;

   scale_factor = scaleUnitsFactor;

   if ((polyline = (Poly  *)calloc(MAX_POLY,sizeof(Poly))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   if ((figure = (Figure  *)calloc(MAX_FIGURE,sizeof(Figure))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   figurecnt = 0;

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(f,"rt")) == NULL)
   {
      ErrorMessage("Error open file", f);
      return;
   }

   file = Graph_File_Start(f, Type_CASE_Plot);
   cur_filenum = file->getFileNumber();      

   // check version
   get_nextline(ifp_line,127,ifp);
   if ((lp = strchr(ifp_line,'#')) == NULL)
   {
      CString     l;
      l.Format("CASE file Version statement #400 expected");
      ErrorMessage(l,"Wrong File Format", MB_OK | MB_ICONHAND);
      goto Exit;
   }
  
   // Units
   get_nextline(ifp_line,127,ifp);
   units = ent = atoi(ifp_line);

   textlist[0].height = 0.1;
   textlist[0].width  = 0.07;
   
   // Windows size size
   get_nextline(ifp_line,127,ifp);
   ent = sscanf(ifp_line,"%d %d %d %d",&wminx,&wmaxx,&wminy,&wmaxy);

   // here do a while loop
   while (get_nextline(ifp_line,127,ifp))
   {
      // first is the entity number
      char   lev[80];

      if ((lp = strtok(ifp_line," \t\n")) == NULL) continue;   // empty line
      if (!strlen(lp))                             continue;   // empty line
      if (lp[0] == '!')                            continue;   // remark  
      if (lp[0] == ';')                            continue;   // remark  

      switch (lp[0])
      {
         case 'C':
            lp = strtok(NULL," \t\n");
            sprintf(lev,"CASE_%s",lp);
            cur_layer = Graph_Level(lev,"", 0);
         break;
         case 'I':
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            startx = cnv_tok_x(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            starty = cnv_tok_y(lp);
         break;
         case 'V':
         {
            double x,y;
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            x = cnv_tok_x(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            y = cnv_tok_y(lp);
            Graph_Line(cur_layer,
                  startx, starty,x,y,
                  0L, 0 , FALSE); // index of width table
            startx = x;
            starty = y;
         }
         break;
         case 'A':
         {
            double x1,y1,x2,y2,x3,y3;
            double xc, yc, r, sa, da;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            x1 = cnv_tok_x(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            y1 = cnv_tok_y(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            x2 = cnv_tok_x(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            y2 = cnv_tok_y(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            x3 = cnv_tok_x(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            y3 = cnv_tok_y(lp);

            ArcPoint3(x1, y1, x2, y2, x3, y3, &xc, &yc, &r, &sa, &da);

            if (fabs(x1-x3) < 0.001 && fabs(y1 - y3) < 0.001)
               Graph_Circle(cur_layer, xc, yc, r, 0L, 0, 0, 0);
            else
               Graph_Arc(cur_layer, xc, yc, r, sa, da,   0L, 0, 0);
         }
         break;
         case 'W':
         {
            double x,y,w;
            int    err;
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            x = cnv_tok_x(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            y = cnv_tok_y(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            w = cnv_tok_x(lp);
            widthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

            Graph_Line(cur_layer,
                  startx, starty,x,y,
                  0L, widthindex , FALSE); // index of width table
            startx = x;
            starty = y;
         }
         break;
         case 'P':   // fill polyon
         {  
            int   p;
            if ((lp = strtok(NULL," \t\n")) == NULL) continue;
            p = atoi(lp);

            if (p >= MAX_POLY)
            {
               char     l[80];
               sprintf(l,"Too many Polylines %d at %ld",p,ifp_linecnt);
               ErrorMessage(l,"Error", MB_OK | MB_ICONHAND);
               for (int i=0;i<p;i++)
               {
                  get_nextline(ifp_line,127,ifp);
               }
               get_nextline(ifp_line,127,ifp);
               break;
            }

            polycnt = 0;  // always closed
            for (int i=0;i<p;i++)
            {
               get_nextline(ifp_line,127,ifp);
               lp = strtok(ifp_line," \t\n");      // x
               polyline[polycnt].x = cnv_tok_x(lp);
               lp = strtok(NULL," \t\n");    // y
               polyline[polycnt].y = cnv_tok_y(lp);
               polycnt++;
            }
            get_nextline(ifp_line,127,ifp);  // E 
            write_poly(cur_layer,0,TRUE);
         }
         break;
         case 'T':
         {
            double x,y,r;
            int    f;
            lp = strtok(NULL," \t\n"); // x
            x = cnv_tok_x(lp);
            lp = strtok(NULL," \t\n");    // x
            y = cnv_tok_y(lp);
            lp = strtok(NULL," \t\n");    // x
            r = atof(lp);
            get_nextline(ifp_line,127,ifp);
            f = ifp_line[0];
            lp = strtok(&(ifp_line[1]),"\n");
            Graph_Text(cur_layer, lp, x, y,
                  textlist[0].height, textlist[0].width, 
                  DegToRad(r), 0L,
                  1, 0,0, 0,0,0);
         }
         break;
         case 'S':
         {
            int      p;
            double   w;

            if ((lp = strtok(NULL," \t\n")) == NULL) continue; // angle ???
            if ((lp = strtok(NULL," \t\n")) == NULL) continue;
            p = atoi(lp);

            if (p >= MAX_POLY)
            {
               CString l;
               l.Format("Too many Polylines %d at %ld",p,ifp_linecnt);
               ErrorMessage(l,"Error", MB_OK | MB_ICONHAND);
               for (int i=0;i<p;i++)
               {
                  get_nextline(ifp_line,127,ifp);
               }
               get_nextline(ifp_line,127,ifp);
               break;
            }

            polycnt = 0;  // always closed
            polyline[polycnt].x = startx;
            polyline[polycnt].y = starty;
            polycnt++;

            for (int i=0;i<p;i++)
            {
               get_nextline(ifp_line,127,ifp);
               lp = strtok(ifp_line," \t\n");      // x
               polyline[polycnt].x = cnv_tok_x(lp);
               lp = strtok(NULL," \t\n");    // y
               polyline[polycnt].y = cnv_tok_y(lp);
               lp = strtok(NULL," \t\n");    // y
               w = cnv_tok(lp);
               polycnt++;
            }
            get_nextline(ifp_line,127,ifp);  // E 
            write_poly(cur_layer,w,FALSE);
         }
         break;
         case 'F':
            {  // Joanne - The following number after F is Aperture decode number and the number with D added in front will be the aperture name.
            int ApNum; 
            CString  ApertureName;
            lp = strtok(NULL," \t\n");  //Get Aperture Number
            ApNum = atoi(lp);
            ApertureName.Format("D%d",ApNum);
            Graph_Block_Reference(ApertureName, NULL, 0, startx, starty, 0.0, 0, 1.0, cur_layer, TRUE);
         }
         break;
         default:
         {
            // unknown entity
            CString     l;
            l.Format("Entity %s at %ld",lp,ifp_linecnt);
            ErrorMessage(l,"Unknown Entity", MB_OK | MB_ICONHAND);
         }
         break;
      }
   }

Exit:
   fclose(ifp);

   free(polyline);
   for (int i=0;i<figurecnt;i++)
      free(figure[i].name);
   free(figure);
}

// end CASPLIN.CPP
