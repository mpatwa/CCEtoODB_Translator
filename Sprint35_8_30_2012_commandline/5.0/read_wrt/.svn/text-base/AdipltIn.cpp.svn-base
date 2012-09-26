// $Header: /CAMCAD/4.5/read_wrt/AdipltIn.cpp 10    11/01/05 2:35p Rick Faltersack $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-96. All Rights Reserved.
*/           

// ADIPLTIN.CPP                  
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
* ReadAutocadADI
*/
void ReadAutocadADI(const char *f, FormatStruct *Format, double scaleUnitsFactor)
{
   FileStruct  *file;
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

   // check version
   get_nextline(ifp_line,127,ifp);
   if ((lp = strtok(ifp_line,",")) == NULL)
   {
      CString  l;
      l.Format("ADI file Version statement 1 expected");
      ErrorMessage(l,"Wrong File Format", MB_OK | MB_ICONHAND);
      return;
   }
   if (atoi(lp) != 1)
   {
      CString  l;
      l.Format("ADI file Version statement 1 expected");
      ErrorMessage(l,"Wrong File Format", MB_OK | MB_ICONHAND);
      return;
   }

   if ((lp = strtok(NULL,"\n")) == NULL)
   {
      CString  l;
      l.Format("ADI file Version statement 1,2 expected");
      ErrorMessage(l,"Wrong File Format", MB_OK | MB_ICONHAND);
      return;
   }
   if (atoi(lp) != 2)
   {
      CString  l;
      l.Format("ADI file Version statement 1,2 expected");
      ErrorMessage(l,"Wrong File Format", MB_OK | MB_ICONHAND);
      return;
   }

   file = Graph_File_Start(f, Type_Autocad_ADI);
   cur_filenum = file->getFileNumber();      

   // here do a while loop
   int eof_plot = FALSE;
   int penup = TRUE;
   int cur_layer = Graph_Level("1","", 0);
   
   while (!eof_plot && get_nextline(ifp_line,127,ifp))
   {
      // first is the entity number
      if ((lp = strtok(ifp_line," ,\t\n")) == NULL)   continue;   // empty line

      switch (atoi(lp))
      {
         case 1: // Begin
         break;
         case 2: // Logical end
            eof_plot = TRUE;
         break;
         case 3: // Move
            if ((lp = strtok(NULL,",")) == NULL)   continue;
            startx = cnv_tok_x(lp);
            if ((lp = strtok(NULL,",")) == NULL)   continue;
            starty = cnv_tok_y(lp);
            penup = TRUE;
         break;
         case 4: // Draw
         {
            if (penup)
            {
               int filled = 0;
               int closed = 0;
               int widthindex = 0;
               Graph_PolyStruct(cur_layer, 0l,FALSE);
               Graph_Poly(NULL,widthindex, 0,0,0);
            }
            double x,y;
            if ((lp = strtok(NULL,",")) == NULL)   continue;
            x = cnv_tok_x(lp);
            if ((lp = strtok(NULL,",")) == NULL)   continue;
            y = cnv_tok_y(lp);
            Graph_Vertex(x,y,0);
            penup = FALSE;
         }
         break;
         case 5: // New Pen
            if ((lp = strtok(NULL,",")) == NULL)   continue;
            cur_layer = Graph_Level(lp,"", 0);
         break;
         case 6: // Select Speed
         break;
         case 7: // Set line type
         break;
         case 8: // Pen change
         break;
         case 9: // Abort plot
            // unknown entity
            ErrorMessage("Abort Plot Command found", "", MB_OK | MB_ICONHAND);
            eof_plot = TRUE;
         break;
         case 10: // New pen width
         break;
         default:
         {
            // unknown entity
            CString  l;
            l.Format("Entity %s at %ld",lp,ifp_linecnt);
            ErrorMessage(l,"Unknown Entity", MB_OK | MB_ICONHAND);
         }
         break;
      }
   }

   fclose(ifp);

   free(polyline);
   for (int i=0;i<figurecnt;i++)
      free(figure[i].name);
   free(figure);
}

// end ADIPLTIN.CPP

