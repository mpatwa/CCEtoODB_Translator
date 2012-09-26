// $Header: /CAMCAD/4.6/read_wrt/GerbLib.cpp 15    9/29/06 5:03p Kurt Van Ness $

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include "gerblib.h"

#define  MAX_POLY 1000

static  FILE    *file;
static  int     cur_app;
static  int     digits;
static  double  decimalFactor;
static  int     units;
static  double  sectorradians;
static  int     circlemode = 0;
static  int     ppolycnt;
static  Point2  *ppoly;
static  Point2  last_vertex;
   
static void Load_Aperature(int app_num);
static void Pen_Down(const Point2& point);
static void Pen_Up(const Point2& point);
static void Pen_Flash(const Point2& point);

/********************************************************
*/
void Load_Aperature(int app_num)
{
   if (app_num > -1) // force aperture select
   {
      if (app_num == cur_app) return;

      fprintf(file, "G54D%d*\n", app_num);
   }

   cur_app = app_num;
   return;
}

/********************************************************
*/
void Pen_Down(const Point2& point)
{
   long x, y;

   x = lround (point.x * decimalFactor);
   y = lround (point.y * decimalFactor);

   fprintf(file, "X%ldY%ldD01*\n", x, y);
}

/********************************************************
*/
void Pen_Up(const Point2& point)
{
   long x, y;

   x = lround (point.x * decimalFactor);
   y = lround (point.y * decimalFactor);

   fprintf(file, "X%ldY%ldD02*\n", x, y);
}

/********************************************************
*/
void Pen_Flash(const Point2& point)
{
   long x, y;

   x = lround (point.x * decimalFactor);
   y = lround (point.y * decimalFactor);

   fprintf(file, "X%ldY%ldD03*\n", x, y);
}

/********************************************************
*/
FILE* wgerb_getFile()
{
	return file;
}

/********************************************************
*/
int wgerb_getCurrentAperture()
{
	return cur_app;
}

/********************************************************
*/
CPoint wgerb_convertPoint(const Point2& p)
{
   CPoint point;

   point.x = lround(p.x * decimalFactor);
   point.y = lround(p.y * decimalFactor);

   return point;
}

/********************************************************
*/
void wgerb_Pen_Down(const Point2& p)
{
	Pen_Down(p);
}

/********************************************************
*/
void wgerb_Pen_Up(const Point2& p)
{
	Pen_Up(p);
}

/********************************************************
*/
void wgerb_comment(const CString& comment)
{
   fprintf(file,"G04 %s*\n",comment);
}

/********************************************************
*/
void wgerb_Load_Aperture(int appnum)
{
   Load_Aperature(appnum);
}

/********************************************************
*/
void wgerb_Set_Format(FormatStruct *format)
{
   int decimal;

   digits        = format->GR.digits;
   decimal       = format->GR.decimal;
   units         = format->PortFileUnits;
   circlemode    = format->GR.circleMode;
   decimalFactor = pow(10.,decimal);

   if (format->GR.sectorizationAngle < 1)
      format->GR.sectorizationAngle = 1;

   if (format->GR.sectorizationAngle > 45)
      format->GR.sectorizationAngle = 45;
   
   sectorradians = DegToRad(format->GR.sectorizationAngle);
}

/********************************************************
*/
int wgerb_Graph_File_Open(const char *out_file)
{
   if ((file = fopen(out_file, "wt")) == NULL)
   {
      // error - can not open the file.
      return -1;
   }

   // here init buffer for Polyline
   if ((ppoly = (Point2 *)calloc(MAX_POLY,sizeof(Point2))) == NULL)
   {
      return 0;
      //exit(1);
   }
   ppolycnt = 0;

   cur_app = -1;
   return 1;
}

/********************************************************
*/
void wgerb_Graph_File_Close()
{
   fprintf(file, "M02*\n");
   fclose(file);

   // here free buffer for Polyline.
   free((char *)ppoly);
   return;
}

/********************************************************
*/
void wgerb_Flash_App(const Point2& point, int app_num)
{
   if (cur_app != app_num)
      Load_Aperature(app_num);
   Pen_Flash(point);
}

/********************************************************
*/
void wgerb_Line(Point2 start, Point2 end, int app_num,int apshape, double w )
{
   start.bulge = 0;
   end.bulge = 0;
   wgerb_Graph_Polyline(start,app_num, apshape,w);
   wgerb_Graph_Vertex(end);

   return;
}

