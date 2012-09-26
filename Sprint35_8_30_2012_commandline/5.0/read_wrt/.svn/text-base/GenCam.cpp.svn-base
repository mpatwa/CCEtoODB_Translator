// $Header: /CAMCAD/4.6/read_wrt/GenCam.cpp 10    4/03/07 5:51p Lynn Phung $

/****************************************************************************
*
   Output for GENCAM
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "math.h"
#include "GENCAM.h"
#include "CCEtoODB.h"

static char    cur_layer[80], cur_mask[80]; //
static int     cur_widthcode = -1;
static int     output_units_accuracy;


/* The Functions and Procedures ...*/
static void init();
static void deinit();

/****************************************************************************/
/*
*/
void init()
{
   cur_widthcode = -1;
   strcpy(cur_layer,"");
   strcpy(cur_mask,"");
}

void deinit()
{
}

void wGENCAM_Graph_Layer(FILE *fp, const char *level)
{
   if (strcmp(cur_layer,level))
   {
      if (strlen(level))   // reset must allow an empty layer
         fprintf(fp,"LAYER %s\n",level);     
      strcpy(cur_layer,level);
   }
   strcpy(cur_mask,""); // delete mask if layer was used.
}

void wGENCAM_Graph_MaskLevel(FILE *fp, const char *level)
{
   if (strcmp(cur_mask,level))
   {
      if (strlen(level))   // reset must allow an empty layer
         fprintf(fp,"MASK %s\n",level);      
      strcpy(cur_mask,level);
   }
   strcpy(cur_layer,"");   // delete layer if mask was used.
}

void wGENCAM_Graph_Artwork(FILE *fp, const char *level, long artworkcnt)
{
   if (strcmp(cur_mask,level))
   {
      fprintf(fp,"ARTWORK artwork%ld %s\n",artworkcnt,level);     
   }
}

void wGENCAM_Graph_Artwork1(FILE *fp, const char *level, long artworkcnt)
{
   fprintf(fp,"ARTWORK artwork%ld\nLAYER %s\n",artworkcnt,level);    
}

int wGENCAM_Graph_Init_Layout(int o)
{
   output_units_accuracy = o;
   init();        /*initialize the Memory for the Layers ...*/
   return 1;
}

void wGENCAM_Graph_File_Open_Layout()
{
   
}

void wGENCAM_Graph_File_Close_Layout()
{
   deinit();      /*deinitialize the Memory for the Layers ...*/
}

void wGENCAM_Graph_Header(FILE *fp, char *GENCAM, double version,const char *fname)
{
   char *months[] = { "January", "February", "March", "April", "May", "June", "July", 
      "August", "September", "October", "November", "December" };
   char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

   //CString camcad;
   //camcad.LoadString(IDR_MAINFRAME);
   CTime time = CTime::GetCurrentTime();
   int hour = time.GetHour();

   fprintf(fp,"$HEADER\n");
   fprintf(fp,"%s %1.1lf\n", GENCAM, version);
   fprintf(fp,"USER MENTOR GRAPHICS - TRANSLATOR %s\n", getApp().getCamCadTitle());
   fprintf(fp,"DRAWING %s\n",fname);
   fprintf(fp,"REVISION :%s, %s %d, %d at %d:%02d%s\n",
         days[time.GetDayOfWeek()-1], months[time.GetMonth()-1], 
         time.GetDay(), time.GetYear(), (hour>12 ? hour-12 : hour), time.GetMinute(),
         (hour>11 ? "pm" : "am"));
   fprintf(fp,"UNITS USER 1000\n");
   fprintf(fp,"ORIGIN 0 0\n");
   fprintf(fp,"INTERTRACK 0\n");
}

void wGENCAM_Graph_Aperture(FILE *fp, double x, double y, double sizeA, double sizeB, 
                            double xoffset, double yoffset, int form, double rot)
{
   switch (form)
   {
      case T_ROUND:
         fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
            output_units_accuracy, xoffset, output_units_accuracy, yoffset, output_units_accuracy, sizeA/2);
      break;
      case T_SQUARE:
         fprintf(fp,"RECTANGLE %1.*lf %1.*lf %1.*lf %1.*lf\n",
            output_units_accuracy, xoffset-sizeA/2, 
            output_units_accuracy, yoffset-sizeA/2, 
            output_units_accuracy, sizeA, output_units_accuracy, sizeA);
      break;
      case T_RECTANGLE:
         fprintf(fp,"RECTANGLE %1.*lf %1.*lf %1.*lf %1.*lf\n",
            output_units_accuracy, xoffset-sizeA/2, 
            output_units_accuracy, yoffset-sizeB/2, 
            output_units_accuracy, sizeA, output_units_accuracy, sizeB);
      break;
      case T_DONUT:                    
         fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
            output_units_accuracy, xoffset, output_units_accuracy, yoffset, output_units_accuracy, sizeA/2);
      break;
      case T_OCTAGON:                  
         fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
            output_units_accuracy, xoffset, output_units_accuracy, yoffset, output_units_accuracy, sizeA/2);
      break;
      case T_OBLONG:                      
         // here I need to do a true finger with lines and arcs
         fprintf(fp,"LINE %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2,
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2),

         fprintf(fp,"ARC %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2,
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2,
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset);

         fprintf(fp,"LINE %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2,
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2),

         fprintf(fp,"ARC %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2,
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2,
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset);
      break;
   }
   
   return;
}

