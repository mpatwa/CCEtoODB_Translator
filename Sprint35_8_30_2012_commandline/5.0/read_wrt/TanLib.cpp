// $Header: /CAMCAD/4.5/read_wrt/TanLib.cpp 9     1/27/05 7:43p Kurt Van Ness $

/****************************************************************************
*
   Output for TAN-Files TANGO SERIES II

   all datas are in MILS
   all layers are in Numbers or Tangonames

   TOPLAYER          0
   BOTTOMLAYER       1
   POWERLAYER        2
   GROUNDLAYER       3
   MID1LAYER         4
   MID2LAYER         5
   MID3LAYER         6
   MID4LAYER         7
   TOPSILKLAYER      8
   BOTTOMSILKLAYER   9                    
   TOPSOLDERLAYER    10
   BOTTOMSOLDERLAYER 11
   TOPASSLAYER       12
   BOTTOMASSLAYER    13
   BOARDLAYER        14
   CONNECTLAYER      15
   DRILLLAYER        16
   KEEPOUTLAYER      17
   TITLELAYER        18
   MID5LAYER         19
   MID6LAYER         20
   MID7LAYER         21
   MID8LAYER         22
   ALL               -1 // this is for all pins
*/

#include "StdAfx.h"
#include "General.h"
#include "math.h"
#include "tanlib.h"

static FILE    *OFP = NULL;   /*Output Filepointer*/
static int     cur_layer = 1; //
static double  cur_width = -1;

/* The Functions and Procedures ...*/
static void init();
static void deinit();
static long cnv_tan(double inch);
static long cnv_tan_width(double inch);
static void sort_box(double *x1,double *y1,double *x2,double *y2);

/****************************************************************************/
/*
*/
long cnv_tan(double mils)
{
   return round(mils);
}

long cnv_tan_width(double width)
{
   if (width < 2)     width = 2;
   if (width > 250)   width = 250;
   return round(width);
}

void wtan_Graph_Width(double w)
{
   cur_width = w;
   return;
}

void init()
{
}
void deinit()
{
}

/* sort box for lowerleft, topright */
void sort_box(double *x1,double *y1,double *x2,double *y2)
{
   double   lx = ((*x1) < (*x2)) ? (*x1) : (*x2);
   double   ly = ((*y1) < (*y2)) ? (*y1) : (*y2);
   double   tx = ((*x1) > (*x2)) ? (*x1) : (*x2);
   double   ty = ((*y1) > (*y2)) ? (*y1) : (*y2);

   *x1 = lx;
   *y1 = ly;
   *x2 = tx;
   *y2 = ty;
}

// Tango allows layers from 0 = top to 22
void wtan_Graph_Level(const char *l)
{
   if (l == NULL) return;

   if (!isdigit(l[0]) && l[0] != '-')
   {
      // here are layer names
      if      (!STRCMPI(l,"TOPLAYER"))  cur_layer = 0;
      else if (!STRCMPI(l,"BOTTOMLAYER")) cur_layer = 1;
      else if (!STRCMPI(l,"POWERLAYER")) cur_layer = 2;
      else if (!STRCMPI(l,"GROUNDLAYER")) cur_layer = 3;
      else if (!STRCMPI(l,"MID1LAYER")) cur_layer = 4;
      else if (!STRCMPI(l,"MID2LAYER")) cur_layer = 5;
      else if (!STRCMPI(l,"MID3LAYER")) cur_layer = 6;
      else if (!STRCMPI(l,"MID4LAYER")) cur_layer = 7;
      else if (!STRCMPI(l,"TOPSILKLAYER")) cur_layer = 8;
      else if (!STRCMPI(l,"BOTTOMSILKLAYER")) cur_layer = 9;
      else if (!STRCMPI(l,"TOPSOLDERLAYER")) cur_layer = 10;
      else if (!STRCMPI(l,"BOTTOMSOLDERLAYER")) cur_layer = 11;
      else if (!STRCMPI(l,"TOPASSLAYER")) cur_layer = 12;
      else if (!STRCMPI(l,"BOTTOMASSLAYER")) cur_layer = 13;
      else if (!STRCMPI(l,"BOARDLAYER")) cur_layer = 14;
      else if (!STRCMPI(l,"CONNECTLAYER")) cur_layer = 15;
      else if (!STRCMPI(l,"DRILLLAYER")) cur_layer = 16;
      else if (!STRCMPI(l,"KEEPOUTLAYER")) cur_layer = 17;
      else if (!STRCMPI(l,"TITLELAYER")) cur_layer = 18;
      else if (!STRCMPI(l,"MID5LAYER")) cur_layer = 19;
      else if (!STRCMPI(l,"MID6LAYER")) cur_layer = 20;
      else if (!STRCMPI(l,"MID7LAYER")) cur_layer = 21;
      else if (!STRCMPI(l,"MID8LAYER")) cur_layer = 22;
      else if (!STRCMPI(l,"ALL")) cur_layer = -1; // this is for all pins
   }
   else
   {
      cur_layer = atoi(l);
      if (cur_layer < 0)   cur_layer = 0;
      if (cur_layer > 22)  cur_layer = 22;
   }
}