/********************************************************
*/
void wgerb_Arc(const Point2& center, double radius, double startradians, 
                double deltaradians, int app_num, int cont)
{
   double   sx,sy, ex, ey;
   long x, y;
   Point2 p;
   
   if (cur_app != app_num)
      Load_Aperature(app_num);
   
   sx = cos(startradians) * radius + center.x;
   sy = sin(startradians) * radius + center.y;
   ex = cos(startradians+deltaradians) * radius + center.x;
   ey = sin(startradians+deltaradians) * radius + center.y;

   if (circlemode)
   {
        p.x = sx;
        p.y = sy;

        if (!cont)   Pen_Up(p);     // only pen up if not continue
        fprintf(file,"G75*"); 

        x = lround (ex * decimalFactor);
        y = lround (ey * decimalFactor);
        if (deltaradians < 0)
            fprintf(file,"G02X%ldY%ld",x,y);
        else
            fprintf(file,"G03X%ldY%ld",x,y);

        x = lround ((center.x-sx) * decimalFactor);
        y = lround ((center.y-sy) * decimalFactor);

        fprintf(file,"I%ldJ%ldD01*",x,y);
        fprintf(file,"G74*G01*\n");
   }
   else
   {
      ppolycnt = MAX_POLY;
      // start center
      ArcPoly2(sx,sy,center.x,center.y,deltaradians,
            1.0,ppoly,&ppolycnt,sectorradians);
      int i;
      
      last_vertex.bulge = 0;  // _Vertex is actin on this

      if (cont)   // continues without starting a new Poly
      {
         for (i=0;i<ppolycnt;i++)
            wgerb_Graph_Vertex(ppoly[i]);
      }
      else
      {
         wgerb_Graph_Polyline(ppoly[0],app_num,T_ROUND,0.0);
         for (i=1;i<ppolycnt;i++)
            wgerb_Graph_Vertex(ppoly[i]);
      }
   }

   return;
}

/********************************************************
*/
void wgerb_Circle(const Point2& center, double radius, int app_num)
{
   double   sx,sy;
   Point2   p;
   long     x,y;

   if (cur_app != app_num)
      Load_Aperature(app_num);
   
   if (circlemode)
   {
        p.x = center.x-radius;
        p.y = center.y;

        Pen_Up(p);
        fprintf(file,"G75*\n");  

        x = lround ((p.x) * decimalFactor);
        y = lround ((p.y) * decimalFactor);
        fprintf(file,"G03X%ldY%ld",x,y);

        x = lround ((radius) * decimalFactor);
        fprintf(file,"I%ldJ0D01*G74*G01*\n",x);
   }
   else
   {
      ppolycnt = MAX_POLY;

      sx = radius + center.x;
      sy = center.y;

      // start center
      ArcPoly2(sx,sy,center.x,center.y,PI2,
            1.0,ppoly,&ppolycnt,sectorradians);
      int i;
      wgerb_Graph_Polyline(ppoly[0],app_num,T_ROUND,0.0);

      for (i=1;i<ppolycnt;i++)
         wgerb_Graph_Vertex(ppoly[i]);
   }
   return;
}

// this just writes out a text into the output file
void wgerb_write2file(const char *prosa)
{
   fprintf(file,"%s",prosa);
   return;
}

void wgerb_write(const char* format,...)
{
   va_list args;
   va_start(args,format);

   vfprintf(file,format,args);
}

// appnum of 0 does not write an aperture.
void wgerb_Graph_Polyline(const Point2& p, int app_num, int apshape, double width)
{
   if (app_num > 0)
   {
      if (cur_app != app_num)
         Load_Aperature(app_num);
   }
   // only first and last koo needs to new adjusted
   Pen_Up(p);
   // what happens if a bulge here
   last_vertex = p;
   return;
}

void wgerb_Graph_Vertex(const Point2& p)
{
   double da = atan(last_vertex.bulge) * 4;

   if (fabs(da) <= SMALLANGLE)
   {
      Pen_Down(p);
   }
   else
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(last_vertex.x,last_vertex.y,p.x,p.y,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      Point2 c;
      c.x = cx; c.y = cy;
      wgerb_Arc(c, r, sa, da, cur_app,TRUE);
   }
   last_vertex = p;
   return;
}
// end GERBLIB.C
 

