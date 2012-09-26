// $Header: /CAMCAD/4.5/read_wrt/BarcoLib.cpp 9     1/27/05 7:43p Kurt Van Ness $

/****************************************************************************
   BARCOLIB.C
*/

#include "StdAfx.h"
#include "General.h"
#include <math.h>
#include <stdio.h>
#include "barcolib.h"

#define  MAX_POLY 1000

static   FILE     *file;
static   long     acnt = 1000;
static   int      ppolycnt;
static   Point2   *ppoly;
static   int      cnt;
static   Point2   lastpoly;

static void Load_Aperture(int app_num);

/********************************************************
*/
void Load_Aperture(int app_num)
{
   if (cnt)    fprintf(file, "\n");
   cnt = 0;
   fprintf(file, "A%d\n", app_num);
   return;
}

/********************************************************
*/
int wbarco_Graph_File_Open(const char *out_file)
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
   cnt = 0;
   return 1;
}

/********************************************************
*/
void wbarco_Graph_File_Close()
{
   fprintf(file, "; end of file\n");
   fclose(file);

   // here free buffer for Polyline.
   free((char *)ppoly);
   return;
}

/********************************************************
*/
void wbarco_Flash_App(Point2 point, int app_num)
{
   Load_Aperture(app_num);
   fprintf(file,"F%1.4lf,%1.4lf\n",point.x, point.y);
   cnt = 0;
}

/********************************************************
*/
void wbarco_Line(Point2 start, Point2 end, int app_num,int apshape, double w )
{
   Load_Aperture(app_num);
   fprintf(file,"M%1.4lf,%1.4lfD%1.4lf,%1.4lf\n",start.x,start.y,end.x,end.y);
   cnt = 0;
   return;
}

/********************************************************
*/
void wbarco_Arc(Point2 center, double radius, double startradians, 
                double deltaradians, int app_num)
{
   double   sx,sy, ex, ey;
   
   sx = cos(startradians) * radius + center.x;
   sy = sin(startradians) * radius + center.y;
   ex = cos(startradians+deltaradians) * radius + center.x;
   ey = sin(startradians+deltaradians) * radius + center.y;

   Load_Aperture(app_num);
   fprintf(file,"M%1.4lf,%1.4lfC%1.4lf,%1.4lf,%1.4lf,%1.4lf,CCW\n",
      sx,sy,ex,ey,center.x,center.y);
   cnt = 0;
   return;
}

/*****************************************************************************/
/*
*/
void wbarco_Circle(Point2 center, double radius, int app_num)
{
   wbarco_Arc(center, radius, 0.0, 2*PI,app_num);
   return;
}

// this just writes out a text into the output file
void wbarco_write2file(const char *prosa)
{
   fprintf(file,"%s",prosa);
   return;
}

/*****************************************************************************/
/*
*/
void xwbarco_Graph_Polyline(Point2 *p, int cnt, int app_num, int apshape, double width,int filled)
{
   int   i;

   Load_Aperture(app_num);

   if (filled)
      fprintf(file,"A%ld=CONTOUR ",++acnt);

   cnt = fprintf(file,"M%1.4lf,%1.4lf",p[0].x,p[0].y);
   // only first and last koo needs to bew adjusted
   for (i=1;i<cnt;i++)
   {
      if (cnt > 80)  
         cnt = fprintf(file,"\n");

      cnt += fprintf(file,"D%1.4lf,%1.4lf",p[i].x,p[i].y);
   }
   fprintf(file,"\n");
   cnt = 0;
   return;
}

/*****************************************************************************/
/*
*/
int wbarco_Graph_Text( char *text, double x1, double y1,
                        double height, double width, double angle, 
                        int mirror, int oblique)
{
   unsigned int   i;

   fprintf(file,"A%ld=TEXT,(\"",++acnt);
   for (i=0;i<strlen(text);i++)
   {
      if (text[i] == '"')  fprintf(file,"\"");
      fprintf(file,"%c",text[i]);
   }
   fprintf(file,"\"),%1.4lf,%1.4lf,R=%d",height,width,
      round(RadToDeg(angle)));
   if (mirror)
      fprintf(file,",M=Y");
   fprintf(file," F%1.4lf,%1.4lf\n",x1,y1);
   cnt = 0;
   return 1;
}