void wGENCAM_Graph_ComplexAperture(FILE *fp, double x, double y, double sizeA, double sizeB, 
                            double xoffset, double yoffset, int form, double rot)
{
   switch (form)
   {
      case T_ROUND:
         wGENCAM_Graph_Arc("",fp, xoffset, yoffset, sizeA/2, 0, PI2);
      break;
      case T_SQUARE:
         wGENCAM_Graph_Line("",fp, xoffset-sizeA/2, yoffset-sizeA/2, 0.0, 
                            xoffset+sizeA/2, yoffset-sizeA/2, 0.0);
         wGENCAM_Graph_Line("",fp, xoffset+sizeA/2, yoffset-sizeA/2, 0.0, 
                            xoffset+sizeA/2, yoffset+sizeA/2, 0.0);
         wGENCAM_Graph_Line("",fp, xoffset+sizeA/2, yoffset+sizeA/2, 0.0, 
                            xoffset+sizeA/2, yoffset+sizeA/2, 0.0);
         wGENCAM_Graph_Line("",fp, xoffset-sizeA/2, yoffset+sizeA/2, 0.0, 
                            xoffset+sizeA/2, yoffset-sizeA/2, 0.0);
      break;
      case T_RECTANGLE:
         wGENCAM_Graph_Line("",fp, xoffset-sizeA/2, yoffset-sizeB/2, 0.0, 
                            xoffset+sizeA/2, yoffset-sizeB/2, 0.0);
         wGENCAM_Graph_Line("",fp, xoffset+sizeA/2, yoffset-sizeB/2, 0.0, 
                            xoffset+sizeA/2, yoffset+sizeB/2, 0.0);
         wGENCAM_Graph_Line("",fp, xoffset+sizeA/2, yoffset+sizeB/2, 0.0, 
                            xoffset+sizeA/2, yoffset+sizeB/2, 0.0);
         wGENCAM_Graph_Line("",fp, xoffset-sizeA/2, yoffset+sizeB/2, 0.0, 
                            xoffset+sizeA/2, yoffset-sizeB/2, 0.0);
      break;
      case T_DONUT:                    
         wGENCAM_Graph_Arc("",fp, xoffset, yoffset, sizeA/2, 0, PI2);
      break;
      case T_OCTAGON:                  
         wGENCAM_Graph_Arc("",fp, xoffset, yoffset, sizeA/2, 0, PI2);
      break;
      case T_OBLONG:                      
         // here I need to do a true finger with lines and arcs
         fprintf(fp,"LINE %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2,
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2),

         fprintf(fp,"ARC %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2,
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2,
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset);

         fprintf(fp,"LINE %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset + (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2,
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2),

         fprintf(fp,"ARC %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset + sizeB/2,
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset - sizeB/2,
                  output_units_accuracy, xoffset - (sizeA - sizeB)/2,
                  output_units_accuracy, yoffset);
      break;
   }
   
   return;
}

void wGENCAM_Graph_Circle(FILE *fp, double center_x, double center_y, double radius )
{
   fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
      output_units_accuracy, center_x,
      output_units_accuracy, center_y,
      output_units_accuracy, radius);
   return;
}

/****************************************************************************
* wGENCAM_Graph_Arc
*/
void wGENCAM_Graph_Arc(const char *ident,
                 FILE *fp, double center_x, double center_y, double radius,
                 double startangle, double deltaangle )
{
   double x1,y1,x2,y2;

   x1 = cos(startangle)*radius+center_x;
   y1 = sin(startangle)*radius+center_y;
   x2 = cos(startangle+deltaangle)*radius+center_x;
   y2 = sin(startangle+deltaangle)*radius+center_y;

   fprintf(fp,"%sCIRCARC: (%1.*lf, %1.*lf), (%1.*lf, %1.*lf), (%1.*lf, %1.*lf), %s;\n",ident,
         output_units_accuracy, x1,
         output_units_accuracy, y1,
         output_units_accuracy, x2,
         output_units_accuracy, y2,
         output_units_accuracy, center_x,
         output_units_accuracy, center_y,
         (deltaangle < 0)?"CLKW":"CCLKW");

}

