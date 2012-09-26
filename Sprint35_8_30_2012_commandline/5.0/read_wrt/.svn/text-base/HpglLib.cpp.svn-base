// $Header: /CAMCAD/4.5/read_wrt/HpglLib.cpp 13    1/27/05 7:43p Kurt Van Ness $

/*****************************************************************************/
/*    
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1999. All Rights Reserved.

  HPGL Library
*/

#include "StdAfx.h"
#include "General.h"
#include <math.h>
#include "hpgllib.h"

#define MAX_LAYER    256
#define END_OF_TEXT  0x3    // HP end of text label.

typedef struct
{
   char  *name;
}HPLayer;

static FILE     *OFP;           /*Output Filepointer*/
static char     temp[1000];     /*Temporary Commandstring */
static HPLayer  *layer;         /*Pointer to Structure of Layers*/
static int      layercnt;       /*Layercounter*/
static int      ActPenNr;       /*Actual Pennumber...*/
static int      hpgl2;          // output is hpgl2 or not
static double   current_width, lastw;  

/* The Functions and Procedures ...*/
static void    Hpgl_Command(FILE *fp,const char *c);

static void    init();
static void    deinit();

// This is always fixed to plotter units.
static int     PageUnits = UNIT_HPPLOTTER;
static int     first;
static int     last_arc;
static Point2  prev_vertex;

//--------------------------------------------------------------------------
static void write_aa(FILE *fp,double x, double y, double angle)
{
   int d = GetDecimals(PageUnits); 
   sprintf(temp,"AA%.*lf,%.*lf,%1.1lf;",
         d,x, d, y, angle);
   Hpgl_Command(fp,temp);
}

/****************************************************************************
*
*/
static void write_pa(FILE *fp,double x, double y)
{
   int d = GetDecimals(PageUnits); 

   sprintf(temp,"PA%.*lf,%.*lf;",
      d, x, d, y);
   Hpgl_Command(fp,temp);
}

/****************************************************************************
*
*/
static void write_ra(FILE *fp,double x, double y)
{
   int d = GetDecimals(PageUnits); 

   sprintf(temp,"RA%.*lf,%.*lf;",
      d, x, d, y);
   Hpgl_Command(fp,temp);
}

/****************************************************************************
*
*/
static void write_pd(FILE *fp,double x, double y)
{
   int d = GetDecimals(PageUnits); 

   sprintf(temp,"PD%.*lf,%.*lf;",
      d, x, d, y);
   Hpgl_Command(fp,temp);
}


/****************************************************************************
*
*/
void Hpgl_Command(FILE *fp, const char *c)
{
   if (fprintf(fp,"%s",c) == EOF)
   {
//    printf( "ERROR [%s]: out of disk space or disk failure\n",c);
      exit(1);
   }
}

/****************************************************************************
*
*/
void init()
{
   if ((layer=(HPLayer *)calloc(MAX_LAYER,sizeof(HPLayer))) == NULL )
      MemErrorMessage(__FILE__,__LINE__);
   layercnt=0;
   ActPenNr=255;
   first = TRUE;
   lastw = current_width = -1;
   last_arc = FALSE;
}

/****************************************************************************
*
*/
void deinit()
{
   int i;

   for (i=0;i<layercnt;i++)
   {
      free(layer[i].name);
   }
   free(layer);
}

void whpgl_Graph_Level( int penNum )
{
   if (penNum != ActPenNr)
   {
     ActPenNr = penNum;
     sprintf(temp, "SP%d;\n", ActPenNr);
     Hpgl_Command(OFP, temp);
   }
}