int wtan_Graph_Init()
{
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

void wtan_Graph_File_Open( FILE *file)
{
   OFP=file;
}

int wtan_Graph_Init_Layout()
{
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

void wtan_Graph_File_Open_Layout( FILE *file)
{
   OFP=file;
   fprintf(OFP,";Tango-PCB PLUS V2.00 Series II\n");
   fprintf(OFP,"VERSION 1 3 A\n");
   fprintf(OFP,"BOARD 0 0 0 0 0 0\n");
}

void wtan_Graph_File_Close_Layout()
{
   deinit();      /*deinitialize the Memory for the Layers ...*/
}

void wtan_Graph_Aperture(double x, double y, double sizea, double sizeb, int form, double rot)
{
   char  *f = "EL";
   int   drill = 0;
   char  *lay = "AL";

   if (cur_layer == 1)  lay = "TL";
   else
   if (cur_layer == 0)  lay = "BL";
   else
      drill = 1;

   if (sizea < 0.001)   sizea = 0.001;
   if (sizeb < 0.001)   sizeb = sizea;

   fprintf(OFP,"PAD %s %ld %ld %d",f,cnv_tan(sizea), cnv_tan(sizeb), drill);
   fprintf(OFP," %s NP \"\" %ld %ld 0 0 0",lay, cnv_tan(x), cnv_tan(y));
   fprintf(OFP,"\n");

   return;
}

void wtan_Graph_Circle( double center_x, double center_y, double radius, double width )
{
   wtan_Graph_Arc(center_x,center_y, radius,0.0, PI2,width );
   return;
}

/****************************************************************************
* wtan_Graph_Arc
*/
void wtan_Graph_Arc( double center_x, double center_y, double radius,
                 double startangle, double deltaangle, double width )
{
   int sa,da;

   if (deltaangle < 0)
   {
      sa = round(RadToDeg(startangle) + RadToDeg(deltaangle));
      da = -round(RadToDeg(deltaangle));
   }
   else
   {
      sa = round(RadToDeg(startangle));
      da = round(RadToDeg(deltaangle));
   }
   if (abs(da) < 1)  da = 1;  // min 1
   if (sa >= 360)  sa = sa - 360;
   if (sa < 0)     sa = sa + 360;       

   if (da > 360)   da = da - 360;
   if (da < 0)     da = da + 360;

   fprintf(OFP,"ARC %ld %d %d %ld %d %ld %ld 0 0 0\n", cnv_tan(radius),sa,da,
      cnv_tan_width(width),cur_layer,cnv_tan(center_x),cnv_tan(center_y));
}

/*****************************************************************************
* wtan_Graph_Line
*/
void wtan_Graph_Line( double x1, double y1, double x2, double y2, double width )
{
   long sx,sy,ex,ey;
   sx = cnv_tan(x1);
   sy = cnv_tan(y1);
   ex = cnv_tan(x2);
   ey = cnv_tan(y2);

   if (sx == ex && sy == ey)  return;  // eliminate  segments which are Zero (throu mil approx);

   wtan_Graph_Width(width);
   fprintf(OFP,"LINE %ld %d %ld %ld %ld %ld 0 0 0\n",
     cnv_tan_width(width),cur_layer,cnv_tan(x1),cnv_tan(y1),cnv_tan(x2),cnv_tan(y2));
   return;
}

void wtan_Graph_Text( char *text, double x, double y,
      double height, double width, double angle, int mirror )
{
   int   rot = round(RadToDeg(angle) / 90);
   double   x1,y1,x2,y2;

   if (strlen(text) == 0)  return;

   x1 = 0.0;y1 = 0.0;
   Rotate(height,strlen(text)*height*6.0/8.0,90.0*rot,&x2,&y2);
   sort_box(&x1,&y1,&x2,&y2);

   if (mirror) rot +=4;

   fprintf(OFP,"TEXT \"");

   for (unsigned int i=0;i<strlen(text);i++)
   {
      if (text[i] == '"')  fprintf(OFP,"\\");
      fprintf(OFP,"%c",text[i]);
   }

   fprintf(OFP,"\" %ld %d 10 %d %ld %ld %ld %ld %ld %ld 0 0\n",
      cnv_tan(height), rot, cur_layer,cnv_tan(x), cnv_tan(y),
      cnv_tan(x1+x), cnv_tan(y1+y), cnv_tan(x2+x), cnv_tan(y2+y) );
}

void wtan_Graph_Polyline( Point2 *coordinates, int pairs, int filled )
{
   int   i;

   if (filled)
   {
      fprintf(OFP,"POLY %d %d 0 0 0\n", cur_layer,pairs);
      for (i=0;i<pairs;i++)
      {
         fprintf(OFP,"PNT %1.0lf %1.0lf\n",coordinates[i].x,coordinates[i].y);
      }
   }
   else
   {
      for (i=1;i<pairs;i++)
      {
         wtan_Graph_Line( coordinates[i-1].x, coordinates[i-1].y,
                       coordinates[i].x,coordinates[i].y,cur_width ) ;
      }
   }
}

// end TANLIB.C