/*****************************************************************************/
/*
*/
int wbarco_Graph_Vertex( Point2 p)
{
   double da = atan(lastpoly.bulge) * 4;

   if (cnt > 80)  
   {
      fprintf(file,"\n");
      cnt = 0;
   }

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy, r, sa;
      ArcPoint2Angle(lastpoly.x,lastpoly.y,p.x,p.y,da,&cx,&cy,&r,&sa);

      cnt += fprintf(file,"C%1.4lf,%1.4lf,%1.4lf,%1.4lf",
         p.x,p.y,cx,cy);
      if (da > 0)
         cnt +=fprintf(file,",CCW ");
      else
         cnt +=fprintf(file,",CW ");
   }
   else
      cnt+=fprintf(file,"D%1.4lf,%1.4lf",p.x,p.y);
   
   lastpoly.x = p.x;
   lastpoly.y = p.y;
   lastpoly.bulge = p.bulge;

   return 1;
}

/*****************************************************************************/
/*
*/
int wbarco_Graph_Polyline( Point2 p, int closed, int filled, int appnum, int apshape)
{
   Load_Aperture(appnum);

   if (cnt) fprintf(file,"\n");

   cnt = 0;
   if (filled)
      cnt += fprintf(file,"A%ld=CONTOUR ",++acnt);

   cnt+=fprintf(file,"M%1.4lf,%1.4lf",p.x,p.y);
   lastpoly.x = p.x;
   lastpoly.y = p.y;
   lastpoly.bulge = p.bulge;

#ifdef WOLF
   // We break acute angled, wide polys because AutoCAD connects 
   // and creates a pointed vertex that extends well beyond vertex
   // if Dxf_Linewidth >= realwidth && angle < 90 degree break polyline and do not close.
   int firstVertex = 0, i = 1, j;
   BOOL DrawPolyNow = FALSE;
   double a, b, c;
   while (i < cnt)
   {
      if (0) // if user wants to keep polys together no matter what, go in here
      {
         i = cnt-1;
         DrawPolyNow = TRUE;
      }

      // SPECIAL CASE:
      // check angle between first and last vertex if still closed
      // break this angle by setting closed to FALSE
      if (closed  &&  i == cnt-1  &&  Dxf_Linewidth >= realwidth  &&  !DrawPolyNow)
      {
         a = LengthPoint2(&coos[cnt-1], &coos[0]);
         b = LengthPoint2(&coos[0], &coos[1]);
         c = LengthPoint2(&coos[cnt-1], &coos[1]); // length of triangle leg opposite angle we are calculating
      
         // Law of Cosines -> cos C = (a^2 + b^2 - c^2) / 2ab > 0 then C < 90
         if ((a*a + b*b - c*c) / 2.0 * a * b > 0.0)
         {
            closed = FALSE; // break closure between 1st and last vertices
            cnt++; // was closed, so eliminated last vertex; put last vertex back
            i++; // skip meaningless loop to increment i to new last vertex
         }
      }

      if (i == cnt-1) 
         DrawPolyNow = TRUE; // draw last poly now
      else if (Dxf_Linewidth >= realwidth) // only break wide polys
      {
         a = LengthPoint2(&coos[i-1], &coos[i]);
         b = LengthPoint2(&coos[i], &coos[i+1]);
         c = LengthPoint2(&coos[i-1], &coos[i+1]); // length of triangle leg opposite angle we are calculating
      
         // Law of Cosines -> cos C = (a^2 + b^2 - c^2) / 2ab > 0 then C < 90
         if ((a*a + b*b - c*c) / 2.0 * a * b > 0.0)
            DrawPolyNow = TRUE;
      }

      // draw poly routine
      if (DrawPolyNow)
      {
         DrawPolyNow = FALSE;
         if (i != cnt-1) // if drawing a poly before hitting last element, set closed to FALSE
         {
            if (closed)
               cnt++; // was closed, so eliminated last vertex; put last vertex back
            closed = FALSE; // if break poly, poly not closed
         }

         // draw first poly and change firstVertex
         Dxf_Poly(fp,Dxf_Level,closed);
      
         if (Dxf_Linewidth >= realwidth)
            Dxf_Width(fp,Dxf_Linewidth);

         for (j = firstVertex; j <= i; j++)
            Dxf_Vertex(fp,Dxf_Level,coos[j].x,coos[j].y);
         Dxf_Seqend(fp,Dxf_Level);

         if (!closed)
         {
            if (lineend == 1) // round
            {
               wdxf_roundend(fp,Dxf_Level,Dxf_Linewidth,coos[firstVertex].x,coos[firstVertex].y);
               wdxf_roundend(fp,Dxf_Level,Dxf_Linewidth,coos[i].x,coos[i].y);
            }
         }
         firstVertex = i;
      }
      i++;
   }
#endif
   return(1);
}

// end BARCOLIB.C
 