/****************************************************************************
*
*/
int whpgl_Graph_Init(int hpgl_2)
{
   hpgl2 = hpgl_2;
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

/****************************************************************************
*
*/
void whpgl_Graph_File_Open( FILE *file)
{
   OFP=file;
   Hpgl_Command(OFP,"IN;");
}

/****************************************************************************
*
*/
void whpgl_Graph_File_Close()
{
   deinit();      /*deinitialize the Memory for the Layers ...*/
}

/****************************************************************************
*
*/
void whpgl2_PenWidth(double w)    // in MM
{
   if (hpgl2)
   {
     CString   t;

     if (lastw == w) return;

     if (w < 0)
       t.Format("WU;"); // just reset
     else
       t.Format("WU;PW%1.1lf;",w);
     Hpgl_Command(OFP,t);
     lastw = w;
   }
}

/****************************************************************************
*
*/
void whpgl_Graph_Line_Width(double w)  // in HP units
{
   current_width = w;
}

/****************************************************************************
*
*/
void whpgl_Graph_Filled_Rectangle(double x1, double y1, double x2, double y2, double penWidth)
{
   write_pa(OFP,x1+penWidth/2,y1+penWidth/2);
   Hpgl_Command(OFP,"RA;");
   write_ra(OFP,x2-penWidth/2,y2-penWidth/2);
   return;
}

/****************************************************************************
*
*/
void whpgl_Filled_Circle(double x, double y, double radius, double penWidth)
{
   if (radius > penWidth/2)
   {
      write_pa(OFP,x,y);

      int d = GetDecimals(PageUnits); 
      char temp[80];
      sprintf(temp,"WG%.*lf,0,360;",d,radius-penWidth/2);
      Hpgl_Command(OFP,temp);
   }
   else
   {
      write_pa(OFP,x,y);

      int d = GetDecimals(PageUnits); 
      char temp[80];
      sprintf(temp,"WG%.*lf,0,360;",d,-(radius*0.1)); // make up an radius if penwidth
                                       // is larger than radius
      Hpgl_Command(OFP,temp);
   }
   return;
}

/****************************************************************************
* 
*/
void whpgl_Graph_Aperture(int type, double x, double y, double sizeA, double sizeB, 
                          double rotation, double xoffset, double yoffset, double penWidth)
{
   penWidth = fabs(penWidth);

   switch (type)
   {
      case T_ROUND:
      {
         if (hpgl2)
         {
            whpgl_Filled_Circle(x,y,sizeA/2,penWidth);
         }
         else
         {
            double radius = sizeA/2;
            if (penWidth > 0)
            {
               for (radius=sizeA/2-penWidth/2; radius>penWidth/2; radius-=penWidth)
                  whpgl_Graph_Circle(x, y, radius);
            }
            else
            {
               whpgl_Graph_Circle(x, y, radius);
            }
            whpgl_Graph_Point(x, y);
         }
      }
      break;  // T_ROUND
      case T_DONUT:
      {
         double radius;
         // draw circles starting at outerrad - half penwidth
         // continue until next circle would be inside innerrad + half penWidth
         if (penWidth > 0)
         {
            for (radius=sizeA/2-penWidth/2; radius>sizeB/2+penWidth/2; radius-=penWidth)
               whpgl_Graph_Circle(x, y, radius);
         }
         // draw circle at innerrad + half penWidth
         whpgl_Graph_Circle(x, y, sizeB/2+penWidth/2);
      }
      break;  // T_DONUT
      case T_SQUARE:
         sizeB = sizeA;
      case T_RECTANGLE:
      {
         if (hpgl2)
         {
            double xx1, yy1;
            double x0, y0, x1 ,y1;
            xx1 = -sizeA/2;
            yy1 = -sizeB/2;
            Rotate(xx1,yy1,RadToDeg(rotation),&x0, &y0);
            xx1 = +sizeA/2;
            yy1 = +sizeB/2;
            Rotate(xx1,yy1,RadToDeg(rotation),&x1, &y1);
            whpgl_Graph_Filled_Rectangle(x-x0,y-y0,x+x1, y+y1, penWidth);
         }
         else
         {
            if (sizeA == sizeB)
            {
               Point2 a, b, c, d;
               int passNum = 0;
               double xx1, yy1;
               double x0, y0, x1 ,y1;
               xx1 = -(sizeA/2);
               yy1 = 0;
               Rotate(xx1,yy1,RadToDeg(rotation),&x0, &y0);
               xx1 = +(sizeA/2);
               yy1 = 0;
               Rotate(xx1,yy1,RadToDeg(rotation),&x1, &y1);
               while (passNum * penWidth * 2 < sizeA)
               {
                  a.x = x+x0 - sin(0.) * (sizeA/2  - passNum*penWidth);
                  a.y = y+y0 + cos(0.) * (sizeA/2  - passNum*penWidth);
                  b.x = x+x1 - sin(0.) * (sizeA/2  - passNum*penWidth);
                  b.y = y+y1 + cos(0.) * (sizeA/2  - passNum*penWidth);
                  c.x = x+x0 + sin(0.) * (sizeA/2  - passNum*penWidth);
                  c.y = y+y0 - cos(0.) * (sizeA/2  - passNum*penWidth);
                  d.x = x+x1 + sin(0.) * (sizeA/2  - passNum*penWidth);
                  d.y = y+y1 - cos(0.) * (sizeA/2  - passNum*penWidth);

                  
                  if (!passNum) // if first time though
                  {
                     Hpgl_Command(OFP,"PU;");
                     write_pa(OFP,a.x,a.y);
                  }
                  else
                     write_pd(OFP,a.x, a.y); // move to new a

                  write_pd(OFP,b.x, b.y);
                  write_pd(OFP,d.x, d.y);
                  write_pd(OFP,c.x, c.y);
                  if (!passNum) // if first time though
                     write_pd(OFP,a.x, a.y); // draw side edge
                  
                  passNum++;
                  if (penWidth < SMALLNUMBER)
                     break;
               }
               write_pd(OFP,x+x0,y+y0);
               write_pd(OFP,x+x1,y+y1);
               Hpgl_Command(OFP,"PU;\n");
            }
            else if (sizeA > sizeB) // horizontal oblong
            {
               double xx1, yy1;
               double x0, y0, x1 ,y1;
               xx1 = -(sizeA/2-sizeB/2);
               yy1 = 0;
               Rotate(xx1,yy1,RadToDeg(rotation),&x0, &y0);
               xx1 = +(sizeA/2-sizeB/2);
               yy1 = 0;
               Rotate(xx1,yy1,RadToDeg(rotation),&x1, &y1);
               whpgl_Graph_Line_with_Width( x+x0, y+y0, 0.0,  x+x1, y+y1, 0.0, sizeB, penWidth, FALSE );
            }
            else if (sizeA < sizeB)// vertical oblong
            {
               double xx1, yy1;
               double x0, y0, x1 ,y1;
               xx1 = 0;
               yy1 = +(sizeB/2-sizeA/2);
               Rotate(xx1,yy1,RadToDeg(rotation),&x0, &y0);
               xx1 = 0;
               yy1 = -(sizeB/2-sizeA/2);
               Rotate(xx1,yy1,RadToDeg(rotation),&x1, &y1);
               whpgl_Graph_Line_with_Width( x+x0, y+y0, 0.0, x+x1, y+y1, 0.0, sizeA, penWidth, FALSE );
            }
         }
      }
      break;  // T_RECTANGLE
      case T_OBLONG:
      {
         if (sizeA >= sizeB) // horizontal oblong
         {
            double xx1, yy1;
            double x0, y0, x1 ,y1;
            xx1 = -(sizeA/2-sizeB/2);
            yy1 = 0;
            Rotate(xx1,yy1,RadToDeg(rotation),&x0, &y0);
            xx1 = +(sizeA/2-sizeB/2);
            yy1 = 0;
            Rotate(xx1,yy1,RadToDeg(rotation),&x1, &y1);
            whpgl_Graph_Line_with_Width( x+x0, y+y0, 0.0,  x+x1, y+y1, 0.0, sizeB, penWidth, TRUE );
         }
         else // vertical oblong
         {
            double xx1, yy1;
            double x0, y0, x1 ,y1;
            xx1 = 0;
            yy1 = +(sizeB/2-sizeA/2);
            Rotate(xx1,yy1,RadToDeg(rotation),&x0, &y0);
            xx1 = 0;
            yy1 = -(sizeB/2-sizeA/2);
            Rotate(xx1,yy1,RadToDeg(rotation),&x1, &y1);
            whpgl_Graph_Line_with_Width( x+x0, y+y0, 0.0, x+x1, y+y1, 0.0, sizeA, penWidth, TRUE );
         }
      }
      break;  // T_OBLONG
      case T_TARGET:
         whpgl_Graph_Circle(x, y, sizeA/4); // /2 target circle is half of size; /2 radius
         whpgl_Graph_Line(x-sizeA/2, y, x+sizeA/2, y);
         whpgl_Graph_Line(x, y+sizeA/2, x, y-sizeA/2);
      break;  // T_TARGET
      case T_OCTAGON:
      {
         Point2 reg[8];
         double radius = sizeA/2;
         double halfSide = sizeA/4.83; // size of half of one side of octagon
         reg[0].x = x - radius;
         reg[0].y = y + halfSide;
         reg[1].x = x - halfSide;
         reg[1].y = y + radius;
         reg[2].x = x + halfSide;
         reg[2].y = y + radius;
         reg[3].x = x + radius;
         reg[3].y = y + halfSide;
         reg[4].x = x + radius;
         reg[4].y = y - halfSide;
         reg[5].x = x + halfSide;
         reg[5].y = y - radius;
         reg[6].x = x - halfSide;
         reg[6].y = y - radius;
         reg[7].x = x - radius;
         reg[7].y = y - halfSide;

         for(int i=0; i<7; i++)
            whpgl_Graph_Line(reg[i].x, reg[i].y, reg[i+1].x, reg[i+1].y);
         whpgl_Graph_Line(reg[7].x, reg[7].y, reg[0].x, reg[0].y);
      }
      break;  // T_OCTAGON
      case T_THERMAL:
      {
         double radius = sizeA/2 - penWidth/2; // radius - half penwidth
         whpgl_Graph_Arc(x, y, radius, 330*(PI/180), 60*(PI/180));
         whpgl_Graph_Arc(x, y, radius, 60*(PI/180), 60*(PI/180));
         whpgl_Graph_Arc(x, y, radius, 150*(PI/180), 60*(PI/180));
         whpgl_Graph_Arc(x, y, radius, 240*(PI/180), 60*(PI/180));
      }
      break;  // T_THERMAL
      case T_COMPLEX:
      case T_UNDEFINED:
      default:
      {
         double radius = /*UApSize*/0.01/2;
         whpgl_Graph_Circle(x, y, radius);
         whpgl_Graph_Line(x - radius, y + radius,
                          x + radius, y - radius);
         whpgl_Graph_Line(x + radius, y + radius,
                          x - radius, y - radius);
      }
      break;  // T_UNDEFINED
   } // end switch
}

/****************************************************************************
* whpgl_Graph_Arc
*/
void whpgl_Graph_Circle( double center_x, double center_y, double radius )
{
   write_pa(OFP,center_x+radius,center_y);
   Hpgl_Command(OFP,"PD;");
   write_aa(OFP,center_x,center_y,-360.0);
   Hpgl_Command(OFP,"PU;\n");

/* do not use the HP CI command. Everybodyt is useing AA with -360
   write_pa(OFP,center_x,center_y);
   sprintf(temp,"CI %2.4lf;",radius);
   Hpgl_Command(OFP,temp);
*/
// return 1;
}

/****************************************************************************
* whpgl_Graph_Arc
*/
void whpgl_Graph_Arc( double center_x, double center_y, double radius,
                 double startangle, double deltaangle )
{
   double start_x, start_y;
   double degrees;

   start_x = center_x + (cos(startangle) * radius);
   start_y = center_y + (sin(startangle) * radius);
   degrees = deltaangle * (180/PI);

   write_pa(OFP, start_x, start_y);
   Hpgl_Command(OFP, "PD;");
   write_aa(OFP,center_x, center_y, degrees);
   Hpgl_Command(OFP, "PU;\n");
}

/*****************************************************************************
* whpgl_Graph_Arc_with_Width
*/
/* a     __     b
    \_--   --_ /
     \        /
      c      d
*/
void whpgl_Graph_Arc_with_Width( double center_x, double center_y, double radius,
                 double startangle, double deltaangle,
                 double lineWidth, double penWidth, int RoundEnd )
{
   double   start_x, start_y, end_x, end_y;
   double   endangle = startangle + deltaangle;
   double   degrees;

   penWidth = fabs(penWidth);
   start_x = center_x + (cos(startangle) * radius);
   start_y = center_y + (sin(startangle) * radius);
   end_x = center_x + (cos(endangle) * radius);
   end_y = center_y + (sin(endangle) * radius);
   degrees = deltaangle*(180.0/PI);

   int      passNum = 0;
   Point2   a, d;

   while (passNum * penWidth * 2 < lineWidth)
   {
      a.x = start_x + cos(startangle) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
      a.y = start_y + sin(startangle) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
      d.x = end_x - cos(endangle) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
      d.y = end_y - sin(endangle) * (lineWidth/2 - penWidth/2 - passNum*penWidth);

      if (RoundEnd)
      {
         if (!passNum) // if first time though
         {
            Hpgl_Command(OFP,"PU;");
            write_pa(OFP,a.x,a.y);
            Hpgl_Command(OFP, "PD;\n");
         }
         else
            write_pd(OFP,a.x,a.y); // drop down to new a

         // arc from a to b
         write_aa(OFP,center_x, center_y,degrees);

         // arc round end from b to d
         if (deltaangle > 0)
            write_aa(OFP,end_x,end_y,180.0);
         else
            write_aa(OFP,end_x,end_y,-180.0);

         // arc from d to c
         write_aa(OFP,center_x, center_y, -degrees);

         // arc round end from c to a
         if (deltaangle > 0)
            write_aa(OFP, start_x, start_y, 180.0);
         else
            write_aa(OFP,start_x, start_y, -180.0);
      }
      else // SquareEnd
      {
         if (!passNum) // if first time though
         {
//          Hpgl_Command(OFP,"PU;");
            write_pa(OFP,a.x,a.y);
//          Hpgl_Command(OFP, "PD;\n");
         }
         else
            write_pd(OFP,a.x, a.y); // move to new a

         // arc from a to b
         write_aa(OFP,center_x, center_y, degrees);

         write_pd(OFP,d.x, d.y);

         // arc from d to c
         write_aa(OFP,center_x, center_y, -degrees);

         if (!passNum) // if first time though
            write_pd(OFP,a.x, a.y); // draw side edge
      }
      passNum++;
      if(penWidth < SMALLNUMBER) break;
   }
   Hpgl_Command(OFP,"PU;\n");
}

/*****************************************************************************
* whpgl_Graph_Line
*/
void whpgl_Graph_Line( double x1, double y1, double x2, double y2 )
{
   write_pa(OFP,x1,y1);
   write_pd(OFP,x2,y2);
   Hpgl_Command(OFP,"PU;\n");
}

/*****************************************************************************
* whpgl_Graph_Line

          _---b
      _---     \
   a--       __-d
    \    __--
     c---
*/
void whpgl_Graph_Line_with_Width( double x1, double y1, double bulge1,
                                  double x2, double y2, double bulge2,
                                  double lineWidth, double penWidth, int RoundEnd )
{
   int passNum = 0;
   penWidth = fabs(penWidth);

/*
   if (hpgl2)
   {
      if (!RoundEnd)
      {
         write_pa(OFP,x1,y1);
         write_ra(OFP,x2,y2);
         return;
      }
   }
*/
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      whpgl_Graph_Arc_with_Width( cx, cy, r, sa, da, lineWidth, penWidth, RoundEnd );
   }
   else
   {
      // ArcTan2 is from geomlib.c and check for 0 etc ..
      double theta = ArcTan2( (y2-y1),(x2-x1) );
      Point2 a, b, c, d;

      while (passNum * penWidth * 2 < lineWidth)
      {
         a.x = x1 - sin(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
         a.y = y1 + cos(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
         b.x = x2 - sin(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
         b.y = y2 + cos(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
         c.x = x1 + sin(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
         c.y = y1 - cos(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
         d.x = x2 + sin(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);
         d.y = y2 - cos(theta) * (lineWidth/2 - penWidth/2 - passNum*penWidth);

         if (RoundEnd)
         {
            if (!passNum) // if first time though
            {
               Hpgl_Command(OFP,"PU;");
               write_pa(OFP,a.x,a.y);
            }
            else
               write_pd(OFP,a.x,a.y);

            write_pd(OFP, b.x, b.y);
      
            write_aa(OFP,x2,y2,-180.0);
            write_pd(OFP,c.x, c.y);
            write_aa(OFP, x1, y1,-180.0);
         }
         else // SquareEnd
         {
            if (!passNum) // if first time though
            {
               Hpgl_Command(OFP,"PU;");
               write_pa(OFP,a.x,a.y);
            }
            else
               write_pd(OFP,a.x, a.y); // move to new a

            write_pd(OFP,b.x, b.y);
            write_pd(OFP,d.x, d.y);
            write_pd(OFP,c.x, c.y);
            if (!passNum) // if first time though
               write_pd(OFP,a.x, a.y); // draw side edge
         }
         passNum++;
         if (penWidth < SMALLNUMBER)
            break;
      }
      Hpgl_Command(OFP,"PU;\n");
   }
   return;
}

/*****************************************************************************
* 
   whpgl_Graph_Point
*/
void whpgl_Graph_Point( double x1, double y1 )
{
   write_pa(OFP,x1,y1);
   Hpgl_Command(OFP,"PD;PU;\n");
}

/*****************************************************************************
* 
*/
void whpgl_Graph_Text( const char *text, double x1, double y1,
      double height, double width, double angle, int oblique, int mirror )
{
   /* width = character width. */
   double run,rise;
   double t = tan(DegToRad(1.0*oblique));

   if (oblique == 0)
       strcpy(temp,"SL0;");
   else
       sprintf(temp,"SL%1.2lf;",t);
   Hpgl_Command(OFP,temp);

   rise = sin (angle);
   run  = cos (angle);
   write_pa(OFP,x1,y1);

   // height and width is always in centimeter
   width *= Units_Factor(PageUnits, UNIT_MM) / 10;
   height *= Units_Factor(PageUnits, UNIT_MM) / 10;

   if (mirror)  width = -width;

   // see reader. and HPGL.IN
   // Char space ratio
   // CAMCAD does a 10% character to folling whitespace ratio.
   // HP says that the default on their system is 15%.
   sprintf(temp,"SI%2.4lf,%2.4lf;",width/1.1,height); // width is without spacing.
   Hpgl_Command(OFP,temp);
   sprintf(temp,"DI%2.4lf,%2.4lf;",run,rise);
   Hpgl_Command(OFP,temp);
   sprintf(temp,"LB%s%c;", text,END_OF_TEXT);
   Hpgl_Command(OFP,temp);
}

void whpgl_Graph_Polyline( Point2 p, int filled)
{
   write_pa(OFP, p.x, p.y);

   prev_vertex = p;

   if (hpgl2 && filled)
        Hpgl_Command(OFP,"PM0");
   Hpgl_Command(OFP,"PD");
   first = TRUE;
}


void whpgl_Graph_Vertex(Point2 p)
{
   if (!first)
      Hpgl_Command(OFP,",");

   int d = GetDecimals(PageUnits); 
   
   double da = atan(prev_vertex.bulge) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(prev_vertex.x,prev_vertex.y,p.x,p.y,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      whpgl_Graph_Arc( cx, cy, r, sa, da);
      last_arc = TRUE;
   }
   else
   {
      if (last_arc)
         Hpgl_Command(OFP,"PD");
      last_arc = FALSE;
      sprintf(temp,"%.*lf,%.*lf",
         d, p.x,
         d, p.y);
      Hpgl_Command(OFP,temp);
      // write_pd(OFP,p.x,p.y);
   }
   first = FALSE;
   prev_vertex = p;
}

void whpgl_Graph_PolyClose(int filled)
{
   if (hpgl2 && filled)
   {
      Hpgl_Command(OFP,"PM2FPEP;");
   }

   Hpgl_Command(OFP,"PU;");
}

// end HPGLLIB.C