/*****************************************************************************
* 
   wGENCAM_Graph_Line
*/
void wGENCAM_Graph_Line(const char *ident, 
                        FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2 )
{
   // wGENCAM_Graph_Rectangle( points[0].x, points[0].y, points[2].x, points[2].y);
   // here check also for bulge
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      wGENCAM_Graph_Arc(ident, fp, cx, cy, r, sa, da);
   }
   else
   {
      if (x1 == x2 && y1 == y2)  return;  // eliminate  segments which are Zero;
      fprintf(fp,"%sLINE: (%1.*lf, %1.*lf), (%1.*lf, %1.*lf);\n",ident,
         output_units_accuracy, x1,
         output_units_accuracy, y1,
         output_units_accuracy, x2,
         output_units_accuracy, y2);
   }

   return;
}

/*****************************************************************************
* 
*/
void wGENCAM_Graph_StartAt(const char *ident, FILE *fp, double x1, double y1)
{

   fprintf(fp,"%sSTARTAT: (%1.*lf, %1.*lf);\n",ident,
         output_units_accuracy, x1,
         output_units_accuracy, y1);

   return;
}

/*****************************************************************************
* 
*/
void wGENCAM_Graph_NextAt(const char *ident, 
                        FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2 )
{
   // wGENCAM_Graph_Rectangle( points[0].x, points[0].y, points[2].x, points[2].y);
   // here check also for bulge
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      // wGENCAM_Graph_Arc(ident, fp, cx, cy, r, sa, da);

      fprintf(fp,"%sCIRCARCTO: (%1.*lf, %1.*lf), (%1.*lf, %1.*lf), %s;\n",ident,
         output_units_accuracy, x2,
         output_units_accuracy, y2,
         output_units_accuracy, cx,
         output_units_accuracy, cy,
         (da < 0)?"CLKW":"CCLKW");

   }
   else
   {
      fprintf(fp,"%sLINETO: (%1.*lf, %1.*lf);\n",ident,
         output_units_accuracy, x2,
         output_units_accuracy, y2);
   }

   return;
}

/*****************************************************************************
* 
*/
void wGENCAM_Graph_EndAt(const char *ident, 
                        FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2 )
{
   // wGENCAM_Graph_Rectangle( points[0].x, points[0].y, points[2].x, points[2].y);
   // here check also for bulge
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      // wGENCAM_Graph_Arc(ident, fp, cx, cy, r, sa, da);

      fprintf(fp,"%sENDCIRCARC: (%1.*lf, %1.*lf), %s;\n",ident,
         output_units_accuracy, cx,
         output_units_accuracy, cy,
         (da < 0)?"CLKW":"CCLKW");

   }
   else
   {
      fprintf(fp,"%sLINETO: (%1.*lf, %1.*lf);\n",ident,
         output_units_accuracy, x2,
         output_units_accuracy, y2);
      fprintf(fp,"%sENDLINE: ;\n",ident);
   }

   return;
}

/*****************************************************************************
* wGENCAM_Graph_Rectangle
*/
void wGENCAM_Graph_Rectangle(FILE *fp, double xx1, double yy1, double xx2, double yy2 )
{
   double x1,y1,x2,y2;

   // need to sort, because it may have been rotated
   if (xx1 < xx2)
   {
      x1 = xx1;
      x2 = xx2;
   }
   else
   {
      x2 = xx1;
      x1 = xx2;
   }

   if (yy1 < yy2)
   {
      y1 = yy1;
      y2 = yy2;
   }
   else
   {
      y2 = yy1;
      y1 = yy2;
   }

   if (x1 == x2 && y1 == y2)  return;  // eliminate  segments which are Zero;

   // rectangle is start and dimx, dimy
   fprintf(fp,"RECTANGLE %1.*lf %1.*lf %1.*lf %1.*lf\n",
      output_units_accuracy, x1,
      output_units_accuracy, y1,
      output_units_accuracy, x2-x1,
      output_units_accuracy, y2-y1);

   return;
}

void wGENCAM_Graph_Text(const char *ident, FILE *fp, char *text, double x, double y,
      double height, double width, double angle, int mirror)
{
   if (strlen(text) == 0)  return;

   fprintf(fp,"%sTEXT: \"%s\", (0, 0), (%lg, %lg), , , (%lg, %lg), %lg %s; \n",ident,
      text, width*strlen(text), height, x, y, RadToDeg(angle),(mirror)?", MIRROR":"");
   
}

// end GENCAM.CPP

